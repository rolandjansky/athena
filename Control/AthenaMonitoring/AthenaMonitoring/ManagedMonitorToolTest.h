/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ManagedMonitorToolTest_H
#define ManagedMonitorToolTest_H

#include <vector>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TRandom3.h"
#include "CxxUtils/checker_macros.h"

class TH1;
class TGraph;
class TTree;
class TH1D;
class TH1D_LW;
class TH1F_LW;
class TH2F;
class TH2F_LW;

class ATLAS_NOT_THREAD_SAFE ManagedMonitorToolTest : public ManagedMonitorToolBase
{
   public:

      ManagedMonitorToolTest( const std::string & type, const std::string & name,
            const IInterface* parent ); 

      virtual ~ManagedMonitorToolTest();

      virtual StatusCode bookHistogramsRecurrent();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();

   protected:

      static const long   s_nEnsembles;
      static const double s_ensembleGenMean;
      static const double s_ensembleGenSigma;

      bool               m_generatorIsInit;
      unsigned long      m_generatorSeedProp;

      std::vector<TH1*>  m_ensembles;
      TH1*               m_ensembleMean;
      TH1*               m_ensembleRMS;
      TH1*               m_ensembleMeanPull;
      TH1*               m_ensembleMeanPullDist;

      TH1*               m_managedHist0_lowStat;
      TH1*               m_managedHist1_lowStat;
      TH1*               m_managedHist2_lowStat;
      TH1*               m_managedHist_lumiBlock;
      TH1*               m_managedHist_eventsBlock;
      TH1*               m_managedHist_run;

      TH1*               m_Lumi;

      TGraph*            m_graph;
      TGraph*            m_managedGraph;
      TTree*             m_ntuple;
      TTree*             m_managedNtuple;
      std::vector<int>   m_spacepoint;
      std::vector<int>   m_event;
      int                m_counter;
      int                m_variable1, m_variable2;

      TH1D_LW            *m_hlw_1D;
      TH1F_LW            *m_hlw_1F;
      TH2F_LW            *m_hlw_2F;

      int                m_offset;
      TRandom3           m_gen;

      TEfficiency*       m_efficiency;
      TEfficiency*       m_managedEfficiency;
      TEfficiency*       m_managedEfficiencyXLB;

      TGraph*            m_managedGraphXLB;
};

#endif
