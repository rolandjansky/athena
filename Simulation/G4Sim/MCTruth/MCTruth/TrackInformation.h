/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackInformation_H
#define TrackInformation_H

#include "VTrackInformation.h"

namespace ISF {
  class ISFParticle;
}

class TrackInformation: public VTrackInformation {
public:
	TrackInformation();
	TrackInformation(const HepMC::GenParticle*,const ISF::ISFParticle* baseIsp=0);
	const HepMC::GenParticle *GetHepMCParticle() const;
	const ISF::ISFParticle *GetBaseISFParticle() const;
	int GetParticleBarcode() const;
	void SetParticle(const HepMC::GenParticle*);
	void SetBaseISFParticle(const ISF::ISFParticle*);
	void SetReturnedToISF(bool returned) {returnedToISF=returned;};
	bool GetReturnedToISF() const {return returnedToISF;};
	void SetRegenerationNr(int i) {regenerationNr=i;};
	int GetRegenerationNr() const {return regenerationNr;};
private:
	int regenerationNr;
	const HepMC::GenParticle *theParticle;
	const ISF::ISFParticle *theBaseISFParticle;
	bool returnedToISF;
};

#endif
