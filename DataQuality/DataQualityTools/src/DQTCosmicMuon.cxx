/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     DQTCosmicMuon.cxx
PACKAGE:  DataQuality/DataQualityTools

AUTHORS:  Vu Anh Tuan
CREATED:  June 2007

PURPOSE:  object containing some muon parameters

********************************************************************/

// INCLUDE HEADER FILES:

#include "DataQualityTools/DQTCosmicMuon.h"

//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructors
// ----------------------------


DQTCosmicMuon::DQTCosmicMuon(const float &eta, const float &phi,
		       const float &x0, const float &y0)
{
  m_eta = eta;
  m_phi = phi;
  m_x0  = x0 ; 
  m_y0  = y0 ; 

}//end of default constructor


// copy constructor
DQTCosmicMuon::DQTCosmicMuon(const DQTCosmicMuon &input)
{
  if (this != &input) {
    *this = input;
  }
}//end of copy constructor

// destructor
DQTCosmicMuon::~DQTCosmicMuon()
{}//end of destructor

// assignment operator

DQTCosmicMuon& DQTCosmicMuon::operator = (const DQTCosmicMuon &input)
{
  if (this != &input) {
    m_eta = input.m_eta;
    m_phi = input.m_phi;
    m_x0  = input.m_x0 ;
    m_y0  = input.m_y0 ;
  }
  return *this;

}//end of assignment operator
