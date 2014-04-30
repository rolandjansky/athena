/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ref. SOS051V07,S0S052V06 
#include "TrigT1TGC/TGCWireTripletSB.hh"
#include <iostream>
#include <cstdlib>

namespace LVL1TGCTrigger {

extern bool g_STRICTWT;

TGCWireTripletSB::TGCWireTripletSB():TGCSlaveBoard()
{}

void TGCWireTripletSB::createSlaveBoardOut()
{
  if(coincidenceOut!=0){
    if ( slaveBoardOut != 0 ) delete slaveBoardOut;
    slaveBoardOut = new  TGCSlaveBoardOut(this, bid);
    if ( !slaveBoardOut ) {
      std::cerr << "TGCWireTripletSB::createSlaveBoardOut: Memory allocation failure."<<std::endl;
      exit(1);
    }
    slaveBoardOut->clear();
    slaveBoardOut->setNumberOfData(NumberOfWireTripletSBData);

    // fill SlaveBoardOut.
    // select largest R hit in each sections.
    int lengthOfSection = (coincidenceOut->getLength()-2*NChAdjInWTSB)/NumberOfWireTripletSBData;
    int i,j;
    for( i=0; i<NumberOfWireTripletSBData; i+=1){ // i=0:a 1:b 2:c
      slaveBoardOut->setPos(i,-1);
      slaveBoardOut->setPos(i,0);
      slaveBoardOut->setHit(i,false);
      for( j=0; j<lengthOfSection; j+=1) {
        if(coincidenceOut->getChannel(NChAdjInWTSB+j+i*lengthOfSection)){
          slaveBoardOut->setPos(i,j);
          slaveBoardOut->setHit(i,true);
          break;
        }
      }
      if(slaveBoardOut->getHit(i)){
          slaveBoardOut->setbPos(i, slaveBoardOut->getPos(i));
#ifdef TGCCOUT
          slaveBoardOut->getbPos(i)->printb();
	  std::cout << " " << i << std::endl;
#endif
      }
    }
  }
}

void TGCWireTripletSB::doCoincidence()
{
  TGCHitPattern* pattern[2];
  // pattern[0] has hit pattern of layer0[36-71].
  // pattern[1] has hit pattern of layer1[0-35] and layer2[36-71] .
  pattern[0] = patchPanelOut->getHitPattern(0);
  pattern[1] = patchPanelOut->getHitPattern(1);
  
  if(pattern[1]!=0){ // necessary for 2/3 coincidence. 

    int length=pattern[1]->getLength();
    int unitLength=length/2;
    int totalLength=3*unitLength;

    lengthOfCoincidenceOut = LengthOfWTSBCoincidenceOut;
    if( coincidenceOut!=0 ) delete coincidenceOut;
    coincidenceOut = new TGCHitPattern(lengthOfCoincidenceOut);

    // rearrange bit pattern for coincidence.
    bool* b = new bool [totalLength];
    int i;
    for( i=0; i<unitLength; i+=1){
      if(pattern[0]!=0){
        b[3*i] = pattern[0]->getChannel(i+unitLength); //layer0(smallest in eta)
      }else{
        b[3*i] = 0;
      }
      if(pattern[1]!=0){
        b[3*i+1] = pattern[1]->getChannel(i); // layer1 
        b[3*i+2] = pattern[1]->getChannel(i+unitLength); //layer2
      }else{
        b[3*i+1] = 0;
        b[3*i+2] = 0;
      }
    }

    // perform 2/3 coincidence
    // bug fixed (KH 19/01/01) 
    bool* out = new bool [totalLength];
    for( i=0; i<totalLength-8; i++){
      if(g_STRICTWT){
	out[i] = ( b[i+3] & b[i+4] & b[i+5] );
      } else {
	out[i] = ( b[i+3] & b[i+5] & !b[i+1] & !b[i+7] ) | 
	  ( b[i+3] & b[i+4] & !b[i+2] ) |
	  ( b[i+4] & b[i+5] & !b[i+6] ) |
	  ( b[i+6] & b[i+2] &  b[i+4] & !b[i+3] & !b[i+5] & !b[i+1] & !b[i+7]);
      }
    }
#ifdef TGCCOUT
    std::cout << "WireTripletSB OUT" << std::endl;
    for( i=0; i<totalLength-8; i+=1){
        std::cout << out[i];
        if(i==totalLength/3-1 || i==2*totalLength/3-1){std::cout << "\n";}
    }
    std::cout << std::endl;
#endif
    //    int base=(totalLength-lengthOfCoincidenceOut)/2;
    int base = 0;
    for( i=0; i<lengthOfCoincidenceOut; i+=1)
      coincidenceOut->setChannel(i,out[i+base]);

    delete [] b;
    delete [] out;
  }
}

TGCWireTripletSB::TGCWireTripletSB(const TGCWireTripletSB& right) :
  TGCSlaveBoard(right)
{
  id = right.id;
  bid = right.bid;
  idHighPtBoard = right.idHighPtBoard;
  type = right.type;
  lengthOfCoincidenceOut = right.lengthOfCoincidenceOut;
  *slaveBoardOut = *right.slaveBoardOut;
  *patchPanel = *right.patchPanel;
  *patchPanelOut = *right.patchPanelOut;

  if ( lengthOfCoincidenceOut > 0) { 
    if ( coincidenceOut ) delete  coincidenceOut; 
    coincidenceOut = new TGCHitPattern (lengthOfCoincidenceOut);
    if ( !coincidenceOut ) 
      std::cerr << "TGCWireTripletSB::TGCWireTripletSB: Memory allocation failure." << std::endl;
  }
}

TGCWireTripletSB& TGCWireTripletSB::operator=(const TGCWireTripletSB& right)
{
  if ( this != &right ) {
    id = right.id;
    bid = right.bid;
    idHighPtBoard = right.idHighPtBoard;
    type = right.type;
    lengthOfCoincidenceOut = right.lengthOfCoincidenceOut;
    *slaveBoardOut = *right.slaveBoardOut;
    *patchPanel = *right.patchPanel;
    *patchPanelOut = *right.patchPanelOut;

    if ( lengthOfCoincidenceOut > 0) { 
      if ( coincidenceOut ) delete  coincidenceOut; 
      coincidenceOut = new TGCHitPattern (lengthOfCoincidenceOut);
      if ( !coincidenceOut ) {
        std::cerr << "TGCWireTripletSB::TGCWireTripletSB: Memory allocation failure." << std::endl;
        exit(1);
      }
    }
  }
  return *this;
}



} //end of namespace bracket
