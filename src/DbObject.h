#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "base/memory/ref_counted.h"
#include "DbObjectId.h"

#define DB_DECLARE_MEMBER(class_name)  \
  static void* Desc() { return (char*)#class_name; }

namespace AcDb {

class DbDatabase;
class DbObjectImpl;
class DbFiler;

class DbObject : public base::RefCounted<DbObject> {
 public:
  DB_DECLARE_MEMBER(DbObject)
  DbObject();
  
  DbObjectID ObjectID();

  DbDatabase* Database();

  void AssertReadEnabled();

  void AssertWriteEnabled(bool auto_undo = true);

  virtual void DataInFields(DbFiler* filer);
  virtual void DataOutFields(DbFiler* filer);

  virtual void ApplyPartialUndo(DbFiler* filer, void* desc);

  DbFiler* UndoFiler();

  void Erase(bool erase);
  bool IsErased();

  void DisableUndoRecording(bool disable);
 protected:
  DbObject(DbObjectImpl* impl);
  virtual ~DbObject();

 private:
  friend class base::RefCounted<DbObject>;

  friend class DbObjectImpl;

  DbObjectImpl* impl_;
};

}  // namespace AcDb

#endif