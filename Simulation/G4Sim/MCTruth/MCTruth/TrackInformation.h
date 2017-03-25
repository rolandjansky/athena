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
	void SetReturnedToISF(bool returned) {m_returnedToISF=returned;};
	bool GetReturnedToISF() const {return m_returnedToISF;};
	void SetRegenerationNr(int i) {m_regenerationNr=i;};
	int GetRegenerationNr() const {return m_regenerationNr;};
private:
	int m_regenerationNr;
	const HepMC::GenParticle *m_theParticle;
	const ISF::ISFParticle *m_theBaseISFParticle;
	bool m_returnedToISF;
};

#endif
