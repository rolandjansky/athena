/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ParticleDataModifier_H
#define ParticleDataModifier_H

// helper class to allow W access to particle data

class G4ParticlePropertyTable;
class G4ParticleTable;

#include <string>

class ParticleDataModifier {
public:
	ParticleDataModifier();
	ParticleDataModifier(const std::string);

	void SetParticleMass(const std::string, double);
	void SetParticleWidth(const std::string, double);
	void SetParticleMass(double) const;
	void SetParticleWidth(double) const;
	void SetParticleLifeTime(const std::string, double);
	void SetParticleLifeTime(double) const;
	void Stable(const std::string, bool);
	void Stable(bool);
	void AddDecayChannel(const std::string, double,const std::string);
	void AddDecayChannel(double,const std::string) const;
	void SetPDGCode(const std::string, int);
	void SetPDGCode(int) const;
        void SetParticleCharge(const std::string, double);
        void SetParticleCharge(double) const;

private:
	G4ParticlePropertyTable* partProperties;
	G4ParticleTable* partTable;
	std::string partName;
};

#endif
