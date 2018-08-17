/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_BEAMSPOTID_H
#define INDET_BEAMSPOTID_H


namespace BeamSpot {
  class ID {
  public:
  ID():m_runNumber(0), m_lumiBlock(0), m_pileup(0), m_bcid(0), m_timeStamp(0) {}
    unsigned int runNumber() const {return m_runNumber; } 
    unsigned int lumiBlock() const {return m_lumiBlock; }
    unsigned int pileup() const    {return m_pileup;    }
    unsigned int bcid()   const    {return m_bcid;      }
    unsigned long timeStamp() const{return m_timeStamp; } 

    void runNumber(unsigned int run) {m_runNumber = run;}
    void lumiBlock(unsigned int lb ) {m_lumiBlock = lb;}
    void pileup(unsigned int pileup) {m_pileup = pileup;}
    void bcid(unsigned int bcid)     {m_bcid   = bcid; }
    void timeStamp(unsigned long time){m_timeStamp = time;}

    bool operator<( const ID & ) const;


  private:
    unsigned int  m_runNumber;
    unsigned int  m_lumiBlock;
    unsigned int  m_pileup;
    unsigned int  m_bcid;
    unsigned long m_timeStamp;
  };
}
#endif


