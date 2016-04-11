/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbObjectAccessor.h 726071 2016-02-25 09:23:05Z krasznaa $
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
    /** Open object using either handle starting
      * from the container handle of the originating object.
      *
      * @param objH      [IN/OUT] Handle to the object to be loaded.
      * @param cntH      [IN]     Handle to source object's container.
      * @param tokenH    [IN]     Reference to token of the target object.
      * @param mod       [IN]     Desired opening mode.
      *
      * @return DbStatus code indicating success or failure.
      */
    static DbStatus open( void**                    ptr,
                          ShapeH                    shape,
                          const DbContainer&        cntH,
                          const Token&              tokenH, 
                          DbAccessMode              mod);

    /// Retrieve hosting container
    static const DbContainer& containedIn(const DbObject* pObj);
    
    /// Access object oid
    static Token::OID_t& objectOid(const DbObject* ptr);

    /// Add persistent association entry
    static DbStatus makeObjectLink( const DbObject*     pObj,
                                    const Token*        pToken,
                                    Token::OID_t&       tokenH);
    
    /// Validate an object association
    static DbStatus getObjectLink(  const DbObject*     pObj,
                                    const Token::OID_t& tokenH,
                                    Token*              pToken);
  };
}       // End namespace pool
#endif  // POOL_DBOBJECTACCESSOR_H
