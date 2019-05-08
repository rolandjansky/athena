/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/FCALHVManager.h"
#include "LArHV/FCALHVLine.h"

#include "FCALHVPayload.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArCabling/LArHVCablingTool.h"
#include "Identifier/HWIdentifier.h"

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

    StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");

    const LArElectrodeID* elecId;
    if (StatusCode::SUCCESS!=detStore->retrieve(elecId, "LArElectrodeID")) return;

    const LArHVLineID* hvId;
    if (StatusCode::SUCCESS!=detStore->retrieve(hvId,"LArHVLineID"))
      return;

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
        HWIdentifier id = hvId->HVLineId(1,1,cannode,line);

        std::vector<HWIdentifier> electrodeIdVec = hvcablingTool->getLArElectrodeIDvec(id);

        for(size_t i=0;i<electrodeIdVec.size();i++) {

          HWIdentifier& elecHWID = electrodeIdVec[i];
          int detector = elecId->detector(elecHWID);
          if (detector==5) {

            //std::cout << " FCAl channel found " << (*citr).first << std::endl; 

            float voltage = -99999.;
            if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
            float current = 0.;
            if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();
            unsigned int status = 0;
            if (!((*citr).second)["R_STAT"].isNull()) status =  ((*citr).second)["R_STAT"].data<unsigned int>();
	    
            unsigned int sideIndex=1-elecId->zside(elecHWID);      // 0 C side, 1 A side (unline HV numbering)
            unsigned int samplingIndex=elecId->hv_eta(elecHWID)-1;   // 0 to 2 for the FCAL modules 1-2-3
            unsigned int sectorIndex=elecId->module(elecHWID);       // 0-15 FCAL1, 0-7 FCAl2, 0-3 FCAL3
            unsigned int lineIndex=elecId->gap(elecHWID);            // 0-3

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

void FCALHVManager::reset() const {
  m_c->init=false;
}
