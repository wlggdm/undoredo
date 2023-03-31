#ifndef _OBJECT_ID_H_
#define _OBJECT_ID_H_

#include <stdint.h>

namespace AcDb {
class DbStub;
class DbObject;
class DbDatabase;

class DbObjectID {
 public:
  DbObjectID(DbStub* holder = nullptr) : holder_(holder) {}
  ~DbObjectID() = default;

  DbObject* Object();

  DbDatabase* Database();

  void SetAddress(DbStub* stub) { holder_ = stub; }

  DbStub* Address() const { return holder_; }

  operator bool() { return !!holder_; }

  DbObjectID& operator==(DbStub* holder) { 
    holder_ = holder;
    return *this;
  }

  bool operator<(const DbObjectID& other) const;

  bool operator==(const DbObjectID& other) const;

  uint64_t Handle() const;

 private:
  DbStub* holder_;
};

}  // namespace AcDb

#endif