#include "DbObjectImpl.h"
#include "DbObject.h"

namespace AcDb {

DbObjectImpl::DbObjectImpl() {}

DbObjectImpl::~DbObjectImpl() {}

DbObjectImpl* DbObjectImpl::GetImpl(DbObject* object){
   return object ? object->impl_ : nullptr;
}

}  // namespace AcDb
