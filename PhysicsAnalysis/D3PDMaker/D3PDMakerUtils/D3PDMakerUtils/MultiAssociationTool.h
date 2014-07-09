// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/MultiAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Type-safe wrapper for multiple-target associator tools.
 */


#ifndef D3PDMAKERUTILS_MULTIASSOCIATIONTOOL_H
#define D3PDMAKERUTILS_MULTIASSOCIATIONTOOL_H


#include "D3PDMakerUtils/MultiAssociationToolImpl.h"


namespace D3PD {


/**
 * @brief Helper for @c MultiAssociationTool.
 *
 * This factors out everything that doesn't depend on the source type.
 */
template <typename TO_T>
class MultiAssociationToolTo
  : public MultiAssociationToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MultiAssociationToolTo (const std::string& type,
                          const std::string& name,
                          const IInterface* parent);


  /**
   * @brief Return the element type for the target of the association.
   *
   * I.e., @c nextUntyped returns a pointer to this type.
   */
  virtual const std::type_info& elementTypeinfo() const;


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const void* nextUntyped();


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const TO_T* next () = 0;


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c nextUntyped().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseElementUntyped (const void* p);


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c next().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseElement (const TO_T* p);
};


//=========================================================================


/**
 * @brief Type-safe wrapper for multiple-target associator tools.
 *
 * This can serve as the base class for a multiple-target association tool,
 * that associates from one source object to a set of target objects.
 *
 * The template arguments give the types of the objects.
 *
 * Derived classes should define the @c reset() and @c next() methods to carry
 * out the association, and optionally @c releaseElement().
 *
 * You may also specify that the tool can take as input any one of a set
 * of types.  To do this, use D3PD::Types<...> as the template argument;
 * for example
 *@code
 *   D3PD::MultiAssociationTool<D3PD::Types<Obj1, Obj2>, Target>
 @endcode
 *
 * In this case, you should define separate @c reset methods
 * taking both @c Obj1 and @c Obj2.
 *
 * If the _output_ type @c TO_T is also @c Types, then the corresponding
 * output type for each input type is taken from it.  In this case,
 * there are multiple `next()` methods, each returning a different type.
 * In order to be able to differentiate between these via overload
 * resolution, the `next()` methods take a dummy pointer argument
 * giving the source type.  Examples:
 *
 *@code
 *  class Assoc1
 *    : public D3PD::MultiAssociationTool<D3PD::Types<Obj1, Obj2>, Target>
 *  {
 *    ...
 *    virtual StatusCode reset (const Obj1& o) override;
 *    virtual StatusCode reset (const Obj2& o) override;
 *
 *    virtual const Target* next() override;
 *  };
 *
 *  class Assoc2
 *    : public D3PD::SingleAssociationTool<D3PD::Types<Obj1, Obj2>,
 *                                         D3PD::Types<Targ1, Targ2> >
 *  {
 *    ...
 *    virtual StatusCode reset (const Obj1& o) override;
 *    virtual StatusCode reset (const Obj2& o) override;
 *
 *    virtual const Targ1* next (const Obj1* dummy) override;
 *    virtual const Targ2* next (const Obj2* dummy) override;
 *  };
 *
 @endcode
 *
 * Note that the choice of which input type to use is made once,
 * during configuration; it is not dynamic.
 */
template <typename FROM_T, typename TO_T = FROM_T>
class MultiAssociationTool
  : public MultiAssociationToolTo<TO_T>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MultiAssociationTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);


  /**
   * @brief Return the @c std::type_info for the source of the association.
   */
  virtual const std::type_info& fromTypeinfo() const;


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode resetUntyped (const void* p);


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const FROM_T& p) = 0;
};


} // namespace D3PD


#include "D3PDMakerUtils/MultiAssociationTool.icc"
#include "D3PDMakerUtils/MultiAssociationToolMulti.h"


#endif // not D3PDMAKERUTILS_MULTIASSOCIATIONTOOL_H
