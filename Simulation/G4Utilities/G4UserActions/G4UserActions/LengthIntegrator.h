/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_LengthIntegrator_H
#define G4UserActions_LengthIntegrator_H

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "G4Pow.hh"
#include "TString.h"

#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"

#include <string>
#include <map>
#include "GeoModelInterfaces/IGeoModelSvc.h"

// Forward declarations
class TProfile;
class TProfile2D;
class TTree;

//class IGeoModelSvc;

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
  class LengthIntegrator final : public G4UserEventAction,
                                 public G4UserSteppingAction
  {

    public:

      /// Constructor takes the name of the histogram service as argument.
      LengthIntegrator(const std::string& histSvcName);

      /// Called at beginning of G4 event to cache some details about the
      /// current primary vertex and particle. Also resets some measurements.
      virtual void BeginOfEventAction(const G4Event*) override;

      /// Called at end of G4 event to finalize measurements and fill hists
      virtual void EndOfEventAction(const G4Event*) override;

      /// Called at every particle step to accumulate thickness.
      virtual void UserSteppingAction(const G4Step*) override;
      void UserSteppingActionOld(const G4Step*);

      std::string getVolumeType(std::string s);
      std::string getLayerName(double r, double z);
    private:

      // Holder for G4 math tools
      G4Pow* m_g4pow;

      TTree* m_tree;

      bool m_doRL;
      bool m_doIL;
      bool m_doRZ;
      bool m_doXY;
      bool m_doEl;
      bool m_doDensity;
      bool m_doEta;
      bool m_doPhi;
      bool m_doZ;
      bool m_doR;

      std::string m_lengthTypes;
      std::string m_histoTypes;
      // ITk Step number
      float m_step;

      bool m_splitModerator;
      bool m_splitPP1;
      bool m_splitLayers;

      double m_rz_zmin;
      double m_rz_zmax;
      double m_rz_rmin;
      double m_rz_rmax;

      // Add elements and values into the map
      void addToDetThickMap(std::string, double, double);

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
      std::map<std::string, std::pair<double, double> > m_detThickMap;

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

      std::map<std::string, TProfile*> m_profMap;

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

      std::map<std::string,TProfile2D*,std::less<std::string> > m_rzMapDensity;

      //Tree Branches
      int   m_genNPart;
      float m_genEta;
      float m_genPhi;
      float m_genZ;
      float m_genR;
      
      //X0 Branches
      float m_total_X0;
      float m_total_L0;

      std::vector<double> m_collected_X0;
      std::vector<double> m_collected_L0;

      std::vector<float> m_collected_inhitr;
      std::vector<float> m_collected_inhitz;

      std::vector<float> m_collected_outhitr;
      std::vector<float> m_collected_outhitz;

      std::vector<float> m_collected_density;
      std::vector<std::string> m_collected_material;
      std::vector<std::string> m_collected_volume;
      
      std::vector<std::string> m_collected_groupedmaterial;
      std::vector<std::string> m_collected_volumetype;

      ServiceHandle< IGeoModelSvc > m_geoModelSvc;
      bool m_isITk;

      void fillNtuple();
      std::string getMaterialClassification(std::string name);
  }; // class LengthIntegrator

} // namespace G4UA

#endif
