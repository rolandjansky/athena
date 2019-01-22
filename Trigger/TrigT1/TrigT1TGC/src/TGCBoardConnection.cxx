/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCBoardConnection.cxx,v 1.3 2009-01-08 10:06:48 isaya Exp $
#include "TrigT1TGC/TGCBoardConnection.hh"

namespace LVL1TGCTrigger {

TGCBoardConnection::TGCBoardConnection()
 : m_nType(0), m_numberOfBoard(0), m_id(0)
{}


TGCBoardConnection::~TGCBoardConnection()
{
  if(m_numberOfBoard!=0) delete [] m_numberOfBoard;
  if(m_id!=0){
    int i;
    for( i=0; i<m_nType; i+=1){
      if(m_id[i]!=0)
        delete [] m_id[i];
    }
    delete [] m_id;
    m_id=0;
  }
}

void TGCBoardConnection::setNumberOfType(int ntype)
{
  m_nType=ntype;
  if(m_numberOfBoard!=0) delete [] m_numberOfBoard; 
  m_numberOfBoard = new int [ntype];
  if(m_id!=0) delete [] m_id; 
  m_id = new int* [ntype];
  int i;
  for( i=0; i<ntype; i+=1) {
    m_id[i]=0;
    m_numberOfBoard[i]=0;
  }
}

void TGCBoardConnection::setNumber(int type, int nBoard)
{
  if(m_numberOfBoard!=0){
    if(type<m_nType){
      m_numberOfBoard[type] = nBoard;
      if(nBoard!=0){
	if(m_id[type]!=0) delete [] m_id[type];
	m_id[type] = new int [nBoard];
      }
    }
  }
}

void TGCBoardConnection::setId(int type, int board, int idIn) 
{
  if(m_numberOfBoard!=0)
    if((type<m_nType)&&(type>=0))
      if((m_id[type]!=0)&&(board<m_numberOfBoard[type])&&(board>=0))
	m_id[type][board] = idIn;
}


TGCBoardConnection::TGCBoardConnection(const TGCBoardConnection& right)
 : m_nType(0), m_numberOfBoard(0), m_id(0)
{
  m_nType = right.m_nType;
  if(m_nType!=0){
    if(m_numberOfBoard!=0) delete [] m_numberOfBoard; 
    m_numberOfBoard = new int [m_nType];
    for(int i=0; i<m_nType; i+=1) m_numberOfBoard[i] = right.m_numberOfBoard[i];

    if(m_id!=0) delete [] m_id;
    m_id = new int* [m_nType];
    for(int i=0; i<m_nType; i+=1){
      if(m_numberOfBoard[i]!=0){
	m_id[i] = new int [m_numberOfBoard[i]];
	for(int j=0; j<m_numberOfBoard[i]; j+=1) m_id[i][j]=right.m_id[i][j];
      }
    }
  }
}

TGCBoardConnection& TGCBoardConnection::operator=(const TGCBoardConnection& right)
{
  if(this!=&right){
    m_nType = right.m_nType;
    if(m_nType!=0){
      if(m_numberOfBoard!=0) delete [] m_numberOfBoard; 
      m_numberOfBoard = new int [m_nType];
      int i;
      for( i=0; i<m_nType; i+=1)
	m_numberOfBoard[i] = right.m_numberOfBoard[i];

      if(m_id!=0)	delete [] m_id; 
      m_id = new int* [m_nType];
      for( i=0; i<m_nType; i+=1){
	if(m_numberOfBoard[i]!=0){
	  m_id[i] = new int [m_numberOfBoard[i]];
	  int j;
	  for( j=0; j<m_numberOfBoard[i]; j+=1)
	    m_id[i][j]=right.m_id[i][j];
	}
      }
    }
  }
  return *this;
}


} //end of namespace bracket
