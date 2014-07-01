// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IndexFillerTool.h 486128 2012-03-02 08:54:20Z krasznaa $
/**
 * @file D3PDMakerCoreComps/src/IndexFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2010
 * @brief Fill an index of an object within a container.
 */


#ifndef D3PDMAKERCORECOMPS_INDEXFILLERTOOL_H
#define D3PDMAKERCORECOMPS_INDEXFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerToolImpl.h"
#include "D3PDMakerUtils/IndexMap.h"


namespace D3PD {


/**
 * @brief Fill an index of an object within a container.
 */
class IndexFillerTool
  : public BlockFillerToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  IndexFillerTool (const std::string& type,
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
  /// Property: The label of the collection getter defining the collection
  /// within which to index.
  std::string m_targetLabel;

  /// Property: The list of labels of collection getters defining the
  /// collections within which to index.  m_targetLabel is added to the
  /// start of this list if it's non-empty.
  std::vector<std::string> m_targetLabels;

  /// Variable name to use for the index of the container
  /// in which the object was found.  Useful if more than
  /// one target container is specified.  Omitted if this is empty.
  std::string m_containerIndexName;

  /// Property: The ICollectionGetterRegistryTool instance.
  ToolHandle<ICollectionGetterRegistryTool> m_registry;

  /// Property: Don't give an error if the target getter fails to find
  /// the input objects.
  bool m_allowMissing;

  /// Index variable.
  int* m_index;

  /// Container index variable.
  int* m_containerIndex;

  /// Map from objects to indices.
  IndexMap m_map;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_INDEXFILLERTOOL_H
