/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_TRACKDEPOSITINCALOTOOL_H
#define CALOTRKMUIDTOOLS_TRACKDEPOSITINCALOTOOL_H

// --- Tools ---
#include "ICaloTrkMuIdTools/ITrackDepositInCaloTool.h"
#include "ICaloTrkMuIdTools/ITrackEnergyInCaloTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "CaloInterface/ICaloNoiseTool.h"

// --- Athena common ---
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// --- Athena ---
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"

// --- STL ---
#include <utility>
#include <vector>

// --- Forward declarations ---
class CaloCellContainer;
class CaloPhiRange;
class TileDetDescrManager;

class TH1F;
class TH2F;


class CaloDetDescrManager;

///////////////////////////////////////////////////////////////////////////////
// TrackDepositInCaloTool
///////////////////////////////////////////////////////////////////////////////
class TrackDepositInCaloTool: public AthAlgTool, virtual public ITrackDepositInCaloTool { 
/** @class TrackDepositInCaloTool

    Tool to get the energy that is deposited by a particle in the calorimeter.
    
    The core method of this tool is the deposits function that returns a std::vector of DepositInCalo. All other
    methods are essentially helper functions for this one. 

    @author  Gustavo.Ordonez.Sanz <Gustavo.Ordonez.Sanz@cern.ch>
    @author  Marcel Raas <marcelra@cern.ch>
  */  
  public:
    TrackDepositInCaloTool(const std::string& type, const std::string& name, const IInterface* pInterface);
    virtual ~TrackDepositInCaloTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize(); 

    /** 
       Deprecated method, do not use. This method will be removed soon.
    */
    std::vector<DepositInCalo> deposits(const Trk::Track* track, const double deltaR=0.3, const bool inCell= true) const;
    /**
       Fills the vector of DepositInCalo using TrackParameters as input. 
       @param par TrackParameters to be used as input. The parameters are assumed to be within the solenoid volume.
       @param deltaR Unused parameter. Kept for backward compatibility.
       @param inCell Unused parameter. Kept for backward compatibility.
    */
    std::vector<DepositInCalo> getDeposits(const Trk::TrackParameters* par, const CaloCellContainer* caloCellCont = nullptr) const;

    std::vector<DepositInCalo> getDeposits(const xAOD::TrackParticle* tp) const;
    
    std::vector<DepositInCalo> deposits(const Trk::TrackParameters *par, const double deltaR=0.3, const bool inCell= true) const;
    /**
       This function determines which calorimeter regions the tracks traverses.
       The vector caloInfo and extrapolations are returned. Straight line approximation is employed in the calorimeter regions 
       after the parameters are extrapolated to the surface of the solenoid. This accounts for the magnetic field bending.
       Covers EMB, TileBar, TileExt, EME and HEC acceptance. The FCAL and TileGap regions are not included. Do not forget to check
       the StatusCode. 

       @param par Input track parameters
       @param caloInfo Resulting vector of CaloDetDescriptor that is traversed. 
       @param extrapolations   Resulting vector of straight-line extrapolations that were obtained.
    */
    StatusCode getTraversedLayers(const Trk::TrackParameters* par, std::map<double, const CaloDetDescriptor*>& caloInfo, std::vector<Amg::Vector3D>& extrapolations) const;
    /**
       Creates a Trk::Surface for a calorimeter region that is described by CaloDetDescr. 
       
       Works for both cylindrical and disc-like surfaces. The side of a cylindrical detector element is a disc and 
       vice-versa. Don't forget to clear memory. CaloSurfaceType can be Entrance, Middle, Exit, Inside, Outside. 
       Inside is the boundary of the region closest to the center of the detector, outside is the 
       boundary that is furthest away.

       @param descr The CaloDetDescriptor for the surface that you want to create.
       @param type What type of surface to create.
    */
    Trk::Surface* createSurface(const CaloDetDescriptor* descr, CaloSurfaceType type) const;
    /**
       Retrieve the CaloCell for which its center is closest to the position of the particle.
       @param par TrackParameters of the particle.
       @param descr Calorimeter detector region information. Only cells from this detector region are considered.
    */
    const CaloCell* getClosestCell(const Trk::TrackParameters* par, const CaloDetDescriptor* descr) const;
    /**
       Calculate the energy using @f$ E^2 = m^2 + p^2 @f$.
       @param par Input TrackParameters
       @param particleHypo Particle type. This determines the mass.
    */
    double calcEnergy(const Trk::TrackParameters* par, const Trk::ParticleHypothesis& particleHypo) const;

    const Trk::TrackParameters* extrapolateToSolenoid(const Trk::TrackParameters* par, bool oppositeMomentum = false) const;  
    /**
       Ordered map of CaloDetDescriptor. Layer distance (\f$R\f$ in case of a cylindric detector element or \f$z\f$ in case of a
       disc-like detector element) versus descriptor.
    */
    typedef std::map<double, std::vector<const CaloDetDescriptor*> > CaloLayerMap;
    /**
       The CaloLayerMap iterator.
    */
    typedef std::map<double, std::vector<const CaloDetDescriptor*> >::const_iterator CaloLayerMapIt;
    /**
      Map of cells in (eta(phi,CaloCell))
    */
    typedef std::map<double, std::map<double, const CaloCell*> > NeighbourMap; 
    
  private:
    /**
       Invoked from initialize(), initializes the CaloLayerMap.
    */
    StatusCode initializeDetectorInfo();
    /**
       Extrapolate track to cylinder surface along straight line.
       (x0, y0, z0) is the starting position, (phi0,theta0) is the direction of the momentum, r is the bound of 
       the cylinder. The values are returned in (xe, ye, ze).
    */
    Amg::Vector3D* extrapolateR(const Amg::Vector3D& initialPosition, double phi0, double theta0, double r) const;
    /**
       Extrapolate track to disc surface along straight line.
       Parameter explanation in the same line as for extrapolateR().
    */
    Amg::Vector3D* extrapolateZ(const Amg::Vector3D& initialPosition, double phi0, double theta0, double z) const;
    /**
       Create histograms and register them to HistSvc.
    */
    StatusCode bookHistos();

    const CaloCell* getClosestCellLAr(const Trk::TrackParameters* par, const CaloDetDescriptor* descr, const CaloCellContainer* caloCellCont) const;
    const CaloCell* getClosestCellTile(const Trk::TrackParameters* par, const CaloDetDescriptor* descr) const;
    std::vector<const CaloCell*>* getCaloCellsForLayer(const CaloDetDescriptor* descr, const Trk::TrackParameters* parEntrance, 
						       const Trk::TrackParameters* parExit, const CaloCellContainer* caloCellCont) const;
    std::vector<const CaloCell*>* getCaloCellsForTile(const CaloDetDescriptor* descr, const Trk::TrackParameters* parEntrance, 
						      const Trk::TrackParameters* parExit, const CaloCellContainer* caloCellCont) const;

    const Trk::TrackParameters* extrapolateToEntranceOfLayer(const Trk::TrackParameters* par, const CaloDetDescriptor* descr) const;
	  const Trk::TrackParameters* extrapolateToExitOfLayer(const Trk::TrackParameters* par, const CaloDetDescriptor* descr) const;

//    double distanceInTile(const Trk::TrackParameters* par, const Trk::TrackParameters* parExit, const CaloDetDescrElement* dde, double cell_z_min, double cell_z_max) const;
    double distance(const Amg::Vector3D& p1, const Amg::Vector3D& p2) const;
    
    bool isInsideDomain(double position, double domainCenter, double domainWidth, bool phiVariable = false) const;
    bool isInsideCell(const Amg::Vector3D& position, const CaloCell* cell) const;
    bool inCell(const CaloCell* cell, const Amg::Vector3D& pos) const;
    void showNeighbours(const CaloCell* cell) const;
    
  private:

    // Services & Tools
    ITHistSvc*                          m_histSvc;                             //!< Pointer to THistSvc
    ToolHandle<Trk::IExtrapolator>      m_extrapolator;                        //!< Extrapolator tool
    const CaloDetDescrManager*          m_caloDDM;                             //!< Calorimeter detector description manager
    const TileDetDescrManager*          m_tileDDM;
    
    ToolHandle <Trk::IParticleCaloExtensionTool> m_caloExtensionTool; //!< Tool to make the step-wise extrapolation
    ToolHandle <Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool; //!< Tool to make the step-wise extrapolation
    
    // Members
    const CaloCellContainer*    m_cellContainer;                       //!< CaloCell container.

    bool            m_doExtr;                                                  //!< Flag to perform extrapolations using m_extrapolator
    bool            m_doHist;                                                  //!< Flag to write histograms to track performance
    bool            m_debugMode;                                               //!< Flag to run in specialized debug mode
    bool            m_showNeighbours;
    double          m_solenoidRadius;                                          //!< Radius of the solenoid surrounding the ID
    //double          m_solenoidHalfLength;                                      //!< Halflength of the solenoid surrounding the ID
    CaloLayerMap    m_barrelLayerMap;                                          //!< std::map of \f$r\f$ distance versus descriptor for cylindrical calo regions
    CaloLayerMap    m_endCapLayerMap;                                          //!< std::map of \f$z\f$ distance versus descriptor for disc-like calo regions
    
    //mutable int     m_stacks;                                                  //!< Counts the number of stacks (diagnoses indefinite recursion).

    // Histograms        
    TH1F* m_hDepositLayer12;
    TH1F* m_hDepositLayer13;
    TH1F* m_hDepositLayer14;
    
    TH2F* m_hParELossEta;
    TH2F* m_hParELossSample;

    ///////////////////////////////
    TH1F* m_hDeltaEtaLastPar;
    TH1F* m_hDeltaRadiusLastPar;
    TH1F* m_hDepositsInCore;
    TH1F* m_hDepositsInCone;
    TH2F* m_hDistDepositsTile;
    TH2F* m_hDistDepositsHEC;
    
    TH2F* m_hEMB1vsdPhi;
    TH2F* m_hEMB2vsdPhi;
    TH2F* m_hEMB3vsdPhi;
    TH2F* m_hEMB1vsdEta;
    TH2F* m_hEMB2vsdEta;
    TH2F* m_hEMB3vsdEta;

};
    
 
#endif
