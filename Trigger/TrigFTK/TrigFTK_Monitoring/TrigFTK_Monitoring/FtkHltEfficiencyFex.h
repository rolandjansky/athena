/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
// filename: MonitoredTrackParticle.h
//
// author: Arthur.Bolz@cern.ch, 2016
//
// Description: HLT FEX algorithm to retrieve HLT and FTK tracks and compare them using the 
//              HltTrackComparison tool.
//
//***************************************************************************

#ifndef __TRIGFTK_TRIGFTK_MONITORING_FTKHLTEFFICIENCYFEX_H__
#define __TRIGFTK_TRIGFTK_MONITORING_FTKHLTEFFICIENCYFEX_H__

// trigger EDM
#include "TrigInterfaces/FexAlgo.h"
// FTK includes
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"


namespace TrigFTK { 

  class FtkHltEfficiencyFex : public HLT::FexAlgo
  {
    public:
      /**
       * @brief constructor
       */
      FtkHltEfficiencyFex( const std::string& name, ISvcLocator* pSvcLocator );
      /**
       * @brief constructor
       */
      virtual ~FtkHltEfficiencyFex();

      /**
       * @brief implementation of the abstract hltInitialize method in HLT::FexAlgo.
       */
      virtual HLT::ErrorCode hltInitialize();
      /**
       * @brief implementation of the abstract hltFinalize method in HLT::FexAlgo.
       */
      virtual HLT::ErrorCode hltFinalize();
      /**
       * @brief implementation of the abstract hltExecute method in HLT::FexAlgo.
       */
      virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement*,   HLT::TriggerElement*);

    private:
      /*
       * @brief bring tracks into format std::vector<const xAOD::TrackParticle*> 
       *        that can be fed to HltTrackComparison
       * @param input_tpcPtr input track collection
       * @param output_trkPtr_vec output track collection
       */
      void SelectTracks(
          const xAOD::TrackParticleContainer* input_tpcPtr, 
          std::vector<const xAOD::TrackParticle*>& output_trkPtr_vec);

      ServiceHandle<IFTK_DataProviderSvc> m_FTKDataProviderSvc;

      double m_deltaR_max;
      class HltTrackComparison* m_efficiencyAnalysis;
      class HltTrackComparison* m_purityAnalysis;

      std::vector<const xAOD::TrackParticle*> m_HLTTrkPtr_vec;
      std::vector<const xAOD::TrackParticle*> m_FTKTrkPtr_vec;

      // Track selection criteria
      double        m_minPt;
      double        m_maxEta;
      double        m_maxZ0;
      double        m_maxD0;

      // for HLT tracks in FTK efficiency measurement
      double        m_maxHLTZ0err;
      double        m_maxHLTD0err;
      int           m_minHLTSiHits;
      int           m_minHLTPixHits;
      int           m_minHLTSctHits;
      int           m_maxHLTPixHoles;
      int           m_maxHLTSctHoles;
      double        m_maxHLTReducedChi2;

      // for FTK tracks in FTK fake rate measurement
      double        m_maxFTKZ0err;
      double        m_maxFTKD0err;
      int           m_minFTKSiHits;
      int           m_minFTKPixHits;
      int           m_minFTKSctHits;
      int           m_maxFTKPixHoles;
      int           m_maxFTKSctHoles;
      double        m_maxFTKReducedChi2;


      int           m_numb2DEtaBins;
      double        m_min2DEta;
      double        m_max2DEta;
      double        m_min2DZ0;
      double        m_max2DZ0;

      std::vector<TrigTimer*> m_timer;

      enum FtkHltEfficiencyTimers
      {
        AlgoTime            =  0,
        GetFtkTracksTime    =  1,
        GetHltTracksTime    =  2,
        EfficiencyToolTime  =  3,
        PurityToolTime      =  4,
        //
        nTimers /// Leave this entry at the end
      };
      int m_eventCounter;
      
      unsigned int m_nFTKRawTracks;

      unsigned int m_nFTKTracks;
      unsigned int m_nFTKTrackParticles;
      unsigned int m_nTrackErrors;
      unsigned int m_nTrackParticleErrors;

      double m_fMissPix0;
      double m_fMissPix1;
      double m_fMissPix2;
      double m_fMissPix3;

      int m_nFailPix;

      double m_fMissSCT0;
      double m_fMissSCT1;
      double m_fMissSCT2;
      double m_fMissSCT3;
      double m_fMissSCT4;
      double m_fMissSCT5;
      double m_fMissSCT6;
      double m_fMissSCT7;


      int m_nFailSCT;

};

} // end namespace
#endif
