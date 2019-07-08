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

      struct Config
      {
        bool isITk=false;
      };

      /// Constructor takes the name of the histogram service as argument.
      LengthIntegrator(const std::string& histSvcName, const Config& config);

      /// Called at beginning of G4 event to cache some details about the
      /// current primary vertex and particle. Also resets some measurements.
      virtual void BeginOfEventAction(const G4Event*) override;

      /// Called at end of G4 event to finalize measurements and fill hists
      virtual void EndOfEventAction(const G4Event*) override;

      /// Called at every particle step to accumulate thickness.
      virtual void UserSteppingAction(const G4Step*) override;

    private:

      std::string getVolumeType(std::string s);
      std::string getLayerName(double r, double z);
      void fillNtuple();
      std::string getMaterialClassification(std::string name);


      // Holder for G4 math tools
      G4Pow* m_g4pow;
      ServiceHandle<ITHistSvc> m_hSvc;
      ServiceHandle< IGeoModelSvc > m_geoModelSvc;
      TTree* m_tree;
      bool m_doElements;
      bool m_isITk;
      Config m_config;
      /// Handle to the histogram service
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

      std::vector<std::vector<double>> m_collected_material_element_X0;
      std::vector<std::vector<double>> m_collected_material_element_L0;
      std::vector<std::vector<std::string>> m_collected_material_elements;

  }; // class LengthIntegrator

} // namespace G4UA

#endif
