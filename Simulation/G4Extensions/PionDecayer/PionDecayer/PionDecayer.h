/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PionDecayer_H
#define PionDecayer_H

#include "FadsKinematics/G4EventAnalyzer.h"
#include <utility>
#include <vector>

class PionDecayer: public FADS::G4EventAnalyzer {
private:
     G4int verboseLevel;
     static double alenz;
     static double cvrad;
     static double ptmuon;
     static bool m_useCustom;
     int m_motherId;
     std::vector<int> *m_childrenId;

     /* select pions and kaons coming from a decay mode specified through jobOptions */
     std::vector<G4PrimaryParticle*> customDecayMode( std::vector<G4PrimaryParticle*>& part );
     
public:
     PionDecayer(std::string s);
     ~PionDecayer();
     inline void switchOn(){TurnOn();};
     inline void switchOff(){TurnOff();};
     void SetVerboseLevel(G4int i) { verboseLevel=i; }
     G4int GetVerboseLevel() const { return verboseLevel; }
     bool GetStatus() {return IsOn();}

      // for the C++ layer only
     void EventInitialization();
     void EventAnalysis(std::vector<G4PrimaryVertex*>&, std::vector<G4PrimaryParticle*>&);
     static void SetIDETlength(double);
     static void SetIDETradius(double);
     static void SetPtmuon(double);
     static void UseCustomDecayMode();
     void SetMother(int);
     void AddDecayProduct(int);

private:
     PionDecayer(const PionDecayer&); //declaring, but not defining as an object of this class should never be copied.
     PionDecayer& operator= (const PionDecayer&); //declaring, but not defining as an object of this class should never be copied.
};

#endif
