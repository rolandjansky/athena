/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_DATACALLBACK_H
#define POOL_DATACALLBACK_H

// Framework include files
#include "StorageSvc/pool.h"

/*
 *   POOL namespace declaration
 */
namespace pool {

  // Forward declarations
  class DbColumn;
  typedef const class Shape *ShapeH;

  /** Db objects: DbCallBack
    *
    * Description:
    * Definition of the generic callback object used to implement
    * customized read and write operations.
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class DataCallBack {
  public:
    enum DataType  {  PRIMITIVE = 0, OBJECT = 1, REFERENCE = 2, TOKEN = 3 };
    enum CallType  {  GET = 1, PUT = 2  };
    enum           {  SKIP = 3          };       // Flag to indicate skipping a column.

    DataCallBack() = default;
    DataCallBack(const DataCallBack&) = default;
    DataCallBack& operator=(const DataCallBack&) = default;

    /// Standard destructor
    virtual ~DataCallBack()             {                                    }

    /// Abstract delete (allows if overridden for callback re-use)
    virtual void release()              {   delete this;                     }

    /// Pass object for conversion data to the callback
    virtual void setObject(const void* ptr) = 0;

    /// Retrieve object from the callback
    virtual const void* object()  const = 0;

    /// Pass shape object for conversion data to the callback
    /**    @param shape          [IN] Reference to shape object
    */
    virtual void setShape(ShapeH shape) = 0;

    /// Retrieve shape object from the callback
    virtual ShapeH shape()  const = 0;

    /// Callback when a read sequence should be started
    /** @param action_type    [IN] Action type: PUT/GET
      * @param data           [IN] Reference to persistent data location (on Write)
      * @param context        [IN] Data context for subsequent calls
      *
      * @return DbStatus indicating success or failure.
      */
    virtual DbStatus start(CallType action_type, void* data, void** context) = 0;

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
    virtual DbStatus bind(CallType             action_type,    // GET or PUT
                          const DbColumn*      col_identifier, // Col ID
                          int                  col_number,     // Col Number
                          void*                context,        // Start context
                          void**               data) = 0;      // data pointer

    /// Callback when a read sequence should be started
    /** @param action    [IN] Action type: PUT/GET
      * @param context   [IN] Data context returned by "start"
      *
      * @return DbStatus indicating success or failure.
      */
    virtual DbStatus end(CallType action, void* context) = 0;
  };
}
#endif // POOL_DATACALLBACK_H
