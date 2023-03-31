#ifndef _HANDLE_TREE_H_
#define _HANDLE_TREE_H_

#include "absl/container/btree_map.h"

namespace AcDb {
class DbDatabase;
class DbStub;

class HandleTree {
 public:
  HandleTree();
  ~HandleTree();

  uint32_t GetNextHandle() const{ return next_handle_; }

  DbStub* AddHandle(DbDatabase* db, uint32_t handle);

  DbStub* GetHandleHolder(uint32_t handle);

 private:
  absl::btree_map<uint32_t, std::unique_ptr<DbStub>> handle_map_;

  uint32_t next_handle_;
};

}  // namespace AcDb

#endif