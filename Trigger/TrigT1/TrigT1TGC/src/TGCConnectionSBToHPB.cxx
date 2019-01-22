/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCConnectionSBToHPB.cxx,v 1.3 2009-03-15 18:07:55 isaya Exp $
#include "TrigT1TGC/TGCConnectionSBToHPB.hh"
#include "TrigT1TGC/TGCBoardConnection.hh"

namespace LVL1TGCTrigger {


TGCConnectionSBToHPB::~TGCConnectionSBToHPB()
{
  int j;
  for( j=0; j<NumberOfSlaveBoardType; j+=1){
    if(m_HPBPortToSB[j]!=0) delete [] m_HPBPortToSB[j];
    m_HPBPortToSB[j]=0;

    if(m_HPBIdToSB[j]!=0) delete [] m_HPBIdToSB[j];
    m_HPBIdToSB[j]=0;
  }
}

TGCConnectionSBToHPB::TGCConnectionSBToHPB()
{
  // NO HPB for Inner
  setNumberOfType(4);
  
  int j;
  for( j=0; j<NumberOfSlaveBoardType; j+=1){
    m_HPBPortToSB[j]=0;
    m_HPBIdToSB[j]=0;
  }
}

TGCConnectionSBToHPB::TGCConnectionSBToHPB(const TGCConnectionSBToHPB& right) :
  TGCBoardConnection(right)
{
  int j,k;
  for( j=0; j<NumberOfSlaveBoardType; j+=1){
    if(m_HPBPortToSB[j]!=0) delete [] m_HPBPortToSB[j];
    m_HPBPortToSB[j] = new int [m_numberOfBoard[j]];
    if(m_HPBIdToSB[j]!=0) delete [] m_HPBIdToSB[j];
    m_HPBIdToSB[j] = new int [m_numberOfBoard[j]];
    for( k=0; k<m_numberOfBoard[j]; k+=1){
      m_HPBPortToSB[j][k] = right.m_HPBPortToSB[j][k];
      m_HPBIdToSB[j][k] = right.m_HPBIdToSB[j][k];
    }
  }
}

TGCConnectionSBToHPB& TGCConnectionSBToHPB::operator=(const TGCConnectionSBToHPB& right)
{
  if(this!=&right){
    TGCBoardConnection::operator=(right); // call base class assignment operator
    int j,k;
    for( j=0; j<NumberOfSlaveBoardType; j+=1){
      if(m_HPBPortToSB[j]!=0) delete [] m_HPBPortToSB[j];
      m_HPBPortToSB[j] = new int [m_numberOfBoard[j]];
      if(m_HPBIdToSB[j]!=0) delete [] m_HPBIdToSB[j];
      m_HPBIdToSB[j] = new int [m_numberOfBoard[j]];
      for( k=0; k<m_numberOfBoard[j]; k+=1){
	m_HPBPortToSB[j][k] = right.m_HPBPortToSB[j][k];
	m_HPBIdToSB[j][k] = right.m_HPBIdToSB[j][k];
      }
    }
  }
  return *this;
}

} //end of namespace bracket
