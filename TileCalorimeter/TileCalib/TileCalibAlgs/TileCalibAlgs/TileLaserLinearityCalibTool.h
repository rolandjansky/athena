/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_TILELASERLINEARITYCALIBTOOL_H
#define TILECALIBALG_TILELASERLINEARITYCALIBTOOL_H

#include "GaudiKernel/AlgTool.h"

#include "TileCalibAlgs/ITileCalibTool.h"
#include <string> 
#include <map>

#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileCalibAlgs/TileLaserDefaultCalibTool.h"

class StoreGateSvc;
class TileRawChannelContainer;
class TileBeamInfoProvider;
class TileLaserObject;
class TileHWID;
class TFile;

class TileLaserLinearityCalibTool : public AlgTool, virtual public ITileCalibTool
{

 public:
  TileLaserLinearityCalibTool(const std::string& type, const std::string& name,const IInterface* pParent);
  virtual ~TileLaserLinearityCalibTool();
 
  virtual StatusCode initialize();
  virtual StatusCode initNtuple(int runNumber, int runType, TFile * rootfile);
  virtual StatusCode execute();
  virtual StatusCode finalizeCalculations();
  virtual StatusCode writeNtuple(int runNumber, int runType, TFile * rootfile);
  virtual StatusCode finalize();

 private:

  // Name of ntuple of the Tool
  std::string m_toolNtuple;

  std::string m_rawChannelContainerName;
  std::string m_laserContainerName;

  // Tools / storegate info
  StoreGateSvc* m_detStore;
  StoreGateSvc* m_evtStore;

  const TileHWID* m_tileHWID;    
  const TileCablingService* m_cabling;
  ToolHandle<TileCondToolEmscale>  m_tileToolEmscale;
  TileBeamInfoProvider *m_beamInfo;

  // Parameter which will end up in the ROOTuple
  //
  // Details concerning this parameters could be found on the LASER webpage :
  // 
  // http://atlas-tile-laser.web.cern.ch/
  //
  // Here we are working on linearity runs, it explains the [8] in the arrays
  // we store one value per filter position

  int m_toolRunNo;                   // The run number
  int m_ADC_problem;                 // Expert tag for LastROD problem noticfication
  int m_las_filter;                  // Filter wheel position
  double m_las_requ_amp;             // Requested amplitude
  double m_hrate;                    // Humidity rate (in %)
  double m_flow;                     // Gas flow in diodes box (in L/h)
  double m_head_temp;                // Temperature of the LASER head
  double m_las_time;                 // Event time
  double m_PMT[8][2];                // Mean value for box PMTs
  double m_PMT_S[8][2];              // Corresponding RMS
  double m_diode[8][4];              // Mean value for box Photodiodes
  double m_diode_S[8][4];            // Corresponding RMS
  double m_mean[8][4][64][48][2];       // Mean signal computed for this run
  double m_mean_S[8][4][64][48][2];     // Corresponding RMS
  double m_ratio[8][4][64][48][2];      // Calib coefficients computed for this run (for all diodes)
  double m_ratio_S[8][4][64][48][2];    // Corresponding RMS
  double m_ratio2[8][4][64][48][2];      // Calib coefficients computed for this run (for all diodes)
  double m_ratio2_S[8][4][64][48][2];    // Corresponding RMS
  int    m_entries[8][4][64][48][2];    // Number of LASER events collected for one channel (and a particular gain)

 
  // Local results - not sent to ROOTuple

  int m_PMT1_ADC_prev;
  int m_PMT2_ADC_prev;

  int first_filter;      // Value of the first filter position

  int last_evt_filter;   // Filter position of the previous event
  int n_same_filt_evts;  // How many events in a row with the same filter setting

  bool m_complete_turn;

  RunningStat* diode_signal[8][4];
  RunningStat* PMT_signal[8][2];
  RunningStat* signal[8][4][64][48][2];
  RunningStat* ratio[8][4][64][48][2];
  RunningStat* ratio2[8][4][64][48][2];

  // Functions
  inline int chanIsConnected(int ros, int chan) {
    if(m_cabling->channel2hole(ros,chan)<0) return 0; //negative means not connected
    return 1;
  }
};



#endif // #ifndef TILECALIBALG_TILELASERLINEARITYCALIBTOOL_H
