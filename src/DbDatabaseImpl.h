#ifndef _DATABASE_IMPL_H_
#define _DATABASE_IMPL_H_

#include "DbObjectImpl.h"
#include "HandleTree.h"

#include "base/memory/scoped_refptr.h"

#include <memory>

namespace AcDb {
class DbDatabase;
class DbUndoFiler;

class DbDatabaseImpl : public DbObjectImpl {
 public:
  DbDatabaseImpl();
  ~DbDatabaseImpl() override;

  static DbDatabaseImpl* GetImpl(DbDatabase* database);

  DbObjectID AddObject(DbObject* object, uint32_t handle = 0);

  DbUndoFiler* UndoFiler(bool create = false, DbDatabase* db = nullptr);

  void SetPerformingDbUndo(bool undoing) { perform_undo_ = undoing; }
  bool GetPerfromingDbUndo() const { return perform_undo_; }

  scoped_refptr<DbUndoFiler> undo_filer;

 private:
  HandleTree handle_tree;

  bool perform_undo_;
};

}  // namespace AcDb

#endif