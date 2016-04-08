/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVDescriptor.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVSubgap.h"
#include <cmath>

#include "HECHVPayload.h"

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

class HECHVManager::Clockwork {
public:
  HECHVDescriptor *descriptor;
  HECHVModuleConstLink linkArray[2][32][4];
  std::vector<HECHVPayload> payloadArray;     
  bool init;
};

//##ModelId=47A07A0C016B
HECHVManager::HECHVManager():m_c(new Clockwork)
{
  m_c->descriptor = new HECHVDescriptor(CellBinning(0,2*M_PI,32));
  m_c->init=false;
}


//##ModelId=47A07A0C0176
const HECHVDescriptor *HECHVManager::getDescriptor() const
{
  return m_c->descriptor;
}

//##ModelId=47A07AC901D3
unsigned int HECHVManager::beginSideIndex() const
{
  return 0;
}

//##ModelId=47A07AC901E4
unsigned int HECHVManager::endSideIndex() const
{
  return 2;
}

//##ModelId=47A07A0C01A1
unsigned int HECHVManager::beginPhiIndex() const
{
  return 0;
}

//##ModelId=47A07A0C01AD
unsigned int HECHVManager::endPhiIndex() const
{
  return 32;
}

//##ModelId=47A07A0C01BA
unsigned int HECHVManager::beginSamplingIndex() const
{
  return 0;
}

//##ModelId=47A07A0C01C9
unsigned int HECHVManager::endSamplingIndex() const
{
  return 4;
}

//##ModelId=47A07A0C01D7
HECHVModuleConstLink HECHVManager::getHVModule(unsigned int iSide, unsigned int iPhi, unsigned int iSampling) const
{
  if (!m_c->linkArray[iSide][iPhi][iSampling]) m_c->linkArray[iSide][iPhi][iSampling]=HECHVModuleConstLink(new HECHVModule(this,iSide, iPhi, iSampling));
  return m_c->linkArray[iSide][iPhi][iSampling];
}

//##ModelId=47A07A0C01E5
HECHVManager::~HECHVManager()
{
  delete m_c->descriptor;
  delete m_c;
}

void HECHVManager::update() const {
  if (!m_c->init) {
    m_c->init=true;
    m_c->payloadArray.reserve(2*32*4*4);
    for (unsigned int i=0;i<1024;i++) {
      m_c->payloadArray[i].voltage = -99999;
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

        // 2. Construct the identifier
        HWIdentifier id = hvId->HVLineId(1,1,cannode,line);

        std::vector<HWIdentifier> electrodeIdVec = hvcablingTool->getLArElectrodeIDvec(id);

	for(size_t i=0;i<electrodeIdVec.size();i++) { 
	    HWIdentifier& elecHWID = electrodeIdVec[i];

            int detector = elecId->detector(elecHWID);

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

	  
	    unsigned int sideIndex=1-elecId->zside(elecHWID);    // 0 for C side, 1 for A side, opposite to HV numbering
	    unsigned int phiIndex=elecId->module(elecHWID);      // 0 to 31
	    unsigned int samplingIndex=elecId->hv_eta(elecHWID)-1; // 0 to 3
	    unsigned int subgapIndex=elecId->gap(elecHWID);        // 0 to 3

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

HECHVPayload *HECHVManager::getPayload(const HECHVSubgap &subgap) const {
  update();
  unsigned int subgapIndex    = subgap.getSubgapIndex();
  HECHVModuleConstLink module =  subgap.getModule();
  unsigned int phiIndex       = module->getPhiIndex();
  unsigned int samplingIndex  = module->getSamplingIndex();
  unsigned int sideIndex      = module->getSideIndex();
  unsigned int index          = 512*sideIndex+16*phiIndex+4*samplingIndex+subgapIndex;
  return &m_c->payloadArray[index];
}

void HECHVManager::reset() const {
  m_c->init=false;
}
