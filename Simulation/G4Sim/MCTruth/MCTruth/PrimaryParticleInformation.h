/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryParticleInformation_H
#define PrimaryParticleInformation_H

#include "G4VUserPrimaryParticleInformation.hh"
#include "HepMC/GenEvent.h"

namespace ISF {
  class ISFParticle;
}

class PrimaryParticleInformation: public G4VUserPrimaryParticleInformation {
public:
	PrimaryParticleInformation();
	PrimaryParticleInformation(const HepMC::GenParticle*, const ISF::ISFParticle* isp=0);
	const HepMC::GenParticle *GetHepMCParticle() const;
	int GetParticleBarcode() const;
	void SuggestBarcode(int bc);
	void SetParticle(const HepMC::GenParticle*);
	void Print() const {}
	int GetRegenerationNr() {return  m_regenerationNr;}
	void SetRegenerationNr(int i) {m_regenerationNr=i;}

	void SetISFParticle(const ISF::ISFParticle* isp);
	const ISF::ISFParticle* GetISFParticle() const;

private:
	const HepMC::GenParticle *m_theParticle;
	const ISF::ISFParticle* m_theISFParticle;

	int m_regenerationNr;
	int m_barcode;
};

#endif
