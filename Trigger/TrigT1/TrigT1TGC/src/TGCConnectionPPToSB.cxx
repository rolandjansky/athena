/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCConnectionPPToSB.h"
#include "TrigT1TGC/TGCBoardConnection.h"

namespace LVL1TGCTrigger {

TGCConnectionPPToSB::~TGCConnectionPPToSB()
{
  int i,j;
  for( i=0; i<NumberOfPPPort; i+=1)
    for( j=0; j<TGCSector::NumberOfPatchPanelType; j+=1){
      if(m_SBIdToPP[i][j]!=0) delete [] m_SBIdToPP[i][j];
      m_SBIdToPP[i][j]=0;
    }
}

TGCConnectionPPToSB::TGCConnectionPPToSB()
{
  setNumberOfType(TGCSector::NumberOfPatchPanelType);
  int i,j;
  for( i=0; i<NumberOfPPPort; i+=1)
    for( j=0; j<TGCSector::NumberOfPatchPanelType; j+=1)
      m_SBIdToPP[i][j]=0;
}

TGCConnectionPPToSB::TGCConnectionPPToSB(const TGCConnectionPPToSB& right) : 
  TGCBoardConnection(right)
{
  int i,j,k;
  for( i=0; i<NumberOfPPPort; i+=1)
    for( j=0; j<TGCSector::NumberOfPatchPanelType; j+=1){
      if(m_SBIdToPP[i][j]!=0) delete [] m_SBIdToPP[i][j];
      m_SBIdToPP[i][j] = new int [m_numberOfBoard[j]];
      for( k=0; k<m_numberOfBoard[j]; k+=1)
	m_SBIdToPP[i][j][k] = right.m_SBIdToPP[i][j][k];
    }
}

TGCConnectionPPToSB& TGCConnectionPPToSB::operator=(const TGCConnectionPPToSB& right)
{
  if(this!=&right){
    int i,j,k;
    for( i=0; i<NumberOfPPPort; i+=1)
      for( j=0; j<TGCSector::NumberOfPatchPanelType; j+=1){
	if(m_SBIdToPP[i][j]!=0) delete [] m_SBIdToPP[i][j];
	m_SBIdToPP[i][j] = new int [m_numberOfBoard[j]];
	for( k=0; k<m_numberOfBoard[j]; k+=1)
	  m_SBIdToPP[i][j][k] = right.m_SBIdToPP[i][j][k];
      }
  }
  return *this;
}

}   // end of namespace
