// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerInterfaces/IBlockFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Abstract interface for tool to fill a block in the D3PD.
 */


#ifndef D3PDMAKERINTERFACES_IBLOCKFILLERTOOL_H
#define D3PDMAKERINTERFACES_IBLOCKFILLERTOOL_H


#include "GaudiKernel/IAlgTool.h"
#include <typeinfo>


namespace D3PD {


class IAddVariable;


/**
 * @brief Abstract interface for tool to fill a block in the D3PD.
 *
 * Each object consists of a set of @e blocks.  A block is a small set of
 * related tuple variables for some object.  Blocks represent the
 * intended granularity for enabling or disabling variables (though it
 * should be possible to control individual variables through the remapping
 * mechanism).  Blocks are filled by tools of type @c IBlockFillerTool.
 * Blocks (and the associated tools) will generally be specific for
 * a particular object type (i.e., fill the egamma PID variables)
 * A few, however, will be more generic: for example, there should be
 * one block tool for filling 4-vector information that's used by all
 * object types (that implement 4-vectors).
 *
 * Each block is associated with a specific `level of detail' in job
 * configuration.  This allows enabling a preselected set of blocks.
 *
 * Each block has a default variable name for each leaf filled.
 * This is concatenated with the object prefix (if any) to produce
 * the full name for the leaf.  It will be possible to configure
 * the filler tools to change the names of the tuple leaves
 * (this should be implemented with generic code in a base class).
 * The same mechanism may be used to disable specific leaves
 * entirely by remapping to a null name.
 * (Remapping is not yet implemented.)
 *
 * User-written code will generally not implement this interface directly,
 * but will instead use the type-safe wrappers provided by @c BlockFillerTool.
 */
class IBlockFillerTool
  : virtual public IAlgTool
{
public:
  /// Gaudi interface definition.
  DeclareInterfaceID(IBlockFillerTool, 1, 0);


  /// Special status code to indicate that we want @c fillAgainUntyped
  /// to be called; see below.
  enum { AGAIN = 1001,
         EMPTY = 1002
  };


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c fillUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti) = 0;


  /**
   * @brief Declare tuple variables.
   *
   * This is called at the start of the first event.
   */
  virtual StatusCode book() = 0;


  /**
   * @brief Fill one block.
   * @param p The input object.
   * @param again Set if this is a subsequent call requested by an AGAIN return
   *
   * This is called once per object.  The type of the object at which @c p
   * points is given by the @c ti argument to @c configure.  The caller
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
   *
   * The return code may also be EMPTY if this tool did not fill in anything.
   * If all tools return empty, the parent may omit the row.
   */
  virtual StatusCode fillUntyped (const void* p,
                                  bool again = false) = 0;


  /**
   * @brief StatusCode check helper: change @c AGAIN or @c EMPTY to @c SUCCESS.
   * @param sc The StatusCode to check.
   *
   * If @a sc is @c AGAIN or @c EMPTY, then return @c SUCCESS.
   * Otherwise, return @a sc.
   */
  static StatusCode againok (StatusCode sc);


  /**
   * @brief StatusCode check helper: change @c AGAIN or @c EMPTY to @c SUCCESS.
   * @param sc The StatusCode to check.
   * @param scout[out] Accumulated status code.
   *
   * If @a sc is @c AGAIN or @c EMPTY, then return @c SUCCESS.
   * Otherwise, return @a sc.
   *
   * If @a sc is @c AGAIN, then @a scout will be set to @c AGAIN.
   * Otherwise, if @a sc is not @c EMPTY, then @c scout will be set to @c sc.
   */
  static StatusCode againok (StatusCode sc, StatusCode& scout);
};


} // namespace D3PD


#include "D3PDMakerInterfaces/IBlockFillerTool.icc"


#endif // not D3PDMAKERINTERFACES_IBLOCKFILLERTOOL_H

