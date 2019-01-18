/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCConnectionHPBToSL.cxx,v 1.2 2006-06-22 00:26:35 nkanaya Exp $
#include "TrigT1TGC/TGCConnectionHPBToSL.hh"
#include "TrigT1TGC/TGCBoardConnection.hh"

namespace LVL1TGCTrigger {

// default constructor
TGCConnectionHPBToSL::TGCConnectionHPBToSL()
{
  setNumberOfType(NumberOfHighPtBoardType);
  for(int j=0; j<NumberOfHighPtBoardType; j++) m_SLPortToHPB[j]=0;
}

// Copy constructor
TGCConnectionHPBToSL::TGCConnectionHPBToSL(const TGCConnectionHPBToSL& right)
 : TGCBoardConnection(right)
{
  for(int j=0; j<NumberOfHighPtBoardType; j++) {
    if(m_SLPortToHPB[j]!=0) delete [] m_SLPortToHPB[j];
    m_SLPortToHPB[j] = new int [m_numberOfBoard[j]];

    for(int k=0; k<m_numberOfBoard[j]; k++) m_SLPortToHPB[j][k] = right.m_SLPortToHPB[j][k];
  }
}


TGCConnectionHPBToSL::~TGCConnectionHPBToSL()
{
  int j;
  for( j=0; j<NumberOfHighPtBoardType; j+=1){
      if(m_SLPortToHPB[j]!=0) delete [] m_SLPortToHPB[j];
      m_SLPortToHPB[j]=0;
  }
}

TGCConnectionHPBToSL& TGCConnectionHPBToSL::operator=(const TGCConnectionHPBToSL& right)
{
  if(this!=&right){
    TGCBoardConnection::operator=(right); // call base class assignment operator
    int j,k;
    for( j=0; j<NumberOfHighPtBoardType; j+=1){
      if(m_SLPortToHPB[j]!=0) delete [] m_SLPortToHPB[j];
      m_SLPortToHPB[j] = new int [m_numberOfBoard[j]]; 
      for( k=0; k<m_numberOfBoard[j]; k+=1)
	m_SLPortToHPB[j][k] = right.m_SLPortToHPB[j][k];
    }
  }
  return *this;
}

} //end of namespace bracket
