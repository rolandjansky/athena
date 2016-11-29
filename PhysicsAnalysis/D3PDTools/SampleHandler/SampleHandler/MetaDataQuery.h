/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__META_DATA_QUERY_H
#define SAMPLE_HANDLER__META_DATA_QUERY_H

#include <SampleHandler/Global.h>

#include <TObject.h>
#include <SampleHandler/MetaDataSample.h>

namespace SH
{
  /// \brief an object containing the result of an AMI meta-data query
  struct MetaDataQuery : public TObject
  {
    /// \brief any general messages from the database query, e.g. a
    ///   link to the web interface, documentation, etc.
  public:
    std::string messages;

    /// \brief the individual samples queried
  public:
    std::vector<MetaDataSample> samples;

    /// \brief add another sample
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    /// \par Rationale
    ///   this is mainly for use by python
  public:
    void addSample (const MetaDataSample& sample);

    ClassDef (MetaDataQuery, 1);
  };
}

#endif
