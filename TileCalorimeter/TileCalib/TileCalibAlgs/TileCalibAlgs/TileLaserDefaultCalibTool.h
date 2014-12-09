/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBALG_TILELASERDEFAULTCALIBTOOL_H
#define TILECALIBALG_TILELASERDEFAULTCALIBTOOL_H

#include "GaudiKernel/AlgTool.h"

#include "TileCalibAlgs/ITileCalibTool.h"
#include <string> 
#include <map>
#include <cmath>

#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/ITileBadChanTool.h"

#define NDIODES 14
#define NCHANNELS 32
#define NPMTS

class StoreGateSvc;
class TileRawChannelContainer;
class TileBeamInfoProvider;
class TileLaserObject;
class TileHWID;
class TFile;
class RunningStat;
class TileDCSSvc;

class TileLaserDefaultCalibTool : public AlgTool, virtual public ITileCalibTool
{

 public:
  TileLaserDefaultCalibTool(const std::string& type, const std::string& name,const IInterface* pParent);
  virtual ~TileLaserDefaultCalibTool();
 
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
	
  bool m_pisaMethod2;
  
  // Tools / storegate info
  StoreGateSvc* m_detStore;
  StoreGateSvc* m_evtStore;

  const TileHWID* m_tileHWID;    
  const TileCablingService* m_cabling;
  ToolHandle<TileCondToolEmscale>  m_tileToolEmscale;
  ToolHandle<ITileBadChanTool>     m_tileBadChanTool;
  TileBeamInfoProvider *m_beamInfo;

  ServiceHandle<TileDCSSvc>   m_tileDCSSvc; //!< Pointer to TileDCSSvc


  // Parameter which will end up in the ROOTuple
  //
  // Details concerning this parameters could be found on the LASER webpage :
  // 
  // http://atlas-tile-laser.web.cern.ch/
  //

  int m_toolRunNo;                   // The run number
  int m_ADC_problem;                 // Expert tag for LastROD problem noticfication
  int m_las_filter;                  // Filter wheel position
  float m_las_requ_amp;             // Requested amplitude
  float m_hrate;                    // Humidity rate (in %)
  float m_flow;                     // Gas flow in diodes box (in L/h)
  float m_head_temp;                // Temperature of the LASER head
  float m_las_time;                 // Event time

  // LASERII
  float m_ratio_LASERII[32][4][64][48][2];       // Calib coefficients computed for this run (for all diodes, PMTs, phocal, CIS)
  float m_ratio_S_LASERII[32][4][64][48][2];     // Corresponding RMS

  RunningStat* rs_chan_Laser[32];
  RunningStat* rs_chan_Pedestal[32];
  RunningStat* rs_chan_LED[32];
  RunningStat* rs_chan_Alpha[32];
  RunningStat* rs_chan_Linearity[32];

  RunningStat* rs_ratio_LASERII[32][4][64][48][2];

  // FIRST PART OF DATA FRAGMENT
  float m_chan_LAS[32];                 // Mean value for monitoring diodes, PMTs, phocal, CIS in Laser runs
  float m_chan_PED[32];                 // Mean value for monitoring diodes, PMTs, phocal, CIS in Pedestal runs
  float m_chan_LED[32];                 // Mean value for monitoring diodes, PMTs, phocal, CIS in LED runs
  float m_chan_APH[32];                 // Mean value for monitoring diodes, PMTs, phocal, CIS in Alpha runs
  float m_chan_LIN[32];                 // Mean value for monitoring diodes, PMTs, phocal, CIS un Linearity runs
  float m_chan_S_LAS[32];               // Corresponding RMS in Laser runs
  float m_chan_S_PED[32];               // Corresponding RMS in Pedestal runs
  float m_chan_S_LED[32];               // Corresponding RMS in LED runs
  float m_chan_S_APH[32];               // Corresponding RMS in Alpha runs
  float m_chan_S_LIN[32];               // Corresponding RMS in Linearity runs

  // SECOND PART OF DATA FRAGMENT
  float m_chan_Ped[32];             // Corresponding pedestal values
  float m_chan_Led[32];             // Corresponding LED values
  float m_chan_Lin[32];             // Corresponding linearity values
  float m_chan_Alpha[32];           // Corresponding alpha peaks
  float m_chan_SPed[32];            // Sigma of pedestal values
  float m_chan_SLed[32];            // Sigma of LED values
  float m_chan_SLin[32];            // Sigma of linearity values
  float m_chan_SAlpha[32];          // Sigma of alpha peaks

  // LASERI
  float m_PMT[2];                       // Mean value for box PMTs
  float m_PMT_S[2];                     // Corresponding RMS
  float m_diode[4];                     // Mean value for box Photodiodes
  float m_diode_S[4];                   // Corresponding RMS
  float m_diode_Ped[4];                 // Corresponding pedestal values
  float m_diode_Alpha[4];               // Corresponding alpha peaks
  float m_diode_SPed[4];                // Sigma of pedestals
  float m_diode_SAlpha[4];              // RMS of alpha spectra
  float m_ratio[4][4][64][48][2];       // Calib coefficients computed for this run (for all diodes)
  float m_ratio_S[4][4][64][48][2];     // Corresponding RMS
  RunningStat* rs_diode_signal[4];
  RunningStat* rs_PMT_signal[2];
  RunningStat* rs_ratio[4][4][64][48][2];

  float m_meantime[4];                  // Mean time computed for this run per partition (to remove 25ns jitter) 
  float m_time[4][64][48][2];           // Mean time computed for this run
  float m_time_S[4][64][48][2];         // Corresponding RMS
  float m_mean[4][64][48][2];           // Mean signal computed for this run
  float m_mean_S[4][64][48][2];         // Corresponding RMS
  float m_raw_mean[4][64][48][2];       // Mean signal computed for this run
  float m_raw_mean_S[4][64][48][2];     // Corresponding RMS
  int    m_entries[4][64][48][2];       // Number of LASER events collected for one channel (and a particular gain)
  float m_kappa[4][64][48][2];          // Kappa correction term
  float m_mean_slice[4][64][48][100][2];
  float m_variance_slice[4][64][48][100][2];
  short  m_status[4][64][48][2];        // Status of the channel in the DB
  float  m_HV[4][64][48]; 
  float  m_HVSet[4][64][48]; 

  // Local results - not sent to ROOTuple
  int m_PMT1_ADC_prev[2];
  int m_PMT2_ADC_prev[2];

  bool m_LASERII;
  long long m_evtNr;

  RunningStat* rs_meantime[4];
  RunningStat* rs_time[4][64][48][2];
  RunningStat* rs_signal[4][64][48][2];
  RunningStat* rs_raw_signal[4][64][48][2];
  RunningStat* rs_reducedKappa[4][64][22][2];

  // Functions
  std::pair<unsigned int, unsigned int> getCoupleOfChan(int ros, int couple);
  short isCellBad(int ros, int drawer, int channel, int gain);
  
  inline int chanIsConnected(int ros, int chan) {
    if(m_cabling->channel2hole(ros,chan)<0) return 0; //negative means not connected
    return 1;
  }
};


// Small class to compute mean & variance in a efficient way
class RunningStat {
	
public:
	// Default constructor
	RunningStat() : m_n(0), m_oldM(0), m_newM(0), m_oldS(0), m_newS(0), m_nTotEntries(0),
	                m_n_slice(0), m_oldM_slice(0), m_newM_slice(0), m_oldS_slice(0), m_newS_slice(0), m_eventsPerSlice(1000) {}

	void Clear() {
		m_n = 0;
	}
	
	// See Knuth TAOCP vol 2, 3rd edition, page 232
	void Push(double x) {
		m_n++;
		m_n_slice++;
		m_nTotEntries++;
		
		if (m_n == 1) {
			m_oldM = m_newM = x;
			m_oldS = 0.0;
		} else {
			m_newM = m_oldM + (x - m_oldM)/m_n;
			m_newS = m_oldS + (x - m_oldM)*(x - m_newM);
   
			// set up for next iteration
			m_oldM = m_newM; 
			m_oldS = m_newS;
		}

		//--- for slicing
		if (m_n_slice == 1) {
			m_oldM_slice = m_newM_slice = x;
			m_oldS_slice = 0.0;
		} else {
			m_newM_slice = m_oldM_slice + (x - m_oldM_slice)/m_n_slice;
			m_newS_slice = m_oldS_slice + (x - m_oldM_slice)*(x - m_newM_slice);
   		m_oldM_slice = m_newM_slice; 
			m_oldS_slice = m_newS_slice;
		}
		
		if( m_n_slice==m_eventsPerSlice ) {
			m_vectMean.push_back( (m_n_slice > 0) ? m_newM_slice : 0.0 );
			m_vectVar.push_back( (m_n_slice > 1) ? m_newS_slice/(m_n_slice - 1) : 0.0 );
			m_n_slice=0;
		}
	}

	inline int NumDataValues() const { return m_nTotEntries;}
	inline double Mean() const { return (m_n > 0) ? m_newM : 0.0; }
	inline double Variance() const { return ( (m_n > 1) ? m_newS/(m_n - 1) : 0.0 ); }
	inline double StandardDeviation() const { return sqrt( Variance() ); }
	inline int EventsPerSlice() { return m_eventsPerSlice;}
	
	inline double Mean(unsigned int iSlice){ 
		if ( iSlice<m_vectMean.size() ) return m_vectMean[iSlice];
		return 0;
	} 

	inline double Variance(unsigned int iSlice){ 
		if (iSlice<m_vectVar.size()) return m_vectVar[iSlice];
		return 0;
	}
	
	inline int GetNSlices(){
		return m_vectVar.size();
	}


private:
	int m_n;
	double m_oldM, m_newM, m_oldS, m_newS;
	int m_nTotEntries;
	int m_n_slice;
	double m_oldM_slice, m_newM_slice, m_oldS_slice, m_newS_slice;
	int m_eventsPerSlice;
	std::vector<double> m_vectMean, m_vectVar;
	
};



#endif // #ifndef TILECALIBALG_TILELASERDEFAULTCALIBTOOL_H
