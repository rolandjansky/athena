/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuonReceiverObj.cxx
// Author   : Joao Gentil Mendes Saraiva (jmendes@cern.ch)
// Created  : February 2014
//
// DESCRIPTION
// 
// Tile Muon Receiver objects to store in a container in the store gate
//  
//
// Properties (JobOption Parameters):
//
// BUGS:
//  
// History:
//   
//****************************************************************************

#include "TileEvent/TileMuonReceiverObj.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// print out

void TileMuonReceiverObj::print() const{
 std::cout << (std::string) (*this) << std::endl;
}

TileMuonReceiverObj::operator std::string() const
 {
   std::ostringstream text(std::ostringstream::out);
   text << whoami();
   text <<" "<< m_id;
   
   text << " Tile decision (4-bit word) ";
   for (unsigned int i=0; i<m_TileMuRcvDecision.size(); ++i) text << " " << m_TileMuRcvDecision[i];
   text << " Thres d6H d6L d56H d56L "; 
   for (unsigned int i=0; i<m_TileMuRcvThresholds.size(); ++i) text << " " << m_TileMuRcvThresholds[i];;
   text << " e(d6) e(d5+d6) ";
   for (unsigned int i=0; i<m_TileMuRcvEne.size(); ++i) text << " " << m_TileMuRcvEne[i];
   text << " t(d6) t(d5+d6/2.) "; 
   for (unsigned int i=0; i<m_TileMuRcvTime.size(); ++i) text << " " << m_TileMuRcvTime[i];

   return text.str();
 }

void TileMuonReceiverObj::clear(){  

  m_TileMuRcvDecision.clear(); 
  m_TileMuRcvThresholds.clear(); 
  m_TileMuRcvEne.clear(); 
  m_TileMuRcvTime.clear();
  return;

}
