/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_TRACKDEPOSITINCALOTOOL_H
#define CALOTRKMUIDTOOLS_TRACKDEPOSITINCALOTOOL_H

// --- Tools ---
#include "ICaloTrkMuIdTools/ITrackDepositInCaloTool.h"
#include "TrkExInterfaces/IExtrapolator.h"

// --- Athena common ---
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

// --- Athena ---
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "xAODTracking/TrackParticle.h"
#include "CxxUtils/CachedValue.h"

// --- STL ---
#include <map>
#include <vector>

// --- Forward declarations ---
class CaloCellContainer;
class CaloPhiRange;
class TileDetDescrManager;

class TH1F;
class TH2F;
class ITHistSvc;


///////////////////////////////////////////////////////////////////////////////
// TrackDepositInCaloTool
///////////////////////////////////////////////////////////////////////////////
class TrackDepositInCaloTool : public AthAlgTool, virtual public ITrackDepositInCaloTool {
    /** @class TrackDepositInCaloTool

        Tool to get the energy that is deposited by a particle in the calorimeter.

        The core method of this tool is the deposits function that returns a std::vector of DepositInCalo. All other
        methods are essentially helper functions for this one.

        @author  Gustavo.Ordonez.Sanz <Gustavo.Ordonez.Sanz@cern.ch>
        @author  Marcel Raas <marcelra@cern.ch>
      */
public:
    TrackDepositInCaloTool(const std::string& type, const std::string& name, const IInterface* pInterface);
    virtual ~TrackDepositInCaloTool() = default;

    StatusCode initialize() override;

    /**
       Fills the vector of DepositInCalo using TrackParameters as input.
       @param par TrackParameters to be used as input. The parameters are assumed to be within the solenoid volume.
       @param deltaR Unused parameter. Kept for backward compatibility.
       @param inCell Unused parameter. Kept for backward compatibility.
    */
    std::vector<DepositInCalo> getDeposits(const Trk::TrackParameters* par, const CaloCellContainer* caloCellCont) const override;

    std::vector<DepositInCalo> getDeposits(const xAOD::TrackParticle* tp, const CaloCellContainer* caloCellCont,
                                           const CaloExtensionCollection* extensionCache) const override;

    std::vector<DepositInCalo> deposits(const Trk::TrackParameters* par, const CaloCellContainer* cellContainer) const override;
   /**
       Creates a Trk::Surface for a calorimeter region that is described by CaloDetDescr.

       Works for both cylindrical and disc-like surfaces. The side of a cylindrical detector element is a disc and
       vice-versa. Don't forget to clear memory. CaloSurfaceType can be Entrance, Middle, Exit, Inside, Outside.
       Inside is the boundary of the region closest to the center of the detector, outside is the
       boundary that is furthest away.

       @param descr The CaloDetDescriptor for the surface that you want to create.
       @param type What type of surface to create.
    */
    Trk::Surface* createSurface(const CaloDetDescriptor* descr, CaloSurfaceType type) const override;

    /**
       Calculate the energy using @f$ E^2 = m^2 + p^2 @f$.
       @param par Input TrackParameters
       @param particleHypo Particle type. This determines the mass.
    */
    double calcEnergy(const Trk::TrackParameters* par, const Trk::ParticleHypothesis& particleHypo) const override;

private:
    std::unique_ptr<const Trk::TrackParameters> extrapolateToSolenoid(const EventContext& ctx, const Trk::TrackParameters* par,
                                                                      bool oppositeMomentum = false) const;

   /**
       Extrapolate track to cylinder surface along straight line.
       (x0, y0, z0) is the starting position, (phi0,theta0) is the direction of the momentum, r is the bound of
       the cylinder. The values are returned in (xe, ye, ze).
    */
    static std::unique_ptr<Amg::Vector3D> extrapolateR(const Amg::Vector3D& initialPosition, double phi0, double theta0, double r) ;
    /**
       Extrapolate track to disc surface along straight line.
       Parameter explanation in the same line as for extrapolateR().
    */
    static std::unique_ptr<Amg::Vector3D> extrapolateZ(const Amg::Vector3D& initialPosition, double phi0, double theta0, double z) ;
    /**
       Create histograms and register them to HistSvc.
    */
    StatusCode bookHistos();

    /**
       Retrieve the CaloCell for which its center is closest to the position of the particle.
       @param par TrackParameters of the particle.
       @param descr Calorimeter detector region information. Only cells from this detector region are considered.
    */
    const CaloCell* getClosestCell(const CaloDetDescrManager* caloDDM,
                                   const Trk::TrackParameters* par,
                                   const CaloDetDescriptor* descr,
                                   const CaloCellContainer* cellContainer) const;
 

    static const CaloCell* getClosestCellLAr(const CaloDetDescrManager* caloDDM,
                                             const Trk::TrackParameters* par,
                                             const CaloDetDescriptor* descr,
                                             const CaloCellContainer* caloCellCont);

    const CaloCell* getClosestCellTile(const CaloDetDescrManager* caloDDM,
                                       const Trk::TrackParameters* par,
                                       const CaloDetDescriptor* descr,
                                       const CaloCellContainer* caloCellCont) const;

    std::vector<const CaloCell*> getCaloCellsForLayer(const CaloDetDescrManager* caloDDM,
                                                      const CaloDetDescriptor* descr,
                                                      const Trk::TrackParameters* parEntrance,
                                                      const Trk::TrackParameters* parExit,
                                                      const CaloCellContainer* caloCellCont) const;

    std::vector<const CaloCell*> getCaloCellsForTile(const CaloDetDescrManager* caloDDM,
                                                     const CaloDetDescriptor* descr,
                                                     const Trk::TrackParameters* parEntrance,
                                                     const Trk::TrackParameters* parExit,
                                                     const CaloCellContainer* caloCellCont) const;

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
    StatusCode getTraversedLayers(const CaloDetDescrManager* caloDDM,
                                  const Trk::TrackParameters* par,
                                  std::map<double, const CaloDetDescriptor*>& caloInfo,
                                  std::vector<Amg::Vector3D>& extrapolations) const;

    std::unique_ptr<const Trk::TrackParameters> extrapolateToEntranceOfLayer(const EventContext& ctx,
                                                                             const Trk::TrackParameters* par,
                                                                             const CaloDetDescriptor* descr) const;

    std::unique_ptr<const Trk::TrackParameters> extrapolateToExitOfLayer(const EventContext& ctx,
                                                                         const Trk::TrackParameters* par,
                                                                         const CaloDetDescriptor* descr) const;

    static double distance(const Amg::Vector3D& p1, const Amg::Vector3D& p2) ;

    static bool isInsideDomain(double position, double domainCenter, double domainWidth, bool phiVariable = false) ;
    static bool isInsideCell(const Amg::Vector3D& position, const CaloCell* cell) ;
    static bool inCell(const CaloCell* cell, const Amg::Vector3D& pos) ;

private:
    // Services & Tools
    ITHistSvc* m_histSvc{};
    ToolHandle<Trk::IExtrapolator> m_extrapolator{this, "ExtrapolatorHandle", ""};

    const TileDetDescrManager* m_tileDDM{nullptr};

    ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtensionTool{this, "ParticleCaloExtensionTool", "",
                                                                    "Tool to make the step-wise extrapolation"};
    ToolHandle<Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool{this, "ParticleCaloCellAssociationTool", ""};

    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloDetDescrMgrKey {
    this,
    "CaloDetDescrManager",
    "CaloDetDescrManager",
    "SG Key for CaloDetDescrManager in the Condition Store"
  };

    // Members
    bool m_doExtr;  //!< Flag to perform extrapolations using m_extrapolator
    bool m_doHist;  //!< Flag to write histograms to track performance

    struct LayerMaps {
      //!< std::map of \f$r\f$ distance versus descriptor for cylindrical calo regions
      CaloLayerMap m_barrelLayerMap;
      //!< std::map of \f$z\f$ distance versus descriptor for disc-like calo regions
      CaloLayerMap m_endCapLayerMap;
    };
    CxxUtils::CachedValue<LayerMaps> m_layerMaps;

    LayerMaps initializeDetectorInfo(const CaloDetDescrManager* caloDDM) const;

    // Histograms
    struct Hists {
      StatusCode book (ITHistSvc& histSvc);

      TH1F* m_hDepositLayer12{};
      TH1F* m_hDepositLayer13{};
      TH1F* m_hDepositLayer14{};

      TH2F* m_hParELossEta{};
      TH2F* m_hParELossSample{};

      ///////////////////////////////
      TH1F* m_hDeltaEtaLastPar{};
      TH1F* m_hDeltaRadiusLastPar{};
      TH1F* m_hDepositsInCore{};
      TH1F* m_hDepositsInCone{};
      TH2F* m_hDistDepositsTile{};
      TH2F* m_hDistDepositsHEC{};

      TH2F* m_hEMB1vsdPhi{};
      TH2F* m_hEMB2vsdPhi{};
      TH2F* m_hEMB3vsdPhi{};
      TH2F* m_hEMB1vsdEta{};
      TH2F* m_hEMB2vsdEta{};
      TH2F* m_hEMB3vsdEta{};
    };
    std::unique_ptr<Hists> m_h;
    Hists& getHists() const;

    const Trk::ParticleMasses m_particlemasses;
};

#endif
