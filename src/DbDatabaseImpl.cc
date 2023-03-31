#include "DbDatabaseImpl.h"
#include "DbDatabase.h"
#include "DbStub.h"
#include "DbUndoFiler.h"

namespace AcDb {

DbDatabaseImpl::DbDatabaseImpl() {}

DbDatabaseImpl::~DbDatabaseImpl() {}

DbDatabaseImpl* DbDatabaseImpl::GetImpl(DbDatabase* database) {
   return database ? database->impl_ : nullptr;
}

DbObjectID DbDatabaseImpl::AddObject(DbObject* object, uint32_t handle) {
  if (handle == 0)
    handle = handle_tree.GetNextHandle();

  DbStub* holder = handle_tree.AddHandle(Database(), handle);

  if (holder)
    holder->SetObject(object);

  DbObjectID id(holder);

  DbObjectImpl::GetImpl(object)->SetObjectID(id);

  return id;
}

DbUndoFiler* DbDatabaseImpl::UndoFiler(bool create, DbDatabase* db) {
  if (!undo_filer && create) {
    undo_filer = new DbUndoFiler;
    undo_filer->SetDatabase(db ? db : Database());
  }

  return undo_filer.get();
}

}  // namespace AcDb