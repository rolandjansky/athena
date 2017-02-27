/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATRACKTOOLS_EMEXTRAPOLATIONTOOLS_H
#define EGAMMATRACKTOOLS_EMEXTRAPOLATIONTOOLS_H
/** 
  @class EMExtrapolationTools
          Tools for track extrapolation to the calorimeter
  @author Thomas Koffas, Christos Anastopoulos
*/

/********************************************************************

NAME:     EMExtrapolationTools.h
PACKAGE:  offline/Reconstruction/egammaTrackTools

AUTHORS:  T. Koffas, C.Anastopoulos

PURPOSE:  Tool which propagate track to 
             - calorimeter
             - cluster
********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"

#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"

#include "TrkCaloExtension/CaloExtensionHelpers.h"

class TRT_ID;
class CaloDepthTool;

namespace Trk
{
  class INeutralParticleParameterCalculator;
  class IExtrapolator;
  class IParticleCaloExtensionTool;
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


  /**  test for cluster/extrapolated track match, from xAOD::TrackParticle,
   * returns true for good match using sampling 2, and
   *  the values for eta/phi, deltaEta/deltaPhi 
   */
  virtual bool  matchesAtCalo(const xAOD::CaloCluster*      cluster, 
                              const xAOD::TrackParticle*    trkPB, 
                              bool                          isTRT, 
                              Trk::PropDirection            direction,
                              std::vector<double>&          eta,
                              std::vector<double>&          phi,
                              std::vector<double>&          deltaEta,
                              std::vector<double>&          deltaPhi,
                              unsigned int                  extrapFrom = fromPerigee) const;


  /**   get eta, phi, deltaEta, and deltaPhi at the four calorimeter
   *    layers given the Trk::ParametersBase.  */
  virtual StatusCode getMatchAtCalo (const xAOD::CaloCluster*      cluster, 
                                     const xAOD::TrackParticle*    trkPB,
				     bool                          isTRT, 
                                     Trk::PropDirection            direction,
                                     std::vector<double>&          eta,
                                     std::vector<double>&          phi,
                                     std::vector<double>&          deltaEta,
                                     std::vector<double>&          deltaPhi,
                                     unsigned int                  extrapFrom = fromPerigee) const;


  /**  test for cluster/extrapolated track match, from NeutralPerigee */
  virtual bool  matchesAtCalo(const xAOD::CaloCluster*                 cluster,       
                              const Trk::NeutralParameters*      perigee, 
                              bool                               isTRT,
                              double&                            deltaEta,
                              double&                            deltaPhi) const;


  /** test for vertex-to-cluster match given also the positions 
      at the calorimeter from the vertex extrapolation  **/
  virtual bool matchesAtCalo(const xAOD::CaloCluster* cluster,
                     const xAOD::Vertex *vertex,
                     float etaAtCalo,
                     float phiAtCalo)  const;


  /** get eta, phi at EM2 given a vertex which is converted to NeutralParameters.
      Return false if the extrapolation fails **/
  virtual bool getEtaPhiAtCalo (const xAOD::Vertex* vertex, 
                                float *etaAtCalo,
                                float *phiAtCalo) const;

  /** get eta, phi at EM2 given NeutralParameters.
      Return false if the extrapolation fails **/
 virtual bool getEtaPhiAtCalo (const Trk::TrackParameters* trkPar, 
			       float *etaAtCalo,
			       float *phiAtCalo) const;

  /** get the momentum of the i-th at the vertex (designed for conversions) **/
  Amg::Vector3D getMomentumAtVertex(const xAOD::Vertex&, unsigned int) const;
  
  /** get sum of the momenta at the vertex (designed for conversions). Retrieve from auxdata if available and <reuse> is true **/
  Amg::Vector3D getMomentumAtVertex(const xAOD::Vertex&, bool reuse = true) const;

    
 private:

  /** @Perform the Rescaling of the perigee parameters with the cluster energy **/
  const Trk::TrackParameters*  getRescaledPerigee(const xAOD::TrackParticle* trkPB, const xAOD::CaloCluster* cluster) const;

  /** @brief Return +/- 1 (2) if track is in positive/negative TRT barrel (endcap) **/
  int getTRTsection(const xAOD::TrackParticle* trkPB) const;

  /** get eta, phi at EM2 For trackparameters 
      Return false if the extrapolation fails **/
  bool getHackEtaPhiAtCalo (const  Trk::TrackParameters*, 
                            float *etaAtCalo,
                            float *phiAtCalo,
			    CaloExtensionHelpers::LayersToSelect& layersToSelect
			    ) const;
  
  ToolHandle<Trk::IParticleCaloExtensionTool>     m_defaultParticleCaloExtensionTool;
  ToolHandle<Trk::IParticleCaloExtensionTool>     m_perigeeParticleCaloExtensionTool;
  ToolHandle<Trk::IExtrapolator>                  m_extrapolator;
       
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
  
  //Use the a cache for track Particle extrapolation
  bool  m_useCaching;
  
};


#endif
