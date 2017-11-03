/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsFieldManager_H
#define FadsFieldManager_H

#include "G4FieldManager.hh"
#include <string>
#include "FadsField/FieldIntParameters.h"
class G4Track;
class G4Region;
class G4MagneticField;
class G4MagIntegratorStepper;

namespace FADS {

class FadsFieldManager: public G4FieldManager {
public:
	FadsFieldManager(std::string);
	std::string GetName() const;
	void SetField(std::string);
	virtual ~FadsFieldManager() {;}
	void SetStepper(G4MagIntegratorStepper* );
private:
	std::string m_name;
	FADS::FieldIntParameters* m_defaultParameters;
    const G4Track* m_previousTrack;
    G4Region* m_previousRegion;
 
};

}	// end namespace

#endif

