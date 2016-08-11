/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Reconstruction/MuonIdentification/muonEvent/MuonQualityWord.h 
 Author: Ketevi A. Assamagan, July 2009
 Transienti class to hold muon quality bits and the methods to manipulate it
 so as to return the the isLoose, isMedium and isTight to the use.
 This object is not persistified. 
 ---------------------------------------------------------------------------
 ***************************************************************************/

#include "muonEvent/MuonQualityWord.h"
#include "GaudiKernel/MsgStream.h"

MuonQualityWord::MuonQualityWord ( uint16_t qualityWord )

    :   m_qualityWord (qualityWord)	
{}

/** copy constructor */
MuonQualityWord::MuonQualityWord (const MuonQualityWord& qualityWord)
    :   m_qualityWord ( qualityWord.m_qualityWord ) 
{}

/** destructor */
MuonQualityWord::~MuonQualityWord() {}

/** Assignment operator */
MuonQualityWord& MuonQualityWord::operator=(const MuonQualityWord& rhs) {
  if ( this != &rhs ) {
     m_qualityWord  = rhs.m_qualityWord;
  }
  return *this;
}

/** MsgStream output */
MsgStream& operator << ( MsgStream& sl, const MuonQualityWord& qword)
// if namespace, need to use MsgStream& Rec::operator :-(
{ 
  sl <<"Muon quality word: "<<qword.qualityWord()<< " [";
  for( int index =15 ; index >= 0 ; index--){
    if (index==12 || index==8 || index==4) {
      if( qword.qualityWord() & (1<<index) ) sl << " 1 ][" ;
      else sl << " 0 ][" ;
    } else if (index==0) {
      if( qword.qualityWord() & (1<<index) ) sl << " 1 ]" ;
      else sl << " 0 ]" ;
    } else {
      if( qword.qualityWord() & (1<<index) ) sl << " 1" ;
      else sl << " 0" ;
    }
  }
  sl << endmsg;
  return sl; 
}

/** MsgStream output */
std::ostream& operator << ( std::ostream& sl, const MuonQualityWord& qword)
// if namespace, need to use MsgStream& Rec::operator :-(
{ 
  sl <<"Muon quality word: "<<qword.qualityWord()<< " [";
  for( int index =15 ; index >= 0 ; index--){
    if (index==12 || index==8 || index==4) {
      if( qword.qualityWord() & (1<<index) ) sl << " 1 ][" ;
      else sl << " 0 ][" ;
    } else if (index==0) {
      if( qword.qualityWord() & (1<<index) ) sl << " 1 ]" ;
      else sl << " 0 ]" ;
    } else {
      if( qword.qualityWord() & (1<<index) ) sl << " 1" ;
      else sl << " 0" ;
    }
  }
  sl << std::endl;
  return sl; 
}
