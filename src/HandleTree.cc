#include "HandleTree.h"
#include "DbObjectId.h"
#include "DbStub.h"

#include <memory>

namespace AcDb {

HandleTree::HandleTree() {}
HandleTree::~HandleTree() {}

DbStub* HandleTree::AddHandle(DbDatabase* db, uint32_t handle) {
  auto ret = handle_map_.emplace(handle, std::make_unique<DbStub>(db, handle));
  if (ret.second == false)
    return nullptr;

  if (next_handle_ <= handle)
    next_handle_ = handle + 1;

  return ret.first->second.get();
}

DbStub* HandleTree::GetHandleHolder(uint32_t handle) {
  auto ret = handle_map_.find(handle);
  if (ret == handle_map_.end())
    return nullptr;

  return ret->second.get();
}

}  // namespace AcDb
