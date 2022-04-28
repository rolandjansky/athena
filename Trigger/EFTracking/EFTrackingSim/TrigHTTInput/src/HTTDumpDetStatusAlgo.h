/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTDumpDetStatusAlgo_h
#define HTTDumpDetStatusAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include "HTTDetectorTool.h" 


/////////////////////////////////////////////////////////////////////////////
class HTTDumpDetStatusAlgo: public AthAlgorithm {
public:
  HTTDumpDetStatusAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTDumpDetStatusAlgo () override = default;
  virtual StatusCode initialize() override ;
  virtual StatusCode execute()    override;


private:
  ToolHandle<HTTDetectorTool> m_detectorTool   {this, "HTTDetectorTool", "HTTDetectorTool/HTTDetectorTool"}; 
  Gaudi::Property<bool> m_DumpBadModules       {this, "DumpBadModules", false, "If true enable dump of bad modules for HTT"};
  Gaudi::Property<bool> m_DumpModuleIDMap      {this, "DumpModuleIDMap", false, "If true dumps the map of the modules in each tower"};
  Gaudi::Property<bool> m_DumpGlobalToLocalMap {this, "DumpGlobalToLocalMap",false, "True if you want to produce the Global-to-Local map"};
  Gaudi::Property<bool> m_DumpIDMap            {this, "DumpIDMap", false};
  Gaudi::Property<bool> m_DumpModulePositions  {this, "DumpModulePositions",false,"To dump the corner positions of the modules"};

};

#endif // HTTDumpDetStatusAlgo_h
