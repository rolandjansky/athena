/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackVertexAssociationTool/MVAInputEvaluator.h"

#include "AthLinks/ElementLink.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/Vertex.h"

#include <cmath>
#include <stdexcept>
#include <vector>

// Private
namespace {

using MiniEvaluator = CP::MVAInputEvaluator::MiniEvaluator;

class Pt final : public MiniEvaluator {
public:
  Pt() {};
  ~Pt() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return trk.pt();
  }
};

class Eta final: public MiniEvaluator {
public:
  Eta() {};
  ~Eta() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return trk.eta();
  }
};

class M final: public MiniEvaluator {
public:
  M() {};
  ~M() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return trk.m();
  }
};

class D0 final: public MiniEvaluator {
public:
  D0() {};
  ~D0() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return trk.d0();
  }
};

class ErrD0 final: public MiniEvaluator {
public:
  ErrD0() {};
  ~ErrD0() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(0, 0));
  }
};

class Z0 final : public MiniEvaluator {
public:
  Z0() {};
  ~Z0() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return trk.z0();
  }
};

class ErrZ0 final : public MiniEvaluator {
public:
  ErrZ0() {};
  ~ErrZ0() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(1, 1));
  }
};

class Phi final : public MiniEvaluator {
public:
  Phi() {};
  ~Phi() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return trk.phi();
  }
};

class ErrPhi final : public MiniEvaluator {
public:
  ErrPhi() {};
  ~ErrPhi() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(2, 2));
  }
};

class Theta final : public MiniEvaluator {
public:
  Theta() {};
  ~Theta() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return trk.theta();
  }
};

class ErrTheta final : public MiniEvaluator {
public:
  ErrTheta() {};
  ~ErrTheta() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(3, 3));
  }
};

class QOverP final : public MiniEvaluator {
public:
  QOverP() {};
  ~QOverP() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return trk.qOverP();
  }
};

class ErrQOverP final : public MiniEvaluator {
public:
  ErrQOverP() {};
  ~ErrQOverP() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(4, 4));
  }
};

class ChiSq final : public MiniEvaluator {
public:
  ChiSq() {};
  ~ChiSq() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return trk.chiSquared();
  }
};

class RedChiSq final : public MiniEvaluator {
public:
  RedChiSq() {};
  ~RedChiSq() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& /*evt*/) const override final {
    return (trk.chiSquared() / trk.numberDoF());
  }
};

class D0Sig final : public MiniEvaluator {
public:
  D0Sig() {};
  ~D0Sig() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& /*vx*/, const xAOD::EventInfo& evt) const override final {
    return xAOD::TrackingHelpers::d0significance(&trk, evt.beamPosSigmaX(), evt.beamPosSigmaY(), evt.beamPosSigmaXY());
  }
};

class DZ final : public MiniEvaluator {
public:
  DZ() {};
  ~DZ() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    return (trk.z0() + trk.vz() - vx.z());
  }
};

class ErrDZ final : public MiniEvaluator {
public:
  ErrDZ() {};
  ~ErrDZ() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(1, 1) + vx.covariancePosition()(2, 2));
  }
};

class DZSinTheta final : public MiniEvaluator {
public:
  DZSinTheta() {};
  ~DZSinTheta() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    return (trk.z0() + trk.vz() - vx.z()) * std::sin(trk.theta());
  }
};

class ErrDZSinTheta final : public MiniEvaluator {
public:
  ErrDZSinTheta() {};
  ~ErrDZSinTheta() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    float dz         = (trk.z0() + trk.vz() - vx.z());
    float sinTheta   = std::sin(trk.theta());
    float cosTheta   = std::cos(trk.theta());
    float errSqDZ    = trk.definingParametersCovMatrix()(1, 1) + vx.covariancePosition()(2, 2);
    float errSqTheta = trk.definingParametersCovMatrix()(3, 3);
    float covZ0Theta = trk.definingParametersCovMatrix()(1, 3);
    return std::sqrt(errSqDZ * std::pow(sinTheta, 2) + std::pow(dz * cosTheta, 2) * errSqTheta + 2 * sinTheta * dz * cosTheta * covZ0Theta);
  }
};

std::vector<const xAOD::TrackParticle*> toVector(const std::vector<ElementLink<xAOD::TrackParticleContainer>>& inTrks) {
  std::vector<const xAOD::TrackParticle*> outTrks;
  for (const auto& trkLink : inTrks) {
    if (trkLink.isValid()) {
      outTrks.push_back((const xAOD::TrackParticle*)(*trkLink));
    }
  }
  return outTrks;
}

int inVector(const xAOD::TrackParticle* trk, const std::vector<const xAOD::TrackParticle*>& vec) {
  int i = 0;
  for (const auto& ele : vec) {
    if (trk == ele) {
      return i;
    }
    i++;
  }
  return -1;
}

class FitWeight final : public MiniEvaluator {
public:
  FitWeight() {};
  ~FitWeight() override final {};
  virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    std::vector<const xAOD::TrackParticle*> fitTrks = toVector(vx.trackParticleLinks());
    float weight = 0.0;
    int pos = inVector(&trk, fitTrks);
    if (pos != -1) {
      weight = vx.trackWeights().at(pos);
    }
    return weight;
  }
};

class VxX final : public MiniEvaluator {
public:
  VxX() {};
  ~VxX() override final {};
  virtual float eval(const xAOD::TrackParticle& /*trk*/, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    return vx.x();
  }
};

class ErrVxX final : public MiniEvaluator {
public:
  ErrVxX() {};
  ~ErrVxX() override final {};
  virtual float eval(const xAOD::TrackParticle& /*trk*/, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    return std::sqrt(vx.covariancePosition()(0, 0));
  }
};

class VxY final : public MiniEvaluator {
public:
  VxY() {};
  ~VxY() override final {};
  virtual float eval(const xAOD::TrackParticle& /*trk*/, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    return vx.y();
  }
};

class ErrVxY final : public MiniEvaluator {
public:
  ErrVxY() {};
  ~ErrVxY() override final {};
  virtual float eval(const xAOD::TrackParticle& /*trk*/, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    return std::sqrt(vx.covariancePosition()(1, 1));
  }
};

class VxZ final : public MiniEvaluator {
public:
  VxZ() {};
  ~VxZ() override final {};
  virtual float eval(const xAOD::TrackParticle& /*trk*/, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    return vx.z();
  }
};

class ErrVxZ final : public MiniEvaluator {
public:
  ErrVxZ() {};
  ~ErrVxZ() override final {};
  virtual float eval(const xAOD::TrackParticle& /*trk*/, const xAOD::Vertex& vx, const xAOD::EventInfo& /*evt*/) const override final {
    return std::sqrt(vx.covariancePosition()(2, 2));
  }
};

} // namespace

namespace CP {

MVAInputEvaluator::~MVAInputEvaluator() {
  clear();
}

void MVAInputEvaluator::add(const std::string& name, const MVAInputEvaluator::Input& type) {
  using Input = MVAInputEvaluator::Input;
  for (const auto& kv : m_evalMap) {
    if (name == kv.first) {
      throw std::runtime_error("ERROR in CP::MVAInputEvaluator::add : input name is already present in map: " + name);
    }
  }
  switch (type) {
    case Input::Pt: {
      m_evalMap[name] = std::make_unique<::Pt>(::Pt());
      break;
    }
    case Input::Eta: {
      m_evalMap[name] = std::make_unique<::Eta>(::Eta());
      break;
    }
    case Input::M: {
      m_evalMap[name] = std::make_unique<::M>(::M());
      break;
    }
    case Input::D0: {
      m_evalMap[name] = std::make_unique<::D0>(::D0());
      break;
    }
    case Input::ErrD0: {
      m_evalMap[name] = std::make_unique<::ErrD0>(::ErrD0());
      break;
    }
    case Input::Z0: {
      m_evalMap[name] = std::make_unique<::Z0>(::Z0());
      break;
    }
    case Input::ErrZ0: {
      m_evalMap[name] = std::make_unique<::ErrZ0>(::ErrZ0());
      break;
    }
    case Input::Phi: {
      m_evalMap[name] = std::make_unique<::Phi>(::Phi());
      break;
    }
    case Input::ErrPhi: {
      m_evalMap[name] = std::make_unique<::ErrPhi>(::ErrPhi());
      break;
    }
    case Input::Theta: {
      m_evalMap[name] = std::make_unique<::Theta>(::Theta());
      break;
    }
    case Input::ErrTheta: {
      m_evalMap[name] = std::make_unique<::ErrTheta>(::ErrTheta());
      break;
    }
    case Input::QOverP: {
      m_evalMap[name] = std::make_unique<::QOverP>(::QOverP());
      break;
    }
    case Input::ErrQOverP: {
      m_evalMap[name] = std::make_unique<::ErrQOverP>(::ErrQOverP());
      break;
    }
    case Input::ChiSq: {
      m_evalMap[name] = std::make_unique<::ChiSq>(::ChiSq());
      break;
    }
    case Input::RedChiSq: {
      m_evalMap[name] = std::make_unique<::RedChiSq>(::RedChiSq());
      break;
    }
    case Input::D0Sig: {
      m_evalMap[name] = std::make_unique<::D0Sig>(::D0Sig());
      break;
    }
    case Input::DZ: {
      m_evalMap[name] = std::make_unique<::DZ>(::DZ());
      break;
    }
    case Input::ErrDZ: {
      m_evalMap[name] = std::make_unique<::ErrDZ>(::ErrDZ());
      break;
    }
    case Input::DZSinTheta: {
      m_evalMap[name] = std::make_unique<::DZSinTheta>(::DZSinTheta());
      break;
    }
    case Input::ErrDZSinTheta: {
      m_evalMap[name] = std::make_unique<::ErrDZSinTheta>(::ErrDZSinTheta());
      break;
    }
    case Input::FitWeight: {
      m_evalMap[name] = std::make_unique<::FitWeight>(::FitWeight());
      break;
    }
    case Input::VxX: {
      m_evalMap[name] = std::make_unique<::VxX>(::VxX());
      break;
    }
    case Input::ErrVxX: {
      m_evalMap[name] = std::make_unique<::ErrVxX>(::ErrVxX());
      break;
    }
    case Input::VxY: {
      m_evalMap[name] = std::make_unique<::VxY>(::VxY());
      break;
    }
    case Input::ErrVxY: {
      m_evalMap[name] = std::make_unique<::ErrVxY>(::ErrVxY());
      break;
    }
    case Input::VxZ: {
      m_evalMap[name] = std::make_unique<::VxZ>(::VxZ());
      break;
    }
    case Input::ErrVxZ: {
      m_evalMap[name] = std::make_unique<::ErrVxZ>(::ErrVxZ());
      break;
    }
    default: {
      throw std::runtime_error("ERROR in CP::MVAInputEvaluator::add : unknown input enum: " + std::to_string(type));
    }
  }
  return;
}

void MVAInputEvaluator::load(const MVAInputEvaluator::InputSelectionMap& selection) {
  clear();
  for (const auto& kv : selection) {
    add(kv.first, kv.second);
  }
  return;
}

void MVAInputEvaluator::eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& evt, std::map<std::string, double>& input) const {
  input.clear();
  for (const auto& kv : m_evalMap) {
    input[kv.first] = kv.second->eval(trk, vx, evt);
  }
  return;
}

void MVAInputEvaluator::clear() {
  m_evalMap.clear();
  return;
}

} // namespace CP
