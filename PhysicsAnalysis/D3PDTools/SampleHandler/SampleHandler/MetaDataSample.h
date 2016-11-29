/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__META_DATA_SAMPLE_H
#define SAMPLE_HANDLER__META_DATA_SAMPLE_H

#include <TObject.h>

namespace SH
{
  /// \brief all the meta-data fields that we may try to read
  ///   from AMI.
  struct MetaDataSample : public TObject
  {
    /// \brief the name of the sample/dataset
  public:
    std::string name;

    /// \brief the database we queried this information from or
    ///   the empty string if we didn't yet query
  public:
    std::string source;

    /// \brief any warning messages from the database query
  public:
    std::string warning;

    /// \brief a flag that is set if the sample is unknown
  public:
    bool unknown;

    /// \brief 1 for data, 0 for MC, or -1 if this is not known.
  public:
    int isData;

    /// \brief 1 for samples that are still running, 0 for
    ///   finished samples, or -1 if this is not known.
  public:
    int running;

    /// \brief the luminosity of the sample or -1 if the
    ///   luminosity is unknown
  public:
    float luminosity;

    /// \brief the cross section of the sample or -1 if the cross
    ///   section is unknown
  public:
    float crossSection;

    /// \brief the number of events for the sample or -1 if the
    ///   number of events is unknown
  public:
    Long64_t nevents;

    /// \brief the k-factor for the sample or -1 if the k-factor
    ///   is not known
  public:
    float kfactor;

    /// \brief the filter efficiency or -1 if the filter
    ///   efficiency is not known
  public:
    float filterEfficiency;

    
    /// \brief standard constructor
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    MetaDataSample (const std::string& val_name = "");

    ClassDef (MetaDataSample, 1);
  };
}

#endif
