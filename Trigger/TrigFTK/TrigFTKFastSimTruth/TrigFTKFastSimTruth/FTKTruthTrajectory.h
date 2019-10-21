/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTruthTrajectory_h
#define FTKTruthTrajectory_h

class FTKRawHit;
namespace HepMC {
	class GenParticle;
}

// An FTKTruthTrajectory is just a holder for a
// truth particle and all FTKRawHits that (after
// clustering) are associated to that particle.
// It does not own the particle, nor the hits.
class FTKTruthTrajectory
{
public:
	std::vector<const FTKRawHit*> hits;
	const HepMC::GenParticle*     genParticle;
};

#endif //FTKTruthTrajectory_h
