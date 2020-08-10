/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVSubgap.h"
#include <cmath>

#include "HECHVPayload.h"

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

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
#include "LArRecConditions/LArHVIdMapping.h"
#endif

#include "Identifier/HWIdentifier.h"

class HECHVManager::Clockwork {
public:
  Clockwork(const HECHVManager* manager) 
  {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iPhi=0; iPhi<32; ++iPhi) {
	for(int iSampling=0; iSampling<4; ++iSampling) {
	  moduleArray[iSide][iPhi][iSampling] = new HECHVModule(manager,iSide,iPhi,iSampling);
	}
      }
    }

    ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "HECHVManager");
    if (StatusCode::SUCCESS!=detStore->retrieve(elecId, "LArElectrodeID")) {
      throw std::runtime_error("HECHVManager failed to retrieve LArElectrodeID");
    }

    if (StatusCode::SUCCESS!=detStore->retrieve(hvId,"LArHVLineID")) {
      throw std::runtime_error("HECHVManager failed to retrieve LArHVLineID");
    }
  }
  ~Clockwork()
  {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iPhi=0; iPhi<32; ++iPhi) {
	for(int iSampling=0; iSampling<4; ++iSampling) {
	  delete moduleArray[iSide][iPhi][iSampling];
	}
      }
    }
  }
  HECHVDescriptor    descriptor{CellBinning(0,2*M_PI,32)};
  const HECHVModule* moduleArray[2][32][4];
  std::atomic<bool>  init{false};
  std::mutex         mtx;
  std::vector<HECHVPayload> payloadArray;     
  const LArElectrodeID* elecId;
  const LArHVLineID* hvId;
};

HECHVManager::HECHVManager()
  : m_c(new Clockwork(this))
{
}

const HECHVDescriptor& HECHVManager::getDescriptor() const
{
  return m_c->descriptor;
}

unsigned int HECHVManager::beginSideIndex() const
{
  return 0;
}

unsigned int HECHVManager::endSideIndex() const
{
  return 2;
}

unsigned int HECHVManager::beginPhiIndex() const
{
  return 0;
}

unsigned int HECHVManager::endPhiIndex() const
{
  return 32;
}

unsigned int HECHVManager::beginSamplingIndex() const
{
  return 0;
}

unsigned int HECHVManager::endSamplingIndex() const
{
  return 4;
}

const HECHVModule& HECHVManager::getHVModule(unsigned int iSide
					     , unsigned int iPhi
					     , unsigned int iSampling) const
{
  return *(m_c->moduleArray[iSide][iPhi][iSampling]);
}

HECHVManager::~HECHVManager()
{
  delete m_c;
}

void HECHVManager::update() const {
  std::lock_guard<std::mutex> lock(m_c->mtx);
  if (!(m_c->init)) {
    m_c->init=true;
    m_c->payloadArray.reserve(2*32*4*4);
    for (unsigned int i=0;i<1024;i++) {
      m_c->payloadArray[i].voltage = -99999;
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

        // 2. Construct the identifier
        HWIdentifier id = m_c->hvId->HVLineId(1,1,cannode,line);

        std::vector<HWIdentifier> electrodeIdVec = hvcablingTool->getLArElectrodeIDvec(id);

	for(size_t i=0;i<electrodeIdVec.size();i++) { 
	    HWIdentifier& elecHWID = electrodeIdVec[i];

            int detector = m_c->elecId->detector(elecHWID);

            // check we are in HEC
            if (detector != 4) {
               //std::cout << " Not HEC ??? " << std::endl;
               continue;
            }

            float voltage = -99999.;
            if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
            float current = 0.;
            if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();
            unsigned int status = 0;
            if (!((*citr).second)["R_STAT"].isNull()) status =  ((*citr).second)["R_STAT"].data<unsigned int>(); 

	  
	    unsigned int sideIndex=1-m_c->elecId->zside(elecHWID);    // 0 for C side, 1 for A side, opposite to HV numbering
	    unsigned int phiIndex=m_c->elecId->module(elecHWID);      // 0 to 31
	    unsigned int samplingIndex=m_c->elecId->hv_eta(elecHWID)-1; // 0 to 3
	    unsigned int subgapIndex=m_c->elecId->gap(elecHWID);        // 0 to 3

	    unsigned int index             = 512*sideIndex+16*phiIndex+4*samplingIndex+subgapIndex;


            if (index>1023) {
              std::cout << "invalid index " << index << " side,phi,sampling,gap " << sideIndex << " " << phiIndex << " " << samplingIndex
                << " " << subgapIndex << std::endl;
              continue;
            }
	  
	    m_c->payloadArray[index].voltage=voltage;
	    m_c->payloadArray[index].current=current;
	    m_c->payloadArray[index].status=status;
	    m_c->payloadArray[index].hvLineNo=chanID;
        } // For (electrodeIdVec)
      } // for (atrlistcol)
    }
  } // if(!m_c->init)
}

void HECHVManager::reset() const {
  m_c->init=false;
}

HECHVPayload *HECHVManager::getPayload(const HECHVSubgap &subgap) const {
  update();
  unsigned int subgapIndex    = subgap.getSubgapIndex();
  const HECHVModule& module   = subgap.getModule();
  unsigned int phiIndex       = module.getPhiIndex();
  unsigned int samplingIndex  = module.getSamplingIndex();
  unsigned int sideIndex      = module.getSideIndex();
  unsigned int index          = 512*sideIndex+16*phiIndex+4*samplingIndex+subgapIndex;
  return &m_c->payloadArray[index];
}

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
int HECHVManager::hvLineNo(const HECHVSubgap& subgap
			   , const LArHVIdMapping* hvIdMapping) const {
  const HECHVModule& module   = subgap.getModule();
  int sideIndex      = module.getSideIndex();
  int phiIndex       = module.getPhiIndex();
  int samplingIndex  = module.getSamplingIndex();
  int subgapIndex    = subgap.getSubgapIndex();

  // ________________________ Construct ElectrodeID ________________________________
  int id_detector = 4;
  int id_zside = 1-sideIndex;
  int id_module = phiIndex;
  int id_hv_phi = 0;
  int id_hv_eta = samplingIndex+1;
  int id_gap = subgapIndex;
  int id_electrode{0};

  switch(samplingIndex) {
  case 0:
    id_electrode=1;
    break;
  case 1:
    id_electrode=9;
    break;
  case 2:
    id_electrode=25;
    break;
  case 3:
    id_electrode=33;
    break;
  default:
    throw std::runtime_error("Unexpected value for samplingIndex in HEC: " + std::to_string(samplingIndex));
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
