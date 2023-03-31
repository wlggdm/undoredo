#include "DbObjectId.h"
#include "DbStub.h"

namespace AcDb {

DbObject* DbObjectID::Object() {
  return holder_ ? holder_->GetObject() : nullptr;
}

DbDatabase* DbObjectID::Database() {
  return holder_ ? holder_->GetDatabase() : nullptr;
}

bool DbObjectID::operator<(const DbObjectID& other) const {
  if (!holder_ || !other.holder_) {
    return holder_ < other.holder_;
  }

  return holder_->GetHandle() < other.holder_->GetHandle();
}


uint64_t DbObjectID::Handle() const {
  return holder_->GetHandle();
}

bool DbObjectID::operator==(const DbObjectID& other) const {
  return holder_ == other.holder_;
}

}  // namespace AcDb