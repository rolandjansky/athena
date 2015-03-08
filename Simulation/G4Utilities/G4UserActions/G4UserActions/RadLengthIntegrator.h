/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RadLengthIntegrator_H
#define RadLengthIntegrator_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>
#include <map>

class TProfile;
class TProfile2D;

// User action to evaluate the thickness (in %r.l.) of all detectors
// traversed by outgoing particles
// this action should be activated at the beginning and the end of a run,
// beginning and the end of the event, and at step level
// Histogramming part to be replaced as soon as profile histograms supported

class RadLengthIntegrator: public FADS::ActionsBase , public FADS::UserAction {

  public:
   RadLengthIntegrator(std::string s): FADS::ActionsBase(s),FADS::UserAction(s),m_etaPrimary(0),m_phiPrimary(0),m_rzProf(0) {}
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void SteppingAction(const G4Step*);

  private:
   double m_etaPrimary ;
   double m_phiPrimary ;
   std::map<std::string,double,std::less<std::string> > m_detThick;
   std::map<std::string,TProfile*,std::less<std::string> > m_etaMap;
   std::map<std::string,TProfile*,std::less<std::string> > m_phiMap;
   TProfile2D* m_rzProf;
};

#endif
