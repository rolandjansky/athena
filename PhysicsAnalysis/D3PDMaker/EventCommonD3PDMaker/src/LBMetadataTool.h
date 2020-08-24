// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/LBMetadataTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Add luminosity metadata info to a D3PD.
 */

#ifndef EVENTCOMMOND3PDMAKER_LBMETADATATOOL_H
#define EVENTCOMMOND3PDMAKER_LBMETADATATOOL_H


#include "D3PDMakerInterfaces/IMetadataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief Add luminosity metadata info to a D3PD.
 */
class LBMetadataTool
  : public AthAlgTool, public IMetadataTool
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  LBMetadataTool (const std::string& type,
                  const std::string& name,
                  const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c queryInterface method.
  virtual StatusCode queryInterface( const InterfaceID& riid,
                                     void** ppvIf );


  /**
   * @brief Write metadata to a D3PD.
   * @param d3pd The D3PD to which to write.
   */
  virtual StatusCode writeMetadata (ID3PD* d3pd);


private:
  /// Key for output metadata.
  std::string m_metakey;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_LBMETADATATOOL_H
