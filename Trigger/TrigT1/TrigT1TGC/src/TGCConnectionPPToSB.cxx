/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCConnectionPPToSB.cxx,v 1.2 2006-06-22 00:26:35 nkanaya Exp $
#include "TrigT1TGC/TGCConnectionPPToSB.hh"
#include "TrigT1TGC/TGCBoardConnection.hh"

namespace LVL1TGCTrigger {


TGCConnectionPPToSB::~TGCConnectionPPToSB()
{
  int i,j;
  for( i=0; i<NumberOfPPPort; i+=1)
    for( j=0; j<NumberOfPatchPanelType; j+=1){
      if(SBIdToPP[i][j]!=0) delete [] SBIdToPP[i][j];
      SBIdToPP[i][j]=0;
    }
}

TGCConnectionPPToSB::TGCConnectionPPToSB()
{
  setNumberOfType(NumberOfPatchPanelType);
  int i,j;
  for( i=0; i<NumberOfPPPort; i+=1)
    for( j=0; j<NumberOfPatchPanelType; j+=1)
      SBIdToPP[i][j]=0;
}

TGCConnectionPPToSB::TGCConnectionPPToSB(const TGCConnectionPPToSB& right) : 
  TGCBoardConnection(right)
{
  int i,j,k;
  for( i=0; i<NumberOfPPPort; i+=1)
    for( j=0; j<NumberOfPatchPanelType; j+=1){
      if(SBIdToPP[i][j]!=0) delete [] SBIdToPP[i][j];
      SBIdToPP[i][j] = new int [numberOfBoard[j]];
      for( k=0; k<numberOfBoard[j]; k+=1)
	SBIdToPP[i][j][k] = right.SBIdToPP[i][j][k];
    }
}

TGCConnectionPPToSB& TGCConnectionPPToSB::operator=(const TGCConnectionPPToSB& right)
{
  if(this!=&right){
    int i,j,k;
    for( i=0; i<NumberOfPPPort; i+=1)
      for( j=0; j<NumberOfPatchPanelType; j+=1){
	if(SBIdToPP[i][j]!=0) delete [] SBIdToPP[i][j];
	SBIdToPP[i][j] = new int [numberOfBoard[j]];
	for( k=0; k<numberOfBoard[j]; k+=1)
	  SBIdToPP[i][j][k] = right.SBIdToPP[i][j][k];
      }
  }
  return *this;
}

} //end of namespace bracket
