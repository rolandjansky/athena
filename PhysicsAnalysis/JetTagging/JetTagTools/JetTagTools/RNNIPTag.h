/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_RNNIPTAG_H
#define JETTAGTOOLS_RNNIPTAG_H

/******************************************************
    @class RNNIPTag
    b-jet tagger based on 2D or 3D impact parameter
    @author CPPM Marseille
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"

#include "xAODTracking/TrackParticleContainerFwd.h"

#include <vector>
#include <functional>
#include <memory>
#include <sstream>

namespace Reco { class ITrackToVertex; }
namespace Trk  { class ITrackToVertexIPEstimator; }
// following two are temporary (to be moved to a separate Tool)
namespace InDet { class IInDetTrackSelectionTool; }
namespace CP { class ITrackVertexAssociationTool;}

namespace lwt {
  class LightweightRNN;
  struct JSONConfig;
  typedef std::map<std::string, double> ValueMap;
}

namespace Analysis {


  class IPInfo;
  class TrackSelector;
  class TrackGrade;
  class GradedTrack;
  class TrackGradePartition;
  class SVForIPTool;
  class ITrackGradeFactory;
  class CalibrationBroker;

  // track struct, and sorting functions
  struct IPxDInfo;
  typedef std::function<bool(const IPxDInfo&, const IPxDInfo&)> TrackSorter;

  class RNNIPTag : public AthAlgTool , virtual public ITagTool {

  public:
    RNNIPTag(const std::string&,const std::string&,const IInterface*);

    /**
       Implementations of the methods defined in the abstract base class
    */
    virtual ~RNNIPTag();
    StatusCode initialize();
    StatusCode finalize();

    /** Set the primary vertex. TODO: This is temporary ! The primary
  vertex should be part of the JetTag IParticle interface
  implementation. The trouble with ElementLink and persistency has to
  be solved for that. Revisit ... */
    void setOrigin(const xAOD::Vertex* priVtx);

    StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging * BTag, const std::string jetName);

    /** calculate individual track contribution to the three likelihoods: */
    // void trackWeight(std::string jetAuthor, TrackGrade grade,
    //                  double sa0, double sz0, double & twb, double &
    //                  twu, double & twc);

    void finalizeHistos() {};

  private:

    std::vector<IPxDInfo> get_track_info(
      const std::vector<GradedTrack>&,
      const Amg::Vector3D& unit,
      const std::vector<const xAOD::TrackParticle*>& v0_tracks) const;

    void add_tags(xAOD::BTagging& tag, const std::string& author,
                  std::vector<IPxDInfo>& tracks);

    /** base name string for persistification in xaod */
    std::string m_xAODBaseName;

    /// VD: bool switches
    bool m_flipIP;              // reverse impact parameter signs for
                                // negative tag calibration method
    bool m_flipZIP;             // reverse Z impact parameter signs
                                // for negative tag calibration method
    bool m_usePosIP;            // use tracks with positive impact
                                // parameter for tagging
    bool m_useNegIP;            // use tracks with negative impact
                                // parameter for tagging
    bool m_useZIPSignForPosNeg; // use Z impact parameter sign as well
                                // to select Pos and Neg tracks
    bool m_use2DSignForIP3D;    // force to use 2D IP sign even for IP3D IP
    bool m_useD0SignForZ0;      // force to use transverse IP sign for
                                // Z impact parameter for IP3D
    bool m_rejectBadTracks;     // TODO: Do we need this?

    std::string m_tagNameSuffix; // appended to tagger name to indicate
                                 // neg, flip, etc
    // bool m_SignWithSvx;
    bool m_unbiasIPEstimation;  // remove track from vertex when computing IP

    // we can save the sorted inputs for each tagger for debugging and
    // optimization
    bool m_writeInputsToBtagObject;
    bool m_writeTrackLinks;

    //// VD: other (non-bool) configurables
    /** Name of the track-to-jet association in the BTagging object */
    std::string m_trackAssociationName;

    /** forcing the calibration folder of a given collection */
    std::string m_ForcedCalibName;

    // Initalize with a config name and path to a network If the
    // network path is empty we read from the calib db Note that this
    // won't support reading different local files for different jet
    // collections.
    std::map<std::string, std::string > m_network_cfg;

    // At runtime, we can run a list of networks for each track
    // sorting. Networks include the configuration name, which is used
    // for the DB lookup and to name the saved output.
    //
    // This structure also owns the network instance.
    struct Network {
      Network(const std::string& name, const lwt::JSONConfig&);
      std::string name;
      std::shared_ptr<lwt::LightweightRNN> network;
      std::vector<std::string> outputs;
    };
    typedef std::vector<Network> Networks;

    // Each network list is grouped with its sort function.
    typedef std::vector<std::pair<TrackSorter, Networks> > SortGroups;
    std::map<std::string, SortGroups > m_networks;

    // load the calibration file from the COOL db
    void update_networks_for(const std::string& author);
    void register_hist(const std::string& name = "/rnnip");
    std::string get_calib_string(
      const std::string& author,
      const std::string& name = "/rnnip");
    std::string m_calibrationDirectory;
    ToolHandle<CalibrationBroker> m_calibrationTool;

    /** names of fools for getting the secondary vertex information */
    std::string m_secVxFinderNameForV0Removal;
    std::string m_secVxFinderNameForIPSign;
    std::string m_secVxFinderName;

    //// VD: auxiliary information to be stored
    /** information to persistify: */
    std::string m_originalTPCollectionName;
    //const xAOD::TrackParticleContainer* m_originalTPCollection;
    /** track classification. */
    std::vector<std::string>          m_trackGradePartitionsDefinition;
    std::vector<TrackGradePartition*> m_trackGradePartitions;
    /** Storage for the primary vertex. Can be removed when JetTag
     * provides origin(). */
    // this pointer does not need to be deleted in the destructor
    // (because it points to something in storegate)
    const xAOD::Vertex* m_priVtx = 0;

    //// VD: list of tools below
    /** TrackToVertex tool */
    ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;

    /** Track selection cuts for IPTagging */
    ToolHandle< TrackSelector >        m_trackSelectorTool;

    /** Pointer to the SV tool */
    ToolHandle< SVForIPTool >          m_SVForIPTool;

    /** ToolHandle for the ITrackGradeFactory tool */
    ToolHandle< ITrackGradeFactory > m_trackGradeFactory;

    /** GP: Tool for the estimation of the IPs to the Vertex */
    ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;

    // VD: for debugging
    //int m_nbjet;
    //int m_ncjet;
    //int m_nljet;

  }; // End class

  inline void RNNIPTag::setOrigin(const xAOD::Vertex* priVtx) {
    m_priVtx = priVtx;
  }

} // End namespace

#endif
