/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_GLOBAL_HH
#define EVENT_LOOP_GLOBAL_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module provides a lot of global definitions, forward
/// declarations and includes that are used by all modules.  As such it
/// doesn't fall into the user vs. expert classification.

namespace EL
{
  class Algorithm;
  class AnaAlgorithmWrapper;
  class BatchDriver;
  struct BatchJob;
  struct BatchSample;
  struct BatchSegment;
  class BatchWorker;
  class CondorDriver;
  class DirectDriver;
  class DirectWorker;
  class Driver;
  struct EventRange;
  class GEDriver;
  class Job;
  class JobConfig;
  class LLDriver;
  class LSFDriver;
  class LocalDriver;
  class OutputStream;
  class SlurmDriver;
  class StatusCode;
  class TorqueDriver;
  class IWorker;
  class Worker;

  namespace Detail
  {
    struct AlgorithmData;
    class Module;
    struct ModuleData;
    class OutputStreamData;

    class AlgorithmStateModule;
  }
}

#endif
