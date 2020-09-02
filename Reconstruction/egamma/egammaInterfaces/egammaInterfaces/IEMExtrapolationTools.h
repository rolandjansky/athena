/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEMEXTRAPOLATIONTOOLS_H
#define EGAMMAINTERFACES_IEMEXTRAPOLATIONTOOLS_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"
#include "TrkCaloExtension/CaloExtension.h"

#include <memory>
#include <array>
#include <unordered_map>



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

  typedef  std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>> Cache;

  /**   get eta, phi, deltaEta, and deltaPhi at the four calorimeter
   *    layers given the Trk::ParametersBase.  
   *    whether or not to extrapolate to each calo sample
   */
  virtual StatusCode getMatchAtCalo(const EventContext& ctx,
                                    const xAOD::CaloCluster& cluster,
                                    const xAOD::TrackParticle& trkPB,
                                    Trk::PropDirection direction,
                                    std::array<double, 4>& eta,
                                    std::array<double, 4>& phi,
                                    std::array<double, 4>& deltaEta,
                                    std::array<double, 4>& deltaPhi,
                                    unsigned int extrapFrom = fromPerigee,
                                    Cache* cache = nullptr) const = 0;

  /** test for vertex-to-cluster match given also the positions 
   * at the calorimeter from the vertex extrapolation  **/
  virtual bool matchesAtCalo(const xAOD::CaloCluster* cluster,
                             const xAOD::Vertex *vertex,
                             float etaAtCalo,
                             float phiAtCalo)  const = 0;


  /** get eta, phi at EM2 given a vertex which is converted to NeutralParameters.
    Return false if the extrapolation fails **/
  virtual bool getEtaPhiAtCalo(const EventContext& ctx,
                               const xAOD::Vertex* vertex,
                               float* etaAtCalo,
                               float* phiAtCalo) const = 0;

  /** get eta, phi at EM2 given NeutralParameters.
    Return false if the extrapolation fails **/
  virtual bool getEtaPhiAtCalo(const EventContext& ctx,
                               const Trk::TrackParameters* trkPar,
                               float* etaAtCalo,
                               float* phiAtCalo) const = 0;

  /** get the momentum of the i-th trackParticle attached to the vertex 
   * at the vertex (designed for conversions) **/
  virtual Amg::Vector3D getMomentumAtVertex(const EventContext& ctx,
                                            const xAOD::Vertex&,
                                            unsigned int) const = 0;

  /** get sum of the momenta at the vertex (designed for conversions). Retrieve from auxdata if available and \<reuse\> is true **/
  virtual Amg::Vector3D getMomentumAtVertex(const EventContext& ctx,
                                            const xAOD::Vertex&,
                                            bool reuse = true) const = 0;
};

inline const InterfaceID& IEMExtrapolationTools::interfaceID()
{
  return IID_IEMExtrapolationTools;
}

#endif
