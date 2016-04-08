/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdint.h>
#include "TrigT1TRT/TrigT1TRT.h"
#include "TrigT1Interfaces/TrtCTP.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include "InDetRawData/TRT_RDORawData.h"
#include "InDetRawData/TRT_RDO_Collection.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

namespace LVL1 {
  
  TrigT1TRT::TrigT1TRT(const std::string& name,
		       ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator), 
						  m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
						  m_TRTL1ContainerName("TRT_RDOs"),
						  m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc",name)

  {
    declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
    declareProperty("TRTL1ContainerName",     m_TRTL1ContainerName = "TRT_RDOs");
    declareProperty("BoardMultiplicity",      m_BoardMultiplicity    =  1); // board multiplicity
    declareProperty("TTCMultiplicity",        m_TTCMultiplicity    =  5); // trigger multiplicity
  }

  StatusCode TrigT1TRT::initialize() {
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Initialising" << endreq;

    if (Algorithm::initialize().isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't initialize Algorithm base class." << endreq;
      return StatusCode::FAILURE;
    }

    StatusCode sc;

    // Connect to the LVL1ConfigSvc to retrieve threshold settings.
    sc = m_configSvc.retrieve();
    if (sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName() << endreq;
      return sc;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Connected to " << m_configSvc.typeAndName() << endreq;
    }

    // retrieve TRT Detector Manager
    sc = detStore()->retrieve(mgr, "TRT");
    if (sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't connect to TRT DetectorManager" << endreq;
      return sc;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Connected to TRT DetectorManager" << endreq;
    }

    // retrieve TRT Helper
    sc = detStore()->retrieve(m_pTRTHelper,"TRT_ID");
    if (sc.isFailure()) {
       if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Couldn't connect to TRT Helper" << endreq;
       return sc;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Connected to TRT Helper" << endreq;
    }

    // retrieve TRT_StrawNeighbourService.
    if (m_TRTStrawNeighbourSvc.name() == "") {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "TRT_StrawNeighbourSvc not given." << endreq;
    } else { sc = m_TRTStrawNeighbourSvc.retrieve();
      if(sc.isFailure()){
        if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Could not get StrawNeighbourSvc!" << endreq;
      }
    }
       
    // Get level 1 TRT threshold settings from the level 1
    // configuration service.
    m_cablestarts[0] = 0; //TRT Fast-OR
    std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->thresholdConfig()->getThresholdVector(TrigConf::L1DataDef::NIM);
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr = thresholds.begin();
    std::vector<TrigConf::TriggerThreshold*>::iterator th_itr_end = thresholds.end();
    for(;th_itr!=th_itr_end;th_itr++) {
      if(*th_itr == 0) continue;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "1-bit Threshold name=" << (*th_itr)->name() << endreq;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "1-bit Threshold value=" << (*th_itr)->triggerThresholdValue(0, 0)->ptcut() << endreq;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "1-bit Thershold cablestart=" << (*th_itr)->cableStart() << endreq;
      if        ((*th_itr)->name() == "TRTfast") {
        m_cablestarts[0]=(*th_itr)->cableStart();
      }
    }

      
    if(thresholds.size()==0)
      {
	msg(MSG::WARNING) << "No TRT thresholds found. Checking NIM thresholds for known TRT thresholds. THIS IS A HACK!"<<endreq;
	thresholds = m_configSvc->thresholdConfig()->getThresholdVector(TrigConf::L1DataDef::NIM);
	th_itr = thresholds.begin();
	th_itr_end = thresholds.end();
	for(;th_itr!=th_itr_end;th_itr++) {
      if(*th_itr == 0) continue;
	  if        ((*th_itr)->name() == "NIMDIR25") {
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "1-bit Threshold name=" << (*th_itr)->name() << endreq;
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "1-bit Threshold value=" << (*th_itr)->triggerThresholdValue(0, 0)->ptcut() << endreq;
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "1-bit Thershold cablestart=" << (*th_itr)->cableStart() << endreq;
	    m_cablestarts[0]=(*th_itr)->cableStart();
	  }
	}
      }


    //Initialize numberOfStraws array for use in strawNumber function.
    for(int j=0; j<75; j++) {
      numberOfStraws[j]=0;
    }
    
    // numberofstraws in a phi module layer. numberOfStraws[<layer number>]
    numberOfStraws[1]=15;
    numberOfStraws[2]=numberOfStraws[3]=numberOfStraws[4]=numberOfStraws[5]=16;
    numberOfStraws[6]=numberOfStraws[7]=numberOfStraws[8]=numberOfStraws[9]=numberOfStraws[10]=17;
    numberOfStraws[11]=numberOfStraws[12]=numberOfStraws[13]=numberOfStraws[14]=numberOfStraws[15]=18;
    numberOfStraws[16]=numberOfStraws[17]=numberOfStraws[18]=19;
    numberOfStraws[19]=18;
    numberOfStraws[20]=19;
    numberOfStraws[21]=numberOfStraws[22]=numberOfStraws[23]=numberOfStraws[24]=numberOfStraws[25]=20;
    numberOfStraws[26]=numberOfStraws[27]=numberOfStraws[28]=numberOfStraws[29]=numberOfStraws[30]=21;
    numberOfStraws[31]=numberOfStraws[32]=numberOfStraws[33]=numberOfStraws[34]=numberOfStraws[35]=22;
    numberOfStraws[36]=numberOfStraws[37]=numberOfStraws[38]=numberOfStraws[39]=numberOfStraws[40]=23;
    numberOfStraws[41]=numberOfStraws[42]=24;
    numberOfStraws[43]=23;
    numberOfStraws[44]=23;
    numberOfStraws[45]=numberOfStraws[46]=numberOfStraws[47]=numberOfStraws[48]=24;
    numberOfStraws[49]=numberOfStraws[50]=numberOfStraws[51]=numberOfStraws[52]=numberOfStraws[53]=25;
    numberOfStraws[54]=numberOfStraws[55]=numberOfStraws[56]=numberOfStraws[57]=numberOfStraws[58]=26;
    numberOfStraws[59]=numberOfStraws[60]=numberOfStraws[61]=numberOfStraws[62]=numberOfStraws[63]=27;
    numberOfStraws[64]=numberOfStraws[65]=numberOfStraws[66]=numberOfStraws[67]=numberOfStraws[68]=28;
    numberOfStraws[69]=numberOfStraws[70]=numberOfStraws[71]=numberOfStraws[72]=29;
    numberOfStraws[73]=28;

    //loop over straw hash index to create straw number mapping for TRTViewer
    unsigned int maxHash = m_pTRTHelper->straw_layer_hash_max();
    for (unsigned int index = 0; index < maxHash; index++) {
      IdentifierHash idHash = index;
      Identifier id = m_pTRTHelper->layer_id(idHash);

      int idBarrelEndcap = m_pTRTHelper->barrel_ec(id);
      int idLayerWheel = m_pTRTHelper->layer_or_wheel(id);
      int idPhiModule = m_pTRTHelper->phi_module(id);
      int idStrawLayer = m_pTRTHelper->straw_layer(id);

      const InDetDD::TRT_BaseElement * element= NULL;

      if (m_pTRTHelper->is_barrel(id)) {
        
	// BARREL
        int idSide = idBarrelEndcap?1:-1;
	if(m_pTRTHelper->barrel_ec(id)==-1) {
          element = mgr->getBarrelElement(idSide, idLayerWheel, idPhiModule, idStrawLayer);
	  if (element == NULL) {
	    continue;
	  }

          for(unsigned int istraw = 0; istraw < element->nStraws(); istraw++) {
            if(istraw>element->nStraws()) {
              continue;
            }

            Identifier strawID = m_pTRTHelper->straw_id(id, int(istraw));
            int i_chip;

            int tempStrawNumber = BarrelStrawNumber(istraw, idStrawLayer, idLayerWheel);

            m_TRTStrawNeighbourSvc->getChip(strawID,i_chip);

            //21 chips in mod layer 0
            //33 chips in mod layer 1
            //50 chips in mod layer 2
            if (idLayerWheel == 1) i_chip+=21;
            if (idLayerWheel == 2) i_chip+=54;

            mat_chip_barrel[idPhiModule][tempStrawNumber]=i_chip;
            mat_chip_barrel[idPhiModule+32][tempStrawNumber]=i_chip;
          }
        }
      } else if (m_pTRTHelper->barrel_ec(id)!=1) {
      
        // ENDCAP
	int idSide = idBarrelEndcap?2:-2;
        if(((m_pTRTHelper->barrel_ec(id)==-2) || (m_pTRTHelper->barrel_ec(id)==2))) {

	  if (m_pTRTHelper->barrel_ec(id)==-2) idSide =0;
          else idSide=1;

          element = mgr->getEndcapElement(idSide, idLayerWheel, idStrawLayer, idPhiModule);
	  if (element == NULL) {
            continue;
	  }
	  
	  for(unsigned int istraw = 0; istraw < element->nStraws(); istraw++) {
            if(istraw>element->nStraws()) {
              continue;
            }
  
            int tempStrawNumber = EndcapStrawNumber(istraw, idStrawLayer, idLayerWheel, idPhiModule, idSide);
	    
	    Identifier strawID = m_pTRTHelper->straw_id(id, int(istraw));

            int i_chip = 0;

            m_TRTStrawNeighbourSvc->getChip(strawID,i_chip);
            ++i_chip -= 104;

            mat_chip_endcap[idPhiModule][tempStrawNumber]=i_chip;
            mat_chip_endcap[idPhiModule+32][tempStrawNumber]=i_chip;
          }
	}
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode TrigT1TRT::execute() {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "execute()" << endreq;
    
    StatusCode sc;
    std::string containerName;
    
    int m_barrel_trigger_board[2][32][9];
    int m_endcap_trigger_board[2][32][20];

    // empty board score board 
    for (int i=0; i<2; i++) {
      for (int j=0; j<32; j++) {
        for (int k=0; k<9; k++) {
	  m_barrel_trigger_board[i][j][k] = 0;
        }
        for (int k=0; k<20; k++) {
	  m_endcap_trigger_board[i][j][k] = 0;	  
	}
      }
    }

    int m_barrel_trigger_ttc[2][8];
    int m_endcap_trigger_ttc[2][16];

    // empty board score board 
    for (int i=0; i<2; i++) {
      for (int j=0; j<8; j++) {
	m_barrel_trigger_ttc[i][j] = 0;
      }
      for (int j=0; j<16; j++) {
	m_endcap_trigger_ttc[i][j] = 0;
      }
    }
      
    //    Retrieve Lvl1 TRT container
    containerName = m_TRTL1ContainerName;
    m_TRTRDO = 0;
    sc = evtStore()->retrieve( m_TRTRDO, containerName);
    if( sc.isFailure()  || !m_TRTRDO ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << containerName << " not found" << endreq; 
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "This event will be skipped." << endreq;
      return StatusCode::SUCCESS;
    }
    else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << containerName << " Container Successfully Retrieved" 
    	    << endreq;
    }
    
    // Loop over all Lvl 1 TRT hits
    Identifier TRT_Identifier;
    TRT_RDO_Container::const_iterator RDO_CollectionBegin = m_TRTRDO->begin();
    TRT_RDO_Container::const_iterator RDO_CollectionEnd   = m_TRTRDO->end();

    for(;RDO_CollectionBegin!=RDO_CollectionEnd; ++RDO_CollectionBegin) {
      //Get pointer to TRT_Collection
      const InDetRawDataCollection<TRT_RDORawData>* TRT_Collection(*RDO_CollectionBegin);
 
      // select only TRT RDOs
      if(!TRT_Collection) {
        continue;
      } else {
        DataVector<TRT_RDORawData>::const_iterator p_rdo;//pointer to trt rdo data vector
        // Loop over TRT RDOs
        for(p_rdo=TRT_Collection->begin(); p_rdo!=TRT_Collection->end(); ++p_rdo) {
          TRT_Identifier = (*p_rdo)->identify();
          int m_barrel_ec = m_pTRTHelper->barrel_ec(TRT_Identifier);

          const TRT_LoLumRawData* p_lolum=dynamic_cast<const TRT_LoLumRawData*>(*p_rdo);
          if(!p_lolum) continue;
 
          //Get TRT Identifier (need to know phi module, module layer, straw layer, and straw # with in the layer, to get proper straw numbering.
          TRT_Identifier = (p_lolum)->identify();
          int m_phi_module = m_pTRTHelper->phi_module(TRT_Identifier);
          int m_layer_or_wheel = m_pTRTHelper->layer_or_wheel(TRT_Identifier);
          int m_straw_layer = m_pTRTHelper->straw_layer(TRT_Identifier);
          int m_straw = m_pTRTHelper->straw(TRT_Identifier);      
          int m_strawNumber = 0;
          int m_chip = 0;
          int m_board = 0;
          
	  if (m_barrel_ec == 1 || m_barrel_ec == -1) {
	  
            int m_side = m_barrel_ec>0?1:0; 
	    m_strawNumber = BarrelStrawNumber(m_straw, m_straw_layer, m_layer_or_wheel);
            m_chip = mat_chip_barrel[m_phi_module][m_strawNumber];
            m_board = BarrelChipToBoard(m_chip);
            if (m_board < 0) { 
              if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Failure in BarrelChipToBoard" << endreq;
              return StatusCode::FAILURE;
            }

	    
	    if ((p_lolum)->trailingEdge() - (p_lolum)->driftTimeBin() > 4 ) {
              m_barrel_trigger_board[m_side][m_phi_module][m_board]++;
            }
	    
          } else if (m_barrel_ec == 2 || m_barrel_ec == -2) {

            int m_side = m_barrel_ec>0?1:0;
            m_strawNumber = EndcapStrawNumber(m_straw, m_straw_layer, m_layer_or_wheel, m_phi_module, m_barrel_ec);
            m_chip = mat_chip_endcap[m_phi_module][m_strawNumber];       
            m_board = EndcapChipToBoard(m_chip);
		    
	    if ((p_lolum)->trailingEdge() - (p_lolum)->driftTimeBin() > 4 ) {
              m_endcap_trigger_board[m_side][m_phi_module][m_board]++;
            }    
	    
          }
        }
      }    
    }
    
    // analyse board score board - fill ttc score board
    for (int i=0; i<2; i++) {
      for (int j=0; j<32; j++) {
        for (int k=0; k<9; k++) {
	  if (m_barrel_trigger_board[i][j][k] >= m_BoardMultiplicity) {
            m_barrel_trigger_ttc[i][j%4]++;
	  }
        }
        for (int k=0; k<20; k++) {
	  if (m_endcap_trigger_board[i][j][k] >= m_BoardMultiplicity) {
            m_endcap_trigger_ttc[i][j%2]++;
	  }	  
	}
      }
    }

    uint32_t cableWord0 = 0;

    // analyse ttc score board - fill ttc score board
    for (int i=0; i<2; i++) {
      for (int j=0; j<8; j++) {
        if (m_barrel_trigger_ttc[i][j] >= m_TTCMultiplicity) {
	  cableWord0 = 1 << m_cablestarts[0];
	}
      }
      for (int j=0; j<16; j++) {
        if (m_endcap_trigger_ttc[i][j] >= m_TTCMultiplicity) {
	  cableWord0 = 1 << m_cablestarts[0];
	}
      }
    }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " cableWord: " << cableWord0 << endreq;    

    // Record the CTP trigger word in StoreGate.
    TrtCTP *trtCTP = new TrtCTP(cableWord0);
    containerName = DEFAULT_TrtCTPLocation;

    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "returning "<< trtCTP->dump() << endreq;
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << trtCTP->print() << endreq;
 
    sc = evtStore()->record(trtCTP, containerName, false);
    if(sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Failed to register " << containerName << endreq;
      return StatusCode::FAILURE;
    } 
    else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << containerName << " registered successfully " << endreq;
    }
   
    return StatusCode::SUCCESS;
  }

  StatusCode TrigT1TRT::finalize() {
    return StatusCode::SUCCESS;
  }
  
  int TrigT1TRT::BarrelChipToBoard(int chip) {
    // return logical board index:
    // 0 for Board 1S (has 10 chips)  0 -  9
    // 1 for 1L (11)                 10 - 20
    // 2 for 2S (15)                 21 - 35
    // 3 for 2L, first 9 chips       36 - 44
    // 4 for 2L, second 9 chips      45 - 53
    // 5 for 3S, first 11            54 - 64
    // 6 for 3S, second 12           65 - 76
    // 7 for 3L, first 13            77 - 89 
    // 8 for 3L, second 14           90 - 103
  
    int list[] = {10, 11, 15, 9, 9, 11, 12, 13, 14};
    int count = 0;
    chip--;
 
    for (int i=0; i<9; i++) {
      count += list[i];
      if (chip < count) return i+1;
      else if (chip == 104) return 9;
    }

    assert(count==104);
    assert(0); // should never come this far
    return -1;
  }
  
  int TrigT1TRT::EndcapChipToBoard(int chip) {
    int Board = -1;

    int remainder = (chip-1) % 12;
    Board = int(((chip -1) - remainder) / 12);
    return Board;
  } 

  int TrigT1TRT::EndcapStrawNumber(int strawNumber,int strawLayerNumber,int LayerNumber,int phi_stack,int side) {

    // Before perfoming map, corrections need to be perfomed.
    ////////  apply special rotations for endcap mappings/////
  
    // for eca, rotate triplets by 180 for stacks 9-16, and 25-32.
    static const int TripletOrientation[2][32] = { 
      {1,1,1,1,1,1,1,1, 
       0,0,0,0,0,0,0,0, 
       1,1,1,1,1,1,1,1, 
       0,0,0,0,0,0,0,0},
      {1,1,1,1,1,1,1,1, 
       0,0,0,0,0,0,0,0, 
       1,1,1,1,1,1,1,1, 
       0,0,0,0,0,0,0,0}
    };
  
    int phi1=-1; 
    if(side==2) phi1=phi_stack, side=1; 
    else if (side==-2) phi1=31-phi_stack, side=0; 
    if (phi1>-1){
      //if (side==1 && TripletOrientation[side][phi1]){
      if (TripletOrientation[side][phi1]){
        //Change straw number from 0-23 in straw layer to 0-192
      
        if (strawLayerNumber < 8)strawNumber = strawNumber + 24*strawLayerNumber;
        if (strawLayerNumber > 7)strawNumber = strawNumber + 24*(strawLayerNumber -8);
      
        strawNumber = (192-1)*TripletOrientation[side][phi1]+strawNumber*(1-2*TripletOrientation[side][phi1]);//actual rotation
      
        //take strawNumber back to 0-23
        if (strawLayerNumber<8) strawLayerNumber = int(strawNumber/24);
        if (strawLayerNumber>7) strawLayerNumber = int(strawNumber/24) + 8;
        strawNumber = strawNumber%24;
      }
    
      //Finish rotation
    
      //Flip straw in layer.
    
      if (side==0) strawNumber = 23 - strawNumber;
    
      //Finish Flipping
    }
  
    // Done with corrections
  
    //Start mapping from athena identifiers to TRTViewer maps
    int strawNumberNew=0;
  
    if(LayerNumber<6 && strawLayerNumber>7) {
      strawNumberNew=strawNumberNew+(384*LayerNumber);
      strawNumberNew=strawNumberNew+192+(strawLayerNumber%8)+(strawNumber*8);
    } else if(LayerNumber<6 && strawLayerNumber<8) {
      strawNumberNew=strawNumberNew+(384*LayerNumber);
      strawNumberNew=strawNumberNew + (strawLayerNumber%8) + (strawNumber*8);
    } else if(LayerNumber>5 && strawLayerNumber>7) {
      strawNumberNew = strawNumberNew + 2304 + 192*(LayerNumber-6);
      strawNumberNew = strawNumberNew + 192 + (strawLayerNumber%8) + (8*strawNumber);
    } else if(LayerNumber>5 && strawLayerNumber<8) {
      strawNumberNew = strawNumberNew + 2304 + 192*(LayerNumber-6);
      strawNumberNew = strawNumberNew + (strawLayerNumber%8) + (8*strawNumber);
    }
  
    strawNumber=strawNumberNew;

    return strawNumber;
  }

  int TrigT1TRT::BarrelStrawNumber( int strawNumber, int strawlayerNumber, int LayerNumber) {
    int addToStrawNumber=0;
    int addToStrawNumberNext=0;
    int i=0;

    do {
      i++;
      addToStrawNumber+=numberOfStraws[i-1];
      addToStrawNumberNext = addToStrawNumber+numberOfStraws[i];
    }
    while(BarrelStrawLayerNumber(strawlayerNumber,LayerNumber)!=i-1);
    /*
    if(BarrelStrawLayerNumber(strawlayerNumber,LayerNumber)%2==-10) {
      strawNumber += addToStrawNumber;
    } else {
      strawNumber = addToStrawNumberNext - strawNumber-1;
    }
    */
    strawNumber = addToStrawNumberNext - strawNumber-1;
    return strawNumber;
  }

  int TrigT1TRT::BarrelStrawLayerNumber( int strawLayerNumber, int LayerNumber) {
    if(LayerNumber==0) {
      strawLayerNumber+=0;
    } else if(LayerNumber==1) {
      strawLayerNumber+=19;
    } else if(LayerNumber==2) {
      strawLayerNumber+=43;
    }
    return strawLayerNumber;
  }
  
}
