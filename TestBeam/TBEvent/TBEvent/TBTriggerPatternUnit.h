//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBTRIGGERPATTERNUNIT_H
#define TBEVENT_TBTRIGGERPATTERNUNIT_H 
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Trigger pattern for an event.
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/CLASS_DEF.h"




#include <vector>




class TBTriggerPatternUnit
{
 public:
  

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////
    
  TBTriggerPatternUnit();
  

  ~TBTriggerPatternUnit();
  
  ///////////////////////
  // Data Manipulation //
  ///////////////////////
    
    //    void setBit(int i,bool status){if(i<m_triggers.size()) m_triggers[i]=status;}
    void setTriggerWord(unsigned int word);

    bool getBit(unsigned int i) const {return i<m_triggers.size()? m_triggers[i]:false ;}
    const std::vector<bool>& getTriggers() const { return m_triggers; }

    unsigned int getTriggerWord() const {return m_triggerWord;}
    
    // copied from Peter Loch code :

    inline bool isParticle() const
    { return ( m_triggerWord & 0x00000100 ) != 0; }
    inline bool isRandom() const
    { return ( m_triggerWord & 0x00000200 ) != 0; }
    inline bool isPedestal() const { return this->isRandom(); }
    inline bool isMonitor() const
    { return ( m_triggerWord & 0x00000400 ) != 0; }
    inline bool isCalib() const { return this->isMonitor(); }
                                                                                                                          
    // specifics
    inline bool hasS1() const
    { return ( m_triggerWord & 0x00000001 ) != 0; }
    inline bool hasS2() const
    { return ( m_triggerWord & 0x00000002 ) != 0; }
    inline bool hasS3() const
    { return ( m_triggerWord & 0x00000004 ) != 0; }
    inline bool hasVeto() const
    { return ( m_triggerWord & 0x00000008 ) != 0; }
    inline bool hasTCFront() const
    { return ( m_triggerWord & 0x00000010 ) != 0; }
    bool hasTCBack() const
    { return ( m_triggerWord & 0x00000020 ) != 0; }
    inline bool hasMuon() const
    { return ( m_triggerWord & 0x00000040 ) != 0; }
    inline bool hasFibre() const
    { return ( m_triggerWord & 0x00000080 ) != 0; }
    inline bool hasLatePileUp() const
    { return ( m_triggerWord & 0x00000800 ) != 0; }
    inline bool hasEarlyPileUp() const
    { return ( m_triggerWord & 0x00001000 ) != 0; }
    inline bool hasCedar6of8() const
    { return ( m_triggerWord & 0x00002000 ) != 0; }
    inline bool hasCedar7of8() const
    { return ( m_triggerWord & 0x00004000 ) != 0; }
    inline bool hasCedar8of8() const
    { return ( m_triggerWord & 0x00008000 ) != 0; }
    


 private:
    
  //////////////////
  // Private Data //
  //////////////////

    unsigned int m_triggerWord;
    std::vector<bool> m_triggers;


};


CLASS_DEF( TBTriggerPatternUnit , 214386007 , 1 )
#endif
