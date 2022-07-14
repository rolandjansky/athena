/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossValidation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXALGS_ENERGYLOSSEXTRAPOLATIONVALIDATION_H
#define TRKEXALGS_ENERGYLOSSEXTRAPOLATIONVALIDATION_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include <string>
#include "AthContainers/DataVector.h"
// CylinderSurfaces
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
//
#include <memory>
class TTree;

#ifndef TRKEXALGS_MAXPARAMETERS
#define TRKEXALGS_MAXPARAMETERS 10
#endif


namespace Trk 
{

  class IExtrapolator;
  class Surface;
  class TrackingGeometry;
  class TrackingVolume;

  /** @class EnergyLossExtrapolationValidation

     The EnergyLossExtrapolationValidation Algorithm runs a number of n test
     extrapolations from randomly distributed Track Parameters to specific
     reference surfaces within the ATLAS Detector (user defined cylinder tubes
     which can be placed e.g. around Beam Pipe, Pixel, SCT, TRT, Calorimeter,
     Muon System, or have completely arbitrary dimensions).

     In the jobOptions one can toggle these surfaces for

     - a) the Inner Detector via DetFlags.ID_setOn()   / DetFlags.ID_setOff()
     - b) the Calorimeter    via DetFlags.Calo_setOn() / DetFlags.Calo_setOff()
     - c) the Muon System    via DetFlags.Muon_setOn() / DetFlags.Calo_setOff()

	 The code of ExtrapolationValidation (by Andreas Salzburger) has been
	 reused and adjusted for this algorithm.

      @author  Wolfgang Lukas     <Wolfgang.Lukas -at- cern.ch>
  */  

  class EnergyLossExtrapolationValidation : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       EnergyLossExtrapolationValidation(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~EnergyLossExtrapolationValidation();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
      /** private helper method to create a Transform */
      static std::unique_ptr<Amg::Transform3D> createTransform(double x,
                                                        double y,
                                                        double z,
                                                        double phi = 0.,
                                                        double theta = 0.,
                                                        double alphaZ = 0.);

      /** the highest volume */
      const TrackingVolume*     m_highestVolume;

      /** The Extrapolator to be retrieved */
      ToolHandle<IExtrapolator> m_extrapolator;

      /** Random Number setup */
      Rndm::Numbers*            m_gaussDist;
      Rndm::Numbers*            m_flatDist;

      bool                      m_materialCollectionValidation; //!< use the material collection (extrapolateM)

      TTree*                    m_validationTree;            //!< Root Validation Tree
      TTree*                    m_validationRunTree;         //!< Root Run Stats Tree

      std::string               m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out
      std::string               m_validationTreeName;        //!< validation tree name - to be accessed by this from root
      std::string               m_validationTreeDescription; //!< validation tree description - second argument in TTree
      std::string               m_validationRunTreeFolder;      //!< stream/folder to for the second TTree to be written out
      std::string               m_validationRunTreeName;        //!< run stats tree name - to be accessed by this from root
      std::string               m_validationRunTreeDescription; //!< run stats tree description - second argument in TTree

      double                    m_maximumR;                  //!< maximum R of the highest
      double                    m_maximumZ;                  //!< maximum halfZ of the highest tracking volume


      size_t                    m_cylinders;      //!< number of cylinder layers
      bool                      m_onion;          //!< strictly hierarchical ordering (onion-like)
      float                     m_momentum;       //!< momentum value
      bool                      m_usePt;          //!< use pt instead of p
      float                     m_minEta;         //!< Minimal eta value
      float                     m_maxEta;         //!< Maximal eta value

      size_t                    m_events;         //!< total number of recorded events
      int                       m_totalRecordedLayers; //!< total number of recorded layers
      float                     m_avgRecordedLayers;   //!< average recorded layers per event

      int                       m_pdg;            //!< PDG code corresponding to m_particleType
      int                       m_particleType;   //!< the particle type for the extrapolation

      // ---- variables for the ROOT event tree
      size_t            m_entries;         //!< effective number of used entries (recorded layers) in this event
      float             m_energy[TRKEXALGS_MAXPARAMETERS];           //!< energy
      float             m_energyLoss[TRKEXALGS_MAXPARAMETERS];       //!< energy loss
      float             m_parameterX0[TRKEXALGS_MAXPARAMETERS];      //!< thickness in X0
      float             m_radius[TRKEXALGS_MAXPARAMETERS];           //!< position radius
//      float             m_parameterLoc1[TRKEXALGS_MAXPARAMETERS];    //!< local 1
//      float             m_parameterLoc2[TRKEXALGS_MAXPARAMETERS];    //!< local 2
      float             m_positionX[TRKEXALGS_MAXPARAMETERS];        //!< position X
      float             m_positionY[TRKEXALGS_MAXPARAMETERS];        //!< position Y
      float             m_positionZ[TRKEXALGS_MAXPARAMETERS];        //!< position Z
      float             m_parameterPhi[TRKEXALGS_MAXPARAMETERS];     //!< phi
      float             m_parameterEta[TRKEXALGS_MAXPARAMETERS];     //!< eta
      float             m_parameterTheta[TRKEXALGS_MAXPARAMETERS];   //!< theta
      float             m_parameterQoverP[TRKEXALGS_MAXPARAMETERS];  //!< qOverP
      float             m_parameterP[TRKEXALGS_MAXPARAMETERS];       //!< P
      size_t            m_layer[TRKEXALGS_MAXPARAMETERS];            //!< layer id

      // ---- output statistics
      size_t            m_triesForward;      //!< extrapolation events forward
      size_t            m_breaksForward;     //!< extrapolation breaks forward
      size_t            m_triesBack;         //!< extrapolation events backward
      size_t            m_breaksBack;        //!< extrapolation breaks backward

      size_t            m_collectedLayerForward;   //!< collected material layers forward
      size_t            m_collectedLayerBack;    //!< collected material layers backward


      // ---- cylinders and discs
      float                     m_cylinderR[TRKEXALGS_MAXPARAMETERS];   //!< radius of cylinder layers (for ROOT tree)
      float                     m_cylinderZ[TRKEXALGS_MAXPARAMETERS];   //!< length of cylinder layers (for ROOT tree)
      DataVector<const Trk::CylinderSurface>* m_theCylinders;
      DataVector<const Trk::DiscSurface>* m_theDiscs1;
      DataVector<const Trk::DiscSurface>* m_theDiscs2;
      std::vector<float>        m_cylinderVR;       //!< radius of cylinder layers (vector from jobOptions)
      std::vector<float>        m_cylinderVZ;       //!< length of cylinder layers (vector from jobOptions)
    }; 
} // end of namespace

#endif
