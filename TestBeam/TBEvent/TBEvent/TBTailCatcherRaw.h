/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBTAILCATCHERRAW_H
#define TBEVENT_TBTAILCATCHERRAW_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Testbeam TailCatcher reconstructed signals.                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaKernel/CLASS_DEF.h"

#include "AthContainers/DataVector.h"
#include "TBEvent/TBBeamDetector.h"
#include "TBEvent/TBScintillatorRaw.h"

#include <vector>



class TBTailCatcherRaw : public TBBeamDetector, 
		      public DataVector< TBScintillatorRaw >
{
 public:

  typedef int signal_type;

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBTailCatcherRaw();
  TBTailCatcherRaw(const std::string& thisTailCatcherName,
		bool overflow,
		const std::vector< TBScintillatorRaw* >& theScints);
 
  ~TBTailCatcherRaw();
  
  ///////////////////////
  // Data Manipulation //
  ///////////////////////

  void addScintillator(TBScintillatorRaw* thisScint);

  const TBScintillatorRaw* 
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
  signal_type getSignal(const TBScintillatorRaw* thisScint) const;
  
 private:

  //////////
  // Data // 
  //////////

  std::vector<int> m_signals;

  unsigned int findIndex(std::string thisScintName) const;

};
CLASS_DEF( TBTailCatcherRaw , 151151490 , 1 )
#endif
