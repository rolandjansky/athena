// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/SingleAssociationToolMulti.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief A specialization of SingleAssociationTool that can accept one
 *        of several types.
 */


#ifndef D3PDMAKERUTILS_SINGLEASSOCIATIONTOOLMULTI_H
#define D3PDMAKERUTILS_SINGLEASSOCIATIONTOOLMULTI_H


#include "D3PDMakerUtils/Types.h"
#include "D3PDMakerUtils/SingleAssociationToolImpl.h"
#ifndef D3PDMAKERUTILS_SINGLEASSOCIATIONTOOL_H // prevent recursive include
#include "D3PDMakerUtils/SingleAssociationTool.h"
#endif

#include "boost/preprocessor/iteration/iterate.hpp"
#include <cstdlib>



namespace D3PD {


/**
 * @brief A specialization of SingleAssociationTool that can accept one
 *        of several types.
 *
 * The input for this tool can either be of type @c T1 ... @c T5.
 * The first one that yields a valid conversion is used, and the appropriate
 * @c get method is called.
 *
 * If the output type is also a @c Types construct, then the corresponding
 * type within that is used as the output type.
 *
 * Note that this is all done at configuration time: this doesn't allow the
 * type to change dynamically.  Rather, this gives a tool that can be
 * configured with more than one type.
 *
 * This specialization is for the 1-argument case.
 * Further specializations for more arguments are defined using the facilities
 * of boost.preprocessor.
 */
template <class T0, class TO_T>
class SingleAssociationTool<Types<T0>, TO_T>
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
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c fillUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti) override;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   * Should be of the type given by @c typeinfo.
   */
  virtual const void* getUntyped (const void* p) override;


  /**
   * @brief Return the @c std::type_info for the source of the association.
   */
  virtual const std::type_info& fromTypeinfo() const override;


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const typename SelectType<TO_T, 0>::type* get (const T0& p) = 0;


  /**
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const override;


  using SingleAssociationToolTo<TO_T>::releaseObject;

  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c get().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObject (const typename SelectType<TO_T, 0>::type* /*p*/)
    override;


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c getUntyped().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObjectUntyped (const void* p) override;


protected:
  /// Index of which type we're expecting.
  size_t m_which;


  /**
   * @brief Helper to decide which which @c get method to call.
   *
   * This either calls @c get using the last type
   * in our template argument list, or chains to the base class.
   *
   * It's virtual so that we can call the most-derived one from
   * @c getUntyped in the base class, but we also declare it as inline
   * so that the base class chaining can be inlined.
   */
  virtual const void* doGetUntyped (const void* p);


  /**
   * @brief Helper to collect the list of @c type_info's that we accept.
   *
   * This first chains to the base class to pick up its list.
   * Then we add the @c type_info corresponding to the last type
   * in the template argument list.
   */
  virtual void push_ti (std::vector<const std::type_info*>& tis);


private:
  /// @c type_info for the selected type.
  const std::type_info* m_fromTypeinfo;
};


} // namespace D3PD


#include "D3PDMakerUtils/SingleAssociationToolMulti.icc"


// Use boost.preprocessor to generate specializations for
// the > 1 argument cases.
// The code is in the icc file, within BOOST_PP_IS_ITERATING.
#define BOOST_PP_ITERATION_LIMITS (1, D3PD_MAX_TYPE_CHOICES-1)
#define BOOST_PP_FILENAME_1  "D3PDMakerUtils/SingleAssociationToolMulti.icc"
#include BOOST_PP_ITERATE()


#endif // not D3PDMAKERUTILS_SINGLEASSOCIATIONTOOLMULTI_H
