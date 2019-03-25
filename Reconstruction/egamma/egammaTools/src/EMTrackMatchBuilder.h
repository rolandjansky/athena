/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMTRACKMATCHBUILDER_H
#define EGAMMATOOLS_EMTRACKMATCHBUILDER_H

/**
  @class EMTrackMatchBuilder
          EMTrackMatch data object builder.
  @author H. Ma
  @author RD schaffer
  @author Thomas Koffas
  @author Christos Anastopoulos
The matching of a track to a cluster is driven by the EMTrackMatchBuilder tool located in the Reconstruction/egamma/egammaTools package. 
*/

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgTool.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "GaudiKernel/ToolHandle.h" 
#include "GaudiKernel/EventContext.h"

#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h" 
#include "TrkEventPrimitives/PropDirection.h"
#include "StoreGate/ReadHandleKey.h"

class egammaRec;

namespace Reco  { class ITrackToVertex; }

class EMTrackMatchBuilder : public AthAlgTool, virtual public IEMTrackMatchBuilder
{

 public:

  /** @brief Default constructor*/
  EMTrackMatchBuilder(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  /** @brief Destructor*/
  ~EMTrackMatchBuilder();
        
  /** @brief Gaudi algorithm hooks*/
  StatusCode initialize() override;
  /** @brief execute method*/
  virtual StatusCode executeRec(const EventContext& ctx, egammaRec* eg) const override final;
  /** @brief execute method*/
  virtual StatusCode trackExecute(const EventContext& ctx, egammaRec* eg,  const xAOD::TrackParticleContainer * trackPC) const override final;

private:

  /** @brief A structure for keeping track match information */
  struct TrackMatch
  {
  public:
    int trackNumber;
    double dR;
    double seconddR;
    bool isTRT;
    int score;
    int hitsScore;
    double deltaPhiLast;
    double deltaEta[4];
    double deltaPhi[4];
    double deltaPhiRescaled[4];
  };

  /** @brief function to sort track matches based on quality */
  static bool TrackMatchSorter(const TrackMatch& match1, const TrackMatch& match2);

  /** @brief Compute for tracks passing the loose matching
   the distance between track extrapolated to 2nd sampling and cluster */
  bool inBroadWindow(const EventContext& ctx,
                     std::vector<TrackMatch>&      trackMatches,
                     const xAOD::CaloCluster*      cluster, 
                     int                           trackNumber,
                     bool                          isTRT,
                     const xAOD::TrackParticle*     trkPB,
                     const Trk::PropDirection      dir) const;

  /** @brief Loose track-cluster matching */
//   bool isCandidateMatch(const CaloCluster&  cluster,
//                      bool                isTRT,
//                      const Trk::Perigee* candidatePerigee,
//                      bool                flip) const;
  bool isCandidateMatch(const xAOD::CaloCluster*  cluster,
                        bool                      isTRT,
                        const xAOD::TrackParticle* track,
                        bool                       flip) const;
   
  
  // configuration:
  /** @brief name of TrackParticle container in TDS*/
  SG::ReadHandleKey<xAOD::TrackParticleContainer>  m_TrackParticlesKey { this,
      "TrackParticlesName", "",
      "Name of the input track particle container"};
 
  /** @brief broad cut on deltaEta*/
  Gaudi::Property<double> m_broadDeltaEta {this,
      "broadDeltaEta", 0.1, "Value of broad cut for delta eta"};

  /** @brief broad cut on deltaPhi*/
  Gaudi::Property<double> m_broadDeltaPhi {this,
      "broadDeltaPhi", 0.1, "Value of broad cut for delta phi"};

  /** @brief narrow cut on deltaEta*/
  Gaudi::Property<double> m_narrowDeltaEta {this,
      "narrowDeltaEta", 0.05,
      "Value of narrow cut for delta eta"};

  /** @brief narrow cut on deltaPhiRescale*/
  Gaudi::Property<double> m_narrowDeltaPhi {this,
      "narrowDeltaPhi", 0.05,
      "Value of the narrowd cut for delta phi"};

  /** @brief narrow cut on deltaPhi for electrons*/
  Gaudi::Property<double> m_narrowDeltaPhiBrem {this,
      "narrowDeltaPhiBrem", 0.1,
      "Value of the narrow cut for delta phi  Brem"};

  /** @brief narrow cut on deltaPhiRescale*/
  Gaudi::Property<double> m_narrowDeltaPhiRescale {this,
      "narrowDeltaPhiRescale",  0.05,
      "Value of the narrow cut for delta phi Rescale"};

  /** @brief narrow cut on deltaPhiRescale for electrons*/
  Gaudi::Property<double> m_narrowDeltaPhiRescaleBrem {this,
      "narrowDeltaPhiRescaleBrem", 0.1,
      "Value of the narrow cut for delta phi Rescale Brem"};

  /** @Maximum deltaPhi (Res) allowed for a match */
  Gaudi::Property<double> m_MaxDeltaPhiRescale {this,
      "MaxDeltaPhiRescale", 0.25,
      "Maximum Value of the deltaPhi rescale"};

  /** @brief flag to turn on/off use of isCandidateMatch*/
  Gaudi::Property<bool> m_useCandidateMatch {this,
      "useCandidateMatch", true,
      "Boolean to use candidate matching"};

  /** @brief flag to either use last measurement hit or perigee */
  Gaudi::Property<bool> m_useLastMeasurement {this,
      "useLastMeasurement", false,
      "Boolean to use last measurement for extrapolation, otherwise use perigee"};

  /** @brief Boolean to favor tracks with Pixel hits*/
  Gaudi::Property<bool> m_useScoring {this,
      "useScoring", true,
      "Boolean to favor tracks with Pixel hits"};

  /** @brief Boolean to use Rescale in the metric*/
  Gaudi::Property<bool> m_UseRescaleMetric {this,
      "UseRescaleMetric", true, "Use Rescale Metric"};

  /** @brief Boolean to do second pass with Rescale*/
  Gaudi::Property<bool> m_SecondPassRescale {this,
      "SecondPassRescale", true, "Do second pass with rescale"};

  /** @brief TrackToCalo extrapolation tool. Handles Trk::ParametersBase as input.
      Extrapolation starts from the last measurement of the track. The
      InDetExtrapolator is used, with all proper material effects inside the
      part of the ID that is traversed. Both charged and neutral particles
      are handled. */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool {this,
      "ExtrapolationTool", "EMExtrapolationTools",
      "Name of the extrapolation tool"};

  /** @brief */
  Gaudi::Property<bool> m_isCosmics {this, 
      "isCosmics", false, "Boolean for use of cosmics"};
};

#endif









