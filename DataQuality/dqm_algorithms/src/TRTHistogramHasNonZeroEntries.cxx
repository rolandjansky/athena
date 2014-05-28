/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file TRTHistogramHasNonZeroEntries.cxx checks if histogram has entries that are not zero and returns dqm_core::Result
 * \author Peter Cwetanski
 */

#include "dqm_algorithms/TRTHistogramHasNonZeroEntries.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/exceptions.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "dqm_core/Result.h"
#include "ers/ers.h"

#include "TH1.h"
#include "TF1.h"
#include "TClass.h"

#include <cmath>
#include <iostream>
#include <map>

static dqm_algorithms::TRTHistogramHasNonZeroEntries staticInstance;

namespace dqm_algorithms
{

TRTHistogramHasNonZeroEntries::TRTHistogramHasNonZeroEntries(): name("TRTHistogram_Has_NonZero_Entries")
{
    dqm_core::AlgorithmManager::instance().registerAlgorithm(name, this);
}

TRTHistogramHasNonZeroEntries::~TRTHistogramHasNonZeroEntries()
{
}

dqm_core::Algorithm *TRTHistogramHasNonZeroEntries::clone()
{
    return new TRTHistogramHasNonZeroEntries(*this);
}

dqm_core::Result *TRTHistogramHasNonZeroEntries::execute(const std::string &name, const TObject &object, const dqm_core::AlgorithmConfig &config)
{
    TH1 *histogram;

    if (object.IsA()->InheritsFrom("TH1")) {
        histogram = (TH1*) &object;
        if (histogram->GetDimension() > 3) {
            throw dqm_core::BadConfig(ERS_HERE, name, "dimension > 3 ");
        }
    } else {
        throw dqm_core::BadConfig(ERS_HERE, name, "does not inherit from TH1");
    }

    const double minstat = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), -1);

    if (histogram->GetEntries() < minstat) {
        // ERS_INFO("Histogram does not satisfy MinStat requirement " << histogram->GetName());
        dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
        result->tags_["InsufficientEntries"] = histogram->GetEntries();
        return result;
    }

    dqm_core::Result *result = new dqm_core::Result();

    if (histogram->GetEntries() == 0 || histogram->Integral() == 0) {
        ERS_DEBUG(1, "Histogram " << histogram->GetName() << " is Empty");
        result->status_ = dqm_core::Result::Red;
        result->tags_["Integral"] = histogram->Integral();
    } else {
        ERS_DEBUG(1, "Histogram " << histogram->GetName() << " is Not Empty");
        result->status_ = dqm_core::Result::Green;
        result->tags_["Integral"] = histogram->Integral();
    }
    return result;
}

void TRTHistogramHasNonZeroEntries::printDescription(std::ostream& out)
{
    out << name << ": Checks if histogram has entries that are not zero." << std::endl;
}

} // namespace dqm_algorithms
