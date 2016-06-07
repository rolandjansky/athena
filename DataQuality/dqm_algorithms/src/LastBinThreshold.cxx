/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file LastBinThreshold.cxx checks whether the last bin(s) in a histogram exceed the given thresholds, returns dqm_core::Result
 * \author Adrian Vogel
 */

#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_algorithms/LastBinThreshold.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"

#include "TClass.h"
#include "TObject.h"
#include "TH1.h"

#include <iostream>
#include <cmath>

namespace { // anonymous
dqm_algorithms::LastBinThreshold instance("LastBinThreshold"); // global instance to have this algorithm registered with the manager
}

dqm_algorithms::LastBinThreshold::LastBinThreshold(const std::string &name): 
	m_nBinsToWatch(0)
	, m_nBinsToExceed(0)
	, m_greaterThan(false)
	, m_valueThresholds(false)
	, m_getEntries(0)
	, m_grn(0.)
	, m_red(0.)
        , m_name(name)
{
    dqm_core::AlgorithmManager::instance().registerAlgorithm(m_name, this);
}

dqm_algorithms::LastBinThreshold::~LastBinThreshold()
{
    /* noop */
}

dqm_algorithms::LastBinThreshold *dqm_algorithms::LastBinThreshold::clone()
{
    return new LastBinThreshold(m_name); // somebody else must delete this
}

dqm_core::Result *dqm_algorithms::LastBinThreshold::execute(const std::string &name, const TObject &object, const dqm_core::AlgorithmConfig &config)
{
    if (!object.IsA()->InheritsFrom("TH1")) { // this could also be TH2 or TH3
        throw dqm_core::BadConfig(ERS_HERE, name, "does not inherit from TH1");
    }
    const TH1 &histogram = dynamic_cast<const TH1 &>(object); // should be possible after the previous check (otherwise we'd get a std::bad_cast)
    if (histogram.GetDimension() > 1) { // only one-dimensional histograms have a last bin, but feel free to generalise this for D > 1 if you like
        throw dqm_core::BadConfig(ERS_HERE, name, "has more than one dimension");
    }

    getParameters(name, config);
    getThresholds(name, config);

    dqm_core::Result *result = new dqm_core::Result(); // somebody else must delete this
    int grnExceeded = 0; // how many of the checked bins exceeded the green threshold
    int redExceeded = 0; // how many of the checked bins exceeded the red threshold

    int currentBin = histogram.GetNbinsX(); // this is the last proper bin (we don't want the overflow bin)
    if        (m_getEntries == 0) { // skip trailing bins with zero content
        while ((histogram.GetBinContent(currentBin) == 0) && (currentBin > 0)) --currentBin;
    } else if (m_getEntries == 1) { // assume that this value corresponds to the bin number of the last bin
        currentBin = histogram.GetEntries();
    } else if (m_getEntries == 2) { /* noop */ } // just start checking from the rightmost bin

    result->tags_["LastBinNumber"] = currentBin; // report where we began the checks (mostly for debugging)
    result->tags_["LastBinCenter"] = histogram.GetBinCenter(currentBin); // report where that is on the x-axis
    // if the histogram is completely empty then currentBin is now zero, and we shouldn't check anything,
    // therefore the break statement is at the front (!) of the following loop

    for (int i = 0; i < m_nBinsToWatch; ++i) { // step through the final NBinsToWatch bins
        if (currentBin == 0) break; // we hit the left end (and we don't want the underflow bin)
        double content = histogram.GetBinContent(currentBin);
        if (m_valueThresholds) {
            if (m_greaterThan) content = histogram.GetXaxis()->GetBinUpEdge(currentBin);
            else               content = histogram.GetXaxis()->GetBinLowEdge(currentBin);
	}
        if (exceeds(content, m_grn)) ++grnExceeded; // can be less-than or greater-than relation
        if (exceeds(content, m_red)) ++redExceeded; // can be less-than or greater-than relation
        --currentBin;
    }

    result->tags_["GreenExceeded"] = grnExceeded;
    result->tags_["RedExceeded"]   = redExceeded;
    if (grnExceeded < redExceeded) // sanity check, this should never happen because getThresholds() makes sure the thresholds have the right order
        throw dqm_core::BadConfig(ERS_HERE, name, "more bins exceeded the red threshold than the green threshold, this shouldn't happen");

    if      (grnExceeded < m_nBinsToExceed) result->status_ = dqm_core::Result::Green;
    else if (redExceeded < m_nBinsToExceed) result->status_ = dqm_core::Result::Yellow;
    else                                    result->status_ = dqm_core::Result::Red;
    return result;
}

void dqm_algorithms::LastBinThreshold::printDescription(std::ostream& out)
{
    out << m_name << ": Checks whether the last bin(s) of a histogram exceed the given thresholds.\n"
        "Mandatory green/red thresholds: Threshold - the thresholds against which the bin contents are compared.\n"
        "Optional parameter: NBinsToWatch - number of final bins that will be checked. (NBinsToWatch >= 1, default = 1)\n"
        "    The result of the algorithm is the worst-case result of all checked bins.\n"
        "Optional parameter: NBinsToExceed - minimal number of checked bins that have to exceed the given thresholds\n"
        "    before the corresponding result is returned. (1 <= NBinsToExceed <= NBinsToWatch, default = 1)\n"
        "Optional parameter: GreaterThan - how the values will be compared. (GreaterThan = {0, 1}, default = 1)\n"
        "    GreaterThan == 0: the given thresholds are lower thresholds. (requires green >= red)\n"
        "    GreaterThan == 1: the given thresholds are upper thresholds. (requires green <= red)\n"
        "Optional parameter: ValueThresholds - which values will be compared. (ValueThresholds = {0, 1}, default = 0)\n"
        "    ValueThresholds == 0: the thresholds correspond to the bin content.\n"
        "    ValueThresholds == 1: the thresholds correspond to the bin x-axis value.\n"
        "Optional parameter: GetEntries - how to determine which bin is the last. (GetEntries = {0, 1, 2}, default = 0)\n"
        "    GetEntries == 0: find the rightmost non-zero bin, ignore trailing zero bins.\n"
        "    GetEntries == 1: take the return value of TH1::GetEntries() as the number of the last bin.\n"
        "    GetEntries == 2: take the rightmost bin as the last bin, regardless of the bin content.\n"
        "Returned value: LastBinNumber - the bin number of the last checked (i. e. non-zero) bin.\n"
        "Returned value: LastBinCenter - the centre of that bin on the horizontal axis.\n"
        "Returned value: GreenExceeded - how many bins exceeded the green threshold.\n"
        "Returned value: RedExceeded - how many bins exceeded the red threshold." << std::endl;
}

void dqm_algorithms::LastBinThreshold::getParameters(const std::string &name, const dqm_core::AlgorithmConfig &config)
{
    // The following intermediate variables do not yet have the correct type,
    // since dqm_core::AlgorithmConfig::getParameters can only return values of type double.
    // We cannot cast them immediately because we still need to perform some manual range checks.
    //
    const double par0 = dqm_algorithms::tools::GetFirstFromMap("NBinsToWatch",    config.getParameters(), 1); // default = 1
    const double par1 = dqm_algorithms::tools::GetFirstFromMap("NBinsToExceed",   config.getParameters(), 1); // default = 1
    const double par2 = dqm_algorithms::tools::GetFirstFromMap("GreaterThan",     config.getParameters(), 1); // default = true
    const double par3 = dqm_algorithms::tools::GetFirstFromMap("ValueThresholds", config.getParameters(), 0); // default = false
    const double par4 = dqm_algorithms::tools::GetFirstFromMap("GetEntries",      config.getParameters(), 0); // default = 0

    if (par0 < 1)
        throw dqm_core::BadConfig(ERS_HERE, name, "NBinsToWatch must be 1 or greater");
    if (par1 < 1)
        throw dqm_core::BadConfig(ERS_HERE, name, "NBinsToExceed must be 1 or greater");
    if (par1 > par0)
        throw dqm_core::BadConfig(ERS_HERE, name, "NBinsToExceed must not be greater than NBinsToWatch");
    if ((par2 != 0) && (par2 != 1))
        throw dqm_core::BadConfig(ERS_HERE, name, "GreaterThan must be 0 or 1");
    if ((par3 != 0) && (par3 != 1))
        throw dqm_core::BadConfig(ERS_HERE, name, "ValueThresholds must be 0 or 1");
    if ((par4 != 0) && (par4 != 1) && (par4 != 2))
        throw dqm_core::BadConfig(ERS_HERE, name, "GetEntries must be 0, 1 or 2");

    m_nBinsToWatch    = static_cast<int>(par0);
    m_nBinsToExceed   = static_cast<int>(par1);
    m_greaterThan     = static_cast<bool>(par2);
    m_valueThresholds = static_cast<bool>(par3);
    m_getEntries      = static_cast<int>(par4);
}

void dqm_algorithms::LastBinThreshold::getThresholds(const std::string &name, const dqm_core::AlgorithmConfig &config)
{
    m_grn = dqm_algorithms::tools::GetFromMap("Threshold", config.getGreenThresholds());
    m_red = dqm_algorithms::tools::GetFromMap("Threshold", config.getRedThresholds());

    if      (m_greaterThan  && (m_red < m_grn))
        throw dqm_core::BadConfig(ERS_HERE, name, "using greater-than comparison, but red threshold is less than green threshold");
    else if (!m_greaterThan && (m_red > m_grn))
        throw dqm_core::BadConfig(ERS_HERE, name, "using less-than comparison, but red threshold is greater than green threshold");
}

inline bool dqm_algorithms::LastBinThreshold::exceeds(double value, double threshold) const
{
    if (m_greaterThan)
        return value > threshold;
    else
        return value < threshold;
    // We could derive that behaviour from testing whether (m_grn < m_red) or (m_grn > m_red), but then we
    // weren't able to find a reasonable behaviour if (m_grn == m_red), i. e. for people who never want yellow.
}
