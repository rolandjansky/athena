/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_LASTBINTHRESHOLD_H
#define DQM_ALGORITHMS_LASTBINTHRESHOLD_H

#include "dqm_core/Algorithm.h"
#include <string>

namespace dqm_core { class AlgorithmConfig; }
class TObject;

namespace dqm_algorithms
{

class LastBinThreshold: public dqm_core::Algorithm
{
public:
    explicit LastBinThreshold(const std::string &name);
    virtual ~LastBinThreshold();

public:
    virtual LastBinThreshold *clone();
    virtual dqm_core::Result *execute(const std::string &name, const TObject &object, const dqm_core::AlgorithmConfig &config);
    using dqm_core::Algorithm::printDescription;
    virtual void printDescription(std::ostream& out);

private:
    void getParameters(const std::string &name, const dqm_core::AlgorithmConfig &config);
    void getThresholds(const std::string &name, const dqm_core::AlgorithmConfig &config);
    inline bool exceeds(double value, double threshold) const;

private:
    // optional parameters
    int m_nBinsToWatch; // how many bins to watch (default = 1), returning worst-case result of those bins
    int m_nBinsToExceed; // how many bins have to exceed the threshold (default = 1), less will be ignored
    bool m_greaterThan; // greater-than or less-than comparison against thresholds? (default = true)
    bool m_valueThresholds; // thresholds set w.r.t num entries per bin (false) or w.r.t to X-axis value? (default = false)
    int  m_getEntries; // search the rightmost non-zero bin (default) 
                       // or just use GetEntries()
                       // or search the rightmost bin regardless of the bin content 
    // neither MinStat nor xmin/xmax makes sense for this kind of algorithm, I assume

    // thresholds
    double m_grn;
    double m_red;

    // other properties
    const std::string m_name;
};

} // namespace

#endif // DQM_ALGORITHMS_LASTBINTHRESHOLD_H
