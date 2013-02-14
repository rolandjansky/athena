/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVDescriptor.h"
#include "LArHV/EMECHVElectrode.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

#include "EMECHVPayload.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArTools/LArHVCablingTool.h"

#include "Identifier/HWIdentifier.h"


class EMECHVManager::Clockwork {
public:
  EMECHVDescriptor *descriptor;
  IOType iWheel;
  EMECHVModuleConstLink linkArray[2][8][8][8]; // not dense
  bool                  init;
  std::vector<EMECHVPayload> payloadArray;
};


//##ModelId=43FBFC0A034F
EMECHVManager::EMECHVManager(IOType wheel):
  c(new Clockwork)
{
  c->init=false;
  c->iWheel=wheel;
  if (wheel==EMECHVModule::OUTER)  {
    CellPartitioning etaBinning;
    etaBinning.addValue(1.375);
    etaBinning.addValue(1.50);
    etaBinning.addValue(1.6);
    etaBinning.addValue(1.8);
    etaBinning.addValue(2.0);
    etaBinning.addValue(2.1);
    etaBinning.addValue(2.3);
    etaBinning.addValue(2.5);
    c->descriptor = new EMECHVDescriptor(etaBinning,CellBinning(0.0, 2*M_PI, 8),CellBinning(0,M_PI/4.0,4));
  }
  else if (wheel==EMECHVModule::INNER)  {
    CellPartitioning etaBinning;
    etaBinning.addValue(2.5);
    etaBinning.addValue(2.8);
    etaBinning.addValue(3.2);
    c->descriptor=new EMECHVDescriptor(etaBinning,CellBinning(0.0, 2*M_PI, 8),CellBinning(0,M_PI/4.0,8));
  }
  else {
    c->descriptor=NULL;
    throw std::runtime_error ("EMECHVManager:  unknown Wheel Type");
  }
}



//##ModelId=478D1079010F
const EMECHVDescriptor *EMECHVManager::getDescriptor() const
{
  return c->descriptor;
}

//##ModelId=478D10790120
unsigned int EMECHVManager::beginPhiIndex() const
{
  return c->descriptor->getPhiBinning().getFirstDivisionNumber();
}

//##ModelId=478D10790129
unsigned int EMECHVManager::endPhiIndex() const
{
  return c->descriptor->getPhiBinning().getFirstDivisionNumber() + c->descriptor->getPhiBinning().getNumDivisions();
}

//##ModelId=478D10790133
unsigned int EMECHVManager::beginEtaIndex() const
{
  return c->descriptor->getEtaBinning().getFirstDivisionNumber();
}

//##ModelId=478D1079013E
unsigned int EMECHVManager::endEtaIndex() const
{
  return c->descriptor->getEtaBinning().getFirstDivisionNumber() + c->descriptor->getEtaBinning().getNumDivisions();
}

//##ModelId=478D10790149
EMECHVModuleConstLink EMECHVManager::getHVModule(unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector) const
{

  if (!c->linkArray[iSide][iEta][iPhi][iSector]) c->linkArray[iSide][iEta][iPhi][iSector] = EMECHVModuleConstLink(new EMECHVModule(this, iSide, iEta,iPhi,iSector));
  return c->linkArray[iSide][iEta][iPhi][iSector];

}

//##ModelId=478D10790154
EMECHVManager::~EMECHVManager()
{
  delete c->descriptor;
  delete c;
}


//##ModelId=47A07A81015E
unsigned int EMECHVManager::beginSideIndex() const
{
  return 0;
}

//##ModelId=47A07A81016F
unsigned int EMECHVManager::endSideIndex() const
{
  return 2;
}

//##ModelId=47A07AA60006
unsigned int EMECHVManager::beginSectorIndex() const
{
  return c->descriptor->getSectorBinning().getFirstDivisionNumber();
}

//##ModelId=47A07AA6006B
unsigned int EMECHVManager::endSectorIndex() const
{
  return c->descriptor->getSectorBinning().getFirstDivisionNumber() + c->descriptor->getSectorBinning().getNumDivisions();
}

//##ModelId=47A26D390123
EMECHVManager::IOType EMECHVManager::getWheelIndex() const
{
  return c->iWheel;
}

void EMECHVManager::update() const {
  if (!c->init) {
    c->init=true;
    if (c->iWheel==EMECHVModule::OUTER)      {
        c->payloadArray.reserve(2*7*8*4*24);
        for (unsigned int i=0;i<10752;i++) {
          c->payloadArray[i].voltage[0]=-99999;
          c->payloadArray[i].voltage[1]=-99999;
        }
    }
    else if (c->iWheel==EMECHVModule::INNER)  {
        c->payloadArray.reserve(2*2*8*8*4);
        for (unsigned int i=0;i<1024;i++) {
          c->payloadArray[i].voltage[0]=-99999;
          c->payloadArray[i].voltage[1]=-99999;
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
        unsigned int _chanID = (*citr).first;
        int _cannode = _chanID/1000;
        int _line = _chanID%1000;
        //std::cout << "    ++ found data for cannode, line " << _cannode << " " << _line << std::endl;

        // 2. Construct the identifier
        HWIdentifier id = hvId->HVLineId(1,1,_cannode,_line);


        std::vector<HWIdentifier> electrodeIdVec = hvcablingTool->getLArElectrodeIDvec(id);

        for(size_t i=0;i<electrodeIdVec.size();i++) {
            HWIdentifier& elecHWID = electrodeIdVec[i];
            int detector = elecId->detector(elecHWID);
            // check we are in EMEC
            if (detector==2) {


	    unsigned int etaIndex=elecId->hv_eta(elecHWID);

	    if ( (etaIndex>6 && c->iWheel==EMECHVModule::INNER) || (etaIndex<7 && c->iWheel==EMECHVModule::OUTER) ) {
	      unsigned int sideIndex=1-elecId->zside(elecHWID);
	      unsigned int phiIndex=elecId->module(elecHWID);      // 0 to 7
// rotation for C side
              if (sideIndex==0) {
                  if (phiIndex<4) phiIndex=3-phiIndex;
                  else phiIndex=11-phiIndex;
              }
	      unsigned int sectorIndex=elecId->hv_phi(elecHWID)-1;    // 0 to 3 in Outer, 0 to 7 in Inner
// rotation for C side
              if (sideIndex==0) {
                  if (c->iWheel==EMECHVModule::OUTER) sectorIndex=3-sectorIndex;
                  else  sectorIndex=7-sectorIndex;
              } 
	      unsigned int electrodeIndex = c->iWheel==EMECHVModule::OUTER ?
                   elecId->electrode(elecHWID)%24:
                   elecId->electrode(elecHWID)%4;
// rotation for C side
              if (sideIndex==0) {
                  if (c->iWheel==EMECHVModule::OUTER) electrodeIndex=23-electrodeIndex;
                  else  electrodeIndex=3-electrodeIndex;
              } 
	  
	      unsigned int index             = c->iWheel==EMECHVModule::OUTER ? 
	        5376*sideIndex+768*etaIndex+96*phiIndex+24*sectorIndex+electrodeIndex:
	         512*sideIndex+256*(etaIndex-7)+32*phiIndex+4*sectorIndex+electrodeIndex;

              if (c->iWheel==EMECHVModule::OUTER && index>10752) {
                std::cout << "invalid index outer " << index << " side,eta,phi,sector,electrode " << sideIndex << " " << etaIndex << " " << phiIndex <<
                 " " << sectorIndex << " " << electrodeIndex << std::endl;
                continue;
              }
              if (c->iWheel==EMECHVModule::INNER && index>1024) {
                std::cout << "invalid index inner " << index << " side,eta,phi,sector,electrode " << sideIndex << " " << etaIndex << " " << phiIndex <<
                 " " << sectorIndex << " " << electrodeIndex << std::endl;
                continue;
              }
	  
	      unsigned int gapIndex=elecId->gap(elecHWID);
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

              //std::cout << "             hvlineId,elecHWID,cannode,line, side,phi module, sector,eta,electrode,gap,index " << std::hex << id << " " << elecHWID << std::dec << " " << _cannode << " " << _line << " " << elecId->zside(elecHWID) << " " << elecId->module(elecHWID) << " " << elecId->hv_phi(elecHWID) << " " << elecId->hv_eta(elecHWID) << " " << elecId->electrode(elecHWID)
               // << " " << gapIndex << "  " << index << " " << voltage << std::endl;

	  
	      c->payloadArray[index].voltage[gapIndex]=voltage;
	      c->payloadArray[index].current[gapIndex]=current;
	      c->payloadArray[index].status[gapIndex]=status;
	      c->payloadArray[index].hvLineNo[gapIndex]=_chanID;
	    } // if etaIndex...
	  } // for (electrodeIdVec)
        } // is EMEC
      } // for (atrlistcol)
    }
  } // if(!c->init)
}




EMECHVPayload *EMECHVManager::getPayload(const EMECHVElectrode &electrode) const {
  update();
  unsigned int electrodeIndex    = electrode.getElectrodeIndex();
  EMECHVModuleConstLink module = electrode.getModule();
  unsigned int etaIndex          = module->getEtaIndex();
  unsigned int phiIndex          = module->getPhiIndex();
  unsigned int sectorIndex       = module->getSectorIndex();
  unsigned int sideIndex         = module->getSideIndex();

  unsigned int index=0;
  if (c->iWheel==EMECHVModule::OUTER)      index= 5376*sideIndex+768*etaIndex+96*phiIndex+24*sectorIndex+electrodeIndex;
  else if (c->iWheel==EMECHVModule::INNER) index= 512*sideIndex+256*etaIndex+32*phiIndex+4*sectorIndex+electrodeIndex;
  return &c->payloadArray[index];
}

void EMECHVManager::reset() const {
  c->init=false;
}
