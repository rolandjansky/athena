/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALCSLOPE_H
#define CSCCALCSLOPE_H
/**CscCalcSlope - algorithm which cycles through all calibration data and 
generates a flat file with the calibration constants. It also generates
a root file where the user can view the histograms used to find the constants,
so that he can determine the validity of the constants
*/
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ToolHandle.h"

//#include "MuonCondData/CscCalibData.h"
#include "DataModel/DataVector.h"
#include "CscCalibData/CscCalibResultCollection.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

//temporary for tests
#include "BipolarFit.h"

#include <vector>
#include <map>
#include <set>

#include "TH1.h"


class TProfile;
class CscICoolStrSvc;
class ICscCalibTool;
class cscIdHelper;
class CscCalibResultCollection;
class TGraphErrors;

namespace MuonCalib{
  /** 
    @class CscCalcSlope

    @brief does calibration of the CSC chambers

    @author lampen@physics.arizona.edu

    @section Description
    CscCalcSlope is an algorithm that cycles through calibration events and generates
    the calibration constants. A root file is also generated where the user can
    view the validity of the constants.
   */

  class CscCalcSlope: public AthAlgorithm
  {
    public:
      CscCalcSlope(const std::string& name, ISvcLocator* pSvcLocator);
      ~CscCalcSlope(void);

      /**basic required functions*/
      StatusCode initialize(void);
      StatusCode execute(void);
      StatusCode finalize(void); 

    private:

      /***********Private member functions*/
      /**event loop functions*/
      StatusCode collectEventInfo();
      /**Finalize functions*/
      StatusCode calculateParameters();
      StatusCode writeCalibrationFile();
      StatusCode storeGateRecord();
      /**Utility functions*/
      StatusCode makeCalibPoints();
      double calShape(double *x, double * par);
      StatusCode calOutput0();
      StatusCode calOutput3();
      void outputParameter3(const CscCalibResultCollection & results, std::ofstream & out);

      /*********Private member variables*/
      /**Services and tools*/
      StoreGateSvc * m_storeGate;
      //ITHistSvc * m_thistSvc;
      ICscCalibTool * m_cscCalibTool;
      ServiceHandle<CscICoolStrSvc> m_cscCoolStrSvc;
      ToolHandle<Muon::ICSC_RDO_Decoder> m_cscRdoDecoderTool;
      const CscIdHelper *m_cscId;
      IChronoStatSvc* m_chronoSvc;

      /**Parameters input through joboptions*/
      std::string m_outputFileName;
      std::string m_calOutputVersion;
      
      //double m_calFuncMin, m_calFuncMax;        
      double m_minDeltaAdc;
      //double m_calFuncEdgeChanMin;
      
      bool m_dumpAllHists;
      bool m_ignoreDatabaseError;
      bool m_doBitHists;

      std::string m_titlePrefix, m_titlePostfix;
      /**Internally global variables*/
      unsigned int m_maxStripHash;
      int m_lastPulserLevel;
      
      //DataVector< std::vector<double> > * m_adcValues;
      //DataVector< std::vector<double> > * m_adcErrors;
      //DataVector< std::vector<int> > * m_pulserLevels;
      DataVector<DataVector<TProfile> >* m_fracProfs;
      DataVector<DataVector<TGraph> >* m_fracGraphs;
      DataVector<TH1I> * m_bitHists;
      std::vector<float> * m_fitReturns;

      TGraph * m_resGraph;        
      DataVector<TGraphErrors> * m_calGraphs;
      //DataVector<TH1F> * m_ampHists;
      TProfile * m_currentAmpProf; 
      std::map<int, TProfile*> * m_ampProfs;
      std::set<int> * m_pulsedChambers;

      /**coherent correction array has the corrections to the coherently pulsed channels to get the basic channels**/
      int m_eventCnt;	

      CscCalibResultCollection *m_slopes, *m_intercepts;


      //Temporary for testing:
      BipolarFit m_bipolarFit;
      bool m_doBipolarFit;
      double * m_crossTalkFix; 
      bool m_doCrossTalkFix;
      //bool m_doPulserFactor;
      std::vector<float>  m_dbLevels;

      float *m_peds, *m_noises;
      //StatusCode onlineToOfflineId(int onlineId, Identifier &chamberId, Identifier &stripId)const;
      StatusCode fillBitHist(TH1I * bitHist, const uint16_t & val);
      bool m_pedFile;
      std::string m_pedFileName;
      //std::string m_pulserFactorFile;

      int m_expectedChamberLayer;
      //bool * m_allowedStrips;

      std::string m_calFitFunc;
      
      //For peaking time
      bool m_findPeakTime;
      TProfile * m_peakTimeProf;
      CscCalibResultCollection * m_peakTimes;

      //allows linear ploting
      bool m_doLinPlot;
      
      //String command to allow interface to patched version
      std::string m_cmd_parameters;

      //Number of bits
      unsigned int m_numBits;
  };//end class CscCalcSlope

  inline StatusCode CscCalcSlope::fillBitHist(TH1I * bitHist, const uint16_t & val)
  {
    if(!bitHist)
      return StatusCode::RECOVERABLE;

    //Num bits should always be m_numBits
    std::bitset<12> bitVal(val);

    for(unsigned int bitIndex = 0; bitIndex < m_numBits; bitIndex++){
      if(bitVal[bitIndex]){
        bitHist->Fill(bitIndex);
      }
    }


    return StatusCode::SUCCESS;
  }

}//end namespace:

#endif
