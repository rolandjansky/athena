/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossRecorder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef EnergyLossRecorder_H
#define EnergyLossRecorder_H

#include "GaudiKernel/NTuple.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <string>
#include <vector>

class TFile;
class TTree;
class TBranch;

/** @class EnergyLossRecorder

    This class records the Energy Loss and other properties of primary particles
    (e.g. Muons) at the Geant4 step level when passing virtual cylinder surfaces
    (while actually using the full simulation geometry).

    It can be used in combination with the EnergyLossExtrapolationValidation
    package (from TrkExtrapolation/TrkExAlgs) for a comparison and validation
    of the Energy Loss between full sim and reconstruction (Tracking Geometry).

    @author Wolfgang.Lukas@cern.ch
*/

#ifndef TRKG4USERACTIONS_MAXLAYERS
#define TRKG4USERACTIONS_MAXLAYERS 10
#endif


#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/IEndRunAction.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"
namespace G4UA{
  
  class EnergyLossRecorder:
  public AthMessaging, public IBeginRunAction,  public IEndRunAction,  public IBeginEventAction,  public IEndEventAction,  public ISteppingAction
  {
    
  public:
    
    struct Config
    {
      size_t cylinders=7;
      float  minEta=-2.7;
      float  maxEta=2.7;
      float  momentum=10.*CLHEP::GeV;      //!< primary particle momentum
      int    pdg=999;           //!< primary particle PDG code
      bool   usePt=true;         //!< use normal or transverse momentum (p or pT)
      std::string   useX0prop="";         //!< also accumulate tInX0 and record it in the ROOT tree
      std::string   onionModeprop="";     //!< use strictly hierarchical ordering of layers ("onion mode")
      std::string   keepAliveprop="";     //!< keep particle alive when all layers are filled (default = off)
      std::vector<float> Rvec; //!< stores the list from python properties
      std::vector<float> Zvec; //!< stores the list from python properties

    };
    
    EnergyLossRecorder(const Config& config);
    virtual void beginOfRun(const G4Run*) override;
    virtual void endOfRun(const G4Run*) override;
    virtual void beginOfEvent(const G4Event*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void processStep(const G4Step*) override;
  private:
    Config m_config;
    TFile* m_outfile;
    TTree* m_eventTree;
    TTree* m_runTree;
    size_t m_entries;
    int    m_layer[TRKG4USERACTIONS_MAXLAYERS];
    //    float  m_energyIn;
    float  m_energy[TRKG4USERACTIONS_MAXLAYERS];
    float  m_eLoss[TRKG4USERACTIONS_MAXLAYERS];
    float  m_tInX0[TRKG4USERACTIONS_MAXLAYERS];
    float  m_radius[TRKG4USERACTIONS_MAXLAYERS];
    float  m_posX[TRKG4USERACTIONS_MAXLAYERS];
    float  m_posY[TRKG4USERACTIONS_MAXLAYERS];
    float  m_posZ[TRKG4USERACTIONS_MAXLAYERS];
    float  m_eta[TRKG4USERACTIONS_MAXLAYERS];
    float  m_theta[TRKG4USERACTIONS_MAXLAYERS];
    float  m_phi[TRKG4USERACTIONS_MAXLAYERS];
    bool   m_used[TRKG4USERACTIONS_MAXLAYERS];
    float  m_thickX0;
    int    m_verboseLevel;

    float  m_cylinderR[TRKG4USERACTIONS_MAXLAYERS];       //!< radius of cylinder layers
    float  m_cylinderZ[TRKG4USERACTIONS_MAXLAYERS];       //!< half length of cylinder layers
    // for runTree

    bool   m_useX0;         //!< also accumulate tInX0 and record it in the ROOT tree
    bool   m_onionMode;
    bool   m_keepAlive;
    size_t m_startLayer;    //!< for onionMode: start stepping intersection loop from here
    size_t m_noMaterialStep;   //!< count the steps in which no material was found
    size_t m_events;           //!< number of recorded events
    int    m_totalRecordedLayers; //!< total amount of all recorded layers
    float  m_avgRecordedLayers;   //!< average recorded layers per event, should equal m_cylinders

    inline bool TurnOffString(const std::string s, const std::string ss="") {
        return !(s=="0" || s=="off" || s=="OFF" || s=="no" || s=="NO" || s=="none" || s=="NONE" || s==ss);
    }

  }; // class EnergyLossRecorder
  
  
} // namespace G4UA 


#endif

