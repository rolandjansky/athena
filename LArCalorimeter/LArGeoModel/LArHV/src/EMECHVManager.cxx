/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVElectrode.h"
#include <cmath>
#include <iostream>

#include "EMECHVPayload.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArCabling/LArHVCablingTool.h"

#ifndef SIMULATIONBASE
#ifndef GENERATIONBASE
#include "LArRecConditions/LArHVIdMapping.h"
#endif
#endif

#include "Identifier/HWIdentifier.h"

#include <atomic>

class EMECHVManager::Clockwork {
public:
  Clockwork(const EMECHVManager* manager, IOType wheel)
    : iWheel(wheel)
  {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iEta=0; iEta<8; ++iEta) {
	for(int iPhi=0; iPhi<8; ++iPhi) {
	  for(int iSector=0; iSector<8; ++iSector) {
	    moduleArray[iSide][iEta][iPhi][iSector] = new EMECHVModule(manager,wheel,iSide,iEta,iPhi,iSector);
	  }
	}
      }
    }

    ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "HECHVManager");
    if (StatusCode::SUCCESS!=detStore->retrieve(elecId, "LArElectrodeID")) {
      throw std::runtime_error("EMECHVManager failed to retrieve LArElectrodeID");
    }

    if (StatusCode::SUCCESS!=detStore->retrieve(hvId,"LArHVLineID")) {
      throw std::runtime_error("EMECHVManager failed to retrieve LArHVLineID");
    }
  }
  ~Clockwork() {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iEta=0; iEta<8; ++iEta) {
	for(int iPhi=0; iPhi<8; ++iPhi) {
          for(int iSector=0; iSector<8; ++iSector) {
	    delete moduleArray[iSide][iEta][iPhi][iSector];
	  }
	}
      }
    }
    delete descriptor;
  }
  EMECHVDescriptor* descriptor{nullptr};
  IOType iWheel;
  const EMECHVModule*        moduleArray[2][8][8][8]; // not dense
  std::atomic<bool>          init{false};
  std::mutex                 mtx;
  std::vector<EMECHVPayload> payloadArray;
  const LArElectrodeID* elecId;
  const LArHVLineID* hvId;
};


EMECHVManager::EMECHVManager(IOType wheel)
   : m_c(new Clockwork(this,wheel))
{
  if (wheel==EMECHVModule::OUTER) {
    CellPartitioning etaBinning;
    etaBinning.addValue(1.375);
    etaBinning.addValue(1.50);
    etaBinning.addValue(1.6);
    etaBinning.addValue(1.8);
    etaBinning.addValue(2.0);
    etaBinning.addValue(2.1);
    etaBinning.addValue(2.3);
    etaBinning.addValue(2.5);
    m_c->descriptor = new EMECHVDescriptor(etaBinning,CellBinning(0.0, 2*M_PI, 8),CellBinning(0,M_PI/4.0,4));
  }
  else {
    CellPartitioning etaBinning;
    etaBinning.addValue(2.5);
    etaBinning.addValue(2.8);
    etaBinning.addValue(3.2);
    m_c->descriptor=new EMECHVDescriptor(etaBinning,CellBinning(0.0, 2*M_PI, 8),CellBinning(0,M_PI/4.0,8));
  }
}

const EMECHVDescriptor& EMECHVManager::getDescriptor() const
{
  return *(m_c->descriptor);
}

unsigned int EMECHVManager::beginPhiIndex() const
{
  return m_c->descriptor->getPhiBinning().getFirstDivisionNumber();
}

unsigned int EMECHVManager::endPhiIndex() const
{
  return m_c->descriptor->getPhiBinning().getFirstDivisionNumber() + m_c->descriptor->getPhiBinning().getNumDivisions();
}

unsigned int EMECHVManager::beginEtaIndex() const
{
  return m_c->descriptor->getEtaBinning().getFirstDivisionNumber();
}

unsigned int EMECHVManager::endEtaIndex() const
{
  return m_c->descriptor->getEtaBinning().getFirstDivisionNumber() + m_c->descriptor->getEtaBinning().getNumDivisions();
}

const EMECHVModule& EMECHVManager::getHVModule(unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector) const
{
  return *(m_c->moduleArray[iSide][iEta][iPhi][iSector]);
}

EMECHVManager::~EMECHVManager()
{
  delete m_c;
}

unsigned int EMECHVManager::beginSideIndex() const
{
  return 0;
}

unsigned int EMECHVManager::endSideIndex() const
{
  return 2;
}

unsigned int EMECHVManager::beginSectorIndex() const
{
  return m_c->descriptor->getSectorBinning().getFirstDivisionNumber();
}

unsigned int EMECHVManager::endSectorIndex() const
{
  return m_c->descriptor->getSectorBinning().getFirstDivisionNumber() + m_c->descriptor->getSectorBinning().getNumDivisions();
}

EMECHVManager::IOType EMECHVManager::getWheelIndex() const
{
  return m_c->iWheel;
}

void EMECHVManager::update() const {
  std::lock_guard<std::mutex> lock(m_c->mtx);
  if (!(m_c->init)) {
    m_c->init=true;
    if (m_c->iWheel==EMECHVModule::OUTER)      {
        m_c->payloadArray.reserve(2*7*8*4*24);
        for (unsigned int i=0;i<10752;i++) {
          m_c->payloadArray[i].voltage[0]=-99999;
          m_c->payloadArray[i].voltage[1]=-99999;
        }
    }
    else if (m_c->iWheel==EMECHVModule::INNER)  {
        m_c->payloadArray.reserve(2*2*8*8*4);
        for (unsigned int i=0;i<1024;i++) {
          m_c->payloadArray[i].voltage[0]=-99999;
          m_c->payloadArray[i].voltage[1]=-99999;
        }
    }
    
    ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "HECHVManager");

    ISvcLocator* svcLocator = Gaudi::svcLocator(); 
    IToolSvc* toolSvc;
    LArHVCablingTool* hvcablingTool;

    if(StatusCode::SUCCESS!=svcLocator->service("ToolSvc",toolSvc))
      return;

    if(StatusCode::SUCCESS!=toolSvc->retrieveTool("LArHVCablingTool",hvcablingTool))
      return;

    std::vector<std::string> colnames;
    colnames.push_back("/LAR/DCS/HV/BARREl/I16");
    colnames.push_back("/LAR/DCS/HV/BARREL/I8");

    std::vector<std::string>::const_iterator it = colnames.begin();
    std::vector<std::string>::const_iterator ie = colnames.end();

    for (;it!=ie;it++) {
      //std::cout << " --- Start reading folder " << (*it) << std::endl;
      const CondAttrListCollection* atrlistcol;
      if (StatusCode::SUCCESS!=detStore->retrieve(atrlistcol,*it)) 
        return;

      for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {
        // Construct HWIdentifier
        // 1. decode COOL Channel ID
        unsigned int chanID = (*citr).first;
        int cannode = chanID/1000;
        int line = chanID%1000;
        //std::cout << "    ++ found data for cannode, line " << cannode << " " << line << std::endl;

        // 2. Construct the identifier
        HWIdentifier id = m_c->hvId->HVLineId(1,1,cannode,line);


        std::vector<HWIdentifier> electrodeIdVec = hvcablingTool->getLArElectrodeIDvec(id);

        for(size_t i=0;i<electrodeIdVec.size();i++) {
            HWIdentifier& elecHWID = electrodeIdVec[i];

            int detector = m_c->elecId->detector(elecHWID);
            // check we are in EMEC
            if (detector==2) {


	    unsigned int etaIndex=m_c->elecId->hv_eta(elecHWID);

	    if ( (etaIndex>6 && m_c->iWheel==EMECHVModule::INNER) || (etaIndex<7 && m_c->iWheel==EMECHVModule::OUTER) ) {

	      unsigned int sideIndex=1-m_c->elecId->zside(elecHWID);
	      unsigned int phiIndex=m_c->elecId->module(elecHWID);      // 0 to 7
// rotation for C side
              if (sideIndex==0) {
                  if (phiIndex<4) phiIndex=3-phiIndex;
                  else phiIndex=11-phiIndex;
              }
	      unsigned int sectorIndex=m_c->elecId->hv_phi(elecHWID)-1;    // 0 to 3 in Outer, 0 to 7 in Inner
// rotation for C side
              if (sideIndex==0) {
                  if (m_c->iWheel==EMECHVModule::OUTER) sectorIndex=3-sectorIndex;
                  else  sectorIndex=7-sectorIndex;
              } 
	      unsigned int electrodeIndex = m_c->iWheel==EMECHVModule::OUTER ?
                   m_c->elecId->electrode(elecHWID)%24:
                   m_c->elecId->electrode(elecHWID)%4;
// rotation for C side
              if (sideIndex==0) {
                  if (m_c->iWheel==EMECHVModule::OUTER) electrodeIndex=23-electrodeIndex;
                  else  electrodeIndex=3-electrodeIndex;
              } 
	  
	      unsigned int index             = m_c->iWheel==EMECHVModule::OUTER ? 
	        5376*sideIndex+768*etaIndex+96*phiIndex+24*sectorIndex+electrodeIndex:
	         512*sideIndex+256*(etaIndex-7)+32*phiIndex+4*sectorIndex+electrodeIndex;

              if (m_c->iWheel==EMECHVModule::OUTER && index>10752) {
                std::cout << "invalid index outer " << index << " side,eta,phi,sector,electrode " << sideIndex << " " << etaIndex << " " << phiIndex <<
                 " " << sectorIndex << " " << electrodeIndex << std::endl;
                continue;
              }
              if (m_c->iWheel==EMECHVModule::INNER && index>1024) {
                std::cout << "invalid index inner " << index << " side,eta,phi,sector,electrode " << sideIndex << " " << etaIndex << " " << phiIndex <<
                 " " << sectorIndex << " " << electrodeIndex << std::endl;
                continue;
              }
	  
	      unsigned int gapIndex=m_c->elecId->gap(elecHWID);
              if (gapIndex>1) {
                std::cout << "invalid gapIndex " << gapIndex << std::endl;
                continue;
              }
              if (sideIndex==0) gapIndex=1-gapIndex;
	      
              float voltage = -99999.;
              if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
              float current = 0.;
              if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();
              unsigned int status = 0;
              if (!((*citr).second)["R_STAT"].isNull()) status =  ((*citr).second)["R_STAT"].data<unsigned int>(); 

              //std::cout << "             hvlineId,elecHWID,cannode,line, side,phi module, sector,eta,electrode,gap,index " << std::hex << id << " " << elecHWID << std::dec << " " << cannode << " " << line << " " << m_c->elecId->zside(elecHWID) << " " << m_c->elecId->module(elecHWID) << " " << m_c->elecId->hv_phi(elecHWID) << " " << m_c->elecId->hv_eta(elecHWID) << " " << m_c->elecId->electrode(elecHWID)
               // << " " << gapIndex << "  " << index << " " << voltage << std::endl;

	  
	      m_c->payloadArray[index].voltage[gapIndex]=voltage;
	      m_c->payloadArray[index].current[gapIndex]=current;
	      m_c->payloadArray[index].status[gapIndex]=status;
	      m_c->payloadArray[index].hvLineNo[gapIndex]=chanID;
	    } // if etaIndex...
	  } // for (electrodeIdVec)
        } // is EMEC
      } // for (atrlistcol)
    }
  } // if(!m_c->init)
}

void EMECHVManager::reset() const {
  m_c->init=false;
}

EMECHVPayload *EMECHVManager::getPayload(const EMECHVElectrode &electrode) const {
  update();
  unsigned int electrodeIndex    = electrode.getElectrodeIndex();
  const EMECHVModule& module     = electrode.getModule();
  unsigned int etaIndex          = module.getEtaIndex();
  unsigned int phiIndex          = module.getPhiIndex();
  unsigned int sectorIndex       = module.getSectorIndex();
  unsigned int sideIndex         = module.getSideIndex();

  unsigned int index=0;
  if (m_c->iWheel==EMECHVModule::OUTER)      index= 5376*sideIndex+768*etaIndex+96*phiIndex+24*sectorIndex+electrodeIndex;
  else if (m_c->iWheel==EMECHVModule::INNER) index= 512*sideIndex+256*etaIndex+32*phiIndex+4*sectorIndex+electrodeIndex;
  return &m_c->payloadArray[index];
}

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int EMECHVManager::hvLineNo(const EMECHVElectrode& electrode
			    , int gap
			    , const LArHVIdMapping* hvIdMapping) const
{
  const EMECHVModule& module      = electrode.getModule();
  int etaIndex          = module.getEtaIndex();
  int phiIndex          = module.getPhiIndex();
  int sectorIndex       = module.getSectorIndex();
  int sideIndex         = module.getSideIndex();
  int electrodeIndex    = electrode.getElectrodeIndex();

  // ________________________ Construct ElectrodeID ________________________________
  int id_detector = 2;
  int id_zside = 1-sideIndex;
  int id_hv_eta = m_c->iWheel==EMECHVModule::OUTER ? etaIndex : etaIndex+7;

  int id_module{0};
  if(sideIndex==0) {
    if(phiIndex<4) {
      id_module=3-phiIndex;
    }
    else {
      id_module=11-phiIndex;
    }
  }
  else {
    id_module = phiIndex;
  }

  int id_hv_phi{0};
  if(sideIndex==0) {
    if (m_c->iWheel==EMECHVModule::OUTER) {
      id_hv_phi=4-sectorIndex;
    }
    else {
      id_hv_phi=8-sectorIndex;
    }
  }
  else {
    id_hv_phi=sectorIndex+1;
  }

  int tmpElec{0};
  if(sideIndex==0) {
    if (m_c->iWheel==EMECHVModule::OUTER) {
      tmpElec = 23-electrodeIndex;
    }
    else {
      tmpElec = 3 - electrodeIndex;
    }
  }
  else {
    tmpElec = electrodeIndex;
  }
  int id_electrode = m_c->iWheel==EMECHVModule::OUTER
    ? (id_hv_phi-1)*24 + tmpElec
    : (id_hv_phi-1)*4 + tmpElec;

  int id_gap = sideIndex==0 ? 1-gap : gap;

  HWIdentifier elecHWID = m_c->elecId->ElectrodeId(id_detector
						   , id_zside
						   , id_module
						   , id_hv_phi
						   , id_hv_eta
						   , id_gap
						   , id_electrode);
  // ________________________ Construct ElectrodeID ________________________________

  // Get LArHVLineID corresponding to a given LArElectrodeId
  HWIdentifier id = hvIdMapping->getLArHVLineID(elecHWID);
  
  // Extract HV Line No
  return m_c->hvId->can_node(id)*1000 + m_c->hvId->hv_line(id);
}
#endif
