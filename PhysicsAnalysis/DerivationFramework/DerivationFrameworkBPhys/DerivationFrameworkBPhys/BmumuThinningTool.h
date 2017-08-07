/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
/**
 * @file   BmumuThinningTool.h
 * @author Wolfgang Walkowiak <wolfgang.walkowiak@cern.ch>
 *
 * @brief  Primary vertex, track and muon thinning for Bmumu analysis.
 */

#ifndef DERIVATIONFRAMEWORK_BmumuThinningTool_H
#define DERIVATIONFRAMEWORK_BmumuThinningTool_H

#include "DerivationFrameworkBPhys/CfAthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODBPhys/BPhysHypoHelper.h"

#include <string>

class IThinningSvc;

namespace SG {
  class AuxElement;
}

namespace xAOD {
  class AuxContainerBase;
}

namespace DerivationFramework {
  ///
  /// @class   BmumuThinningTool
  /// @author Wolfgang Walkowiak <wolfgang.walkowiak@cern.ch>
  ///
  /// @brief  Primary vertex, track and muon thinning for Bmumu analysis.
  ///
  /// This class provides a thinning tool to thin the primary vertex,
  /// the muon and calibrated muon collections as well as the inner
  /// detector track selection depending on:
  /// - the selected secondary vertices
  /// - the selected PV-to-SV vertex association method
  /// - additional track sub-collection (e.g. "closest tracks")
  ///
  /// This tool is primarily written for the B(s)->mumu analysis
  /// and used by the BPHY8 derivation.
  /// 
  /// ### Job options
  /// <table border="0">
  /// <tr><th align="left">Name</td>
  ///     <th align="left">Description</th></tr>
  /// <tr><td valign="top">ThinningService</td>
  ///     <td>Thinning service handle</td></tr>
  /// <tr><td valign="top">TrackParticleContainerName</td>
  ///     <td>TrackParticle container name
  ///         (default: InDetTrackParticles)</td></tr>
  /// <tr><td valign="top">VertexContainerNames</td>
  ///     <td>List of secondary vertex container names</td></tr>
  /// <tr><td valign="top">VertexPassFlags</td>
  ///     <td>List of pass flags for the seconary vertices
  ///         empty list lets all vertices pass.
  ///         List length needs to be identical to length of
  ///         VertexContainerNames list if AlignPassToVertexList
  ///         is True</td></tr>
  /// <tr><td valign="top">AlignPassToVertexList</td>
  ///     <td>Align VertexPassFlags to VertexContainerNames list?
  ///         This option causes a 1:1 correlation between the two lists,
  ///         i.e. a flag is only applied to the corresponding container
  ///         if this option is set to True. (default: false)</td></tr>
  /// <tr><td valign="top">PVContainerName</td>
  ///     <td>Primary vertex container name</td></tr>
  /// <tr><td valign="top">RefPVContainerNames</td>
  ///     <td>Refitted primary vertex container names.
  ///         This list must be of same length and order as the
  ///          m_vtxContNames list.
  ///         (or empty: no thinning of refitted primary
  ///          vertex containers)</td></tr>
  /// <tr><td valign="top">MuonContainerName</td>
  ///     <td> Name of the used muon container (default: "")</td></tr>
  /// <tr><td valign="top">CalibMuonContainerName</td>
  ///     <td>Name of the calibrated muons container (default: "")</td></tr>
  /// <tr><td valign="top">CloseTrackBranchBaseName</td>
  ///     <td>Closest track branch base name</td></tr>
  /// <tr><td valign="top">CloseTrackBranchPrefixes</td>
  ///     <td>Closest track branch prefixes</td></tr>
  /// <tr><td valign="top">KeepTracksForSelectedPVs</td>
  ///     <td>Keep tracks for selected (refitted) primary vertices?
  ///         (default: false)</td></tr>
  /// <tr><td valign="top">MatchCalibratedMuons</td>
  ///     <td>Match vertex muons with calibrated muons?
  ///         (default: false)</td></tr>
  /// <tr><td valign="top">MarkMatchedMuons</td>
  ///     <td>Mark orginal muons for matched calibrated muons as well?
  ///         (only makes sense if MatchCalibratedMuons = True;
  ///          default: false)</td></tr>
  /// <tr><td valign="top">MarkMatchedCalMuons</td>
  ///     <td>Mark calibrated muons for matched calibrated muons as well?
  ///         (only makes sense if MatchedCalibratedMuons = False;
  ///          default: false)</td></tr>
  /// <tr><td valign="top">SyncMatchedMuonsBothWays</td>
  ///     <td> Force syncing marked muons both ways?
  ///          (default: false)</td></tr>
  /// <tr><td valign="top">AllowFastMuonMaskSync</td>
  ///     <td>Allow fast sync of myon masks?
  ///         (Set to 'False' to force in-depth synchronization
  ///          of muon masks. Default: false)</td></tr>
  /// <tr><td valign="top">KeepCloseTracks</td>
  ///     <td>Keep tracks for closest tracks? (default: false)</td></tr>
  /// <tr><td valign="top">KeepTracksForMuons</td>
  ///     <td>Keep tracks for selected muons? (default: false)</td></tr>
  /// <tr><td valign="top">KeepTracksForCalMuons</td>
  ///     <td>Keep tracks for selected calibrated muons?
  ///         (default: false)</td></tr>
  /// <tr><td valign="top">KeepMuonsForTracks</td>
  ///     <td>Keep (original) muons for selected tracks?
  ///         (default: false)</td></tr>
  /// <tr><td valign="top">KeepCalMuonsForTracks</td>
  ///     <td>Keep calibrated muons for selected tracks?
  ///         (default: false)</td></tr>
  /// <tr><td valign="top">ApplyAndForVertices</td>
  ///     <td>Apply AND for mask matching for vertices?
  ///         (default: false)</td></tr>
  /// <tr><td valign="top">ApplyAndForTracks</td>
  ///     <td>Apply AND for mask matching for tracks?
  ///         (default: false)</td></tr>
  /// <tr><td valign="top">ApplyAndForMuons</td>
  ///     <td>Apply AND for mask matching for muons?
  ///         (default: false)</td></tr>
  /// <tr><td valign="top">ThinPVs"</td>
  ///     <td>Thin primary vertex collection? (default: true)</td></tr>
  /// <tr><td valign="top">ThinRefittedPVs"</td>
  ///     <td>Thin refitted primary vertex collection?
  ///         (default: true)</td></tr>
  /// <tr><td valign="top">ThinTracks"</td>
  ///     <td>Thin ID track collection?
  ///         (default: true)</td></tr>
  /// <tr><td valign="top">ThinMuons"</td>
  ///     <td>Thin muon collections?
  ///         (default: true)</td></tr>
  /// </table>
  ///
  class BmumuThinningTool : public CfAthAlgTool, public IThinningTool {

    // useful typedefs
    typedef xAOD::BPhysHelper::pv_type pv_type;
    typedef ElementLink<xAOD::TrackParticleContainer> TrackParticleLink;
    
  public:
    /// @name pv_type to string map
    //  Note: may later be migrated to xAODBPhys/BPhysHelper
    static std::map<pv_type, std::string> PvTypeToVarNameMap;
    
  public:
    /// @brief Main constructor
    BmumuThinningTool(const std::string& t, const std::string& n,
		      const IInterface* p);
    /// @brief Default destructor
    ~BmumuThinningTool();
    /// @brief Initialize tool
    StatusCode initialize();
    /// @brief Finalize tool
    StatusCode finalize();
    /// @brief Main thinning method executed for each event
    virtual StatusCode doThinning() const;

  protected:
    ///
    /// @brief Helper checking for hypothesis passing
    ///
    /// Helper to check whether an element is marked as passing a specific
    /// hypothesis.
    ///
    /// @param[in] em auxillary storage element
    /// @param[in] hypo name of the hypothesis
    /// @returns   true if hypothesis element contains true
    ///
    bool pass(const SG::AuxElement& em, std::string hypo) const;
    ///
    /// @brief Helper to get a TrackParticle link
    ///
    /// @param[in] vtx  secondary vertex containing link
    /// @param[in] name name of the link
    /// @returns   pointer to TrackParticle (NULL if not found)
    ///
    const xAOD::TrackParticle* getTrackParticle(const xAOD::Vertex* vtx,
						std::string name) const;
    ///
    /// @name Thinning mask application
    /// @{
    ///
    /// @brief Apply thinning mask to TrackParticleContainer
    ///
    /// @param[in] trkCont pointer to TrackParticleContainer
    /// @param[in] trkMask vector with mask per track
    /// @param[in] doAnd   use AND instead of OR for mask combinations
    /// @param[in] name    name of TrackParticleContainer
    /// @returns   StatusCode
    ///
    StatusCode applyThinMask(const xAOD::TrackParticleContainer* trkCont,
			     const std::vector<bool>& trkMask,
			     bool doAnd, std::string name) const;
    ///
    /// @brief Apply thinning mask to VertexContainer
    ///
    /// @param[in] vtxCont pointer to VertexContainer
    /// @param[in] vtxMask vector with mask per vertex
    /// @param[in] doAnd   use AND instead of OR for mask combinations
    /// @param[in] name    name of VertexContainer
    /// @returns   StatusCode
    ///
    StatusCode applyThinMask(const xAOD::VertexContainer* vtxCont,
			     const std::vector<bool>& vtxMask,
			     bool doAnd, std::string name) const;
    ///
    /// @brief Apply thinning mask to MuonContainer
    ///
    /// @param[in] muCont  pointer to MuonContainer
    /// @param[in] muMask  vector with mask per muon
    /// @param[in] doAnd   use AND instead of OR for mask combinations
    /// @param[in] name    name of MuonContainer
    /// @returns   StatusCode
    ///
    StatusCode applyThinMask(const xAOD::MuonContainer* muCont,
			     const std::vector<bool>& muMask,
			     bool doAnd, std::string name) const;
    /// @}
    ///

    ///
    /// @brief Mark muons matched to secondary vertices
    ///
    /// @param[in]     muCont      pointer to MuonContainer
    /// @param[in,out] muMask      vector with mask per muon
    /// @param[in]     vtx         secondary vertex
    /// @param[in]     counterName name of counter
    /// @returns       StatusCode
    ///
    StatusCode matchMuons(const xAOD::MuonContainer* muCont,
			  std::vector<bool>& muMask,
			  xAOD::BPhysHelper& vtx,
			  std::string counterName) const;
    ///
    /// @name Sync-mark methods
    ///
    /// @{
    ///
    /// @brief Mark original muons for accepted calibrated muons
    ///
    /// @param[in]     muCont        pointer to MuonContainer of
    ///                              (original) muons
    /// @param[in]     cmuCont       pointer to MuonContainer of
    ///                              calibrated muons
    /// @param[in,out] muMask        mask for (original) muons
    /// @param[in]     cmuMask       mask for calibrated muons
    /// @param[in]     counterName   base name for counters
    /// @param[in]     allowFastSync use fast synchronization method
    /// @returns       StatusCode
    ///
    StatusCode markOrigMuons(const xAOD::MuonContainer* muCont,
			     const xAOD::MuonContainer* cmuCont,
			     std::vector<bool>& muMask,
			     std::vector<bool>& cmuMask,
			     std::string counterName,
			     bool allowFastSync=true) const;
    ///
    /// @brief Mark calibrated muons for accepted (original) muons
    ///
    /// @param[in]     muCont        pointer to MuonContainer of
    ///                              (original) muons
    /// @param[in]     cmuCont       pointer to MuonContainer of
    ///                              calibrated muons
    /// @param[in]     muMask        mask for (original) muons
    /// @param[in,out] cmuMask       mask for calibrated muons
    /// @param[in]     counterName   base name for counters
    /// @param[in]     allowFastSync use fast synchronization method
    /// @returns       StatusCode
    ///
    StatusCode markCalibMuons(const xAOD::MuonContainer* muCont,
			      const xAOD::MuonContainer* cmuCont,
			      std::vector<bool>& muMask,
			      std::vector<bool>& cmuMask,
			      std::string counterName,
			      bool allowFastSync) const;
    ///
    /// @brief Mark ID tracks of selected (original or calibrated) muons
    /// 
    /// @param[in]     trkPartCont   pointer to TrackParticle container
    /// @param[in,out] trkMask       mask for tracks
    /// @param[in]     muCont        pointer to MuonContainer
    /// @param[in]     muMask        mask for muons
    /// @param[in]     counterName   base name for counters
    /// @returns       StatusCode
    ///
    StatusCode markTrksForSelMuons(const xAOD::TrackParticleContainer*
				   trkPartCont,
				   std::vector<bool>& trkMask,
				   const xAOD::MuonContainer* muCont,
				   std::vector<bool>& muMask,
				   std::string counterName) const;
    ///
    /// @brief Mark muons for selected ID tracks
    /// 
    /// @param[in]     trkPartCont   pointer to TrackParticle container
    /// @param[in]     trkMask       mask for tracks
    /// @param[in,out] muCont        pointer to MuonContainer
    /// @param[in]     muMask        mask for muons
    /// @param[in]     counterName   base name for counters
    /// @returns       StatusCode
    ///
    StatusCode markMuonsForSelTracks(const xAOD::TrackParticleContainer*
				     trkPartCont,
				     std::vector<bool>& trkMask,
				     const xAOD::MuonContainer* muCont,
				     std::vector<bool>& muMask,
				     std::string counterName) const;
    /// @}
    ///

    ///
    /// @brief Obtain all auxillary elements matching a certain pattern.
    ///
    /// Helper to filter all names of auxillary elements of an aux container
    /// according to a certain pattern.  The pattern must be a regular
    /// expression pattern.
    ///
    /// @param[in] auxCont pointer to AuxContainer
    /// @param[in] pattern regular expression pattern to be matched by names
    /// @returns   vector<string> of auxillary element names
    ///
    std::vector<std::string>
      filterAuxElements(const xAOD::AuxContainerBase* auxCont,
			std::string pattern) const;
    ///
    /// @brief Determine aux elements to be looked at -- for (refitted) PVs
    ///
    /// @param[in]  auxCont    pointer to AuxContainer
    /// @param[out] vLinkNames vector of aux element names selected
    /// @param[out] vLinkTypes vector of PV-to-SV types corresponding to
    ///                        aux element names selected
    /// @param[in] pattern     regular expression pattern to be matched by names
    ///
    void selectAuxElements(const xAOD::AuxContainerBase* auxCont,
			   std::vector<std::string>& vLinkNames,
			   std::vector<pv_type>&     vLinkTypes,
			   std::string pattern) const;    
    ///
    /// @brief Determine aux elements to be looked at -- for closest tracks
    ///
    /// @param[in]  auxCont    pointer to AuxContainer
    /// @param[out] vLinkNames vector of aux element names selected
    /// @param[in]  vPrefixes  vector of prefixes to be concatenated with
    ///                        pattern for search
    /// @param[out] vLinkTypes vector of PV-to-SV types corresponding to
    ///                        aux element names selected
    /// @param[in] pattern     regular expression pattern to be matched by names
    ///
    void selectAuxElements(const xAOD::AuxContainerBase* auxCont,
			   std::vector<std::string>& vLinkNames,
			   std::vector<std::string>  vPrefixes,
			   std::vector<pv_type>&     vLinkTypes,
			   std::string pattern) const;    
    ///
    /// @brief Dump a vector<str> to a string
    ///
    /// @param[in] vs      vector<string> to be dumped
    /// @param[in] header  header string to be prepended
    /// @param[in] nBlanks number of blanks to prepend each line with
    ///
    std::string dumpVS(const std::vector<std::string>& vs,
		       const std::string header="",
		       size_t nBlanks=0) const;
    ///
    /// @brief Wrap string at line breaks and print with
    ///        appropriate message level
    ///
    /// @param[in] str  string to be printed
    /// @param[in] lvl  MSG::Level chosen
    /// 
    void logWrappedMsg(const std::string& str, const MSG::Level lvl) const;
    ///
    /// @brief Check two masks for consistency
    ///
    /// This is a method returning debugging information.
    ///
    /// @param[in] mask1  first mask vector to be checked
    /// @param[in] mask2  second mask vector to be checked
    /// @param[in] name1  name of first mask vector
    /// @param[in] name2  name of second mask vector
    /// @param[in] header text to be prepended to output string
    /// @returns   string with debugging information
    ///
    std::string checkMaskConsistency(const std::vector<bool>& mask1,
				     const std::vector<bool>& mask2,
				     const std::string name1,
				     const std::string name2,
				     const std::string header="") const;

  private:
    ///
    /// @name Job options
    /// @{
    ServiceHandle<IThinningSvc> m_thinningSvc;
    std::string                 m_trkPartContName;
    std::vector<std::string>    m_vtxContNames;
    std::vector<std::string>    m_vtxPassFlags;
    std::string                 m_PVContName;
    std::vector<std::string>    m_refPVContNames;
    std::string                 m_muonContName;
    std::string                 m_calMuonContName;
    std::string                 m_ctBranchBaseName;
    std::vector<std::string>    m_ctBranchPrefixes;
    bool                        m_alignPassToVertexList;
    bool                        m_keepPVTracks;
    bool                        m_matchCalMuons;
    bool                        m_markMuons;
    bool                        m_markCalMuons;
    bool                        m_syncMuonsBothWays;
    bool                        m_keepCloseTracks;
    bool                        m_keepSelMuonTracks;
    bool                        m_keepSelCalMuonTracks;
    bool                        m_keepSelTrackMuons;
    bool                        m_keepSelTrackCalMuons;
    bool                        m_allowFastMuonMaskSync;
    bool                        m_thinPVs;
    bool                        m_thinRefPVs;
    bool                        m_thinTracks;
    bool                        m_thinMuons;
    bool                        m_vertexAnd;
    bool                        m_trackAnd;
    bool                        m_muonAnd;
    /// @}
    
    ///
    /// @name internal member variables
    ///
    /// process close tracks
    bool m_doCloseTracks;
    /// process primary vertices
    bool m_doPVs;
    /// process refitted primary vertices
    bool m_doRefPVs;
    /// process (original) muons
    bool m_doMuons;
    /// process refitted muons
    bool m_doCalMuons;
    /// process ID tracks
    bool m_doTracks;
    /// @}

    /// 
    /// @name aux element link name caches
    ///
    /// @{
    ///
    /// caching aux element link names (and pv types)
    /// for original and refitted PVs
    ///
    mutable std::vector<std::vector<std::string> > m_vvOrigPVLinkNames;
    mutable std::vector<std::vector<pv_type> >     m_vvOrigPVLinkTypes;
    mutable std::vector<std::vector<std::string> > m_vvRefPVLinkNames;
    mutable std::vector<std::vector<pv_type> >     m_vvRefPVLinkTypes;

    ///
    /// caching aux element link names (and pv types)
    /// for closest tracks
    mutable std::vector<std::vector<std::string> > m_vvCtLinkNames;
    mutable std::vector<std::vector<pv_type> >     m_vvCtLinkTypes;
    /// @}

  };
  
} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORK_BmumuThinningTool_H
