#ifndef MVAINPUTEVALUATOR_H
#define MVAINPUTEVALUATOR_H

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexFwd.h"

#include <iterator>
#include <map>
#include <string>

namespace CP {

class MVAInputEvaluator {
public:
  MVAInputEvaluator() {};
  ~MVAInputEvaluator();

  // Nested class for evaluating single input features
  class MiniEvaluator {
  public:
    MiniEvaluator() {};
    virtual ~MiniEvaluator() {};
    virtual float eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& evt) const = 0;
  };

  // Enumerate the different possible inputs
  enum Input {
    Pt,
    Eta,
    M,
    D0,
    ErrD0,
    Z0,
    ErrZ0,
    Phi,
    ErrPhi,
    Theta,
    ErrTheta,
    QOverP,
    ErrQOverP,
    ChiSq,
    RedChiSq,
    D0Sig,
    DZ,
    ErrDZ,
    DZSinTheta,
    ErrDZSinTheta,
    FitWeight,
    VxX,
    ErrVxX,
    VxY,
    ErrVxY,
    VxZ,
    ErrVxZ
  };

  // Handy typedefs
  // Defines the mapping of strings to Inputs:
  typedef std::map<std::string, Input> InputSelectionMap;
  // Defines the mapping of those same strings to MiniEvaluators:
  typedef std::map<std::string, MiniEvaluator*> InputEvaluatorMap;

  void add(const std::string& name, const MVAInputEvaluator::Input& type);
  void load(const MVAInputEvaluator::InputSelectionMap& selection);
  void eval(const xAOD::TrackParticle& trk, const xAOD::Vertex& vx, const xAOD::EventInfo& evt, std::map<std::string, double>& input) const;
  void clear();

private:
  // Loaded evaluator map
  InputEvaluatorMap m_evalMap = {};

};

} // namepace CP

#endif // MVAINPUTEVALUATOR_H
