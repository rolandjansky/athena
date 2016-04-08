// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/MultiAssociationToolMulti.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief A specialization of MultiAssociationTool that can accept one
 *        of several types.
 */


#ifndef D3PDMAKERUTILS_MULTIASSOCIATIONTOOLMULTI_H
#define D3PDMAKERUTILS_MULTIASSOCIATIONTOOLMULTI_H


#include "D3PDMakerUtils/Types.h"
#include "D3PDMakerUtils/MultiAssociationToolImpl.h"
#ifndef D3PDMAKERUTILS_MULTIASSOCIATIONTOOL_H // prevent recursive include
#include "D3PDMakerUtils/MultiAssociationTool.h"
#endif

#include "boost/preprocessor/iteration/iterate.hpp"
#include <cstdlib>



namespace D3PD {


/**
 * @brief A specialization of MultiAssociationTool that can accept one
 *        of several types.
 *
 * The input for this tool can either be of type @c T1 ... @c T5.
 * The first one that yields a valid conversion is used, and the appropriate
 * @c get method is called.
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
class MultiAssociationTool<Types<T0>, TO_T>
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
   * @brief Return the @c std::type_info for the source of the association.
   */
  virtual const std::type_info& fromTypeinfo() const override;


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode resetUntyped (const void* p) override;


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const T0& p) = 0;


protected:
  /// Index of which type we're expecting.
  size_t m_which;


  /**
   * @brief Helper to decide which which @c reset method to call.
   *
   * This either calls @c reset using the last type
   * in our template argument list, or chains to the base class.
   *
   * It's virtual so that we can call the most-derived one from
   * @c resetUntyped in the base class, but we also declare it as inline
   * so that the base class chaining can be inlined.
   */
  virtual StatusCode doResetUntyped (const void* p);


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


/**
 * @brief A specialization of MultiAssociationTool that can accept one
 *        of several types, mapping to multiple output types.
 *
 * This specialization is for the 1-argument case.
 * Further specializations for more arguments are defined using the facilities
 * of boost.preprocessor.
 */
template <class T0, class U0>
class MultiAssociationTool<Types<T0>, Types<U0> >
  : public MultiAssociationTool<Types<T0>, U0>
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
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const T0& p) override = 0;


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * This overload is not used for the case where we can return multiple
   * output types.  In this case, @c next takes a dummy pointer to the
   * input type in order to fix the type.
   */
  virtual const U0* next () override;


  /**
   * @brief Return a pointer to the next element in the association.
   * @param dummy Dummy argument to fix the type.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const U0* next (const T0* dummy) = 0;


  /**
   * @brief Return the element type for the target of the association.
   *
   * I.e., @c nextUntyped returns a pointer to this type.
   */
  virtual const std::type_info& elementTypeinfo() const override;


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c next().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseElement (const U0* /*p*/) override;


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c nextUntyped().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseElementUntyped (const void* p) override;


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const void* nextUntyped() override;
};


} // namespace D3PD


#include "D3PDMakerUtils/MultiAssociationToolMulti.icc"


// Use boost.preprocessor to generate specializations for
// the > 1 argument cases.
// The code is in the icc file, within BOOST_PP_IS_ITERATING.
#define BOOST_PP_ITERATION_LIMITS (1, D3PD_MAX_TYPE_CHOICES-1)
#define BOOST_PP_FILENAME_1  "D3PDMakerUtils/MultiAssociationToolMulti.icc"
#include BOOST_PP_ITERATE()


#endif // not D3PDMAKERUTILS_MULTIASSOCIATIONTOOLMULTI_H
