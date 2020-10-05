/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VTrackInformation_H
#define VTrackInformation_H

#include "G4VUserTrackInformation.hh"

enum TrackClassification { Primary, RegeneratedPrimary, RegisteredSecondary, Secondary, BarcodeOnly } ;

#include "AtlasHepMC/GenParticle_fwd.h"

namespace ISF {
  class ISFParticle;
}

class VTrackInformation: public G4VUserTrackInformation {
public:
  VTrackInformation(TrackClassification tc=Primary);
  HepMC::ConstGenParticlePtr GetPrimaryHepMCParticle() const;
  void  SetPrimaryHepMCParticle(HepMC::ConstGenParticlePtr);
  virtual HepMC::ConstGenParticlePtr GetHepMCParticle() const;
  virtual const ISF::ISFParticle *GetBaseISFParticle() const;
  virtual bool GetReturnedToISF() const;
  virtual int  GetParticleBarcode() const =0;
  virtual void SetParticle(HepMC::ConstGenParticlePtr);
  virtual void SetBaseISFParticle(const ISF::ISFParticle*);
  virtual void SetReturnedToISF(bool) ;
  virtual void Print() const {}
  void SetClassification(TrackClassification tc) {m_classify=tc;}
  TrackClassification GetClassification() {return m_classify;}
private:
  TrackClassification m_classify;
  HepMC::ConstGenParticlePtr m_thePrimaryParticle{};
};

#endif
