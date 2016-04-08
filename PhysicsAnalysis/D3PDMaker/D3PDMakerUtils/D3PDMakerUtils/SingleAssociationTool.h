// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/SingleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Type-safe wrapper for single associator tools.
 */


#ifndef D3PDMAKERUTILS_SINGLEASSOCIATIONTOOL_H
#define D3PDMAKERUTILS_SINGLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationToolImpl.h"
#include "D3PDMakerUtils/Types.h"



namespace D3PD {


/**
 * @brief Helper for @c SingleAssociationTool.
 *
 * This factors out everything that doesn't depend on the source type.
 */
template <typename TO_T>
class SingleAssociationToolTo
  : public SingleAssociationToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SingleAssociationToolTo (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);


  /**
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const;


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c get().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObject (const typename SelectType<TO_T, 0>::type* p);


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c getUntyped().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObjectUntyped (const void* p);
};


//=========================================================================


/**
 * @brief Type-safe wrapper for single associator tools.
 *
 * This can serve as the base class for a single association tool,
 * that associates from one source object to zero or one target objects.
 *
 * The template arguments give the types of the objects.
 *
 * Derived classes should define the @c get() method to carry
 * out the association, and optionally @c releaseObject().
 *
 * You may also specify that the tool can take as input any one of a set
 * of types.  To do this, use D3PD::Types<...> as the template argument;
 * for example
 *@code
 *   D3PD::SingleAssociationTool<D3PD::Types<Obj1, Obj2>, Target>
 @endcode
 *
 * In this case, you should define separate @c get methods
 * taking both @c Obj1 and @c Obj2.
 *
 * If the _output_ type @c TO_T is also @c Types, then the corresponding
 * output type for each input type is taken from it.  Examples:
 *
 *@code
 *  class Assoc1
 *    : public D3PD::SingleAssociationTool<D3PD::Types<Obj1, Obj2>, Target>
 *  {
 *    ...
 *    virtual const Target* get (const Obj1& o) override;
 *    virtual const Target* get (const Obj2& o) override;
 *  };
 *
 *  class Assoc2
 *    : public D3PD::SingleAssociationTool<D3PD::Types<Obj1, Obj2>,
 *                                         D3PD::Types<Targ1, Targ2> >
 *  {
 *    ...
 *    virtual const Targ1* get (const Obj1& o) override;
 *    virtual const Targ2* get (const Obj2& o) override;
 *  };
 *
 @endcode
 *
 * Note that the choice of which input type to use is made once,
 * during configuration; it is not dynamic.
 */
template <typename FROM_T, typename TO_T = FROM_T>
class SingleAssociationTool
  : public SingleAssociationToolTo<TO_T>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SingleAssociationTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   * Should be of the type given by @c typeinfo.
   */
  virtual const void* getUntyped (const void* p);


  /**
   * @brief Return the @c std::type_info for the source of the association.
   */
  virtual const std::type_info& fromTypeinfo() const;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const TO_T* get (const FROM_T& p) = 0;
};


} // namespace D3PD


#include "D3PDMakerUtils/SingleAssociationTool.icc"
#include "D3PDMakerUtils/SingleAssociationToolMulti.h"


#endif // not D3PDMAKERUTILS_SINGLEASSOCIATIONTOOL_H
