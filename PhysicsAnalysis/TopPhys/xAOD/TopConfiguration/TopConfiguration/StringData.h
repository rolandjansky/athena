/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRINGDATA_H_
#define STRINGDATA_H_

#include <string>

namespace top {

/**
 * @brief A little helper to hold the data, a little information that's useful
 * to humans and a flag (true) if the settings has been set.
 */
struct StringData {
    ///The data set by the user
    std::string m_data;

    ///A helpful message that tells the user what's expected here
    std::string m_human_explanation;

    ///A flag that lets us know if the user has set something or not.
    bool m_set;
};

}

#endif
