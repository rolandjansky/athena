// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/BlockFillerToolMulti.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2011
 * @brief A specialization of BlockFillerTool that can accept one
 *        of several types.
 */


#ifndef D3PDMAKERUTILS_BLOCKFILLERTOOLMULTI_H
#define D3PDMAKERUTILS_BLOCKFILLERTOOLMULTI_H


#include "D3PDMakerUtils/Types.h"
#include "D3PDMakerUtils/BlockFillerToolImpl.h"
#ifndef D3PDMAKERUTILS_BLOCKFILLERTOOL_H // prevent recursive include
#include "D3PDMakerUtils/BlockFillerTool.h"
#endif

#include "boost/preprocessor/iteration/iterate.hpp"
#include <cstdlib>


namespace D3PD {


/**
 * @brief A specialization of BlockFillerTool that can accept one
 *        of several types.
 *
 * The input for this tool can either be of type @c T1 ... @c T5.
 * The first one that yields a valid conversion is used, and the appropriate
 * @c fill method is called.
 *
 * Note that this is all done at configuration time: this doesn't allow the
 * type to change dynamically.  Rather, this gives a tool that can be
 * configured with more than one type.
 *
 * This specialization is for the 1-argument case.
 * Further specializations for more arguments are defined using the facilities
 * of boost.preprocessor.
 */
template <class T0>
class BlockFillerTool<Types<T0> >
  : public BlockFillerToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  BlockFillerTool (const std::string& type,
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
                                    const std::type_info& ti);


  /**
   * @brief Declare tuple variables.
   *
   * This is called at the start of the first event.
   */
  virtual StatusCode book() = 0;


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const T0& /*p*/) = 0;


  /**
   * @brief Fill one block, after @c AGAIN has been returned (type-safe).
   * @param p The input object.
   *
   * Once @c fill returns @c AGAIN, the parent should
   * call @c fillAgain with the same arguments.
   * This continues until @c fillAgain returns something
   * other than @c AGAIN.
   *
   * By default, this just calls @c fill().
   */
  virtual StatusCode fillAgain (const T0& p) { return this->fill(p); }


  /**
   * @brief Fill one block.
   * @param p The input object.
   * @param again Set if this is a subsequent call requested by an AGAIN return
   *
   * This is called once per object.  The type of the object at which @c p
   * points is given by the @c ti argument to @c configureD3PD.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   *
   * If the return status is the special code @c AGAIN (defined above),
   * then this filler tool wants to make multiple entries.
   * The parent should set up to capture a new `row' and run
   * through the list of block filler
   * tools again, but for this tool call @c fillAgainUntyped
   * instead of @c fillUntyped.  This should be repeated as long
   * as @c fillAgainUntyped returns @c AGAIN.
   *
   * Once @c fillUntyped returns @c AGAIN, the parent should
   * call @c fillUntyped with the same @a p argument and @c again
   * set to @c true.  This continues until @c fillUntyped returns something
   * other than @c AGAIN.
   *
   * Not all parents may support this.  In that case, returning
   * @c AGAIN will be treated as an error.
   */
  virtual StatusCode fillUntyped (const void* p, bool again = false);


protected:
  /// Index of which type we're expecting.
  size_t m_which;


  /**
   * @brief Helper to decide which which @c fill method to call.
   *
   * This either calls @c fill / @c fillAgain using the last type
   * in our template argument list, or chains to the base class.
   *
   * It's virtual so that we can call the most-derived one from
   * @c fillUntyped in the base class, but we also declare it as inline
   * so that the base class chaining can be inlined.
   */
  virtual StatusCode doFillUntyped (const void* p, bool again);


  /**
   * @brief Helper to collect the list of @c type_info's that we accept.
   *
   * This first chains to the base class to pick up its list.
   * Then we add the @c type_info corresponding to the last type
   * in the template argument list.
   */
  virtual void push_ti (std::vector<const std::type_info*>& tis);
};


} // namespace D3PD


#include "D3PDMakerUtils/BlockFillerToolMulti.icc"


// Use boost.preprocessor to generate specializations for
// the > 1 argument cases.
// The code is in the icc file, within BOOST_PP_IS_ITERATING.
#define BOOST_PP_ITERATION_LIMITS (1, D3PD_MAX_TYPE_CHOICES-1)
#define BOOST_PP_FILENAME_1  "D3PDMakerUtils/BlockFillerToolMulti.icc"
#include BOOST_PP_ITERATE()


#endif // not D3PDMAKERUTILS_BLOCKFILLERTOOLMULTI_H
