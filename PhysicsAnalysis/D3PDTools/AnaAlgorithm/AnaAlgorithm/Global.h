/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__GLOBAL_H
#define ANA_ALGORITHM__GLOBAL_H

namespace EL
{
  class AnaAlgorithm;

#ifdef ROOTCORE
  struct AlgorithmWorkerData;
  class AnaAlgorithmConfig;
  class AnaAlgorithmWrapper;
  class IAlgorithmWrapper;
  class IFilterWorker;
  class IHistogramWorker;
  class ITreeWorker;
#endif
}

#endif
