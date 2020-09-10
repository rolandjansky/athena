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

class Pt: public MiniEvaluator {
public:
  Pt() {};
  ~Pt() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return trk.pt();
  }
};

class Eta: public MiniEvaluator {
public:
  Eta() {};
  ~Eta() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return trk.eta();
  }
};

class M: public MiniEvaluator {
public:
  M() {};
  ~M() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return trk.m();
  }
};

class D0: public MiniEvaluator {
public:
  D0() {};
  ~D0() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return trk.d0();
  }
};

class ErrD0: public MiniEvaluator {
public:
  ErrD0() {};
  ~ErrD0() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(0, 0));
  }
};

class Z0: public MiniEvaluator {
public:
  Z0() {};
  ~Z0() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return trk.z0();
  }
};

class ErrZ0: public MiniEvaluator {
public:
  ErrZ0() {};
  ~ErrZ0() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(1, 1));
  }
};

class Phi: public MiniEvaluator {
public:
  Phi() {};
  ~Phi() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return trk.phi();
  }
};

class ErrPhi: public MiniEvaluator {
public:
  ErrPhi() {};
  ~ErrPhi() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(2, 2));
  }
};

class Theta: public MiniEvaluator {
public:
  Theta() {};
  ~Theta() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return trk.theta();
  }
};

class ErrTheta: public MiniEvaluator {
public:
  ErrTheta() {};
  ~ErrTheta() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(3, 3));
  }
};

class QOverP: public MiniEvaluator {
public:
  QOverP() {};
  ~QOverP() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return trk.qOverP();
  }
};

class ErrQOverP: public MiniEvaluator {
public:
  ErrQOverP() {};
  ~ErrQOverP() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(4, 4));
  }
};

class ChiSq: public MiniEvaluator {
public:
  ChiSq() {};
  ~ChiSq() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return trk.chiSquared();
  }
};

class RedChiSq: public MiniEvaluator {
public:
  RedChiSq() {};
  ~RedChiSq() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return (trk.chiSquared() / trk.numberDoF());
  }
};

class D0Sig: public MiniEvaluator {
public:
  D0Sig() {};
  ~D0Sig() override final {};
  float eval(const xAOD::TrackParticle& trk, __attribute__((unused)) const xAOD::Vertex& vx, const xAOD::EventInfo& evt) const override final {
    return xAOD::TrackingHelpers::d0significance(&trk, evt.beamPosSigmaX(), evt.beamPosSigmaY(), evt.beamPosSigmaXY());
  }
};

class DZ: public MiniEvaluator {
public:
  DZ() {};
  ~DZ() override final {};
  float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return (trk.z0() + trk.vz() - vx.z());
  }
};

class ErrDZ: public MiniEvaluator {
public:
  ErrDZ() {};
  ~ErrDZ() override final {};
  float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& evt) const override final {
    return std::sqrt(trk.definingParametersCovMatrix()(1, 1) + std::pow(evt.beamPosSigmaZ(), 2) + vx.covariancePosition()(2, 2));
  }
};

class DZSinTheta: public MiniEvaluator {
public:
  DZSinTheta() {};
  ~DZSinTheta() override final {};
  float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return (trk.z0() + trk.vz() - vx.z()) * std::sin(trk.theta());
  }
};

class ErrDZSinTheta: public MiniEvaluator {
public:
  ErrDZSinTheta() {};
  ~ErrDZSinTheta() override final {};
  float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& evt) const override final {
    float dz         = (trk.z0() + trk.vz() - vx.z());
    float sinTheta   = std::sin(trk.theta());
    float cosTheta   = std::cos(trk.theta());
    float errSqDZ    = trk.definingParametersCovMatrix()(1, 1) + std::pow(evt.beamPosSigmaZ(), 2) + vx.covariancePosition()(2, 2);
    float errSqTheta = trk.definingParametersCovMatrix()(3, 3);
    float covZ0Theta = trk.definingParametersCovMatrix()(1, 3);
    return (errSqDZ * std::pow(sinTheta, 2) + std::pow(dz * cosTheta, 2) * errSqTheta + 2 * sinTheta * dz * cosTheta * covZ0Theta);
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

class FitWeight: public MiniEvaluator {
public:
  FitWeight() {};
  ~FitWeight() override final {};
  float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    std::vector<const xAOD::TrackParticle*> fitTrks = toVector((const std::vector<ElementLink<xAOD::TrackParticleContainer>>&)vx.trackParticleLinks());
    float weight = 0.0;
    int pos = inVector(&trk, fitTrks);
    if (pos != -1) {
      weight = vx.trackWeights().at(pos);
    }
    return weight;
  }
};

class VxX: public MiniEvaluator {
public:
  VxX() {};
  ~VxX() override final {};
  float eval(__attribute__((unused)) const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return vx.x();
  }
};

class ErrVxX: public MiniEvaluator {
public:
  ErrVxX() {};
  ~ErrVxX() override final {};
  float eval(__attribute__((unused)) const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return std::sqrt(vx.covariancePosition()(0, 0));
  }
};

class VxY: public MiniEvaluator {
public:
  VxY() {};
  ~VxY() override final {};
  float eval(__attribute__((unused)) const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return vx.y();
  }
};

class ErrVxY: public MiniEvaluator {
public:
  ErrVxY() {};
  ~ErrVxY() override final {};
  float eval(__attribute__((unused)) const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return std::sqrt(vx.covariancePosition()(1, 1));
  }
};

class VxZ: public MiniEvaluator {
public:
  VxZ() {};
  ~VxZ() override final {};
  float eval(__attribute__((unused)) const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return vx.z();
  }
};

class ErrVxZ: public MiniEvaluator {
public:
  ErrVxZ() {};
  ~ErrVxZ() override final {};
  float eval(__attribute__((unused)) const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, __attribute__((unused)) const xAOD::EventInfo& evt) const override final {
    return std::sqrt(vx.covariancePosition()(2, 2));
  }
};

} // namespace

namespace CP {

MVAInputEvaluator::~MVAInputEvaluator() {
  this->clear();
}

void MVAInputEvaluator::add(const std::string& name, const MVAInputEvaluator::Input& type) {
  using Input = MVAInputEvaluator::Input;
  for (auto it = m_evalMap.begin(); it != m_evalMap.end(); it++) {
    if (name == it->first) {
      throw std::runtime_error("ERROR in CP::MVAInputEvaluator::add : input name is already present in map: " + name);
    }
  }
  switch (type) {
    case Input::Pt: {
      m_evalMap[name] = new ::Pt();
      break;
    }
    case Input::Eta: {
      m_evalMap[name] = new ::Eta();
      break;
    }
    case Input::M: {
      m_evalMap[name] = new ::M();
      break;
    }
    case Input::D0: {
      m_evalMap[name] = new ::D0();
      break;
    }
    case Input::ErrD0: {
      m_evalMap[name] = new ::ErrD0();
      break;
    }
    case Input::Z0: {
      m_evalMap[name] = new ::Z0();
      break;
    }
    case Input::ErrZ0: {
      m_evalMap[name] = new ::ErrZ0();
      break;
    }
    case Input::Phi: {
      m_evalMap[name] = new ::Phi();
      break;
    }
    case Input::ErrPhi: {
      m_evalMap[name] = new ::ErrPhi();
      break;
    }
    case Input::Theta: {
      m_evalMap[name] = new ::Theta();
      break;
    }
    case Input::ErrTheta: {
      m_evalMap[name] = new ::ErrTheta();
      break;
    }
    case Input::QOverP: {
      m_evalMap[name] = new ::QOverP();
      break;
    }
    case Input::ErrQOverP: {
      m_evalMap[name] = new ::ErrQOverP();
      break;
    }
    case Input::ChiSq: {
      m_evalMap[name] = new ::ChiSq();
      break;
    }
    case Input::RedChiSq: {
      m_evalMap[name] = new ::RedChiSq();
      break;
    }
    case Input::D0Sig: {
      m_evalMap[name] = new ::D0Sig();
      break;
    }
    case Input::DZ: {
      m_evalMap[name] = new ::DZ();
      break;
    }
    case Input::ErrDZ: {
      m_evalMap[name] = new ::ErrDZ();
      break;
    }
    case Input::DZSinTheta: {
      m_evalMap[name] = new ::DZSinTheta();
      break;
    }
    case Input::ErrDZSinTheta: {
      m_evalMap[name] = new ::ErrDZSinTheta();
      break;
    }
    case Input::FitWeight: {
      m_evalMap[name] = new ::FitWeight();
      break;
    }
    case Input::VxX: {
      m_evalMap[name] = new ::VxX();
      break;
    }
    case Input::ErrVxX: {
      m_evalMap[name] = new ::ErrVxX();
      break;
    }
    case Input::VxY: {
      m_evalMap[name] = new ::VxY();
      break;
    }
    case Input::ErrVxY: {
      m_evalMap[name] = new ::ErrVxY();
      break;
    }
    case Input::VxZ: {
      m_evalMap[name] = new ::VxZ();
      break;
    }
    case Input::ErrVxZ: {
      m_evalMap[name] = new ::ErrVxZ();
      break;
    }
    default: {
      throw std::runtime_error("ERROR in CP::MVAInputEvaluator::add : unknown input enum: " + std::to_string(type));
    }
  }
  return;
}

void MVAInputEvaluator::load(const MVAInputEvaluator::InputSelectionMap& selection) {
  this->clear();
  std::string k;
  for (auto it = selection.begin(); it != selection.end(); it++) {
    this->add(it->first, it->second);
  }
  return;
}

void MVAInputEvaluator::eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& evt, std::map<std::string, double>& input) const {
  input.clear();
  for (auto it = m_evalMap.begin(); it != m_evalMap.end(); it++) {
    input[it->first] = it->second->eval(trk, vx, evt);
  }
  return;
}

void MVAInputEvaluator::clear() {
  for (auto it = m_evalMap.begin(); it != m_evalMap.end(); it++) {
    delete it->second;
    it->second = nullptr;
  }
  m_evalMap.clear();
  return;
}

} // namespace CP
