// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainedVectorMultiAssociationFillerTool.h 618070 2014-09-22 19:05:34Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/ContainedVectorMultiAssociationFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Represent a multiple association by containment inside vectors.
 */


#ifndef D3PDMAKERCORECOMPS_CONTAINEDVECTORMULTIASSOCIATION_H
#define D3PDMAKERCORECOMPS_CONTAINEDVECTORMULTIASSOCIATION_H


#include "D3PDMakerUtils/VectorFillerToolBase.h"
#include "D3PDMakerInterfaces/IMultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace  D3PD {


/**
 * @brief Represent a multiple association by containment inside vectors.
 *
 * This is a block filler tool which represents a multiple association by
 * containment; the variables in the contained blocks will appear
 * inside of vectors.  (This implies that if this block itself is
 * used with, say, @c VectorFillerTool, then we'll end up with nested vectors.)
 *
 * It takes as a property an @c IMultiAssociationTool.
 * For each input object, it asks the association tool which objects
 * are associated with it; the result is a collection of 0 or more
 * objects.  This tool also has a list of block filler
 * tools; these tools are called for the result of the association.
 *
 * Properties:
 *
 *   Associator   - The @c IMultiAssociationTool.
 *
 * Properties inherited from @c VectorFillerToolBase:
 *
 *   BlockFillers - List of IBlockFillerTool instances.
 *   Prefix       - Prefix to add to variable names for this block.
 *   NrowName     - Name of the variable for the count of rows.
 *                  Omitted if empty.
 *                  Default: `n'.
 *   NobjName     - Name of the variable for the count of objects.
 *                  (May be less than the number of rows if AGAIN is used.)
 *                  Omitted if empty (default).
 *   ObjIndexName - Name of the variable for the object index.
 *                  Omitted if empty (default).
 */
class ContainedVectorMultiAssociationFillerTool
  : public extends1<VectorFillerToolBase, IBlockFillerTool>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  ContainedVectorMultiAssociationFillerTool (const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


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
   */
  virtual StatusCode fillUntyped (const void* p,
                                  bool again = false);



private:
  /// Property: The association tool instance.
  ToolHandle<IMultiAssociationTool> m_associator;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_CONTAINEDVECTORMULTIASSOCIATIONFILLERTOOL_H

