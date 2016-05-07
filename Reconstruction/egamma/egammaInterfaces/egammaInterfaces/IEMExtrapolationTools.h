/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEMEXTRAPOLATIONTOOLS_H
#define EGAMMAINTERFACES_IEMEXTRAPOLATIONTOOLS_H

#include "GaudiKernel/AlgTool.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"


static const InterfaceID IID_IEMExtrapolationTools("IEMExtrapolationTools", 1, 0);

class IEMExtrapolationTools : virtual public IAlgTool {

 public:

  /** Enum for track extrapolation to calo */
  enum TrkExtrapDef {
    /** from the last measurement of TrackParticle */
    fromLastMeasurement,
    /** from the perigee of TrackParticle */
    fromPerigee,
    /** from the perigee of TrackParticle recaled by Ecluster */
    fromPerigeeRescaled,
    /** from the calo to the perigee - fast extrapolation */
    fromCaloToPerigee
  };

  virtual ~IEMExtrapolationTools() {};

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** initialize method*/
  virtual StatusCode initialize() =  0;
  /** finalize method*/
  virtual StatusCode finalize() = 0;

  /**  test for cluster/extrapolated track match, from Trk::TrackParticleBase,
   *   returns true for good match, and the values for eta/phi, 
   *     deltaEta/deltaPhi for sampling 2
   */
  virtual bool  matchesAtCalo(const xAOD::CaloCluster*      cluster, 
                              const xAOD::TrackParticle*    trkPB, 
                              bool                          isTRT, 
                              Trk::PropDirection            direction,
                              std::vector<double>&          eta,
                              std::vector<double>&          phi,
                              std::vector<double>&          deltaEta,
                              std::vector<double>&          deltaPhi,
                              unsigned int                  extrapFrom = fromPerigee) const = 0;


  /**   get eta, phi, deltaEta, and deltaPhi at the four calorimeter
   *    layers given the Trk::ParametersBase.  
   *    whether or not to extrapolate to each calo sample
   */
  virtual StatusCode getMatchAtCalo (const xAOD::CaloCluster*            cluster, 
                                     const xAOD::TrackParticle* trkPB,
				      bool                          isTRT,                         
				     Trk::PropDirection            direction,
                                     std::vector<double>&          eta,
                                     std::vector<double>&          phi,
                                     std::vector<double>&          deltaEta,
                                     std::vector<double>&          deltaPhi,
                                     unsigned int                  extrapFrom = fromPerigee) const = 0;


  /**  test for cluster/extrapolated track match, from NeutralParticle */
  virtual bool  matchesAtCalo(const xAOD::CaloCluster*           cluster,       
                              const Trk::NeutralParameters*      perigee, 
                              bool                               isTRT,
                              double&                            deltaEta,
                              double&                            deltaPhi) const = 0;

  /** test for vertex-to-cluster match given also the positions 
    * at the calorimeter from the vertex extrapolation  **/
  virtual bool matchesAtCalo(const xAOD::CaloCluster* cluster,
                             const xAOD::Vertex *vertex,
                             float etaAtCalo,
                             float phiAtCalo)  const = 0;


  /** get eta, phi at EM2 given a vertex which is converted to NeutralParameters.
      Return false if the extrapolation fails **/
  virtual bool getEtaPhiAtCalo (const xAOD::Vertex* vertex, 
                                float *etaAtCalo,
                                float *phiAtCalo) const = 0;

  /** get eta, phi at EM2 given NeutralParameters.
      Return false if the extrapolation fails **/
  virtual bool getEtaPhiAtCalo (const Trk::TrackParameters* trkPar, 
				float *etaAtCalo,
				float *phiAtCalo) const =0;
  
  /** get the momentum of the i-th at the vertex (designed for conversions) **/
  virtual Amg::Vector3D getMomentumAtVertex(const xAOD::Vertex&, unsigned int) const = 0;
    
  /** get sum of the momenta at the vertex (designed for conversions). Retrieve from auxdata if available and <reuse> is true **/
  virtual Amg::Vector3D getMomentumAtVertex(const xAOD::Vertex&, bool reuse = true) const = 0;

};

inline const InterfaceID& IEMExtrapolationTools::interfaceID()
{
  return IID_IEMExtrapolationTools;
}

#endif
