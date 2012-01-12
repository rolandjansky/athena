/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTAUCHAIN_H
#define TRIGTAUTAUCHAIN_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTauChain.h
/// Package : TrigTauPerformAnalysis
/// Author  : Olga Igonkina (Nikhef)
/// Created : May 2009
///
/// DESCRIPTION:
///           Container to associate L1-L2-EF into a trigger chain
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformNtuple/TrigTauL1Object.h"
#include "TrigTauPerformNtuple/TrigTauL2Object.h"
#include "TrigTauPerformNtuple/TrigTauRecEFObject.h"
#include "TrigTauPerformNtuple/TrigTauTrackCollection.h"

class TrigTauTauChain : public TObject {
 public:

  TrigTauTauChain();
  virtual ~TrigTauTauChain();
  void print() const;
  void reset();
  
  TrigTauTauChain& operator = (const TrigTauTauChain& other );

  // The following function returns the 4-vector of the highest
  // available trigger object (EF, L2, L1) available in this chain.
  // This is intended for matching to the offline reco objects.

  const TLorentzVector* trigger() const;

  // public members
  inline const TrigTauL1Object*    L1() const;
  inline const TrigTauL2Object*    L2() const;
         const TrigTauRecEFObject* EF() const ; // points to tauRec candidate
  inline const TrigTauRecEFObject* EF(unsigned int i) const;
  inline const TrigTauTrackCollection * tracksRoIEF() const;

  inline  TrigTauL1Object*    L1() ;
  inline  TrigTauL2Object*    L2() ;
          TrigTauRecEFObject* EF()  ; // points to tauRec candidate
  inline  TrigTauRecEFObject* EF(unsigned int i) ;
  inline  TrigTauTrackCollection * tracksRoIEF();



  TRef             r_L1;
  TRef             r_L2;
  TRefArray        r_EFlist;
  TRef             r_tracksRoIEF;
  TrigTauFlags     m_tags;

  

 private:

  ClassDef(TrigTauTauChain, 1) ;
};


const TrigTauL1Object*    TrigTauTauChain::L1() const { return (const TrigTauL1Object*) r_L1.GetObject(); }
const TrigTauL2Object*    TrigTauTauChain::L2() const { return (const TrigTauL2Object*) r_L2.GetObject(); } 
const TrigTauRecEFObject* TrigTauTauChain::EF(unsigned int i) const 
{ return (const TrigTauRecEFObject*) r_EFlist.At(i); } // this one we used for trigger 

const TrigTauTrackCollection * TrigTauTauChain::tracksRoIEF()const 
{ return (const TrigTauTrackCollection*) r_tracksRoIEF.GetObject(); }

 TrigTauL1Object*    TrigTauTauChain::L1()  { return ( TrigTauL1Object*) r_L1.GetObject(); }
 TrigTauL2Object*    TrigTauTauChain::L2()  { return ( TrigTauL2Object*) r_L2.GetObject(); } 
 TrigTauRecEFObject* TrigTauTauChain::EF(unsigned int i)  
{ return (TrigTauRecEFObject *) r_EFlist.At(i); } // this one we used for trigger 

 TrigTauTrackCollection * TrigTauTauChain::tracksRoIEF() 
{ return ( TrigTauTrackCollection*) r_tracksRoIEF.GetObject(); }




#endif
