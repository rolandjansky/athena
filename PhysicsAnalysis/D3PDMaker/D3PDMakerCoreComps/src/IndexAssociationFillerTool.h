// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IndexAssociationFillerTool.h 618070 2014-09-22 19:05:34Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/IndexAssociationFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Represent a single association by index.
 */


#ifndef D3PDMAKERCORECOMPS_INDEXASSOCIATIONTOOL_H
#define D3PDMAKERCORECOMPS_INDEXASSOCIATIONTOOL_H


#include "D3PDMakerInterfaces/IBlockFillerTool.h"
#include "D3PDMakerInterfaces/ISingleAssociationTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "D3PDMakerUtils/AddVariable.h"
#include "D3PDMakerUtils/TypeConverter.h"
#include "D3PDMakerUtils/IndexMap.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Represent a single association by index.
 *
 * This is a block filler tool which represents a single association by
 * index.
 *
 * It takes as a property an @c ISingleAssociationTool.
 * For each input object, it asks the association tool which object
 * is associated with it.  It also takes as a property a collection getter
 * label.  This defines an `index' collection.
 * The tool searches this collection
 * for the target of the association and finds the index of that
 * object within that collection.  That index is then booked
 * as a variable.
 *
 * The index is set to -1 if the association fails or if the object
 * isn't found in the index container.
 *
 * Properties:
 *   Associator   - The ISingleAssociationTool instance.
 *   Target       - Label of the collection getter defining the collection
 *                  within which to index.
 *   Prefix       - Variable name prefix for the contained blocks.
 *   CollectionGetterRegistry - The ICollectionGetterRegistryTool instance.
 */
class IndexAssociationFillerTool
  : public extends1<AthAlgTool, IBlockFillerTool>, public AddVariable
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  IndexAssociationFillerTool (const std::string& type,
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
  ToolHandle<ISingleAssociationTool> m_associator;

  /// Property: The label of the  collection getter defining the collection
  /// within which to index.
  std::string m_targetLabel;

  /// Property: The variable name prefix.
  std::string m_prefix;

  /// Property: the name of this block.
  std::string m_blockName;

  /// Property: The ICollectionGetterRegistryTool instance.
  ToolHandle<ICollectionGetterRegistryTool> m_registry;

  /// Property: Don't give an error if the target getter fails to find
  /// the input objects.
  bool m_allowMissing;

  /// Index variable.
  int* m_index;

  /// Helper to convert from pointer returned by the associator
  /// to pointer into the index container.
  TypeConverter m_converter;

  /// Map from objects to indices.
  IndexMap m_map;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_INDEXASSOCIATIONTOOL_H

