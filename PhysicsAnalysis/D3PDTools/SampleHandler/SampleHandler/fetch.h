/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef META_FETCHER__FETCH_H
#define META_FETCHER__FETCH_H

#include <SampleHandler/Global.h>

namespace SH
{
  /// effects: fetch information on all the samples/datasets specified
  /// guarantee: basic
  void fetchMetaData (MetaDataQuery& query);

  /// effects: fetch information on all the samples in the sample
  ///   handler.  if override is specified it will override the existing
  ///   meta-data fields, otherwise they will only be used if the given
  ///   fields do not yet exist
  /// guarantee: basic
  void fetchMetaData (SampleHandler& sh, bool override);
}

#endif
