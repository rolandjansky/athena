/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DL2_HIGHLEVEL_TOOLS_H
#define DL2_HIGHLEVEL_TOOLS_H

// local includes
#include "FlavorTagDiscriminants/DL2.h"

// STL includes
#include <regex>

namespace FlavorTagDiscriminants {

  // ____________________________________________________________________
  // High level adapter stuff
  //
  // We define a few structures to map variable names to type, default
  // value, etc. These are only used by the high level interface.
  //
  typedef std::vector<std::pair<std::regex, EDMType> > TypeRegexes;
  typedef std::vector<std::pair<std::regex, std::string> > StringRegexes;
  typedef std::vector<std::pair<std::regex, SortOrder> > SortRegexes;
  typedef std::vector<std::pair<std::regex, TrackSelection> > TrkSelRegexes;

  // Function to map the regular expressions + the list of inputs to a
  // list of variable configurations.
  std::vector<DL2InputConfig> get_input_config(
    const std::vector<std::string>& variable_names,
    const TypeRegexes& type_regexes,
    const StringRegexes& default_flag_regexes);

  // Since the names of the inputs are stored in the NN config, we
  // also allow some user-configured remapping. Items in replaced_vars
  // are removed as they are used.
  void remap_inputs(std::vector<lwt::Input>& nn,
                    std::map<std::string, std::string>& replaced_vars,
                    std::map<std::string, double>& defaults);

  // Function to map the regex + list of inputs to variable config,
  // this time for sequence inputs.
  std::vector<DL2TrackSequenceConfig> get_track_input_config(
    const std::vector<std::pair<std::string, std::vector<std::string>>>& names,
    const TypeRegexes& type_regexes,
    const SortRegexes& sort_regexes,
    const TrkSelRegexes& select_regexes);

  // replace strings for flip taggers
  void rewriteFlipConfig(lwt::GraphConfig&, const StringRegexes&);
  void flipSequenceSigns(lwt::GraphConfig&, const std::regex&);
}
#endif
