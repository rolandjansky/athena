/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauMETL1Sel.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply L1 MET selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _TRIGTAUMETL1SEL_H
#define _TRIGTAUMETL1SEL_H

class TrigTauEtMissL1Object;

//
// L1 MET trigger selector
//

class TrigTauMETL1Sel {
 public:
  TrigTauMETL1Sel();
  TrigTauMETL1Sel(const TrigTauMETL1Sel& other);

  void setCuts(float MET=0.); 

  void print() const;

  bool passed(const TrigTauEtMissL1Object& tau) const;
  void setDebug( bool newval) { _debug = newval;};

 private:
  float cut_MET;
  bool _debug;
};

#endif
