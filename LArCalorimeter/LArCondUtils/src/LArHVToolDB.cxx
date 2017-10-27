/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCondUtils/LArHVToolDB.h" 
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArElecCalib/ILArHVPathologyDbTool.h"
#include "LArRecConditions/LArHVPathologiesDb.h"
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

#include "LArCabling/LArHVCablingTool.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

//#include <iostream> 
#include <cmath> 
#include <cstdlib>
#include "AthenaKernel/errorcheck.h"


const unsigned LArHVToolDB::m_nHVCoolChannels=453+4384;

// constructor 
LArHVToolDB::LArHVToolDB(const std::string& type,
			 const std::string& name,
			 const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_larem_id(nullptr),
    m_larhec_id(nullptr),
    m_larfcal_id(nullptr),
    m_electrodeID(nullptr),
    m_hvLineID(nullptr),
    m_pathologyTool("LArHVPathologyDbTool"),
    m_hvCablingTool("LArHVCablingTool"),
    m_pathologyContainer(nullptr)
 {
 
  declareInterface< ILArHVTool >( this );
  m_DCSFolderNames.push_back(std::string("/LAR/DCS/HV/BARREl/I16"));
  m_DCSFolderNames.push_back(std::string("/LAR/DCS/HV/BARREL/I8"));
  declareProperty("DCSFolder",m_DCSFolderNames,"LAr HV DCS Folders");
  declareProperty("HVPathologiesFolder",m_HVPathologiesFolderName="/LAR/HVPathologiesOfl/Pathologies","LAr HV Pathologies Folder");
}

// destructor 
LArHVToolDB::~LArHVToolDB()
{ }

StatusCode LArHVToolDB::LoadCalibration(IOVSVC_CALLBACK_ARGS_K( keys)) {

  std::set<size_t> DCSfolderIndices;
  bool doPathology=false;
  for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {
    ATH_MSG_DEBUG("Processing callback for SG key " << *itr);
    if (*itr == m_HVPathologiesFolderName) {
       doPathology=true;
    }
    else if(*itr == "/LAR/IdentifierOfl/HVLineToElectrodeMap") { //FIXME hardcoded foldername .The name is actually a jobO of LArHVCablingTool
      ATH_MSG_INFO( "Received a callback on HV Cable Map. Need to (re)laod all HV DCS folders" );
      for(size_t i=0;i<m_DCSFolderNames.size();++i) {
	DCSfolderIndices.insert(i);
	m_voltageCache[i].clear(); //Clear voltage cache
      }
    }
    else {
       for(size_t i=0;i<m_DCSFolderNames.size();++i) {
	 if (*itr==m_DCSFolderNames[i]) {
	   DCSfolderIndices.insert(i);
	   m_nUpdatesPerFolder[i].m_nCallbacks++;
	 }
       }
     }
  }
  if (DCSfolderIndices.size()) {
    ATH_MSG_DEBUG("Received a callback for " << DCSfolderIndices.size() << " HV folders.");
    if (fillUpdatedHVChannelsVec(DCSfolderIndices).isFailure()) {
      ATH_MSG_ERROR( "Call to fillUpdatedHVChannelsVec failed." );
      return StatusCode::FAILURE;
    }
  }
  if (m_updatedElectrodes.size()>0) {
    const LArHVManager *manager = NULL; 
    if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
      manager->reset();
    }
  }

  if (doPathology) {
    ATH_MSG_INFO( " LArHVToolDB::LoadCalibration: Load HV pathology database " );
    if (m_pathologyContainer) delete m_pathologyContainer;
    if (detStore()->contains<AthenaAttributeList>(m_HVPathologiesFolderName)) {
      if (m_pathologiesHandle.isValid()) {
	m_pathologyContainer = m_pathologyTool->attrList2HvPathology((*m_pathologiesHandle));
          ATH_MSG_INFO( " Number of HV pathologies found " << m_pathologyContainer->m_v.size() );
          for(unsigned i=0; i<m_pathologyContainer->m_v.size(); ++i) {
         
              LArHVPathologiesDb::LArHVElectPathologyDb electPath = m_pathologyContainer->m_v[i];
              Identifier id(electPath.cellID);
              if (m_larem_id->is_lar_em(id)) {
                  IdentifierHash idHash = m_larem_id->channel_hash(id);
                  unsigned int index = (unsigned int)(idHash);
                  if (index<m_hasPathologyEM.size()) {
                     if(m_hasPathologyEM[index].size()) {
                       if(m_hasPathologyEM[index].size()<static_cast<size_t>(abs(electPath.electInd+1)))
                         m_hasPathologyEM[index].resize(electPath.electInd+1);
                        m_hasPathologyEM[index][electPath.electInd]=electPath.pathologyType;
                     } else {
                        std::vector<unsigned short> svec;
                        svec.resize(electPath.electInd+1);
                        svec[electPath.electInd]=electPath.pathologyType;
                        m_hasPathologyEM[index]=svec;
                     }
                  }
              }
              if (m_larhec_id->is_lar_hec(id)) {
                  IdentifierHash idHash = m_larhec_id->channel_hash(id);
                  unsigned int index = (unsigned int)(idHash);
                  if (index<m_hasPathologyHEC.size()) {
                     if(m_hasPathologyHEC[index].size()) {
                       if(m_hasPathologyHEC[index].size()<static_cast<size_t>(abs(electPath.electInd+1)))
                         m_hasPathologyHEC[index].resize(electPath.electInd+1);
                        m_hasPathologyHEC[index][electPath.electInd]=electPath.pathologyType;
                     } else {
                        std::vector<unsigned short> svec;
                        svec.resize(electPath.electInd+1);
                        svec[electPath.electInd]=electPath.pathologyType;
                        m_hasPathologyHEC[index]=svec;
                     }
                  }
              }
              if (m_larfcal_id->is_lar_fcal(id)) {
                  IdentifierHash idHash = m_larfcal_id->channel_hash(id);
                  unsigned int index = (unsigned int)(idHash);
                  if (index<m_hasPathologyFCAL.size()) {
                     if(m_hasPathologyFCAL[index].size()) {
                       if(m_hasPathologyFCAL[index].size()<static_cast<size_t>(abs(electPath.electInd+1)))
                         m_hasPathologyFCAL[index].resize(electPath.electInd+1);
                       m_hasPathologyFCAL[index][electPath.electInd]=electPath.pathologyType;
                     } else {
                        std::vector<unsigned short> svec;
                        svec.resize(electPath.electInd+1);
                        svec[electPath.electInd]=electPath.pathologyType;
                        m_hasPathologyFCAL[index]=svec;
                     }

                  }
              }
          }
      }
    }
  }
 
  return StatusCode::SUCCESS;

}


StatusCode LArHVToolDB::finalize() {
  for (size_t i=0;i<m_nUpdatesPerFolder.size();++i) {
    ATH_MSG_INFO( "Folder: " << m_DCSFolderNames[i] 
		   << ": # of Callbacks:" << m_nUpdatesPerFolder[i].m_nCallbacks
		   << ", # of actual voltage changes:" << m_nUpdatesPerFolder[i].m_nVoltageUpdates );
  }
  if (m_pathologyContainer) delete m_pathologyContainer;
  return StatusCode::SUCCESS;
}


// intialize 
StatusCode LArHVToolDB::initialize(){
// retrieve LArEM id helpers
  CHECK(detStore()->retrieve(m_caloIdMgr));

  m_larem_id   = m_caloIdMgr->getEM_ID();
  m_larhec_id   = m_caloIdMgr->getHEC_ID();
  m_larfcal_id   = m_caloIdMgr->getFCAL_ID();

//  retrieve CaloDetDescrMgr 
  CHECK(detStore()->retrieve(m_calodetdescrmgr));

  // retrieve the LArElectrodeID helper
  CHECK(detStore()->retrieve(m_electrodeID));

  //retrieve the LArHVLineID helper
  CHECK(detStore()->retrieve(m_hvLineID));

// register callback to Database folder
  m_voltageCache.resize(m_DCSFolderNames.size());
  m_nUpdatesPerFolder.resize(m_DCSFolderNames.size());
  for (size_t i=0;i<m_DCSFolderNames.size();++i) {
    const DataHandle<CondAttrListCollection> cHdl;
    if ((detStore()->regFcn(&ILArHVTool::LoadCalibration,dynamic_cast<ILArHVTool*>(this),cHdl,m_DCSFolderNames[i])).isFailure()) {
      ATH_MSG_ERROR( "cannot register callback to " << m_DCSFolderNames[i] );  
    }
    else
      ATH_MSG_INFO( "Registered callback to DCS folder " << m_DCSFolderNames[i] );
  }

  m_pathologyContainer = 0;
  m_hasPathologyEM.resize(m_larem_id->channel_hash_max());
  m_hasPathologyHEC.resize(m_larhec_id->channel_hash_max());
  m_hasPathologyFCAL.resize(m_larfcal_id->channel_hash_max());

  if (detStore()->contains<AthenaAttributeList>( m_HVPathologiesFolderName)) {
    StatusCode sc=detStore()->regFcn(&ILArHVTool::LoadCalibration,dynamic_cast<ILArHVTool*>(this),m_pathologiesHandle,m_HVPathologiesFolderName);
    if (sc.isFailure())
      ATH_MSG_ERROR( "cannot register callback to " << m_HVPathologiesFolderName );
    else
      ATH_MSG_INFO( "registered callback to " << m_HVPathologiesFolderName  );
  }
  else
    ATH_MSG_INFO( " no information for HV pathology available " );


  // Get HVPathology tool
  StatusCode sc = m_pathologyTool.retrieve();
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR( "Unable to initialize LArHVPathologyDbTool" );
    return sc;
  }
  else {
    ATH_MSG_INFO( "Retrieved LArHVPathologyDbTool"  );
  }


  sc=m_hvCablingTool.retrieve();
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR( "Unable to retrieve LArHVCablingTool" );
    return sc;
  }
  

  sc=detStore()->regFcn(&LArHVCablingTool::iovCallback,&(*m_hvCablingTool),
			&ILArHVTool::LoadCalibration,dynamic_cast<ILArHVTool*>(this));
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR( "Unable to register callback on LArHVCablingTool::iovCallback" );
    return sc;
  }
  else
    ATH_MSG_INFO( "registered callback to LArHVCablingTool" );
  return StatusCode::SUCCESS;
}


StatusCode LArHVToolDB::getHV(const Identifier& id,
         std::vector< HV_t > & v  )  
{
 std::vector< CURRENT_t> ihv;
 return getPayload(id,v,ihv);
}

StatusCode LArHVToolDB::getCurrent(const Identifier&  id,
         std::vector< CURRENT_t > & ihv  ) 
{
 std::vector< HV_t> v;  
 return getPayload(id,v,ihv);
}


StatusCode LArHVToolDB::getPayload(const Identifier& id, std::vector< HV_t > & v, std::vector< CURRENT_t > & ihv) const
{

  if (id==m_id) {
     v = m_v;
     ihv = m_i;
     return StatusCode::SUCCESS;
  }

  m_id = id;

  v.clear();
  ihv.clear();

  std::vector<unsigned int> listElec;

// check identifier in LAR
  if (m_larem_id->is_lar_em(id) && m_larem_id->sampling(id) > 0) {
// LAr EMB
     if (abs(m_larem_id->barrel_ec(id))==1) {
        //std::cout << " in barrel " << m_larem_id->show_to_string(id) << std::endl;
        unsigned int index = (unsigned int)(m_larem_id->channel_hash(id));
        bool hasPathology=false; 
        if (index<m_hasPathologyEM.size()) {
         if (m_hasPathologyEM[index].size()) {
          hasPathology=true;
          listElec = getElecList(id);
         }
        }
        const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
        if (!embElement) std::abort();
        const EMBCellConstLink cell = embElement->getEMBCell();
        unsigned int nelec = cell->getNumElectrodes();
        //std::cout << " nelec " << nelec << std::endl;
        unsigned int ngap = 2*nelec;
        double wt = 1./ngap;
        for (unsigned int i=0;i<nelec;i++) {
            const EMBHVElectrodeConstLink electrode = cell->getElectrode(i);
            //std::cout << "electrode: endcap index, eta index , phi index, sector index , electrode index " << electrode->getModule()->getSideIndex() <<
            //   " " << electrode->getModule()->getEtaIndex() << " " << electrode->getModule()->getPhiIndex() << 
            //   " " << electrode->getModule()->getSectorIndex() << " " << electrode->getElectrodeIndex() << std::endl;
            for (unsigned int igap=0;igap<2;igap++) {
                double hv;
                double curr;
                electrode->voltage_current(igap,hv,curr);
                if (hasPathology) {
                   ATH_MSG_DEBUG( "Has pathology for id: "<< m_larem_id->print_to_string(id)<<" "<<m_hasPathologyEM[index]);
                   ATH_MSG_DEBUG( "Original hv: "<<hv<<" ");
                   for (unsigned int ii=0;ii<listElec.size();ii++) {
                      if (listElec[ii]==(2*i+igap) && listElec[ii]<m_hasPathologyEM[index].size() && m_hasPathologyEM[index][listElec[ii]]) {
                         if(m_hasPathologyEM[index][listElec[ii]]&0xF) hv=0.; else hv=((m_hasPathologyEM[index][listElec[ii]]&0xFFF0)>>4);
                         curr=0.;
                      }
                   }
                   ATH_MSG_DEBUG( "set hv: "<<hv);
                }
                //std::cout << "     hv value " << hv << std::endl;
                //if (igap==1 && hv>1.) std::cout << " --- non zero value found for gap1 in barrel " << std::endl;
                addHV(v,hv,wt);
                addCurr(ihv,curr,wt);
               
            }
        }        
     }
// LAr EMEC
     else {
        //std::cout << " in EMEC " << m_larem_id->show_to_string(id) << std::endl;
        unsigned int index = (unsigned int)(m_larem_id->channel_hash(id));
        bool hasPathology=false;
        if (index<m_hasPathologyEM.size()) {
         if (m_hasPathologyEM[index].size()) {
          hasPathology=true;
          listElec = getElecList(id);
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
               double hv;
               double curr;
               electrode->voltage_current(igap,hv,curr);
                if (hasPathology) {
                   ATH_MSG_DEBUG( "Has pathology for id: "<< m_larem_id->print_to_string(id)<<" "<<m_hasPathologyEM[index] );
                   for (unsigned int ii=0;ii<listElec.size();ii++) {
                      if (listElec[ii]==(2*i+igap) && listElec[ii]<m_hasPathologyEM[index].size() && m_hasPathologyEM[index][listElec[ii]]) {
                         if(m_hasPathologyEM[index][listElec[ii]]&0xF) hv=0.; else hv=((m_hasPathologyEM[index][listElec[ii]]&0xFFF0)>>4);
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

     }
  }
// LAr HEC
  else if (m_larhec_id->is_lar_hec(id)) {
      //std::cout << " in HEC " << std::endl;
      unsigned int index = (unsigned int)(m_larhec_id->channel_hash(id));
      bool hasPathology=false;
      if (index<m_hasPathologyHEC.size()) {
       if (m_hasPathologyHEC[index].size()) {
        hasPathology=true;
        listElec = getElecList(id);
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
          double hv;
          double curr;
          subgap->voltage_current(hv,curr);
          //std::cout << "     hv value " << hv << std::endl;
          if (hasPathology) {
             ATH_MSG_DEBUG( "Has pathology for id: "<< m_larhec_id->print_to_string(id)<<" "<<m_hasPathologyHEC[index] );
             for (unsigned int ii=0;ii<listElec.size();ii++) {
                if (listElec[ii]==i && listElec[ii]<m_hasPathologyHEC[index].size() && m_hasPathologyHEC[index][listElec[ii]]) {
                     if(m_hasPathologyHEC[index][listElec[ii]]&0xF) hv=0.; else hv=((m_hasPathologyHEC[index][listElec[ii]]&0xFFF0)>>4);
                     curr=0.;
                }
             }
          }

          addHV(v,hv,wt);
          addCurr(ihv,curr,wt);
      }

  }
// LAr FCAL
  else if (m_larfcal_id->is_lar_fcal(id)) {
      //std::cout << " in FCAL " << std::endl;
      unsigned int index = (unsigned int)(m_larfcal_id->channel_hash(id));
      bool hasPathology=false;
      if (index<m_hasPathologyFCAL.size()) {
       if (m_hasPathologyFCAL[index].size()) {
        hasPathology=true;
        listElec = getElecList(id);
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
          //std::cout << " line " << line;
          double hv;
          double curr;
          line->voltage_current(hv,curr);
          if (hasPathology) {
             ATH_MSG_DEBUG( "Has pathology for id: "<< m_larfcal_id->print_to_string(id)<<" "<<m_hasPathologyFCAL[index] );
             for (unsigned int ii=0;ii<listElec.size();ii++) {
                if (listElec[ii]==i && listElec[ii]<m_hasPathologyFCAL[index].size() && m_hasPathologyFCAL[index][listElec[ii]]) {
                     if(m_hasPathologyFCAL[index][listElec[ii]]&0xF) hv=0.; else hv=((m_hasPathologyFCAL[index][listElec[ii]]&0xFFF0)>>4);
                     curr=0.;
                }
             }
          }
          //std::cout << "     hv value " << hv << std::endl;
          addHV(v,hv,wt);
          addCurr(ihv,curr,wt);
        }
      }

  }
// Presamplers
  else if (m_larem_id->is_lar_em(id) && m_larem_id->sampling(id)==0) {

     if (abs(m_larem_id->barrel_ec(id))==1) {

        const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
        if (!embElement) std::abort();
        const EMBCellConstLink cell = embElement->getEMBCell();

        const EMBPresamplerHVModuleConstLink hvmodule =  cell->getPresamplerHVModule ();

        double wt = 0.5;
        for (unsigned int igap=0;igap<2;igap++) {
            double hv;
            double curr;
            hvmodule->voltage_current(igap,hv,curr);
            addHV(v,hv,wt);
            addCurr(ihv,curr,wt);
        }

     }
     else {

        const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
        if (!emecElement) std::abort();
        const EMECCellConstLink cell = emecElement->getEMECCell();

        const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule ();

        double wt = 0.5; 
        for (unsigned int igap=0;igap<2;igap++) {
            double hv;
            double curr;
            hvmodule->voltage_current(igap,hv,curr);
            addHV(v,hv,wt);
            addCurr(ihv,curr,wt);
        }

     }
  }
  else {
    ATH_MSG_WARNING( " cell neither in EM nor HEC nor FCAL !!!!!  return empty HV " );
  }

  m_v = v;
  m_i = ihv;

  return StatusCode::SUCCESS; 
}

void LArHVToolDB::addHV(std::vector< HV_t > & v , double hv, double wt) const
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
           HV_t hh;
           hh.hv = hv;
           hh.weight = wt;
           v.push_back(hh);
         }     // not already in the list
}

void LArHVToolDB::addCurr(std::vector< CURRENT_t > & ihv , double current, double wt) const
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
           CURRENT_t ii;
           ii.current = current;
           ii.weight = wt;
           ihv.push_back(ii);
         }     // not already in the list
}

std::vector<unsigned int> LArHVToolDB::getElecList(const Identifier& id) const
{
    std::vector<unsigned int> myList;
    myList.clear();
    for(unsigned i=0; i<m_pathologyContainer->m_v.size(); ++i) {
      LArHVPathologiesDb::LArHVElectPathologyDb electPath = m_pathologyContainer->m_v[i];
      if (electPath.cellID == (unsigned int)(id.get_identifier32().get_compact())) {
          myList.push_back(electPath.electInd);
      }
    }
    return myList;

}



StatusCode LArHVToolDB::fillUpdatedHVChannelsVec(const std::set<size_t>& folderIndices) {
  m_updatedElectrodes.clear();

  std::vector<unsigned> updatedHVLines;
  //Loop over the list of DCS folders for with a callback has been fired
  std::set<size_t>::const_iterator fldrIt=folderIndices.begin();
  std::set<size_t>::const_iterator fldrIt_e=folderIndices.end();
  for(;fldrIt!=fldrIt_e;++fldrIt) {
    const size_t idx=*fldrIt;
    //Get the internal cache and the AttributeListCollection for this folder
    std::vector<std::pair<unsigned,float> >& voltageCache=m_voltageCache[idx];
    //if (!m_DCSHandles[idx].isValid()) {
    //  ATH_MSG_ERROR( "DataHandle for folder " << m_DCSFolderNames[idx] << " not valid!" );
    //  return StatusCode::FAILURE;
    //}
    const CondAttrListCollection* attrlist=NULL; //m_DCSHandles[idx].cptr();
    if (detStore()->retrieve(attrlist,m_DCSFolderNames[idx]).isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve CondAttrListCollection with key " <<  m_DCSFolderNames[idx] << "." );
      return StatusCode::FAILURE;
    }

    const size_t cacheSize=voltageCache.size();
    if (!cacheSize) {
      //First call, fill cache
      updatedHVLines.reserve(m_nHVCoolChannels);
      m_updatedElectrodes.reserve(m_electrodeID->electrodeHashMax());
      voltageCache.reserve(500);
      CondAttrListCollection::const_iterator citr=attrlist->begin(); 
      CondAttrListCollection::const_iterator citr_e=attrlist->end();
      for(;citr!=citr_e;++citr) {
	const unsigned chan=citr->first;
	updatedHVLines.push_back(chan); //For the first callback, every HV channel will be marked as 'updated'
	const coral::Attribute& attr=((citr)->second)["R_VMEAS"];
	float voltage=-999;
	if (!attr.isNull()) voltage=attr.data<float>(); //Ignore NULL values
	voltageCache.push_back(std::make_pair(chan,voltage));
      }//end loop over attributeListCollection
      ATH_MSG_DEBUG("Filling cache for folder: " << m_DCSFolderNames[idx] <<". " << voltageCache.size() << " Channels.");
    }//end if cachesize==0
    else {
      //Cache already filled    
      size_t cacheIdx=0;
      unsigned voltageHasChanged=0;
      if (cacheIdx+attrlist->size() > cacheSize) {
	ATH_MSG_ERROR( "Folder: " << m_DCSFolderNames[idx] 
			<< ": Got more COOL channels than in previous callback (" << cacheSize << " -> " << cacheIdx+attrlist->size() << ")." 
			);
	return StatusCode::FAILURE;
      }
      CondAttrListCollection::const_iterator citr=attrlist->begin(); 
      CondAttrListCollection::const_iterator citr_e=attrlist->end();
      for(;citr!=citr_e;++citr,++cacheIdx) {
	std::pair<unsigned,float>& currChanCache=voltageCache[cacheIdx];
	if (currChanCache.first != citr->first) {
	  ATH_MSG_ERROR( "Folder: " << m_DCSFolderNames[idx] 
			  << ": COOL channel in unexpected order! Got channel #" << citr->first << " expected #" <<  currChanCache.first 
			  );
	  return StatusCode::FAILURE;
	}
	const coral::Attribute& attr=((citr)->second)["R_VMEAS"];
	if (attr.isNull()) continue; //Ignore NULL values
	const float voltage=attr.data<float>();
	if (voltage!=currChanCache.second) {
	  updatedHVLines.push_back(currChanCache.first);
	  currChanCache.second=voltage;
	  voltageHasChanged=1;
	}
      }//end loop over AttibuteListCollection
      m_nUpdatesPerFolder[idx].m_nVoltageUpdates+=voltageHasChanged;
      ATH_MSG_DEBUG("Folder: " << m_DCSFolderNames[idx] << ": Found " << updatedHVLines.size() << " Channels with voltage changes so far.");
    }//end else
  }//end loop over folders


  //Build vector of electrode IDs:
  std::vector<unsigned>::const_iterator hvLineIt=updatedHVLines.begin();
  std::vector<unsigned>::const_iterator hvLineIt_e=updatedHVLines.end();
  for(;hvLineIt!=hvLineIt_e;++hvLineIt) {
    const unsigned node=(*hvLineIt)/1000;
    const unsigned line=(*hvLineIt)%1000;
    HWIdentifier hvlineid=m_hvLineID->HVLineId(1,1,node,line);
    const std::vector<HWIdentifier>& electrodeIdVec=m_hvCablingTool->getLArElectrodeIDvec(hvlineid);
    m_updatedElectrodes.insert(m_updatedElectrodes.end(),electrodeIdVec.begin(),electrodeIdVec.end());
  }

  ATH_MSG_DEBUG("Number of electrodes with voltage change:" << m_updatedElectrodes.size() << " (max=" << m_electrodeID->electrodeHashMax() << ")");
  return StatusCode::SUCCESS;
}
