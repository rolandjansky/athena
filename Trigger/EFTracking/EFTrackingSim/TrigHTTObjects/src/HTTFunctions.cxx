/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTFunctions.h"


/**
 * Given a list of sizes (of arrays), generates a list of all combinations of indices to
 * index one element from each array.
 *
 * For example, given [2 3], generates [(0 0) (1 0) (0 1) (1 1) (0 2) (1 2)].
 *
 * This basically amounts to a positional number system of where each digit has its own base.
 * The number of digits is sizes.size(), and the base of digit i is sizes[i]. Then all combinations
 * can be uniquely represented just by counting from [0, nCombs).
 *
 * For a decimal number like 1357, you get the thousands digit with n / 1000 = n / (10 * 10 * 10).
 * So here, you get the 0th digit with n / (base_1 * base_2 * base_3);
 */
std::vector<std::vector<int>> getComboIndices(std::vector<size_t> const & sizes)
{
    size_t nCombs = 1;
    std::vector<size_t> nCombs_prior(sizes.size());
    std::vector<int> temp(sizes.size(), 0);

    for (size_t i = 0; i < sizes.size(); i++)
    {
        if (sizes[i] > 0)
        {
            nCombs_prior[i] = nCombs;
            nCombs *= sizes[i];
        }
        else temp[i] = -1;
    }

    std::vector<std::vector<int>> combos(nCombs, temp);

    for (size_t icomb = 0; icomb < nCombs; icomb++)
    {
        size_t index = icomb;
        for (size_t isize = sizes.size() - 1; isize < sizes.size(); isize--)
        {
            if (sizes[isize] == 0) continue;
            combos[icomb][isize] = static_cast<int>(index / nCombs_prior[isize]);
            index = index % nCombs_prior[isize];
        }
    }

    return combos;
}


/**
 * This function is used to calculate RMS95 value for 1D histograms.
 * It was ported from 
 * https://gitlab.cern.ch:8443/fpastore/l1tracksim/-/blob/master/PatRec/macros/FraMacros/Functions.cxx#L253
 * See more details there.
 */
double rms95(TH1 const * h)
{
    double const frac = 0.95;
    double entries = h->Integral(0, h->GetNbinsX() + 1);

    // Not enough entries for this fraction, i.e. we want 0.95 then need 5% to be 1 events, so need at least 20 events.
    if ((1.0 - frac) * entries < 1 || entries == 0) return h->GetRMS();

    TH1* h_tmp = dynamic_cast<TH1*>(h->Clone());
    h_tmp->GetXaxis()->SetRange(1, h_tmp->GetNbinsX());

    int meanbin  = h->GetXaxis()->FindBin(h_tmp->GetMean());
    int lowerbin = meanbin;
    int upperbin = meanbin;

    double sum = h->GetBinContent(meanbin);
    double lowerfrac = 0;
    double upperfrac = 0;

    int i = 1;
    while (true) {
        int this_lowerbin = meanbin - i;
        int this_upperbin = meanbin + i;
        if (this_lowerbin < 1 || this_upperbin > h->GetNbinsX()) break;

        sum += h_tmp->GetBinContent(this_lowerbin) + h_tmp->GetBinContent(this_upperbin);
        if (sum >= entries * frac) break;

        lowerfrac = sum / entries;
        lowerbin = this_lowerbin;
        upperbin = this_upperbin;

        i++;
    }
    upperfrac = sum / entries;

    if (upperfrac == lowerfrac) return h->GetRMS();

    double rms_lower = 0;
    double rms_upper = 0;

    h_tmp->GetXaxis()->SetRange(lowerbin, upperbin);
    rms_lower = h_tmp->GetRMS();

    h_tmp->GetXaxis()->SetRange(lowerbin - 1, upperbin + 1);
    rms_upper = h_tmp->GetRMS();

    double rms = rms_lower + (frac - lowerfrac) * (rms_upper - rms_lower) / (upperfrac - lowerfrac);

    return rms * 1.1479538518;
}

