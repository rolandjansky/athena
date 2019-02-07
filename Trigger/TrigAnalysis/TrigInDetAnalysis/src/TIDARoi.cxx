/**
 **     @file    TIDARoi.cxx
 **
 **     @author  mark sutton
 **     @date    Tue  2 Feb 2010 00:11:09 GMT 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include "TrigInDetAnalysis/TIDARoi.h"

ClassImp(TIDA::Roi)


TIDA::Roi::Roi() { } 
TIDA::Roi::Roi(const TIDARoiDescriptor& roi) : m_roi(roi) { } 

TIDA::Roi::~Roi() { } 



void TIDA::Roi::addTracks( const std::vector<TIDA::Track*>& trackp) { 
   clear();
   m_tracks.reserve(trackp.size());
   for ( unsigned i=0 ; i<trackp.size() ; i++ )  m_tracks.push_back(*trackp[i]);
}

