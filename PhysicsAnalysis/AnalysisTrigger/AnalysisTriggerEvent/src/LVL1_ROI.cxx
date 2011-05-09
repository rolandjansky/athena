/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LVL1_ROI.cxx,v 1.2 2008-05-08 15:00:11 krasznaa Exp $

// Local include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"

/**
 * The constructor only makes sure that the member variables are initialised
 * to zero length.
 */
LVL1_ROI::LVL1_ROI()
   : m_muonROIs( 0 ), m_emtauROIs( 0 ), m_jetROIs( 0 ), m_jetetROIs( 0 ),
     m_energysumROIs( 0 ) {

}

/**
 * The destructor doesn't have to do anything.
 */
LVL1_ROI::~LVL1_ROI() {

}

/**
 * This function is used by the persistency converter(s). Should not be
 * interesting to the users.
 */
void LVL1_ROI::clear() {

   m_muonROIs.clear();
   m_emtauROIs.clear();
   m_jetROIs.clear();
   m_jetetROIs.clear();
   m_energysumROIs.clear();

   return;
}
