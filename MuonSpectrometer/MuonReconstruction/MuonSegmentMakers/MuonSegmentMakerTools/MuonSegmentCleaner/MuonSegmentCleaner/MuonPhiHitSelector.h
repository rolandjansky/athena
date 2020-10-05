/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSegmentCleaner_MuonPhiHitSelector_H
#define MuonSegmentCleaner_MuonPhiHitSelector_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonHitSelector.h"

namespace Trk {
class PrepRawData;
class RIO_OnTrack;
}  // namespace Trk

class MuonPhiHitSelector : public AthAlgTool, virtual public Muon::IMuonHitSelector {
  public:
    MuonPhiHitSelector(const std::string&, const std::string&, const IInterface*);
    virtual ~MuonPhiHitSelector() = default;

    virtual StatusCode initialize();

    /** @brief Selects and builds a cleaned vector of RIO
        fits the associatedHits and build new RIOs, if m_competingRios true then for ambiguous hits competing rios are
       built
    */
    virtual std::vector<const Trk::MeasurementBase*>* select_rio(
        const double pmom, const std::vector<const Trk::RIO_OnTrack*>& associatedHits,
        const std::vector<const Trk::PrepRawData*>& unassociatedHits) const;

  private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };

    /** Toolhandle to CompetingRIOsOnTrackTool creator */
    ToolHandle<Muon::IMuonCompetingClustersOnTrackCreator> m_competingRIOsOnTrackTool{
        this,
        "MuonCompetingClustersOnTrackCreator",
        "Muon::MuonCompetingClustersOnTrackCreator/MuonCompetingClustersOnTrackCreator",
    };
    /** Toolhandle to ClusterOnTrackTool creator */
    ToolHandle<Muon::IMuonClusterOnTrackCreator> m_clusterCreator{
        this,
        "MuonClusterOnTrackCreator",
        "Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator",
    };

    /** flag to print out a summary of what comes in and what comes out */
    bool m_summary;
    /** flag for use of cosmics, straight line model will be used, no interaction point constraint */
    bool m_cosmics;
    /** flag that performs a clusterization and return clusters (default: false) */
    bool m_makeClusters;
    /** flag that build competing rios on track for amibguous trigger hits (default: false) */
    bool m_competingRios;

    /** fit method curved track model */
    void fitRecPhi(const double pmom, const std::vector<Identifier>& phiId, const std::vector<double>& phiHitx,
                   const std::vector<double>& phiHity, const std::vector<double>& phiHitz,
                   const std::vector<double>& phiError, std::vector<int>& quality, const int nphi,
                   std::vector<double>& phiPull, std::vector<int>& phiMult, std::vector<int>& phiSelect, double& chi2,
                   double& r0, double& phi, std::vector<double>& errorM, int& nfit) const;

    /** fit method straight line model */
    void fitPhiSL(const double pmom, const std::vector<Identifier>& id, const std::vector<double>& hitx,
                  const std::vector<double>& hity, const std::vector<double>& hitz, const std::vector<double>& error,
                  std::vector<int>& select, const int n, std::vector<double>& pull, int& imax, double& chi2, double& r0,
                  double& phi, std::vector<double>& errorM, bool fast) const;

    /** @brief clusterization method

    Use hits (select > 0) and pulls to make clusters

    Inputs
    id             = identifiers hits
    hitx hity hitz = position in space
    error          = associated error (in x-y plane)
    pull (from fit)= residual (hit -fit) /error
    select         = > 0 for selected hits
    n              = total number of hits
    fast = true    = fast fit without scattering centres and no error propagation
           false   = fit with scattering centres and error propagation

    Outputs
    clusterX Y Z   = cluster positions
    clusterError   =          errors
    clusterId      = cluster identifier (smallest pull)
    clusterHits    = number of hits per cluster
    ncl            = number of clusters
    chi2          = total chi2
    r0            = perigee parameter of fit (0,0)
    phi           = azimuthal angle of fit at perigee
    */

    void clusterPhi(const std::vector<Identifier>& id, const std::vector<double>& hitx, const std::vector<double>& hity,
                    const std::vector<double>& hitz, const std::vector<double>& error, const std::vector<double>& pull,
                    std::vector<int>& select, const int n, std::vector<double>& clusterX, std::vector<double>& clusterY,
                    std::vector<double>& clusterZ, std::vector<double>& clusterError,
                    std::vector<Identifier>& clusterId, std::vector<int>& clusterHits, std::vector<int>& clusterSelect,
                    std::vector<int>& clusterInt, int& ncl) const;
};

#endif  // MuonSegmentCleaner_MuonPhiHitSelector_H
