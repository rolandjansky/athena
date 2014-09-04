/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasAlg_H
#define G4AtlasAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include <string>
#include <map>

class AthenaStackingAction;
class AthenaTrackingAction;
class G4AtlasRunManager;

template <class ConcreteAlgorithm> class AlgFactory;


class G4AtlasAlg : public AthAlgorithm {

    friend class AlgFactory<G4AtlasAlg>;

public:

	G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator);
  	virtual ~G4AtlasAlg() {  };

  	StatusCode initialize();
  	StatusCode finalize();
  	StatusCode execute();

	void initializeG4();

private:

  	G4AtlasRunManager* p_runMgr;

	// Properties for the jobOptions
  	std::string libList;
	std::string physList;
	std::string generator;
	std::string fieldMap;
	std::string rndmGen;
	bool m_KillAllNeutrinos;
	double m_KillLowEPhotons;
	bool m_releaseGeoModel;
        bool m_IncludeParentsInG4Event;
        bool m_killAbortedEvents;
        bool m_flagAbortedEvents;

	std::map<std::string,std::string> m_verbosities;

	AthenaStackingAction* stackingAction;
	AthenaTrackingAction* trackingAction;

    // Random number service
    ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;
};

#endif
