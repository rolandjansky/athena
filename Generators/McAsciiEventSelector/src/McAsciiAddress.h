///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiAddress.h 
// Header file for class McAsciiAddress
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCASCIIEVENTSELECTOR_MCASCIIADDRESS_H 
#define MCASCIIEVENTSELECTOR_MCASCIIADDRESS_H 

// STL includes
#include <string>

// Gaudi includes
#include "GaudiKernel/Kernel.h" // CLID
#include "GaudiKernel/GenericAddress.h"

// Forward declaration

class McAsciiAddress : public GenericAddress
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  McAsciiAddress( const CLID& clid, const std::string& fname,
		  const std::string& cname );

  McAsciiAddress( const CLID& clid, 
		  const std::string& fname,
		  const unsigned int runNbr, 
		  const unsigned int evtNbr,
		  const unsigned int timeStamp );

  /// Destructor: 
  virtual ~McAsciiAddress(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  
  unsigned int runNumber()   const;
  unsigned int eventNumber() const;
  unsigned int timeStamp()   const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// set all data members in one go
  void setRunEvt( const unsigned int runNbr, const unsigned int evtNbr,
		  const unsigned int timeStamp );

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  McAsciiAddress();

  /// Copy constructor: 
  McAsciiAddress( const McAsciiAddress& rhs );

  /// Assignment operator: 
  McAsciiAddress& operator=( const McAsciiAddress& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  unsigned int m_runNbr;
  unsigned int m_eventNbr;
  unsigned int m_timeStamp;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline unsigned int McAsciiAddress::runNumber() const
{ return m_runNbr; }

inline unsigned int McAsciiAddress::eventNumber() const
{ return m_eventNbr; }

inline unsigned int McAsciiAddress::timeStamp() const
{ return m_timeStamp; }

inline void McAsciiAddress::setRunEvt( const unsigned int runNbr, 
				       const unsigned int evtNbr,
				       const unsigned int timeStamp )
{
  m_runNbr    = runNbr;
  m_eventNbr  = evtNbr;
  m_timeStamp = timeStamp;
}

#endif //> MCASCIIEVENTSELECTOR_MCASCIIADDRESS_H
