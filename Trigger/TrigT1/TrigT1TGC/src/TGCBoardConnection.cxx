/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCBoardConnection.cxx,v 1.3 2009-01-08 10:06:48 isaya Exp $
#include "TrigT1TGC/TGCBoardConnection.hh"

namespace LVL1TGCTrigger {

TGCBoardConnection::TGCBoardConnection()
 : nType(0), numberOfBoard(0), id(0)
{}


TGCBoardConnection::~TGCBoardConnection()
{
  if(numberOfBoard!=0) delete [] numberOfBoard;
  if(id!=0){
    int i;
    for( i=0; i<nType; i+=1){
      if(id[i]!=0)
        delete [] id[i];
    }
    delete [] id;
    id=0;
  }
}

void TGCBoardConnection::setNumberOfType(int ntype)
{
  nType=ntype;
  if(numberOfBoard!=0) delete [] numberOfBoard; 
  numberOfBoard = new int [ntype];
  if(id!=0) delete [] id; 
  id = new int* [ntype];
  int i;
  for( i=0; i<ntype; i+=1) {
    id[i]=0;
    numberOfBoard[i]=0;
  }
}

void TGCBoardConnection::setNumber(int type, int nBoard)
{
  if(numberOfBoard!=0){
    if(type<nType){
      numberOfBoard[type] = nBoard;
      if(nBoard!=0){
	if(id[type]!=0) delete [] id[type];
	id[type] = new int [nBoard];
      }
    }
  }
}

void TGCBoardConnection::setId(int type, int board, int idIn) 
{
  if(numberOfBoard!=0)
    if((type<nType)&&(type>=0))
      if((id[type]!=0)&&(board<numberOfBoard[type])&&(board>=0))
	id[type][board] = idIn;
}


TGCBoardConnection::TGCBoardConnection(const TGCBoardConnection& right)
 : nType(0), numberOfBoard(0), id(0)
{
  nType = right.nType;
  if(nType!=0){
    if(numberOfBoard!=0) delete [] numberOfBoard; 
    numberOfBoard = new int [nType];
    for(int i=0; i<nType; i+=1) numberOfBoard[i] = right.numberOfBoard[i];

    if(id!=0) delete [] id;
    id = new int* [nType];
    for(int i=0; i<nType; i+=1){
      if(numberOfBoard[i]!=0){
	id[i] = new int [numberOfBoard[i]];
	for(int j=0; j<numberOfBoard[i]; j+=1) id[i][j]=right.id[i][j];
      }
    }
  }
}

TGCBoardConnection& TGCBoardConnection::operator=(const TGCBoardConnection& right)
{
  if(this!=&right){
    nType = right.nType;
    if(nType!=0){
      if(numberOfBoard!=0) delete [] numberOfBoard; 
      numberOfBoard = new int [nType];
      int i;
      for( i=0; i<nType; i+=1)
	numberOfBoard[i] = right.numberOfBoard[i];

      if(id!=0)	delete [] id; 
      id = new int* [nType];
      for( i=0; i<nType; i+=1){
	if(numberOfBoard[i]!=0){
	  id[i] = new int [numberOfBoard[i]];
	  int j;
	  for( j=0; j<numberOfBoard[i]; j+=1)
	    id[i][j]=right.id[i][j];
	}
      }
    }
  }
  return *this;
}


} //end of namespace bracket
