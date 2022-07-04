/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__ALGORITHM_WORKER_DATA_H
#define ANA_ALGORITHM__ALGORITHM_WORKER_DATA_H

#ifndef ROOTCORE
#ifndef __CPPCHECK__
#error only include this header in AnalysisBase
#endif
#endif

namespace asg
{
  class SgTEvent;
}

namespace EL
{
  class IHistogramWorker;
  class ITreeWorker;
  class IFilterWorker;
  class IWorker;

  /// \brief all the external components an algorithm needs before
  /// initialization (in EventLoop)

  struct AlgorithmWorkerData final
  {
    asg::SgTEvent *m_evtStore = nullptr;
    IHistogramWorker *m_histogramWorker = nullptr;
    ITreeWorker *m_treeWorker = nullptr;
    IFilterWorker *m_filterWorker = nullptr;
    IWorker *m_wk = nullptr;
  };
}

#endif
