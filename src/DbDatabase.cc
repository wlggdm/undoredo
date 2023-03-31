#include "DbDatabase.h"
#include "DbDatabaseImpl.h"
#include "DbUndoFiler.h"

#include "base/notreached.h"

namespace AcDb {

DbDatabase::DbDatabase() : DbObject(new DbDatabaseImpl()) {
  DbObjectImpl* object_impl = DbObjectImpl::GetImpl(static_cast<DbObject*>(this));
  impl_ = static_cast<DbDatabaseImpl*>(object_impl);

  impl_->SetDatabase(this);
}

DbDatabase::~DbDatabase() {}

DbObjectID DbDatabase::AppendObject(DbObject* object) {
  DbObjectID id = impl_->AddObject(object);
  if (id) {
    object->AssertWriteEnabled(false);
    DbFiler* undo_filer = UndoFiler();
    if (undo_filer) {
      DbObjectImpl::WriteAppendUndo(undo_filer, true);
    }
  }

  return id;
}

void DbDatabase::StartUndoRecord() {
  DbUndoFiler* undo_filer = impl_->UndoFiler(true);
  if (undo_filer)
    undo_filer->StartUndoRecord();
}

bool DbDatabase::HasUndo() const {
  scoped_refptr<DbUndoFiler> undo_filer = impl_->UndoFiler();
  return undo_filer && undo_filer->HasUndo();
}

bool DbDatabase::HasRedo() const {
  scoped_refptr<DbUndoFiler> undo_filer = impl_->UndoFiler();
  if (undo_filer)
    undo_filer = undo_filer->RedoFiler();

  return undo_filer && undo_filer->HasUndo();
}

void DbDatabase::Undo() {
  scoped_refptr<DbUndoFiler> undo_filer = impl_->undo_filer;
  if (undo_filer && undo_filer->HasUndo()) {
    impl_->undo_filer = undo_filer->RedoFiler(true);
    if (!impl_->undo_filer)
      DisableUndoRecording(true);

    impl_->SetPerformingDbUndo(true);

    StartUndoRecord();

    undo_filer->Undo();

    impl_->SetPerformingDbUndo(false);

    if (!impl_->undo_filer)
      DisableUndoRecording(false);

    undo_filer->RedoFiler() = impl_->undo_filer;
    impl_->undo_filer = undo_filer;
  } else {
    NOTREACHED();
  }
}

void DbDatabase::Redo() {
  DbUndoFiler* redo_filer = impl_->UndoFiler(true);
  if (redo_filer)
    redo_filer = redo_filer->RedoFiler().get();

  if (redo_filer && redo_filer->HasUndo()) {
    impl_->SetPerformingDbUndo(true);

    StartUndoRecord();
    redo_filer->Undo();

    impl_->SetPerformingDbUndo(false);
  } else {
    NOTREACHED();
  }
}


DbFiler* DbDatabase::UndoFiler() {
  return impl_->UndoFiler();
}


void DbDatabase::ApplyPartialUndo(DbFiler* filer, void* desc) {
  if (desc == DbDatabase::Desc()) {
    uint16_t flag;
    if (!filer->ReadU16(&flag))
      return;

    switch (flag) {
      case 0: {
        static_cast<DbUndoFiler*>(filer)->UndoDone();
        break;
      }
    }
  } else {
    DbObject::ApplyPartialUndo(filer, desc);
  }
}

}  // namespace AcDb