/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// G4HadIntProcessor.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////// 
#ifndef ISF_FATRASTOOLSG4_G4HADINTPROCESSOR_H
#define ISF_FATRASTOOLSG4_G4HADINTPROCESSOR_H

// GaudiKernel & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Fatras
#include "ISF_FatrasInterfaces/IHadronicInteractionProcessor.h"

//Barcode
#include "BarcodeEvent/PhysicsProcessCode.h"

// Geant4
#include "G4ThreeVector.hh"

// Trk
#include "TrkEventPrimitives/ParticleHypothesis.h"

// ISF
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticleVector.h"

// Forward Declarations
class TTree;
class G4DynamicParticle;
class G4VProcess;
class G4RunManager;
class G4VUserPhysicsList;
class G4LayerDetectorConstruction;
class G4LayerPrimaryGeneratorAction;
class G4LayerTrackingAction;
class G4Step;
class G4StepPoint;
class G4AtlasRunManager;
class G4Material;
class G4MaterialCutsCouple;

namespace Trk {
  class Material;
  class MaterialProperties;       // TODO: get rid of MatProp dependence
}

namespace ISF {
  class IParticleBroker;
  class ITruthSvc;
  class IG4RunManagerHelper;
}

namespace iFatras {

  class IPhysicsValidationTool;

  /** @class G4HadIntProcessor

    Wrapper class for multiple scattering, energyloss, hadronic interaction
    tool from Geant4.

    @author Andreas.Salzburger@cern.ch
    */

  class G4HadIntProcessor : public extends<AthAlgTool, iFatras::IHadronicInteractionProcessor> {
    public:      
      /** AlgTool constructor for G4HadIntProcessor*/
      G4HadIntProcessor(const std::string&,const std::string&,const IInterface*);
      /**Destructor*/
      virtual ~G4HadIntProcessor();

      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();

      /** interface for processing of the nuclear interactions */
      bool hadronicInteraction(const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
			       double p, double E, double charge, 
                               const Trk::MaterialProperties& mprop, double pathCorrection,
                               Trk::ParticleHypothesis particle=Trk::pion) const;

      bool doHadronicInteraction(double time, const Amg::Vector3D& position, const Amg::Vector3D& momentum,
                                 const Trk::Material *ematprop,
				 Trk::ParticleHypothesis particle=Trk::pion,
                                 bool  processSecondaries=true) const;

      /** interface for processing of the presampled nuclear interactions on layer*/
      ISF::ISFParticleVector doHadIntOnLayer(const ISF::ISFParticle* parent, double time, 
					     const Amg::Vector3D& position, const Amg::Vector3D& momentum,
					     const Trk::Material *ematprop,
					     Trk::ParticleHypothesis particle=Trk::pion) const ;

    private:
      /** initialize G4RunManager on first call if not done by then */
      bool initG4RunManager() const;

      /** collect secondaries for layer material update */                           
      ISF::ISFParticleVector getHadState(const ISF::ISFParticle* parent,
					 double time, const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
					 const Trk::Material *ematprop) const;

      //!< Initialize inleastic hadronic Geant4 processes 
      std::map<int,G4VProcess*>::iterator  initProcessPDG(int pdg) const;

      //!< choose for list of predefined (pure) materials
      std::pair<G4Material*,G4MaterialCutsCouple*> retrieveG4Material(const Trk::Material* ematprop) const;

      //!< random number service
      ServiceHandle<IAtRndmGenSvc>         m_rndGenSvc;

      ToolHandle<ISF::IG4RunManagerHelper> m_g4RunManagerHelper;

      //!< steering: enable elastic interactions?
      bool                                 m_doElastic;

      /* scale factors for hadronic/electromagnetic interactions */
      double                               m_hadIntProbScale;

      // internal steering : clone type
      double                               m_minMomentum;
      mutable bool                         m_cloneParameters;

      /** describe deflection parametric/do real deflection */
      bool                                  m_parametricScattering;

      //!< Geant4 engine
      mutable G4RunManager*                m_g4runManager;
      G4VUserPhysicsList*                  m_g4physicsList;
      G4LayerDetectorConstruction*         m_g4detector;

      //!< Geant4 processes <PDGcode, process>  TODO : fission, capture
      mutable std::map<int, G4VProcess*>   m_g4HadrInelasticProcesses;
      mutable std::map<int, G4VProcess*>   m_g4HadrElasticProcesses;

      //!< locally stored Geant4 instances (speeds up processing)
      mutable G4DynamicParticle*           m_g4dynPar;
      mutable const G4ThreeVector*         m_g4zeroPos;
      mutable G4Step*                      m_g4step;
      mutable G4StepPoint*                 m_g4stepPoint;
      mutable std::vector<std::pair<float,std::pair< G4Material*, G4MaterialCutsCouple*> > > m_g4Material;

      /** ISF services & Tools */
      ServiceHandle<ISF::IParticleBroker>  m_particleBroker;
      ServiceHandle<ISF::ITruthSvc>        m_truthRecordSvc;

      /** MCTruth process code for TruthIncidents created by this tool */
      Barcode::PhysicsProcessCode          m_processCode;

      /** Random engine  */
      CLHEP::HepRandomEngine*              m_randomEngine;
      std::string                          m_randomEngineName;       //!< Name of the random number stream

      /** projection factor for the non-parametric scattering */
      static double                        s_projectionFactor;

      // ------------------------ Validation section ------------------------------------
      bool                          m_validationMode;
      ToolHandle<IPhysicsValidationTool>  m_validationTool;
      std::string                   m_validationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string                   m_validationTreeDescription; //!< validation tree description - second argument in TTree
      std::string                   m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out

      TTree*                        m_validationTree;            //!< Root Validation Tree

      // ----------- ntuple branches
      mutable int                  m_layerIndex;                //!< ntuple variable : layer index of material effects update
      mutable float                m_tInX0;                     //!< nutple variable : t/X0
      mutable float                m_thetaMSproj;               //!< ntuple variable : projected ms
      mutable float                m_thetaMSphi;                //!< ntuple variable : ms in phi
      mutable float                m_thetaMStheta;              //!< ntuple variable : ms in theta
      mutable float                m_deltaP;                    //!< nutple variable : energy loss
      mutable float                m_deltaPsigma;               //!< ntuple variable : stragling on energy loss


      bool                          m_bremValidation;
      std::string                   m_bremValidationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string                   m_bremValidationTreeDescription; //!< validation tree description - second argument in TTree
      std::string                   m_bremValidationTreeFolder;      //!< stream/folder to for the TTree to be written out

      TTree*                        m_bremValidationTree;            //!< Root Validation Tree

      // ------ ntuple branches
      mutable float                 m_bremPointX;               //!< ntuple variable : brem point x coordinate
      mutable float                 m_bremPointY;               //!< ntuple variable : brem point y coordinate
      mutable float                 m_bremPointR;               //!< ntuple variable : brem point r distance
      mutable float                 m_bremPointZ;               //!< ntuple variable : brem point z coordinate
      mutable float                 m_bremMotherEnergy;         //!< ntuple variable : brem mother energy
      mutable float                 m_bremPhotonEnergy;         //!< ntuple variable : brem photon energy
      mutable float                 m_bremPhotonAngle;          //!< ntuple variable : brem photon angle

      // --------------------------------------------------------------------------------

      bool                          m_edValidation;
      std::string                   m_edValidationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string                   m_edValidationTreeDescription; //!< validation tree description - second argument in TTree
      std::string                   m_edValidationTreeFolder;      //!< stream/folder to for the TTree to be written out

      TTree*                        m_edValidationTree;           //!< Root Validation Tree

      // ------ ntuple branches
      mutable float                 m_edLayerIntersectX;          //!< ntuple variable : energy deposit x coordinate
      mutable float                 m_edLayerIntersectY;          //!< ntuple variable : energy deposit y coordinate
      mutable float                 m_edLayerIntersectZ;          //!< ntuple variable : energy deposit z coordinate
      mutable float                 m_edLayerIntersectR;          //!< ntuple variable : energy deposit r coordinate
      mutable float                 m_edLayerEnergyDeposit;       //!< ntuple variable : energy despoit - value
      mutable float                 m_edLayerSample;              //!< ntuple variable : layer sample
  };

}

#endif
