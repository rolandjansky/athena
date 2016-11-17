/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PhotonConversionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_PHOTONCONVERSIONTOOL_H
#define ISF_FATRASTOOLS_PHOTONCONVERSIONTOOL_H

// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkDetDescrUtils/GeometrySignature.h" 
// ISF
#include "ISF_Event/ITruthIncident.h"
// Fatras
#include "ISF_FatrasInterfaces/IPhotonConversionTool.h"
// Barcode
#include "BarcodeEvent/PhysicsProcessCode.h"

class TTree;

class StoreGateSvc;

namespace Trk {
  class Layer;
  class CylinderVolumeBounds;
  class PdgToParticleHypothesis;
  class TrackingGeometry;
  class ITrackingGeometrySvc;
}

namespace ISF {
  class IParticleBroker;
  class ITruthSvc;
}

namespace iFatras {

  class IPhysicsValidationTool;
  
  /** @class PhotonConversionTool
      
     The photon conversion tool, to be called by the MaterialUpdator.
  
     @author Sarka.Todorova@cern.ch
  */
   
  class PhotonConversionTool : public AthAlgTool,
                               virtual public iFatras::IPhotonConversionTool {
  public:      
      /**AlgTool constructor for PhotonConversionTool*/
      PhotonConversionTool(const std::string&,const std::string&,const IInterface*);
      /**Destructor*/
      virtual ~PhotonConversionTool();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();

    
      /** interface for processing of the pair production */
      bool pairProduction(const Trk::MaterialProperties& mprop,
			  double pathCorrection,
			  double p) const;
      
      /** interface for processing of the presampled pair production */
      bool doConversion(double time, const Trk::NeutralParameters& parm,
			const Trk::ExtendedMaterialProperties* extMatProp=0) const;
      
      /** interface for processing of the presampled nuclear interactions on layer*/
      ISF::ISFParticleVector doConversionOnLayer(const ISF::ISFParticle* parent, 
						 double time, const Trk::NeutralParameters& parm,
						 const Trk::ExtendedMaterialProperties *ematprop=0) const;


    
   private:
      /** record childs - create interface already for mu-/mu+ pair production*/
      void recordChilds(double time, 
			const Amg::Vector3D& vertex,
			const Amg::Vector3D& photonDirection,
			double childEnergy, double photonEnergy,
			const Amg::Vector3D& childDirection,
			Trk::ParticleHypothesis childType=Trk::electron) const;
      
      /** get childs - for layer update */
      ISF::ISFParticleVector getChilds(const ISF::ISFParticle* parent,
				       double time,
				       const Amg::Vector3D& vertex,
				       const Amg::Vector3D& photonDirection,
				       double childEnergy, double photonEnergy,
				       const Amg::Vector3D& childDirection,
				       Trk::ParticleHypothesis childType) const;
	
      /** simulate the child energy */
      double childEnergyFraction(const Trk::MaterialProperties& mprop, double gammaMom) const;
      double childEnergyFraction(double gammaMom) const;
      
      /** simulate the one child direction - the second one is given clear then*/
      Amg::Vector3D childDirection(const Amg::Vector3D& gammaMom,
					  double childE) const;
      
      /** helper functions for the Phi1/phi2 */
      double phi1(double delta) const;
      
      /** helper functions for the Phi1/phi2 */
      double phi2(double delta) const;

      /** ISF services & Tools */
      ServiceHandle<ISF::IParticleBroker>          m_particleBroker;
      ServiceHandle<ISF::ITruthSvc>                m_truthRecordSvc;
      
      /** MCTruth process code for TruthIncidents created by this tool */
      Barcode::PhysicsProcessCode                  m_processCode;
      
      /** Switch to use reference material */
      bool                                         m_referenceMaterial;
      
      /** private member to pass geometry signature */
      mutable Trk::GeometrySignature               m_geomSignature;
      
      /** The cut from which on the child products are followed */
      double                                       m_minChildEnergy;
      double                                       m_childEnergyScaleFactor;
      double                                       m_conversionProbScaleFactor;
      
      
      /** Random Generator service  */
      ServiceHandle<IAtRndmGenSvc>                 m_rndGenSvc;
      CLHEP::HepRandomEngine*                             m_randomEngine;
      std::string                                  m_randomEngineName;         //!< Name of the random number stream
      
      /** debug output - count the conversions */
      mutable int                                  m_recordedConversions;
      mutable int                                  m_droppedConversions;
      
      // ------------------------ Validation section ------------------------------------
      bool                                         m_validationMode;
      std::string                                  m_validationTreeName;        //!< validation tree name - to be acessed by this from root
      std::string                                  m_validationTreeDescription; //!< validation tree description - second argument in TTree
      std::string                                  m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out
      
      TTree*                                       m_validationTree;            //!< Root Validation Tree
      ToolHandle<IPhysicsValidationTool>           m_validationTool;
      
      // ------ ntuple branches
      mutable float                                m_conversionPointX;
      mutable float                                m_conversionPointY;
      mutable float                                m_conversionPointR;
      mutable float                                m_conversionPointZ;
      mutable float                                m_conversionPhotonEnergy;
      mutable float                                m_conversionChildEnergy;
      mutable float                                m_conversionChildAngle;
      // --------------------------------------------------------------------------------
      
      /** struct of Particle Masses */
      static Trk::ParticleMasses                   s_particleMasses;
      static Trk::PdgToParticleHypothesis          s_pdgToHypo;
      /** Inverse fine structure constant */
      static double                                s_alpha;
      static double                                s_oneOverThree;
   };
  

inline double PhotonConversionTool::phi1(double delta) const {
  if (delta <= 1.)
     return 20.867 - 3.242 * delta  + 0.625*delta*delta;
  else
    return 21.12 - 4.184*log(delta+0.952);
}

inline double PhotonConversionTool::phi2(double delta) const {
  if (delta <= 1.)
     return 20.209 - 1.930 * delta  - 0.086*delta*delta;
   return 21.12 - 4.184*log(delta+0.952);
}

}

#endif
