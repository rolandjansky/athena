/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Checks and removes outliers from TH1-Histos and tests the remaining distribution for flatness. Originally by Steffen Schaepe.

#include "dqm_algorithms/OutlierAndFlatnessTest.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"

#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/exceptions.h"
#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/Result.h"

#include "TMath.h"
#include "TH1.h"
#include "TF1.h"
#include "TH1C.h"
#include "TH2C.h"
#include "TClass.h"

#include <memory>

namespace
{
dqm_algorithms::OutlierAndFlatnessTest OutlierAndFlatnessTest("OutlierAndFlatnessTest");
}

dqm_algorithms::OutlierAndFlatnessTest::OutlierAndFlatnessTest(const std::string &name) : m_name(name)
{
    dqm_core::AlgorithmManager::instance().registerAlgorithm(name, this);
}

dqm_algorithms::OutlierAndFlatnessTest *dqm_algorithms::OutlierAndFlatnessTest::clone()
{
    return new OutlierAndFlatnessTest(m_name);
}

dqm_core::Result *dqm_algorithms::OutlierAndFlatnessTest::execute(const std::string &name, const TObject &object, const dqm_core::AlgorithmConfig &config)
{
    if (!object.IsA()->InheritsFrom("TH1")) {
        throw dqm_core::BadConfig(ERS_HERE, name, "does not inherit from TH1");
    }
    std::auto_ptr<TH1> histogram(static_cast<TH1 *>(object.Clone())); // we just checked that this is really a TH1, so we can safely type-cast the pointer
    if (histogram->GetDimension() > 2) {
        throw dqm_core::BadConfig(ERS_HERE, name, "dimension > 2");
    }
    const bool isOneDimensional = (histogram->GetDimension() == 1); // can only be 1 or 2 after the previous check

    // check for minimum statistics

    const double minstat = tools::GetFirstFromMap("MinStat", config.getParameters(), 1);
    if (histogram->GetEntries() < minstat) {
        dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
        result->tags_["InsufficientEntries"] = histogram->GetEntries();
        return result;
    }

    // check if histogram conatins only zeros

    const bool checkZeroContent = static_cast<bool>(tools::GetFirstFromMap("CheckZeroContent", config.getParameters(), 1));
    if (checkZeroContent && (histogram->Integral() == 0) && (histogram->GetEntries() > 0)) {
        ERS_DEBUG(1, "Histogram " << histogram->GetName() << " is filled with zeros!");
        dqm_core::Result* result = new dqm_core::Result(dqm_core::Result::Red);
        result->tags_["Integral"] = histogram->Integral();
        return result;
    }

    // get configuration parameters and set defaults

    const bool checkFlatness = static_cast<bool>(tools::GetFirstFromMap("CheckFlatness", config.getParameters(), 1)); // default: true
    const bool fitCircular = static_cast<bool>(tools::GetFirstFromMap("FitCircular", config.getParameters(), 0)); // default: false
    const bool ignore0 = static_cast<bool>(tools::GetFirstFromMap("Ignore0", config.getParameters(), 1)); // default: true

    const bool checkSigmaDev = static_cast<bool>(tools::GetFirstFromMap("CheckSigmaDev", config.getParameters(), 1)); // default: true
    const double sigmaDev = checkSigmaDev ? tools::GetFirstFromMap("SigmaDev", config.getParameters()) : 0; // only applicable if checkSigmaDev, then mandatory
    const bool dontCountSigmaOutliers = checkSigmaDev ? static_cast<bool>(tools::GetFirstFromMap("DontCountSigmaOutliers", config.getParameters(), 0)) : false; // only applicable if checkSigmaDev, default: false

    const bool checkAbsDev = static_cast<bool>(tools::GetFirstFromMap("CheckAbsDev", config.getParameters(), 0)); // default: false
    const double absDev = checkAbsDev ? tools::GetFirstFromMap("AbsDev", config.getParameters()) : 0; // only applicable if checkAbsDev, then mandatory

    const bool checkRelDev = static_cast<bool>(tools::GetFirstFromMap("CheckRelDev", config.getParameters(), 0)); // default: false
    const double relDev = checkRelDev ? tools::GetFirstFromMap("RelDev", config.getParameters()) : 0; // only applicable if checkRelDev, then mandatory

    const bool checkAbsLimit = static_cast<bool>(tools::GetFirstFromMap("CheckAbsLimit", config.getParameters(), 0)); // default: false
    const double absLimit = checkAbsLimit ? tools::GetFirstFromMap("AbsLimit", config.getParameters()) : 0; // only applicable if checkAbsLimit, then mandatory

    const bool normRef = static_cast<bool>(tools::GetFirstFromMap("DivideByReference", config.getParameters(), 0)); // default: false
    const bool diffRef = static_cast<bool>(tools::GetFirstFromMap("SubtractReference", config.getParameters(), 0)); // default: false

    // if the checks are to be done against a reference take care of this now

    TH1 *refhist = 0;
    if (diffRef || normRef) {
        if (diffRef && normRef) {
            throw dqm_core::BadConfig(ERS_HERE, name, "Can either divide or subtract reference, not both");
        }
        try {
            refhist = static_cast<TH1 *>(config.getReference());
        } catch(dqm_core::Exception &ex) {
            throw dqm_core::BadRefHist(ERS_HERE, name, "Could not retrieve reference");
        }
        if (histogram->GetDimension() != refhist->GetDimension()) {
            throw dqm_core::BadRefHist(ERS_HERE, name, "Wrong dimension of reference");
        }
        if ((histogram->GetNbinsX() != refhist->GetNbinsX()) || (histogram->GetNbinsY() != refhist->GetNbinsY())) {
            throw dqm_core::BadRefHist(ERS_HERE, name, "Non-matching number of bins of reference");
        }

        // scale reference to the same integral as the test hist
        refhist->Sumw2();
        refhist->Scale(histogram->Integral() / refhist->Integral());

        if (diffRef) histogram->Add(refhist, -1.0);
        if (normRef) histogram->Divide(refhist);
    }

    const std::vector<int> range = dqm_algorithms::tools::GetBinRange(histogram.get(), config.getParameters());
    const int xminBin = range[0];
    const int xmaxBin = range[1];
    const int yminBin = range[2];
    const int ymaxBin = range[3];

    int zeroBins = 0;
    for (int i = xminBin; i <= xmaxBin; ++i) {
        for (int j = yminBin; j <= ymaxBin; ++j) {
            if (histogram->GetBinContent(i, j) == 0) ++zeroBins; // count empty or zero bins
        }
    }

    // compute outlier bins by using a simplified Grubb's test without cutting on a t-distribution but by giving an external threshold defining how many standard deviations a value has to differ from the mean to be classified as an outlier or by giving an absolute threshold for classifying values as outliers

    int totalBadBins = 0;
    int totalUncountedBadBins = 0;
    double mean = 0;
    double stddev = 0;
    int currentIteration = 0;
    const int maxIterations = 1000;

    const int nBinsX = histogram->GetNbinsX();
    const int nBinsY = histogram->GetNbinsY();
    std::auto_ptr<TH1> knownOutliers(isOneDimensional ? // keep a record of known outliers (histogram is only used to set boolean flags)
        static_cast<TH1 *>(new TH1C("knownOutliers", "knownOutliers", nBinsX, 0, nBinsX)) : // save the space for the additional frame of underflow/overflow bins
        static_cast<TH1 *>(new TH2C("knownOutliers", "knownOutliers", nBinsX, 0, nBinsX, nBinsY, 0, nBinsY))); // ... but not sure if the savings make up for ugly code

    while (++currentIteration < maxIterations) { // we break earlier if no new outliers are found
        int newBadBins = 0;
        int newUncountedBadBins = 0;

        // compute the arithmetic mean and standard deviation
        int nBins = 0;
        double sum = 0;
        double sumsqr = 0;
        for (int i = xminBin; i <= xmaxBin; ++i) {
            for (int j = yminBin; j <= ymaxBin; ++j) {
                const bool isOutlier = knownOutliers->GetBinContent(i, j);
                if (isOutlier) continue; // skip known outliers
                const double binContent = histogram->GetBinContent(i, j);
                if ((binContent == 0) && ignore0) continue; // skip zero bins if requested
                
                ++nBins;
                sum += binContent;
                sumsqr += binContent * binContent;
            }
        }
        mean   = nBins ? (sum / nBins) : 0; // avoid division by zero
        stddev = nBins ? std::sqrt(sumsqr / nBins - mean * mean) : 0; // one-pass: sigma^2 = < X^2 > - < X >^2

        // check how many bins are out of N sigma range (SigmaDev) or out of the absolute dev (AbsDev) or over absolute limit (AbsLimit), count them as (uncounted) outliers and remove them from the histogram

        if (checkRelDev || checkAbsDev || checkAbsLimit || checkSigmaDev) {
            for (int i = xminBin; i <= xmaxBin; ++i) {
                for (int j = yminBin; j <= ymaxBin; ++j) {
                    const bool isOutlier = knownOutliers->GetBinContent(i, j);
                    if (isOutlier) continue; // skip known outliers
                    const double binContent = histogram->GetBinContent(i, j);
                    if ((binContent == 0) && ignore0) continue; // skip zero bins if requested

                    bool foundOutlier = false;
                    if (checkAbsLimit && !foundOutlier) {
                        if (binContent > absLimit) {
                            ++newBadBins;
                            foundOutlier = true;
                        }
                    }
                    if (checkAbsDev && !foundOutlier) {
                        if (std::fabs(binContent - mean) > absDev) {
                            ++newBadBins;
                            foundOutlier = true;
                        }
                    }
                    if (checkRelDev && !foundOutlier) {
                        if (std::fabs(binContent - mean) > (relDev * mean)) {
                            ++newBadBins;
                            foundOutlier = true;
                        }
                    }
                    if (checkSigmaDev && !foundOutlier) {
                        if (std::fabs(binContent - mean) > (sigmaDev * stddev)) {
                            if (dontCountSigmaOutliers) ++newUncountedBadBins;
                            else ++newBadBins;
                            foundOutlier = true;
                        }
                    }
                    if (foundOutlier) {
                        histogram->SetBinContent(i, j, 0); // ignore this bin in the subsequent fit (below)
                        histogram->SetBinError(i, j, 0); // ignore this bin in the subsequent fit (below)
                        knownOutliers->SetBinContent(i, j, true); // ingnore this bin in the next iteration of this loop
                    }
                } // for (int j)
            } // for (int i)
        } // if (check...)
        if ((newBadBins == 0) && (newUncountedBadBins == 0)) break; // we're done
        totalUncountedBadBins += newUncountedBadBins;
        totalBadBins += newBadBins;
    } // while (protection against endless looping)
    if (currentIteration == maxIterations) {
        throw dqm_core::BadConfig(ERS_HERE, name, "maximum number of iterations reached while searching for outliers"); // should never happen
    }
    if (totalUncountedBadBins > 0) {
        ERS_DEBUG(1, "Histogram " << histogram->GetName() << " has " << totalUncountedBadBins << " bins exceeding sigma limit which are NOT counted as outliers, but which are omitted for calculating mean and stddev");
    }

    // prepare map for results and commit outlier specific parts
    std::map<std::string, double> results; // you can set flagging thresholds on any of these result tags
    results["Number_of_outlier_bins"] = totalBadBins;
    results["Corrected_mean"] = mean;
    results["Corrected_standard_deviation"] = stddev;
    results["Number_of_bins_equal_zero"] = zeroBins;

    if (checkFlatness) {
        if (!isOneDimensional) {
            throw dqm_core::BadConfig(ERS_HERE, name, "cannot check 2D histograms for flatness, please set CheckFlatness = 0");
        }
        TF1 *occupancyFit = 0;

        const double xminAxis = histogram->GetXaxis()->GetBinLowEdge(xminBin);
        const double xmaxAxis = histogram->GetXaxis()->GetBinUpEdge(xmaxBin);
        const double width  = (xmaxAxis - xminAxis);
        const double center = (xmaxAxis + xminAxis) / 2;

        if (fitCircular)  {
            occupancyFit = new TF1("occupancyFit", "[0]+[1]*sin([3]*(x-[4]-[5]))+[2]*cos(2*[3]*(x-[4]-[5]))"); // poor man's Fourier analysis: first-order sin() plus second-order cos()
            occupancyFit->SetParName(0, "constant");
            occupancyFit->SetParName(1, "asym");
            occupancyFit->SetParName(2, "sym");
            occupancyFit->SetParName(3, "scale");
            occupancyFit->SetParName(4, "offset");
            occupancyFit->SetParName(5, "phaseoffset");
            occupancyFit->SetParLimits(5, -0.5 * width, +0.5 * width);
            occupancyFit->FixParameter(3, 2 * TMath::Pi() / width);
            occupancyFit->FixParameter(4, center - width / 2);
        } else {
            occupancyFit = new TF1("occupancyFit", "[0]+[1]*(x-[3])+[2]*(x-[3])*(x-[3])"); // parabola: second-order polynomial
            occupancyFit->SetParName(0, "constant");
            occupancyFit->SetParName(1, "asym");
            occupancyFit->SetParName(2, "sym");
            occupancyFit->SetParName(3, "offset");
            // shift zero for fit to the center of the distribution
            occupancyFit->FixParameter(3, center);
        }

        occupancyFit->SetRange(xminAxis, xmaxAxis);
        histogram->Fit(occupancyFit, "QNR");

        double maxBin = 0;
        double phaseOffset = 0;
        if (fitCircular) {
            // compute maximum deviation from sine and cosine distribution at the point of maximum deviation
            phaseOffset = occupancyFit->GetParameter("phaseoffset");
            maxBin = center + width / 4 + phaseOffset;
        } else {
            // compute maximum deviation from quadratic and linear contibutrions in the fit at the edges of the distribution
            maxBin = center + width / 2;
        }

        const double constant = occupancyFit->GetParameter("constant");

        double asym = 0;
        double sym = 0;

        if (diffRef) {
            asym = occupancyFit->GetParameter("asym");
        } else {
            const double parBuff = occupancyFit->GetParameter("sym");
            occupancyFit->SetParameter("sym", 0);
            asym = std::fabs(occupancyFit->Eval(maxBin) - constant);
            occupancyFit->SetParameter("sym", parBuff);
            asym = constant ? std::fabs(asym / constant) : 0;
        }

        if (diffRef) {
            sym = occupancyFit->GetParameter("sym");
        } else {
            const double parBuff = occupancyFit->GetParameter("asym");
            occupancyFit->SetParameter("asym", 0);
            sym = std::fabs(occupancyFit->Eval(maxBin) - constant);
            occupancyFit->SetParameter("asym", parBuff);
            sym = constant ? std::fabs(sym / constant) : 0;
            if (!fitCircular) {
                sym /= 2;
            }
        }

        const int    ndf = occupancyFit->GetNDF();
        const double chisquare = occupancyFit->GetChisquare();
        const double chisquareNDF = (ndf > 0) ? (chisquare / ndf) : 0; // prevent fits with no degrees of freedom from returning NaN or INF.

        // commit flatness specific results to result map
        results["Max_rel_sym_deviation"] = sym;
        results["Max_rel_asym_deviation"] = asym;
        results["Chisquare_ndf"] = chisquareNDF;
        results["Phase_offset"] = phaseOffset;

        delete occupancyFit;
    } // if (checkFlatness)

    // compare with given thresholds and compute DQ status
    return tools::MakeComparisons(results, config.getGreenThresholds(), config.getRedThresholds()); // this utility function handles all threshold comparisons
}

void dqm_algorithms::OutlierAndFlatnessTest::printDescription(std::ostream& out)
{
    out << m_name << ": Checks TH1-inherited histograms for bins which lie either Nsigma or AbsDev away from the mean (by default excludes bins with zero entries) or which exceed a given limit and removes them from the distribution.\n"
        "Remaining (corrected for outliers) distribution is fitted with either a quadratic or a sinusoidal function (option FitCircular) and symmetric and asymmetric deviations from a flat distribution are computed by evaluating the quadratic and linear fit contributions respectively.\n"
        "Parameters:\n"
        "\tFitCircular:\tFit sinoidal function instead of quadratic. This is usefull for distributions expected to show circular behaviour (e.g. phi distributions) (default 0).\n"
        "\tCheckFlatness:\tThis switch can be used to disable the flatness test and just check for outliers (default 1).\n"
        "\tCheckAbsDev:\tCheck for absolute deviation from mean (default 0).\n"
        "\tCheckRelDev:\tCheck for relative deviation from mean (default 0).\n"
        "\tCheckAbsLimit:\tCheck for values exceeding given absolute limit (default 0).\n"
        "\tCheckSigmaDev:\tCheck for deviation in units of standard deviations from mean (default 1).\n"
        "\tDontCountSigmaDev:\tBins deviating by a given number of standard deviations are removed from the distribution for mean and flatness computation, but not counted as outliers (default 0).\n"
        "\tCheckZeroContent:\tIf set to 1 the algorithm will check whether the histogram is filled with only zeros and return red in this case. WARNING this uses the TH1::Integral function which seems to be problematic in Lightweigth Histograms. (default 1).\n"
        "\tMinStat:\tMinimum Statistics for the histogram to be checked (default 1).\n"
        "\txmin and/or xmax:\tRestrict all counting and fitting to this x-axis interval (default: full axis range).\n"
        "\tIgnore0:\tBins with 0 content are ignored for outlier computation (ignored for flatness-test anyway) (default 1).\n"
        "\tSigmaDev:\tNumber of Standard Deviations a single bin has to differ from the mean of the distribution to be classified as outlier. Has to be given if \"CheckSigmaDev\" is set to 1. (default 5)\n"
        "\tAbsDev:\tAbsolute value a single bin has to differ from the mean of the distribution to be classified as outlier.  Has to be given if \"CheckAbsDev\" is set to 1.\n"
        "\tRelDev:\tFraction of the mean value a single bin has to differ from the mean of the distribution to be classified as outlier (i.e. a bin is classified as outlier if it deviates more than |AbsDev*mean| from the mean of the distribution).  Has to be given if \"CheckRelDev\" is set to 1.\n"
        "\tAbsLimit:\tAbsolute limit a single bin has to exceed to be classified as outlier.  Has to be given if \"CheckAbsLimit\" is set to 1.\n"
        "\tDivideByReference:\tDivide test histogram by reference histogram and perform checks on the resulting ratio. (default 0)\n"
        "\tSubtractReference:\tSubtract reference histogram from test histogram and perform checks on the resulting difference. Carefull! This yields pretty unstable results for the flatness tests! (default 0)\n"
        "Thresholds:\n"
        "\tNumber_of_outlier_bins:\tNumber of bins classified as outliers using the given thresholds.\n"
        "\tCorrected_mean:\tMean of distribution ignoring outliers.\n"
        "\tCorrected_standard_deviation:\tStandard Deviation of distribution ignoring outliers.\n"
        "\tMax_rel_sym_deviation:\tMaximum relative symmetric deviation from a flat distribution.\n"
        "\tMax_rel_asym_deviation:\tMaximum relative asymmetric deviation from a flat distribution.\n"
        "\tNumber_of_bins_equal_zero:\tNumber of Bins with zero content." << std::endl;
}
