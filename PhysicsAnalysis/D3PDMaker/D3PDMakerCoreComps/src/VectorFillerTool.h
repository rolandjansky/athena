// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VectorFillerTool.h 618070 2014-09-22 19:05:34Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/VectorFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Object filler tool for a collection of objects, saved as vectors.
 */


#ifndef D3PDMAKERCORECOMPS_VECTORFILLERTOOL_H
#define D3PDMAKERCORECOMPS_VECTORFILLERTOOL_H


#include "D3PDMakerUtils/VectorFillerToolBase.h"
#include "D3PDMakerInterfaces/IObjFillerTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"


namespace D3PD {

// Forward declaration(s):
class ID3PD;

/**
 * @brief Object filler tool for a collection of objects, saved as vectors.
 *
 * This is an object filler tool for a collection of objects.
 * The object variables are saved as vectors.
 * I.e., if a block books a variable of type `int', the variable
 * we actually create is of type `vector<int>'.
 * An additional variable giving the object count is added automatically
 * (can be controlled via the @c NrowName property).
 *
 * This tool has the following properties:
 *
 *   Getter       - ICollectionGetterTool instance.
 *   AllowMissing - If true, then it is not considered an error
 *                  for the requested input object to be missing.
 *
 * and the following inherited from @c VectorFillerToolBase:
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
 *
 * For each event, we call the getter to retrieve the collection.
 * We then loop over the objects in the collection.
 * For each object, we set the variable pointers to point
 * to the proper locations within the vectors.
 * Then we loop over all block filler tools, passing the object
 * to each.
 */
class VectorFillerTool
  : public extends2<VectorFillerToolBase, IObjFillerTool, IIncidentListener>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  VectorFillerTool (const std::string& type,
                    const std::string& name,
                    const IInterface* parent);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Configure the tool.
   * @param tree The parent D3PD tree.
   *
   * This is called during initialization.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree);


  /**
   * @brief Declare tuple variables.
   *
   * This is called at the start of the first event.
   */
  virtual StatusCode book();


  /**
   * @brief Fill one object.
   *
   * This is called once per event.  It should:
   *   - Fetch the input.
   *   - If it's a container, loop over the contents.
   *   - Loop over the contained block filler tools.
   */ 
  virtual StatusCode fill();


  // addVariable() inherited from base.


  /**
   * @brief Find out if the filler handles containers or single objects
   *
   * This is used by the D3PDReader code generator to know how to
   * create the generated source files exactly.
   */
  virtual bool isContainerFiller() const;


  /**
   * @brief Handle end of run incidents to save the metadata at that point
   */
  virtual void handle( const Incident& inc );


private:
  /// Property: The collection getter tool.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: the name of the D3PDObject that created this tool
  std::string m_objectName;

  /// Property: If true, the input object is allowed to be missing.
  bool m_allowMissing;

  /// Property: If true, the metadata about the variables is saved
  bool m_saveMetadata;

  /// Pointer to the ID3PD object used
  ID3PD* m_tree;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_VECTORFILLERTOOL_H
