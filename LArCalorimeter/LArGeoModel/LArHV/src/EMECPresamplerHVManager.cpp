/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

#include "EMECPresamplerHVPayload.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArCabling/LArHVCablingTool.h"

#include "Identifier/HWIdentifier.h"
#include "GeoModelKernel/CellBinning.h"

#include <atomic>

class EMECPresamplerHVManager::Clockwork {
public:
  Clockwork(const EMECPresamplerHVManager* manager) {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iPhi=0; iPhi<64; ++iPhi) {
	moduleArray[iSide][iPhi] = new EMECPresamplerHVModule(manager, iSide, iPhi);
      }
    }
  }
  ~Clockwork() {
    for(int iSide=0; iSide<2; ++iSide) {
      for(int iPhi=0; iPhi<64; ++iPhi) {
	delete moduleArray[iSide][iPhi];
      }
    }    
  }
  CellBinning                   phiBinning{0.0, 2*M_PI, 64};
  const EMECPresamplerHVModule* moduleArray[2][64]; // not dense
  std::atomic<bool>             init{false};
  std::mutex                    mtx;
  std::vector<EMECPresamplerHVPayload> payloadArray;
};

EMECPresamplerHVManager::EMECPresamplerHVManager()
  : m_c(new Clockwork(this))
{
}

EMECPresamplerHVManager::~EMECPresamplerHVManager()
{
  delete m_c;
}

const CellBinning *EMECPresamplerHVManager::getPhiBinning() const
{
  return &(m_c->phiBinning);
}

unsigned int EMECPresamplerHVManager::beginPhiIndex() const
{
  return m_c->phiBinning.getFirstDivisionNumber();
}

unsigned int EMECPresamplerHVManager::endPhiIndex() const
{
  return m_c->phiBinning.getFirstDivisionNumber() + m_c->phiBinning.getNumDivisions();
}

const EMECPresamplerHVModule& EMECPresamplerHVManager::getHVModule(unsigned int iSide, unsigned int iPhi) const
{
  return *(m_c->moduleArray[iSide][iPhi]);
}

unsigned int EMECPresamplerHVManager::beginSideIndex() const
{
  return 0;
}

unsigned int EMECPresamplerHVManager::endSideIndex() const
{
  return 2;
}

void EMECPresamplerHVManager::update() const {
  std::lock_guard<std::mutex> lock(m_c->mtx);
  if (!(m_c->init)) {
    m_c->init=true;
    {
        m_c->payloadArray.reserve(2*64);
        for (unsigned int i=0;i<64;i++) {
          m_c->payloadArray[i].voltage[0]=-99999;
          m_c->payloadArray[i].voltage[1]=-99999;
        }
    }
    
    StoreGateSvc *detStore = StoreGate::pointer("DetectorStore");

    const LArElectrodeID* elecId;
    if (StatusCode::SUCCESS!=detStore->retrieve(elecId, "LArElectrodeID")) 
      return;

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
	 
        // Construct HWIdentifier
        // 1. decode COOL Channel ID
        unsigned int chanID = (*citr).first;
        int cannode = chanID/1000;
        int line = chanID%1000;
        //std::cout << "    ++ found data for cannode, line " << cannode << " " << line << std::endl;

        // 2. Construct the identifier
        HWIdentifier id = hvId->HVLineId(1,1,cannode,line);


        std::vector<HWIdentifier> electrodeIdVec = hvcablingTool->getLArElectrodeIDvec(id);

        for(size_t i=0;i<electrodeIdVec.size();i++) {
            HWIdentifier& elecHWID = electrodeIdVec[i];
            int detector = elecId->detector(elecHWID);
            // check we are in EMECPresampler
            if (detector==3) {


	      unsigned int sideIndex=1-elecId->zside(elecHWID);
	      unsigned int phiIndex=elecId->module(elecHWID);        // from 0 to 31
// rotation for C side
              if (sideIndex==0) {
                  if (phiIndex<16) phiIndex=15-phiIndex;
                  else phiIndex=47-phiIndex;
              }

// GU  January 2017  -   fix for HV EMEC PS distribution
// 0-31 in phi module
// each module has 2 cell in phi, in the mapping database this is referred by "gapIndex" 
//   0 is on the low phi side (in the ATLAS frame)
//   1 in on the  high phi side
// so in total 64 sectors in phi given by 2*phiIndex+gapIndex
// the two gap of these sectors are powered by the same line and have the same HV
    
            unsigned int gapIndex=elecId->gap(elecHWID);
 
            unsigned int index = 64*sideIndex+2*phiIndex+gapIndex;

	      
            float voltage = -99999.;
            if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
            float current = 0.;
            if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();
            unsigned int status = 0;
            if (!((*citr).second)["R_STAT"].isNull()) status =  ((*citr).second)["R_STAT"].data<unsigned int>(); 


              for (unsigned int gap=0;gap<2;gap++) {
	        m_c->payloadArray[index].voltage[gap]=voltage;
	        m_c->payloadArray[index].current[gap]=current;
	        m_c->payloadArray[index].status[gap]=status;
	        m_c->payloadArray[index].hvLineNo[gap]=chanID;
              }
	  } // for (electrodeIdVec)
        } // is EMECPresampler
      } // for (atrlistcol)
    }
  } // if(!m_c->init)
}




EMECPresamplerHVPayload *EMECPresamplerHVManager::getPayload(const EMECPresamplerHVModule & module) const {
  update();
  unsigned int phiIndex          = module.getPhiIndex();
  unsigned int sideIndex         = module.getSideIndex();

  unsigned int index = 64*sideIndex+phiIndex;

  return &m_c->payloadArray[index];
}

void EMECPresamplerHVManager::reset() const {
  m_c->init=false;
}
