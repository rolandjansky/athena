/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 *
 * constants for the ALFA analysis programs
 *
 */

#include <cmath>

constexpr unsigned int NUMPMTMD = 20;  //     Main Detector number of PMFs == PMTs
constexpr unsigned int NUMCHAMD = 64;  //     Main Detector number of channels
constexpr unsigned int NUMLAYMD = 20;  //     Main Detector number of layers
constexpr unsigned int NUMFIBMD = 64;  //     Main Detector number of fibers

constexpr unsigned int NUMPMTOD =  3;  //  Overlap Detector number of PMFs == PMTs
constexpr unsigned int NUMCHAOD = 64;  //  Overlap Detector number of channels
constexpr unsigned int NUMLAYOD =  6;  //  Overlap Detector number of layers
constexpr unsigned int NUMFIBOD = 30;  //  Overlap Detector number of fibers

// constants for LAL test-board

constexpr unsigned int NUMKAPLAL = 2;  // Two kaptons read by two test-board
constexpr unsigned int NUMPMTLAL = 5;  // 5 PMT per kapton

constexpr unsigned int NEVTFILE =  2;  //  Max nr. events in file (2 => 2000 events)
constexpr unsigned int HEADSIZE = 20;  //  4*4+1*4 Bytes
constexpr unsigned int PMFSIZE  =  8;  //  2*4 Bytes

constexpr unsigned int NUMPMTALL = NUMPMTMD + NUMPMTOD + 1;
constexpr unsigned int NUMPMFALL = 25;
constexpr unsigned int NUMRUN = 46;

constexpr bool simData = false;
constexpr bool mapReadData = false;

constexpr char* ROOTFILENAME = "./output.root";
constexpr char* TREENAME = "AlfaTree";

// User control, prints, etc
// -------------------------
constexpr unsigned int NEVTMAX   = 999999999; // <--- maximum possible (not change)
constexpr unsigned int KEVTMAX   = 10000;     // <---- up to user maximum (user)
constexpr unsigned int NCOSTEP   = 1000;
constexpr unsigned int nPrintEvt = 1000;

constexpr unsigned int MULCHA_Max = 64;
constexpr unsigned int MULFIB_Max = 64;
constexpr unsigned int MAX_SUMACT_CH = 100000;


// DAQ numbering
// -------------
constexpr unsigned int nDaqOD1 = 4;
constexpr unsigned int nDaqOD2 = 3;
constexpr unsigned int nDaqOD3 = 2;


// List noisy or dead fibers
// -------------------------
constexpr unsigned int jBadLayFib[10][10] = { {10} , {8} };

// constants for Connectivity.C
// ----------------------------
// Mapping from MAROC bits to MAPMT channels

constexpr char* MAROC2PMTFILE = "./mapping/MAROC_to_MAPMT.dat";

// CON & GEO Data files
// --------------------
constexpr char* CONFILE_MD = "./mapping/conn_md.dat";
constexpr char* GEOFILE_MD = "./mapping/geom_md.dat";

constexpr char* CONFILE_OD = "./mapping/OD_mapping.dat";
constexpr char* GEOFILE_OD = "./mapping/geom_od_last_bis.dat";
//constexpr char* GEOFILE_OD = "./mapping.d/geo_ultim_OD.dat";
//constexpr char* GEOFILE_OD = "./mapping.d/geo_last_OD.dat";
//constexpr char* GEOFILE_OD = "./mapping.d/geom_od_56.dat";

constexpr char* CONFILE_LAL = "./mapping/conn_lal.dat";

//Variable for the reconstruction program
//---------------------------------------

constexpr double t_low    = M_PI/4;
constexpr double t_high   =  3*M_PI/4;
constexpr double r_low    = 0.0;
constexpr double r_high   = 64.0;
constexpr int t_bins      = 100;              // # Theta Bins
constexpr int r_bins      = 64;              // # R Bins

constexpr double r_low_SI    = 0.5;
constexpr double r_high_SI   = 640.5;
constexpr int t_bins_SI      = 100;              // # Theta Bins
constexpr int r_bins_SI      = 105;            // # R Bins

// Time variables
// --------------

//time_t      rawtime;
//struct tm * timeinfo;
//char        timebuffer [80];
//string      str_of_time;

