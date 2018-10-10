///////////////////////// -*- C++ -*- /////////////////////////////
// FrontPanelCTP.cxx 
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// Implementation file for class FrontPanelCTP
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// TrigT1Interfaces includes
#include "TrigT1Interfaces/FrontPanelCTP.h"


// Constructors
LVL1::FrontPanelCTP::FrontPanelCTP()
{}

LVL1::FrontPanelCTP::FrontPanelCTP( unsigned int cableword0clock0,
                                    unsigned int cableword0clock1,
                                    unsigned int cableword1clock0,
                                    unsigned int cableword1clock1,
                                    unsigned int cableword2clock0,
                                    unsigned int cableword2clock1 ) :
   m_cableWord0{ cableword0clock0, cableword0clock1 },
   m_cableWord1{ cableword1clock0, cableword1clock1 }, 
   m_cableWord2{ cableword2clock0, cableword2clock1 }
{}

 
// Destructor
LVL1::FrontPanelCTP::~FrontPanelCTP()
{}


/** set the data that is sent on cable 0 */
void
LVL1::FrontPanelCTP::setCableWord0(unsigned int clock, uint32_t data) {
   m_cableWord0[clock==0?0:1] = data;
}

/** set the data that is sent on cable 1 */
void
LVL1::FrontPanelCTP::setCableWord1(unsigned int clock, uint32_t data) {
   m_cableWord1[clock==0?0:1] = data;
}

/** set the data that is sent on cable 2 */
void
LVL1::FrontPanelCTP::setCableWord2(unsigned int clock, uint32_t data) {
   m_cableWord2[clock==0?0:1] = data;
}


/** return the data that is sent on cable 0 */
unsigned int
LVL1::FrontPanelCTP::cableWord0(unsigned int clock) const {
   return m_cableWord0[clock==0?0:1];
}
  
/** return the data that is sent on cable 1 */
unsigned int
LVL1::FrontPanelCTP::cableWord1(unsigned int clock) const {
   return m_cableWord1[clock==0?0:1];
}

/** return the data that is sent on cable 2 */
unsigned int
LVL1::FrontPanelCTP::cableWord2(unsigned int clock) const {
   return m_cableWord2[clock==0?0:1];
}
  
  
