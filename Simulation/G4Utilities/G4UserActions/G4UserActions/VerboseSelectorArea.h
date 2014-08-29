/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VerboseSelectorArea_H
#define VerboseSelectorArea_H

#include "FadsActions/UserAction.h"
#include <string>

class VerboseSelectorArea: public FADS::UserAction {

public:
	VerboseSelectorArea(std::string s): FADS::UserAction(s), m_Event(0),m_verb(0),
	  verboseLevel(0), m_nev(0), m_Xmin(0), m_Xmax(0), m_Ymin(0), m_Ymax(0), m_Zmin(0), m_Zmax(0) {}

	void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);
        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);
        void SteppingAction(const G4Step*);

private:

	int m_Event;
        int m_verb;
        int verboseLevel;
        int m_nev;
	double m_Xmin, m_Xmax;
        double m_Ymin, m_Ymax;
        double m_Zmin, m_Zmax;
	void ParseProperties();
};


#endif
