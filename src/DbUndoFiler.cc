#include "DbUndoFiler.h"
#include "DbObject.h"
#include "DbDatabase.h"

#include <string>

namespace AcDb {

DbUndoFiler::DbUndoFiler() {
  stream_ = base::MakeRefCounted<PageStream>(1000);
}

DbUndoFiler::~DbUndoFiler() = default;

DbDatabase* DbUndoFiler::Database() const {
  return db_;
}

void DbUndoFiler::Push() {
  Write(&record_pos_);
  uint64_t length = Tell();
  Seek(0, SeekType::SET);

  std::string buffer;
  buffer.resize(length);

  ReadBytes(reinterpret_cast<uint8_t*>(&buffer[0]), length);

  records.push(std::move(buffer));
}

bool DbUndoFiler::Pop() {
  if (records.empty())
    return false;

  Seek(0, SeekType::SET);
  std::string& buffer = records.top();
  WriteBytes(reinterpret_cast<uint8_t*>(&buffer[0]), buffer.size());

  records.pop();

  return true;
}

void DbUndoFiler::WriteObjectUndo(DbObject* object, bool automatic) {
  int32_t pre_pos = record_pos_;
  record_pos_ = Tell();

  Write(&pre_pos);
  WriteAddress(object);
  WriteBool(automatic);

  if (automatic)
    object->DataOutFields(this);
}

bool DbUndoFiler::Load() {
  if (!records.empty()) {
    bool ret = Pop();
    if (Tell()) {
      Seek(-4, SeekType::CUR);
      if (!Read(&record_pos_))
        return false;

      Seek(-4, SeekType::CUR);

      record_started_ = true;
      return true;
    }
  }

  return false;
}

void DbUndoFiler::StartUndoRecord() {
  if (record_started_)
    Push();

  Seek(0, SeekType::SET);
  redo_filer_.reset();

  record_pos_ = -1;

  WriteObjectUndo(Database(), false);
  WriteAddress(DbDatabase::Desc());
  WriteU16(0);

  record_started_ = true;
}

bool DbUndoFiler::HasUndo() const {
  return record_started_ || !records.empty();
}

void DbUndoFiler::Undo() {
  int32_t rec_pos = 0;
  uint64_t end_pos = Tell();
  undo_done_ = false;

  do {
    if (record_pos_ < 0) {
      if (!Load()) {
        record_started_ = false;
        break;
      }
      end_pos = Tell();
    }

    Seek(record_pos_, SeekType::SET);
    rec_pos = record_pos_;

    if (!Read(&record_pos_))
      break;

    DbObject* object;
    if (!ReadAddress((void**)&object))
      break;

    bool automatic;
    if (!ReadBool(&automatic))
      break;

    if (automatic)
      object->DataInFields(this);
    else if (Tell() < end_pos) {
      void* desc = nullptr;
      if (!ReadAddress(&desc))
        break;
      object->ApplyPartialUndo(this, desc);
    }
      
    end_pos = rec_pos;
  } while (!undo_done_);
}


void DbUndoFiler::UndoDone() {
  undo_done_ = true;
  record_started_ = false;
}

scoped_refptr<DbUndoFiler>& DbUndoFiler::RedoFiler(bool create /*= false*/) {
  if (!redo_filer_ && create) {
    redo_filer_ = new DbUndoFiler;
    redo_filer_->SetDatabase(Database());
  }

  return redo_filer_;
}

uint64_t DbUndoFiler::Seek(int64_t offset, SeekType type) {
  return stream_->Seek(offset, type);
}

uint64_t DbUndoFiler::Tell() {
  return stream_->Tell();
}

bool DbUndoFiler::ReadBool(bool* v) {
  static uint8_t value;
  bool ret = ReadU8(&value);

  if (ret)
    *v = value != 0;

  return ret;
}

bool DbUndoFiler::ReadU8(uint8_t* v) {
  return stream_->ReadByte(v);
}

bool DbUndoFiler::ReadU16(uint16_t* v) {
  return Read(v);
}

bool DbUndoFiler::ReadU32(uint32_t* v) {
  return Read(v);
}

bool DbUndoFiler::Read64(uint64_t* v) {
  return Read(v);
}

bool DbUndoFiler::ReadDouble(double* v) {
  return Read(v);
}

bool DbUndoFiler::ReadString(std::string* v) {
  static uint8_t size;

  if (!ReadU8(&size))
    return false;

  v->resize(size);

  return ReadBytes(reinterpret_cast<uint8_t*>(v->data()), size);
}

bool DbUndoFiler::ReadAddress(void** v) {
  return ReadBytes(reinterpret_cast<uint8_t*>(v), sizeof(void*));
}

bool DbUndoFiler::ReadBytes(uint8_t* buf, uint32_t size) {
  return stream_->ReadBytes(buf, size);
}

bool DbUndoFiler::ReadHardOwnershipId(DbObjectID& id) {
  DbStub* stub = nullptr;
  bool ret = ReadAddress(reinterpret_cast<void**>(&stub));
  if (ret)
     id.SetAddress(stub);

  return ret;
}

bool DbUndoFiler::ReadSoftOwndershipId(DbObjectID& id) {
  DbStub* stub = nullptr;
  bool ret = ReadAddress(reinterpret_cast<void**>(&stub));
  if (ret)
    id.SetAddress(stub);

  return ret;
}

bool DbUndoFiler::ReadHardPointerId(DbObjectID& id) {
  DbStub* stub = nullptr;
  bool ret = ReadAddress(reinterpret_cast<void**>(&stub));
  if (ret)
    id.SetAddress(stub);

  return ret;
}

bool DbUndoFiler::ReadSoftPointerId(DbObjectID& id) {
  DbStub* stub = nullptr;
  bool ret = ReadAddress(reinterpret_cast<void**>(&stub));
  if (ret)
    id.SetAddress(stub);

  return ret;
}

bool DbUndoFiler::WriteBool(bool v) {
  return WriteU8(v ? 1 : 0);
}

bool DbUndoFiler::WriteU8(uint8_t v) {
  return Write(&v);
}

bool DbUndoFiler::WriteU16(uint16_t v) {
  return Write(&v);
}

bool DbUndoFiler::WriteU32(uint32_t v) {
  return Write(&v);
}

bool DbUndoFiler::Write64(uint64_t v) {
  return Write(&v);
}

bool DbUndoFiler::WriteDouble(double v) {
  return Write(&v);
}

bool DbUndoFiler::WriteString(const std::string& v) {
  static uint8_t size;
  size = v.size();

  if (!Write(&size))
    return false;

  return WriteBytes((uint8_t*)(&v[0]), size);
}

bool DbUndoFiler::WriteAddress(void* v) {
  return WriteBytes(reinterpret_cast<uint8_t*>(&v), sizeof(void*));
}

bool DbUndoFiler::WriteBytes(const uint8_t* buf, uint32_t size) {
  return stream_->WriteBytes(buf, size);
}

bool DbUndoFiler::WriteHardOwnershipId(const DbObjectID& id) {
  return WriteAddress(id.Address());
}

bool DbUndoFiler::WriteSoftOwndershipId(const DbObjectID& id) {
  return WriteAddress(id.Address());
}

bool DbUndoFiler::WriteHardPointerId(const DbObjectID& id) {
  return WriteAddress(id.Address());
}

bool DbUndoFiler::WriteSoftPointerId(const DbObjectID& id) {
  return WriteAddress(id.Address());
}

}  // namespace AcDb