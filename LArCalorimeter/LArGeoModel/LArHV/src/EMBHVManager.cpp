/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/EMBHVManager.h"
#include "LArHV/EMBHVDescriptor.h"
#include "LArHV/EMBHVElectrode.h"
#include "GeoModelKernel/CellBinning.h"
#include <cmath>
#include <vector>
#include "EMBHVPayload.h"

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


class EMBHVManager::Clockwork {
public:
  EMBHVDescriptor      *descriptor;
  EMBHVModuleConstLink  linkArray[2][8][16][2];
  bool                  init;
  std::vector<EMBHVPayload> payloadArray;
};





EMBHVManager::EMBHVManager()
:m_c(new Clockwork)
{
  m_c->descriptor = new EMBHVDescriptor(CellBinning(0.0, 1.4, 7, 1),CellBinning(0.0, 2*M_PI, 16));	
  m_c->init=false;
  
}


EMBHVManager::~EMBHVManager()
{
  delete m_c->descriptor;
  delete m_c;
}

const EMBHVDescriptor *EMBHVManager::getDescriptor() const
{
  return m_c->descriptor;
}

unsigned int EMBHVManager::beginPhiIndex() const
{
  return m_c->descriptor->getPhiBinning().getFirstDivisionNumber();
}

unsigned int EMBHVManager::endPhiIndex() const
{
  return m_c->descriptor->getPhiBinning().getFirstDivisionNumber() + m_c->descriptor->getPhiBinning().getNumDivisions();
}

unsigned int EMBHVManager::beginEtaIndex() const
{
  return m_c->descriptor->getEtaBinning().getFirstDivisionNumber();
}

unsigned int EMBHVManager::endEtaIndex() const
{
  return m_c->descriptor->getEtaBinning().getFirstDivisionNumber() + m_c->descriptor->getEtaBinning().getNumDivisions();
}

EMBHVModuleConstLink EMBHVManager::getHVModule(unsigned int iSide, unsigned int iEta,unsigned int iPhi, unsigned int iSector) const
{
  if (!m_c->linkArray[iSide][iEta][iPhi][iSector]) m_c->linkArray[iSide][iEta][iPhi][iSector] = EMBHVModuleConstLink(new EMBHVModule(this, iSide, iEta,iPhi,iSector));
  return m_c->linkArray[iSide][iEta][iPhi][iSector];
}

unsigned int EMBHVManager::beginSectorIndex() const
{
  return 0;
}

unsigned int EMBHVManager::endSectorIndex() const
{
  return 2;
}

unsigned int EMBHVManager::beginSideIndex() const
{
  return 0;
}

unsigned int EMBHVManager::endSideIndex() const
{
  return 2;
}

void EMBHVManager::update() const {
  if (!m_c->init) {

    m_c->init=true;
    m_c->payloadArray.reserve(2*8*16*2*32);

    for (int i=0;i<16384;i++) {
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
            if (detector==0) {

               //std::cout << "       in Barrel " << std::endl;

// side  in standard offline 0 for z<0 (C) 1 for z>0 (A)
//  in electrode numbering, this is the opposite (0 for A and 1 for C)
	    unsigned int sideIndex=1-elecId->zside(elecHWID);
// eta index, no trouble
	    unsigned int etaIndex=elecId->hv_eta(elecHWID);
// phi index   
//  offline 0 to 2pi in 2pi/16 bins
// this is module in the electrode numbering: on the A side 0 to 15, 0 is halfway around phi=0 (FT-1 (hv_phi=1 is a lower phi)
//   offline phi      0               pi                    2pi
//   Module           M0 M1 M1                          M15 M0
//   FT               0 -1  0                           0  -1
//   hv_phi           0  1  0                           0   1
//   phiIndex         0  0  1                          15   15
// sector Index       0  1  0                           0   1
	    unsigned int phiIndex;
            unsigned int sectorIndex;
            if (sideIndex==1) {
             phiIndex=elecId->module(elecHWID);
             sectorIndex=elecId->hv_phi(elecHWID);
            }
// module numbering on the C side 0 around phi=pi, running backwards
//   offline phi     0               pi                      2pi
//   Module          P8 P7 P7      P0 P0                 P9 P8
//   FT              -1 0 -1       0  -1                -1  0
//   hv_phi           1 0  1       0   1                 1   0
//  phiIndex          0 0  1       7   8                15  15
// sectorIndex        0 1  0       1   0                 0  1
            else {
              int imodule=elecId->module(elecHWID);
              if (imodule<9) phiIndex = 8 - imodule;
              else           phiIndex = 24 - imodule;
              sectorIndex = 1-elecId->hv_phi(elecHWID);
            }

            if (sectorIndex==1) {
               if (phiIndex>0) phiIndex = phiIndex - 1;
               else phiIndex=15;
            }

	    unsigned int electrodeIndex=elecId->electrode(elecHWID);
            if (sideIndex==0) {
               if (elecId->hv_phi(elecHWID)==1) electrodeIndex=31-electrodeIndex;  // FT-1 change 0->31 to 31->0
               else                             electrodeIndex=63-electrodeIndex;  // FT 0 change 32->63 to 31-0
            }
            else {
                if (elecId->hv_phi(elecHWID)==0) electrodeIndex=electrodeIndex-32;  // FT 0 change 31-63 to 0-31
            }
	  
	    unsigned int index             = 8192*sideIndex+1024*etaIndex+64*phiIndex+32*sectorIndex+electrodeIndex;
	  
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

EMBHVPayload *EMBHVManager::getPayload(const EMBHVElectrode &electrode) const {
  update();
  unsigned int electrodeIndex    = electrode.getElectrodeIndex();
  EMBHVModuleConstLink module = electrode.getModule();
  unsigned int etaIndex          = module->getEtaIndex();
  unsigned int phiIndex          = module->getPhiIndex();
  unsigned int sectorIndex       = module->getSectorIndex();
  unsigned int sideIndex         = module->getSideIndex();
  unsigned int index             =  8192*sideIndex+1024*etaIndex+64*phiIndex+32*sectorIndex+electrodeIndex;
  return &m_c->payloadArray[index];
}

void  EMBHVManager::reset() const {
  m_c->init=false;
}
