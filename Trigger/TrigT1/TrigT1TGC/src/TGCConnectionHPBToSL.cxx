/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCConnectionHPBToSL.cxx,v 1.2 2006-06-22 00:26:35 nkanaya Exp $
#include "TrigT1TGC/TGCConnectionHPBToSL.hh"
#include "TrigT1TGC/TGCBoardConnection.hh"

namespace LVL1TGCTrigger {

// default constructor
TGCConnectionHPBToSL::TGCConnectionHPBToSL()
{
  setNumberOfType(NumberOfHighPtBoardType);
  for(int j=0; j<NumberOfHighPtBoardType; j++) SLPortToHPB[j]=0;
}

// Copy constructor
TGCConnectionHPBToSL::TGCConnectionHPBToSL(const TGCConnectionHPBToSL& right)
 : TGCBoardConnection(right)
{
  for(int j=0; j<NumberOfHighPtBoardType; j++) {
    if(SLPortToHPB[j]!=0) delete [] SLPortToHPB[j];
    SLPortToHPB[j] = new int [numberOfBoard[j]];

    for(int k=0; k<numberOfBoard[j]; k++) SLPortToHPB[j][k] = right.SLPortToHPB[j][k];
  }
}


TGCConnectionHPBToSL::~TGCConnectionHPBToSL()
{
  int j;
  for( j=0; j<NumberOfHighPtBoardType; j+=1){
      if(SLPortToHPB[j]!=0) delete [] SLPortToHPB[j];
      SLPortToHPB[j]=0;
  }
}

TGCConnectionHPBToSL& TGCConnectionHPBToSL::operator=(const TGCConnectionHPBToSL& right)
{
  if(this!=&right){
    TGCBoardConnection::operator=(right); // call base class assignment operator
    int j,k;
    for( j=0; j<NumberOfHighPtBoardType; j+=1){
      if(SLPortToHPB[j]!=0) delete [] SLPortToHPB[j];
      SLPortToHPB[j] = new int [numberOfBoard[j]]; 
      for( k=0; k<numberOfBoard[j]; k+=1)
	SLPortToHPB[j][k] = right.SLPortToHPB[j][k];
    }
  }
  return *this;
}

} //end of namespace bracket
