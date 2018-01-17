/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/RNNIPTag.h"
#include "JetTagTools/LightweightNeuralNetwork.h"
#include "JetTagTools/parse_json.h"
#include "JetTagTools/Exceptions.h"

#include "JetTagTools/TrackSelector.h"
#include "JetTagTools/GradedTrack.h"
#include "JetTagTools/SVForIPTool.h"
#include "JetTagTools/ITrackGradeFactory.h"
#include "JetTagTools/JetTagUtils.h"

#include "JetTagInfo/TrackGrade.h"
#include "JetTagInfo/TrackGradesDefinition.h"
#include "JetTagCalibration/CalibrationBroker.h"

#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "ITrackToVertex/ITrackToVertex.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "GaudiKernel/IToolSvc.h"

// #include "TLorentzVector.h"
#include "TObjString.h"

#include <cmath>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>

// locally used functions and typedefs
namespace {
  using namespace Analysis;
  typedef std::map<std::string, std::vector<double> > VectorMap;
  VectorMap get_nn_inputs(const std::vector<IPxDInfo>&);
  size_t fill_posteriors(xAOD::BTagging&,
                         const std::string& author,
                         const std::vector<std::string>& names,
                         const lwt::ValueMap& = lwt::ValueMap());
  void fill_inputs(xAOD::BTagging&,
                   const std::string& author,
                   const lwt::VectorMap& inputs);

  typedef std::vector<ElementLink<xAOD::TrackParticleContainer> > TrackLinks;

  // keys to lookup track sorting info
  const std::string SORT_BY = "sort_by";
  const std::string UNKNOWN_SORT = "unknown_sort";
  std::string get_sort_name(const lwt::JSONConfig& cfg);
  TrackSorter get_sort_function(const std::string& key);

struct TrackSorterFunctional
{
  TrackSorterFunctional (const TrackSorter& sorter)
    : m_sorter (sorter) {}
  bool operator() (const IPxDInfo& a, const IPxDInfo& b) const
  {
    return m_sorter (a, b);
  }

  const TrackSorter& m_sorter;
};

}

// Track variable names, for both the NN inputs and for writing to the
// xAOD EDM.
namespace trkvar {
  const std::string D0 = "d0";
  const std::string D0_SIG = "d0sig";
  const std::string Z0 = "z0";
  const std::string Z0_SIG = "z0sig";
  const std::string D0Z0_SIG = "d0z0sig";
  const std::string GRADE = "grade";
  const std::string FROM_V0 = "fromV0";
  const std::string PT = "pt";
  const std::string DPHI = "dPhi";
  const std::string ABS_ETA = "absEta";
  const std::string DETA = "dEta";
  const std::string CHARGE = "charge";

  const std::string PT_FRAC = "pTFrac";
  const std::string DR = "dR";

  // hit info
  const std::string CHI2 = "chi2";
  const std::string N_INN_HITS = "nInnHits";
  const std::string N_NEXT_TO_INN_HITS = "nNextToInnHits";
  const std::string N_BL_HITS = "nBLHits";
  const std::string N_SHARED_BL_HITS = "nsharedBLHits";
  const std::string N_SPLIT_BL_HITS = "nsplitBLHits";
  const std::string N_PIX_HITS = "nPixHits";
  const std::string N_SHARED_PIX_HITS = "nsharedPixHits";
  const std::string N_SPLIT_PIX_HITS = "nsplitPixHits";
  const std::string N_SCT_HITS = "nSCTHits";
  const std::string N_SHARED_SCT_HITS = "nsharedSCTHits";
  const std::string EXPECT_BL_HIT = "expectInnermostPixelLayerHit";
  const std::string EXPECT_INNERMOST_PIX_HIT =
    "expectInnermostPixelLayerHit";
  const std::string EXPECT_NEXT_TO_INNERMOST_PIX_HIT =
    "expectNextToInnermostPixelLayerHit";
}

namespace Analysis {
  struct IPxDInfo {
    ElementLink<xAOD::TrackParticleContainer> trk;
    double d0;
    double d0sig;
    double z0;
    double z0sig;
    double dphi;
    double ptfrac;
    double dr;
    double deta;
    TrackGrade grade;
    const xAOD::TrackParticle* trkP;
    bool fromV0;
  } ;

  bool StructPTsorting(const IPxDInfo& objA, const IPxDInfo& objB) {
    return (objA.trkP)->pt() > (objB.trkP)->pt();
  }

  bool StructD0Sigsorting(const IPxDInfo& objA, const IPxDInfo& objB) {
    return fabs(objA.d0sig) > fabs(objB.d0sig);
  }

  bool StructZ0D0Sigsorting(const IPxDInfo& objA, const IPxDInfo& objB) {
    double a = pow(objA.d0sig,2)+pow(objA.z0sig,2);
    double b = pow(objB.d0sig,2)+pow(objB.z0sig,2);
    return a > b;
  }


  /**
      @class RNNIPTag
      b-jet tagger based on 2D or 3D impact parameter
      @author CPPM Marseille
  */


  RNNIPTag::RNNIPTag(const std::string& t,
                     const std::string& n,
                     const IInterface* p)
    : AthAlgTool(t,n,p),
      //m_secVxFinderNameForV0Removal("InDetVKalVxInJetTool"),
      //m_secVxFinderNameForIPSign("InDetVKalVxInJetTool"),
      m_unbiasIPEstimation(true),
      m_calibrationDirectory("RNNIP"),
      m_calibrationTool("Analysis::CalibrationBroker"),
      m_secVxFinderName("InDetVKalVxInJetTool"),
      m_trackToVertexTool("Reco::TrackToVertex"),
      m_trackSelectorTool("Analysis::TrackSelector"),
      m_SVForIPTool("Analysis::SVForIPTool"),
      m_trackGradeFactory("Analysis::BasicTrackGradeFactory")
  {

    declareInterface<ITagTool>(this);

    // global configuration:
    declareProperty("xAODBaseName"  , m_xAODBaseName);

    declareProperty("flipIPSign"          , m_flipIP              = false);
    declareProperty("flipZIPSign"         , m_flipZIP             = false);
    declareProperty("usePosIP"            , m_usePosIP            = true);
    declareProperty("useNegIP"            , m_useNegIP            = true);
    declareProperty("useZIPSignForPosNeg" , m_useZIPSignForPosNeg = false);
    declareProperty("use2DSignForIP3D"    , m_use2DSignForIP3D    = false);
    declareProperty("useD0SignForZ0"      , m_useD0SignForZ0      = false);
    declareProperty("RejectBadTracks"     , m_rejectBadTracks     = true);
    declareProperty("unbiasIPEstimation"  , m_unbiasIPEstimation);
    declareProperty("writeInputsToBtagObject",
                    m_writeInputsToBtagObject = false);
    declareProperty("writeTrackLinks",
                    m_writeTrackLinks = false);

    declareProperty("trackAssociationName"    ,
                    m_trackAssociationName = "BTagTrackToJetAssociator");
    declareProperty("originalTPCollectionName",
                    m_originalTPCollectionName = "InDetTrackParticles");
    declareProperty("ForcedCalibrationName"   , m_ForcedCalibName = "");
    declareProperty("NetworkConfig"           , m_network_cfg);

    declareProperty("trackGradePartitions"    ,
                    m_trackGradePartitionsDefinition);
    m_trackGradePartitionsDefinition.push_back("Good");

    declareProperty("SecVxFinderName"         ,m_secVxFinderName);

    // tools:
    declareProperty("trackSelectorTool"         , m_trackSelectorTool     );
    declareProperty("SVForIPTool"               , m_SVForIPTool           );
    declareProperty("trackGradeFactory"         , m_trackGradeFactory      );
    declareProperty("TrackToVertexIPEstimator"  , m_trackToVertexIPEstimator);
    declareProperty("calibration_directory", m_calibrationDirectory);
    declareProperty("calibrationTool", m_calibrationTool);
  }


  RNNIPTag::~RNNIPTag() {
    for (size_t i = 0; i < m_trackGradePartitions.size(); i++)
      delete m_trackGradePartitions[i];

    // Networks are owned by the structure they are created in
  }


  StatusCode RNNIPTag::initialize() {

    /** retrieving TrackToVertex: */
    /*if ( m_trackToVertexTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " <<
      m_trackToVertexTool); return StatusCode::FAILURE; } else {

    }*/
    // FF: comment out V0 finding
    if (m_SVForIPTool.retrieve().isFailure() )  {
       ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_SVForIPTool);
       return StatusCode::FAILURE;
    } else {
       ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_SVForIPTool);
    }
    if (m_trackToVertexIPEstimator.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " <<
                    m_trackToVertexIPEstimator);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackToVertexIPEstimator);
    }


    /** creation of TrackSelector: (private instance) */
    if ( m_trackSelectorTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackSelectorTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackSelectorTool);
    }

    /** retrieving the track grade factory */
    if ( m_trackGradeFactory.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackGradeFactory);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackGradeFactory);
    }

    /** prepare the track partitions: */
    int nbPart = m_trackGradePartitionsDefinition.size();
    ATH_MSG_INFO("#BTAG# Defining " << nbPart <<" track partitions: ");
    for(int i=0;i<nbPart;i++) {
      TrackGradePartition* part(0);
      try {
        part = new TrackGradePartition(m_trackGradePartitionsDefinition[i],
                                       *m_trackGradeFactory);
      }
      catch (std::string error) {
        ATH_MSG_ERROR("#BTAG# Reported error " << error);
        ATH_MSG_ERROR("#BTAG# List of categories provided to IPTag by jO : ");
        for (int l=0;l<nbPart;l++) {
          ATH_MSG_ERROR(
            "#BTAG#  string " << m_trackGradePartitionsDefinition[l]);
        }
        ATH_MSG_ERROR("#BTAG# List of categories provided by the"
                      " TrackGradeFactory " << m_trackGradeFactory);

        if (msgLvl(MSG::ERROR)) {
          const auto& grades = m_trackGradeFactory->getTrackGradesDefinition();
          for (const auto& grade: grades.getList() ) {
            ATH_MSG_ERROR("#BTAG# n. " << grade.gradeNumber() <<
                          " string " << grade.gradeString());
          }
        }
        ATH_MSG_ERROR("#BTAG# Terminating now... ");
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO((*part));
      m_trackGradePartitions.push_back(part);
    }

    for (const auto& rnn_name_pair: m_network_cfg) {
      if (rnn_name_pair.second.size() == 0) {
        ATH_MSG_VERBOSE("registering RNN " << rnn_name_pair.first);
        register_hist(rnn_name_pair.first);
      }
    }


    return StatusCode::SUCCESS;
  }


  StatusCode RNNIPTag::finalize() {
    return StatusCode::SUCCESS;
  }


  StatusCode RNNIPTag::tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag) {

    /** author to know which jet algorithm: */
    std::string author = JetTagUtils::getJetAuthor(jetToTag);
    if (m_ForcedCalibName.size() > 0) author = m_ForcedCalibName;
    ATH_MSG_VERBOSE("#BTAG# Using jet type " << author << " for calibrations");

    std::vector<GradedTrack> tracksInJet;
    int nbPart = m_trackGradePartitionsDefinition.size();

    std::vector<const xAOD::TrackParticle*> TrkFromV0;
    // Amg::Vector3D SvxDirection;
    // bool canUseSvxDirection=false;

    // if (m_SignWithSvx) {
    //   m_SVForIPTool->getDirectionFromSecondaryVertexInfo(
    //     SvxDirection,canUseSvxDirection,//output
    //     jetToTag,BTag,m_secVxFinderName,*m_priVtx);//input
    //   // jetToTag,BTag,m_secVxFinderNameForIPSign,*m_priVtx);//input
    // }

    // bad tracks from V0s, conversions, interactions:
    m_SVForIPTool->getTrkFromV0FromSecondaryVertexInfo(
      TrkFromV0,//output
      jetToTag,BTag,m_secVxFinderName);//input

    ATH_MSG_VERBOSE("#BTAG# VALERIO TrkFromV0 : number of reconstructed"
                    " bad tracks: " << TrkFromV0.size());
    if (TrkFromV0.size()!=0)  ATH_MSG_DEBUG(
      "#BTAG# TrkFromV0 : number of reconstructed bad tracks: " <<
      TrkFromV0.size());

    /** extract the TrackParticles from the jet and apply track selection: */
    int nbTrak = 0;
    m_trackSelectorTool->primaryVertex(m_priVtx->position());
    m_trackSelectorTool->prepare();

    const auto& associationLinks = BTag->auxdata<TrackLinks>(
      m_trackAssociationName);
    double sumTrkpT = 0;
    if( associationLinks.size() == 0 ) {
      ATH_MSG_VERBOSE("#BTAG#  Could not find tracks associated with"
                      " BTagging object as " << m_trackAssociationName);
    } else {

      // We loop over the tracks twice: the first time is to compute
      // the summed pt of all tracks satisfying the "normal" criteria;
      // the second time a possibly tighter pt cut may be applied
      for(const auto& trk: associationLinks) {
        const xAOD::TrackParticle* aTemp = *trk;
        if (m_trackSelectorTool->selectTrack(aTemp)) sumTrkpT += aTemp->pt();
      }
      // m_trackSelectorTool->setSumTrkPt(sumTrkpT);

      for(const auto& trk: associationLinks) {
        const xAOD::TrackParticle* aTemp = *trk;
        nbTrak++;
        if( m_trackSelectorTool->selectTrack(aTemp, sumTrkpT) ) {
          TrackGrade* theGrade = m_trackGradeFactory->getGrade(
            *aTemp, jetToTag.p4() );
          ATH_MSG_VERBOSE("#BTAG#  result of selectTrack is OK, grade= "
                          << theGrade->gradeString() );
          bool tobeUsed = false;
          for(int i=0;i<nbPart;i++) {
            if( std::find( (m_trackGradePartitions[i]->grades()).begin(),
                           (m_trackGradePartitions[i]->grades()).end(),
                           *theGrade )
                != (m_trackGradePartitions[i]->grades()).end() ){
              tobeUsed = true;
            }
          }
          // is it a bad track ?
          if( std::find(TrkFromV0.begin(),TrkFromV0.end(),aTemp)
              != TrkFromV0.end() ) {
            ATH_MSG_VERBOSE("#BTAG# Bad track in jet, pt = " << aTemp->pt() <<
                            " eta = " << aTemp->eta() << " phi = " <<
                            aTemp->phi() );
            if (m_rejectBadTracks) tobeUsed = false;
          }
          // check required IP sign:
          if (tobeUsed) tracksInJet.push_back(GradedTrack(trk, *theGrade));
          delete theGrade;
          theGrade=0;
        }
      }
    }

    ATH_MSG_VERBOSE("#BTAG# #tracks = " << nbTrak);
    ATH_MSG_VERBOSE("#BTAG# the z of the primary = " <<
                    m_priVtx->position().z());

    /** jet direction: */
    Amg::Vector3D jetDirection(jetToTag.px(),jetToTag.py(),jetToTag.pz());

    /** prepare vectors with all track information: TP links,
     * i.p. significances, track grade, etc */
    auto track_info = get_track_info(tracksInJet, jetDirection, TrkFromV0);

    // add the tags
    add_tags(*BTag, author, track_info);

    if (m_writeTrackLinks) {
      TrackLinks ip_tracks;
      for (const auto& trk: track_info) {
        ip_tracks.push_back( trk.trk );
      }

      // specific fast accessors for mainstream instances of IPTag: IP3D, IP2D
      //// need to handle to persistify for dynamic values before adding this
      BTag->setVariable<TrackLinks> (
        m_xAODBaseName, "TrackParticleLinks", ip_tracks );
      BTag->setDynTPELName( m_xAODBaseName, "TrackParticleLinks");
    }

    return StatusCode::SUCCESS;
  }

  // ___________________________________________________________
  // apply tags loop
  void RNNIPTag::add_tags(xAOD::BTagging& tag, const std::string& author,
                          std::vector<IPxDInfo>& track_info) {

    // check the calibraiton database for networks
    update_networks_for(author);

    // first loop is over the different sorting functions
    for (const auto& sort_group: m_networks.at(author)) {
      const auto& sort_func = sort_group.first;
      const auto& networks = sort_group.second;
      // This doesn't compile with clang 3.8 and gcc 6.1
      //std::sort( track_info.begin(), track_info.end(), sort_func);
      TrackSorterFunctional sorter (sort_func);
      std::sort( track_info.begin(), track_info.end(), sorter);
      const auto inputs = get_nn_inputs(track_info);

      // inner loop is over the networks that use this sort function
      for (const auto& network: networks) {
        if (track_info.size() == 0) {
          ATH_MSG_DEBUG("no tracks, filling with defaults");
          fill_posteriors(tag, network.name, network.outputs);
          continue;
        }
        try {
          lwt::ValueMap out = network.network->reduce(inputs);
          fill_posteriors(tag, network.name, network.outputs, out);
          if (msgLvl(MSG::DEBUG)) {
            std::string op = "output for " + network.name + ": ";
            for (const auto& pr: out) {
              op.append(pr.first + " = " + std::to_string(pr.second) + ", ");
            }
            ATH_MSG_DEBUG(op);
          }
        } catch (lwt::NNEvaluationException& e) {
          ATH_MSG_WARNING(e.what() << " tagging with " + network.name
                          + ", will fill with defaults");
          fill_posteriors(tag, network.name, network.outputs);
        }
      } // end loop over networks
      // for each sorting, store inputs
      if (networks.size() > 0) {
        if (m_writeInputsToBtagObject) {
          fill_inputs(tag, networks.front().name, inputs);
        }
      } else {
        ATH_MSG_ERROR("Found sorting function in RNNIP with no accompanying"
                      " inputs. This should not happen.");
      }
    } // end loop over sorting functions

  }

  RNNIPTag::Network::Network(const std::string& net_name,
                             const lwt::JSONConfig& cf):
    name(net_name),
    network(new lwt::LightweightRNN(cf.inputs, cf.layers, cf.outputs)),
    outputs(cf.outputs)
  {
  }


  // _____________________________________________________________________
  // Calibration stuff
  void RNNIPTag::register_hist(const std::string& file_name) {
    m_calibrationTool->registerHistogram(m_calibrationDirectory, file_name);
    ATH_MSG_DEBUG(" #BTAG# Registered NN histograms with directory: " <<
		  m_calibrationDirectory);

    m_calibrationTool->printStatus();
  }

  std::string RNNIPTag::get_calib_string(const std::string& author,
                                         const std::string& name)
  {
    // if we have an override we ignore the author and just read from
    // a text file
    if (m_network_cfg.at(name).size() > 0) {
      ATH_MSG_DEBUG("Reading from local file");
      std::ifstream file(m_network_cfg.at(name));
      std::stringstream out;
      out << file.rdbuf();
      return out.str();
    } else {
      ATH_MSG_DEBUG("reading out from DB");
    }
    const auto stringpair = m_calibrationTool->retrieveTObject<TObject>(
      m_calibrationDirectory, author, name);
    if (stringpair.second == true) {
      m_calibrationTool->updateHistogramStatus(
        m_calibrationDirectory, author, name, false);
    }
    TObjString* cal_string = dynamic_cast<TObjString*>(stringpair.first);

    if (cal_string == 0){  //catch if no string was found
      std::string fuller_name = m_calibrationDirectory + "/" + author +
        "/" + name;
      if (stringpair.first) {
        fuller_name.append(" [but an object was found]");
      }
      ATH_MSG_WARNING("can't retreve calibration: " + fuller_name);
      return std::string();
    }
    std::string calibration(cal_string->GetString().Data());
    return calibration;
  }



  void RNNIPTag::update_networks_for(const std::string& author) {
    using std::map;
    using std::string;
    using std::pair;
    using std::vector;
    using namespace lwt;

    // TODO: delete networks if they've gone out of the IOV, needs
    // some calibration broker code. On the other hand, we've never
    // had an IOV end within a job. Will we?
    if (m_networks.count(author)) return;

    // order the networks we need to build by the sort function
    typedef map<string, vector<pair<string, JSONConfig > > > NWBySort;
    NWBySort nw_by_sort_func;
    for (const auto& cfg: m_network_cfg) {
      const auto& cfg_name = cfg.first;
      std::string calstring = get_calib_string(author, cfg_name);
      if (calstring.size() == 0) {
        ATH_MSG_WARNING("cant load '" + cfg_name + "' for '" + author + "'");
        continue;
      }
      ATH_MSG_DEBUG("calstring for " + author + ", " + cfg_name + ": "
                    << calstring.size() << " characters");
      std::stringstream stream(calstring);
      try {
        lwt::JSONConfig nncfg = lwt::parse_json(stream);
        auto sort_name = get_sort_name(nncfg);
        nw_by_sort_func[sort_name].emplace_back(cfg_name, nncfg);
      } catch (std::exception& e) {
        ATH_MSG_WARNING(e.what() << ", will not run '" + cfg_name
                        + "' for '" + author + "' jets");
      }
    }

    // build the groups for this author
    m_networks[author] = SortGroups();
    for (const auto& sf_nw: nw_by_sort_func) {
      // TODO: add more sort func
      TrackSorter sort = get_sort_function(sf_nw.first);
      m_networks.at(author).emplace_back(sort, Networks());
      for (const auto& name_and_network_conf: sf_nw.second) {
        const auto& nw_config = name_and_network_conf.second;
        const auto& nw_name = name_and_network_conf.first;
        ATH_MSG_DEBUG(nw_name + ": " << nw_config.layers.size() << " layers");
        auto& nw_group = m_networks.at(author).back().second;
        try {
          nw_group.emplace_back(nw_name, nw_config);
        } catch (lwt::NNConfigurationException& e) {
          ATH_MSG_WARNING(nw_name + " init failure: " + e.what());
        }
      }
    }
  }

  // ____________________________________________________________
  // Get track info vector

  std::vector<IPxDInfo> RNNIPTag::get_track_info(
    const std::vector<GradedTrack>& tracksInJet,
    const Amg::Vector3D& jetDirection,
    const std::vector<const xAOD::TrackParticle*>& TrkFromV0) const {
    Amg::Vector3D unit = jetDirection.unit();
    double jet_pt = std::hypot(jetDirection.x(), jetDirection.y());

    std::vector<IPxDInfo> track_info;

    for (std::vector<GradedTrack>::const_iterator trkItr = tracksInJet.begin();
         trkItr != tracksInJet.end(); ++trkItr) {

      const xAOD::TrackParticle* trk = *(trkItr->track);
      const auto& trk4 = trk->p4();
      const Amg::Vector3D trk_momentum(trk4.Px(), trk4.Py(), trk4.Pz());

      bool isFromV0 = (
        std::find(TrkFromV0.begin(),TrkFromV0.end(),trk) != TrkFromV0.end());

      /** track parameters defined at the primary vertex: */
      double d0wrtPriVtx(0.);
      double z0wrtPriVtx(0.);
      double d0ErrwrtPriVtx(1.);
      double z0ErrwrtPriVtx(1.);

      /** use new Tool for "unbiased" IP estimation */
      const Trk::ImpactParametersAndSigma* myIPandSigma(0);
      if (m_trackToVertexIPEstimator) {
        myIPandSigma = m_trackToVertexIPEstimator->estimate(
          trk, m_priVtx, m_unbiasIPEstimation);
      }
      if(0==myIPandSigma) {
        ATH_MSG_WARNING("#BTAG# IPTAG: trackToVertexIPEstimator failed !");
      } else {
        d0wrtPriVtx=myIPandSigma->IPd0;
        d0ErrwrtPriVtx=myIPandSigma->sigmad0;
        z0wrtPriVtx=myIPandSigma->IPz0SinTheta;
        z0ErrwrtPriVtx=myIPandSigma->sigmaz0SinTheta;
        delete myIPandSigma;
        myIPandSigma=0;
      }

      /** sign of the impact parameter */
      double signOfIP(1.);
      if ( m_use2DSignForIP3D ) {
        signOfIP=m_trackToVertexIPEstimator->get2DLifetimeSignOfTrack(
          trk->perigeeParameters(),unit, *m_priVtx);
      } else {
        signOfIP=m_trackToVertexIPEstimator->get3DLifetimeSignOfTrack(
          trk->perigeeParameters(), unit, *m_priVtx);
      }
      double signOfZIP = m_trackToVertexIPEstimator->getZLifetimeSignOfTrack(
        trk->perigeeParameters(), unit, *m_priVtx);

      if (m_useD0SignForZ0) signOfZIP = signOfIP;

      /** option to flip signs for negative tag method */
      if(m_flipIP  )  signOfIP = -signOfIP;
      if(m_flipZIP ) signOfZIP = -signOfZIP;

      /** select according to sign: skip tracks if not ok */
      if(signOfIP>0 && !m_usePosIP) continue;
      if(signOfIP<0 && !m_useNegIP) continue;
      if( m_useZIPSignForPosNeg ) {
        if(signOfZIP>0 && !m_usePosIP) continue;
        if(signOfZIP<0 && !m_useNegIP) continue;
      }

      /** significances */
      double sIP         = signOfIP*fabs(d0wrtPriVtx);
      double significance= signOfIP*fabs(d0wrtPriVtx/d0ErrwrtPriVtx);
      double szIP        = signOfZIP*fabs(z0wrtPriVtx);
      double z0Sig       = signOfZIP*fabs(z0wrtPriVtx/z0ErrwrtPriVtx);

      // VD: I know that this is ugly but I want to minimise the
      // changes as much as I can
      IPxDInfo tmpObj;
      tmpObj.trk=trkItr->track;
      tmpObj.d0   =sIP;
      tmpObj.d0sig=significance;
      tmpObj.z0   =szIP;
      tmpObj.z0sig=z0Sig;
      tmpObj.grade=trkItr->grade;
      tmpObj.trkP =trk;
      tmpObj.fromV0=isFromV0;
      tmpObj.dphi = Amg::deltaPhi(trk_momentum, unit);
      tmpObj.dr = Amg::deltaR(trk_momentum, unit);
      tmpObj.deta = trk_momentum.eta() - unit.eta();
      tmpObj.ptfrac = trk->pt() / jet_pt;
      track_info.push_back(tmpObj);
    }
    return track_info;
  }

}//end namespace

namespace {
  double val(const xAOD::TrackParticle& tp, const xAOD::SummaryType& prop) {
    uint8_t val;
    tp.summaryValue(val, prop);
    return val;
  }

  VectorMap get_nn_inputs(const std::vector<IPxDInfo>& tracks) {
    using namespace trkvar;
    const std::vector<std::string> inputs{
      D0, D0_SIG, Z0, Z0_SIG, D0Z0_SIG, GRADE, FROM_V0, PT, DPHI, ABS_ETA,
        PT_FRAC, DR, DETA, CHARGE,
        CHI2, N_INN_HITS, N_NEXT_TO_INN_HITS, N_BL_HITS, N_SHARED_BL_HITS,
        N_SPLIT_BL_HITS, N_PIX_HITS, N_SHARED_PIX_HITS, N_SPLIT_PIX_HITS,
        N_SCT_HITS, N_SHARED_SCT_HITS, EXPECT_BL_HIT,
        EXPECT_INNERMOST_PIX_HIT, EXPECT_NEXT_TO_INNERMOST_PIX_HIT};
    VectorMap out;
    for (const auto& input: inputs) {
      out.emplace(input, std::vector<double>());
    }
    for (const auto& tk: tracks) {
      out.at(D0).push_back(tk.d0);
      out.at(D0_SIG).push_back(tk.d0sig);
      out.at(Z0).push_back(tk.z0);
      out.at(Z0_SIG).push_back(tk.z0sig);
      out.at(D0Z0_SIG).push_back(std::hypot(tk.z0sig, tk.d0sig));
      out.at(GRADE).push_back(tk.grade);
      out.at(FROM_V0).push_back(tk.fromV0);
      out.at(PT).push_back(tk.trkP->pt());
      out.at(DPHI).push_back(tk.dphi);
      out.at(ABS_ETA).push_back(std::abs(tk.trkP->eta()));
      out.at(PT_FRAC).push_back(tk.ptfrac);
      out.at(DR).push_back(tk.dr);
      out.at(DETA).push_back(tk.deta);
      out.at(CHARGE).push_back(tk.trkP->charge());

      const auto& tp = *tk.trkP;
      out.at(CHI2).push_back(tp.chiSquared());
      out.at(N_INN_HITS).push_back(
        val(tp,xAOD::numberOfInnermostPixelLayerHits));
      out.at(N_NEXT_TO_INN_HITS).push_back(
        val(tp, xAOD::numberOfNextToInnermostPixelLayerHits));
      out.at(N_BL_HITS).push_back(
        val(tp, xAOD::numberOfInnermostPixelLayerHits));
      out.at(N_SHARED_BL_HITS).push_back(
        val(tp, xAOD::numberOfInnermostPixelLayerSharedHits));
      out.at(N_SPLIT_BL_HITS).push_back(
        val(tp, xAOD::numberOfInnermostPixelLayerSplitHits));
      out.at(N_PIX_HITS).push_back(
        val(tp, xAOD::numberOfPixelHits));
      out.at(N_SHARED_PIX_HITS).push_back(
        val(tp, xAOD::numberOfPixelSharedHits));
      out.at(N_SPLIT_PIX_HITS).push_back(
        val(tp, xAOD::numberOfPixelSplitHits));
      out.at(N_SCT_HITS).push_back(
        val(tp, xAOD::numberOfSCTHits));
      out.at(N_SHARED_SCT_HITS).push_back(
        val(tp, xAOD::numberOfSCTSharedHits));
      out.at(EXPECT_BL_HIT).push_back(
        val(tp, xAOD::expectInnermostPixelLayerHit));
      out.at(EXPECT_INNERMOST_PIX_HIT).push_back(
        val(tp, xAOD::expectInnermostPixelLayerHit));
      out.at(EXPECT_NEXT_TO_INNERMOST_PIX_HIT).push_back(
        val(tp, xAOD::expectNextToInnermostPixelLayerHit));
    }
    return out;
  }

  size_t fill_posteriors(xAOD::BTagging& tag,
                         const std::string& author,
                         const std::vector<std::string>& outputs,
                         const lwt::ValueMap& vals) {
    size_t found = 0;
    for (const auto& out: outputs) {
      double val = 0;         // was NaN, see jira: AFT-194, ATLASRECTS-3897
      bool is_valid = false;
      if (vals.count(out)) {
        val = vals.at(out);
        found++;
        is_valid = true;
      }
      tag.setVariable<double>(author, out, val);
      std::string valid_key = out + "IsValid";
      tag.setVariable<char>(author, valid_key, is_valid);
    }
    return found;
  }
  void fill_inputs(xAOD::BTagging& tag,
                   const std::string& author,
                   const lwt::VectorMap& inputs) {
    // cast to float to save space
    typedef std::vector<float> OutVector;
    for (const auto& in: inputs) {
      OutVector outvec(in.second.begin(), in.second.end());
      tag.setVariable<OutVector>(author, in.first, outvec);
    }
  }

  std::string get_sort_name(const lwt::JSONConfig& cfg) {
    if (!cfg.miscellaneous.count(SORT_BY)) {
      throw std::logic_error("sort function not given");
    }
    return cfg.miscellaneous.at(SORT_BY);
  }

  Analysis::TrackSorter get_sort_function(const std::string& key) {
    using namespace trkvar;
    using namespace Analysis;
    if (key == D0_SIG) return StructD0Sigsorting;
    if (key == PT) return StructPTsorting;
    if (key == D0Z0_SIG) return StructZ0D0Sigsorting;
    throw std::logic_error("sort by " + key + " is not defined");
  }

}
