/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_ScoringPlane_H
#define G4UserActions_ScoringPlane_H

#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/IEndRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"

#include <string>

// Forward declarations
class TTree;

namespace G4UA
{

  /// @class ScoringPlane
  /// @todo TODO NEEDS DOCUMENTATION
  class ScoringPlane : public IBeginRunAction, public IEndRunAction,
                       public ISteppingAction, public IBeginEventAction,
                       public IEndEventAction
  {

    public:

      struct Config
      {
        double      plane=22600;
        int         pkill=0;
        std::string fname="ufo.root";
      };

      ScoringPlane(const Config& config);
      virtual void beginOfRun(const G4Run*) override;
      virtual void endOfRun(const G4Run*) override;
      virtual void processStep(const G4Step*) override;
      virtual void beginOfEvent(const G4Event*) override;
      virtual void endOfEvent(const G4Event*) override;
    private:
      Config m_config;

      TTree* m_tree0;
      TTree* m_tree1;

      int    m_evt;
      int    m_ntr;

      int    m_pdg;
      double m_cha;
      double m_ene;
      double m_vx;
      double m_vy;
      double m_vz;
      double m_x0;
      double m_y0;
      double m_z0;
      double m_t0;
      double m_px0;
      double m_py0;
      double m_pz0;
      double m_x1;
      double m_y1;
      double m_z1;
      double m_t1;
      double m_px1;
      double m_py1;
      double m_pz1;
      double m_x;
      double m_y;
      double m_z;

  }; // class ScoringPlane

} // namespace G4UA

#endif
