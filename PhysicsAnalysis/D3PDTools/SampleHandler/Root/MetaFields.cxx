/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SampleHandler/MetaFields.h>

#include <SampleHandler/MetaNames.h>

//
// method implementations
//

namespace SH
{
  const std::string MetaFields::gridName           = "nc_grid";
  const std::string MetaFields::gridFilter         = "nc_grid_filter";
  const std::string MetaFields::gridFilter_default = "*.root*";
  const std::string MetaFields::gridSourceOptions  = "nc_grid_source_options";
  const std::string MetaFields::sourceSample       = "nc_source";
  const std::string MetaFields::treeName           = "nc_tree";
  const std::string MetaFields::treeName_default   = "CollectionTree";
  const std::string MetaFields::crossSection       = "nc_xs";
  const std::string MetaFields::crossSectionRelUncertainty = "nc_xs_rel_err";
  const std::string MetaFields::numEvents          = "nc_nevt";
  const std::string MetaFields::numEventsPerFile   = "nc_nevtfile";
  const std::string MetaFields::sampleName         = MetaNames::sampleName ();
  const std::string MetaFields::lumi               = "lumi";
  const std::string MetaFields::kfactor            = "kfactor";
  const std::string MetaFields::isData             = "is_data";
  const std::string MetaFields::filterEfficiency   = "filter_efficiency";

  /// warning: this has to be synchronized with EventLoop::Job
  const std::string MetaFields::cacheSize          = "nc_cache_size";
}
