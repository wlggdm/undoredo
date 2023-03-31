#ifndef _OBJECT_IMPL_H_
#define _OBJECT_IMPL_H_

#include "DbObjectId.h"

namespace AcDb {
class DbFiler;
class DbObject;
class DbDatabase;

class DbObjectImpl {
 public:
  DbObjectImpl();
  virtual ~DbObjectImpl();
  
  static DbObjectImpl* GetImpl(DbObject* object);

  DbObjectID ObjectID() { return object_id_; }
  void SetObjectID(DbObjectID id) { object_id_ = id; }

  DbDatabase* Database() { return db_; }
  void SetDatabase(DbDatabase* db) { db_ = db; }

  void DisableUndoRecording(bool disable) { disable_undo_recording_ = disable; }
  bool IsUndoRecordingDisabled() const { return disable_undo_recording_; }

  void SetUndoAppend(bool append) { undo_append = append; }
  bool IsUndoAppend() const { return undo_append; }

  void SetErased(bool erase) { erased = erase; }
  bool IsErased() const { return erased; }

  static void WriteAppendUndo(DbFiler* filer, bool append);
  static void WriteEraseUndo(DbFiler* filer, bool erase);

 private:
  DbObjectID object_id_;

  DbDatabase* db_;

  bool disable_undo_recording_;

  bool undo_append = false;

  bool erased = false;
};

}  // namespace AcDb

#endif