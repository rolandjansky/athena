/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTDETECTORTOOL_H
#define HTTDETECTORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"

#include <string>
#include <vector>
#include <list>
#include <set>
#include <fstream>

#include "StoreGate/DataHandle.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

class PixelID;
class SCT_ID;


/* This class interface the ID hits with the HTT simulation
    implemented in Athena. Original code */

class HTTDetectorTool : public ::AthAlgTool {
 public:

  HTTDetectorTool(const std::string&, const std::string&, const IInterface*);
  virtual ~HTTDetectorTool() = default;
  virtual StatusCode initialize() override;
  void dumpGlobalToLocalModuleMap();

private:
  ServiceHandle<ITrigHTTMappingSvc>   m_HTTMapping {this, "TrigHTTMappingSvc", "TrigHTTMappingSvc"};
  Gaudi::Property<std::string> m_global2local_path {this,"GlobalToLocalMapPath", "global-to-local-map.moduleidmap"};
  Gaudi::Property<std::string> m_sram_path_pix     {this, "SRAMPathPixel", "sram_lookup_pixel.txt"};
  Gaudi::Property<std::string> m_sram_path_sct     {this, "SRAMPathSCT", "sram_lookup_sct.txt"};
  Gaudi::Property<bool>        m_dumpAllModules    {this, "dumpAllModules", false};


  const InDetDD::SiDetectorManager*  m_PIX_mgr = nullptr;
  const PixelID*   m_pixelId = nullptr;
  const SCT_ID*    m_sctId = nullptr;

};

#endif // HTTDETECTORTOOL_H
