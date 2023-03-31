#include "DbObject.h"
#include "DbObjectImpl.h"
#include "DbDatabaseImpl.h"
#include "DbUndoFiler.h"
#include "DbDatabase.h"

#include "base/notreached.h"

namespace {
constexpr uint16_t kErase = 1;
constexpr uint16_t kUnErase = 2;
constexpr uint16_t kAppend = 3;
constexpr uint16_t kUnAppend = 4;
}

namespace AcDb {

DbObject::DbObject() : impl_(new DbObjectImpl()){
}

DbObject::DbObject(DbObjectImpl* impl) : impl_(impl){
}

DbObject::~DbObject() {
    delete impl_;
}

DbObjectID DbObject::ObjectID() {
  return impl_->ObjectID();
}

DbDatabase* DbObject::Database() {
  return impl_->Database();
}

void DbObject::DataInFields(DbFiler* filer) {
}

void DbObject::DataOutFields(DbFiler* filer) {
}

void DbObject::ApplyPartialUndo(DbFiler* filer, void* desc) {
  if (desc == DbObject::Desc()) {
    uint16_t code;
    if (!filer->ReadU16(&code))
      return;

    switch (code) { 
      case kAppend:
      case kUnAppend: {
        impl_->SetUndoAppend(true);
        Erase(code == kAppend);
        break;
      }
      case kErase:
      case kUnErase: {
        Erase(code == kUnErase);
        break;
      }
    }
  } else {
    NOTREACHED();
  }
}

DbFiler* DbObject::UndoFiler() {
  if (!Database())
    return nullptr;

  return !impl_->IsUndoRecordingDisabled() ? Database()->UndoFiler() : nullptr;
}

void DbObject::Erase(bool erase) {
  if (IsErased() != erase) {
    AssertWriteEnabled(false);
    impl_->SetErased(erase);

    DbFiler* undo_filer = UndoFiler();
    if (undo_filer) {
      if (impl_->IsUndoAppend()) {
        DbObjectImpl::WriteAppendUndo(undo_filer, !erase);
        impl_->SetUndoAppend(false);
      }
      else
        DbObjectImpl::WriteEraseUndo(undo_filer, erase);
    }
  }
}

bool DbObject::IsErased() {
  return impl_->IsErased();
}

void DbObject::DisableUndoRecording(bool disable) {
  impl_->DisableUndoRecording(disable);
}

void DbObject::AssertReadEnabled() {
}

void DbObject::AssertWriteEnabled(bool auto_undo /*= true*/) {
  DbDatabaseImpl* dbimpl = DbDatabaseImpl::GetImpl(Database());
  DbUndoFiler* undo_filer = dbimpl->UndoFiler();

  if (undo_filer && !undo_filer->HasUndo())
    undo_filer = nullptr;

  if (undo_filer && !impl_->IsUndoRecordingDisabled()) {
    if (auto_undo)
      undo_filer->WriteObjectUndo(this, auto_undo);
  }
}

void DbObjectImpl::WriteAppendUndo(DbFiler* filer, bool append) {
  filer->WriteAddress(DbObject::Desc());
  filer->WriteU16(append ? kAppend : kUnAppend);
}

void DbObjectImpl::WriteEraseUndo(DbFiler* filer, bool erase) {
  filer->WriteAddress(DbObject::Desc());
  filer->WriteU16(erase ? kErase : kUnErase);
}

}  // namespace AcDb
