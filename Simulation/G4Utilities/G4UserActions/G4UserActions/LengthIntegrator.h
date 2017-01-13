/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_LengthIntegrator_H
#define G4UserActions_LengthIntegrator_H

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>
#include <map>

#include "G4Pow.hh"
#include "TString.h"

class TProfile;
class TProfile2D;


// User action to evaluate the thickness (in %r.l. or i.l.) of all detectors
// traversed by outgoing particles

#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"

namespace G4UA
{

  /// @class LengthIntegrator
  /// @brief A user action used to evaluate thickness of all detectors
  ///        traversed by outgoing particles.
  ///
  /// This user action is currently used only in special runs with geantinos.
  /// Thickness is recorded in terms of both rad length and int length.
  ///
  /// NOTE: the current design is safe for multi-threading, but _not_
  /// performant due to sharing of the histograms and excessive locking. If
  /// this action needs to be used in multi-threaded jobs, we can rewrite it so
  /// that each instance has its own copy of the histograms which get merged in
  /// finalization of the LengthIntegratorTool.
  ///
  class LengthIntegrator final : public IBeginEventAction,
                                 public IEndEventAction,
                                 public ISteppingAction
  {

    public:

      /// Constructor takes the name of the histogram service as argument.
      LengthIntegrator(const std::string& histSvcName);

      /// Called at beginning of G4 event to cache some details about the
      /// current primary vertex and particle. Also resets some measurements.
      virtual void beginOfEvent(const G4Event*) override;

      /// Called at end of G4 event to finalize measurements and fill hists
      virtual void endOfEvent(const G4Event*) override;

      /// Called at every particle step to accumulate thickness.
      virtual void processStep(const G4Step*) override;

    private:

      // Holder for G4 math tools
      G4Pow* m_g4pow;

      bool m_doRL;
      bool m_doIL;
      bool m_doRZ;
      bool m_doXY;
      bool m_doEl;

      // Add elements and values into the map
      void addToDetThick(std::string, double, double);

      /// Setup one set of measurement hists for a detector name.
      void regAndFillHist(const std::string&, const std::pair<double, double>&);

      /// this method checks if a histo is on THsvc already and caches a local pointer to it
      /// if the histo is not present, it creates and registers it
      TProfile2D* getOrCreateProfile(std::string regName, TString histoname, TString xtitle, int nbinsx, float xmin, float xmax,TString ytitle, int nbinsy,float ymin, float ymax,TString ztitle);

      /// Handle to the histogram service
      ServiceHandle<ITHistSvc> m_hSvc;

      /// Cached eta of the current primary
      double m_etaPrimary;
      /// Cached phi of the current primary
      double m_phiPrimary;
      /// Cached z of the current primary
      double m_zPrimary;
      /// Cached r of the current primary
      double m_rPrimary;

      /// Map of detector thickness measurements for current event
      std::map<std::string, std::pair<double, double> > m_detThick;

      // active sensor hit counter
      int    m_nActiveSensorHits;
      bool   m_PreviousActiveSensorHit;
      // X0 and lambda0 sum counters (per track)
      double m_totalX0;
      double m_totalN0;

      /// Rad-length profile hist in R-Z
      TProfile2D* m_rzProfRL;
      /// Rad-length profile hist in eta
      std::map<std::string, TProfile*> m_etaMapRL;
      /// Rad-length profile hist in phi
      std::map<std::string, TProfile*> m_phiMapRL;
      /// Rad-length profile hist in z
      std::map<std::string, TProfile*> m_zMapRL;
      /// Rad-length profile hist in r
      std::map<std::string, TProfile*> m_rMapRL;

      /// Int-length profile hist in R-Z
      TProfile2D* m_rzProfIL;
      /// Int-length profile hist in eta
      std::map<std::string, TProfile*> m_etaMapIL;
      /// Int-length profile hist in phi
      std::map<std::string, TProfile*> m_phiMapIL;
      /// Int-length profile hist in z
      std::map<std::string, TProfile*> m_zMapIL;
      /// Int-length profile hist in r
      std::map<std::string, TProfile*> m_rMapIL;

      // 2D plots of rad-length and int-length
      std::map<std::string,TProfile2D*,std::less<std::string> > m_rzMapRL;
      std::map<std::string,TProfile2D*,std::less<std::string> > m_xyMapRL;

      std::map<std::string,TProfile2D*,std::less<std::string> > m_rzMapIL;
      std::map<std::string,TProfile2D*,std::less<std::string> > m_xyMapIL;

  }; // class LengthIntegrator

} // namespace G4UA

#endif
