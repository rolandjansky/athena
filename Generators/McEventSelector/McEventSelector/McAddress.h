/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//	McAddress.h
//--------------------------------------------------------------------
//
//	Package    : McEventSelector 
//
//  Description: Definition of Mc address object
//
//	Author     : P. Calafiura
//====================================================================
#ifndef MCEVENTSELECTOR_MCADDRESS_H
#define MCEVENTSELECTOR_MCADDRESS_H

// C/C++ include files
#include <string>

// Framework include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/GenericAddress.h"


class McAddress   : public GenericAddress   {
public:
  /// Standard Destructor
  virtual ~McAddress() {}; //doesn't own event

  /// Standard Constructor
  McAddress(const CLID& clid, const std::string& fname,
		  const std::string& cname);
  McAddress(const CLID& clid, const std::string& fname,
	    const unsigned int& run, const uint64_t& event, const unsigned int& LB, const unsigned int& time);

  ///Event accessors
  inline void setRunEvt(unsigned int run, uint64_t evt, 
			unsigned int LB, unsigned int time) {
    m_runNo     = run;
    m_eventNo   = evt;
    m_LBNo      = LB;
    m_timeStamp = time;
  }

  unsigned int runNumber()   const { return m_runNo; }
  uint64_t     eventNumber() const { return m_eventNo; }
  unsigned int lumiBlock()   const { return m_LBNo; }
  unsigned int timeStamp()   const { return m_timeStamp; }

private:
  unsigned int m_runNo;
  uint64_t     m_eventNo;
  unsigned int m_LBNo;
  unsigned int m_timeStamp;
};
#endif // MCEVENTSELECTOR_MCADDRESS_H


