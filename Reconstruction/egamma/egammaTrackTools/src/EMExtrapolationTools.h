/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATRACKTOOLS_EMEXTRAPOLATIONTOOLS_H
#define EGAMMATRACKTOOLS_EMEXTRAPOLATIONTOOLS_H
/** 
  @class EMExtrapolationTools
          Tools for track extrapolation to the calorimeter
  @author Thomas Koffas
*/

/********************************************************************

NAME:     EMExtrapolationTools.h
PACKAGE:  offline/Reconstruction/egammaTrackTools

AUTHORS:  T. Koffas
CREATED:  Mar , 2009

PURPOSE:  Tool which propagate track to 
             - calorimeter
             - cluster
UPDATED:
********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"

#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"

class IExtrapolateToCaloTool;
class TRT_ID;
class CaloDepthTool;

namespace Trk
{
  class INeutralParticleParameterCalculator;
}

class EMExtrapolationTools : virtual public IEMExtrapolationTools, public AthAlgTool {

 public:

  /** @brief Constructor with parameters */
  EMExtrapolationTools (const std::string& type,const std::string& name, const IInterface* parent);
  /** @brief Destructor */
  virtual ~EMExtrapolationTools();

  /** @brief initialize method */
  virtual StatusCode initialize();
  /** @brief finalize method */
  virtual StatusCode finalize();

  /**  test for cluster/extrapolated track match, from xAOD::TrackParticle */
  virtual bool  matchesAtCalo(const xAOD::CaloCluster*      cluster,       
                              const xAOD::TrackParticle*    trkPB, 
                              bool                          isTRT,
                              unsigned int                  extrapFrom = fromLastMeasurement);

  /**  test for cluster/extrapolated track match, from NeutralPerigee */
  virtual bool  matchesAtCalo(const xAOD::CaloCluster*                 cluster,       
                              const Trk::NeutralParameters*      perigee, 
                              bool                               isTRT,
                              double&                            deltaEta,
                              double&                            deltaPhi);

  /**  test for cluster/extrapolated track match, from xAOD::TrackParticle,
   *   returns true for good match, and
   *           the values for eta/phi, deltaEta/deltaPhi for sampling 2
   */
  virtual bool  matchesAtCalo(const xAOD::CaloCluster*      cluster, 
                              const xAOD::TrackParticle*    trkPB, 
                              bool                          isTRT, 
                              Trk::PropDirection            direction,
                              const std::vector<bool>&      doSample,
                              std::vector<double>&          eta,
                              std::vector<double>&          phi,
                              std::vector<double>&          deltaEta,
                              std::vector<double>&          deltaPhi,
                              unsigned int                  extrapFrom = fromLastMeasurement) const;

  /** test for vertex-to-cluster match given also the positions 
    * at the calorimeter from the vertex extrapolation  **/
  bool matchesAtCalo(const xAOD::CaloCluster* cluster,
                     const xAOD::Vertex *vertex,
                     float etaAtCalo,
                     float phiAtCalo)  const;

  /**   get eta, phi, deltaEta, and deltaPhi at the four calorimeter
   *    layers given the Trk::ParametersBase.  doSample indicates
   *    whether or not to extrapolate to each calo sample
   */
  virtual StatusCode getMatchAtCalo (const xAOD::CaloCluster*      cluster, 
                                     const xAOD::TrackParticle*    trkPB,
                                     Trk::PropDirection            direction,
                                     const std::vector<bool>&      doSample,
                                     std::vector<double>&          eta,
                                     std::vector<double>&          phi,
                                     std::vector<double>&          deltaEta,
                                     std::vector<double>&          deltaPhi,
                                     unsigned int                  extrapFrom = fromLastMeasurement) const;



  /**   get eta, phi at the four calorimeter layers given the
   *    Trk::TrackParameters.  doSample indicates whether or not to
   *    extrapolate to each calo sample. Also return whether the
   *    extrapolation is in barrel or not.
   */
  virtual StatusCode getEtaPhiAtCalo (const xAOD::TrackParticle*    trkPB,
                                      Trk::PropDirection            direction,
                                      const std::vector<bool>&      doSample,
                                      std::vector<double>&          eta,
                                      std::vector<double>&          phi,
                                      std::vector<bool>&            isBarrel,
                                      unsigned int                  extrapFrom = fromLastMeasurement) const;

  /** get eta, phi at EM2 given a vertex which is converted to NeutralParameters.
      Return false if the extrapolation fails **/
  bool getEtaPhiAtCalo (const xAOD::Vertex* vertex, 
                        float *etaAtCalo,
                        float *phiAtCalo) const;
  
  /** get sum of the momenta at the vertex (designed for conversions). Retrieve from auxdata if available and <reuse> is true **/
  Amg::Vector3D getMomentumAtVertex(const xAOD::Vertex&, bool reuse = true) const;

  /** @brief Return +/- 1 (2) if track is in positive/negative TRT barrel (endcap) **/
  int getTRTsection(const xAOD::TrackParticle* trkPB) const;
    
 private:
 
  const Trk::TrackParameters*  getRescaledPerigee(const xAOD::TrackParticle* trkPB, const xAOD::CaloCluster* cluster) const;
   
  /** @brief TrackToCalo extrapolation tool. 
      Handles Trk::TrackParameters as input.
      Extrapolation starts from the last measurement of the track. The
      InDetExtrapolator is used, with all proper material effects inside the
      part of the ID that is traversed. Both charged and neutral particles
      are handled. */ 
  ToolHandle<IExtrapolateToCaloTool>     m_extrapolateToCalo;
  /** @brief Tool to retrieve the calorimeter depth (for track extrapolation)*/
  ToolHandle<CaloDepthTool>              m_calodepth;         
  /** @brief */
  CaloPhiRange                           m_phiHelper;

  /** @brief Utility to calculate the converted photon track parameters and covariance matrix*/
  ToolHandle<Trk::INeutralParticleParameterCalculator> m_convUtils;
  

  // Track-to-cluster match cuts
  double                                m_broadDeltaEta;
  double                                m_broadDeltaPhi;
  double                                m_narrowDeltaEta;
  double                                m_narrowDeltaPhi;
  double                                m_narrowDeltaPhiBrem;
  double                                m_narrowDeltaPhiTRTbarrel;
  double                                m_narrowDeltaPhiBremTRTbarrel;
  double                                m_narrowDeltaPhiTRTendcap;
  double                                m_narrowDeltaPhiBremTRTendcap;
  double                                m_TRTbarrelDeltaEta;
  double                                m_TRTendcapDeltaEta;

  // ID TRT helper
  const TRT_ID*                         m_trtId;
  
  // Use Trk::Track instead of TrackParticle to determine TRT section
  // due to missing association with detector element
  bool m_useTrkTrackForTRT;
  
  // Guess TRT section from eta, instead of using track parameters
  bool m_guessTRTsection;
};


#endif
