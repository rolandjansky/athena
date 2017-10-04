/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// Liquid Argon H8 Combined TestBeam Geometry 
// -----------------------------------------
//
// 25.10.2004 Creation of the class by claire.bourdarios@cern.ch
//
// ***************************************************************************

#ifndef TRACKTOCALO_TBEXTRAPOLTRACKTOCALO_H
#define TRACKTOCALO_TBEXTRAPOLTRACKTOCALO_H

//#include "ITrackToCalo/IExtrapolTrackToCaloTool.h"
//#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/NeutralParticle.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

class CaloDetDescrManager;
class ICaloCoordinateTool;
class IMessageSvc;
class CaloDepthTool;
class ICaloSurfaceBuilder;
class ImpactInCalo;

//#include "CLHEP/Geometry/Transform3D.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkDetDescrUtils/Intersection.h"

namespace Trk {
  class Track;
  class IExtrapolator;
  class TrackParticleBase;
}

static const InterfaceID IID_TBExtrapolTrackToCaloTool("TBExtrapolTrackToCaloTool", 1 , 0); 

//<<<<<< INCLUDES >>>>>>

/** This class takes a given TrkTrack, extrapolates it to the Calo
    and returns the variables needed to compare with the cluster
    position.

    It heavily uses the Tracking PreconfiguredExtrapolator class. 
 
    The client is asked to select explicitely the CaloCell_ID::CaloSample where he wants to extrapolate.
    The 4 steps are combined :
       - first guess of eta : take it from last point measured for Trk::Track or the imput Trk::Parameters
       - CreateUserSurface(sample,offset,trketa)
       - extrapolate
       - compute calo variables ( neeed for H8, for Atlas correspond to the usual eta/phi )

    The extrapolarion is performed WITH boundary checks. 

   INTERFACE :

   Calorimeter : the enum  CaloCell_ID::CaloSample is strongly recommended, the other one
                  being here for backward compatibility.

   Trk::Trk  is recommended while running the full reconstruction or on ESD. If so, the 
             extrapolation starts from the last measured point.

   Trk::Parameter  is mandatory while running on AOD's and can be used if you want to start
                   the extrapolation from the perigee.

   BEHIND THE SCEENE : the tools of the package CaloTrackingGeometry/CaloSurfaceBuilder are used to
                       create the proper geometry elements ( Trk::Surface, Trk::Layer, Trk::Volume )

 */

//class TBExtrapolTrackToCaloTool : public AthAlgTool, 
//	     virtual public IExtrapolateToCaloTool
class TBExtrapolTrackToCaloTool : public AthAlgTool
{
public:
  
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_TBExtrapolTrackToCaloTool; } 
  // Constructor:

  TBExtrapolTrackToCaloTool(const std::string& type, 
		  const std::string& name, 
		  const IInterface* parent); 

  ~TBExtrapolTrackToCaloTool();

  StatusCode initialize();
  StatusCode finalize();


  /** now return the variables needed to compare with CaloClusters: */
  void CaloVariables (const Trk::TrackParameters* parm, 
		       double& etaCaloLocal, double& phiCaloLocal);
  void CaloLocalPoint (const Trk::TrackParameters* parm,
		       Amg::Vector3D* pt_ctb,
                       Amg::Vector3D* pt_local);


  /** The "do-it-all" method which combines the 3 steps */
  bool TrackSeenByCalo (const Trk::Track* trk, 
			const CaloCell_ID::CaloSample sample,
			const double offset, 
			Amg::Vector3D* pt_ctb,
                        Amg::Vector3D* pt_local);

  bool TrackSeenByCalo (const Trk::TrackParameters* parm, 
			const CaloCell_ID::CaloSample sample,
			const double offset, 
			Amg::Vector3D* pt_ctb,
                        Amg::Vector3D* pt_local);


  bool TrackSeenByCalo (const Trk::Track* trk, 
			const bool barrel,
			const CaloCell_ID::SUBCALO subcalo, 
			const int sampling_or_module, 
			const double offset, 
			Amg::Vector3D* pt_ctb,
                        Amg::Vector3D* pt_local);

  bool TrackSeenByCalo (const Trk::TrackParameters* parm, 
			const bool barrel,
			const CaloCell_ID::SUBCALO subcalo, 
			const int sampling_or_module, 
			const double offset, 
			Amg::Vector3D* pt_ctb,
                        Amg::Vector3D* pt_local);

  bool TrackSeenByCalo (const Trk::Track* trk, 
			const CaloCell_ID::CaloSample sample,
			const double offset, 
			Amg::Vector3D* pt_ctb,
                        Amg::Vector3D* pt_local,
			double& trketa_atcalo, double& trkphi_atcalo);

  /* new methods in base class, not implemented yet */
  const Trk::TrackParameters* extrapolate (const Trk::Track& /*trk*/,
    const CaloCell_ID::CaloSample /*sample*/,
    const double /*offset*/,
    const Trk::PropDirection /*dir*/=Trk::alongMomentum,
    const Trk::ParticleHypothesis /*particle*/=Trk::undefined) const{std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}  

  const Trk::TrackParameters* extrapolate (const Trk::TrackParticleBase& /*trk*/,
    const CaloCell_ID::CaloSample /*sample*/,
    const double /*offset*/,
    const Trk::PropDirection /*dir*/=Trk::alongMomentum,
    const Trk::ParticleHypothesis /*particle*/=Trk::undefined) const {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  const Trk::TrackParameters* extrapolate (const Trk::TrackParameters& /*trkPar*/,
    const CaloCell_ID::CaloSample /*sample*/,
    const double /*offset*/,
    Trk::ParticleHypothesis /*particle*/,
    const Trk::PropDirection /*dir*/=Trk::alongMomentum) const  {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  const Trk::NeutralParameters* extrapolate (const Trk::NeutralParameters& /*trkPar*/,
    const CaloCell_ID::CaloSample /*sample*/,
    const double /*offset*/,
    Trk::ParticleHypothesis /*particle*/,
    const Trk::PropDirection /*dir*/=Trk::alongMomentum) const  {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

    virtual const Trk::TrackParameters* extrapolate (const  xAOD::TrackParticle& /*trk*/,
            const CaloCell_ID::CaloSample /*sample*/,
            const double /*offset*/,
            const Trk::PropDirection /*dir*/=Trk::alongMomentum,
            const Trk::ParticleHypothesis /*particle*/=Trk::undefined) const
      {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

    /** get Trk::NeutralParameters at the calorimeter sample from xAOD::NeutralParticle */
    virtual const Trk::NeutralParameters* extrapolate (const xAOD::NeutralParticle& /*trk*/,
            const CaloCell_ID::CaloSample /*sample*/,
            const double /*offset*/,
            const Trk::PropDirection /*dir*/=Trk::alongMomentum ) const
    {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const Trk::TrackParameters& /*parameters*/,
    Trk::ParticleHypothesis /*particle*/,
    const Trk::PropDirection /*dir*/=Trk::alongMomentum) const  {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  virtual const DataVector< const Trk::NeutralParameters >* getParametersInCalo (const Trk::NeutralParameters& /*parameters*/,
    Trk::ParticleHypothesis /*particle*/,
    const Trk::PropDirection /*dir*/=Trk::alongMomentum) const  {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const Trk::TrackParticleBase& /*trk*/,
    const Trk::ParticleHypothesis /*particle*/=Trk::undefined,
    const Trk::PropDirection /*dir*/=Trk::alongMomentum) const  {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const Trk::Track& /*trk*/,
    const Trk::ParticleHypothesis /*particle*/=Trk::undefined,
    const Trk::PropDirection /*dir*/=Trk::alongMomentum) const  {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  virtual const DataVector< const Trk::TrackParameters >* getParametersInCalo (const xAOD::TrackParticle& /*trk*/,
               const Trk::ParticleHypothesis /*particle*/=Trk::undefined,
                                const Trk::PropDirection /*dir*/=Trk::alongMomentum) const {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}


  ImpactInCalo* getImpactInCalo ( const Trk::Track& /*trk*/,
                                  const Trk::ParticleHypothesis /*particle*/=Trk::undefined,
                                  const Trk::PropDirection /*dir*/=Trk::alongMomentum) const  {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  ImpactInCalo* getImpactInCalo ( const Trk::TrackParticleBase& /*trk*/,
                                  const Trk::ParticleHypothesis /*particle*/=Trk::undefined,
                                  const Trk::PropDirection /*dir*/=Trk::alongMomentum) const  {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  virtual ImpactInCalo* getImpactInCalo (const Trk::TrackParameters& /*parameters*/,
            Trk::ParticleHypothesis /*particleHypo*/,
            const Trk::PropDirection /*dir*/=Trk::alongMomentum) const {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  virtual ImpactInCalo* getImpactInCalo (const Trk::NeutralParameters& /*parameters*/,
            Trk::ParticleHypothesis /*particleHypo*/,
            const Trk::PropDirection /*dir*/=Trk::alongMomentum) const {std::cout<<"Not Implemented !!!!"<<std::endl; return 0;}

  /** access to the private tool used to define the extrapolation depth, needed to play with delta-eta */
  CaloDepthTool* getCaloDepth();


  /** to be used on AOD's or to extrapolate from perigee */
  const Trk::TrackParameters* extrapolate(const Trk::TrackParameters* parm,
					  const Trk::Surface* surf);

  /** will use the last measured hit, will not work on AOD's */
  const Trk::TrackParameters* extrapolate(const Trk::Track* trk,
					  const Trk::Surface* surf);


  virtual Amg::Vector3D getMomentumAtVertex(const xAOD::Vertex& vertex, bool /*reuse*/) const;

#if 0
  virtual Trk::Intersection getIntersectionInCalo(const Amg::Vector3D& position, const Amg::Vector3D& momentum, const CaloCell_ID::CaloSample sample) const;
#endif

private:



  // CaloDetDescr usal stuff
  const CaloCell_ID* m_calo_id;
  const CaloDetDescrManager* m_calo_dd;
  ICaloCoordinateTool* m_calo_tb_coord;

  CaloPhiRange m_range;

  // Defines the surface  for extrapolation : 
  ToolHandle<ICaloSurfaceBuilder> m_calosurf;


  // Pre-configured extrapolator : will be choosen via jobOpt
  ToolHandle< Trk::IExtrapolator > m_extrapolator;
  // Trk::IExtrapolator*  m_extrapolator;



};

 
#endif // TRACKTOCALO_EXTRAPOLTRACKTOCALO_H






