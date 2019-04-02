/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//        Package    : RootStorageSvc (The POOL project)
//
//        Author     : M.Frank
//====================================================================
#ifndef POOL_ROOTSTORAGESVC_ROOTKEYIOHANDLER_H
#define POOL_ROOTSTORAGESVC_ROOTKEYIOHANDLER_H 1

// Forward declarations
class TClass;
class TKey;

namespace pool {

  /** @class IRootKeyIOHandler IRootKeyIOHandler.h RootStorageSvc/IRootKeyIOHandler.h
    * Helper to perform keyed object I/O.
    *
    * @author  M.Frank
    * @date    1/2/2003
    * @version 1.0
    */
  class RootKeyIOHandler {
  public:
    /// Initializing constructor
    RootKeyIOHandler() {}
    /// Standard Destructor
    virtual ~RootKeyIOHandler() {}
    /// Release object.
    virtual void release();

    /// Write object by key: Specification of the object class can considerably speed-up
    /** Write object using ROOT Key mode
      * @param cl          [IN]    ROOT Class of the specified object
      * @param knam        [IN]    Key-name of the object to be written
      * @param obj         [IN]    Pointer to the object to be written.
      * @param opt         [IN]    Root write flags.
      * @param siz         [IN]    Optimal buffer size.
      *
      * @return Number of bytes written to file. 0 or negative on failure.
      */
    virtual int  write(TClass* cl, const char* knam, const void* obj, int opt, int siz=0) const;

    /// Write object by key: Specification of the object class can considerably speed-up
    /** Write object using ROOT Key mode
      * @param cl          [IN]    ROOT Class of the specified object
      * @param knam        [IN]    Key-name of the object to be written
      * @param title       [IN]    Title of the object to be written
      * @param obj         [IN]    Pointer to the object to be written.
      * @param opt         [IN]    Root write flags.
      * @param siz         [IN]    Optimal buffer size.
      *
      * @return Number of bytes written to file. 0 or negative on failure.
      */
    virtual int  write(TClass* cl, const char* knam, const char* title, const void* obj, int opt, int siz=0) const;

    /// Read object with key.
    /** Read object using ROOT Key mode
      * @param key         [IN]    Key of the object.
      * @param obj         [IN]    Location to store pointer to the object.
      *
      * @return Number of bytes read from file. 0 or negative on failure.
      */
    virtual int  read(TKey* key, void** obj) const;

    /// Read object with key.
    /** Read object using ROOT Key mode
      * @param knam        [IN]    Key-name of the object.
      * @param obj         [IN]    Location to store pointer to the object.
      *
      * @return Number of bytes read from file. 0 or negative on failure.
      */
    virtual int  read(const char* knam, void** obj) const;

    /// Destroy object with key.
    /** Destroy object using ROOT Key mode
      * @param knam        [IN]    Key-name of the object.
      *
      * @return Number of bytes read from file. 0 or negative on failure.
      */
    virtual int  destroy(const char* knam) const;
  };
}
#endif // POOL_ROOTSTORAGESVC_ROOTKEYIOHANDLER_H
