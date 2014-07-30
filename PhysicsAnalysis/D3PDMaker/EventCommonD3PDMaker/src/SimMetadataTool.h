// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/SimMetadataTool.h
 * @author Thomas Gillam <thomas.gillam@cern.ch>
 * @date May, 2012
 * @brief Add simulation flavor metadata to D3PD.
 */

#ifndef EVENTCOMMOND3PDMAKER_SIMMETADATATOOL_H
#define EVENTCOMMOND3PDMAKER_SIMMETADATATOOL_H


#include "D3PDMakerInterfaces/IMetadataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/ToolHandle.h"


namespace D3PD {

/**
 * @brief Add luminosity metadata info to a D3PD.
 */
class SimMetadataTool
  : public AthAlgTool, public IMetadataTool
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SimMetadataTool (const std::string& type,
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
  ServiceHandle<StoreGateSvc> m_metaDataStore;
  /// Key for output metadata.
  std::string m_metakey;

  void getValueMap(const std::string &metadataPath, const std::vector<std::string> &inKeys, std::map<std::string, std::string> &outMap);

  void printMap(const std::vector<std::string> &intendedKeys, const std::map<std::string, std::string> &arg);

  StatusCode addMapToD3PD(ID3PD *d3pd, const std::vector<std::string> &intendedKeys, const std::map<std::string, std::string> &arg);
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_SIMMETADATATOOL_H
