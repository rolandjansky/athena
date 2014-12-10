// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainedMultiAssociationFillerTool.h 618070 2014-09-22 19:05:34Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/ContainedMultiAssociationFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Represent a multiple association by containment.
 */


#ifndef D3PDMAKERCORECOMPS_CONTAINEDMULTIASSOCIATION_H
#define D3PDMAKERCORECOMPS_CONTAINEDMULTIASSOCIATION_H


#include "D3PDMakerInterfaces/IBlockFillerTool.h"
#include "D3PDMakerInterfaces/IMultiAssociationTool.h"
#include "D3PDMakerUtils/AddVariable.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace  D3PD {


/**
 * @brief Represent a multiple association by containment.
 *
 * This is a block filler tool which represents a multiple association by
 * containment.  It uses the AGAIN feature of a block filler tool.
 * The effect is that for each source object, there will be multiple
 * entries made, one for each target object.  Other variables
 * will generally just be copied.
 *
 * It takes as a property an @c IMultiAssociationTool.
 * For each input object, it asks the association tool which objects
 * are associated with it; the result is a collection of 0 or more
 * objects.  This tool also has a list of block filler
 * tools; these tools are called for the result of the association.
 *
 * The tool in addition creates a counter variable to count the
 * of target objects for each source object.  If this is -1,
 * then the target object variables are not filled in.
 *
 * Properties:
 *   Associator   - The ISingleAssociationTool.
 *   BlockFillers - List of contained block filler tools.
 *   Prefix       - Variable name prefix for the contained blocks.
 *   Counter      - Variable name to use for the target object counter.
 *                  Omitted if empty.
 */
class ContainedMultiAssociationFillerTool
  : public extends1<AthAlgTool, IBlockFillerTool>, public AddVariable
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ContainedMultiAssociationFillerTool (const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c finalize method.
  virtual StatusCode finalize();


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
  virtual StatusCode book();


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
   *
   * The return code may also be EMPTY if this tool did not fill in anything.
   * If all tools return empty, the parent may omit the row.
   */
  virtual StatusCode fillUntyped (const void* p,
                                  bool again = false);



private:
  /// Property: The association tool instance.
  ToolHandle<IMultiAssociationTool> m_associator;

  /// Property: The list of block filler tools.
  ToolHandleArray<IBlockFillerTool> m_blockFillers;

  /// Property: The variable name prefix.
  std::string m_prefix;

  /// Property: the name of this block.
  std::string m_blockName;

  /// Next object to process.
  const void* m_next;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_CONTAINEDMULTIASSOCIATIONFILLERTOOL_H

