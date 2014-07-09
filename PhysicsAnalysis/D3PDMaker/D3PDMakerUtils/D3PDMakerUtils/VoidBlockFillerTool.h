// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file      D3PDMakerUtils/VoidBlockFillerTool.h
 * @author    Haifeng Li <Haifeng.Li@cern.ch>
 * @date      Aug, 2009
 * @brief     Type-safe wrapper for block filler tools,
 *            for tools taking no input.
 */


#ifndef D3PDMAKERUTILS_VOIDBLOCKFILLERTOOL_H
#define D3PDMAKERUTILS_VOIDBLOCKFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerToolImpl.h"
#ifndef D3PDMAKERUTILS_BLOCKFILLERTOOL_H // prevent recursive include
#include "D3PDMakerUtils/BlockFillerTool.h"
#endif

namespace D3PD {


/**
 * @brief     Type-safe wrapper for block filler tools,
 *            for tools taking no input.
 *
 * This is specialization of @c BlockFillerTool for the case where the
 * tool gets no input.  (An example is filling global trigger decisions.)
 *
 * There are two virtual functions that derived classes need to define:
 *
 *@code
 *   virtual StatusCode book () = 0;
 *   virtual StatusCode fill () = 0;
 @endcode
 *
 * @c book() is called at the start of the first event; it should perform
 * the variable registration.  @c fill() is then called for each object
 * to be processed.
 *
 * A derived class may also define
 *
 *@code
 *   virtual StatusCode fillAgain ();
 @endcode
 *
 * if it wants to use @c AGAIN status returns
 * (see @c IBlockFillerTool).
 */
template <>
class BlockFillerTool <void>
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
  virtual StatusCode fillUntyped (const void* p,
                                  bool again /*= false*/);


  /**
   * @brief Fill one block --- type-safe version.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill () = 0;


  /**
   * @brief Fill one block, after @c AGAIN has been returned (type-safe).
   *
   * Once @c fill returns @c AGAIN, the parent should
   * call @c fillAgain with the same arguments.
   * This continues until @c fillAgain returns something
   * other than @c AGAIN.
   *
   * By default, this just calls @c fill().
   */
  virtual StatusCode fillAgain ();
};


} // namespace D3PD


#endif // not D3PDMAKERUTILS_VOIDBLOCKFILLERTOOL_H
