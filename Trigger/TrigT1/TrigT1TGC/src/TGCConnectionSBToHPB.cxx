/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCConnectionSBToHPB.cxx,v 1.3 2009-03-15 18:07:55 isaya Exp $
#include "TrigT1TGC/TGCConnectionSBToHPB.hh"
#include "TrigT1TGC/TGCBoardConnection.hh"

namespace LVL1TGCTrigger {


TGCConnectionSBToHPB::~TGCConnectionSBToHPB()
{
  int j;
  for( j=0; j<NumberOfSlaveBoardType; j+=1){
    if(HPBPortToSB[j]!=0) delete [] HPBPortToSB[j];
    HPBPortToSB[j]=0;

    if(HPBIdToSB[j]!=0) delete [] HPBIdToSB[j];
    HPBIdToSB[j]=0;
  }
}

TGCConnectionSBToHPB::TGCConnectionSBToHPB()
{
  // NO HPB for Inner
  setNumberOfType(4);
  
  int j;
  for( j=0; j<NumberOfSlaveBoardType; j+=1){
    HPBPortToSB[j]=0;
    HPBIdToSB[j]=0;
  }
}

TGCConnectionSBToHPB::TGCConnectionSBToHPB(const TGCConnectionSBToHPB& right) :
  TGCBoardConnection(right)
{
  int j,k;
  for( j=0; j<NumberOfSlaveBoardType; j+=1){
    if(HPBPortToSB[j]!=0) delete [] HPBPortToSB[j];
    HPBPortToSB[j] = new int [numberOfBoard[j]];
    if(HPBIdToSB[j]!=0) delete [] HPBIdToSB[j];
    HPBIdToSB[j] = new int [numberOfBoard[j]];
    for( k=0; k<numberOfBoard[j]; k+=1){
      HPBPortToSB[j][k] = right.HPBPortToSB[j][k];
      HPBIdToSB[j][k] = right.HPBIdToSB[j][k];
    }
  }
}

TGCConnectionSBToHPB& TGCConnectionSBToHPB::operator=(const TGCConnectionSBToHPB& right)
{
  if(this!=&right){
    TGCBoardConnection::operator=(right); // call base class assignment operator
    int j,k;
    for( j=0; j<NumberOfSlaveBoardType; j+=1){
      if(HPBPortToSB[j]!=0) delete [] HPBPortToSB[j];
      HPBPortToSB[j] = new int [numberOfBoard[j]];
      if(HPBIdToSB[j]!=0) delete [] HPBIdToSB[j];
      HPBIdToSB[j] = new int [numberOfBoard[j]];
      for( k=0; k<numberOfBoard[j]; k+=1){
	HPBPortToSB[j][k] = right.HPBPortToSB[j][k];
	HPBIdToSB[j][k] = right.HPBIdToSB[j][k];
      }
    }
  }
  return *this;
}

} //end of namespace bracket
