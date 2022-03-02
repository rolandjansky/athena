/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBOBJECTACCESSOR_H
#define POOL_DBOBJECTACCESSOR_H 1

#ifndef POOL_DBOBJECT_H
#error "The header file DbObjectAccessor can only be included by DbObject.h"
#endif

/*
 * POOL namespace declaration
 */
namespace pool  {

  /** Db objects: class DbObjectAccessor

    Description:
    Access class to implement handle actions

    @author  M.Frank
    @version 1.0
  */
  class DbObjectAccessor   {
  public:
    /** Read object from a container, using its OID
      *
      * @param ptr       [OUT]    Pointer to the retrieved object 
      * @param shape     [IN]     Object type information
      * @param cntH      [IN]     Handle to source object's container.
      * @param OID       [IN]     OID of the target object.
      *
      * @return DbStatus code indicating success or failure.
      */
    static DbStatus read( void**                    ptr,
                          ShapeH                    shape,
                          DbContainer&              cntH,
                          const Token::OID_t&       oid,
                          int                       merge_section );

    /// Retrieve hosting container
    static const DbContainer& containedIn(const DbObject* pObj);
    
    /// Access object oid
    static Token::OID_t& objectOid(DbObject* ptr);

    /// Access object oid
    static const Token::OID_t& objectOid(const DbObject* ptr);

    /// Add persistent association entry
    static DbStatus makeObjectLink ATLAS_NOT_THREAD_SAFE
      ( const DbObject*     pObj,
        Token*              pToken,
        Token::OID_t&       tokenH );
    
  };
}       // End namespace pool
#endif  // POOL_DBOBJECTACCESSOR_H
