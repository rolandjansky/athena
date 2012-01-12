/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTRUEPARTICLE_H
#define TRIGTAUTRUEPARTICLE_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTrueParticle
/// Package : TauTrigPerformance
/// Author  : Olya Igonkina (University of Oregon)
/// Created : April 2006
///
/// DESCRIPTION:
///          MC truth
///
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "TRef.h"
#include "TRefArray.h"
#include "TLorentzVector.h"

class TrigTauTrueParticle : public TLorentzVector
{
 public:

  TrigTauTrueParticle();
  TrigTauTrueParticle(const  TrigTauTrueParticle & other );
  virtual ~TrigTauTrueParticle();
  
  TrigTauTrueParticle &         operator = ( const TrigTauTrueParticle & other );


  inline       TrigTauTrueParticle *  mother() ;
  inline const TrigTauTrueParticle *  mother() const ;

  inline unsigned int           nDaughters() const {return m_nDaughters;};
  inline unsigned int           nSavedDaughters() const {return r_daughters.GetEntriesFast();};
  inline       TrigTauTrueParticle *  daughter(unsigned int i )       ;
  inline const TrigTauTrueParticle *  daughter(unsigned int i ) const ;

  inline int                    pdgId() const {return m_pdgId;};
  inline int                    status() const {return m_status;};
  int                           charge() const ; // determined from pdgId
  const char *                  pdgName() const { return pdgName(pdgId()); };
  const char *                  statusName() const { return statusName(m_status); };


  void                          print(int shift = 0, bool completeTree=true) const;
  void                          prettyPrintDecayChain(unsigned int indent) const;

  // set functions

  void setMother(TrigTauTrueParticle & mother) { r_mother =  &mother;};
  void addDaughter( TrigTauTrueParticle & dau) { r_daughters.Add(&dau);};// dau.setMother(*this);};
  void setPdgId( int pdg) {m_pdgId = pdg;};
  void setStatus(int stat) {m_status = stat;};
  void setNDaughters(int nDau) { m_nDaughters = nDau;};
  const TrigTauTrueParticle * leadingParticle() const;

  static const char * pdgName(int pdgId);
  static const char * statusName(int status);


  // susy MC id :
  bool  isSusyParticle() const;
  bool  isStartOfSusyChain() const;
  const TrigTauTrueParticle*  findMotherOfFirstSusyDecay() const;



 private :
  int m_nDaughters; // actual number of daughters
  int m_pdgId;
  int m_status; // 1 - final; 2- decayed or fragmented; 3 - documentation line
  TRef r_mother;
  TRefArray r_daughters; // or all daughters or 0 !

 private:

  ClassDef(TrigTauTrueParticle, 3)  ;
};


//============================================== inline functions

TrigTauTrueParticle * TrigTauTrueParticle::mother() { return ( TrigTauTrueParticle*) r_mother.GetObject(); }

const TrigTauTrueParticle * TrigTauTrueParticle::mother() const { return (const TrigTauTrueParticle*) r_mother.GetObject(); }

const TrigTauTrueParticle *  TrigTauTrueParticle::daughter(unsigned int i ) const
{ return  (TrigTauTrueParticle*)r_daughters.At(i); }
TrigTauTrueParticle *  TrigTauTrueParticle::daughter(unsigned int i )
{ return  (TrigTauTrueParticle*)r_daughters.At(i); }




#endif
