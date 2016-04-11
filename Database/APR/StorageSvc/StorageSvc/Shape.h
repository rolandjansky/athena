/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  Shape definition file
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//====================================================================
#ifndef POOL_SHAPE_H
#define POOL_SHAPE_H

// Framework include files
#include "PersistentDataModel/Guid.h"
#include <string>

/*
 *  POOL namespace
 */
namespace pool  {

  /** @class Shape Shape.h StorageSvc/Shape.h
    *
    * Description:
    *
    * Without a concrete implementation behind, this class
    * is rather useless. Objects of this type cannot be used
    * directly. This class can hence only serve as a base class.
    *
    * @author   M.Frank
    * @version  1.0
    * @date     03/09/2002
    */
  class Shape  {
  protected:
    /// Object global identifier
    Guid                  m_id;
  public:
    /// Standard Constructor
    Shape()                                   {                               }
    /// Standard Destructor
    virtual ~Shape()                          {                               }
    /// Access database identifier
    const Guid& shapeID() const               {  return m_id;                 }
          Guid& shapeID()                     {  return m_id;                 }
    /// Access database identifier
    void setShapeID(const Guid& id)           {  m_id=id;                     }
    Guid& setShapeID(const std::string &id)   {  m_id.fromString(id); return  shapeID(); }
  };
}       // End namespace pool
#endif  // POOL_SHAPE_H
