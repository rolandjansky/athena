/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SELECTIONCONFIGURATIONDATA_H_
#define SELECTIONCONFIGURATIONDATA_H_

#include <string>
#include <vector>

namespace top {

/**
 * @brief Some configuration data to setup the selections.
 *
 * My understanding is that we'll only have a single stream of data next year,
 * so you might want to run ee, mumu and emu in the same job (i.e. selections).
 */
struct SelectionConfigurationData {
    ///The name of the selection, e.g. ee
    std::string m_name;

    ///The cuts for this particular selection stored as strings, as read from the text file
    std::vector<std::string> m_cutnames;
};

}

#endif
