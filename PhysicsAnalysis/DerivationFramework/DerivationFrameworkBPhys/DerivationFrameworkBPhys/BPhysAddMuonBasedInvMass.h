/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 *  @file   BPhysAddMuonBasedInvMass.h
 *  @author Wolfgang Walkowiak <wolfgang.walkowiak@cern.ch>
 *
 *  @brief  Augmentation with muon-information based invariant mass.
 *
 */  
//
#ifndef DERIVATIONFRAMEWORK_BPhysAddMuonBasedInvMass_H
#define DERIVATIONFRAMEWORK_BPhysAddMuonBasedInvMass_H

#include <string>
#include <vector>
#include <set>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODBPhys/BPhysHelper.h"
#include "EventPrimitives/EventPrimitives.h"
#include "ITrackToVertex/ITrackToVertex.h"

namespace DerivationFramework {
  //
  // typedefs -- to abbreviate long lines
  //
  typedef std::vector<const xAOD::TrackParticle*> TrackBag;
  typedef std::vector<const xAOD::Muon*>          MuonBag;

  ///
  /// @class BPhysAddMuonBasedInvMass
  /// @author Wolfgang Walkowiak <wolfgang.walkowiak@cern.ch>
  ///
  /// @brief Augment secondary vertices with muon-information-based mass.
  /// 
  ///  Add muon-information based invarient mass to secondary vertices using
  ///  a four vector sum.  Optionally, it also calculates the minimum
  ///  chi2 for all muon tracks of the secondary vertex candidate w.r.t.
  ///  any primary vertex matching the selection criteria.
  ///
  ///  ### Job options provided by this class:
  /// <table border="0">
  /// <tr><th align="left">Name</th>         
  ///     <th align="left">Description</th>
  /// </tr>
  /// <tr><td>BranchPrefix</td>
  ///     <td>assign the prefix of added branches
  ///         (possibly the derivation format's name)</td>
  /// </tr>
  /// <tr><td>VertexContainerName</td> 
  ///     <td>name of container for vertices</td>
  /// </tr>
  /// <tr><td>TrkMasses</td>
  ///     <td>ordered list of track masses
  ///         (Important to keep proper order: J/psi muons go first!)</td>
  /// </tr>
  /// <tr><td>TrackToVertexTool</td>   
  ///     <td>ToolHandle for track-to-vertex tool</td>
  /// </tr>
  /// <tr><td>AdjustToMuonKinematics</td>
  ///     <td>Adjust the primary track particle's kinematics to the one of 
  ///         the muon.</td>
  /// </tr>
  /// <tr><td valign="top">AddMinChi2ToAnyPVMode</td>
  ///      <td>mode of minLogChi2ToAnyPV calculation: (default: 0)
  ///          <table border="0"> 
  ///            <tr><th align="left">Value</th>
  ///                <th align="left">Explanation<th></tr>
  ///            <tr><td>  0  </td><td>no such calculation</td></tr>
  ///            <tr><td>  1  </td><td>use all PVs of requested
  ///                                  type(s)</td></tr>
  ///            <tr><td>  2  </td><td>exclude PVs associated to SVs</td></tr>
  ///            <tr><td>  3  </td><td>replace PVs associated to SVs by 
  ///                                  corresponding refitted PVs</td></tr>
  ///          </table></td>
  /// </tr>
  /// <tr><td>PrimaryVertexContainerName</td>
  ///     <td>name of container for primary vertices</td>
  /// </tr>
  /// <tr><td>MinNTracksInPV</td>
  ///     <td>minimum number of tracks in PV
  ///         for PV to be considered in calculation
  ///         of minChi2MuToAnyPV variable.</td>
  /// </tr>
  /// <tr><td>PVTypesToConsider</td>
  ///     <td>list of primary vertex types to consider
  ///         (default: {1, 3})</td>
  /// </tr>
  /// <tr><td>DoVertexType</td>
  ///     <td>PV-to-SV association types to be considered (bitwise variable,
  ///         see xAODBPhys::BPhysHelper)<br>
  ///         Note: only needed for AddMinChi2ToAnyPVMode > 1</td>
  /// </tr>
  /// </table>
  ///                             
  ///  @note
  ///
  ///  For a usage example see BPHY8.py .
  /// 
  class BPhysAddMuonBasedInvMass : virtual public AthAlgTool,
    virtual public IAugmentationTool {

  public:
      ///
      /// @brief Main contructor
      /// 
      BPhysAddMuonBasedInvMass(const std::string& t, const std::string& n,
			       const IInterface* p);
      
      /// @brief Initialize augmentation tool.
      virtual StatusCode initialize();
      /// @brief Finalize augmentation tool.
      virtual StatusCode finalize();
      /// @brief Main method called for each event.
      virtual StatusCode addBranches() const;

  protected:
      ///
      /// @name Internal protected methods
      /// @{
      ///
      /// @brief Calculate muon-information based mass values if available.
      ///
      /// @param[in] vtx secondary vertex
      /// @param[in] trkMasses ordered vector of track mass values
      /// @param[in] nMuRequested number of muons requested
      /// @returns muon-information based invariant mass for secondary
      ///          vertex and the corresponding uncertainty
      /// 
      std::pair<double, double> getMuCalcMass(xAOD::BPhysHelper& vtx,
					      std::vector<double>
					      trkMasses,
					      int nMuRequested) const;
      ///
      /// @brief Obtain a set of tracks with muon track information if available
      ///
      /// @param[in] vtx secondary vertex
      /// @returns   container of muon tracks, number of muons found
      ///
      std::pair<TrackBag, int> getTracksWithMuons(xAOD::BPhysHelper& vtx) const;
      ///
      /// @brief Calculate invariant mass and uncertainty from a set of tracks.
      ///
      /// Returns invariant mass and mass error given
      /// a set of tracks, their mass hypotheses and a reference position. 
      /// Each track must have a separate mass hypothesis in
      /// the vector, and they must be in the same order as the tracks in the
      /// track vector.  Otherwise it will go horribly wrong.
      ///
      /// @param[in] trksIn container with tracks to be considered
      /// @param[in] massHypoTheses vector of mass hypotheses in the same
      ///            order as the tracks
      /// @param[in] pos position of the vertex
      /// @returns   invariant mass value and uncertainty
      ///
      std::pair<double,double>
	getInvariantMassWithError(TrackBag trksIn,
				  std::vector<double> massHypotheses,
				  const Amg::Vector3D& pos) const;
      ///
      /// @brief Determine minimum log chi2 of signal muon tracks w.r.t.
      //         any primary vertex.
      ///
      /// Find minimum log chi2 distance of signal muons w.r.t any primary
      /// vertex of required types and with a minimum number of tracks cut.
      /// It also depends on the mode w.r.t. the treatment of the associated
      /// primary vertex and the type of PV-to-SV association.
      /// Returns this minimum chi2.
      /// 
      /// @param[in] vtx secondary vertex
      /// @param[in] pvContainer container of primary vertices
      /// @parma[in] pvtypes vector of primary vertex types to be considered
      /// @param[in] minNTracksInPV minimum number of tracks in primary
      ///            vertex for it to be considered
      /// @param[in] mode mode of operation (possible values: 0, 1, 2 ,3)
      /// @param[in] pv_type type of PV-to-SV association
      /// @returns   minimum log chi2 = log(d0^2/d0e^+z0^2/z0e^2) w.r.t.
      ///            any primary vertex
      ///
      double getMinChi2ToAnyPV(xAOD::BPhysHelper& vtx,
			       const xAOD::VertexContainer* pvContainer,
			       const std::vector<int>& pvtypes,
			       const int minNTracksInPV,
			       const int mode,
			       const xAOD::BPhysHelper::pv_type&
			       pvAssocType) const;
      ///
      /// @brief Calculate log chi2 value of a track w.r.t. a position.
      ///
      /// Calculate the log chi2 ( = log((d0/d0e)^2+(z0/z0e)^2) contribution
      /// of a track at the position closest to the given PV.
      ///
      /// @param[in] track track considered
      /// @param[in] pos   position considered
      /// @returns   log chi2 value
      ///
      double getTrackPVChi2(const xAOD::TrackParticle& track,
			    const Amg::Vector3D& pos) const;
      ///
      /// @brief Extract 3x3 momentum covariance matrix from a TrackParticle.
      ///
      /// Extract the 3x3 momentum covariance matrix in (x,y,z) notation
      /// from the (phi, theta, qoverp) notation from a TrackParticle.
      ///
      /// @param[in] track TrackParticle considered
      /// @returns   3x3 momentum covariance matrix
      ///
      AmgSymMatrix(3) getMomentumCov(const xAOD::TrackParticle* track) const;
      /// 
      /// @brief Extract 3x3 momentum covariance matrix from a Perigee.
      ///
      /// Extract the 3x3 momentum covariance matrix in (x,y,z) notation
      /// from the (phi, theta, qoverp) notation from a Perigee.
      ///
      /// @param[in] perigee Trk::Perigee considered
      /// @returns   3x3 momentum covariance matrix
      ///
      AmgSymMatrix(3) getMomentumCov(const Trk::Perigee* perigee) const;
      /// 
      /// @brief Extract 3x3 momentum covariance matrix from a track parameter
      /// vector and 5x5 covariance matrix.
      ///
      /// Extract the 3x3 momentum covariance matrix in (x,y,z) notation
      /// from the (phi, theta, qoverp) notation from a vector of
      /// track parameters and the 5x5 error matrix.
      ///
      /// @param[in] pars 5-vector of track parameters
      /// @param[in] cov  5x5 covariance matrix of track parameters
      /// @returns   3x3 momentum covariance matrix
      ///
      AmgSymMatrix(3) getMomentumCov(const AmgVector(5)& pars,
				     const AmgSymMatrix(5)& cov) const;
      ///
      /// @brief Find all muons associated to secondary vertex.
      ///
      /// Returns a vector of xAOD::Muon objects found
      /// in this vertex and subsequent decay vertices.
      /// Recursively calls itself if necessary.
      ///
      /// @param[in] vtx secondary vertex
      /// @returns   container of muons found
      ///
      MuonBag findAllMuonsInDecay(xAOD::BPhysHelper& vtx) const;
      ///
      /// @brief Obtain a set of ID tracks for a set of muons.
      ///
      /// @param[in] muons container of muon objects
      /// @returns   container of associated ID tracks
      ///
      TrackBag getIdTracksForMuons(MuonBag& muons) const;
      ///
      /// @brief Extract TrackParticle for Muon and adjust kinematics.
      ///
      /// Extract primary track particle from muon;
      /// if configured adjust pt, eta and phi of it before returning
      /// a pointer to it.
      ///
      /// @param[in] muon pointer to muon
      /// @returns   TrackParticle pointer
      ///
      const xAOD::TrackParticle* adjustTrackParticle(const xAOD::Muon* muon)
	const;
      ///
      /// @brief Initialize PV-to-SV association type vector.
      ///
      void initPvAssocTypeVec();
      ///
      /// @brief Clear the cache of adjusted TrackParticles.
      ///
      void clearAdjTpCache() const;
      /// @}
  private:      
      /// @name job options
      /// @{
      std::string                      m_branchPrefix;
      std::string                      m_vertexContainerName;
      std::vector<double>              m_trkMasses;
      ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
      bool                             m_adjustToMuonKinematics;
      int                              m_addMinChi2ToAnyPVMode;
      std::string                      m_pvContainerName;
      int                              m_minNTracksInPV;
      std::vector<int>                 m_pvTypesToConsider;
      int                              m_doVertexType;
      /// @}
      ///
      /// map original -> adjusted track particles
      typedef std::map<const xAOD::TrackParticle*, const xAOD::TrackParticle*>
	TpMap_t;
      /// map of adjusted track particles as cache
      mutable TpMap_t m_adjTpCache; 

      /// cache for individual vertex types
      std::vector<xAOD::BPhysHelper::pv_type> m_pvAssocTypes;

  }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_BPhysAddMuonBasedInvMass_H
