/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_RawData.cxx
//   Implementation file for class BCM_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
// Version 1.1 19/08/2008 Daniel Dobos for BCM
// Version 1.2 15/05/2008 Daniel Dobos
// Version 1.3 23/05/2008 Dominique Tardif
// Version 1.4 02/03/2009 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include <new>
#include "InDetBCM_RawData/BCM_RawData.h"

////////////////////////
// default constructor
////////////////////////
BCM_RawData::BCM_RawData():
  m_word1(0),
  m_word2(0)
{}

////////////////////////
// constructor with parameters: word1, word2
////////////////////////
BCM_RawData::BCM_RawData(const unsigned int word1, const unsigned int word2) :
  m_word1(word1),
  m_word2(word2)
{}

////////////////////////
// constructor with parameter list:
// Channel identifier       0-15 
// Position of first pulse  0-63
// Width of first pulse     0-31
// Position of second pulse 0-63
// Width of second pulse    0-31
// Level 1 accept           0-15
// Bunch Crosing ID         0-4095
// Level1 Trigger ID        0-65535
////////////////////////
BCM_RawData::BCM_RawData(const unsigned int Channel, 
                         const unsigned int Pulse1Position,
                         const unsigned int Pulse1Width,
                         const unsigned int Pulse2Position,
                         const unsigned int Pulse2Width,
                         const unsigned int LVL1A,
                         const unsigned int BCID,
                         const unsigned int LVL1ID
                        ) :
 m_word1(((Channel        & 0x0000F)<< 0)+
         ((Pulse1Position & 0x0003F)<< 4)+
         ((Pulse1Width    & 0x0001F)<<10)+
         ((Pulse2Position & 0x0003F)<<15)+
         ((Pulse2Width    & 0x0001F)<<21)+
         ((LVL1A          & 0x0003F)<<26)
        ),
 m_word2(((BCID           & 0x00FFF)<< 0)+
         ((LVL1ID         & 0x0FFFF)<<12)
        )
{}

////////////////////////
// constructor with full parameter list:
// Channel identifier       0-15
// Position of first pulse  0-63
// Width of first pulse     0-31
// Position of second pulse 0-63
// Width of second pulse    0-31
// Level 1 accept           0-15
// Bunch Crosing ID         0-4095
// Level1 Trigger ID        0-65535
// Error                    0-15  
////////////////////////   
BCM_RawData::BCM_RawData(const unsigned int Channel,
                         const unsigned int Pulse1Position,
                         const unsigned int Pulse1Width,   
                         const unsigned int Pulse2Position,
                         const unsigned int Pulse2Width,   
                         const unsigned int LVL1A,
                         const unsigned int BCID, 
                         const unsigned int LVL1ID,
                         const unsigned int Error
                        ) :
 m_word1(((Channel        & 0x0000F)<< 0)+
         ((Pulse1Position & 0x0003F)<< 4)+
         ((Pulse1Width    & 0x0001F)<<10)+
         ((Pulse2Position & 0x0003F)<<15)+
         ((Pulse2Width    & 0x0001F)<<21)+
         ((LVL1A          & 0x0003F)<<26) 
        ),
 m_word2(((BCID           & 0x00FFF)<< 0)+
         ((LVL1ID         & 0x0FFFF)<<12)+
         ((Error          & 0x0000F)<<28)
        )
{}

////////////////////////
// destructor
////////////////////////
BCM_RawData::~BCM_RawData()
{}
