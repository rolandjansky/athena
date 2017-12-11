/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__TRTCALIBRATOR_H
#define TRT_CALIBTOOLS__TRTCALIBRATOR_H

/********************************************************************

   NAME: TRTCalibrator.h    
PACKAGE: TRTCalibTools 

AUTHORS: Johan Lundquist  
CREATED: 27-03-2009 

PURPOSE: Tool for calibrating the TRT
          
********************************************************************/

#include <string>
#include "TRT_CalibTools/ITRTCalibrator.h"
//#include "TRT_ConditionsTools/ITRTStrawNeighbourTool.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
//#include "StoreGate/StoreGateSvc.h"
#include <TNtuple.h>
#include <TFile.h>
#include <TVectorD.h>

class ITRT_StrawSummarySvc;
class AtlasDetectorID;
class TRT_ID;
class ITRT_CalDbSvc ;

/**
The TRTCalibrator is called from TRTCalibrationMgr.
*/
class TRTCalibrator :  virtual public ITRTCalibrator, public AthAlgTool {
public:

  /**
     Constructor
  */
  TRTCalibrator(const std::string& type, const std::string& name, const IInterface* parent);

  /**
     Destructor
  */
  virtual ~TRTCalibrator() {}

  /**
      Pre-define standard calibration configuration

      The string-vectors "CalibrateRt" ("CalibrateT0") contains the names of the levels
      for which R-t (T0) calibration should be performed

      The string-vectors "PrintT0Out" and "PrintRtOut" contains the names of the levels for 
      text files containing the calibration constants should be generated

      The string-vector "PrintTxtOutput" contains the names of the levels for which a
      text file containing the calibration constants should be generated

      This standard configuration divides the Barrel in 9 parts:

      "all": a single calibration for all hits in the whole %TRT
      "-1" & "1": calibration of the barrel C- & A-side
      "X_Y": calibration of layer Y in barrel X-side
  */
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual bool fill(const Trk::Track* aTrack, TRT::TrackInfo* output);

  bool IncludedLevels(std::string, int*);

  /**
     return sub-level i from a selection string with the form _X_X_X_X_X_X_X
  */
  std::string SubLev(std::string, int);  

  /**
     returns true if the ...
  */
  bool IsSubLev(std::string, int, std::string);  

  /**
     returns true if the ...
  */
  void DumpStrawData(int);  

  /**
     fills a set of integers with the indexes af the sub-modules that are conatined in a selection 
  */
  int GetSubLevels(std::string, int, std::set<int>*);  

  /**
     makes the key strings for a sub-module
  */
  void MakeBDKeys(int, int, int, int, int, int);

  /**
     Method for doing the actual calibration
  */
  virtual bool calibrate();

private:
  const AtlasDetectorID* m_DetID;
  const TRT_ID* m_TRTID; 
  const InDetDD::TRT_DetectorManager* m_trtmanager;
  //ToolHandle<ITRT_CalDbSvc> m_trtcaldbtool ;
  //ToolHandle<ITRTStrawNeighbourTool> m_neighbourtool;  
  ServiceHandle<ITRT_CalDbSvc> m_trtcaldbSvc ;
  ServiceHandle<ITRT_StrawNeighbourSvc> m_neighbourSvc ;
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawSummarySvc; //!< The ConditionsSummaryTool
  float m_maxDistance ;
  float m_maxTimeResidual ;
  int m_minTimebinsOverThreshold ;
  float m_maxTrackChisquarePerDof ;
  unsigned int m_numOfHitsTotal ;
  unsigned int m_numOfHitsAccepted ;
  unsigned int m_numOfProcessedTracks ;

  //StoreGateSvc     * m_store_gate;
  
  std::string m_comTimeName;

  /**minimum number of hits required to do R-t calibration*/ int m_minrt;
  /**minimum number of hits required to do T0 calibration*/ int m_mint0;
  /**The number of hits or histograms read from the input file*/ int m_nevents;
  /**The t0 offset*/ float m_t0offset;

  std::string m_calsub;
  std::string m_rtbinning;
  std::string m_selstring;
  std::string m_options;
  std::string m_hittuple;
  /**the rt relation to use*/ std::string m_rtrel;
  std::string m_ntrtmanager;
  std::vector<std::string> m_doRt;
  std::vector<std::string> m_doT0;
  std::vector<std::string> m_doRes;
  std::vector<std::string> m_beQuiet;
  std::vector<std::string> m_useBoardRef;
  bool m_SplitBarrel;
  bool m_useP0;
  bool m_floatP3;
  bool m_DoShortStrawCorrection;
  bool m_DoArXenonSep;
  std::vector<std::string> m_doLogPrint;
  std::vector<std::string> m_doOutPrint;
  std::vector<std::string> m_doRtPrint;

  std::string m_Tkey,m_Dkey,m_Lkey,m_Mkey,m_Bkey,m_Ckey,m_Skey,m_Lkey_acc,m_Dkey_acc;
  
  /** dummy dictionary class*/ class BDzero{public: int z;};
  /** dictionary class for a straw*/ class BDstraw{public: std::map<std::string,BDzero> s;};
  /** dictionary class for a chip*/ class BDchip{public: std::map<std::string,BDstraw> c;};
  /** dictionary class for a board*/ class BDboard{public: std::map<std::string,BDchip> b;};
  /** dictionary class for a module*/ class BDmodule{public: std::map<std::string,BDboard> m;};
  /** dictionary class for a layer*/ class BDlayer{public: std::map<std::string,BDmodule> l;};
  /** dictionary class for a detector*/ class BDdetector{public: std::map<std::string,BDlayer> d;};
  /** dictionary class for the full %TRT*/ class BDTRT{public: std::map<std::string,BDdetector> t;};
  /** instance if the dictionary class for the full %TRT*/ BDTRT m_trt;

  /** instance if the dictionary class for the full %TRT*/ BDTRT m_trt_acc;

  
  class epdata{
  public:
    float run,event,track,nhits,ephase1,ephase2,t0,t,ttrack;
  };

  struct conf {
    std::string Selstring, Options;
    std::vector<std::string> CalibrateRt, CalibrateT0, FitResidual, NoHistograms, PrintLog, PrintT0Out, PrintRtOut;
    std::map< std::string,std::set<int> > selections;//TRTsel, detsel, laysel, modsel, brdsel, chpsel, stwsel;
  };
  std::map<std::string,conf> m_config;

  enum {CALIB_ALL=-3, CALIB_NONE=-4};

  TFile* m_histfile;
  
};

#endif //  TRT_CALIBTOOLS__TRTCALIBRATOR_H
