#ifndef _UNDO_FILER_H_
#define _UNDO_FILER_H_

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_refptr.h"
#include "DbFiler.h"
#include "../filer/PageStream.h"

#include <stack>

namespace AcDb {
class DbObject;

class DbUndoFiler : public DbFiler, public base::RefCounted<DbUndoFiler> {
 public:
  DbUndoFiler();

  DbDatabase* Database() const override;
  void SetDatabase(DbDatabase* db) { db_ = db; }

  //void SetStream(Stream* stream) { stream_ = stream; }

  void StartUndoRecord();

  bool HasUndo() const;

  void Undo();

  void UndoDone();

  scoped_refptr<DbUndoFiler>& RedoFiler(bool create = false);

  void Push();

  bool Pop();

  void WriteObjectUndo(DbObject* object, bool automatic);

  bool Load();

  uint64_t Seek(int64_t offset, SeekType type) override;
  uint64_t Tell() override;

  // Read
  bool ReadBool(bool* v) override;
  bool ReadU8(uint8_t* v) override;
  bool ReadU16(uint16_t* v) override;
  bool ReadU32(uint32_t* v) override;
  bool Read64(uint64_t* v) override;
  bool ReadDouble(double* v) override;
  bool ReadString(std::string* v) override;
  bool ReadAddress(void** v) override;
  bool ReadBytes(uint8_t* buf, uint32_t size) override;
  bool ReadHardOwnershipId(DbObjectID& id) override;
  bool ReadSoftOwndershipId(DbObjectID& id) override;
  bool ReadHardPointerId(DbObjectID& id) override;
  bool ReadSoftPointerId(DbObjectID& id) override;

  template <typename T>
  bool Read(T* value) {
    return ReadBytes(reinterpret_cast<uint8_t*>(value), sizeof(T));
  }

  // Write
  bool WriteBool(bool v) override;
  bool WriteU8(uint8_t v) override;
  bool WriteU16(uint16_t v) override;
  bool WriteU32(uint32_t v) override;
  bool Write64(uint64_t v) override;
  bool WriteDouble(double v) override;
  bool WriteString(const std::string& v) override;
  bool WriteAddress(void* v) override;
  bool WriteBytes(const uint8_t* buf, uint32_t size) override;
  bool WriteHardOwnershipId(const DbObjectID& id) override;
  bool WriteSoftOwndershipId(const DbObjectID& id) override;
  bool WriteHardPointerId(const DbObjectID& id) override;
  bool WriteSoftPointerId(const DbObjectID& id) override;

  template <typename T>
  bool Write(T* value) {
    return WriteBytes(reinterpret_cast<uint8_t*>(value), sizeof(T));
  }

protected:
  virtual ~DbUndoFiler();

 private:
  friend class base::RefCounted<DbUndoFiler>;

  DbDatabase* db_ = nullptr;

  scoped_refptr<Stream> stream_;

  scoped_refptr<DbUndoFiler> redo_filer_;

  std::stack<std::string> records;

  bool record_started_ = false;

  int32_t record_pos_ = 0;

  bool undo_done_ = false;
};

}  // namespace AcDb

#endif