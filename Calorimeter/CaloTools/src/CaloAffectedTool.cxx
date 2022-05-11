/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTools/CaloAffectedTool.h" 
#include "xAODBase/IParticle.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CaloGeoHelpers/CaloPhiRange.h"


CaloAffectedTool::CaloAffectedTool (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
    AthAlgTool(type, name, parent)
{ 
  declareInterface<ICaloAffectedTool>(this);
}
                                                                                
//-----------------------------------------------------------------

CaloAffectedTool::~CaloAffectedTool() = default;


//-------------------------------------------------------------------

StatusCode CaloAffectedTool::initialize() {
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------


bool CaloAffectedTool::isAffected(const xAOD::IParticle *p, const CaloAffectedRegionInfoVec *vAff, float deta, float dphi, int layer_min, int layer_max, int problemType) const
{

   if(!vAff) return false;

 static const float epsilon=1e-6;

  //std::cout << " in isAffected " << p->eta() << " " << p->phi() << std::endl;

  std::vector<CaloAffectedRegionInfo>::const_iterator reg1 = vAff->begin();
  std::vector<CaloAffectedRegionInfo>::const_iterator reg2 = vAff->end();
  for (;reg1 != reg2; ++reg1) {
    const CaloAffectedRegionInfo* region = &(*reg1);

    int problem=region->get_problem();
    //std::cout << " problem,problemType " << problem << " " << problemType << std::endl;
    if (problemType>=0 && (problem != problemType)) continue;

    int layermin=region->get_layer_min();
    int layermax=region->get_layer_max();
    //std::cout << " layermin, layermax " << layermin << " " << layermax << std::endl;
    if ((layer_max>=layer_min) && (layermax < layer_min || layermin > layer_max)) continue;


    float etamin=region->get_eta_min();
    float etamax=region->get_eta_max();
    float eta = p->eta();
    //std::cout << " eta region " << etamin << " " << etamax << std::endl;
    if ((eta+deta)<etamin || (eta-deta)>etamax) continue;

    float phimin=region->get_phi_min();
    float phimax=region->get_phi_max();
    float phi = p->phi();
    //std::cout << " phi region " << phimin << " " << phimax << std::endl;
    float phi2 = CaloPhiRange::fix(phi+dphi+epsilon);
    float phi1 = CaloPhiRange::fix(phi-dphi-epsilon);

    if ((phimax >= phimin) && (phi2 >= phi1) && (phi2<phimin || phi1>phimax)) continue;
    if ((phimax >= phimin) && (phi2 <= phi1) && (phi1>phimax && phi2<phimin)) continue;
    if ((phimax <= phimin) && (phi2 >= phi1) && (phi1>phimax && phi2<phimin)) continue;

    //std::cout << " in region " << std::endl;

    return true;

  }

  return false;

}
//-------------------------------------------------

bool CaloAffectedTool::listAffected(const xAOD::IParticle*p, const CaloAffectedRegionInfoVec *vAff, std::vector<int>& layer_list, std::vector<int>& problem_list, float deta, float dphi, int problemType) const
{

  if(!vAff) return false;

  bool found = false;

  static const float epsilon=1e-6;

  layer_list.clear();
  problem_list.clear();


  std::vector<CaloAffectedRegionInfo>::const_iterator reg1 = vAff->begin();
  std::vector<CaloAffectedRegionInfo>::const_iterator reg2 = vAff->end();
  for (;reg1 != reg2; ++reg1) {
    const CaloAffectedRegionInfo* region = &(*reg1);

    int problem=region->get_problem();
    if (problemType>=0 && (problem != problemType)) continue;

    int layermin=region->get_layer_min();
    int layermax=region->get_layer_max();

    float etamin=region->get_eta_min();
    float etamax=region->get_eta_max();
    float eta = p->eta();
    if ((eta+deta)<etamin || (eta-deta)>etamax) continue;

    float phimin=region->get_phi_min();
    float phimax=region->get_phi_max();
    float phi = p->phi();
    float phi2 = CaloPhiRange::fix(phi+dphi+epsilon);
    float phi1 = CaloPhiRange::fix(phi-dphi-epsilon);

    if ((phimax >= phimin) && (phi2 >= phi1) && (phi2<phimin || phi1>phimax)) continue;
    if ((phimax >= phimin) && (phi2 <= phi1) && (phi1>phimax && phi2<phimin)) continue;
    if ((phimax <= phimin) && (phi2 >= phi1) && (phi1>phimax && phi2<phimin)) continue;

    found = true;

    for (int ilayer=layermin;ilayer<=layermax;ilayer++) {
        layer_list.push_back(ilayer);
        problem_list.push_back(problem);
    }


  }

  return found;

}
