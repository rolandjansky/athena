/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file BinContentDump.cxx dumps contents of all bins in a histogram (preferably char labeled TH1).  Method to store quantities in db; returns dqm_core::Result
 * \author Justin Griffiths
 */

#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_algorithms/BinContentDump.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "TH1.h"
#include "TClass.h"
#include "ers/ers.h"

#include <string>
#include <sstream>

static dqm_algorithms::BinContentDump myInstance;

dqm_algorithms::BinContentDump::BinContentDump()
{
    dqm_core::AlgorithmManager::instance().registerAlgorithm("BinContentDump", this);
}

dqm_algorithms::BinContentDump::~BinContentDump()
{
}

dqm_algorithms::BinContentDump *dqm_algorithms::BinContentDump::clone()
{
    return new BinContentDump();
}

dqm_core::Result *dqm_algorithms::BinContentDump::execute(const std::string &name, const TObject &object, const dqm_core::AlgorithmConfig &config)
{
    TH1 *histogram;

    if (object.IsA()->InheritsFrom("TH1")) {
        histogram = (TH1*)&object;
        if (histogram->GetDimension() > 2) {
            throw dqm_core::BadConfig(ERS_HERE, name, "dimension > 2");
        }
    } else {
        throw dqm_core::BadConfig(ERS_HERE, name, "does not inherit from TH1");
    }

    const double minstat = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), -1);
    const unsigned int publishType = static_cast<unsigned int>(dqm_algorithms::tools::GetFirstFromMap("PublishType", config.getParameters(), 1));
    // const int maxpublish = (int) dqm_algorithms::tools::GetFirstFromMap("MaxPublish", config.getParameters(), 20);

    if (histogram->GetEntries() < minstat) {
        dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
        result->tags_["InsufficientEntries"] = histogram->GetEntries();
        return result;
    }

    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Green);

    for (int i = 1; i <= histogram->GetNbinsX(); ++i) {
        const double binContent = histogram->GetBinContent(i);
        const double binError = histogram->GetBinError(i);
        std::string label = histogram->GetXaxis()->GetBinLabel(i);

        // Check if user has defined a minimum value on this bin
        const double undefined = -99999;
        const double binThreshold = dqm_algorithms::tools::GetFirstFromMap(label.c_str(), config.getParameters(), undefined);
        if ((binThreshold != undefined) && (binContent < binThreshold)) result->status_ = dqm_core::Result::Red;

        if (label.empty()) {
            std::ostringstream oss;
            oss << "Bin_" << i;
            label = oss.str();
        }

        if (publishType & 0x01) result->tags_[label] = binContent;
        if (publishType & 0x02) result->tags_[label + "Error"] = binError;
    }
    return result;
}
void dqm_algorithms::BinContentDump::printDescription(std::ostream& out)
{
    out << "BinContentDump: Dumps the contents of all bins, labelled either with their bin label or with \"Bin_N\". Returns green status.\n"
        "Optional parameters: <label>: The bin with this label must be greater than the given value, otherwise the algorithm returns red status.\n"
        "Optional parameter: PublishType: What to publish. 0 = nothing, 1 = bin contents (default), 2 = bin errors, 3 = both\n"
        "Thresholds: None." << std::endl;
}
