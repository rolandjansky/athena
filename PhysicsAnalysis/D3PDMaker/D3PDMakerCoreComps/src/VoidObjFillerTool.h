// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VoidObjFillerTool.h 618070 2014-09-22 19:05:34Z ssnyder $
/**
 * @file    D3PDMakerCoreComps/src/VoidObjFillerTool.h
 * @author  Haifeng Li <Haifeng.Li@cern.ch>
 * @date    Aug, 2009
 * @brief   Object filler tool for tools taking no input.
 */


#ifndef D3PDMAKERCORECOMPS_VOIDOBJFILLERTOOL_H
#define D3PDMAKERCORECOMPS_VOIDOBJFILLERTOOL_H


#include "D3PDMakerInterfaces/IBlockFillerTool.h"
#include "D3PDMakerInterfaces/IObjFillerTool.h"
#include "D3PDMakerUtils/AddVariable.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"


namespace D3PD {

// Forward declaration(s):
class ID3PD;

/**
 * @brief   Object filler tool for tools taking no input.
 *
 * This is an object filler tool for tools that take no input.
 * It has the following properties:
 *
 *   BlockFillers - List of IBlockFillerTool instances.
 *   Prefix       - Prefix to add to variable names for this block.
 *
 * For each event, we loop over all block filler tools.
 */
class VoidObjFillerTool
  : public extends2<AthAlgTool, IObjFillerTool, IIncidentListener>,
    public AddVariable
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  VoidObjFillerTool (const std::string& type,
		     const std::string& name,
		     const IInterface* parent
		     );


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
  /// Property: The list of block filler tools.
  ToolHandleArray<IBlockFillerTool> m_blockFillers;

  /// Property: Variable prefix for this block.
  std::string m_prefix;

  /// Property: the name of this block.
  std::string m_blockName;

  /// Property: the name of the D3PDObject that created this tool
  std::string m_objectName;

  /// Property: If true, the metadata about the variables is saved
  bool m_saveMetadata;

  /// Pointer to the ID3PD object used
  ID3PD* m_tree;
};


} // namespace D3PD



#endif // not D3PDMAKERCORECOMPS_VOIDOBJFILLERTOOL_H
