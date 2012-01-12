/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTRIGCHAINLIST_H
#define TRIGTAUTRIGCHAINLIST_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTrigChainList.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           List of trigger chains, and associated analysis functions
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformAnalysis/TrigTauTrigChain.h"
#include "TrigTauPerformNtuple/TrigTauEvent.h"

class TrigTauSelectionManager;

class TrigTauTrigChainList : public TObject {

 public:
  TrigTauTrigChainList();
  virtual ~TrigTauTrigChainList();

  //
  // Build trigger chains (before trigger has been applied)
  // There is one chain per L1 ROI
  //
  void fillChainList(const TrigTauEvent* event);
  void printChainList() const;
  bool checkRoIWord() const;

  // Matches based on dR to the L1 object.  No trigger selection applied
  const TrigTauTrigChain* getChain(const TLorentzVector& goodTau) const;

  // Trigger chain accessors
  unsigned int nTrigChains() const;
  const TrigTauTrigChain& trigChainEntry(unsigned int i) const;

  //
  // Apply trigger conditions (user must provide trigMan)
  //
  void fillTriggerList(TrigTauSelectionManager& trigMan);

  //
  // Trigger chain accessors only for objects passing trigger conditions
  //
  unsigned int nPassL1Chains() const;
  unsigned int nPassL2CalChains() const;
  unsigned int nPassL2TrkChains() const;
  unsigned int nPassL2Chains() const;
  unsigned int nPassEFChains() const;

  const TrigTauTrigChain& passL1ChainEntry(unsigned int i) const;
  const TrigTauTrigChain& passL2CalChainEntry(unsigned int i) const;
  const TrigTauTrigChain& passL2TrkChainEntry(unsigned int i) const;
  const TrigTauTrigChain& passL2ChainEntry(unsigned int i) const;
  const TrigTauTrigChain& passEFChainEntry(unsigned int i) const;
  
  //
  // MC truth - trigger object matching functions (or any other 4-vector)
  // Return object which passed the trigger, closest in dR.
  // Return NULL for no match in dR range
  //
  const TrigTauL1Object*    getL1Tau(const TLorentzVector& goodTau) const;
  const TrigTauL2Object*    getL2CalTau(const TLorentzVector& goodTau) const;
  const TrigTauL2Object*    getL2TrkTau(const TLorentzVector& goodTau) const;
  const TrigTauL2Object*    getL2Tau(const TLorentzVector& goodTau) const;
  const TrigTauRecEFObject* getEFTau(const TLorentzVector& goodTau) const;

  // Same matching, but return the entire chain
  const TrigTauTrigChain*   getL1Chain(const TLorentzVector& goodTau) const;
  const TrigTauTrigChain*   getL2CalChain(const TLorentzVector& goodTau) const;
  const TrigTauTrigChain*   getL2TrkChain(const TLorentzVector& goodTau) const;
  const TrigTauTrigChain*   getL2Chain(const TLorentzVector& goodTau) const;
  const TrigTauTrigChain*   getEFChain(const TLorentzVector& goodTau) const;

  // Note also the getChain accessor above for unselected chains

  // Choose how to do the matching.  RoiWord is correct, but only
  // available in TTP05 and beyond.  Otherwise, dR matching is used
  // by default
  void setRoiWordMatching(bool val=true) {_useRoiWord = val;}

  // Set matching dR 
  void setMaxL1dR(float dR) {_maxL1dR = dR;}
  void setMaxL2dR(float dR) {_maxL2dR = dR;}
  void setMaxEFdR(float dR) {_maxEFdR = dR;}
  void setMaxRedR(float dR) {_maxRedR = dR;}  // Offline reco dR

  // From TTP10 onwards, there can be multiple instances of
  // ROIs in HLT depending on the algorithms run.
  // The following functions can restrict which ROI types are considered
  // the default is IDSCAN (tauNoCut) signatures
  void setHLTAuthor(TrigTauFlags::HLTAUTHOR type) {_hltAuthorType = type;}

  // Shortcuts
  void setAnyAuthor()        {setHLTAuthor(TrigTauFlags::ANYAUTHOR);}
  void setCaloOnly()         {setHLTAuthor(TrigTauFlags::CALOONLY);}
  void setIDOnly()           {setHLTAuthor(TrigTauFlags::IDONLY);}
  void setIDSCAN()           {setHLTAuthor(TrigTauFlags::IDSCAN);}
  void setSITRK()            {setHLTAuthor(TrigTauFlags::SITRK);}
  void setTRT()              {setHLTAuthor(TrigTauFlags::TRTONLY);}

  // ---------------- data members ------------------------  
 private:

  // Chain list
  std::vector<TrigTauTrigChain> _chainList;

  // Trigger lists containing full chains passing trigger selectors
  std::vector<const TrigTauTrigChain*> _tauL1;
  std::vector<const TrigTauTrigChain*> _tauL2Cal;
  std::vector<const TrigTauTrigChain*> _tauL2Trk;
  std::vector<const TrigTauTrigChain*> _tauL2;
  std::vector<const TrigTauTrigChain*> _tauEF;

  // Matching cuts
  float _maxL1dR;
  float _maxL2dR;
  float _maxEFdR;
  float _maxRedR;

  // How to perform matching
  bool _useRoiWord;

  void fillChainListRoiWord(const TrigTauEvent* event);
  void fillChainListdRMatch(const TrigTauEvent* event);

  // Radius utilities
  //  float dRadius2(const TrigTauL1Object& t1, const TLorentzVector& t2) const;
  float dRadius2(const TLorentzVector&  t1, const TLorentzVector& t2) const;

  TrigTauFlags::HLTAUTHOR _hltAuthorType;

private:

  ClassDef(TrigTauTrigChainList, 2) ;
};

#endif
