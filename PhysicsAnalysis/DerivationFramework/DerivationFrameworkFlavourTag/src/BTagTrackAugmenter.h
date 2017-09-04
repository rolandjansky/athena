/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_TRACKAUGMENTER_HH
#define BTAG_TRACKAUGMENTER_HH

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/AuxElement.h"
#include "GeoPrimitives/GeoPrimitives.h"
// #include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Trk  {
  class ITrackToVertexIPEstimator;
  class IExtrapolator;
}
// namespace Amg {
//   class Vector3D;
// }
class Vector3DComponentDecorator {
public:
  Vector3DComponentDecorator(const std::string& prefix);
  void set(const SG::AuxElement& el, const Amg::Vector3D& vec) const;
private:
  SG::AuxElement::Decorator< float > m_x;
  SG::AuxElement::Decorator< float > m_y;
  SG::AuxElement::Decorator< float > m_z;
};

class Vector3DDecorator {
public:
  Vector3DDecorator(const std::string& prefix);
  void set(const SG::AuxElement& el, const Amg::Vector3D& vec) const;
private:
  SG::AuxElement::Decorator< std::vector<float> > m_dec;
};


class BTagTrackAugmenter: public AthAlgorithm {
public:
  BTagTrackAugmenter(const std::string& name,
                      ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
private:
  std::string m_jet_collection;
  std::string m_track_associator;
  bool m_save_track_vectors;
  ToolHandle< Trk::ITrackToVertexIPEstimator > m_track_to_vx;
  ToolHandle< Trk::IExtrapolator >  m_extrapolator;

  // to get ghost tracks
  typedef ElementLink<DataVector<xAOD::IParticle> > ParticleLink;
  typedef std::vector<ParticleLink> PartLinks;
  SG::AuxElement::ConstAccessor<PartLinks> m_ghost_accessor;

  // decorators for tracks
  SG::AuxElement::Decorator< float > m_dec_z0_raw;
  SG::AuxElement::Decorator< float > m_dec_d0;
  SG::AuxElement::Decorator< float > m_dec_z0;
  SG::AuxElement::Decorator< float > m_dec_phi;
  // SG::AuxElement::Decorator< float > m_dec_theta;
  SG::AuxElement::Decorator< float > m_dec_d0_sigma;
  SG::AuxElement::Decorator< float > m_dec_z0_sigma;

  Vector3DDecorator m_dec_track_pos;
  Vector3DDecorator m_dec_track_mom;
};

#endif
