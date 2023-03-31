#ifndef _OBJECT_HOLDER_H_
#define _OBJECT_HOLDER_H_

#include <stdint.h>

namespace AcDb {
class DbObject;
class DbDatabase;

class DbStub {
 public:
  DbStub(DbDatabase* db, uint32_t handle) : db_(db), handle_(handle) {}
  ~DbStub() = default;

  uint32_t  GetHandle() { return handle_; }
 
  DbObject* GetObject() { return object_; }
  void      SetObject(DbObject* object) { object_ = object; }

  DbDatabase* GetDatabase() { return db_; }

 private:
  DbDatabase* db_;
  uint32_t  handle_;

  DbObject* object_ = nullptr;
};

}  // namespace AcDb

#endif