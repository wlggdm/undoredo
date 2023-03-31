#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "DbObject.h"

namespace AcDb {

class DbDatabaseImpl;

class DbDatabase : public DbObject {
 public:
  DB_DECLARE_MEMBER(DbDatabase)
  DbDatabase();

  DbObjectID AppendObject(DbObject* object);

  void StartUndoRecord();

  bool HasUndo() const;
  bool HasRedo() const;

  void Undo();
  void Redo();

  DbFiler* UndoFiler();

  void ApplyPartialUndo(DbFiler* filer, void* desc) override;

 protected:
  ~DbDatabase() override;

 private:
  friend class DbDatabaseImpl;
  DbDatabaseImpl* impl_;
};

}  // namespace AcDb

#endif