/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:      CscCalibMonTool.h
// PACKAGE:   MuonCalibMonTool.h
//
// AUTHORS:   Caleb Parnell-Lampen <lampen@physics.arizona.edu>
//
// ********************************************************************

#ifndef MUONCALIBMONITORING_CSCCALIBMONTOOLPED_H
#define MUONCALIBMONITORING_CSCCALIBMONTOOLPED_H

#include "CscCalibMonToolBase.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "TH1.h"
#include "TProfile.h"

/////////////////////////////////////////////////////////////////////////////
/**
  @class CscCalibMonToolPed
  @brief Class for monitoring pedestal calibration results

  This class monitors pedestals and noises created from CscCalcPed.cxx in
  the CscCalibAlgs package. It produces numerous histograms to illustrate
  the results. It runs during the finalize stage of the job. The info it 
  looks at includes:
  -The pedestals and noises (sigma).
  -Pedestals and noises stored in COOL
  -Difference between these values
  -Chi^2 of pedestal fit

  CscCalibMonToolBase provides the basis for much of the operation of this class.
  Generally, the program flow osciallates between base class functions and
  CscCalibMonToolPed functions.
*/
class CscCalibMonToolPed: public CscCalibMonToolBase 
{

    public:
        /**Constructor*/
        CscCalibMonToolPed (const std::string & type, const std::string & name, 
                const IInterface* parent);

        /**Destructor*/
        ~CscCalibMonToolPed();
       
        /**initialize function. Calls base class's initialize function.*/
        StatusCode initialize(); 
        
        /**finalize function. Calls base class's finalize function.*/
        StatusCode finalize();
        
        /**Books all histograms not retrieved driectly from CscCalcPed*/
        StatusCode bookHistograms();

    private:
        /**handleParameter sets things up for the base class procParameter. It makes
          decisions based on the parameter being passed in parVals on how to handle the data
          It then copies the arrays generated from procParameter to all the appropriate histograms
          @param parVals Values for a particular parameter*/
        virtual StatusCode handleParameter(const CscCalibResultCollection* parVals);
        
        /**postProc does some post processing work after the main parameter histograms are filled*/
        virtual StatusCode postProc();
        
        void genThreshold(HistCollection * pedColl, HistCollection * noiseColl, HistCollection * tholdColl, float multiplier) ;


        void makeErrorReport();

        // CSC identifier helper
        //const CscIdHelper * m_cscIdHelper;


    private:
        /* Muon Detector Descriptor*/
        const MuonGM::MuonDetectorManager * m_muon_mgr;

        /**Histograms that simply histogram all entries for a value*/
        TH1F *m_h_pedMissingChannels;

        /**Allowed deviations from COOL data*/
        float m_pedMaxDiff,m_noiseMaxDiff, m_rmsMaxDiff, m_f001MaxDiff, m_onlTHoldBreachMax;

        /** Allowed maximum values*/
        float m_chi2Max;

        /** Allowed number of entries*/
        int m_minAmpHistEntries;

        /** name of file indicating all is well*/
        std::string m_allGoodFileName;
        /**Status report name*/
        std::string m_statusReportName;
        /**Status report prefix*/
        std::string m_statusReportPrefix;
        /**DO bit correlation plots?"*/
        bool m_doBitCorrelations;

        /**Bin in m_h_numBen corresponding to each type of data*/
        int m_pedBadBin, m_noiseBadBin, m_rmsBadBin, m_f001BadBin, m_nEntriesBadBin, m_chi2BadBin, m_missingBadBin, m_onlTHoldBreachBadBin;

        //Error report
        bool m_alwaysPrintErrorReport;

        /**Samples*/
        //int m_doPedSamples;

        /**Storegate key for pedestal amplitude histograms*/
        std::string m_histKey;

        /**Stores number of channels that are bad in several categories*/
        TH1I *m_h_numBad;

        /**HistCollections. See CscCalibMonToolBase for definition*/ 
        HistCollection *m_pedNewColl, *m_pedOldColl, *m_pedDiffColl;
        HistCollection *m_noiseNewColl, *m_noiseOldColl, *m_noiseDiffColl;
        HistCollection *m_chi2Coll;        
        HistCollection *m_rmsNewColl, *m_rmsOldColl, *m_rmsDiffColl;
        HistCollection *m_f001NewColl, *m_f001OldColl, *m_f001DiffColl;
        HistCollection *m_onlTHoldBreachColl;


        HistCollection *m_nEntriesColl;

        HistCollection * m_tholdDiffColl;
        
        HistCollection * m_maxBitCorrColl;



        TH2I * m_h2_rmsVnoiseEta, * m_h2_rmsVnoisePhi;

        bool m_doRmsVNoise;
};


#endif
