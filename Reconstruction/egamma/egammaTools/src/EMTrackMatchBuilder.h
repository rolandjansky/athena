/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "egammaBaseTool.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "TrackMatchSorter.h"
#include "GaudiKernel/ToolHandle.h" 
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h" 
#include "TrkEventPrimitives/PropDirection.h"

class egammaRec;
class IEMExtrapolationTools;

namespace Reco  { class ITrackToVertex; }

class EMTrackMatchBuilder : public egammaBaseTool, virtual public IEMTrackMatchBuilder
{

 public:

  /** @brief Default constructor*/
  EMTrackMatchBuilder(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  /** @brief Destructor*/
  ~EMTrackMatchBuilder();
        
  /** @brief Gaudi algorithm hooks*/
  StatusCode initialize();
  /** @brief execute method*/
  virtual StatusCode executeRec(egammaRec* eg);
  /** @brief execute method*/
  virtual StatusCode trackExecute(egammaRec* eg,  const xAOD::TrackParticleContainer * trackPC);

private:

  /** @brief Compute for tracks passing the loose matching
   the distance between track extrapolated to 2nd sampling and cluster */
  bool inBroadWindow(std::vector<TrackMatch>&      trackMatches,
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
  std::string           m_TrackParticlesName; 
  /** @brief broad cut on deltaEta*/
  double                m_broadDeltaEta;
  /** @brief broad cut on deltaPhi*/
  double                m_broadDeltaPhi;
  /** @brief narrow cut on deltaEta*/
  double                m_narrowDeltaEta;
  /** @brief narrow cut on deltaPhiRescale*/
  double                m_narrowDeltaPhi;
  /** @brief narrow cut on deltaPhi for electrons*/
  double                m_narrowDeltaPhiBrem;
  /** @brief narrow cut on deltaPhiRescale*/
  double                m_narrowDeltaPhiRescale;
  /** @brief narrow cut on deltaPhiRescale for electrons*/
  double                m_narrowDeltaPhiRescaleBrem;
  /** @Maximum deltaPhi (Res) allowed for a match */
  double                m_MaxDeltaPhiRescale;
  /** @brief flag to turn on/off use of isCandidateMatch*/
  bool                  m_useCandidateMatch;
  /** @brief flag to either use last measurement hit or perigee */
  bool                  m_useLastMeasurement;
  /** @brief Boolean to favor tracks with Pixel hits*/
  bool                  m_useScoring;
  /** @brief Boolean to use Rescale in the metric*/
  bool m_UseRescaleMetric;
  /** @brief Boolean to do second pass with Rescale*/
  bool m_SecondPassRescale;
  /** @brief TrackToCalo extrapolation tool. Handles Trk::ParametersBase as input.
      Extrapolation starts from the last measurement of the track. The
      InDetExtrapolator is used, with all proper material effects inside the
      part of the ID that is traversed. Both charged and neutral particles
      are handled. */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool;
  /** @brief */
  bool                  m_isCosmics;
  //int                   m_nSi;
  //int                   m_nTrt;
};

#endif









