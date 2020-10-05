/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/FCALHVManager.h"
#include "LArHV/FCALHVLine.h"

#include "FCALHVPayload.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArCabling/LArHVCablingTool.h"
#include "Identifier/HWIdentifier.h"

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
#include "LArRecConditions/LArHVIdMapping.h"
#endif

#include <atomic>

class FCALHVManager::Clockwork {
public:
  Clockwork(const FCALHVManager* manager)
  {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iSector=0; iSector<16; ++iSector) {
	for(int iSampling=0; iSampling<3; ++iSampling) {
	  moduleArray[iSide][iSector][iSampling] = new FCALHVModule(manager,iSide,iSector,iSampling);
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
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iSector=0; iSector<16; ++iSector) {
	for(int iSampling=0; iSampling<3; ++iSampling) {
	  delete moduleArray[iSide][iSector][iSampling];
	}
      }
    }
  }
  const FCALHVModule* moduleArray[2][16][3];
  std::atomic<bool>          init{false};
  std::mutex                 mtx;
  std::vector<FCALHVPayload> payloadArray;
  const LArElectrodeID* elecId;
  const LArHVLineID* hvId;
};

FCALHVManager::FCALHVManager()
  : m_c(new Clockwork(this))
{
}

FCALHVManager::~FCALHVManager()
{
  delete m_c;
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

void FCALHVManager::update() const {
  std::lock_guard<std::mutex> lock(m_c->mtx);
  if (!(m_c->init)) {
    m_c->init=true;
    m_c->payloadArray.reserve(2*16*3*4);
    for (unsigned int i=0;i<384;i++) {
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

        // 1. decode COOL Channel ID
        unsigned int chanID = (*citr).first;
        int cannode = chanID/1000;
        int line = chanID%1000;
        //std::cout << " cannode,line " << cannode << " " << line << std::endl;

        // 2. Construct the identifier
        HWIdentifier id = m_c->hvId->HVLineId(1,1,cannode,line);

        std::vector<HWIdentifier> electrodeIdVec = hvcablingTool->getLArElectrodeIDvec(id);

        for(size_t i=0;i<electrodeIdVec.size();i++) {

          HWIdentifier& elecHWID = electrodeIdVec[i];
          int detector = m_c->elecId->detector(elecHWID);
          if (detector==5) {

            //std::cout << " FCAl channel found " << (*citr).first << std::endl; 

            float voltage = -99999.;
            if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
            float current = 0.;
            if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();
            unsigned int status = 0;
            if (!((*citr).second)["R_STAT"].isNull()) status =  ((*citr).second)["R_STAT"].data<unsigned int>();
	    
            unsigned int sideIndex=1-m_c->elecId->zside(elecHWID);      // 0 C side, 1 A side (unline HV numbering)
            unsigned int samplingIndex=m_c->elecId->hv_eta(elecHWID)-1;   // 0 to 2 for the FCAL modules 1-2-3
            unsigned int sectorIndex=m_c->elecId->module(elecHWID);       // 0-15 FCAL1, 0-7 FCAl2, 0-3 FCAL3
            unsigned int lineIndex=m_c->elecId->gap(elecHWID);            // 0-3

            //std::cout << " channel found " << sideIndex << " " << samplingIndex << " " << sectorIndex << " " << lineIndex << " "<< voltage << std::endl;

            // do we have to worry about phi sector numbering running backwards in phi for z<0 like in EM/HEC  ????

            unsigned int index             = 192*sideIndex+12*sectorIndex+4*samplingIndex+lineIndex;

            if (index>384) {
              std::cout << " invalid index for FCAL " << sideIndex << " " << samplingIndex << " " << sectorIndex << " " << lineIndex << std::endl;
              continue;
            }
	    
	    
            m_c->payloadArray[index].voltage=voltage;
            m_c->payloadArray[index].current=current;
            m_c->payloadArray[index].status=status;
            m_c->payloadArray[index].hvLineNo=chanID;
          }   // if FCAL
        }  //   loop over electrodes
      }   // loop over collection
    }     // loop over folders
  }   // m_c->init
}

void FCALHVManager::reset() const {
  m_c->init=false;
}

FCALHVPayload *FCALHVManager::getPayload(const FCALHVLine &line) const {
  update();
  unsigned int lineIndex         = line.getLineIndex();
  const FCALHVModule& module     = line.getModule();
  unsigned int sectorIndex       = module.getSectorIndex();
  unsigned int sideIndex         = module.getSideIndex();
  unsigned int samplingIndex     = module.getSamplingIndex();
  unsigned int index             = 192*sideIndex+12*sectorIndex+4*samplingIndex+lineIndex;
  //std::cout << "in Fcal getPayload: " << this << ' ' << index << ' ' << sideIndex << ' ' << sectorIndex << ' ' << samplingIndex << ' ' << lineIndex << std::endl;
  return &m_c->payloadArray[index];
}


#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
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
