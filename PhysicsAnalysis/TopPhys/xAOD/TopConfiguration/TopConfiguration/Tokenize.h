/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: Tokenize.h
// Description:
// Author: Fabian Wilk
// Created: Tue Aug 11 10:12:30 2015
//
// (c) by The ATLAS Collaboration
// (c) by Fabian Wilk <mail@fabianwilk.de>
//
// This file is licensed under a Creative Commons Attribution-ShareAlike 4.0
// International License.
//
// You should have received a copy of the license along with this work.
// If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.

#ifndef _UTILS_TOKENIZE_H_
#define _UTILS_TOKENIZE_H_

#include <string>

namespace top{
    /*!
    * @brief Tokenize an input string using a set of delimiters.
    * @tparam Container The type of the output container.
    * @param input The input string which is to be tokenized.
    * @param output The container onto which the tokens are to be appended. Must
    *  support an emplace_back function which takes as pointer to const char (the
    *  string data) and a std::size_t (the string length).
    *
    * For maximum performance use a non-allocating string in the out container,
    * e.g. use `Container = std::vector<std::string_view>` in c++14.
    */
    template <typename Container>
    void tokenize(const std::string & input, Container & output,
                  const std::string & delimiters = " ", bool trim_empty = false) {
        const auto size = input.size();
        std::string::size_type pos{0};
        std::string::size_type last_pos{0};

        while (last_pos < size + 1) {
            pos = input.find_first_of(delimiters, last_pos);
            if (pos == std::string::npos) {
                pos = size;
            }

            if (pos != last_pos || not trim_empty){
                output.emplace_back(input.data() + last_pos, pos - last_pos);
            }

            last_pos = pos + 1;
        }
    }
}

#endif /* _UTILS_TOKENIZE_H_ */
