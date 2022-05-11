// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTNNMap.cxx
 * @author Elliott Cheu
 * @date April 27, 2021
 * @brief See HTTNNMap.h
 */

#include "TrigHTTMaps/HTTNNMap.h"

#include <string>
#include <sstream>
#include <exception>
#include <AsgMessaging/MessageCheck.h>

using namespace std;
using namespace asg::msgUserCode;

///////////////////////////////////////////////////////////////////////////////
// Constructor/Desctructor
///////////////////////////////////////////////////////////////////////////////



HTTNNMap::HTTNNMap(const std::string & filepath) 
{

    // Open file with NN weights
    std::string weightsFileName = filepath;
    std::ifstream input_cfg(weightsFileName.c_str());
    if (input_cfg.is_open()) 
      ANA_MSG_INFO("Opened file: " << weightsFileName);
    else {
        ANA_MSG_FATAL("Unable to open file: " << weightsFileName);
        throw ("HTTNNMap could not open " + weightsFileName);
    }

    auto cfg = lwt::parse_json_graph( input_cfg );
    m_lwnn_map = std::shared_ptr<lwt::LightweightGraph> (new lwt::LightweightGraph( cfg ));
}

// Returns pointer to NN weighting map

std::shared_ptr<lwt::LightweightGraph> HTTNNMap::getNNMap() const {return m_lwnn_map;}

