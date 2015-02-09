/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGINDETROADMAKERTOOL_H__
#define __TRIGINDETROADMAKERTOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetToolInterfaces/ITrigL2LayerNumberTool.h"
#include "TrigInDetToolInterfaces/ITrigInDetRoadMakerTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class IRoiDescriptor;
class IRegSelSvc;

  
class TrigInDetRoadMakerTool: public AthAlgTool, virtual public ITrigInDetRoadMakerTool { 
  
 public: 

  TrigInDetRoadMakerTool(const std::string&, const std::string&, const IInterface*);
  virtual ~TrigInDetRoadMakerTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode makeRoads(std::vector<TrigInDetRoad>&, const IRoiDescriptor* );

 private:

  virtual StatusCode getRoadLayersRS( const IRoiDescriptor& roi, std::set<unsigned int>& l);
  virtual StatusCode getRoadLayersLUT(float etaMin, float etaMax, std::set<unsigned int>& l);

 private:

  // SERVICES
  ToolHandle<ITrigL2LayerNumberTool> m_numberingTool;
  ServiceHandle<IRegSelSvc> m_regionSelector;

  // PROPERTIES
  bool m_useRegSel;

  int m_layerOneDepth;
  int m_layerTwoDepth;

  float m_minPt;
  float m_maxD0;

  bool m_etaBin;
  bool m_phiBin;
  bool m_zedBin;
  float m_etaBinWidth;
  float m_phiBinWidth;
  float m_zedBinWidth;

};



#endif


 
