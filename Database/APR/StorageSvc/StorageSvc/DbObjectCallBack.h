/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGESVC_DBOBJECTCALLBACK_H
#define STORAGESVC_DBOBJECTCALLBACK_H

// POOL include files
#include "StorageSvc/DataCallBack.h"

/*
 * POOL namespace
 */
namespace pool {

  /** 
    * @class DbObjectCallBack DbObjectCallBack.h StorageSvc/DbObjectCallBack.h
    *
    * A default implementation of a DataCallBack object performing
    * the identity transformation (tranient shape = persistent shape)
    */
  class DbObjectCallBack : public DataCallBack  {
  public:
    /// Constructor
    explicit DbObjectCallBack();
    /// Destuctor
    ~DbObjectCallBack();
    /// Passes the pointer to the transient data buffer
    void setObject( const void* ptr );
    /// Retrieves the pointer to the transient data buffer
    const void* object() const;
    /// Passes the persistent shape
    void setShape( ShapeH shape );
    /// Retrieves the persistent shape
    ShapeH shape() const;

    /// Callback when a read/write sequence should be started
    /** @param action_type    [IN] Action type: PUT/GET
      * @param data           [IN] Reference to persistent data location (on Write)
      * @param context        [IN] Data context for subsequent calls
      *
      * @return DbStatus indicating success or failure.
      */
    DbStatus start( CallType  action_type,
                          void*     data,
                          void**    context);

    /// Callback to retrieve the absolute address of a column
    /** @param action_type    [IN] Action type: PUT/GET
      * @param col_identifier [IN] Reference to persistent column to be processed.
      * @param col_number     [IN] Number of persistent column.
      * @param context        [IN] Data context returned by "start"
      * @param data          [OUT] Reference to data location
      *
      * @return DbStatus indicating success or failure.
      *         If SKIP is returned, the column will not be processed.
      */
    DbStatus bind(CallType        action_type,
                        const DbColumn* col_identifier,
                        int             col_number,
                        void*           context,
                        void**          data );

    /// Callback when a read/write sequence should be finalized
    /** @param action    [IN] Action type: PUT/GET
      * @param context   [IN] Data context returned by "start"
      *
      * @return DbStatus indicating success or failure.
      */
    DbStatus end( CallType action, void* context );
    
  protected:
    const Shape*                       m_shape;
    const void*                        m_objectData;

    // No assignment operator
    DbObjectCallBack& operator=( const DbObjectCallBack& );
    // No copy constructor
    DbObjectCallBack( const DbObjectCallBack& );
  };
}

#include "StorageSvc/DbObjectCallBack.inl"

#endif
