/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *
 * Constants for the ALFA analysis programs
 *
 */

const unsigned int NUMPMTMD = 20;  //     Main Detector number of PMFs == PMTs
const unsigned int NUMCHAMD = 64;  //     Main Detector number of channels
const unsigned int NUMLAYMD = 20;  //     Main Detector number of layers
const unsigned int NUMFIBMD = 64;  //     Main Detector number of fibers

const unsigned int NUMPMTOD =  3;  //  Overlap Detector number of PMFs == PMTs
const unsigned int NUMCHAOD = 64;  //  Overlap Detector number of channels
const unsigned int NUMLAYOD =  6;  //  Overlap Detector number of layers
const unsigned int NUMFIBOD = 30;  //  Overlap Detector number of fibers

// Constants for LAL test-board

const unsigned int NUMKAPLAL = 2;  // Two kaptons read by two test-board
const unsigned int NUMPMTLAL = 5;  // 5 PMT per kapton

const unsigned int NEVTFILE =  2;  //  Max nr. events in file (2 => 2000 events)
const unsigned int HEADSIZE = 20;  //  4*4+1*4 Bytes
const unsigned int PMFSIZE  =  8;  //  2*4 Bytes

const unsigned int NUMPMTALL = NUMPMTMD + NUMPMTOD + 1;
const unsigned int NUMPMFALL = 25;
const unsigned int NUMRUN = 46;

const bool simData = false;
const bool mapReadData = false;

const char* ROOTFILENAME = "./output.root";
const char* TREENAME = "AlfaTree";

// User control, prints, etc
// -------------------------
const unsigned int NEVTMAX   = 999999999; // <--- maximum possible (not change)
const unsigned int KEVTMAX   = 10000;     // <---- up to user maximum (user)
const unsigned int NCOSTEP   = 1000;
const unsigned int nPrintEvt = 1000;

const unsigned int MULCHA_Max = 64;
const unsigned int MULFIB_Max = 64;
const unsigned int MAX_SUMACT_CH = 100000;


// DAQ numbering
// -------------
const unsigned int nDaqOD1 = 4;
const unsigned int nDaqOD2 = 3;
const unsigned int nDaqOD3 = 2;


// List noisy or dead fibers
// -------------------------
const unsigned int jBadLayFib[10][10] = { {10} , {8} };

// constants for Connectivity.C
// ----------------------------
// Mapping from MAROC bits to MAPMT channels

const char* MAROC2PMTFILE = "./mapping/MAROC_to_MAPMT.dat";

// CON & GEO Data files
// --------------------
const char* CONFILE_MD = "./mapping/conn_md.dat";
const char* GEOFILE_MD = "./mapping/geom_md.dat";

const char* CONFILE_OD = "./mapping/OD_mapping.dat";
const char* GEOFILE_OD = "./mapping/geom_od_last_bis.dat";
//const char* GEOFILE_OD = "./mapping.d/geo_ultim_OD.dat";
//const char* GEOFILE_OD = "./mapping.d/geo_last_OD.dat";
//const char* GEOFILE_OD = "./mapping.d/geom_od_56.dat";

const char* CONFILE_LAL = "./mapping/conn_lal.dat";

//Variable for the reconstruction program
//---------------------------------------

const double PrivPI = 3.14159265359;

const double t_low    = PrivPI/4;
const double t_high   =  3*PrivPI/4;
const double r_low    = 0.0;
const double r_high   = 64.0;
const int t_bins      = 100;              // # Theta Bins
const int r_bins      = 64;              // # R Bins

const double r_low_SI    = 0.5;
const double r_high_SI   = 640.5;
const int t_bins_SI      = 100;              // # Theta Bins
const int r_bins_SI      = 105;            // # R Bins

// Time variables
// --------------

//time_t      rawtime;
//struct tm * timeinfo;
//char        timebuffer [80];
//string      str_of_time;

