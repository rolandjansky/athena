/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VTrackInformation_H
#define VTrackInformation_H

#include "G4VUserTrackInformation.hh"

enum TrackClassification { Primary, RegeneratedPrimary, RegisteredSecondary, Secondary, BarcodeOnly } ;

namespace HepMC {
  class GenParticle;
}

namespace ISF {
  class ISFParticle;
}

class VTrackInformation: public G4VUserTrackInformation {
public:
	VTrackInformation(TrackClassification tc=Primary);
	const HepMC::GenParticle *GetPrimaryHepMCParticle() const;
	void  SetPrimaryHepMCParticle(const HepMC::GenParticle*);
	virtual const HepMC::GenParticle *GetHepMCParticle() const;
	virtual const ISF::ISFParticle *GetBaseISFParticle() const;
	virtual bool GetReturnedToISF() const;
	virtual int  GetParticleBarcode() const =0;
	virtual void SetParticle(const HepMC::GenParticle*);
	virtual void SetBaseISFParticle(const ISF::ISFParticle*);
	virtual void SetReturnedToISF(bool) ;
	virtual void Print() const {}
	void SetClassification(TrackClassification tc) {m_classify=tc;}
	TrackClassification GetClassification() {return m_classify;}
private:
	TrackClassification m_classify;
	const HepMC::GenParticle *m_thePrimaryParticle;
};

#endif
