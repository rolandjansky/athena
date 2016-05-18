/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
    @class CaloAffectedTool
    @brief Tool to check if object is in an affected region
    @author G.Unal
*/

#ifndef CALOTOOLS_CaloAffectedTool_H
#define CALOTOOLS_CaloAffectedTool_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "CaloInterface/ICaloAffectedTool.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "AthenaKernel/IOVSvcDefs.h"

namespace xAOD {
  class IParticle;
}

class StoreGateSvc;

class CaloAffectedTool: public AthAlgTool,
	             virtual public ICaloAffectedTool, virtual public IIncidentListener
{
private: 
  
  CaloAffectedRegionInfoVec* m_affectedRegions;

  StatusCode updateAffectedRegionsFromDB(IOVSVC_CALLBACK_ARGS);

  StatusCode readDB();

    void handle(const Incident& inc);

  bool m_read;

  bool m_readRaw;
  
public:    
  
  CaloAffectedTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 
  virtual ~CaloAffectedTool();  

  virtual bool isAffected(const xAOD::IParticle *p, float deta=0., float dphi=0., int layer_min=0, int layer_max=-1, int problemType=-1);

  virtual bool listAffected(const xAOD::IParticle* p, std::vector<int>& layer_list, std::vector<int>& problem_list, float deta=0, float dphi=0, int problemType=-1);

  StatusCode initialize();


};

#endif
