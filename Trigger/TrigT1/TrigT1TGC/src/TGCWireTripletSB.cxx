/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  if(m_coincidenceOut!=0){
    if ( m_slaveBoardOut != 0 ) delete m_slaveBoardOut;
    m_slaveBoardOut = new  TGCSlaveBoardOut(this, m_bid);
    if ( !m_slaveBoardOut ) {
      std::cerr << "TGCWireTripletSB::createSlaveBoardOut: Memory allocation failure."<<std::endl;
      exit(1);
    }
    m_slaveBoardOut->clear();
    m_slaveBoardOut->setNumberOfData(NumberOfWireTripletSBData);

    // fill SlaveBoardOut.
    // select largest R hit in each sections.
    int lengthOfSection = (m_coincidenceOut->getLength()-2*NChAdjInWTSB)/NumberOfWireTripletSBData;
    int i,j;
    for( i=0; i<NumberOfWireTripletSBData; i+=1){ // i=0:a 1:b 2:c
      m_slaveBoardOut->setPos(i,-1);
      m_slaveBoardOut->setPos(i,0);
      m_slaveBoardOut->setHit(i,false);
      for( j=0; j<lengthOfSection; j+=1) {
        if(m_coincidenceOut->getChannel(NChAdjInWTSB+j+i*lengthOfSection)){
          m_slaveBoardOut->setPos(i,j);
          m_slaveBoardOut->setHit(i,true);
          break;
        }
      }
      if(m_slaveBoardOut->getHit(i)){
          m_slaveBoardOut->setbPos(i, m_slaveBoardOut->getPos(i));
#ifdef TGCCOUT
          m_slaveBoardOut->getbPos(i)->printb();
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
  pattern[0] = m_patchPanelOut->getHitPattern(0);
  pattern[1] = m_patchPanelOut->getHitPattern(1);
  
  if(pattern[1]!=0){ // necessary for 2/3 coincidence. 

    int length=pattern[1]->getLength();
    int unitLength=length/2;
    int totalLength=3*unitLength;

    m_lengthOfCoincidenceOut = LengthOfWTSBCoincidenceOut;
    if( m_coincidenceOut!=0 ) delete m_coincidenceOut;
    m_coincidenceOut = new TGCHitPattern(m_lengthOfCoincidenceOut);

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
    //    int base=(totalLength-m_lengthOfCoincidenceOut)/2;
    int base = 0;
    for( i=0; i<m_lengthOfCoincidenceOut; i+=1)
      m_coincidenceOut->setChannel(i,out[i+base]);

    delete [] b;
    delete [] out;
  }
}

TGCWireTripletSB::TGCWireTripletSB(const TGCWireTripletSB& right) :
  TGCSlaveBoard(right)
{
  m_id = right.m_id;
  m_bid = right.m_bid;
  m_idHighPtBoard = right.m_idHighPtBoard;
  m_type = right.m_type;
  m_lengthOfCoincidenceOut = right.m_lengthOfCoincidenceOut;
  *m_slaveBoardOut = *right.m_slaveBoardOut;
  *m_patchPanel = *right.m_patchPanel;
  *m_patchPanelOut = *right.m_patchPanelOut;

  if ( m_lengthOfCoincidenceOut > 0) { 
    if ( m_coincidenceOut ) delete  m_coincidenceOut; 
    m_coincidenceOut = new TGCHitPattern (m_lengthOfCoincidenceOut);
    if ( !m_coincidenceOut ) 
      std::cerr << "TGCWireTripletSB::TGCWireTripletSB: Memory allocation failure." << std::endl;
  }
}

TGCWireTripletSB& TGCWireTripletSB::operator=(const TGCWireTripletSB& right)
{
  if ( this != &right ) {
    m_id = right.m_id;
    m_bid = right.m_bid;
    m_idHighPtBoard = right.m_idHighPtBoard;
    m_type = right.m_type;
    m_lengthOfCoincidenceOut = right.m_lengthOfCoincidenceOut;
    *m_slaveBoardOut = *right.m_slaveBoardOut;
    *m_patchPanel = *right.m_patchPanel;
    *m_patchPanelOut = *right.m_patchPanelOut;

    if ( m_lengthOfCoincidenceOut > 0) { 
      if ( m_coincidenceOut ) delete  m_coincidenceOut; 
      m_coincidenceOut = new TGCHitPattern (m_lengthOfCoincidenceOut);
      if ( !m_coincidenceOut ) {
        std::cerr << "TGCWireTripletSB::TGCWireTripletSB: Memory allocation failure." << std::endl;
        exit(1);
      }
    }
  }
  return *this;
}



} //end of namespace bracket
