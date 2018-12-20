/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_GLOBAL_HH
#define SAMPLE_HANDLER_GLOBAL_HH

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module provides a lot of global definitions, forward
/// declarations and includes that are used by all modules.  As such it
/// doesn't fall into the user vs. expert classification.  This module
/// is considered to be in the pre-alpha stage.

namespace SH
{
  class DiskList;
  class DiskListEOS;
  class DiskListLocal;
  class DiskListSRM;
  class DiskListXRD;
  class DiskOutput;
  class DiskOutputLocal;
  class DiskOutputXRD;
  class DiskWriter;
  class DiskWriterLocal;
  class DiskWriterXRD;
  class Meta;
  template<class T> class MetaData;
  struct MetaDataQuery;
  struct MetaDataSample;
  struct MetaFields;
  struct MetaNames;
  class MetaObject;
  template<class T> class MetaVector;
  class Sample;
  class SampleComposite;
  class SampleGrid;
  class SampleHandler;
  class SampleHist;
  class SampleLocal;
  class SampleMeta;
  class SamplePtr;
  struct ScanDir;
  class TagList;
}

#endif
