/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./LArHVCondAlg.h" 
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandle.h"
#include "LArElecCalib/ILArHVPathologyDbTool.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArReadoutGeometry/EMBCell.h"
#include "LArHV/LArHVManager.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVModuleConstLink.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVSubgapConstLink.h"
#include "LArHV/HECHVSubgap.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/FCALHVLineConstLink.h"
#include "LArHV/FCALHVLine.h"

#include "LArHV/LArHVManager.h"

#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArIdentifier/LArOnlineID.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

//#include <iostream> 
#include <cmath> 
#include <cstdlib>
#include "AthenaKernel/errorcheck.h"

#define VDIFF_MAX 0.01 // maximum voltage difference allowed to be treated as equal
#define WDIFF_MAX 0.0001 // maximum weight difference allowed to be treated as equal

#define HV_NON_NOMINAL_TOLERANCE 10 // tolerance : 1V for HV
#define DEAD_HV_THRESHOLD 10 // HV <10 V="dead"


// constructor 
LArHVCondAlg::LArHVCondAlg( const std::string& name, ISvcLocator* pSvcLocator )
  : AthReentrantAlgorithm(name,pSvcLocator),
    m_calodetdescrmgr(nullptr),
    m_larem_id(nullptr),
    m_larhec_id(nullptr),
    m_larfcal_id(nullptr),
    m_electrodeID(nullptr),
    m_hvLineID(nullptr),
    m_condSvc("CondSvc",name)
 {
  declareProperty("doHV",m_doHV=true,"create HV data");
  declareProperty("doAffected",m_doAffected=true,"create affected region info");
  declareProperty("doAffectedHV",m_doAffectedHV=true,"include HV non nominal regions info");
 
}

// destructor 
LArHVCondAlg::~LArHVCondAlg()
{ }

//initialize
StatusCode LArHVCondAlg::initialize(){
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );

  m_larem_id   = idHelper->em_idHelper();
  m_larhec_id   = idHelper->hec_idHelper();
  m_larfcal_id   = idHelper->fcal_idHelper();

  ATH_CHECK(detStore()->retrieve(m_calodetdescrmgr));

  ATH_CHECK(detStore()->retrieve(m_electrodeID));

  ATH_CHECK(detStore()->retrieve(m_hvLineID));

  ATH_CHECK( detStore()->retrieve(m_onlineID));

  // Read Handles
  ATH_CHECK(m_pathologiesKey.initialize());
  ATH_CHECK(m_DCSFolderKeys.initialize());
  ATH_CHECK( m_cablingKey.initialize());
  ATH_CHECK( m_BFKey.initialize() );

  // Write Handle
  
  // Register write handle
  ATH_CHECK( m_condSvc.retrieve() );
  ATH_CHECK(m_hvDataKey.initialize());
  ATH_CHECK(m_outKey.initialize());

  if(m_doHV || m_doAffectedHV) {
    if (m_condSvc->regHandle(this, m_hvDataKey).isFailure()) {
      ATH_MSG_ERROR("unable to register WriteCondHandle " << m_hvDataKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }
  if(m_doAffected) {
     if (m_condSvc->regHandle(this, m_outKey).isFailure()) {
       ATH_MSG_ERROR("unable to register WriteCondHandle " << m_outKey.fullKey() << " with CondSvc");
       return StatusCode::FAILURE;
     }
  }

  ATH_MSG_DEBUG("Configured with doHV " << m_doHV << " doAffected " << m_doAffected << " doAffectedHV " << m_doAffectedHV);

  return StatusCode::SUCCESS;
}


StatusCode LArHVCondAlg::execute(const EventContext& ctx) const {

  ATH_MSG_DEBUG("executing");
  bool doHVData=false;
  bool doAffected=false;

  SG::WriteCondHandle<LArHVData> writeHandle{m_hvDataKey};
  if(m_doHV || m_doAffectedHV) {
    if (writeHandle.isValid()) {
      ATH_MSG_DEBUG("Found valid write LArHVData handle");
    } else {
      doHVData=true;
    }
  } 

  SG::WriteCondHandle<CaloAffectedRegionInfoVec> writeAffectedHandle{m_outKey};
  if(m_doAffected){
    if (writeAffectedHandle.isValid()) {
      ATH_MSG_DEBUG("Found valid write LArAffectedRegions handle");
    } else {
      doAffected=true;
    }
  }

  ATH_MSG_DEBUG("Executing with doHV " << doHVData << " doAffected " << doAffected );

  // Define validity of the output cond object 
  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0, 0, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  EventIDRange rangeW{start, stop};

  std::vector<const CondAttrListCollection*> attrvec;

  if(doHVData || (doAffected && m_doAffectedHV) ) {
    // get handles to DCS Database folders
    for (auto fldkey: m_DCSFolderKeys ) {
      SG::ReadCondHandle<CondAttrListCollection> cHdl(fldkey, ctx);
      const CondAttrListCollection* cattr = *cHdl;
      if(cattr) {
        ATH_MSG_DEBUG("Folder: "<<cHdl.key()<<" has size: "<<std::distance(cattr->begin(),cattr->end()));
        attrvec.push_back(cattr);
        EventIDRange rangeW_tmp;
        if(!cHdl.range(rangeW_tmp)) {
          ATH_MSG_ERROR("Failed to retrieve validity range for " << cHdl.key());
          return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("and validity range: "<<rangeW_tmp);
        rangeW.intersect(rangeW,rangeW_tmp);
      } else {
         ATH_MSG_WARNING("Why do not have DCS folder " << fldkey.fullKey());
      }
    } // over DCS folders
  }
 
  const LArHVManager *manager = NULL; 
  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
        manager->reset();
  }
  std::vector<float> voltage;
  std::vector<float> current;
  std::vector<unsigned int> hvlineidx;
 
  if(doHVData) {
    // Fill pathology info
    pathVec  hasPathologyEM;
    pathVec  hasPathologyHEC;
    pathVec  hasPathologyFCAL;
    hasPathologyEM.resize(m_larem_id->channel_hash_max());
    hasPathologyHEC.resize(m_larhec_id->channel_hash_max());
    hasPathologyFCAL.resize(m_larfcal_id->channel_hash_max());
 
    bool doPathology=true; 
    SG::ReadCondHandle<LArHVPathology> pHdl(m_pathologiesKey,ctx);
    const LArHVPathology* pathologyContainer = *pHdl;
    if(!pathologyContainer) {
       ATH_MSG_WARNING("Why do not have HV pathology object " << m_pathologiesKey.fullKey() << " ? Work without pathologies !!!");
       doPathology=false;
    }
 
    if(doPathology) {
       const std::vector<LArHVPathologiesDb::LArHVElectPathologyDb> &pathCont = pathologyContainer->getPathology();
       ATH_MSG_INFO( " Number of HV pathologies found " << pathCont.size());
       for(unsigned i=0; i<pathologyContainer->getPathology().size(); ++i) {
           LArHVPathologiesDb::LArHVElectPathologyDb electPath = pathologyContainer->getPathology()[i];
           Identifier id(electPath.cellID);
           if (m_larem_id->is_lar_em(id)) {
               IdentifierHash idHash = m_larem_id->channel_hash(id);
               unsigned int index = (unsigned int)(idHash);
               if (index<hasPathologyEM.size()) {
                  if(hasPathologyEM[index].size()) {
                    if(hasPathologyEM[index].size()<static_cast<size_t>(abs(electPath.electInd+1)))
                      hasPathologyEM[index].resize(electPath.electInd+1);
                     hasPathologyEM[index][electPath.electInd]=electPath.pathologyType;
                  } else {
                     std::vector<unsigned short> svec;
                     svec.resize(electPath.electInd+1);
                     svec[electPath.electInd]=electPath.pathologyType;
                     hasPathologyEM[index]=svec;
                  }
               }
           }
           if (m_larhec_id->is_lar_hec(id)) {
               IdentifierHash idHash = m_larhec_id->channel_hash(id);
               unsigned int index = (unsigned int)(idHash);
               if (index<hasPathologyHEC.size()) {
                  if(hasPathologyHEC[index].size()) {
                    if(hasPathologyHEC[index].size()<static_cast<size_t>(abs(electPath.electInd+1)))
                      hasPathologyHEC[index].resize(electPath.electInd+1);
                     hasPathologyHEC[index][electPath.electInd]=electPath.pathologyType;
                  } else {
                     std::vector<unsigned short> svec;
                     svec.resize(electPath.electInd+1);
                     svec[electPath.electInd]=electPath.pathologyType;
                     hasPathologyHEC[index]=svec;
                  }
               }
           }
           if (m_larfcal_id->is_lar_fcal(id)) {
               IdentifierHash idHash = m_larfcal_id->channel_hash(id);
               unsigned int index = (unsigned int)(idHash);
               if (index<hasPathologyFCAL.size()) {
                  if(hasPathologyFCAL[index].size()) {
                    if(hasPathologyFCAL[index].size()<static_cast<size_t>(abs(electPath.electInd+1)))
                      hasPathologyFCAL[index].resize(electPath.electInd+1);
                    hasPathologyFCAL[index][electPath.electInd]=electPath.pathologyType;
                  } else {
                     std::vector<unsigned short> svec;
                     svec.resize(electPath.electInd+1);
                     svec[electPath.electInd]=electPath.pathologyType;
                     hasPathologyFCAL[index]=svec;
                  }
 
               }
           }
       } // Pathology containers
    }//doPathology
    // now call filling of HV values
    if(doHVData || (doAffected && m_doAffectedHV)) {
      ATH_CHECK(fillUpdatedHVChannelsVec(voltage, current, hvlineidx, attrvec));
    }
    // try to get old cond. object
    const LArHVData* hvdataOld = nullptr;
    
    std::unique_ptr<LArHVData> hvdata = std::make_unique<LArHVData>();
    LArHVData* p_hvdata = hvdata.get();
  
    ATH_CHECK(fillPayload(p_hvdata, hvdataOld, voltage, current, hvlineidx, pathologyContainer, hasPathologyEM, hasPathologyHEC, hasPathologyFCAL));
  
  
    const EventIDRange crangeW(rangeW);
    if(writeHandle.record(crangeW,hvdata.release()).isFailure()) {
        ATH_MSG_ERROR("Could not record LArHVData object with " << writeHandle.key()
                      << " with EventRange " << crangeW << " into Conditions Store");
        return StatusCode::FAILURE;
    }
  
    ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << crangeW << " into Conditions Store");
  } // doHVData

  if(doAffected) {
       SG::ReadCondHandle<LArBadFebCont> readBFHandle{m_BFKey};
       const LArBadFebCont* bfCont{*readBFHandle};
       if(!bfCont){
         ATH_MSG_WARNING(" Do not have Bad FEBs info, will be not filled " << m_BFKey.key() );
       }

       SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
       const LArOnOffIdMapping* cabling{*cablingHdl};
       if(!cabling) {
         ATH_MSG_ERROR("Do not have cabling mapping from key " << m_cablingKey.key() );
         return StatusCode::FAILURE;
       }

       // Define validity of the output cond object
       EventIDRange rangeW_tmp;
       if(!readBFHandle.range(rangeW_tmp)) {
          ATH_MSG_ERROR("Failed to retrieve validity range for " << readBFHandle.key());
          return StatusCode::FAILURE;
       }
       ATH_MSG_DEBUG("and validity range: "<<rangeW_tmp);
       rangeW.intersect(rangeW,rangeW_tmp);

       if(!cablingHdl.range(rangeW_tmp)) {
          ATH_MSG_ERROR("Failed to retrieve validity range for " << cablingHdl.key());
          return StatusCode::FAILURE;
       }
       ATH_MSG_DEBUG("and validity range: "<<rangeW_tmp);
       rangeW.intersect(rangeW,rangeW_tmp);
   
       CaloAffectedRegionInfoVec *vAffected = new CaloAffectedRegionInfoVec();
   
       if (m_doAffectedHV) {
         ATH_CHECK(searchNonNominalHV_EMB(vAffected, voltage, hvlineidx));
         ATH_CHECK(searchNonNominalHV_EMEC_OUTER(vAffected, voltage, hvlineidx));
         ATH_CHECK(searchNonNominalHV_EMEC_INNER(vAffected, voltage, hvlineidx));
         ATH_CHECK(searchNonNominalHV_HEC(vAffected, voltage, hvlineidx));
         ATH_CHECK(searchNonNominalHV_FCAL(vAffected, voltage, hvlineidx));
       }
   
   
       ATH_CHECK(updateMethod(vAffected, bfCont, cabling));
   
       std::unique_ptr<CaloAffectedRegionInfoVec> affectedVec = std::make_unique<CaloAffectedRegionInfoVec>(std::move(*vAffected));
   
       const EventIDRange crangeW(rangeW);
       if(writeAffectedHandle.record(crangeW,affectedVec.release()).isFailure()) {
                ATH_MSG_ERROR("Could not record CaloAffectedRegionInfoVec object with " << writeHandle.key()
                              << " with EventRange " << crangeW << " into Conditions Store");
                return StatusCode::FAILURE;
       }
   
       ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << crangeW << " into Conditions Store");
  } //doAffected

  return StatusCode::SUCCESS;

}


StatusCode LArHVCondAlg::finalize() {
  //ATH_MSG_INFO("# of actual voltage changes:" << m_updatedElectrodes.size()); 
  return StatusCode::SUCCESS;
}


StatusCode LArHVCondAlg::fillPayload(LArHVData* hvdata, const LArHVData* hvdataOld, std::vector<float> &voltage, std::vector<float> &current, std::vector<unsigned int> &hvlineidx , const LArHVPathology& pathologies, pathVec& hasPathologyEM, pathVec& hasPathologyHEC, pathVec& hasPathologyFCAL) const
{
  LArHVData::hvMap &hvmap = hvdata->m_voltage;
  LArHVData::currMap &currmap = hvdata->m_current;
  std::set<Identifier> &updatedCells = hvdata->m_updatedCells;

  std::vector<unsigned int> listElec;

  updatedCells.clear();
  hvmap.clear();
  currmap.clear();

  std::vector<LArHVData::HV_t> v;
  std::vector<LArHVData::CURRENT_t> ihv;
  // loop over all EM Identifiers
  for (auto id: m_larem_id->channel_ids()) {
      v.clear();
      ihv.clear();
      if (abs(m_larem_id->barrel_ec(id))==1 && m_larem_id->sampling(id) > 0) { // LAr EMB
         //std::cout << " in barrel " << m_larem_id->show_to_string(id) << std::endl;
         unsigned int index = (unsigned int)(m_larem_id->channel_hash(id));
         bool hasPathology=false; 
         if (index<hasPathologyEM.size()) {
          if (hasPathologyEM[index].size()) {
           hasPathology=true;
           listElec = getElecList(id,pathologies);
          }
         }
         const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
         if (!embElement) std::abort();
         const EMBCellConstLink cell = embElement->getEMBCell();
         unsigned int nelec = cell->getNumElectrodes();
         //std::cout << " nelec " << nelec << std::endl;
         unsigned int ngap = 2*nelec;
         double wt = 1./ngap;
         v.clear(); ihv.clear();
         for (unsigned int i=0;i<nelec;i++) {
             const EMBHVElectrodeConstLink electrode = cell->getElectrode(i);
             //std::cout << "electrode: endcap index, eta index , phi index, sector index , electrode index " << electrode->getModule()->getSideIndex() <<
             //   " " << electrode->getModule()->getEtaIndex() << " " << electrode->getModule()->getPhiIndex() << 
             //   " " << electrode->getModule()->getSectorIndex() << " " << electrode->getElectrodeIndex() << std::endl;
             for (unsigned int igap=0;igap<2;igap++) {
                 const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), electrode->hvLineNo(igap));
                 if(itrLine == hvlineidx.end()) { // error, could not find HVline index
                    ATH_MSG_ERROR("Do not have hvline: "<<electrode->hvLineNo(igap)<<" in LArHVData mapping !!!");
                    return StatusCode::FAILURE;
                 }
                 unsigned idx = itrLine - hvlineidx.begin(); 
                 double hv=voltage[idx];
                 double curr=current[idx];
                 if (hasPathology) {
                    ATH_MSG_DEBUG( "Has pathology for id: "<< m_larem_id->print_to_string(id)<<" "<<hasPathologyEM[index]);
                    msg(MSG::DEBUG) << "Original hv: "<<hv<<" ";
                    for (unsigned int ii=0;ii<listElec.size();ii++) {
                       if (listElec[ii]==(2*i+igap) && listElec[ii]<hasPathologyEM[index].size() && hasPathologyEM[index][listElec[ii]]) {
                          if(hasPathologyEM[index][listElec[ii]]&0xF) hv=0.; else hv=((hasPathologyEM[index][listElec[ii]]&0xFFF0)>>4);
                          curr=0.;
                       }
                    }
                    msg(MSG::DEBUG) << "set hv: "<<hv<<endmsg;
                 }
                 //std::cout << "     hv value " << hv << std::endl;
                 //if (igap==1 && hv>1.) std::cout << " --- non zero value found for gap1 in barrel " << std::endl;
                 addHV(v,hv,wt);
                 addCurr(ihv,curr,wt);
                
             }
         }        
         hvmap.insert(std::make_pair(id,v));
         currmap.insert(std::make_pair(id,ihv));
      } else if (abs(m_larem_id->barrel_ec(id))==1 && m_larem_id->sampling(id) == 0) { // EMBPS

         const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
         if (!embElement) std::abort();
         const EMBCellConstLink cell = embElement->getEMBCell();
 
         const EMBPresamplerHVModuleConstLink hvmodule =  cell->getPresamplerHVModule ();
 
         double wt = 0.5;
         for (unsigned int igap=0;igap<2;igap++) {
             const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), hvmodule->hvLineNo(igap));
             if(itrLine == hvlineidx.end()) { // error, could not find HVline index
                ATH_MSG_ERROR("Do not have hvline: "<<hvmodule->hvLineNo(igap)<<" in LArHVData mapping !!!");
                return StatusCode::FAILURE;
             }
             unsigned idx = itrLine - hvlineidx.begin(); 
             double hv=voltage[idx];
             double curr=current[idx];
             addHV(v,hv,wt);
             addCurr(ihv,curr,wt);
         }

      } else if (abs(m_larem_id->barrel_ec(id))>1 && m_larem_id->sampling(id) > 0){ // LAr EMEC
         //std::cout << " in EMEC " << m_larem_id->show_to_string(id) << std::endl;
         unsigned int index = (unsigned int)(m_larem_id->channel_hash(id));
         bool hasPathology=false;
         if (index<hasPathologyEM.size()) {
          if (hasPathologyEM[index].size()) {
           hasPathology=true;
           listElec = getElecList(id, pathologies);
          }
         }
 
         const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
         if (!emecElement) std::abort();
         const EMECCellConstLink cell = emecElement->getEMECCell();
         unsigned int nelec = cell->getNumElectrodes();
         //std::cout << " nelec " << nelec << std::endl;
         unsigned int ngap = 2*nelec;
         double wt = 1./ngap;
         for (unsigned int i=0;i<nelec;i++) {
             const EMECHVElectrodeConstLink electrode = cell->getElectrode(i);
           //  std::cout << "electrode: endcap index, eta index , phi index, sector index , electrode index " << electrode->getModule()->getSideIndex() <<
           //     " " << electrode->getModule()->getEtaIndex() << " " << electrode->getModule()->getPhiIndex() << 
           //     " " << electrode->getModule()->getSectorIndex() << " " << electrode->getElectrodeIndex() << std::endl;
             for (unsigned int igap=0;igap<2;igap++) {
                 const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), electrode->hvLineNo(igap));
                 if(itrLine == hvlineidx.end()) { // error, could not find HVline index
                   ATH_MSG_ERROR("Do not have hvline: "<<electrode->hvLineNo(igap)<<" in LArHVData mapping !!!");
                   return StatusCode::FAILURE;
                 }
                 unsigned idx = itrLine - hvlineidx.begin(); 
                 double hv=voltage[idx];
                 double curr=current[idx];
                 if (hasPathology) {
                    msg(MSG::DEBUG) << "Has pathology for id: "<< m_larem_id->print_to_string(id)<<" "<<hasPathologyEM[index]<<endmsg;
                    for (unsigned int ii=0;ii<listElec.size();ii++) {
                       if (listElec[ii]==(2*i+igap) && listElec[ii]<hasPathologyEM[index].size() && hasPathologyEM[index][listElec[ii]]) {
                          if(hasPathologyEM[index][listElec[ii]]&0xF) hv=0.; else hv=((hasPathologyEM[index][listElec[ii]]&0xFFF0)>>4);
                          curr=0.;
                       }
                    }
                 }
                //std::cout << "     hv value " << hv << std::endl;
                //if (igap==1 && hv>1.) std::cout << " --- non zero value found for gap1 in endcap " << std::endl;
                addHV(v,hv,wt);
                addCurr(ihv,curr,wt);
             }
         }
 
      } else if (abs(m_larem_id->barrel_ec(id))>1 &&  m_larem_id->sampling(id)==0) { // EMECPS

         const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
         if (!emecElement) std::abort();
         const EMECCellConstLink cell = emecElement->getEMECCell();
 
         const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule ();
 
         double wt = 0.5; 
         for (unsigned int igap=0;igap<2;igap++) {
             const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), hvmodule->hvLineNo(igap));
             if(itrLine == hvlineidx.end()) { // error, could not find HVline index
                ATH_MSG_ERROR("Do not have hvline: "<<hvmodule->hvLineNo(igap)<<" in LArHVData mapping !!!");
                return StatusCode::FAILURE;
             }
             unsigned idx = itrLine - hvlineidx.begin(); 
             double hv=voltage[idx];
             double curr=current[idx];
             addHV(v,hv,wt);
             addCurr(ihv,curr,wt);
         }

      } else { // something wrong
         ATH_MSG_ERROR("This could not be, what happened with EM identifiers ?");
         return StatusCode::FAILURE;
      }

      hvmap.emplace(id,v);
      currmap.emplace(id,ihv);
      if(!hvdataOld) { // all cells are updated
         updatedCells.emplace(id);
      } else { // check if it was changed
         std::vector< LArHVData::HV_t > oldv;
         ATH_CHECK(hvdataOld->getHV(id, oldv));
         if(v.size() == oldv.size()) {
            unsigned int found=0;
            for(unsigned int i=0;i<v.size();++i) {
               for(unsigned int j=0; j<v.size(); ++j) {
                  if(fabs(v[i].hv - oldv[j].hv) < VDIFF_MAX  && fabs(v[i].weight - oldv[j].weight) < WDIFF_MAX) {
                     ++found;
                     break;
                  }
               }
            }   
            if(found != v.size()) updatedCells.emplace(id);

         } else { // different, changed
            updatedCells.emplace(id);
         }
      }
  } // loop over EM
  // LAr HEC
  for( auto id: m_larhec_id->channel_ids()) {
    v.clear();
    ihv.clear();
    //std::cout << " in HEC " << std::endl;
    unsigned int index = (unsigned int)(m_larhec_id->channel_hash(id));
    bool hasPathology=false;
    if (index<hasPathologyHEC.size()) {
     if (hasPathologyHEC[index].size()) {
      hasPathology=true;
      listElec = getElecList(id, pathologies);
     }
    }
    const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(m_calodetdescrmgr->get_element(id));
    if (!hecElement) std::abort();
    const HECCellConstLink cell = hecElement->getHECCell();
    unsigned int nsubgaps = cell->getNumSubgaps();
    double wt = 1./nsubgaps;
    //std::cout << " nsubgaps " << nsubgaps << std::endl;
    for (unsigned int i=0;i<nsubgaps;i++) {
        const HECHVSubgapConstLink subgap = cell->getSubgap(i);
        const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), subgap->hvLineNo());
        if(itrLine == hvlineidx.end()) { // error, could not find HVline index
           ATH_MSG_ERROR("Do not have hvline: "<<subgap->hvLineNo()<<" in LArHVData mapping !!!");
           return StatusCode::FAILURE;
        }
        unsigned idx = itrLine - hvlineidx.begin(); 
        double hv=voltage[idx];
        double curr=current[idx];
        //std::cout << "     hv value " << hv << std::endl;
        if (hasPathology) {
           msg(MSG::DEBUG) << "Has pathology for id: "<< m_larhec_id->print_to_string(id)<<" "<<hasPathologyHEC[index]<<endmsg;
           for (unsigned int ii=0;ii<listElec.size();ii++) {
              if (listElec[ii]==i && listElec[ii]<hasPathologyHEC[index].size() && hasPathologyHEC[index][listElec[ii]]) {
                   if(hasPathologyHEC[index][listElec[ii]]&0xF) hv=0.; else hv=((hasPathologyHEC[index][listElec[ii]]&0xFFF0)>>4);
                   curr=0.;
              }
           }
        }

        addHV(v,hv,wt);
        addCurr(ihv,curr,wt);
    }
    hvmap.emplace(id,v);
    currmap.emplace(id,ihv);
    if(!hvdataOld) { // all cells are updated
       updatedCells.emplace(id);
    } else { // check if it was changed
       std::vector< LArHVData::HV_t > oldv;
       ATH_CHECK(hvdataOld->getHV(id, oldv));
       if(v.size() == oldv.size()) {
          unsigned int found=0;
          for(unsigned int i=0;i<v.size();++i) {
             for(unsigned int j=0; j<v.size(); ++j) {
                if(fabs(v[i].hv - oldv[j].hv) < VDIFF_MAX  && fabs(v[i].weight - oldv[j].weight) < WDIFF_MAX) {
                   ++found;
                   break;
                }
             }
          }   
          if(found != v.size()) updatedCells.emplace(id);

       } else { // different, changed
          updatedCells.emplace(id);
       }
    }
   } // loop over HEC
   for(auto id: m_larfcal_id->channel_ids()) { // LAr FCAL
      v.clear();
      ihv.clear();
      //std::cout << " in FCAL " << std::endl;
      unsigned int index = (unsigned int)(m_larfcal_id->channel_hash(id));
      bool hasPathology=false;
      if (index<hasPathologyFCAL.size()) {
       if (hasPathologyFCAL[index].size()) {
        hasPathology=true;
        listElec = getElecList(id, pathologies);
       }
      }
      const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!fcalElement) std::abort();
      const FCALTile* tile = fcalElement->getFCALTile();
      //std::cout << " --- in FCAL cell id " << m_larfcal_id->show_to_string(id) << std::endl;
      //std::cout << "  Side, Module, I,J index " << tile->getModule()->getEndcapIndex() << " " << tile->getModule()->getModuleIndex()
      //  << " " << tile->getIndexI() << " " << tile->getIndexJ() << std::endl;
      unsigned int nlines = tile->getNumHVLines();
      unsigned int nlines_found=0;
      for (unsigned int i=0;i<nlines;i++) {
        const FCALHVLineConstLink line = tile->getHVLine(i);
        if (line) nlines_found++;
      }
      //std::cout << " nlines " << nlines << " " << nlines_found << std::endl;
      if (nlines_found>0) {
        double wt = 1./nlines_found;
        for (unsigned int i=0;i<nlines;i++) {
          const FCALHVLineConstLink line = tile->getHVLine(i);
          if (!line) continue;
          const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), line->hvLineNo());
          if(itrLine == hvlineidx.end()) { // error, could not find HVline index
           ATH_MSG_ERROR("Do not have hvline: "<<line->hvLineNo()<<" in LArHVData mapping !!!");
           return StatusCode::FAILURE;
          }
          unsigned idx = itrLine - hvlineidx.begin(); 
          double hv=voltage[idx];
          double curr=current[idx];
          //std::cout << " line " << line;
          if (hasPathology) {
             msg(MSG::DEBUG) << "Has pathology for id: "<< m_larfcal_id->print_to_string(id)<<" "<<hasPathologyFCAL[index]<<endmsg;
             for (unsigned int ii=0;ii<listElec.size();ii++) {
                if (listElec[ii]==i && listElec[ii]<hasPathologyFCAL[index].size() && hasPathologyFCAL[index][listElec[ii]]) {
                     if(hasPathologyFCAL[index][listElec[ii]]&0xF) hv=0.; else hv=((hasPathologyFCAL[index][listElec[ii]]&0xFFF0)>>4);
                     curr=0.;
                }
             }
          }
          //std::cout << "     hv value " << hv << std::endl;
          addHV(v,hv,wt);
          addCurr(ihv,curr,wt);
        }
      }
      hvmap.emplace(id,v);
      currmap.emplace(id,ihv);
      if(!hvdataOld) { // all cells are updated
         updatedCells.emplace(id);
      } else { // check if it was changed
         std::vector< LArHVData::HV_t > oldv;
         ATH_CHECK(hvdataOld->getHV(id, oldv));
         if(v.size() == oldv.size()) {
            unsigned int found=0;
            for(unsigned int i=0;i<v.size();++i) {
               for(unsigned int j=0; j<v.size(); ++j) {
                  if(fabs(v[i].hv - oldv[j].hv) < VDIFF_MAX  && fabs(v[i].weight - oldv[j].weight) < WDIFF_MAX) {
                     ++found;
                     break;
                  }
               }
            }   
            if(found != v.size()) updatedCells.emplace(id);
 
         } else { // different, changed
            updatedCells.emplace(id);
         }
      }
   } // loop over FCAL
  
  return StatusCode::SUCCESS; 
}

void LArHVCondAlg::addHV(std::vector< LArHVData::HV_t > & v , double hv, double wt) const
{
         bool found=false;
         for (unsigned int i=0;i<v.size();i++) {
            if (std::fabs(hv-v[i].hv) < 0.1) {
               found=true;  
               v[i].weight += wt;
               break;
             }
         }
         if (!found) {
           LArHVData::HV_t hh;
           hh.hv = hv;
           hh.weight = wt;
           v.push_back(hh);
         }     // not already in the list
}

void LArHVCondAlg::addCurr(std::vector< LArHVData::CURRENT_t > & ihv , double current, double wt) const
{
         bool found=false;
         for (unsigned int i=0;i<ihv.size();i++) {
            if (std::fabs(current-ihv[i].current) < 0.1) {
               found=true;  
               ihv[i].weight += wt;
               break;
             }
         }
         if (!found) {
           LArHVData::CURRENT_t ii;
           ii.current = current;
           ii.weight = wt;
           ihv.push_back(ii);
         }     // not already in the list
}

std::vector<unsigned int> LArHVCondAlg::getElecList(const Identifier& id, const LArHVPathology& pathologyContainer) const
{
    std::vector<unsigned int> myList;
    myList.clear();
    for(unsigned i=0; i<pathologyContainer.getPathology().size(); ++i) {
      LArHVPathologiesDb::LArHVElectPathologyDb electPath = pathologyContainer.getPathology()[i];
      if (electPath.cellID == (unsigned int)(id.get_identifier32().get_compact())) {
          myList.push_back(electPath.electInd);
      }
    }
    return myList;

}



StatusCode LArHVCondAlg::fillUpdatedHVChannelsVec(std::vector<float> &voltageCache, std::vector<float> &currentCache, std::vector<unsigned int> &hvlineidx, std::vector<const CondAttrListCollection* > fldvec) const {

  //const unsigned nHVCoolChannels=453+4384;

  //Loop over the list of DCS folders

  voltageCache.clear();
  currentCache.clear();
  hvlineidx.clear();
  ATH_MSG_DEBUG("Got "<<fldvec.size()<<" DCS HV folders");
  for(auto attrlist : fldvec) { // loop over all DCS folders
    CondAttrListCollection::const_iterator citr=attrlist->begin(); 
    CondAttrListCollection::const_iterator citr_e=attrlist->end();
    ATH_MSG_DEBUG("Length: "<<std::distance(citr,citr_e));
    for(;citr!=citr_e;++citr) {
      const unsigned chan=citr->first;
      ATH_MSG_DEBUG("Got HV cool chan: "<< chan);
      const coral::Attribute& attr=((citr)->second)["R_VMEAS"];
      float voltage=-999;
      if (!attr.isNull()) voltage=attr.data<float>(); //Ignore NULL values
      voltageCache.push_back(voltage);
      const coral::Attribute& attrc=((citr)->second)["R_IMEAS"];
      float current=0.;
      if (!attrc.isNull()) current=attrc.data<float>(); //Ignore NULL values
      ATH_MSG_DEBUG("read voltage: "<<voltage<<" and current: "<<current );
      currentCache.push_back(current);
      hvlineidx.push_back(chan);
    }//end loop over attributeListCollection
    ATH_MSG_DEBUG("Filling voltage, "<< voltageCache.size() << " channels.");
  }
  return StatusCode::SUCCESS;
}

//=========================================================================================
StatusCode LArHVCondAlg::searchNonNominalHV_EMB(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const {  // deals with LAr HV, EMBarrel

  ATH_MSG_DEBUG(" start HV_EMB ");
  const LArHVManager *manager = nullptr;

  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
    
    // accordion calorimeter
    float HVnominal = HV_nominal("EMB",0.);
    const EMBHVManager* hvManager_EMB=manager->getEMBHVManager();
    
    for (unsigned int iSide=hvManager_EMB->beginSideIndex();iSide<hvManager_EMB->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMB->beginPhiIndex();iPhi<hvManager_EMB->endPhiIndex();iPhi++) {
	for (unsigned int iSector=hvManager_EMB->beginSectorIndex();iSector<hvManager_EMB->endSectorIndex();iSector++) {
	  for (unsigned int iEta=hvManager_EMB->beginEtaIndex();iEta<hvManager_EMB->endEtaIndex();iEta++) { //0 to 7
	    EMBHVModuleConstLink hvMod = hvManager_EMB->getHVModule(iSide,iEta,iPhi,iSector);

            float eta_min=hvMod->getEtaMin();
            float eta_max=hvMod->getEtaMax();

            ATH_MSG_DEBUG("iSide,iPhi,iSector,iEta " << iSide << " " << iPhi << " " << iSector << " " << iEta);
	    float phi_min=+30.,phi_max=-30.;
	    
	    bool are_previous_HV_affected=false;
            bool are_previous_HV_dead=false;
	    for (unsigned int ielec=0;ielec<32;ielec++) { //use hvMod->getNumElectrodes when bug is corrected
	      EMBHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);

	      double hv[2];
	      for (unsigned int iGap=0;iGap<2;iGap++) { // EMB : 2, TRY TO FIND AUTOMATICALLY NB OF GAPS
                const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), electrode->hvLineNo(iGap));
                if(itrLine == hvlineidx.end()) { // error, could not find HVline index
                  ATH_MSG_ERROR("Do not have hvline: "<<electrode->hvLineNo(iGap)<<" in LArHVData !!!");
                  return StatusCode::FAILURE;
                }
                //unsigned idx = itrLine - hvlineidx.begin(); 
		hv[iGap]=voltage[itrLine - hvlineidx.begin()];
	      } //end for iGap

              ATH_MSG_DEBUG(" electrode HV " << ielec << " " << electrode->getPhi() << " "<< hv[0] << " " << hv[1] );

	      //take decisions according to all the gaps HV :
              bool isDead=false;
              if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
              bool isAffected=false;
              if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
              ATH_MSG_DEBUG(" dead/affected " << isDead << " " << isAffected);
          
              // end previous dead region
              if (are_previous_HV_dead && !isDead) {
                 are_previous_HV_dead=false;
                 ATH_MSG_DEBUG(" -- end dead region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max);
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,1,3,CaloAffectedRegionInfo::HVdead);
                 vAffected->push_back(current_CaloAffectedRegionInfo);
              }

              // end previous affected region
              if (are_previous_HV_affected && !isAffected) {
                 are_previous_HV_affected=false;
                 ATH_MSG_DEBUG(" -- end affected region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max);
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,1,3,CaloAffectedRegionInfo::HVaffected);
                 vAffected->push_back(current_CaloAffectedRegionInfo);
              }

              if (isDead) {
                 if (!are_previous_HV_dead) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  ATH_MSG_DEBUG(" -- start dead region " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max);
                  are_previous_HV_dead = true;
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  ATH_MSG_DEBUG(" extend dead region " << phi_min << " " << phi_max);
                 }
              }

              if (isAffected) {
                 if (!are_previous_HV_affected) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  ATH_MSG_DEBUG(" -- start affected region " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max);
                  are_previous_HV_affected = true;
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  ATH_MSG_DEBUG(" extend affected region " << phi_min << " " << phi_max);
                 }
              }

	    } // end for ielec
	    
	    if (are_previous_HV_affected) { //in case a non nominal exists, stores it if we finish the 32 electrodes (because else the are_previous_HV_affected will be reinitialized for the next 32 electrodes serie )
              ATH_MSG_DEBUG("  -- finish affected region after electrode loop " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max);
	      CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,1,3,CaloAffectedRegionInfo::HVaffected);
	      vAffected->push_back(current_CaloAffectedRegionInfo);	      
	    }
            if (are_previous_HV_dead) {
              ATH_MSG_DEBUG("  -- finish dead region after electrode loop " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max);
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,1,3,CaloAffectedRegionInfo::HVdead);
              vAffected->push_back(current_CaloAffectedRegionInfo);
            }
	  } // end for iEta
	} // end for iSector
      } // end for iPhi
    } //end for iSide

    // barrel presampler
    const EMBPresamplerHVManager* hvManager_EMBPS=manager->getEMBPresamplerHVManager();
    HVnominal = HV_nominal("EMBPS",0.);
    for (unsigned int iSide=hvManager_EMBPS->beginSideIndex();iSide<hvManager_EMBPS->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMBPS->beginPhiIndex();iPhi<hvManager_EMBPS->endPhiIndex();iPhi++) {
          for (unsigned int iEta=hvManager_EMBPS->beginEtaIndex();iEta<hvManager_EMBPS->endEtaIndex();iEta++) { //0 to 7
            EMBPresamplerHVModuleConstLink hvMod = hvManager_EMBPS->getHVModule(iSide,iEta,iPhi);
            ATH_MSG_DEBUG("iSide,iPhi,iEta " << iSide << " " << iPhi << " " << iEta);
            double hv[2];
            for (int iGap=0;iGap<2;iGap++) {
                const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), hvMod->hvLineNo(iGap));
                if(itrLine == hvlineidx.end()) { // error, could not find HVline index
                  ATH_MSG_ERROR("Do not have hvline: "<<hvMod->hvLineNo(iGap)<<" in LArHVData !!!");
                  return StatusCode::FAILURE;
                }
		hv[iGap]=fabs(voltage[itrLine - hvlineidx.begin()]);
            }
            float eta_min=hvMod->getEtaMin();
            float eta_max=hvMod->getEtaMax();
            float phi_min=CaloPhiRange::fix(hvMod->getPhiMin());
            float phi_max=CaloPhiRange::fix(hvMod->getPhiMax());
            ATH_MSG_DEBUG("  HV " <<  hv[0] << " " << hv[1] << " " << "  etamin,etamax,phimin,phimax " << eta_min << " " << eta_max << " " << phi_min << " " << phi_max);

            //take decisions according to all the gaps HV :
            bool isDead=false;
            if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
            bool isAffected=false;
            if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
            ATH_MSG_DEBUG(" dead/affected " << isDead << " " << isAffected );

            if (isDead) {
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,0,0,CaloAffectedRegionInfo::HVdead);
              vAffected->push_back(current_CaloAffectedRegionInfo);             
            }
            if (isAffected) {
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,0,0,CaloAffectedRegionInfo::HVaffected);
              vAffected->push_back(current_CaloAffectedRegionInfo);
            }
          }       // loop over iEta EMBPS
      }        // loop over iphi EMBPS
    }    // lop over EMBPS side
  } else {
     ATH_MSG_ERROR("Do not have EMB HV Manager !!! ");
     return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=========================================================================================
StatusCode LArHVCondAlg::searchNonNominalHV_EMEC_OUTER(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const { // deals with LAr HV, EM EndCap OUTER

  const LArHVManager *manager = nullptr;

  ATH_MSG_DEBUG(" start HV_EMEC_OUTER ");

  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
      
    const EMECHVManager* hvManager_EMEC=manager->getEMECHVManager(EMECHVModule::OUTER);

    for (unsigned int iSide=hvManager_EMEC->beginSideIndex();iSide<hvManager_EMEC->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMEC->beginPhiIndex();iPhi<hvManager_EMEC->endPhiIndex();iPhi++) {
	for (unsigned int iSector=hvManager_EMEC->beginSectorIndex();iSector<hvManager_EMEC->endSectorIndex();iSector++) {
	  for (unsigned int iEta=hvManager_EMEC->beginEtaIndex();iEta<hvManager_EMEC->endEtaIndex();iEta++) {
	    EMECHVModuleConstLink hvMod=hvManager_EMEC->getHVModule(iSide,iEta,iPhi,iSector);

            float etaMod = 0.5*(fabs(hvMod->getEtaMin())+fabs(hvMod->getEtaMax()));
            float HVnominal = HV_nominal("EMEC",etaMod);

            float eta_min=hvMod->getEtaMin();
            float eta_max=hvMod->getEtaMax();

            ATH_MSG_DEBUG("iSide,iPhi,iSector,iEta " << iSide << " " << iPhi << " " << iSector << " " 
                          << iEta << " eta_min , eta_max " << eta_min << " " << eta_max );


	    float phi_min=+30.,phi_max=-30.;

	    bool are_previous_HV_affected=false;
            bool are_previous_HV_dead=false;
	    for (unsigned int ielec=0;ielec<hvMod->getNumElectrodes();ielec++) { //use hvMod->getNumElectrodes when bug is corrected
	      EMECHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);

	      double hv[2];
	      for (unsigned int iGap=0;iGap<2;iGap++) { //EMEC : 2 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
                  const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), electrode->hvLineNo(iGap));
                  if(itrLine == hvlineidx.end()) { // error, could not find HVline index
                     ATH_MSG_ERROR("Do not have hvline: "<<electrode->hvLineNo(iGap)<<" in LArHVData !!!");
                     return StatusCode::FAILURE;
                  }
		  hv[iGap]=voltage[itrLine - hvlineidx.begin()];
	      } //end for iGap

	      //------------------
	      //take decisions according to all the gaps HV :
              bool isDead=false;
              if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
              bool isAffected=false;
              if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
              ATH_MSG_DEBUG(" electrode HV " << ielec << " " << electrode->getPhi() << " " << hv[0] 
                            << " " << hv[1] << " " << " isDead/isAffected " << isDead << " " << isAffected );
          
              // end previous dead region
              if (are_previous_HV_dead && !isDead) {
                 are_previous_HV_dead=false;
                 ATH_MSG_DEBUG(" -- end dead region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max);
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVdead);
                 vAffected->push_back(current_CaloAffectedRegionInfo);
              }

              // end previous affected region
              if (are_previous_HV_affected && !isAffected) {
                 are_previous_HV_affected=false;
                 ATH_MSG_DEBUG(" -- end affected region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max);
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVaffected);
                 vAffected->push_back(current_CaloAffectedRegionInfo);
              }

              if (isDead) {
                 if (!are_previous_HV_dead) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  are_previous_HV_dead = true;
                  ATH_MSG_DEBUG(" -- start dead region " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max);
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  ATH_MSG_DEBUG("  extend affected region " << phi_min << " " << phi_max);
                 }
              }

              if (isAffected) {
                 if (!are_previous_HV_affected) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  are_previous_HV_affected = true;
                  ATH_MSG_DEBUG(" -- start affected region " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max);
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  ATH_MSG_DEBUG(" extend affected region " << phi_min << " " << phi_max);
                 }
              }

	    } // end for ielec
	    
	    if (are_previous_HV_affected) { 
              //in case a non nominal exists, stores it if we finish the 32 electrodes 
              //(because else the are_previous_HV_affected will be reinitialized 
              //for the next 32 electrodes serie )
              ATH_MSG_DEBUG("   - finih affected region after electrode loop " << eta_min << " " << eta_max 
                            << " " << phi_max << " " <<phi_max);
	      CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVaffected);
	      vAffected->push_back(current_CaloAffectedRegionInfo);	      
	    }
            if (are_previous_HV_dead) {
              ATH_MSG_DEBUG(" -- finish dead region after electrode loop " << eta_min << " " << eta_max << " " << phi_max << " " <<phi_max);
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVdead);
              vAffected->push_back(current_CaloAffectedRegionInfo);
            }
	  } // end for iEta
	} // end for iSector
      } // end for iPhi
    } //end for iSide

    // endcap presampler
    const EMECPresamplerHVManager* hvManager_EMECPS=manager->getEMECPresamplerHVManager();
    float HVnominal = HV_nominal("EMECPS",0.);
    for (unsigned int iSide=hvManager_EMECPS->beginSideIndex();iSide<hvManager_EMECPS->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMECPS->beginPhiIndex();iPhi<hvManager_EMECPS->endPhiIndex();iPhi++) {
            EMECPresamplerHVModuleConstLink hvMod = hvManager_EMECPS->getHVModule(iSide,iPhi);
            double hv[2];
            for (int iGap=0;iGap<2;iGap++) {
                const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), hvMod->hvLineNo(iGap));
                if(itrLine == hvlineidx.end()) { // error, could not find HVline index
                  ATH_MSG_ERROR("Do not have hvline: "<<hvMod->hvLineNo(iGap)<<" in LArHVData !!!");
                  return StatusCode::FAILURE;
                }
		hv[iGap]=fabs(voltage[itrLine - hvlineidx.begin()]);
            }
            float eta_min=hvMod->getEtaMin(); 
            float eta_max=hvMod->getEtaMax();
            float phi_min=CaloPhiRange::fix(hvMod->getPhiMin());
            float phi_max=CaloPhiRange::fix(hvMod->getPhiMax());
            ATH_MSG_DEBUG("iSide,iPhi" << iSide << " " << iPhi << "  HV " <<  hv[0] << " " << hv[1] << " "
                          << "  etamin,etamax,phimin,phimax " << eta_min << " " << eta_max << " " 
                          << phi_min << " " << phi_max);

            //take decisions according to all the gaps HV :
            bool isDead=false;
            if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
            bool isAffected=false;
            if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
            ATH_MSG_DEBUG(" dead/affected " << isDead << " " << isAffected);

            if (isDead) {
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,4,4,CaloAffectedRegionInfo::HVdead);
              vAffected->push_back(current_CaloAffectedRegionInfo);             
            }
            if (isAffected) {
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,4,4,CaloAffectedRegionInfo::HVaffected);
              vAffected->push_back(current_CaloAffectedRegionInfo);
            }
      }        // loop over iphi EMECPS
    }    // lop over EMECPS side
  } else {
     ATH_MSG_ERROR("DO not have MEC HV manager !");
     return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=========================================================================================
StatusCode LArHVCondAlg::searchNonNominalHV_EMEC_INNER(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const { // deals with LAr HV, EM EndCap INNER
  const LArHVManager *manager = nullptr;

  ATH_MSG_DEBUG(" start loop over EMEC_INNER ");
  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {

    const EMECHVManager* hvManager_EMEC=manager->getEMECHVManager(EMECHVModule::INNER);

    for (unsigned int iSide=hvManager_EMEC->beginSideIndex();iSide<hvManager_EMEC->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMEC->beginPhiIndex();iPhi<hvManager_EMEC->endPhiIndex();iPhi++) {
	for (unsigned int iSector=hvManager_EMEC->beginSectorIndex();iSector<hvManager_EMEC->endSectorIndex();iSector++) {
	  for (unsigned int iEta=hvManager_EMEC->beginEtaIndex();iEta<hvManager_EMEC->endEtaIndex();iEta++) {
	    EMECHVModuleConstLink hvMod=hvManager_EMEC->getHVModule(iSide,iEta,iPhi,iSector);

            float etaMod = 0.5*(fabs(hvMod->getEtaMin())+fabs(hvMod->getEtaMax()));
            float HVnominal = HV_nominal("EMEC",etaMod);

            float eta_min = hvMod->getEtaMin();
            float eta_max = hvMod->getEtaMax();

            ATH_MSG_DEBUG("iSide,iPhi,iSector,iEta " << iSide << " " << iPhi << " " << iSector << " " 
                          << iEta << " eta_min , eta_max " << eta_min << " " << eta_max);

	    float phi_min=+30.,phi_max=-30.;

	    bool are_previous_HV_affected=0;
	    bool are_previous_HV_dead=0;
	    for (unsigned int ielec=0;ielec<hvMod->getNumElectrodes();ielec++) { 
	      EMECHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);

	      double hv[2];
	      for (unsigned int iGap=0;iGap<2;iGap++) { 
                  const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), electrode->hvLineNo(iGap));
                  if(itrLine == hvlineidx.end()) { // error, could not find HVline index
                     ATH_MSG_ERROR("Do not have hvline: "<<electrode->hvLineNo(iGap)<<" in LArHVData !!!");
                     return StatusCode::FAILURE;
                  }
		  hv[iGap]=voltage[itrLine - hvlineidx.begin()];
	      } //end for iGap

	      //------------------
	      //take decisions according to all the gaps HV :
              bool isDead=false;
              if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
              bool isAffected=false;
              if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
              ATH_MSG_DEBUG(" electrode HV " << ielec << " " << electrode->getPhi() << " "
                            << hv[0] << " " << hv[1] << " " << " isDead, isAffected " 
                            << isDead << " " << isAffected);
          
              // end previous dead region
              if (are_previous_HV_dead && !isDead) {
                 are_previous_HV_dead=false;
                 ATH_MSG_DEBUG(" -- end dead region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max);
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVdead);
                 vAffected->push_back(current_CaloAffectedRegionInfo);
              }

              // end previous affected region
              if (are_previous_HV_affected && !isAffected) {
                 are_previous_HV_affected=false;
                 ATH_MSG_DEBUG(" -- end affected region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max);
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVaffected);
                 vAffected->push_back(current_CaloAffectedRegionInfo);
              }

              if (isDead) {
                 if (!are_previous_HV_dead) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  ATH_MSG_DEBUG(" -- start dead region " << phi_min << " " << phi_max);
                  are_previous_HV_dead = true;
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  ATH_MSG_DEBUG("  extend dead region " << phi_min << " " << phi_max);
                 }
              }

              if (isAffected) {
                 if (!are_previous_HV_affected) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  are_previous_HV_affected = true;
                  ATH_MSG_DEBUG(" -- start affected region " << phi_min << " " << phi_max);
                 }
                 else {
                   extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                   ATH_MSG_DEBUG("  extend affected region " << phi_min << " " << phi_max);
                 }
              }

	    } // end for ielec
	    
	    if (are_previous_HV_affected) { 
              //in case a non nominal exists, stores it if we finish the 32 electrodes 
              //(because else the are_previous_HV_affected will be reinitialized 
              //for the next 32 electrodes serie )
              ATH_MSG_DEBUG("   - finish affected region after electrode loop " << eta_min << " " 
                            << eta_max << " " << phi_max << " " <<phi_max);
	      CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVaffected);
	      vAffected->push_back(current_CaloAffectedRegionInfo);	      
	    }
            if (are_previous_HV_dead) {
              ATH_MSG_DEBUG("    - end dead region after electrode loop " << eta_min << " " << eta_max << " " << phi_max << " " <<phi_max);
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVdead);
              vAffected->push_back(current_CaloAffectedRegionInfo);
            }
	  } // end for iEta
	} // end for iSector
      } // end for iPhi
    } //end for iSide
  } else {
     ATH_MSG_ERROR("DO not have EMEC HV manager ");
     return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=========================================================================================
StatusCode LArHVCondAlg::searchNonNominalHV_HEC(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const { // deals with LAr HV, HEC
  
  ATH_MSG_DEBUG(" in HEC ");
  const LArHVManager *manager = nullptr;
  float etamax_layer[4]={3.3,3.1,3.1,3.3};
  float etamin_layer[4]={1.5,1.5,1.6,1.7};
 
  float HVnominal = HV_nominal("HEC",0.);


  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
  
    const HECHVManager* hvManager_HEC=manager->getHECHVManager();
    
    for (unsigned int iSide=hvManager_HEC->beginSideIndex();iSide<hvManager_HEC->endSideIndex();iSide++) { // loop over HV modules      
      for (unsigned int iPhi=hvManager_HEC->beginPhiIndex();iPhi<hvManager_HEC->endPhiIndex();iPhi++) {
	for (unsigned int iSampling=hvManager_HEC->beginSamplingIndex();iSampling<hvManager_HEC->endSamplingIndex();iSampling++) {
          float eta_min,eta_max;
          if (iSide==1) {
           eta_min = etamin_layer[iSampling];
           eta_max = etamax_layer[iSampling];
          } else {
           eta_min = -1.*etamax_layer[iSampling];
           eta_max = -1.*etamin_layer[iSampling];
         }

	  HECHVModuleConstLink hvMod = hvManager_HEC->getHVModule(iSide,iPhi,iSampling);
          ATH_MSG_DEBUG(" iSide,iPhi,iSampling " << iSide << " " << iPhi << " " << iSampling);

	  double hv[4] = {0}; // 4 subgaps in HEC
	  for (unsigned int iGap=0;iGap<hvMod->getNumSubgaps();iGap++) {
	    HECHVSubgapConstLink subgap=hvMod->getSubgap(iGap);
            const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), subgap->hvLineNo());
            if(itrLine == hvlineidx.end()) { // error, could not find HVline index
              ATH_MSG_ERROR("Do not have hvline: "<<subgap->hvLineNo()<<" in LArHVData !!!");
              return StatusCode::FAILURE;
            }
	    if(iGap<4) hv[iGap]=voltage[itrLine - hvlineidx.begin()];
	  }// end for iGap

	  //------------------
          //take decisions according to all the gaps HV :
          bool isDead=false;
          if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD  && fabs(hv[2])<DEAD_HV_THRESHOLD && fabs(hv[3])<DEAD_HV_THRESHOLD) isDead=true;
          bool isAffected=false;
          if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || 
                           (fabs(hv[2]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[3]-HVnominal)>HV_NON_NOMINAL_TOLERANCE)) ) isAffected=true;
          ATH_MSG_DEBUG(" HV values " << hv[0] << " " << hv[1] << " " << hv[2] << " " << hv[3] << " " 
                        << " isDead/isAffected " << isDead << " " << isAffected);

          float phiMin = CaloPhiRange::fix(hvMod->getPhiMin());
          float phiMax = CaloPhiRange::fix(hvMod->getPhiMax());


	  if (isDead) { //stores it, DEAD means all hvs < threshold
            ATH_MSG_DEBUG(" new dead region " << eta_min << " " << eta_max << " " << phiMin << " " << phiMax << " layer " << 8+iSampling);
	    CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phiMin,phiMax,8+iSampling,8+iSampling,CaloAffectedRegionInfo::HVdead);
	    vAffected->push_back(current_CaloAffectedRegionInfo);
	  }
          if (isAffected) {
            ATH_MSG_DEBUG(" new affected region " << eta_min << " " << eta_max << " " << phiMin << " " << phiMax << " layer " << 8+iSampling);
            CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phiMin,phiMax,8+iSampling,8+iSampling,CaloAffectedRegionInfo::HVaffected);
            vAffected->push_back(current_CaloAffectedRegionInfo);
          }

	} //end for iSampling
      }//end for iPhi
    }//end for iSide
  } else {
     ATH_MSG_ERROR("Do not have HEC HV manager ");
     return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=========================================================================================
StatusCode LArHVCondAlg::searchNonNominalHV_FCAL(CaloAffectedRegionInfoVec *vAffected, const std::vector<float> &voltage, const std::vector<unsigned int> &hvlineidx) const { // deals with LAr HV, FCAL

  ATH_MSG_DEBUG( " inFCAL ");
  const LArHVManager *manager = nullptr;
  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {  
    
    const FCALHVManager *hvManager_FCAL=manager->getFCALHVManager();
    
    for (unsigned int iSide=hvManager_FCAL->beginSideIndex();iSide<hvManager_FCAL->endSideIndex();iSide++) { // loop over HV modules
      float eta_min=3.1,eta_max=4.9;
      if (iSide==0) {
         eta_min=-4.9;
         eta_max=-3.1;
      }
      for (unsigned int iSampling=hvManager_FCAL->beginSamplingIndex();iSampling<hvManager_FCAL->endSamplingIndex();iSampling++) {
        float HVnominal = HV_nominal("FCAL",(float)(iSampling));
	for (unsigned int iSector=hvManager_FCAL->beginSectorIndex(iSampling);iSector<hvManager_FCAL->endSectorIndex(iSampling);iSector++) {

	  FCALHVModuleConstLink hvMod = hvManager_FCAL->getHVModule(iSide,iSector,iSampling);
          ATH_MSG_DEBUG(" FCAL HVModule side,sampling,sector " << iSide << " " << iSampling << " " 
                        << iSector << "   HV nominal " << HVnominal);
 
          float dphi=CaloPhiRange::twopi()/16;
          if (iSampling==1) dphi=CaloPhiRange::twopi()/8.;
          if (iSampling==2) dphi=CaloPhiRange::twopi()/4.;
          float phi_min = ((float)(iSector))*dphi;
          phi_min =   CaloPhiRange::fix(phi_min);
          float phi_max = CaloPhiRange::fix(dphi+phi_min);
        
          ATH_MSG_DEBUG(" eta_min,eta_max,phi_min,phi_max " << eta_min << " " << eta_max << " " << phi_min 
                        << " " << phi_max << "   number of lines " << hvMod->getNumHVLines());
          float hv[4] = {0};
	  for (unsigned int iLine=0;iLine<hvMod->getNumHVLines();iLine++) {
	    FCALHVLineConstLink hvline = hvMod->getHVLine(iLine);
            const std::vector<unsigned int>::const_iterator itrLine=std::find(hvlineidx.begin(), hvlineidx.end(), hvline->hvLineNo());
            if(itrLine == hvlineidx.end()) { // error, could not find HVline index
              ATH_MSG_ERROR("Do not have hvline: "<<hvline->hvLineNo()<<" in LArHVData !!!");
              return StatusCode::FAILURE;
            }
	    if (iLine<4) hv[iLine] = voltage[itrLine - hvlineidx.begin()];
          }
	  //------------------
	  //take decisions according to all the gaps HV :
          bool isDead=false;
          if (fabs(hv[0]) < DEAD_HV_THRESHOLD && fabs(hv[1]) < DEAD_HV_THRESHOLD && fabs(hv[2]) < DEAD_HV_THRESHOLD && fabs(hv[3]) < DEAD_HV_THRESHOLD) isDead=true;
          bool isAffected=false;
          if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) ||
                           (fabs(hv[2]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[3]-HVnominal)>HV_NON_NOMINAL_TOLERANCE)) ) isAffected=true;
          ATH_MSG_DEBUG(" HV values " << hv[0] << " " << hv[1] << " " << hv[2] << " " << hv[3] <<  " " 
                        << " isDead/isAffected " << isDead << " " << isAffected);

	    
	  if (isAffected) { 
            ATH_MSG_DEBUG(" -- store affected region ");
	    CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,21+iSampling,21+iSampling,CaloAffectedRegionInfo::HVaffected);
	    vAffected->push_back(current_CaloAffectedRegionInfo);	      
	  }
          if (isDead) { 
            ATH_MSG_DEBUG(" -- store dead region ");
            CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,21+iSampling,21+iSampling,CaloAffectedRegionInfo::HVdead);
            vAffected->push_back(current_CaloAffectedRegionInfo);
          }
	} //end iSector
      } //end iSampling
    }// end iSide
  } else {
     ATH_MSG_ERROR("Do not have FCAL HV manager");
     return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=========================================================================================
StatusCode LArHVCondAlg::updateMethod(CaloAffectedRegionInfoVec *vAffected, const LArBadFebCont* bfCont, const LArOnOffIdMapping* cabling) const { //store informations on the missing Febs w/ range of eta, phi, layer
  ATH_MSG_INFO ( "updateMethod()" );
  
  std::vector<HWIdentifier>::const_iterator febid_it=m_onlineID->feb_begin();
  std::vector<HWIdentifier>::const_iterator febid_end_it=m_onlineID->feb_end();

  for (;febid_it!=febid_end_it;++febid_it) {
      
    bool IsMissingFeb=(bfCont->status(*febid_it).deadAll() || bfCont->status(*febid_it).deadReadout());
    
    if (IsMissingFeb) {       //flag for special treatment for FEB that has non contiguous eta regions, so we have to separate them
      bool is_normal=0; //FEB without discontinuity
      bool is_additive1=0; //FEB with a discontinuity
      
      int layer_min=+30,layer_max=-30;
      float eta_min=+30,eta_max=-30;
      float phi_min=+30,phi_max=-30;
      
      int layer_min_additive1=+30,layer_max_additive1=-30;
      float eta_min_additive1=+30,eta_max_additive1=-30;
      float phi_min_additive1=+30,phi_max_additive1=-30;
      
      int chans_per_feb = m_onlineID->channelInSlotMax(*febid_it);
      
      for (int icha=0;icha<chans_per_feb;icha++) {   //loop on each channel of the relevant FEB
	HWIdentifier channelId=m_onlineID->channel_Id(*febid_it,icha);

	if (cabling->isOnlineConnected(channelId)) {
	  Identifier offlineId=cabling->cnvToIdentifier(channelId);
	  const CaloDetDescrElement* caloddElement=m_calodetdescrmgr->get_element(offlineId);
	  
	  CaloCell_ID::CaloSample current_layer=caloddElement->getSampling(); // calo layer
	  float current_eta=caloddElement->eta();
	  float current_eta_low=caloddElement->eta()-0.5*caloddElement->deta();
	  float current_eta_high=caloddElement->eta()+0.5*caloddElement->deta();
	  float current_phi_low=caloddElement->phi()-0.5*caloddElement->dphi();
	  float current_phi_high=caloddElement->phi()+0.5*caloddElement->dphi();
	  
	  if (caloddElement->is_lar_em_barrel() && caloddElement->getLayer()==3 && fabs(current_eta)>0.79 && fabs(current_eta)<1.33) { //EMB, back sampling, slot 12 : special treatment : this FEB has non contiguous eta regions, so we have to separate them
	    is_additive1=1; //in order to add it to the summary only if it happens
	    
	    if (current_layer<layer_min_additive1)
	      layer_min_additive1=current_layer;
	    if (current_layer>layer_max_additive1)
	      layer_max_additive1=current_layer;
	    
	    if (current_eta_low<eta_min_additive1)
	      eta_min_additive1=current_eta_low;
	    if (current_eta_high>eta_max_additive1)
	      eta_max_additive1=current_eta_high;
	    
            extendPhiRegion(current_phi_low,phi_min_additive1,phi_max_additive1);
            extendPhiRegion(current_phi_high,phi_min_additive1,phi_max_additive1);
	    
	    //	    std::cout << "found the special case, icha=" << icha << ", etamin=" << eta_min_additive1 << ", current layer=" << current_layer << ", current eta=" << current_eta << std::endl;
	  }
	  else { //normal case
	    is_normal=1; // normal case
	    
	    if (current_layer<layer_min)
	      layer_min=current_layer;
	    if (current_layer>layer_max)
	      layer_max=current_layer;
	    
	    if (current_eta_low<eta_min)
	      eta_min=current_eta_low;
	    if (current_eta_high>eta_max)
	      eta_max=current_eta_high;
	    
            extendPhiRegion(current_phi_low,phi_min,phi_max);
            extendPhiRegion(current_phi_high,phi_min,phi_max);

	  }
	} //end of isOnlineConnected()
      } // end of loop on channels
      
      if (is_normal) {
	CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,layer_min,layer_max,CaloAffectedRegionInfo::missingReadout);
	vAffected->push_back(current_CaloAffectedRegionInfo);
      }
      
      if (is_additive1) {
	CaloAffectedRegionInfo current_additive1_CaloAffectedRegionInfo(eta_min_additive1,eta_max_additive1,phi_min_additive1,phi_max_additive1,layer_min_additive1,layer_max_additive1,CaloAffectedRegionInfo::missingReadout);
	vAffected->push_back(current_additive1_CaloAffectedRegionInfo);
      }
    } // end of isMissingFeb  
  } // end of loop on Febs  
  return StatusCode::SUCCESS;
}
//====================================================================================
float LArHVCondAlg::HV_nominal(const char *identification,const float myparameter) const
{
  if (strcmp(identification,"EMBPS")==0) 
    return 1200.;
  else if (strcmp(identification,"EMECPS")==0)
    return 1600.;
  else if (strcmp(identification,"EMB")==0)
    return 2000.;
  else if (strcmp(identification,"EMEC")==0) {
    if ( fabs(myparameter)<1.5 )
      return 2500.;
    else if (fabs(myparameter)<1.6)
      return 2300.;
    else if (fabs(myparameter)<1.8 )
      return 2100.;
    else if ( fabs(myparameter) < 2.0 )
      return 1700.;
    else if ( fabs(myparameter) < 2.1 )
      return 1500.;
    else if ( fabs(myparameter) < 2.3 )
      return 1250.;
    else if ( fabs(myparameter) < 2.5 )
      return 1000.;
    else if ( fabs(myparameter) < 2.8 )
      return 2300.;
    else return 1800.;
  }
  else if (strcmp(identification,"HEC")==0) {
    return 1800.;
  }
  else if (strcmp(identification,"FCAL")==0) {
    if (myparameter<0.5)
      return 250.;
    else if (myparameter<1.5)
      return 375.;
    else if (myparameter<2.5)
      return 500.;
  }

  return -1;
}
//=========================================================================================

void LArHVCondAlg::extendPhiRegion(float phi, float & phi_min, float & phi_max) const {

  static float epsilon=1e-4;
  
  phi = CaloPhiRange::fix(phi);

  if (phi_min>10. || phi_max<-10.) {
     phi_min = CaloPhiRange::fix(phi-epsilon);
     phi_max = CaloPhiRange::fix(phi+epsilon);
     return;
  }

  bool isInRegion=false;
  if (phi_min<phi_max) {
     if (phi>phi_min && phi<phi_max) isInRegion=true;
  }
  else {
     if (phi>phi_min || phi<phi_max) isInRegion=true;
  }
  if (isInRegion) return;

  float dphi1 = CaloPhiRange::diff(phi,phi_min); 
  float dphi2 = CaloPhiRange::diff(phi,phi_max); 
  if (fabs(dphi1)<fabs(dphi2) )
     phi_min=phi;
  else
     phi_max=phi;
  
  return;

}
