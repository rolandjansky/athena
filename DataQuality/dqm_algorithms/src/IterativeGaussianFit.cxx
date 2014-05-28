/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/AlgorithmManager.h"
#include "dqm_algorithms/IterativeGaussianFit.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"
#include "ers/ers.h"

#include "TClass.h"
#include "TObject.h"
#include "TH1.h"
#include "TF1.h"

#include <memory>
#include <iostream>
#include <cmath>

namespace { // anonymous
dqm_algorithms::IterativeGaussianFit instance("IterativeGaussianFit"); // global instance to have this algorithm registered with the manager
}

dqm_algorithms::IterativeGaussianFit::IterativeGaussianFit(const std::string &name): m_name(name)
{
    dqm_core::AlgorithmManager::instance().registerAlgorithm(m_name, this);
}

dqm_algorithms::IterativeGaussianFit::~IterativeGaussianFit()
{
    /* noop */
}

dqm_algorithms::IterativeGaussianFit *dqm_algorithms::IterativeGaussianFit::clone()
{
    return new IterativeGaussianFit(m_name); // somebody else must delete this
}

dqm_core::Result *dqm_algorithms::IterativeGaussianFit::execute(const std::string &name, const TObject &object, const dqm_core::AlgorithmConfig &config)
{
    if (!object.IsA()->InheritsFrom("TH1")) { // this could also be TH2 or TH3
        throw dqm_core::BadConfig(ERS_HERE, name, "histogram does not inherit from TH1");
    }
    const TH1 &histogram = dynamic_cast<const TH1 &>(object); // should be possible after the previous check (otherwise we'd get a std::bad_cast)
    if (histogram.GetDimension() > 1) { // sorry, we only do one-dimensional fits here
        throw dqm_core::BadConfig(ERS_HERE, name, "histogram has more than one dimension");
    }
    const int minstat = dqm_algorithms::tools::GetFirstFromMap("MinStat", config.getParameters(), 1000);
    if (histogram.GetEffectiveEntries() < minstat) { // you'd better have at least some entries, or the fit may yield strange results
        dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
        result->tags_["InsufficientEffectiveEntries"] = histogram.GetEffectiveEntries();
        return result;
    }
    const double sigmaRange = dqm_algorithms::tools::GetFirstFromMap("SigmaRange", config.getParameters()); // mandatory parameter
    if (sigmaRange <= 0) {
        throw dqm_core::BadConfig(ERS_HERE, name, "SigmaRange must be greater than zero.");
    }
    if (sigmaRange < 1) {
        ERS_DEBUG(1, "Careful, your SigmaRange = " << sigmaRange << " is rather small"); // or should we just throw an exception?
    }
    const double meanNominal = dqm_algorithms::tools::GetFirstFromMap("MeanNominal", config.getParameters(), 0); // optional parameter
    const double sigmaMax = dqm_algorithms::tools::GetFirstFromMap("SigmaMax", config.getParameters(), 0); // optional parameter
    if (sigmaMax < 0) {
        throw dqm_core::BadConfig(ERS_HERE, name, "SigmaMax must be greater than zero."); // zero means unlimited
    }

    std::auto_ptr<TH1> h(static_cast<TH1 *>(histogram.Clone())); // we need a non-const copy of the object passed to this function
    // under normal conditions we would have used the copy constructor here, but the one provided by ROOT gives nothing but a segmentation fault
    std::auto_ptr<TF1> f(new TF1("f", "gaus")); // we use auto_ptrs to avoid cleaning up manually, because this function has some emergency exits
    if (sigmaMax) {
        f->SetParLimits(2, 0, sigmaMax); // limit the possible range of sigma to avoid meaningless fit results
        f->SetParameter(2, sigmaMax / 2); // set some arbitrary initial value that falls into the allowed range, otherwise Minuit will complain
        f->SetRange(meanNominal - sigmaMax, meanNominal + sigmaMax); // very first guess, let's hope that this is not completely off
    }

    const unsigned int maxIterations = 10; // don't do more than this number of iterations, otherwise terminate abnormally
    const double sufficientPrecision = 0.01; // terminate the iteration as soon as the relative change of sigma is less than this

    unsigned int i = 0; // iteration counter
    while (true) { // don't worry, there's a break down there
        const double sigmaOld = (i == 0) ? 0 : f->GetParameter(2); // compare old and new sigma to terminate the iteration
        std::string            fitOptions("INQ"); // integrate over each bin, do not draw the function, do not print messages
        if (i > 0 || sigmaMax) fitOptions.append("R"); // do the initial fit without a range restriction, but only if sigma is not limited
        if (sigmaMax)          fitOptions.append("B"); // if a limit for sigma was set, then use it (but always setting this would not work)

        const int fitStatus = h->Fit(f.get(), fitOptions.c_str()); // you hardly notice you've got an auto_ptr, except in cases like this
        if (fitStatus) { // see the documentation of TH1::Fit about the meaning of this status flag ... it should be zero if the fit went well
            ERS_DEBUG(1, "Fit failed in iteration " << i << ", status = " << fitStatus);
            dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Red);
            if (fitStatus < 0) result->status_ = dqm_core::Result::Undefined; // this usually happens because of an empty histogram (normally already caught by MinStat)
            result->tags_["Iteration"] = i;
            result->tags_["FitStatus"] = fitStatus;
            return result;
        }

        const double mean = f->GetParameter(1);
        const double sigma = f->GetParameter(2);
        ERS_DEBUG(2, "Iteration " << i << ": mean = " << mean << ", sigma = " << sigma);
        f->SetRange(mean - sigmaRange * sigma, mean + sigmaRange * sigma); // this is the important feature!

        if (sigma == 0) { // maybe this is not really fatal, but at least it shows that something strange is going on
            ERS_DEBUG(1, "Fit yielded sigma = 0 in iteration " << i);
            dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Red);
            result->tags_["Iteration"] = i;
            result->tags_["Mean"] = mean;
            result->tags_["Sigma"] = sigma;
            return result;
        } // at least we're now sure that we can safely divide by sigma

        if (sigma == sigmaMax) { // we've hit the parameter limit, so the fit cannot yield sensible results
            ERS_DEBUG(1, "Fit result for sigma is at the limit " << sigmaMax);
            dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Red);
            result->tags_["Iteration"] = i;
            result->tags_["Sigma"] = sigma;
            result->tags_["SigmaMax"] = sigmaMax;
            return result;
        }

        if ((std::fabs(sigma - sigmaOld) / sigma) < sufficientPrecision) break; // this is good enough for us

        if (i >= maxIterations) { // stop here, but we should have reached the break (above) much earlier
            ERS_DEBUG(1, "Sorry, this is not taking us anywhere: the iteration does not seem to converge");
            dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined); // may sometimes happen by chance, especially if the distribution is rather narrow
            result->tags_["Iteration"] = i;
            result->tags_["SigmaOld"] = sigmaOld;
            result->tags_["SigmaNew"] = sigma;
            return result;
        }
        ++i; // next iteration
    } // while

    const int    ndf     = f->GetNDF();
    const double chi2    = f->GetChisquare();
    const double chi2NDF = (ndf > 0) ? (chi2 / ndf) : 0; // prevent fits with no degrees of freedom from returning NaN or INF.

    std::map<std::string, double> resultValues; // all elements in this map will be published, and they can also be used as thresholds
    resultValues["Constant"]      = f->GetParameter(0); // hm, does the constant tell us anything?
    resultValues["ConstantError"] = f->GetParError(0); // and does anybody care about the errors of the fit parameters?
    resultValues["Mean"]          = f->GetParameter(1);
    resultValues["MeanError"]     = f->GetParError(1);
    resultValues["Sigma"]         = f->GetParameter(2);
    resultValues["SigmaError"]    = f->GetParError(2);
    resultValues["Chi2NDF"]       = chi2NDF; // for the sake of completeness
    resultValues["Probability"]   = f->GetProb(); // maybe somebody wants to set a threshold on this?
    resultValues["MeanDeviation"] = std::fabs(f->GetParameter(1) - meanNominal); // useful to detect shifts from the expected value
    return dqm_algorithms::tools::MakeComparisons(resultValues, config.getGreenThresholds(), config.getRedThresholds()); // this function does all the magic
}

void dqm_algorithms::IterativeGaussianFit::printDescription(std::ostream& out)
{
    out << m_name << ": Applies a Gaussian fit over a limited fit range.\n"
        "    The fit range is iteratively determined in terms of Gaussian sigmas on each side of the mean.\n"
        "    The algorithm stops if sigma doesn't change anymore, or after a maximum number of iterations.\n"
        "Mandatory parameter: SigmaRange - over plus/minus how many sigmas the fit should be applied.\n"
        "Optional parameter: MeanNominal - the expected mean value, used for calculating MeanDeviation (default 0)\n"
        "Optional parameter: SigmaMax - limit the maximum possible fit result for sigma (default unlimited)\n"
        "Optional parameter: MinStat - minimum number of entries required in the histogram (default 1000).\n"
        "Returned values: Constant, Mean, and Sigma of the fitted Gaussian including their errors, Chi2NDF and Probability.\n"
        "    MeanDeviation - the absolute difference of Mean and MeanNominal, useful for detecting shifts.\n"
        "Thresholds can be set on any of the returned values, or a combination thereof." << std::endl;
    // xmin/xmax parameters wouldn't make sense, because it's a feature of this algorithm to determine these values itself
}
