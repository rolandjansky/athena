/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVSubgap.h"
#include <cmath>

#include "HECHVPayload.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArCabling/LArHVCablingSimTool.h"

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
#include "LArRecConditions/LArHVIdMapping.h"
#endif

#include "Identifier/HWIdentifier.h"


namespace {


struct SimIdFunc
{
  SimIdFunc();
  std::vector<HWIdentifier> operator()(HWIdentifier id)
  {
    return m_cablingTool->getLArElectrodeIDvec (id);
  }
  LArHVCablingSimTool* m_cablingTool;
};


SimIdFunc::SimIdFunc()
{
  ToolHandle<LArHVCablingSimTool> tool ("LArHVCablingSimTool");
  if (!tool.retrieve().isSuccess()) {
    std::abort();
  }
  m_cablingTool = tool.get();
}


} // Anonymous namespace


class HECHVManager::Clockwork {
public:
  Clockwork(const HECHVManager* manager) 
  {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iPhi=0; iPhi<32; ++iPhi) {
	for(int iSampling=0; iSampling<4; ++iSampling) {
	  moduleArray[iSide][iPhi][iSampling] = std::make_unique<HECHVModule>(manager,iSide,iPhi,iSampling);
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
  }
  HECHVDescriptor    descriptor{CellBinning(0,2*M_PI,32)};
  std::unique_ptr<const HECHVModule> moduleArray[2][32][4];
  const LArElectrodeID* elecId = nullptr;
  const LArHVLineID* hvId = nullptr;
};


class HECHVManager::HECHVData::Payload
{
public:
  std::vector<HECHVPayload> m_payloadArray;
};


HECHVManager::HECHVData::HECHVData()
{
}
  

HECHVManager::HECHVData::HECHVData (std::unique_ptr<Payload> payload)
  : m_payload (std::move (payload))
{
}
  

HECHVManager::HECHVData&
HECHVManager::HECHVData::operator= (HECHVData&& other)
{
  if (this != &other) {
    m_payload = std::move (other.m_payload);
  }
  return *this;
}
  

HECHVManager::HECHVData::~HECHVData()
{
}
  

bool HECHVManager::HECHVData::hvOn (const HECHVSubgap& subgap) const
{
  return voltage (subgap) > INVALID;
}


double HECHVManager::HECHVData::voltage (const HECHVSubgap& subgap) const
{
  return m_payload->m_payloadArray[index(subgap)].voltage;
}


double HECHVManager::HECHVData::current (const HECHVSubgap& subgap) const
{
  return m_payload->m_payloadArray[index(subgap)].current;
}


int  HECHVManager::HECHVData::hvLineNo  (const HECHVSubgap& subgap) const
{
  return m_payload->m_payloadArray[index(subgap)].hvLineNo;
}


int  HECHVManager::HECHVData::index  (const HECHVSubgap& subgap) const
{
  unsigned int subgapIndex    = subgap.getSubgapIndex();
  const HECHVModule& module   = subgap.getModule();
  unsigned int phiIndex       = module.getPhiIndex();
  unsigned int samplingIndex  = module.getSamplingIndex();
  unsigned int sideIndex      = module.getSideIndex();
  unsigned int index          = 512*sideIndex+16*phiIndex+4*samplingIndex+subgapIndex;
  return index;
}


HECHVManager::HECHVManager()
  : m_c (std::make_unique<Clockwork> (this))
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
}

HECHVManager::HECHVData
HECHVManager::getData (idfunc_t idfunc,
                       const std::vector<const CondAttrListCollection*>& attrLists) const
{
  auto payload = std::make_unique<HECHVData::Payload>();
  payload->m_payloadArray.reserve(2*32*4*4);
  for (unsigned int i=0;i<1024;i++) {
    payload->m_payloadArray[i].voltage = HECHVData::INVALID;
  }

  for (const CondAttrListCollection* atrlistcol : attrLists) {

    for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {

      // Construct HWIdentifier
      // 1. decode COOL Channel ID
      unsigned int chanID = (*citr).first;
      int cannode = chanID/1000;
      int line = chanID%1000;

      // 2. Construct the identifier
      HWIdentifier id = m_c->hvId->HVLineId(1,1,cannode,line);

      std::vector<HWIdentifier> electrodeIdVec = idfunc(id);

      for(size_t i=0;i<electrodeIdVec.size();i++) { 
        HWIdentifier& elecHWID = electrodeIdVec[i];

        int detector = m_c->elecId->detector(elecHWID);

        // check we are in HEC
        if (detector != 4) {
          continue;
        }

        float voltage = HECHVData::INVALID;
        if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
        float current = 0.;
        if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();

	  
        unsigned int sideIndex=1-m_c->elecId->zside(elecHWID);    // 0 for C side, 1 for A side, opposite to HV numbering
        unsigned int phiIndex=m_c->elecId->module(elecHWID);      // 0 to 31
        unsigned int samplingIndex=m_c->elecId->hv_eta(elecHWID)-1; // 0 to 3
        unsigned int subgapIndex=m_c->elecId->gap(elecHWID);        // 0 to 3

        unsigned int index             = 512*sideIndex+16*phiIndex+4*samplingIndex+subgapIndex;


        if (index>1023) {
          MsgStream msg (Athena::getMessageSvc(), "EMECHVManager");
          msg << MSG::ERROR << "invalid index " << index << " side,phi,sampling,gap " << sideIndex << " " << phiIndex << " " << samplingIndex
              << " " << subgapIndex << endmsg;
          continue;
        }
	  
        payload->m_payloadArray[index].voltage=voltage;
        payload->m_payloadArray[index].current=current;
        payload->m_payloadArray[index].hvLineNo=chanID;
      } // For (electrodeIdVec)
    } // for (atrlistcol)
  }

  return HECHVManager::HECHVData (std::move (payload));
}


HECHVManager::HECHVData
HECHVManager::getDataSim() const
{
  std::vector<const CondAttrListCollection*> attrLists;
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "EMBHVManager");
  const CondAttrListCollection* atrlistcol = nullptr;
  // Not a typo --- this folder has a lower-case l in the database...
  if (detStore->retrieve(atrlistcol, "/LAR/DCS/HV/BARREl/I16").isSuccess()) {
    attrLists.push_back (atrlistcol);
  }
  if (detStore->retrieve(atrlistcol, "/LAR/DCS/HV/BARREL/I8").isSuccess()) {
    attrLists.push_back (atrlistcol);
  }
  return getData (SimIdFunc(), attrLists);
}


#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
HECHVManager::HECHVData
HECHVManager::getData (const LArHVIdMapping& hvIdMapping,
                       const std::vector<const CondAttrListCollection*>& attrLists) const
{
  auto idfunc = [&] (HWIdentifier id) { return hvIdMapping.getLArElectrodeIDvec(id); };
  return getData (idfunc, attrLists);
}


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
