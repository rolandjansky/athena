/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBHVManager.h"
#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVElectrode.h"
#include "GeoModelKernel/CellBinning.h"
#include <cmath>
#include <vector>
#include "EMBHVPayload.h"

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
#include <mutex>
#include <atomic>

class EMBHVManager::Clockwork {
public:
  Clockwork(const EMBHVManager* manager) {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iEta=0; iEta<8; ++iEta) {
	for(int iPhi=0; iPhi<16; ++iPhi) {
	  for(int iSector=0; iSector<2; ++iSector) {
	    moduleArray[iSide][iEta][iPhi][iSector] = new EMBHVModule(manager,iSide,iEta,iPhi,iSector);
	  }
	}
      }
    }

    ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "HECHVManager");
    if (StatusCode::SUCCESS!=detStore->retrieve(elecId, "LArElectrodeID")) {
      throw std::runtime_error("EMBHVManager failed to retrieve LArElectrodeID");
    }

    if (StatusCode::SUCCESS!=detStore->retrieve(hvId,"LArHVLineID")) {
      throw std::runtime_error("EMBHVManager failed to retrieve LArHVLineID");
    }
  }
  Clockwork(const Clockwork&) = delete;
  ~Clockwork() {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iEta=0; iEta<8; ++iEta) {
	for(int iPhi=0; iPhi<16; ++iPhi) {
	  for(int iSector=0; iSector<2; ++iSector) {
	    delete moduleArray[iSide][iEta][iPhi][iSector];
	  }
	}
      }
    }
  }
  EMBHVDescriptor       descriptor{CellBinning(0.0, 1.4, 7, 1),CellBinning(0.0, 2*M_PI, 16)};
  const EMBHVModule*    moduleArray[2][8][16][2];
  std::atomic<bool>     init{false};
  std::mutex            mtx;
  std::vector<EMBHVPayload> payloadArray;
  const LArElectrodeID* elecId;
  const LArHVLineID* hvId;
};

EMBHVManager::EMBHVManager()
  : m_c(new Clockwork(this))
{
}

EMBHVManager::~EMBHVManager()
{
  delete m_c;
}

const EMBHVDescriptor& EMBHVManager::getDescriptor() const
{
  return m_c->descriptor;
}

unsigned int EMBHVManager::beginPhiIndex() const
{
  return m_c->descriptor.getPhiBinning().getFirstDivisionNumber();
}

unsigned int EMBHVManager::endPhiIndex() const
{
  return m_c->descriptor.getPhiBinning().getFirstDivisionNumber() + m_c->descriptor.getPhiBinning().getNumDivisions();
}

unsigned int EMBHVManager::beginEtaIndex() const
{
  return m_c->descriptor.getEtaBinning().getFirstDivisionNumber();
}

unsigned int EMBHVManager::endEtaIndex() const
{
  return m_c->descriptor.getEtaBinning().getFirstDivisionNumber() + m_c->descriptor.getEtaBinning().getNumDivisions();
}

const EMBHVModule& EMBHVManager::getHVModule(unsigned int iSide, unsigned int iEta,unsigned int iPhi, unsigned int iSector) const
{
  return *(m_c->moduleArray[iSide][iEta][iPhi][iSector]);
}

unsigned int EMBHVManager::beginSectorIndex() const
{
  return 0;
}

unsigned int EMBHVManager::endSectorIndex() const
{
  return 2;
}

unsigned int EMBHVManager::beginSideIndex() const
{
  return 0;
}

unsigned int EMBHVManager::endSideIndex() const
{
  return 2;
}

void EMBHVManager::update() const {
  std::lock_guard<std::mutex> lock(m_c->mtx);
  if (!(m_c->init)) {
    m_c->init=true;
    m_c->payloadArray.reserve(2*8*16*2*32);

    for (int i=0;i<16384;i++) {
       m_c->payloadArray[i].voltage[0]=-99999.;
       m_c->payloadArray[i].voltage[1]=-99999.;
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

	for(size_t i=0;i<electrodeIdVec.size();i++)
	{
            HWIdentifier& elecHWID = electrodeIdVec[i];
            int detector = m_c->elecId->detector(elecHWID);
            if (detector==0) {

               //std::cout << "       in Barrel " << std::endl;

// side  in standard offline 0 for z<0 (C) 1 for z>0 (A)
//  in electrode numbering, this is the opposite (0 for A and 1 for C)
	    unsigned int sideIndex=1-m_c->elecId->zside(elecHWID);
// eta index, no trouble
	    unsigned int etaIndex=m_c->elecId->hv_eta(elecHWID);
// phi index   
//  offline 0 to 2pi in 2pi/16 bins
// this is module in the electrode numbering: on the A side 0 to 15, 0 is halfway around phi=0 (FT-1 (hv_phi=1 is a lower phi)
//   offline phi      0               pi                    2pi
//   Module           M0 M1 M1                          M15 M0
//   FT               0 -1  0                           0  -1
//   hv_phi           0  1  0                           0   1
//   phiIndex         0  0  1                          15   15
// sector Index       0  1  0                           0   1
	    unsigned int phiIndex;
            unsigned int sectorIndex;
            if (sideIndex==1) {
             phiIndex=m_c->elecId->module(elecHWID);
             sectorIndex=m_c->elecId->hv_phi(elecHWID);
            }
// module numbering on the C side 0 around phi=pi, running backwards
//   offline phi     0               pi                      2pi
//   Module          P8 P7 P7      P0 P0                 P9 P8
//   FT              -1 0 -1       0  -1                -1  0
//   hv_phi           1 0  1       0   1                 1   0
//  phiIndex          0 0  1       7   8                15  15
// sectorIndex        0 1  0       1   0                 0  1
            else {
              int imodule=m_c->elecId->module(elecHWID);
              if (imodule<9) phiIndex = 8 - imodule;
              else           phiIndex = 24 - imodule;
              sectorIndex = 1-m_c->elecId->hv_phi(elecHWID);
            }

            if (sectorIndex==1) {
               if (phiIndex>0) phiIndex = phiIndex - 1;
               else phiIndex=15;
            }

	    unsigned int electrodeIndex=m_c->elecId->electrode(elecHWID);
            if (sideIndex==0) {
               if (m_c->elecId->hv_phi(elecHWID)==1) electrodeIndex=31-electrodeIndex;  // FT-1 change 0->31 to 31->0
               else                             electrodeIndex=63-electrodeIndex;  // FT 0 change 32->63 to 31-0
            }
            else {
                if (m_c->elecId->hv_phi(elecHWID)==0) electrodeIndex=electrodeIndex-32;  // FT 0 change 31-63 to 0-31
            }
	  
	    unsigned int index             = 8192*sideIndex+1024*etaIndex+64*phiIndex+32*sectorIndex+electrodeIndex;
	  
	    unsigned int gapIndex=m_c->elecId->gap(elecHWID);
            if (sideIndex==0) gapIndex=1-gapIndex;

            float voltage = -99999.;
            if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
            float current = 0.;
            if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();
            unsigned int status = 0;
            if (!((*citr).second)["R_STAT"].isNull()) status =  ((*citr).second)["R_STAT"].data<unsigned int>(); 

         // std::cout << "             hvlineId,elecHWID,cannode,line, side,phi module, sector,eta,electrode,gap,index " << std::hex << id << " " << elecHWID << std::dec << " " << cannode << " " << line << " " << m_c->elecId->zside(elecHWID) << " " << m_c->elecId->module(elecHWID) << " " << m_c->elecId->hv_phi(elecHWID) << " " << m_c->elecId->hv_eta(elecHWID) << " " << m_c->elecId->electrode(elecHWID)
         //  << " " << gapIndex << "  " << index << " " << voltage << std::endl;

	  
	    m_c->payloadArray[index].voltage[gapIndex]=voltage;
	    m_c->payloadArray[index].current[gapIndex]=current;
	    m_c->payloadArray[index].status[gapIndex]=status;
	    m_c->payloadArray[index].hvLineNo[gapIndex]=chanID;
	  }
//	  std::cerr << "\n";
        }
      }
    }

  }
}

void  EMBHVManager::reset() const {
  m_c->init=false;
}

EMBHVPayload* EMBHVManager::getPayload(const EMBHVElectrode &electrode) const {
  update();
  unsigned int electrodeIndex    = electrode.getElectrodeIndex();
  const EMBHVModule& module      = electrode.getModule();
  unsigned int etaIndex          = module.getEtaIndex();
  unsigned int phiIndex          = module.getPhiIndex();
  unsigned int sectorIndex       = module.getSectorIndex();
  unsigned int sideIndex         = module.getSideIndex();
  unsigned int index             = 8192*sideIndex+1024*etaIndex+64*phiIndex+32*sectorIndex+electrodeIndex;
  return &m_c->payloadArray[index];
}

#ifndef SIMULATIONBASE
#ifndef GENERATIONBASE
int EMBHVManager::hvLineNo(const EMBHVElectrode& electrode
			   , int gap
			   , const LArHVIdMapping* hvIdMapping) const
{
  const EMBHVModule& module      = electrode.getModule();
  int etaIndex          = module.getEtaIndex();
  int phiIndex          = module.getPhiIndex();
  int sectorIndex       = module.getSectorIndex();
  int sideIndex         = module.getSideIndex();
  int electrodeIndex    = electrode.getElectrodeIndex();

  // ________________________ Construct ElectrodeID ________________________________
  int id_detector = 0;
  int id_zside = 1 - sideIndex;
  int id_hv_phi = (sideIndex==1 ? sectorIndex : 1-sectorIndex);
  int id_hv_eta = etaIndex;
  int id_gap = (sideIndex==0 ? 1-gap : gap);
  int tmpPhi = phiIndex;
  if(sectorIndex==1) {
    if(tmpPhi==15) {
      tmpPhi=0;
    }
    else {
      tmpPhi += 1;
    }
  }

  int id_module;
  if(sideIndex==1) {
    id_module = tmpPhi;
  }
  else {
    if(tmpPhi<9) {
      id_module = 8-tmpPhi;
    }
    else {
      id_module = 24-tmpPhi;
    }
  }

  int id_electrode;
  if(sideIndex==0) {
    if(id_hv_phi==1) {
      id_electrode = 31-electrodeIndex;
    }
    else {
      id_electrode = 63-electrodeIndex;
    }
  }
  else {
    if(id_hv_phi==0) {
      id_electrode = electrodeIndex+32;
    }
    else {
      id_electrode = electrodeIndex;
    }
  }

  HWIdentifier elecHWID = m_c->elecId->ElectrodeId(id_detector
						   , id_zside
						   , id_module
						   , id_hv_phi
						   , id_hv_eta
						   , id_gap
						   , id_electrode);
  // ________________________  ________________________________

  // Get LArHVLineID corresponding to a given LArElectrodeId
  HWIdentifier id = hvIdMapping->getLArHVLineID(elecHWID);

  // Extract HV Line No
  return m_c->hvId->can_node(id)*1000 + m_c->hvId->hv_line(id);
}
#endif
#endif


