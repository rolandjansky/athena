/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_StrawNeighbourSvc.cxx
 *  @brief Service to information on straws electronic grouping
 *  Develuped for the study of noise correlations between straws
 *  @author Esben Klinkby <klinkby@nbi.dk>
 **/

#include "TRT_StrawNeighbourSvc.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

#include "StoreGate/StoreGateSvc.h"

#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "PathResolver/PathResolver.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include "TRT_ConditionsData/ExpandedIdentifier.h"
#include "TRT_ConditionsData/TRTStrawStatusData.h"
#include "TRT_ConditionsData/StrawStatusContainer.h"

//Geometry db
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/GaudiException.h"

//Geomodel
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"


TRT_StrawNeighbourSvc::TRT_StrawNeighbourSvc( const std::string& name,
					    ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),
    m_detStore("DetectorStore",name),
    m_trtid(0)
{
  declareProperty("DetectorStore",m_detStore);
}


TRT_StrawNeighbourSvc::~TRT_StrawNeighbourSvc()
{
}


StatusCode TRT_StrawNeighbourSvc::initialize() 
{
  ATH_MSG_DEBUG("TRT_StrawNeighbourSvc initialize method called");

  if (StatusCode::SUCCESS!=m_detStore.retrieve()) {
    msg(MSG::FATAL) << "Couldn't retrieve " << m_detStore << endmsg;
    return StatusCode::FAILURE;
  }
  
   // Get the TRT ID helper
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtid,"TRT_ID")) {
    msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endmsg;
    return StatusCode::FAILURE;
  }


  int chipConversionSocketToChip_m1[]={0,1,2,3,4,21,7,6,5,20,19,8,9,16,17,18,10,15,14,13,12,11};
  for(int j=0; j<22; j++) m_chipConversionSocketToChip_m1[j]=chipConversionSocketToChip_m1[j];
  int chipConversionChipToSocket_m1[] = {0,1,2,3,4,8,7,6,11,12,16,21,20,19,18,17,13,14,15,10,9,5};
  for(int j=0; j<22; j++) m_chipConversionChipToSocket_m1[j]=chipConversionChipToSocket_m1[j];

  int chipConversionSocketToChip_m2[] = {0,5,4,3,2,1,33,6,7,8,9,32,31,12,11,10,27,28,29,30,13,14,26,25,24,23,22,15,16,17,18,19,20,21};
  for(int j=0; j<34; j++) m_chipConversionSocketToChip_m2[j]=chipConversionSocketToChip_m2[j];
  int chipConversionChipToSocket_m2[] = {0,5,4,3,2,1,7,8,9,10,15,14,13,20,21,27,28,29,30,31,32,33,26,25,24,23,22,16,17,18,19,12,11,6};
  for(int j=0; j<34; j++) m_chipConversionChipToSocket_m2[j]=chipConversionChipToSocket_m2[j];

  int chipConversionSocketToChip_m3[] = {0,1,2,3,4,5,6,50,11,10,9,8,7,49,12,13,14,15,48,47,46,18,17,16,42,43,44,45,19,20,41,40,39,38,37,22,21,31,32,33,34,35,36,23,30,29,28,27,26,25,24};
  for(int j=0; j<51; j++)   m_chipConversionSocketToChip_m3[j]=chipConversionSocketToChip_m3[j];
  int chipConversionChipToSocket_m3[] = {0,1,2,3,4,5,6,12,11,10,9,8,14,15,16,17,23,22,21,28,29,36,35,43,50,49,48,47,46,45,44,37,38,39,40,41,42,34,33,32,31,30,24,25,26,27,20,19,18,13,7};
  for(int j=0; j<51; j++) m_chipConversionChipToSocket_m3[j]=chipConversionChipToSocket_m3[j];

  if (1) { // initialize endcap chip maps, sasa 27 July 2009

    int chipIndexToChipHardware_A0[] = {2, 1, 3, 0, 6, 5, 7, 4, 10, 9, 11, 8};
    int chipIndexToChipHardware_A8[] = {8, 11, 9, 10, 4, 7, 5, 6, 0, 3, 1, 2};
    int chipIndexToChipHardware_C0[] = {1, 2, 0, 3, 5, 6, 4, 7, 9, 10, 8, 11};
    int chipIndexToChipHardware_C8[] = {11, 8, 10, 9, 7, 4, 6, 5, 3, 0, 2, 1};
    int *map_list[] = {m_endcapChipMapA0, m_endcapChipMapA8, m_endcapChipMapC0, m_endcapChipMapC8};
    int *chip_list[] = {chipIndexToChipHardware_A0, chipIndexToChipHardware_A8, chipIndexToChipHardware_C0, chipIndexToChipHardware_C8};
    for (int i=0; i<4; i++) for (int j=0; j<12; j++) map_list[i][j] = chip_list[i][j];
  }



  //Geometry db
  // RDBAccessSvc (Interface to the DD database).
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap.h
  IRDBAccessSvc* iAccessSvc = NULL;
  StatusCode result = svcLocator->service("RDBAccessSvc",iAccessSvc);
  if ( result.isFailure()  ||  iAccessSvc == NULL ) {
    msg(MSG::FATAL) << "Could not initialize RDBAccessSvc!" << endmsg;
    throw GaudiException("Could not initalize RDBAccessSvc","TRT_GeoModel",StatusCode::FAILURE);
  }
  
  IGeoModelSvc *geoModel;
  result = svcLocator->service ("GeoModelSvc",geoModel);
  if ( result.isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endmsg;
    throw GaudiException("Could not locate GeoModelSvc","TRT_GeoModel",StatusCode::FAILURE);
  }
  DecodeVersionKey versionKey(geoModel, "TRT");
  std::string detectorKey  = versionKey.tag();
  std::string detectorNode = versionKey.node();
  IRDBRecordset_ptr RDB_TRTElec  = iAccessSvc->getRecordsetPtr("TRTBarElecToStrawRel",detectorKey,detectorNode);
  
  if (RDB_TRTElec->size()==0) {
    RDB_TRTElec = iAccessSvc->getRecordsetPtr("TRTBarElecToStrawRel","TRTBarElecToStrawRel-02");
    msg(MSG::INFO) << "The folder: InnerDetector->TRT->TRTBarrel->TRTBarrelElectronics not found in DetDesc tag. Using hardcoded tag: TRTBarElecToStrawRel-02" << endmsg;
  }
  

  // initialization
  
  const int numberOfm1layers=19;
  const int numberOfm2layers=24;
  const int numberOfm3layers=30;

  int layer_m1_array[numberOfm1layers]= {15,16,16,16,16,17,17,17,17,17,18,18,18,18,18,19,19,19,18};
  int layer_m2_array[numberOfm2layers]= {19,20,20,20,20,20,21,21,21,21,21,22,22,22,22,22,23,23,23,23,23,24,24,23};
  int layer_m3_array[numberOfm3layers]= {23,24,24,24,24,25,25,25,25,25,26,26,26,26,26,27,27,27,27,27,28,28,28,28,28,29,29,29,29,28};

  int i =0;
  unsigned int acc=0;

  for (i=0; i<numberOfm1layers ;i++){
    m_layer_m1.push_back(layer_m1_array[i]);
    acc +=layer_m1_array[i];
    m_layer_m1_acc.push_back(acc);
  }
  
  acc=0;    
  for (i=0; i<numberOfm2layers;i++){
    m_layer_m2.push_back(layer_m2_array[i]);
    acc +=layer_m2_array[i];
    m_layer_m2_acc.push_back(acc);
  }
  
  acc=0;
  for (i=0; i<numberOfm3layers;i++){
    m_layer_m3.push_back(layer_m3_array[i]);
    acc +=layer_m3_array[i];
    m_layer_m3_acc.push_back(acc);
  }  




  ///////////Configuring the tool from database/////////////////////////////


    int straw =0; 
    int stnm=0, pad=0, previous_pad=1, layer = 0, pad_rel_mod=0;
    double chip=0.;
    
    std::vector<std::vector<int> > dummy1;
    std::vector<int> dummy12;
    std::vector<std::vector<int> > dummy2;
    std::vector<int> dummy22;
    std::vector<std::vector<int> > dummy3;
    std::vector<int> dummy32;
                                                                                                                                                             
    std::vector<std::vector<int> > dumdumdum1;
    std::vector<std::vector<int> > dumdumdum2;
    std::vector<std::vector<int> > dumdumdum3;
    std::vector<int> dumdum ;
    for (unsigned int d =0; d<=100; d++ ) {
      dumdumdum1.push_back(dumdum);
      dumdumdum2.push_back(dumdum);
      dumdumdum3.push_back(dumdum);
    }
    
    m_m1.push_back(0);
    m_m2.push_back(0);
    m_m3.push_back(0);
    
    
    dummy1.clear();
    dummy12.clear();;
    while(layer == 0) {
      const IRDBRecord* RDBVars__TRTElec = (*RDB_TRTElec)[straw];
      layer =  RDBVars__TRTElec->getInt("LAYER");
      if (!(layer==0)) break;
      stnm = RDBVars__TRTElec->getInt("STRAWNUM");
      pad = RDBVars__TRTElec->getInt("HVPADRELSTRAW");
      pad_rel_mod = RDBVars__TRTElec->getInt("HVPADRELMOD");
      m_m1.push_back(pad_rel_mod);
      if (pad==previous_pad) dummy12.push_back(stnm);
      else if (pad != previous_pad) {
	previous_pad=pad;
	dummy1.push_back(dummy12);
	dummy12.clear();
	dummy12.push_back(stnm);
      }
      chip=(RDBVars__TRTElec->getDouble("CHIPRELSTRAW"));
      m_chip_vector1.push_back(chip);
      (dumdumdum1[(int)chip]).push_back(straw+1);
      straw++;
    }
    m_pad_to_straw.push_back(dummy1);
    m_chip_to_straw.push_back(dumdumdum1);
    
    
    dummy2.clear();
    dummy22.clear();;
    previous_pad=1;
    while(layer == 1) {
      const IRDBRecord* RDBVars__TRTElec = (*RDB_TRTElec)[straw];
      layer =  RDBVars__TRTElec->getInt("LAYER");
      if (!(layer==1)) break;
      stnm = RDBVars__TRTElec->getInt("STRAWNUM");
      pad = RDBVars__TRTElec->getInt("HVPADRELSTRAW");
      pad_rel_mod = RDBVars__TRTElec->getInt("HVPADRELMOD");
      m_m2.push_back(pad_rel_mod);
      if (pad==previous_pad) dummy22.push_back(stnm);
      else if (pad != previous_pad) {
	previous_pad=pad;
	dummy2.push_back(dummy22);
	dummy22.clear();
	dummy22.push_back(stnm);
      }
      chip=(RDBVars__TRTElec->getDouble("CHIPRELSTRAW"));
      m_chip_vector2.push_back(chip);
      (dumdumdum2[(int)chip]).push_back(straw+1-329);
      straw++;
    }
    m_pad_to_straw.push_back(dummy2);
    m_chip_to_straw.push_back(dumdumdum2);
   
    
    
    dummy3.clear();
    dummy32.clear();;
    previous_pad=1;
    while(layer == 2) {
      if (straw>1641) break;// return StatusCode::SUCCESS;
      const IRDBRecord* RDBVars__TRTElec = (*RDB_TRTElec)[straw];
      layer =  RDBVars__TRTElec->getInt("LAYER");
      if (!(layer==2)) break;
      stnm = RDBVars__TRTElec->getInt("STRAWNUM");
      pad = RDBVars__TRTElec->getInt("HVPADRELSTRAW");
      pad_rel_mod = RDBVars__TRTElec->getInt("HVPADRELMOD");
      m_m3.push_back(pad_rel_mod);
      if (pad==previous_pad) dummy32.push_back(stnm);
      else if (pad != previous_pad) {
	previous_pad=pad;
	dummy3.push_back(dummy32);
	dummy32.clear();
	dummy32.push_back(stnm);
      }
      chip=(RDBVars__TRTElec->getDouble("CHIPRELSTRAW"));
      m_chip_vector3.push_back(chip);
      (dumdumdum3[(int)chip]).push_back(straw+1-329-520);
      straw++;
      
    }
    m_pad_to_straw.push_back(dummy3);
    m_chip_to_straw.push_back(dumdumdum3);
    
  

  ATH_MSG_DEBUG("TRT_StrawNeighbourSvc initialized ");
  if (!((m_m1.size()==330)&&(m_m2.size()==521)&&(m_m3.size()==794) )) ATH_MSG_WARNING("Initialization of TRT_StrawNeighbourTool incomplete. Depending on usage, this can give rise to runtime problems");


  //for strawLayer barrel
  int  numberOfStraws[75] = {0,
			     15,
			     16,16,16,16,
			     17,17,17,17,17,
			     18,18,18,18,18,
			     19,19,19,
			     18,
			     19,
			     20,20,20,20,20,
			     21,21,21,21,21,
			     22,22,22,22,22,
			     23,23,23,23,23,
			     24,24,
			     23,23,
			     24,24,24,24,
			     25,25,25,25,25,
			     26,26,26,26,26,
			     27,27,27,27,27,
			     28,28,28,28,28,
			     29,29,29,29,
			     28,
			     0};
 for(int j=0; j<75; j++) m_numberOfStraws[j]=numberOfStraws[j];

 int TripletOrientation[2][32] = { 
   {1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0},
   {1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0}
 };
 


 for(int j=0; j<2; j++) 
   for(int jj=0; jj<32; jj++) 
     m_TripletOrientation[j][jj]=TripletOrientation[j][jj];
 
 
 return StatusCode::SUCCESS;
 

 
}

///////////////////////////////////////////////////



StatusCode TRT_StrawNeighbourSvc::finalize()
{

  msg(MSG::INFO) << "TRT_StrawNeighbourSvc finalize method called" << endmsg;
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////

void TRT_StrawNeighbourSvc::getAtlasIdentifier(int strawnumber, Identifier &outputID, Identifier inputID){
  int straw=0,layer=0;
  int moduleType = m_trtid->layer_or_wheel(inputID);

  if (moduleType==0) {
    for (unsigned int i=0; i< m_layer_m1_acc.size();i++){
      if (((unsigned int)strawnumber)<=m_layer_m1_acc[i]) {
	layer=i;
	straw= m_layer_m1_acc[i]-strawnumber;
	break;
      }
    }
  }
  else if (moduleType==1) {
    for (unsigned int i=0; i< m_layer_m2_acc.size();i++){
      if (((unsigned int)strawnumber)<=m_layer_m2_acc[i]) {
	layer=i;
	straw= m_layer_m2_acc[i]-strawnumber;
	break;
      }
    }
  }
  else if (moduleType==2) {
    for (unsigned int i=0; i< m_layer_m3_acc.size();i++){
      if (((unsigned int)strawnumber)<=m_layer_m3_acc[i]) {
	layer=i;
	straw= m_layer_m3_acc[i]-strawnumber;
	break;
      }
    }
  }


  outputID = m_trtid->straw_id(m_trtid->barrel_ec(inputID),
			       m_trtid->phi_module(inputID),
			       m_trtid->layer_or_wheel(inputID),
			       layer,
			       straw);
  return;
}



///////////////////////////////////////////////////

int TRT_StrawNeighbourSvc::getRunningNumbering(Identifier offlineID){

  if (abs((m_trtid->barrel_ec(offlineID)))!=1 ) {
    msg(MSG::ERROR) << "Sorry, getRunningNumbering only works for barrel" << endmsg;
    return 0;
  }
  
  int layer = m_trtid->straw_layer(offlineID);
  int straw = m_trtid->straw(offlineID);
  int moduleType = m_trtid->layer_or_wheel(offlineID);


  int strawnumber=0;
  if (moduleType==0) {  
    strawnumber += m_layer_m1_acc[layer];
    strawnumber -= straw;
  }
  else if (moduleType==1) {  
    strawnumber += m_layer_m2_acc[layer];
    strawnumber -= straw;
  }
  else if (moduleType==2) {  
    strawnumber += m_layer_m3_acc[layer];
    strawnumber -= straw;
  }
  return strawnumber;
}


///////////////////////////////////////////
void TRT_StrawNeighbourSvc::getSocket(Identifier offlineID, int&socket){

  if ( !(abs(m_trtid->barrel_ec(offlineID))==1)) {
    msg(MSG::ERROR) << "Sorry, this only works for barrel"<<endmsg;
    return;
  }  else {
    
    

  int chip = 0;
  getChip(offlineID,chip);
  int layer_or_wheel =  m_trtid->layer_or_wheel(offlineID);

  if (layer_or_wheel==0 )socket = m_chipConversionChipToSocket_m1[chip];
  else if (layer_or_wheel==1 )socket = m_chipConversionChipToSocket_m2[chip];
  else if (layer_or_wheel==2 )socket = m_chipConversionChipToSocket_m3[chip];

  return;
  }  
}


void TRT_StrawNeighbourSvc::getChip(Identifier offlineID, int& chip ){

  int socket=0;
  chip=0;

  int layer_or_wheel =  m_trtid->layer_or_wheel(offlineID);
  
  if (abs((m_trtid->barrel_ec(offlineID)))!=2 ) {
     if (layer_or_wheel == 0) socket = (int)m_chip_vector1[getRunningNumbering(offlineID)-1];
     else if (layer_or_wheel == 1) socket = (int)m_chip_vector2[getRunningNumbering(offlineID)-1];
     else if (layer_or_wheel == 2) socket = (int)m_chip_vector3[getRunningNumbering(offlineID)-1];
     else { msg(MSG::ERROR) << "Something is very wrong: According to identifier, straw belongs to a barrel module which is not of type 1,2 or 3 (corresponding to offline numbering: layer_or_wheel = 0,1 or 2) " << endmsg;}
     
     
     if (layer_or_wheel == 0) chip = m_chipConversionSocketToChip_m1[socket] ;
     else if (layer_or_wheel == 1) chip = m_chipConversionSocketToChip_m2[socket] ;
     else if (layer_or_wheel == 2) chip = m_chipConversionSocketToChip_m3[socket] ;
     return;
  }
  int straw =  m_trtid->straw(offlineID);
  int strawlayer =  m_trtid->straw_layer(offlineID);
  int bec =  m_trtid->barrel_ec(offlineID);
  int phi =  m_trtid->phi_module(offlineID);

// FIX ENDCAP CHIP MAPPING - SASA, 27 July 2009 

  chip = (straw - straw%4) / 2;
  if (strawlayer%8 >= 4) chip++; 

  int nominal_reversed = 1 - ( (phi - phi%8)/8 ) %2 ;
  int *map = 0;
  if (bec==2) { 
     map = nominal_reversed ? m_endcapChipMapA8 : m_endcapChipMapA0;
     chip = map[chip];
  }
  else if (bec==-2) {
     map = nominal_reversed ? m_endcapChipMapC8 : m_endcapChipMapC0;
     chip = map[chip];
  }
  else { msg(MSG::ERROR) << "Something is very wrong: endcap chip mapping " << bec << endmsg; chip = 12; }

  chip += 104;

  int board = layer_or_wheel;
  if (board<6) { board *= 2; if (strawlayer>7) board++; }
  else { board += 6; }
  chip += board * 12;

  return;

// end FIX ENDCAP CHIP MAPPING - SASA, 27 July 2009

  //orientation
  
  // endcap A is normal. endcap C is reverse
/*
  int orientation = 1;
  if (bec < 0) orientation = -1;

  //some phi sectors has reverse chip orientation 
  if (!( ( phi >= 24 ) || ( (phi >= 8) && ( phi <= 15)) )) orientation = orientation * (-1 );

  int strawlayer_temp = strawlayer % 8 ;
  int straw_temp=straw;

  if (bec<0) straw_temp=23-straw_temp;

  //case normal orientation
  if (orientation > 0 ) { 
    if (strawlayer_temp < 4 ) { 
      if (straw_temp<4) chip = 1;
      else if (straw_temp<8) chip = 0;
      else if (straw_temp<12) chip = 5;
      else if (straw_temp<16) chip = 4;
      else if (straw_temp<20) chip = 9;
      else chip = 8;
    }else{ 
      if (straw_temp<4) chip = 2;
      else if (straw_temp<8) chip = 3;
      else if (straw_temp<12) chip = 6;
      else if (straw_temp<16) chip = 7;
      else if (straw_temp<20) chip = 10;
      else chip = 11;
    }
  }else{
    if (strawlayer_temp < 4 ) { 
      if (straw_temp<4) chip = 11;
      else if (straw_temp<8) chip = 10;
      else if (straw_temp<12) chip = 7;
      else if (straw_temp<16) chip = 6;
      else if (straw_temp<20) chip =3;
      else chip = 2;
    }else{ 
      if (straw_temp<4) chip = 8;
      else if (straw_temp<8) chip = 9;
      else if (straw_temp<12) chip = 4;
      else if (straw_temp<16) chip = 5;
      else if (straw_temp<20) chip = 0;
      else chip = 1;
    }
  }
  

  chip+=104;
  if (layer_or_wheel<=5) chip+=layer_or_wheel*24+ (strawlayer <8?0:12);
  else chip+=6*24+(layer_or_wheel-6)*12;

 
  return;
*/
}










void TRT_StrawNeighbourSvc::getPin(Identifier offlineID, int& pin ){


  pin=0;
  
  int layer_or_wheel =  m_trtid->layer_or_wheel(offlineID);
  if (abs((m_trtid->barrel_ec(offlineID)))==1 ) {
    if (layer_or_wheel == 0) {
      pin = (int)(100*fmod(m_chip_vector1[getRunningNumbering(offlineID)-1],1.)+0.1);
    }
    else if (layer_or_wheel == 1) {
      pin = (int)(100*fmod(m_chip_vector2[getRunningNumbering(offlineID)-1],1.)+0.1);
    }
    else if (layer_or_wheel == 2){
      pin = (int)(100*fmod(m_chip_vector3[getRunningNumbering(offlineID)-1],1.)+0.1);
    }
    else { msg(MSG::ERROR) << "Something is very wrong: According to identifier, straw belongs to a barrel module which is not of type 1,2 or 3 (corresponding to offline numbering: layer_or_wheel = 0,1 or 2) " << endmsg;}
    
    return;
  }
  
  else { 
    msg(MSG::WARNING) << " Sorry getPin only implemented for barrel so far. Returning pin=0 "  << endmsg;
    pin=0;
    
    return ;
  }
}




void TRT_StrawNeighbourSvc::getPad(Identifier offlineID, int& pad ){

  if (abs((m_trtid->barrel_ec(offlineID)))!=1 ) {
    //    msg(MSG::ERROR) << "Sorry, getPad only works for barrel. barrel_ec(offlineID) = "<<m_trtid->barrel_ec(offlineID) << endmsg;
    int temp_straw = (m_trtid->straw(offlineID))/8;
    int temp_straw_layer = (m_trtid->straw_layer(offlineID))/4;
    pad= temp_straw_layer*3+temp_straw+1;
    return;
  }


  int strawnumber=getRunningNumbering(offlineID);
  int moduleType = m_trtid->layer_or_wheel(offlineID);


  strawnumber=strawnumber%1642;



  if (moduleType==0) {
    pad = m_m1[strawnumber];
    return;
  }
  else if (moduleType==1) {
    pad = m_m2[strawnumber];
    return;
  }
  else if (moduleType==2) {
    pad = m_m3[strawnumber]; 
    return;
  }
  msg(MSG::ERROR) << "Something went wrong: Pad not found, returning 0" << endmsg;
  pad = 0;

  return;
}


///////////////////////////////////////////
void TRT_StrawNeighbourSvc::convert_numbering_bar(int& strawnumber, int& straw, int& layer, bool first) {
  /* 
     Barrel only.
     Converts from hardware (testbeam/testbench) numbering, i.e. running numbering to ATLAS numbering i.e. straw, layer.
     Give input '-1' for the value(s) to be determined.
  */

  strawnumber=strawnumber%1642;
  const int numberOfm1layers=19;
  const int numberOfm2layers=24;
  const int numberOfm3layers=30;
  int layer_m1_array[numberOfm1layers]= {15,16,16,16,16,17,17,17,17,17,18,18,18,18,18,19,19,19,18};
  int layer_m2_array[numberOfm2layers]= {19,20,20,20,20,20,21,21,21,21,21,22,22,22,22,22,23,23,23,23,23,24,24,23};
  int layer_m3_array[numberOfm3layers]= {23,24,24,24,24,25,25,25,25,25,26,26,26,26,26,27,27,27,27,27,28,28,28,28,28,29,29,29,29,28};
  std::vector<unsigned int> layer_m1;
  std::vector<unsigned int> layer_m1_acc;
  std::vector<unsigned int> layer_m2;
  std::vector<unsigned int> layer_m2_acc;
  std::vector<unsigned int> layer_m3;
  std::vector<unsigned int> layer_m3_acc;
    
  if (first) {

    int i =0;
    unsigned int acc=0;

    for (i=0; i<numberOfm1layers ;i++){
      layer_m1.push_back(layer_m1_array[i]);
      acc +=layer_m1_array[i];
      layer_m1_acc.push_back(acc);
    }

    acc=0;    
    for (i=0; i<numberOfm2layers;i++){
      layer_m2.push_back(layer_m2_array[i]);
      acc +=layer_m2_array[i];
      layer_m2_acc.push_back(acc);
    }
    
    acc=0;
    for (i=0; i<numberOfm3layers;i++){
      layer_m3.push_back(layer_m3_array[i]);
      acc +=layer_m3_array[i];
      layer_m3_acc.push_back(acc);
    }  
  
  } //first

  unsigned int i;

  if (((straw==-1)||(layer==-1))&&(strawnumber>-1)) {


    if (getModuleType(strawnumber)==1) {
      for (i=0; i<= layer_m1_acc.size();i++){
	if (((unsigned int)strawnumber)<layer_m1_acc[i]) {
	  layer=i;
	  if (i>0) straw=strawnumber- layer_m1_acc[i-1];
	  else if (i==0) straw=strawnumber;
	  return;
	}
      }
    }
    else if (getModuleType(strawnumber)==2) {
      for (i=0; i<= layer_m2_acc.size();i++){
	if (((unsigned int)strawnumber)<layer_m2_acc[i]) {
	  layer=i;  
	  if (i>0) straw=strawnumber-layer_m2_acc[i-1];
	  else if (i==0) straw=strawnumber;
	  return;
	}
      }
    }
    else if (getModuleType(strawnumber)==3) {
      for (i=0; i<= layer_m3_acc.size();i++){
	if (((unsigned int)strawnumber)<layer_m3_acc[i]) {
	  layer=i;  
	  if (i>0) straw=strawnumber-layer_m3_acc[i-1];
	  else if (i==0) straw=strawnumber;
	  return;
	}
      }
    }
  } // run mode i.e. which way to convert

  else if (((straw>-1)&&(layer>-1))&&(strawnumber==-1)) {
    strawnumber=0;
    if (getModuleType(strawnumber)==1) {  
      strawnumber += layer_m1_acc[layer];
      strawnumber += straw;
      return;
    }
    else if (getModuleType(strawnumber)==2) {  
      strawnumber += layer_m2_acc[layer];
      strawnumber += straw;
      return;
    }
    else if (getModuleType(strawnumber)==3) {  
      strawnumber += layer_m3_acc[layer];
      strawnumber += straw;
      return;
    }
    else {
      msg(MSG::ERROR) <<" Error ! write the author" << endmsg;
      return;
    }
  }

  else {
    msg(MSG::WARNING) <<" Confused by input. straw = "<<straw<<" strawnumber = "<<strawnumber<<" layer = "<<layer<< endmsg;
    return;
  }

  
} // function end

//////////////////////////////////////////

void TRT_StrawNeighbourSvc::convert_numbering_ec(int electronics_row,int electronics_layer,int  electronics_chip,int  electronics_wheel,int  electronics_phi, int& straw, int& strawlayer, int& bec,  int& sector, int& wheel, Identifier inputID) {

   /* JAAA this code is BROKEN. It does not work. It does not seem to be called anywhere in ATLAS. Do nothing now */
   msg(MSG::WARNING) << " You called a function, convert_numbering_ec() that is deprivated and not used anymore. Sorry! It never worked anyway. So it was only going to return something bad, anyway"<<endmsg;
   msg(MSG::WARNING) << " electronics_row = " << electronics_row << " and electronics_layer = " << electronics_layer << " and electronics_chip = " << electronics_chip << " and electronics_wheel = " << electronics_wheel << " and electronics_phi = " << electronics_phi << " and straw = " << straw << " and strawlayer = " << strawlayer << " and bec = " << bec << " and sector = " << sector << " and wheel = " << wheel << " and identifier = " << inputID << std::endl;
   

  return; 
}





///////////////////////////////////////////


void TRT_StrawNeighbourSvc::getStrawsFromPad(Identifier offlineID, std::vector<Identifier>& neighbourIDs) {

    Identifier outputID;

  if (abs((m_trtid->barrel_ec(offlineID))) != 1 ) {
    //    msg(MSG::ERROR) << "Sorry, getStrawsFromPad only works for barrel" << endmsg;
    // The Endcap getStrawsFromPad has NOT been properly tested!!!! 

    int temp_straw = (m_trtid->straw(offlineID))/8;
    int temp_straw_layer = (m_trtid->straw_layer(offlineID))/4;


    for (int temp_it = temp_straw * 8 ;  temp_it <  temp_straw * 8 +8 ; temp_it++){
      for (int temp_it2 = temp_straw_layer * 4 ;  temp_it2 <  temp_straw_layer * 4 +4 ; temp_it2++){
	outputID = m_trtid->straw_id(m_trtid->barrel_ec(offlineID),
				     m_trtid->phi_module(offlineID),
				     m_trtid->layer_or_wheel(offlineID),
				     temp_straw_layer,
				     temp_straw);
	
	neighbourIDs.push_back(outputID);
      }
    }
    
  }else{
    int moduleType = m_trtid->layer_or_wheel(offlineID);
    
    int pad = 0;
    getPad(offlineID, pad);
    
   

    for ( unsigned int j=0; j<(m_pad_to_straw[moduleType][pad-1]).size();j++){ 
      getAtlasIdentifier((m_pad_to_straw[moduleType][pad-1][j]),outputID,offlineID);
      neighbourIDs.push_back(outputID);
      if (j>9) {
	msg(MSG::ERROR) << " Error in  getStrawsFromPad. Returning "<<endmsg;
	continue;
      }
    }
  }
  return;
}

/////////////////////////////////////////////



void TRT_StrawNeighbourSvc::getStrawsFromChip(Identifier inputID, std::vector<Identifier>& neighbourIDs) {

    Identifier outputID;

  if (abs((m_trtid->barrel_ec(inputID))) == 2) {
  
    unsigned int straw = (m_trtid->straw(inputID));
    unsigned int straw_layer = (m_trtid->straw_layer(inputID));
    
    unsigned int local_straw = (m_trtid->straw(inputID))%4;
    unsigned int local_straw_layer = (m_trtid->straw_layer(inputID))%4;
    

    //    int sign_straw_layer = 0;  // set but not used
    //    int sign_straw = 0;        // set but not used
    
    for( unsigned int i = 0 ; i < 4 ; i ++ ) {
      for( unsigned int j = 0 ; j < 4 ; j ++ ) {


	 // set but not used
	 //	sign_straw = (i<local_straw ?
	 //		      -1 : 
	 //		      +1);

	// set but not used
	//	sign_straw_layer = (j<local_straw ?
	//			    -1 :
	//			    +1);

	outputID = m_trtid->straw_id(m_trtid->barrel_ec(inputID),
                                     m_trtid->phi_module(inputID),
                                     m_trtid->layer_or_wheel(inputID),
                                     ( straw_layer + j - local_straw_layer),
                                     ( straw + i - local_straw));
        neighbourIDs.push_back(outputID);
	//	sign_straw=0;
	//	sign_straw_layer=0;  // set but not used
      }
    }
  } 
  else if  (abs((m_trtid->barrel_ec(inputID))) == 1 ) {
    //m_chip_to_straw    

    int moduleType = m_trtid->layer_or_wheel(inputID);
    int chip = 0, chiptest=-10;
    getChip(inputID, chip);

    for ( unsigned int j=0; j<(m_chip_to_straw[moduleType][chip]).size(); j++){
      if ( (m_chip_to_straw[moduleType][chip][j] ) ==0 ) continue;
      getAtlasIdentifier((m_chip_to_straw[moduleType][chip][j]),outputID,inputID);
      getChip(outputID,chiptest);
      neighbourIDs.push_back(outputID);
      if (j>17) {
        msg(MSG::ERROR) << " Error in  getStrawsFromChip. Returning "<<endmsg;
        continue;
      }
    }
  }
  else if ( (abs((m_trtid->barrel_ec(inputID))) != 1 ) &&  (abs((m_trtid->barrel_ec(inputID))) != 2 )) {
    msg(MSG::ERROR) << "Attempt to use getStrawsFromChip based on a non-TRT identifier. barrel_ec(inputID) = " << m_trtid->barrel_ec(inputID) <<endmsg; }
 


  return;
}

///////////////////////////////////////////////////

TRTCond::ExpandedIdentifier  TRT_StrawNeighbourSvc::getFirstStrawFromChip(Identifier inputID) {
  Identifier outputID;
                                                                                                                                                             
  if (abs(m_trtid->barrel_ec(inputID)) == 2 ) {
    
    unsigned int straw = (m_trtid->straw(inputID));
    unsigned int straw_layer = (m_trtid->straw_layer(inputID));
    
    unsigned int local_straw = (m_trtid->straw(inputID))%4;
    unsigned int local_straw_layer = (m_trtid->straw_layer(inputID))%4;

    //FIXME extra layer not needed

    //construt identifier of first straw in chip
    outputID = m_trtid->straw_id(m_trtid->barrel_ec(inputID),
				 m_trtid->phi_module(inputID),
				 m_trtid->layer_or_wheel(inputID),
				 ( straw_layer - local_straw_layer),
				 ( straw - local_straw ) );
    
  }
  else if (abs((m_trtid->barrel_ec(inputID))) == 1 ) {
    
    int moduleType = m_trtid->layer_or_wheel(inputID);
    int chip = 0;
    getChip(inputID, chip);
   
    getAtlasIdentifier(m_chip_to_straw[moduleType][chip][0],outputID,inputID);
    
  }
  else { msg(MSG::ERROR) << "Attempt to use getFirstStrawFromChip based on a non-TRT identifier. barrel_ec(inputID) = "<<m_trtid->barrel_ec(inputID) << endmsg; }
  
  int level = TRTCond::ExpandedIdentifier::STRAW ;
  TRTCond::ExpandedIdentifier id=  TRTCond::ExpandedIdentifier( m_trtid->barrel_ec(outputID),m_trtid->layer_or_wheel(outputID),
								m_trtid->phi_module(outputID),m_trtid->straw_layer(outputID),
								m_trtid->straw(outputID),level );
  return id;
}


int TRT_StrawNeighbourSvc::chipToBoardBarrel(int chip, int layer) {

  // First translate between internal software numbering and hardware numbering:
  int hardwarechip = 0;

  if ( (chip<1) || (chip > 50) || (abs(layer)>2) )  msg(MSG::ERROR) << "Attempt to use chipToBoard(int chip, int layer) with input outside bounds: 1<=chip<=50 and 0<=|layer|<=2 . Input was: chip = "<<chip<<" layer = "<<layer <<endmsg;  

  if (abs(layer)==0) hardwarechip = chip -1;
  else if  (abs(layer)==1) hardwarechip = chip + 20;
  else if  (abs(layer)==2) hardwarechip = chip + 20 + 33; 
			
  //From Sasa

  // return logical board index:
  // 0 for Board 1S (has 10 chips_
  // 1 for 1L (11)
  // 2 for 2S (15)
  // 3 for 2L, first 9 chips
  // 4 for 2L, second 9 chips
  // 5 for 3S, first 11
  // 6 for 3S, second 12
  // 7 for 3L, first 13
  // 8 for 3L, second 14
                                                                                                                                                             
  int list[] = {10, 11, 15, 9, 9, 11, 12, 13, 14};
  int count = 0;
  for (int i=0; i<9; i++) {
    count += list[i];
    if (hardwarechip < count) return i;
  }
  assert(count==104);
  assert(0); // should never come this far

  msg(MSG::ERROR) << "Something went wrong in chipToBoardBarrel - please contact the author "<<endmsg; 

  return -1;
}


int TRT_StrawNeighbourSvc::getRing( const Identifier &id ){
  
  if ( abs( m_trtid->barrel_ec( id ) ) != 2 ) { 
    ATH_MSG_WARNING( "getRing is not valid for barrel straws!" ); 
    return -1;  
  }
  
  //  const TRT_ID* m_trtid;
  unsigned int wheel = m_trtid->layer_or_wheel( id );
  unsigned int strawlayer = m_trtid->straw_layer( id );
  
  if (wheel < 6)
    return (16*wheel + strawlayer) / 4;
  else
    return (16*6+8*(wheel-6) + strawlayer) /4;
}

int TRT_StrawNeighbourSvc::chipToBoardEndCap(int chip)
 //----------------------------------------------------------------
{
  int Board = -1;
  int remainder = (chip-1) % 12;
  Board = int(((chip -1) - remainder) / 12);
  return Board;
}



int TRT_StrawNeighbourSvc::strawNumber( Identifier id)
//-----------------------------------------------------------------//
{
  int bec = m_trtid->barrel_ec(id);
  int phi_stack = m_trtid->phi_module(id);
  int LayerNumber = m_trtid->layer_or_wheel(id);
  int strawlayerNumber = m_trtid->straw_layer(id);
  int strawNumber = m_trtid->straw(id);

   
  if (abs(bec)==1) {
  
    int addToStrawNumber=0;
    int addToStrawNumberNext=0;
    int i=0;
    
    
    do
      {
	i++;
	addToStrawNumber+=m_numberOfStraws[i-1];
	addToStrawNumberNext = addToStrawNumber+m_numberOfStraws[i];
      }
    while(strawLayerNumber(id)!=i-1);
    /*
    if(strawLayerNumber(id)%2==-10)
      {
	strawNumber += addToStrawNumber;
      }
    else
      {
	strawNumber = addToStrawNumberNext - strawNumber-1;
      }
    */
    strawNumber = addToStrawNumberNext - strawNumber-1;
    
    return strawNumber;
  }
  
  //strawNumber() for barrel
  
  else if (abs(bec)==2) {
    int phi1=-1;
    int side=bec;
    if(side==2) phi1=phi_stack, side=1;
    else if (side==-2) phi1=31-phi_stack, side=0;
    if (phi1>-1){
      //if (side==1 && TripletOrientation[side][phi1]){
      if (m_TripletOrientation[side][phi1]){
	//Change straw number from 0-23 in straw layer to 0-192
	
	if (strawlayerNumber < 8)strawNumber = strawNumber + 24*strawlayerNumber;
	if (strawlayerNumber > 7)strawNumber = strawNumber + 24*(strawlayerNumber -8);
	
	strawNumber = (192-1)*m_TripletOrientation[side][phi1]+strawNumber*(1-2*m_TripletOrientation[side][phi1]);//actual rotation
	
	//take strawNumber back to 0-23
	if (strawlayerNumber<8) strawlayerNumber = int(strawNumber/24);
	if (strawlayerNumber>7) strawlayerNumber = int(strawNumber/24) + 8;
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
    
    if(LayerNumber<6 && strawlayerNumber>7) 
      {
	strawNumberNew=strawNumberNew+(384*LayerNumber);
	strawNumberNew=strawNumberNew+192+(strawlayerNumber%8)+(strawNumber*8);
      }
    else if(LayerNumber<6 && strawlayerNumber<8)
      {
	strawNumberNew=strawNumberNew+(384*LayerNumber);
	strawNumberNew=strawNumberNew + (strawlayerNumber%8) + (strawNumber*8);
      }
    
    else if(LayerNumber>5 && strawlayerNumber>7)
      {
	strawNumberNew = strawNumberNew + 2304 + 192*(LayerNumber-6);
	strawNumberNew = strawNumberNew + 192 + (strawlayerNumber%8) + (8*strawNumber);
      }
    else if(LayerNumber>5 && strawlayerNumber<8)
      {
	strawNumberNew = strawNumberNew + 2304 + 192*(LayerNumber-6);
	strawNumberNew = strawNumberNew + (strawlayerNumber%8) + (8*strawNumber);
      }
    
    strawNumber=strawNumberNew;
    
    return strawNumber;
  }
  else {    
    msg(MSG::ERROR) << "corrupted input identifier to TRT_StrawNeighbourSvc::strawNumber(). barrel_ec=" <<bec<< endmsg;
  }
  return -1;
}
  int TRT_StrawNeighbourSvc::strawLayerNumber( Identifier id )
  //----------------------------------------------------------------------//
{

  int bec = m_trtid->barrel_ec(id);
  //  int phi_stack = m_trtid->phi_module(id);
  int LayerNumber = m_trtid->layer_or_wheel(id);
  int strawLayerNumber = m_trtid->straw_layer(id);
  //  int strawNumber = m_trtid->straw(id);

   
  if (abs(bec)==1) {

  if(LayerNumber==0)
    {
      strawLayerNumber+=0;
    }
  else if(LayerNumber==1)
    {
      strawLayerNumber+=19;
    }
  else if(LayerNumber==2)
    {
      strawLayerNumber+=43;
    }
  
  return strawLayerNumber;
  }//strawLayerNumber()
  else{ 
    msg(MSG::ERROR) << "Sorry,  TRT_StrawNeighbourSvc::strawLayerNumber only works for barrel" << endmsg;
  }
  return -1;
}
