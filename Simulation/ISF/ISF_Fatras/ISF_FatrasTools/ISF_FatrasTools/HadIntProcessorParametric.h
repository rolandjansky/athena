/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HadIntProcessorParametric.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_HadIntProcessorParametric_H
#define ISF_FATRASTOOLS_HadIntProcessorParametric_H

// GaudiKernel & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Fatras
#include "ISF_FatrasInterfaces/IHadronicInteractionProcessor.h"

// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

// ISF
#include "ISF_Event/ITruthIncident.h"
#include "ISF_Event/ISFParticleVector.h"

// Barcode
#include "BarcodeEvent/PhysicsProcessCode.h"

#ifndef MAXHADINTCHILDREN
#define MAXHADINTCHILDREN 20
#endif

class StoreGateSvc;

class TTree;
  
namespace Trk {
  class Material;
  class MaterialProperties;
  class TrackingGeometry;
  class ITrackingGeometrySvc;
}

namespace ISF {
  class IParticleBroker;
  class ITruthSvc;    
}
namespace iFatras {

  class IPhysicsValidationTool;
      
  /** @class HadIntProcessorParametric
  
      Parametric implementation of nuclear interactions to be used
      in Fatras. The parameterisation is gathered from Geant4.

      @author Andreas.Salzburger@cern.ch, Carsten.Magass@cern.ch
      
   */
   
  class HadIntProcessorParametric : public extends<AthAlgTool, iFatras::IHadronicInteractionProcessor> {
    public:      
      /**AlgTool constructor for HadIntProcessorParametric*/
      HadIntProcessorParametric(const std::string&,const std::string&,const IInterface*);
      /**Destructor*/
      virtual ~HadIntProcessorParametric();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();

      /** interface for processing of the nuclear interactions */
      bool hadronicInteraction(const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
			       double p, double E, double charge,
                               const Trk::MaterialProperties& mprop, double pathCorrection,
                               Trk::ParticleHypothesis particle=Trk::pion) const;

      /** interface for processing of the presampled nuclear interaction */                           
      bool recordHadState(double time, double p,
			  const Amg::Vector3D& vertex,
			  const Amg::Vector3D& particleDir,
			  Trk::ParticleHypothesis particle ) const;

      bool doHadronicInteraction(double time, const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
				 const Trk::Material* emat,
				 Trk::ParticleHypothesis particle,
				 bool processSecondaries) const;
 
      ISF::ISFParticleVector doHadIntOnLayer(const ISF::ISFParticle* parent, double time,
					     const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
					     const Trk::Material* emat,
					     Trk::ParticleHypothesis particle) const;
 
   private:
      /** interface for calculation of absorption length */
      double absorptionLength(const Trk::MaterialProperties* mat,
            double p, double q, Trk::ParticleHypothesis particle=Trk::pion) const;

      /** collect secondaries for layer material update */                           
      ISF::ISFParticleVector getHadState(const ISF::ISFParticle* parent,
					 double time, double p,
					 const Amg::Vector3D& vertex,
					 const Amg::Vector3D& particleDir,
					 Trk::ParticleHypothesis particle ) const;
                           
       /** hadronic interaction setting */
       double                       m_minimumHadOutEnergy;
       double                       m_childMomParam;
       bool                         m_cutChain;
       bool                         m_hadIntFromX0;
       double                       m_hadIntProbScale;
       double                       m_minimumHadInitialEnergy;


       /** ISF services & Tools */
       ServiceHandle<ISF::IParticleBroker>  m_particleBroker;
       ServiceHandle<ISF::ITruthSvc>        m_truthRecordSvc;
                  
       /** Random Generator service */
       ServiceHandle<IAtRndmGenSvc>         m_rndGenSvc;
       /** MCTruth process code for TruthIncidents created by this tool */
       Barcode::PhysicsProcessCode          m_processCode;

       /** Random engine  */        
       CLHEP::HepRandomEngine*                     m_randomEngine;
       std::string                          m_randomEngineName; //!< Name of the random number stream

       /** struct of Particle Masses */
       static Trk::ParticleMasses    s_particleMasses;
                    
       bool                          m_validationMode;
       ToolHandle<IPhysicsValidationTool>  m_validationTool;

       bool                          m_hadIntValidation;
       std::string                   m_hadIntValidationTreeName;        //!< validation tree name - to be acessed by this from root
       std::string                   m_hadIntValidationTreeDescription; //!< validation tree description - second argument in TTree
       std::string                   m_hadIntValidationTreeFolder;      //!< stream/folder to for the TTree to be written out
                                       
       TTree*                        m_hadIntValidationTree;            //!< Root Validation Tree
                                       
       // ------ ntuple branches       
       mutable float                 m_hadIntPointX;               //!< ntuple variable : hadronic interaction point x coordinate
       mutable float                 m_hadIntPointY;               //!< ntuple variable : hadronic interaction point y coordinate
       mutable float                 m_hadIntPointR;               //!< ntuple variable : hadronic interaction point r distance
       mutable float                 m_hadIntPointZ;               //!< ntuple variable : hadronic interaction point z coordinate
       mutable int                   m_hadIntMotherPdg;            //!< ntuple variable : hadronic interaction mother Pdg
       mutable int                   m_hadIntMotherBarcode;         //!< ntuple variable : hadronic interaction mother barcode
       mutable float                 m_hadIntMotherP;         //!< ntuple variable : hadronic interaction mother momentum
       mutable float                 m_hadIntMotherPt;         //!< ntuple variable : hadronic interaction mother momentum
       mutable float                 m_hadIntMotherPhi;            //!< ntuple variable : hadronic interaction mother phi
       mutable float                 m_hadIntMotherEta;            //!< ntuple variable : hadronic interaction photon eta

       mutable int                   m_hadIntChildren;             				//!< nutple variable : hadronic interaction children numbers
       mutable float                 m_hadIntChildE;             				//!< nutple variable : hadronic interaction children total energy
       mutable float                 m_hadIntChildP[MAXHADINTCHILDREN];     //!< nutple variable : hadronic interaction child Energy
       mutable float                 m_hadIntChildPcms[MAXHADINTCHILDREN];     //!< nutple variable : hadronic interaction child Energy
       mutable int                   m_hadIntChildPdg[MAXHADINTCHILDREN];        //!< nutple variable : hadronic interaction child Pdg
       mutable float                 m_hadIntChildPhi[MAXHADINTCHILDREN];        //!< nutple variable : hadronic interaction child phi
       mutable float                 m_hadIntChildEta[MAXHADINTCHILDREN];        //!< nutple variable : hadronic interaction child eta
       mutable float                 m_hadIntChildTh[MAXHADINTCHILDREN];        //!< nutple variable : hadronic interaction child phi
       mutable float                 m_hadIntChildThc[MAXHADINTCHILDREN];        //!< nutple variable : hadronic interaction child eta
       mutable float                 m_hadIntChildDeltaPhi[MAXHADINTCHILDREN];   //!< nutple variable : hadronic interaction child delta phi
       mutable float                 m_hadIntChildDeltaEta[MAXHADINTCHILDREN];   //!< nutple variable : hadronic interaction child delta eta

   };
}

#endif // ISF_FATRASTOOLS_HadIntProcessorParametric_H
