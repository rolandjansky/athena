// ISmoothedWZDisCoJetTagger.h

#ifndef ISmoothedWZDisCoJetTagger_H
#define ISmoothedWZDisCoJetTagger_H

#include "AsgTools/IAsgTool.h"

class ISmoothedWZDisCoJetTagger : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(ISmoothedWZDisCoJetTagger)

    public:

    virtual float GetCNNScore(const xAOD::Jet& jet) const = 0;
    virtual float GetDisCoDNNScore(const xAOD::Jet& jet) const = 0;
    virtual std::map<std::string, double> GetJSSVars(const xAOD::Jet& jet) const = 0;
    virtual StatusCode ReadScaler() = 0;

};

#endif
