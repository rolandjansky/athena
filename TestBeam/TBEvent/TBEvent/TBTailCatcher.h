/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBTAILCATCHER_H
#define TBEVENT_TBTAILCATCHER_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Testbeam TailCatcher reconstructed signals.                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaKernel/CLASS_DEF.h"

#include "AthContainers/DataVector.h"
#include "TBEvent/TBBeamDetector.h"
#include "TBEvent/TBScintillator.h"

#include <vector>



class TBTailCatcher : public TBBeamDetector, 
		      public DataVector< TBScintillator >
{
 public:

  typedef double signal_type;

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBTailCatcher();
  TBTailCatcher(const std::string& thisTailCatcherName,
		bool overflow,
		const std::vector< TBScintillator* >& theScints);
 
  ~TBTailCatcher();
  
  ///////////////////////
  // Data Manipulation //
  ///////////////////////

  void addScintillator(TBScintillator* thisScint);

  const TBScintillator* 
    getScintillator(std::string thisScintName) const;

  // global overflow
  //  virtual void setOverflow();
  //  virtual void setOverflow(bool overflow);
  //  virtual void resetOverflow();

  /////////////////
  // Data Access //
  /////////////////

  const std::vector<signal_type>& getSignals() const;
  signal_type getSignal(std::string thisScintName) const;
  signal_type getSignal(const TBScintillator* thisScint) const;
  
 private:

  //////////
  // Data // 
  //////////

  std::vector<double> m_signals;

  unsigned int findIndex(std::string thisScintName) const;

};
CLASS_DEF( TBTailCatcher , 100992621 , 1 )
#endif
