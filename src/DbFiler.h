#include "../filer/stream.h"
#include "DbObjectId.h"

namespace AcDb {

class DbDatabase;

class DbFiler {
 public:
  virtual DbDatabase* Database() const = 0;

  virtual uint64_t Seek(int64_t offset, SeekType type) = 0;

  virtual uint64_t Tell() = 0;

  //Read
  virtual bool ReadBool(bool* v) = 0;

  virtual bool ReadU8(uint8_t* v) = 0;

  virtual bool ReadU16(uint16_t* v) = 0;

  virtual bool ReadU32(uint32_t* v) = 0;

  virtual bool Read64(uint64_t* v) = 0;

  virtual bool ReadDouble(double* v) = 0;

  virtual bool ReadString(std::string* v) = 0;

  virtual bool ReadAddress(void** v) = 0;

  virtual bool ReadBytes(uint8_t* buf, uint32_t size) = 0;

  virtual bool ReadHardOwnershipId(DbObjectID& id) = 0;

  virtual bool ReadSoftOwndershipId(DbObjectID& id) = 0;

  virtual bool ReadHardPointerId(DbObjectID& id) = 0;

  virtual bool ReadSoftPointerId(DbObjectID& id) = 0;

  //Write
  virtual bool WriteBool(bool v) = 0;

  virtual bool WriteU8(uint8_t v) = 0;

  virtual bool WriteU16(uint16_t v) = 0;

  virtual bool WriteU32(uint32_t v) = 0;

  virtual bool Write64(uint64_t v) = 0;

  virtual bool WriteDouble(double v) = 0;

  virtual bool WriteString(const std::string& v) = 0;

  virtual bool WriteAddress(void* v) = 0;

  virtual bool WriteBytes(const uint8_t* buf, uint32_t size) = 0;

  virtual bool WriteHardOwnershipId(const DbObjectID& id) = 0;

  virtual bool WriteSoftOwndershipId(const DbObjectID& id) = 0;

  virtual bool WriteHardPointerId(const DbObjectID& id) = 0;

  virtual bool WriteSoftPointerId(const DbObjectID& id) = 0;
};

}  // namespace AcDb