/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTUtils/HTTFunctions.h"





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
