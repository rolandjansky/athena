/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMBPresamplerHVDescriptor.h"
#include "GeoModelKernel/CellBinning.h"
#include "GeoModelKernel/CellPartitioning.h"
#include <cmath>
#include <vector>
#include <fstream>
#include "EMBPresamplerHVPayload.h"

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


class EMBPresamplerHVManager::Clockwork {
public:
  EMBPresamplerHVDescriptor      *descriptor;
  EMBPresamplerHVModuleConstLink  linkArray[2][4][32];
  bool                  init;
  std::vector<EMBPresamplerHVPayload> payloadArray;
};





EMBPresamplerHVManager::EMBPresamplerHVManager()
:m_c(new Clockwork)
{

  CellPartitioning etaPartitioning;
  for (unsigned int i= 0; i<4; i++)  etaPartitioning.addValue(i*0.4);
  etaPartitioning.addValue(1.525);
  
  m_c->descriptor = new EMBPresamplerHVDescriptor(etaPartitioning,CellBinning(0.0, 2*M_PI, 32));	
  m_c->init=false;

}


EMBPresamplerHVManager::~EMBPresamplerHVManager()
{
  delete m_c->descriptor;
  delete m_c;
}

const EMBPresamplerHVDescriptor *EMBPresamplerHVManager::getDescriptor() const
{
  return m_c->descriptor;
}

unsigned int EMBPresamplerHVManager::beginPhiIndex() const
{
  return m_c->descriptor->getPhiBinning().getFirstDivisionNumber();
}

unsigned int EMBPresamplerHVManager::endPhiIndex() const
{
  return m_c->descriptor->getPhiBinning().getFirstDivisionNumber() + m_c->descriptor->getPhiBinning().getNumDivisions();
}

unsigned int EMBPresamplerHVManager::beginEtaIndex() const
{
  return m_c->descriptor->getEtaPartitioning().getFirstDivisionNumber();
}

unsigned int EMBPresamplerHVManager::endEtaIndex() const
{
  return m_c->descriptor->getEtaPartitioning().getFirstDivisionNumber() + m_c->descriptor->getEtaPartitioning().getNumDivisions();
}

EMBPresamplerHVModuleConstLink EMBPresamplerHVManager::getHVModule(unsigned int iSide, unsigned int iEta,unsigned int iPhi) const
{
  if (!m_c->linkArray[iSide][iEta][iPhi]) m_c->linkArray[iSide][iEta][iPhi] = EMBPresamplerHVModuleConstLink(new EMBPresamplerHVModule(this, iSide, iEta,iPhi));
  return m_c->linkArray[iSide][iEta][iPhi];
}

unsigned int EMBPresamplerHVManager::beginSideIndex() const
{
  return 0;
}

unsigned int EMBPresamplerHVManager::endSideIndex() const
{
  return 2;
}

void EMBPresamplerHVManager::update() const {
  if (!m_c->init) {

    m_c->init=true;
    m_c->payloadArray.reserve(2*4*32);

    for (int i=0;i<256;i++) {
       m_c->payloadArray[i].voltage[0]=-99999.;
       m_c->payloadArray[i].voltage[1]=-99999.;
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


	for(size_t i=0;i<electrodeIdVec.size();i++)
	{
            HWIdentifier& elecHWID = electrodeIdVec[i];
            int detector = elecId->detector(elecHWID);
            if (detector==1) {

               //std::cout << "       in Barrel " << std::endl;

// side  in standard offline 0 for z<0 (C) 1 for z>0 (A)
//  in electrode numbering, this is the opposite (0 for A and 1 for C)
	    unsigned int sideIndex=1-elecId->zside(elecHWID);
// eta index, no trouble
	    unsigned int etaIndex=elecId->hv_eta(elecHWID)-1;
// phi index   
//  offline 0 to 2pi in 2pi/32 bins  (2 presampler cells per phi_HV)
//  module from elecID : 0 to 31
//    phi        0                                     pi                                            2pi
//               M0 M1  M2  M3  M4  M5   M6    M7    M8    M9    M10   M11   M12   M13   M14   M15   M0
//     FT        0 -1 0                                                                        -1 0  -1
//   Module      0  1 2 3 4 5 6 7 9 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 39 30 31
//  offline phi  0  1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
	    unsigned int phiIndex;
            if (sideIndex==1) {
             phiIndex=elecId->module(elecHWID);
            }
// module numbering on the C side 0 around phi=pi, running backwards
//   phi           0                                     pi                                            2pi
//                 P8 P7  P6  P5  P4  P3   P2    P1    P0    P15   P14   P13   P12   P11   P10   P9    P8
//     FT          -1 0                                0  -1                                      0 -1  0
// Module          15 14        9 8 7 6 5  4  3  2  1   0 31 30 29 38 27 26 25 24 23 22 21 20 19 18 17 16
//  offline phi    0  1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 
            else {
              int imodule=elecId->module(elecHWID);
              if (imodule<16) phiIndex = 15 - imodule;
              else           phiIndex =  47 - imodule;
            }

	    unsigned int index             =  128*sideIndex+32*etaIndex+phiIndex;
	  
	    unsigned int gapIndex=elecId->gap(elecHWID);
            if (sideIndex==0) gapIndex=1-gapIndex;


            float voltage = -99999.;
            if (!((*citr).second)["R_VMEAS"].isNull()) voltage = ((*citr).second)["R_VMEAS"].data<float>();
            float current = 0.;
            if (!((*citr).second)["R_IMEAS"].isNull()) current = ((*citr).second)["R_IMEAS"].data<float>();
            unsigned int status = 0;
            if (!((*citr).second)["R_STAT"].isNull()) status =  ((*citr).second)["R_STAT"].data<unsigned int>(); 


         // std::cout << "             hvlineId,elecHWID,cannode,line, side,phi module, sector,eta,electrode,gap,index " << std::hex << id << " " << elecHWID << std::dec << " " << cannode << " " << line << " " << elecId->zside(elecHWID) << " " << elecId->module(elecHWID) << " " << elecId->hv_phi(elecHWID) << " " << elecId->hv_eta(elecHWID) << " " << elecId->electrode(elecHWID)
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

EMBPresamplerHVPayload *EMBPresamplerHVManager::getPayload(const EMBPresamplerHVModule &module) const {
  update();
  unsigned int sideIndex         = module.getSideIndex();
  unsigned int phiIndex          = module.getPhiIndex();
  unsigned int etaIndex          = module.getEtaIndex();
  unsigned int index             =  128*sideIndex+32*etaIndex+phiIndex;
  return &m_c->payloadArray[index];
}

void  EMBPresamplerHVManager::reset() const {
  m_c->init=false;
}
