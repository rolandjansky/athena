/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "./LArHVCondAlg.h" 
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandle.h"
#include "LArElecCalib/ILArHVPathologyDbTool.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArReadoutGeometry/EMBCell.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVSubgapConstLink.h"
#include "LArHV/HECHVSubgap.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArHV/FCALHVLineConstLink.h"
#include "LArHV/FCALHVLine.h"

#include "LArHV/LArHVManager.h"

#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

//#include <iostream> 
#include <cmath> 
#include <cstdlib>
#include "AthenaKernel/errorcheck.h"

#define VDIFF_MAX 0.01 // maximum voltage difference allowed to be treated as equal
#define WDIFF_MAX 0.0001 // maximum weight difference allowed to be treated as equal


// constructor 
LArHVCondAlg::LArHVCondAlg( const std::string& name, ISvcLocator* pSvcLocator )
  : AthReentrantAlgorithm(name,pSvcLocator),
    m_larem_id(nullptr),
    m_larhec_id(nullptr),
    m_larfcal_id(nullptr),
    m_electrodeID(nullptr),
    m_hvLineID(nullptr),
    m_condSvc("CondSvc",name)
 {
 
}

// destructor 
LArHVCondAlg::~LArHVCondAlg()
{ }

//initialize
StatusCode LArHVCondAlg::initialize(){
  ATH_CHECK(detStore()->retrieve(m_caloIdMgr));

  m_larem_id   = m_caloIdMgr->getEM_ID();
  m_larhec_id   = m_caloIdMgr->getHEC_ID();
  m_larfcal_id   = m_caloIdMgr->getFCAL_ID();

  ATH_CHECK(detStore()->retrieve(m_calodetdescrmgr));

  ATH_CHECK(detStore()->retrieve(m_electrodeID));

  ATH_CHECK(detStore()->retrieve(m_hvLineID));

  // Read Handles
  ATH_CHECK(m_pathologiesKey.initialize());
  ATH_CHECK(m_DCSFolderKeys.initialize());
  // Write Handle
  ATH_CHECK(m_hvDataKey.initialize());
  
  // Register write handle
  ATH_CHECK( m_condSvc.retrieve() );
  if (m_condSvc->regHandle(this, m_hvDataKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_hvDataKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}


StatusCode LArHVCondAlg::execute_r(const EventContext& ctx) const {

  ATH_MSG_DEBUG("executing");

  SG::WriteCondHandle<LArHVData> writeHandle{m_hvDataKey};

  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  } 

  // Define validity of the output cond object 
  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0, 0, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  EventIDRange rangeW{start, stop};
  std::vector<const CondAttrListCollection*> attrvec;
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

  }

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


  const LArHVManager *manager = NULL; 
  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
      manager->reset();
  }

  if (doPathology) {
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
     }
  } //doPathology
  // try to get old cond. object
  const LArHVData* hvdataOld = nullptr;
 
  std::unique_ptr<LArHVData> hvdata = std::make_unique<LArHVData>();
  LArHVData* p_hvdata = hvdata.get();

  // and now to call filling
  std::vector<float> voltage;
  std::vector<float> current;
  std::vector<unsigned int> hvlineidx;
  ATH_CHECK(fillUpdatedHVChannelsVec(voltage, current, hvlineidx, attrvec));
  ATH_CHECK(fillPayload(p_hvdata, hvdataOld, voltage, current, hvlineidx, pathologyContainer, hasPathologyEM, hasPathologyHEC, hasPathologyFCAL));


  const EventIDRange crangeW(rangeW);
  if(writeHandle.record(crangeW,hvdata.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record LArHVData object with " << writeHandle.key()
                  << " with EventRange " << crangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << crangeW << " into Conditions Store");

  
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
