/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** 
    @class CaloAffectedTool
    @brief Tool to check if object is in an affected region
    @author G.Unal
    @ changed to use conditions object by P. Strizenec
*/

#ifndef CALOTOOLS_CaloAffectedTool_H
#define CALOTOOLS_CaloAffectedTool_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloAffectedTool.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "AthenaKernel/IOVSvcDefs.h"

namespace xAOD {
  class IParticle;
}

class CaloAffectedTool: public AthAlgTool,
	             virtual public ICaloAffectedTool
{
  
public:    
  
  CaloAffectedTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 
  virtual ~CaloAffectedTool();  

  virtual bool isAffected(const xAOD::IParticle *p, const CaloAffectedRegionInfoVec *vAff, float deta=0., float dphi=0., int layer_min=0, int layer_max=-1, int problemType=-1) const override;

  virtual bool listAffected(const xAOD::IParticle* p, const CaloAffectedRegionInfoVec *vAff, std::vector<int>& layer_list, std::vector<int>& problem_list, float deta=0, float dphi=0, int problemType=-1) const override;

  virtual StatusCode initialize() override;


};

#endif
