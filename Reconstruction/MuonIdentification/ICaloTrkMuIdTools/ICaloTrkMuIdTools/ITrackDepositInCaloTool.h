/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_ITRACKDEPOSITINCALOTOOL_H
#define CALOTRKMUIDTOOLS_ITRACKDEPOSITINCALOTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "muonEvent/DepositInCalo.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTracking/TrackParticle.h"

static const InterfaceID IID_ITrackDepositInCaloTool("ITrackDepositInCaloTool", 1, 0);

class ITrackDepositInCaloTool: virtual public IAlgTool
{

  /** @class ITrackDepositInCaloTool
    
      @author  Gustavo.Ordonez.Sanz <Gustavo.Ordonez.Sanz@cern.ch>
  */  
  public:
    enum CaloSurfaceType {
     Entrance = 0,
     Middle = 1,
     Exit = 2,
     Inside = 3,
     Outside = 4
    };   
    virtual ~ITrackDepositInCaloTool(){}

    static const InterfaceID& interfaceID(){return IID_ITrackDepositInCaloTool;}

    /**
       Deprecated function. Do not use anymore.
    */
    virtual std::vector<DepositInCalo> deposits(const Trk::Track* track, const double deltaR=0.3, const bool inCell= true) const = 0;
    /**
       Fills the vector of DepositInCalo using trackParameters as input
    */
    virtual std::vector<DepositInCalo> deposits(const Trk::TrackParameters *par, const double deltaR=0.3, const bool inCell= true) const = 0;
    /**
       Preselection of calorimeter layers can be done by @code getTraversedLayers(). The vector @code caloInfo and 
       @code extrapolations are returned. Straight line approximation. Covers EMB, TileBar, TileExt, EME and HEC acceptance.
    */
    virtual std::vector<DepositInCalo> getDeposits(const Trk::TrackParameters* par, const CaloCellContainer* caloCellCont = nullptr) const = 0;
    
    virtual std::vector<DepositInCalo> getDeposits(const xAOD::TrackParticle* tp) const = 0;
    
    virtual StatusCode getTraversedLayers(const Trk::TrackParameters* par, std::map<double, const CaloDetDescriptor*>& caloInfo, std::vector<Amg::Vector3D>& extrapolations) const = 0;
    /**
       Create surface for @code descr. CaloSurfaceType can be Entrance, Middle, Exit, Inside, Outside. 
       Works for both cylindrical and disc-like surfaces. The side of a cylindrical detector element is a disc and 
       vice-versa.
    */
    virtual Trk::Surface* createSurface(const CaloDetDescriptor* descr, CaloSurfaceType type) const = 0;
    /**
       Retrieve closest cell of the same as @code descr near @code par.
    */
    virtual const CaloCell* getClosestCell(const Trk::TrackParameters* par, const CaloDetDescriptor* descr) const = 0;
    /**
       Calculate the energy for @code par assuming particle to be of type @code particleHypo
    */
    virtual double calcEnergy(const Trk::TrackParameters* par, const Trk::ParticleHypothesis& particleHypo) const = 0;

    virtual const Trk::TrackParameters* extrapolateToSolenoid(const Trk::TrackParameters* par, bool oppositeMomentum = false) const = 0;
  
    /**
       Ordered map of CaloDetDescriptor. Layer distance (\f$R\f$ or \f$z\f$) versus descriptor.
    */
    typedef std::map<double, std::vector<const CaloDetDescriptor*> > CaloLayerMap;
    typedef std::map<double, std::vector<const CaloDetDescriptor*> >::const_iterator CaloLayerMapIt;

};

 
#endif
