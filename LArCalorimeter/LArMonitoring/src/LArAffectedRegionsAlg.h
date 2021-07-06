/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArAffectedRegionsAlg
 * @author Margherita Spalla <mspalla@cern.ch> (updated from LArAffectedRegions by Jessica Leveque <jleveque@in2p3.fr>)
 *
 */

#ifndef LARAFFECTEDREGIONSALG_H
#define LARAFFECTEDREGIONSALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"

#include <string>




class LArAffectedRegionsAlg: public AthMonitorAlgorithm
{
 public:
  LArAffectedRegionsAlg(const std::string& name,ISvcLocator* pSvcLocator );		      

  /** @brief Default destructor */
  virtual ~LArAffectedRegionsAlg();

  /** @brief Overwrite dummy method from AlgTool */
  virtual StatusCode initialize() override;


  /** Called each event */
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:

  //affected regions handle
  SG::ReadCondHandleKey<CaloAffectedRegionInfoVec> m_affKey {this,"LArAffectedRegionKey", "LArAffectedRegionInfo", "SG key for affected regions cond object"};

  // Properties
  //MonGroup(s) name
  Gaudi::Property<std::string> m_MonGroupName {this,"AffectedRegionsGroupName","LArAffectedRegionsMonGroup"};
  Gaudi::Property<std::string> m_embpsName {this,"EMBPSname","EMBPS"};
  Gaudi::Property<std::string> m_embName {this,"EMBname","EMB"};
  Gaudi::Property<std::string> m_emecpsName {this,"EMECPSname","EMECPS"};
  Gaudi::Property<std::string> m_emecName {this,"EMECname","EMEC"};
  Gaudi::Property<std::string> m_hec0Name {this,"HEC0name","HEC0"};
  Gaudi::Property<std::string> m_hec1Name {this,"HEC1name","HEC1"};
  Gaudi::Property<std::string> m_hec2Name {this,"HEC2name","HEC2"};
  Gaudi::Property<std::string> m_hec3Name {this,"HEC3name","HEC3"};
  Gaudi::Property<std::string> m_fcal0Name {this,"FCal0name","FCAL0"};
  Gaudi::Property<std::string> m_fcal1Name {this,"FCal1name","FCAL1"};
  Gaudi::Property<std::string> m_fcal2Name {this,"FCal2name","FCAL2"};

  //Other properties
  Gaudi::Property<bool> m_IsOnline {this,"IsOnline",false};
  Gaudi::Property<float> m_delta_phi {this,"PhiGranularity",0.01};
  Gaudi::Property<float> m_delta_eta {this,"EtaGranularity",0.01};

};

#endif

