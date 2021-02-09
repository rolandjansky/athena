/**
 * How to use in C++
 * =================
 * 
 * Just include this header. It depends on ROOT
 * 
 * Example:
 * 
 *     #include <iostream>
 *     #include "widthestimators.h"
 * 
 *     using widthestimators;
 * 
 *     int main()
 *     {
 *       TH1F h("h", "h", 100, -5, 5);
 *       h.FillRandom("gaus");
 *       std::cout << "s68 : " << binned::s68(h) << std::endl;
 *       return 0;
 *     }
 * 
 **/

#include <utility>
#include <vector>
#include <numeric>
#include <iostream>
#include <algorithm>

namespace widthestimators
{
    const double PROB_1SIGMA = 0.683;

    namespace binned
    {
        /** cumulative sum (e.g. (non-normalized) empirical CDF) */
        // need to use template, and not TH1, since GetArray is implemeted for each specialization
        // but not in TH1
        template <typename T>
        std::vector<double> get_cum_sum(const T &histo, bool normed = false)
        {
            // use double since it can weighted
            std::vector<double> cum_sum(histo->GetNbinsX() + 2);

            std::partial_sum(
                histo->GetArray(),
                histo->GetArray() + histo->GetNbinsX() + 2,
                cum_sum.begin());

            if (normed)
            {
                const double last_value = cum_sum.back();
                std::transform(cum_sum.begin(), cum_sum.end(), cum_sum.begin(),
                               [&last_value](auto &c) { return c / last_value; });
            }

            return cum_sum;
        }

        /** return smallest window (first value, second value) containing prob fraction of the events */
        template <typename T>
        std::pair<double, double> find_window(const T &histo, double prob = PROB_1SIGMA)
        {
            if (histo->GetNbinsX() <= 3)
            {
                return std::make_pair<double>(0, 0);
            }

            std::vector<double> cum_sum = get_cum_sum(histo, true);
            double min_width = histo->GetBinCenter(histo->GetNbinsX()) - histo->GetBinCenter(1);
            std::pair<int, int> window_bin(0, histo->GetNbinsX() + 1);

            for (int ibin = 0; ibin != histo->GetNbinsX() + 1; ++ibin)
            {
                const double target_prob = prob + cum_sum[ibin];
                if (target_prob > 1)
                    break;

                const auto up_it = std::upper_bound(cum_sum.begin() + ibin, cum_sum.end(), target_prob);
                const auto end_bin = std::distance(cum_sum.begin(), up_it);
                const double width = histo->GetBinCenter(end_bin) - histo->GetBinCenter(ibin);

                if (width < min_width)
                {
                    min_width = width;
                    window_bin.first = ibin;
                    window_bin.second = end_bin;
                }
            }
            return std::make_pair<double>(histo->GetBinCenter(window_bin.first), histo->GetBinCenter(window_bin.second));
        }

        template <typename T>
        double smallest_interval(const T &histo, double prob = PROB_1SIGMA)
        {
            const auto window = find_window(histo, prob);
            return 0.5 * (window.second - window.first);
        }

        template <typename T>
        double s68(const T &histo) { return smallest_interval(histo, PROB_1SIGMA); }

        template <typename T>
        double s90(const T &histo) { return smallest_interval(histo, 0.90); }

    } // namespace binned
}     // namespace widthestimators