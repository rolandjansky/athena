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
	virtual const HepMC::GenParticle *GetHepMCParticle() const;
	virtual const ISF::ISFParticle *GetISFParticle() const;
	virtual int GetParticleBarcode() const =0;
	virtual void SetParticle(const HepMC::GenParticle*);
	virtual void SetISFParticle(const ISF::ISFParticle*);
	virtual void Print() const {}
	void SetClassification(TrackClassification tc) {classify=tc;}
	TrackClassification GetClassification() {return classify;}
private:
	TrackClassification classify;
};

#endif
