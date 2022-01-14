/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEPULSESHAPES_H
#define TILECONDITIONS_TILEPULSESHAPES_H

#include <vector>
#include "GaudiKernel/MsgStream.h"

/** 48 pmts (channels) in one drawer */
#define MAX_PMT 48

#define MAX_SAMPLES 16

#define MAX_LO_PULSE_CIS 100
#define MAX_LO_PULSE_CIS_SMALL 100
#define MAX_HI_PULSE_CIS 100
#define MAX_HI_PULSE_CIS_SMALL 100
#define MAX_LO_PULSE_PHYS 401
#define MAX_HI_PULSE_PHYS 401 
#define MAX_LO_PULSE_LAS 100
#define MAX_HI_PULSE_LAS 100 
 
// constants for digi data pulse fits 
#define EPS_DG 1.0E-8           
#define SATURATED_ADC_VALUE 1023.0
#define DELTA_CHI2 0.01
#define MAX_CHI2 1.0E+5

typedef struct {

  /** Low Gain Pulse CIS */
  std::vector<double> m_tlcis;           // CIS pulse shape, big cap 100 pF
  std::vector<double> m_ylcis; 
  std::vector<double> m_tslcis;          // CIS pulse shape, small cap 5.2 pF
  std::vector<double> m_yslcis;
  std::vector<double> m_tleaklo;
  std::vector<double> m_leaklo;
  std::vector<double> m_tsleaklo;
  std::vector<double> m_sleaklo;
  
  /** Hi Gain Pulse CIS */
  std::vector<double> m_thcis;           // CIS pulse shape, big cap 100 pF
  std::vector<double> m_yhcis;
  std::vector<double> m_tshcis;          // CIS pulse shape, small cap 5.2 pF
  std::vector<double> m_yshcis;
  std::vector<double> m_tleakhi;
  std::vector<double> m_leakhi;
  std::vector<double> m_tsleakhi;
  std::vector<double> m_sleakhi;
  
  /** Low Gain Pulse Physics */
  std::vector<double> m_tlphys;
  std::vector<double> m_ylphys;
  
  /** Hi Gain Pulse Physics */
  std::vector<double> m_thphys;
  std::vector<double> m_yhphys;
  
  /** Low Gain Pulse Laser */
  std::vector<double> m_tllas;
  std::vector<double> m_yllas;
  
  /** Hi Gain Pulse Laser */
  std::vector<double> m_thlas;
  std::vector<double> m_yhlas;
  
  // Derivatives of the pulse shapes:
  
  /** Low Gain Pulse CIS */
  std::vector<double> m_tdlcis;
  std::vector<double> m_ydlcis;
  std::vector<double> m_tdslcis;
  std::vector<double> m_ydslcis;
  std::vector<double> m_tdleaklo;
  std::vector<double> m_dleaklo;
  std::vector<double> m_tdsleaklo;
  std::vector<double> m_dsleaklo;
  
  /** Hi Gain Pulse CIS */
  std::vector<double> m_tdhcis;
  std::vector<double> m_ydhcis;
  std::vector<double> m_tdshcis;
  std::vector<double> m_ydshcis;
  std::vector<double> m_tdleakhi;
  std::vector<double> m_dleakhi;
  std::vector<double> m_tdsleakhi;
  std::vector<double> m_dsleakhi;
  
  /** Low Gain Pulse Physics */
  std::vector<double> m_tdlphys;
  std::vector<double> m_ydlphys;
  
  /** Hi Gain Pulse Physics */
  std::vector<double> m_tdhphys;
  std::vector<double> m_ydhphys;
  
  /** Low Gain Pulse Laser */
  std::vector<double> m_tdllas;
  std::vector<double> m_ydllas;
  
  /** Hi Gain Pulse Laser */
  std::vector<double> m_tdhlas;
  std::vector<double> m_ydhlas;
  
  // Digitizer noise RMS as a function of channel number
  
  /** (1) Original noise */
  std::vector<double> m_noiseOrigLo;
  std::vector<double> m_noiseOrigHi;
  
  /** (2) Noise with resistors added to PMT channels (so-called noise-killers)*/
  std::vector<double> m_noiseNkLo;
  std::vector<double> m_noiseNkHi;
  
} TilePulseShapesStruct;

class  TilePulseShapes {

friend class TileInfoLoader;

 public:

  TilePulseShapes();
  ~TilePulseShapes();

  void load(MsgStream &log);

  const TilePulseShapesStruct * TilePSstruct () const { return m_shapes; }

  bool loaded() { return m_loaded; }

 private:
  
  // CIS Pulse shapes and derivatives
  std::string m_filenameLoCIS;
  std::string m_filenameLoCISDer;
  std::string m_filenameSLoCIS;
  std::string m_filenameSLoCISDer;
  std::string m_filenameHiCIS;
  std::string m_filenameHiCISDer;
  std::string m_filenameSHiCIS;
  std::string m_filenameSHiCISDer;

  // CIS Leakage Pulse shapes and derivatives
  std::string m_filenameSLeakLo;
  std::string m_filenameLeakLo;
  std::string m_filenameDSLeakLo;
  std::string m_filenameDLeakLo;
  std::string m_filenameSLeakHi;
  std::string m_filenameLeakHi;
  std::string m_filenameDSLeakHi;
  std::string m_filenameDLeakHi;
  
  // Physics Pulse shapes and derivatives
  std::string m_filenameLoPhys;
  std::string m_filenameHiPhys;
  std::string m_filenameLoPhysDer;
  std::string m_filenameHiPhysDer;
  
  // Laser Pulse shapes and derivatives
  std::string m_filenameLoLas;
  std::string m_filenameHiLas;
  std::string m_filenameLoLasDer;
  std::string m_filenameHiLasDer;
  
  // Digitizer noise RMS as a function of channel number 
  std::string m_filenameOrigNoise;
  std::string m_filenameNkNoise;

  bool m_loaded;
  TilePulseShapesStruct * m_shapes;

  bool ReadFile(MsgStream &log, std::string fname, const char *xname, const char * yname,
                std::vector<double> & x, std::vector<double> & y, int nskip=0);
};

#endif // TILECONDITIONS_TILEPULSESHAPES_H
