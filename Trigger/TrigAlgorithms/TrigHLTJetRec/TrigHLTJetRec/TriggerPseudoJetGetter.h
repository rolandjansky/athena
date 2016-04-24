/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TriggerPseudoJetGetter.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: TriggerPseudoJetGetter is a dual-use tool to retrieve and 
// build the pseudojet inputs used in jet reconstruction.
// 
// ********************************************************************

#ifndef TRIGHLTJETREC_TRIGGERPSEUDOJETGETTER_H
#define TRIGHLTJETREC_TRIGGERPSEUDOJETGETTER_H

#include "TrigHLTJetRec/ITriggerPseudoJetGetter.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetEDM/PseudoJetVector.h"


class TriggerPseudoJetGetter: public PseudoJetGetter,
  virtual public ITriggerPseudoJetGetter {

 public:
	  
  TriggerPseudoJetGetter(const std::string&,
                         const std::string&,
                         const IInterface*); 

  virtual ~TriggerPseudoJetGetter();

  StatusCode initialize();

  // Stores a vector of pseudo jets to be used in get()
  void prime(const PseudoJetVector* ) override;

  // Returns the pseudojet collection
  const PseudoJetVector* get() const override;

  // Write properties to the log
  void print() const override;


 private: 

  bool m_primed;  // Indicates if prime has been called before get()

  const PseudoJetVector*  m_pseudoJetVector;

};

#endif
