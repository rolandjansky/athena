/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/FCALHVManager.h"
#include "LArHV/FCALHVModule.h"
#include "LArHV/FCALHVLine.h"
#include "LArHV/FCALHVDescriptor.h"

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


class FCALHVManager::Clockwork {
public:
  FCALHVDescriptor *descriptor;
  bool init;
  FCALHVModuleConstLink linkArray[2][16][3];
  std::vector<FCALHVPayload> payloadArray;
};

//##ModelId=47ABAE9302D3
FCALHVManager::FCALHVManager():m_c(new Clockwork())
{
  m_c->descriptor = new FCALHVDescriptor();
  m_c->init=false;
}


//##ModelId=47ABAE930373
FCALHVManager::~FCALHVManager()
{
  delete m_c->descriptor;
  delete m_c;
}

//##ModelId=47ABAF5E0079
const FCALHVDescriptor *FCALHVManager::getDescriptor() const
{
  return m_c->descriptor;
}

//##ModelId=47ABAF5E0092
unsigned int FCALHVManager::beginSideIndex() const
{
  return 0;
}

//##ModelId=47ABAF5E009F
unsigned int FCALHVManager::endSideIndex() const
{
  return 2;
}

//##ModelId=47ABAF5E00AD
unsigned int FCALHVManager::beginSectorIndex(unsigned int /*iSampling*/) const
{
  return 0;
}

//##ModelId=47ABAF5E00BB
unsigned int FCALHVManager::endSectorIndex(unsigned int iSampling) const
{
  if (iSampling==0) return 16;
  if (iSampling==1) return 8;
  if (iSampling==2) return 4;
  return 0;
}

//##ModelId=47ABAF5E00C9
unsigned int FCALHVManager::beginSamplingIndex() const
{
  return 0;
}

//##ModelId=47ABAF5E00D9
unsigned int FCALHVManager::endSamplingIndex() const
{
  return 3;
}

//##ModelId=47ABAF5E00E8
FCALHVModuleConstLink FCALHVManager::getHVModule(unsigned int iSide, unsigned int iSector, unsigned int iSampling) const
{
  if (!m_c->linkArray[iSide][iSector][iSampling]) m_c->linkArray[iSide][iSector][iSampling]=FCALHVModuleConstLink(new FCALHVModule(this,iSide,iSector, iSampling));
  return m_c->linkArray[iSide][iSector][iSampling];
}

void FCALHVManager::update() const {
  if (!m_c->init) {
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
  FCALHVModuleConstLink module = line.getModule();
  unsigned int sectorIndex       = module->getSectorIndex();
  unsigned int sideIndex         = module->getSideIndex();
  unsigned int samplingIndex     = module->getSamplingIndex();
  unsigned int index             = 192*sideIndex+12*sectorIndex+4*samplingIndex+lineIndex;
  //std::cout << "in Fcal getPayload: " << this << ' ' << index << ' ' << sideIndex << ' ' << sectorIndex << ' ' << samplingIndex << ' ' << lineIndex << std::endl;
  return &m_c->payloadArray[index];
}

void FCALHVManager::reset() const {
  m_c->init=false;
}
