/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__META_FIELDS_H
#define SAMPLE_HANDLER__META_FIELDS_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <string>

namespace SH
{
  /// \brief This class contains the known meta-data names.
  ///
  /// These are all the meta-data fields known within SampleHandler
  /// and EventLoop.  The main purpose of this is to avoid typos, or
  /// "magical numbers" inside the code.  It's all collected inside a
  /// single class to make it easier to create a dictionary.
  struct MetaFields
  {
    /// \brief the field containing the name of the dataset on
    ///   the grid
    static const std::string gridName;

    /// \brief the field containing the file filter for the
    ///   dataset on the grid
    static const std::string gridFilter;

    /// \brief the default value for \ref gridFilter
    static const std::string gridFilter_default;

    /// \brief the field containing the source options for datasets on
    /// the grid
    static const std::string gridSourceOptions;

    /// \brief the original sample from which this sample was
    ///   derived
    static const std::string sourceSample;

    /// \brief the name of the tree in the sample
    static const std::string treeName;

    /// \brief the default value of \ref treeName
    static const std::string treeName_default;

    /// \brief the cross section field
    static const std::string crossSection;

    /// \brief the relative uncertainty on the cross section
    static const std::string crossSectionRelUncertainty;

    /// \brief the number of events
    static const std::string numEvents;

    /// \brief the number of events in each file
    static const std::string numEventsPerFile;

    /// \brief the name of the sample
    static const std::string sampleName;

    /// \brief the size of the TTreeCache
    static const std::string cacheSize;

    /// \brief the luminosity of the sample
    static const std::string lumi;

    /// \brief whether the sample is data
    static const std::string isData;

    /// \brief the k-factor of the sample
    static const std::string kfactor;

    /// \brief the filter efficiency of the sample
    static const std::string filterEfficiency;
  };
}

#endif
