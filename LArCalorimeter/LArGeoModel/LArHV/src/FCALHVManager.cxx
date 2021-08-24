/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/FCALHVManager.h"
#include "LArHV/FCALHVLine.h"

#include "FCALHVPayload.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArCabling/LArHVCablingSimTool.h"
#include "Identifier/HWIdentifier.h"

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
#include "LArRecConditions/LArHVIdMapping.h"
#endif

#include <atomic>


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


class FCALHVManager::Clockwork {
public:
  Clockwork(const FCALHVManager* manager)
  {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iSector=0; iSector<16; ++iSector) {
	for(int iSampling=0; iSampling<3; ++iSampling) {
	  moduleArray[iSide][iSector][iSampling] = std::make_unique<FCALHVModule>(manager,iSide,iSector,iSampling);
	}
      }
    }

    ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "HECHVManager");
    if (StatusCode::SUCCESS!=detStore->retrieve(elecId, "LArElectrodeID")) {
      throw std::runtime_error("FCALHVManager failed to retrieve LArElectrodeID");
    }

    if (StatusCode::SUCCESS!=detStore->retrieve(hvId,"LArHVLineID")) {
      throw std::runtime_error("FCALHVManager failed to retrieve LArHVLineID");
    }
  }
  ~Clockwork()
  {
  }
  std::unique_ptr<const FCALHVModule> moduleArray[2][16][3];
  const LArElectrodeID* elecId = nullptr;
  const LArHVLineID* hvId = nullptr;
};


class FCALHVManager::FCALHVData::Payload
{
public:
  std::vector<FCALHVPayload> m_payloadArray;
};


FCALHVManager::FCALHVData::FCALHVData()
{
}
  

FCALHVManager::FCALHVData::FCALHVData (std::unique_ptr<Payload> payload)
  : m_payload (std::move (payload))
{
}
  

FCALHVManager::FCALHVData&
FCALHVManager::FCALHVData::operator= (FCALHVData&& other)
{
  if (this != &other) {
    m_payload = std::move (other.m_payload);
  }
  return *this;
}
  

FCALHVManager::FCALHVData::~FCALHVData()
{
}
  

bool FCALHVManager::FCALHVData::hvOn (const FCALHVLine& line) const
{
  return voltage (line) > INVALID;
}


double FCALHVManager::FCALHVData::voltage (const FCALHVLine& line) const
{
  return m_payload->m_payloadArray[index(line)].voltage;
}


double FCALHVManager::FCALHVData::current (const FCALHVLine& line) const
{
  return m_payload->m_payloadArray[index(line)].current;
}


int  FCALHVManager::FCALHVData::hvLineNo  (const FCALHVLine& line) const
{
  return m_payload->m_payloadArray[index(line)].hvLineNo;
}


int  FCALHVManager::FCALHVData::index  (const FCALHVLine& line) const
{
  unsigned int lineIndex         = line.getLineIndex();
  const FCALHVModule& module     = line.getModule();
  unsigned int sectorIndex       = module.getSectorIndex();
  unsigned int sideIndex         = module.getSideIndex();
  unsigned int samplingIndex     = module.getSamplingIndex();
  unsigned int index             = 192*sideIndex+12*sectorIndex+4*samplingIndex+lineIndex;
  return index;
}


FCALHVManager::FCALHVManager()
  : m_c (std::make_unique<Clockwork> (this))
{
}

FCALHVManager::~FCALHVManager()
{
}

unsigned int FCALHVManager::beginSideIndex() const
{
  return 0;
}

unsigned int FCALHVManager::endSideIndex() const
{
  return 2;
}

unsigned int FCALHVManager::beginSectorIndex(unsigned int /*iSampling*/) const
{
  return 0;
}

unsigned int FCALHVManager::endSectorIndex(unsigned int iSampling) const
{
  if (iSampling==0) return 16;
  if (iSampling==1) return 8;
  if (iSampling==2) return 4;
  return 0;
}

unsigned int FCALHVManager::beginSamplingIndex() const
{
  return 0;
}

unsigned int FCALHVManager::endSamplingIndex() const
{
  return 3;
}

const FCALHVModule& FCALHVManager::getHVModule(unsigned int iSide, unsigned int iSector, unsigned int iSampling) const
{
  return *(m_c->moduleArray[iSide][iSector][iSampling]);
}

FCALHVManager::FCALHVData
FCALHVManager::getData (idfunc_t idfunc,
                       const std::vector<const CondAttrListCollection*>& attrLists) const
{
  auto payload = std::make_unique<FCALHVData::Payload>();
  payload->m_payloadArray.reserve(2*16*3*4);
  for (unsigned int i=0;i<384;i++) {
    payload->m_payloadArray[i].voltage = FCALHVData::INVALID;
  }

  for (const CondAttrListCollection* atrlistcol : attrLists) {

    for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {

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
        if (detector==5) {

          float voltage = FCALHVData::INVALID;;
          if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
          float current = 0.;
          if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();
	    
          unsigned int sideIndex=1-m_c->elecId->zside(elecHWID);      // 0 C side, 1 A side (unline HV numbering)
          unsigned int samplingIndex=m_c->elecId->hv_eta(elecHWID)-1;   // 0 to 2 for the FCAL modules 1-2-3
          unsigned int sectorIndex=m_c->elecId->module(elecHWID);       // 0-15 FCAL1, 0-7 FCAl2, 0-3 FCAL3
          unsigned int lineIndex=m_c->elecId->gap(elecHWID);            // 0-3

          // do we have to worry about phi sector numbering running backwards in phi for z<0 like in EM/HEC  ????

          unsigned int index             = 192*sideIndex+12*sectorIndex+4*samplingIndex+lineIndex;

          if (index>384) {
            MsgStream msg (Athena::getMessageSvc(), "EMECHVManager");
            msg << MSG::ERROR << " invalid index for FCAL " << sideIndex << " " << samplingIndex << " " << sectorIndex << " " << lineIndex << endmsg;
            continue;
          }
	    
	    
          payload->m_payloadArray[index].voltage=voltage;
          payload->m_payloadArray[index].current=current;
          payload->m_payloadArray[index].hvLineNo=chanID;
        }   // if FCAL
      }  //   loop over electrodes
    }   // loop over collection
  }     // loop over folders

  return FCALHVManager::FCALHVData (std::move (payload));
}

FCALHVManager::FCALHVData
FCALHVManager::getDataSim() const
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
FCALHVManager::FCALHVData
FCALHVManager::getData (const LArHVIdMapping& hvIdMapping,
                        const std::vector<const CondAttrListCollection*>& attrLists) const
{
  auto idfunc = [&] (HWIdentifier id) { return hvIdMapping.getLArElectrodeIDvec(id); };
  return getData (idfunc, attrLists);
}


int FCALHVManager::hvLineNo(const FCALHVLine& line
			    , const LArHVIdMapping* hvIdMapping) const
{
  const FCALHVModule& module     = line.getModule();
  int sideIndex         = module.getSideIndex();
  int sectorIndex       = module.getSectorIndex();
  int samplingIndex     = module.getSamplingIndex();
  int lineIndex         = line.getLineIndex();

  // ________________________ Construct ElectrodeID ________________________________
  int id_detector = 5;
  int id_zside = 1-sideIndex;
  int id_module = sectorIndex;
  int id_hv_phi{0};
  int id_hv_eta = samplingIndex + 1;
  int id_gap = lineIndex;
  int id_electrode{0};

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
