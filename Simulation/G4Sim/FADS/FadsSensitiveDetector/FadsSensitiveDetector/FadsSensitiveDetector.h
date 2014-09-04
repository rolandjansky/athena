/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsSensitiveDetector_H
#define FadsSensitiveDetector_H

#include <string>
#include "globals.hh"
#include "G4VSensitiveDetector.hh"
#include "AthenaKernel/MsgStreamMember.h"

// To expose all the inheriting classes to the macros
#include "AthenaBaseComps/AthMsgStreamMacros.h"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class G4VPhysicalVolume;

#include "G4Transform3D.hh"
#include <deque>
#include <string>

namespace FADS {

class FadsSensitiveDetector: public G4VSensitiveDetector {
private:
	std::string name;
	std::string detName;   // name of the detector an instance of this SD will be associated with
	G4Step *currentStep;
	std::deque<int> volumeIdentifiers;
	std::deque<std::string> volumeNames;
	void RetrieveVolumeTree();

protected:
	/// Log a message using the Athena controlled logging system
	MsgStream& msg( MSG::Level lvl ) const { return m_msg.get() << lvl; }
	/// Check whether the logging system is active at the provided verbosity level
	bool msgLvl( MSG::Level lvl ) { return m_msg.get().level() <= lvl; }
	/// Private message stream member
	mutable Athena::MsgStreamMember m_msg;
	bool m_allowMods;

public:

	void Register();
	std::string GetName();
	std::string GetDetectorName() {return detName;}
	void SetDetectorName(std::string n) {detName=n;}
	FadsSensitiveDetector(std::string n);
	virtual ~FadsSensitiveDetector();
	virtual void Initialize(G4HCofThisEvent *)=0; 
// We do provide an implementation of ProcessHits to transfer responsibility
// onto ProcessStep. Still declare it virtual to ensure backwards compat. ADA
	virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*);
	virtual void EndOfEvent(G4HCofThisEvent*)=0;
	virtual void ProcessStep() {}

	virtual void InitializeATLASEvent() {}
	virtual void EndOfATLASEvent() {}

	enum coordinates {WorldCoordinates, LocalCoordinates};
	G4ThreeVector InitialStepPosition(coordinates);
	G4ThreeVector FinalStepPosition(coordinates);

	void setAllowMods(bool allowMods) { m_allowMods=allowMods; }
};

}	// end namespace

#endif	// FadsSensitiveDetector
	
