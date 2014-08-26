/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossRecorder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef EnergyLossRecorder_H
#define EnergyLossRecorder_H

#include "FadsActions/UserAction.h"
#include "GaudiKernel/NTuple.h"

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


class EnergyLossRecorder: public FADS::UserAction {

  public:
    /** Standard FADS UserAction */
    EnergyLossRecorder(std::string s);

    /** All G4 interface methods */
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void SteppingAction(const G4Step*);
    void ParseProperties();

  private:

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
    size_t m_cylinders;
    float  m_cylinderR[TRKG4USERACTIONS_MAXLAYERS];       //!< radius of cylinder layers
    float  m_cylinderZ[TRKG4USERACTIONS_MAXLAYERS];       //!< half length of cylinder layers
    // for runTree
    float  m_minEta;
    float  m_maxEta;
    float  m_momentum;      //!< primary particle momentum
    int    m_pdg;           //!< primary particle PDG code
    bool   m_usePt;         //!< use normal or transverse momentum (p or pT)
    // steering options
    bool   m_useX0;         //!< also accumulate tInX0 and record it in the ROOT tree
    bool   m_onionMode;     //!< use strictly hierarchical ordering of layers ("onion mode")
    bool   m_keepAlive;     //!< keep particle alive when all layers are filled (default = off)
    size_t m_startLayer;    //!< for onionMode: start stepping intersection loop from here

    size_t m_noMaterialStep;   //!< count the steps in which no material was found
    size_t m_events;           //!< number of recorded events
    int    m_totalRecordedLayers; //!< total amount of all recorded layers
    float  m_avgRecordedLayers;   //!< average recorded layers per event, should equal m_cylinders

    inline bool TurnOffString(const std::string s, const std::string ss="") {
        return !(s=="0" || s=="off" || s=="OFF" || s=="no" || s=="NO" || s=="none" || s=="NONE" || s==ss);
    }

};

#endif

