/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBUTILS_TBPATTERNUNITHELPER_H
#define TBUTILS_TBPATTERNUNITHELPER_H

#include "TBEvent/TBTriggerPatternUnit.h"

#include <string>

struct TBPatternUnitContext
{
  public:
  
  enum Context { H6 = 0, H8 = 1, Unknown = -1 };
}

struct TBH6PatternUnitHelper
{
public:
 
enum Bits { S1          = 0,
	    S2          = 1,
	    S3          = 2,
	    Veto        = 3,
	    TCFront     = 4,
	    TCBack      = 5,
	    Muon        = 6,
	    EarlyPileUp = 7,
	    LatePileUp  = 8,
	    Unknown     = 9 };
  
    // copied from Peter Loch code :

    inline bool isParticle()
    { return ( m_triggerWord & 0x00000100 ) != 0; }
    inline bool isRandom()
    { return ( m_triggerWord & 0x00000200 ) != 0; }
    inline bool isPedestal() { return this->isRandom(); }
    inline bool isMonitor()
    { return ( m_triggerWord & 0x00000400 ) != 0; }
    inline bool isCalib() { return this->isMonitor(); }
                                                                                                                          
    // specifics
    inline bool hasS1()
    { return ( m_triggerWord & 0x00000001 ) != 0; }
    inline bool hasS2()
    { return ( m_triggerWord & 0x00000002 ) != 0; }
    inline bool hasS3()
    { return ( m_triggerWord & 0x00000004 ) != 0; }
    inline bool hasVeto()
    { return ( m_triggerWord & 0x00000008 ) != 0; }
    inline bool hasTCFront()
    { return ( m_triggerWord & 0x00000010 ) != 0; }
    bool hasTCBack()
    { return ( m_triggerWord & 0x00000020 ) != 0; }
    inline bool hasMuon()
    { return ( m_triggerWord & 0x00000040 ) != 0; }
    inline bool hasFibre()
    { return ( m_triggerWord & 0x00000080 ) != 0; }
    inline bool hasLatePileUp()
    { return ( m_triggerWord & 0x00000800 ) != 0; }
    inline bool hasEarlyPileUp()
    { return ( m_triggerWord & 0x00001000 ) != 0; }
    inline bool hasCedar6of8()
    { return ( m_triggerWord & 0x00002000 ) != 0; }
    inline bool hasCedar7of8()
    { return ( m_triggerWord & 0x00004000 ) != 0; }
    inline bool hasCedar8of8()
    { return ( m_triggerWord & 0x00008000 ) != 0; }
    


 
  static bool hasS1(const TBTriggerPatternUnit* aTrigger)
  {

  private:

    static std::vector<unsigned int> m_maskBits;
    static std::vector<std::string>  m_maskNames;

};
#endif
