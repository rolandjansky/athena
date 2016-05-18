/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTools/CaloAffectedTool.h" 
//#include "EventKernel/I4Momentum.h"
#include "xAODBase/IParticle.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CaloGeoHelpers/CaloPhiRange.h"


CaloAffectedTool::CaloAffectedTool (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_read(false),
    m_readRaw(true)
{ 
  m_affectedRegions=0;
  declareInterface<ICaloAffectedTool>(this);
  declareProperty("readRaw",m_readRaw);
}
                                                                                
//-----------------------------------------------------------------

CaloAffectedTool::~CaloAffectedTool() {
  if (m_affectedRegions && !m_readRaw) delete m_affectedRegions;
}


//-------------------------------------------------------------------

StatusCode CaloAffectedTool::initialize() {

  MsgStream log( msgSvc(), name() );

  if (!m_readRaw) {
   if (detStore()->contains<CondAttrListCollection>("/LAR/LArAffectedRegionInfo")) {
      const DataHandle<CondAttrListCollection> affectedRegionH;
      if (detStore()->regFcn(&CaloAffectedTool::updateAffectedRegionsFromDB,
                          this,
                          affectedRegionH,
                          "/LAR/LArAffectedRegionInfo").isSuccess()) {
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "Registered callback for  LArAffectedRegion " << endreq; 
      }
        else {
             log << MSG::WARNING << "Cannot register callback for LArAffectedRegion " << endreq; 
      }
   }
    else {  
      log << MSG::WARNING << " no LArAffectedRegion information available from metadata " << endreq; 
   }
  }
  else {
    // register incident handler for begin event
    IIncidentSvc* incSvc; 
    if (service( "IncidentSvc", incSvc ).isFailure()) {
      log << MSG::ERROR << "Unable to get the IncidentSvc" << endreq; 
      return StatusCode::FAILURE;
    }
    long int priority=100;
    incSvc->addListener(this,"BeginEvent",priority);

  }

  m_read=false;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------

void CaloAffectedTool::handle(const Incident& inc) {

   MsgStream log(msgSvc(), name()); 

  if (inc.type()!="BeginEvent")
    return; 

  if (detStore()->contains<CaloAffectedRegionInfoVec>("LArAffectedRegion")) {
     if (detStore()->retrieve(m_affectedRegions,"LArAffectedRegion").isFailure()) {
      log << MSG::WARNING << " cannot read LArAffectedRegion at begin of event " << endreq;
      return;
     }
     m_read=true;
     //std::cout << " got affected regions at beginning of event " << std::endl;
     return;
  }
  return;

}


//--------------------------------------------------

StatusCode CaloAffectedTool::updateAffectedRegionsFromDB(IOVSVC_CALLBACK_ARGS) {

  return this->readDB();
}

StatusCode  CaloAffectedTool::readDB() {

  MsgStream log(msgSvc(), name());   // Part 1: Get the messaging service, print where you are
  log << MSG::INFO << "updateAffectedRegionsFromDB()" << endreq;

  // retrieve from detStore
  const CondAttrListCollection* attrListColl = 0;
  StatusCode sc = detStore()->retrieve(attrListColl, "/LAR/LArAffectedRegionInfo");
  if (sc.isFailure()) {
     log << MSG::WARNING  << "attrrListColl not found for /LAR/CaloAffectedRegionInfo " << endreq;
     return StatusCode::SUCCESS;
  }

  if (m_affectedRegions) {
     m_affectedRegions->clear();
  }
  else  {
     m_affectedRegions =  new CaloAffectedRegionInfoVec();
  }

  // Loop over collection
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
      std::ostringstream attrStr1;
      (*first).second.toOutputStream( attrStr1 );
      log << MSG::DEBUG << "ChanNum " << (*first).first <<
          " Attribute list " << attrStr1.str() << endreq;
      //      const AttributeList& attrList = (*first).second;
      const coral::AttributeList& attrList = (*first).second;
      CaloAffectedRegionInfo info;
      float eta_min = attrList["eta_min"].data<float>();
      float eta_max = attrList["eta_max"].data<float>();
      float phi_min = attrList["phi_min"].data<float>();
      float phi_max = attrList["phi_max"].data<float>();
      int layer_min = attrList["layer_min"].data<int>();
      int layer_max = attrList["layer_max"].data<int>();
      //std::cout << " affected region found " << eta_min << " " << eta_max << " " << phi_min << " " << phi_max << " " << layer_min << " " << layer_max << " " << attrList["problem"].data<int>() << std::endl;
      CaloAffectedRegionInfo::type_problem problem = (CaloAffectedRegionInfo::type_problem)(attrList["problem"].data<int>());

      info.FillCaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,layer_min,layer_max,problem);
      m_affectedRegions->push_back(info);
  }

  m_read = true;
  return StatusCode::SUCCESS;

}

//-------------------------------------------------

bool CaloAffectedTool::isAffected(const xAOD::IParticle *p, float deta, float dphi, int layer_min, int layer_max, int problemType) 
{

 static float epsilon=1e-6;

  //std::cout << " in isAffected " << p->eta() << " " << p->phi() << std::endl;
  if (!m_read &&  !m_readRaw) {
   if (this->readDB().isFailure()) return false;
  }
  if (!m_read) return false;
  if (!m_affectedRegions) return false;

  std::vector<CaloAffectedRegionInfo>::const_iterator reg1 = m_affectedRegions->begin();
  std::vector<CaloAffectedRegionInfo>::const_iterator reg2 = m_affectedRegions->end();
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

bool CaloAffectedTool::listAffected(const xAOD::IParticle*p, std::vector<int>& layer_list, std::vector<int>& problem_list, float deta, float dphi, int problemType)
{

  bool found = false;

  static float epsilon=1e-6;

  layer_list.clear();
  problem_list.clear();

  if (!m_read && !m_readRaw ) {
   if (this->readDB().isFailure()) return false;
  }
  if (!m_read) return false;
  if (!m_affectedRegions) return false;

  std::vector<CaloAffectedRegionInfo>::const_iterator reg1 = m_affectedRegions->begin();
  std::vector<CaloAffectedRegionInfo>::const_iterator reg2 = m_affectedRegions->end();
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
