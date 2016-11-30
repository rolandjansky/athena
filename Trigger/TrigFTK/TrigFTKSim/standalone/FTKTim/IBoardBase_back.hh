/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#ifndef IBOARDBASE_HH
#define IBOARDBASE_HH

#include "TObject.h"

#include "IOTimeBase.hh"

// *** global *** // -----------------------------------
#define NREGION 8
#define NLAYER 11 // pixel 0-2 sct 3-10 

#define NDATAFORMATTER 0
//#define NDATAORGANIZER 6
//#define NDATAORGANIZER 8
#define NDATAORGANIZER 8
#define NASSOCIATIVEMEMORY 4  // new boad one AM have 4 LAM
#define NTRACKFITTER 4 // board*GF
//#define NTRACKFITTER 8 // board*GF
//#define NTRACKFITTER 16 // board*GF
#define NGF 1

#define NSECSTAGEDATAORGANIZER 4
//#define NSECSTAGETRACKFITTER 16 // new boad
#define NSECSTAGETRACKFITTER 4 // new boad
#define FTKTimeSim_Debug 4 // 4 is full debug mode, 3 is summary

// *** DO *** // -------------------------------------------------
#define DO_T_IN 5     // nouse
#define DO_T_OUT 0    // nouse
//#define DO_T_PROC 40   // test
//#define DO_T_PROC 25   // 40 MHz (ns)
#define DO_T_PROC  10   // 100 MHz (ns)
#define DO_T_DELAY 10   // 4 clock by 100MHz

// *** AM *** // -------------------------------------------------
//#define AM_T_IN 10  // 85 MHz * 12 port i.e. 1G Hz
#define AM_T_IN 5  // 85 MHz * 12 port i.e. 1G Hz
#define AM_T_OUT 0
//#define AM_T_PROC 25  // 40 MHz
//#define AM_T_PROC 10  // 100 MHz TSP? 
//#define AM_T_PROC 5  // 200 MHz (=5nsec) TSP?
//#define AM_T_PROC 2.5  // 400 MHz AM? //20 GHz for split?
//#define AM_T_PROC 1.6  // 600 MHz TSP? //20 GHz for split?
#define AM_T_PROC 1.25  // 800 MHz (Design Spec)
#define AM_T_DELAY 250 // 10 clock by 40MHz
//#define AM_T_DELAY 500 // 10 clock by 40MHz

// *** SecDO *** // ---------------------------------------------
#define SecDO_T_IN 1  // I'm using 4 AM's # road, it mean i used 4 DO(8 stream)
//#define SecDO_T_IN 5  // I'm using 4 AM's # road, it mean i used 4 DO(8 stream) Paola
#define SecDO_T_OUT 0
//#define SecDO_T_PROC 25   // 40 MHz
//#define SecDO_T_PROC 10   // 40 MHz
//#define SecDO_T_PROC 0   // 
//#define SecDO_T_PROC 6.7   //  20ns(50MHz) / 3 for one DO
//#define SecDO_T_PROC 2.5   //  20ns(50MHz) / 4 for one DO
#define SecDO_T_PROC 1   //  20ns(50MHz) / 4 for one DO
#define SecDO_T_DELAY 200   // 3-4 clock by 40MHz 3.5->87.5 ns

// *** TF *** // --------------------------------------------------
//#define TF_T_IN 5
#define TF_T_IN 1
#define TF_T_OUT 0
//#define TF_T_PROC 1 // 1GHz
#define TF_T_PROC 0.25 // 1GHz
//#define TF_T_PROC 1.7 // 600 MHz
//#define TF_T_PROC 2 // 500 MHz

//#define TF_T_PROC 4   // 250 MHz
#define TF_T_DELAY 300

// *** SecStageSecDO *** // -------------------------------------
//#define SecStageSecDO_T_IN 0 Origine
//#define SecStageSecDO_T_IN 5   // Hideyoshi
#define SecStageSecDO_T_IN 2.5   // Hideyoshi
#define SecStageSecDO_T_OUT 0
//#define SecStageSecDO_T_PROC 0  //
//#define SecStageSecDO_T_PROC 1.7  // Origine
//#define SecStageSecDO_T_PROC 5  // Hideyoshi
#define SecStageSecDO_T_PROC 2.5  // Hideyoshi

//#define SecStageSecDO_T_DELAY 88   // 3-4 clock by 40MHz 3.5->87.5 ns
#define SecStageSecDO_T_DELAY 40    // 4 clock by 100MHz Origine --temporary--
//#define SecStageSecDO_T_DELAY 40    // 4 clock by 100MHz  

// *** SecStageTF *** // -----------------------------------------
#define SecStageTF_T_IN 1
#define SecStageTF_T_OUT 0
#define SecStageTF_T_PROC 1
#define SecStageTF_T_DELAY 10


//=================================================================
typedef struct InputValues {
  int LVL1Gap;
  int nentry;
  int s_overlap; // 1 : process time consider about event overlap
  int t_kill_TF; 
  int tt; 
  int split;
  int FirstStage; 
  vector<double> TF_ew_out_history;
  vector<double> nhit_nohw_integ_history[NLAYER];
  vector<double> nss_integ_history[NLAYER];
  double nroad_rw, nroad_norw;// ..cofusing norw is after rw..... // no use nroad_rw....
  double nroad_norw_AM[NASSOCIATIVEMEMORY];
  double nroad_rw_AM[NASSOCIATIVEMEMORY];
  double ncomb;
  double ntrack;
  double ntrack_hw;
  double nfit;
  double nhit_raw[NLAYER];
  double nhit_hw[NLAYER], nhit_nohw[NLAYER];  // ..cofusing nohw is after hw.....
  double nhit_raw_tot;  // Creep! =[0]+....[NLAYER]
  double nhit_hw_tot, nhit_nohw_tot; // Creep!
  double nss_raw[NLAYER];
  double nss_hw[NLAYER], nss[NLAYER];  // ..cofusing nohw is after hw.....
  double nss_raw_tot;  // Creep! =[0]+....[NLAYER]
  double nss_hw_tot, nss_tot; // Creep!
  //  double test[NDATAORGANIZER];

  // *** for the buffer study  *** //
  double nroad_rw_integ, nroad_norw_integ;// ..cofusing norw is after rw..... // no use nroad_rw....
  double nroad_norw_AM_integ[NASSOCIATIVEMEMORY];
  double nroad_rw_AM_integ[NASSOCIATIVEMEMORY];
  double ncomb_integ;
  double ntrack_integ;
  double ntrack_hw_integ;
  double nfit_integ;
  double nhit_raw_integ[NLAYER];
  double nhit_hw_integ[NLAYER], nhit_nohw_integ[NLAYER];  // ..cofusing nohw is after hw..... // hw is in TF ... no remove hit.......
  double nhit_raw_tot_integ;  // Creep! =[0]+....[NLAYER]
  double nhit_hw_tot_integ, nhit_nohw_tot_integ; // Creep!
  double nss_raw_integ[NLAYER];
  double nss_hw_integ[NLAYER], nss_integ[NLAYER];  // ..cofusing nohw is after hw..... // hw is in TF ... no remove ss.......
  double nss_raw_tot_integ;  // Creep! =[0]+....[NLAYER]
  double nss_hw_tot_integ, nss_tot_integ; // Creep!

  // *** for 4L Secound Stage *** //
  vector<double> SecStage_TF_ew_out_history;
  double SecStage_nroad_rw, SecStage_nroad_norw;// ..cofusing norw is after rw..... // no use nroad_rw....
  double SecStage_nroad_norw_AM[NASSOCIATIVEMEMORY];
  double SecStage_nroad_rw_AM[NASSOCIATIVEMEMORY];
  double SecStage_ncomb;
  double SecStage_ntrack;
  double SecStage_ntrack_hw;
  double SecStage_nfit;
  double SecStage_nroad_rw_integ, SecStage_nroad_norw_integ;// ..cofusing norw is after rw..... // no use nroad_rw....
  double SecStage_nroad_norw_AM_integ[NASSOCIATIVEMEMORY];
  double SecStage_nroad_rw_AM_integ[NASSOCIATIVEMEMORY];
  double SecStage_ncomb_integ;
  double SecStage_ntrack_integ;
  double SecStage_ntrack_hw_integ;
  double SecStage_nfit_integ;

} InputValues;

class IBoardBase : public TObject {

protected:
  InputValues iv;
  std::vector<IOTimeBase> iotime;
  std::vector<IOTimeBase> pre_iotime;
  std::vector<IOTimeBase> prepre_iotime;
  std::vector<IOTimeBase> sppre_iotime;
  std::vector<IOTimeBase> sp2pre_iotime;

public:

  virtual ~IBoardBase(){}

  void SetInputValue(InputValues iv) {
    this->iv = iv;
  }
  InputValues GetInputValue() {
    return this->iv;
  }
  void Set(std::vector<IOTimeBase> iotime) 
  {
    this->iotime = iotime;
  }
  IOTimeBase Get(int idx) { return this->iotime[idx]; }
  std::vector<IOTimeBase> GetV() { return this->iotime; }

  void SetPre(std::vector<IOTimeBase> pre_iotime) 
  {
    this->pre_iotime = pre_iotime;
  }
  IOTimeBase GetPre(int idx) { return this->pre_iotime[idx]; }
  std::vector<IOTimeBase> GetPreV() { return this->pre_iotime; }

  void SetPrePre(std::vector<IOTimeBase> prepre_iotime) 
  {
    this->prepre_iotime = prepre_iotime;
  }
  IOTimeBase GetPrePre(int idx) { return this->prepre_iotime[idx]; }
  std::vector<IOTimeBase> GetPrePreV() { return this->prepre_iotime; }

  void SetSPPre(std::vector<IOTimeBase> sppre_iotime) 
  {
    this->sppre_iotime = sppre_iotime;
  }
  IOTimeBase GetSPPre(int idx) { return this->sppre_iotime[idx]; }
  std::vector<IOTimeBase> GetSPPreV() { return this->sppre_iotime; }


  void SetSP2Pre(std::vector<IOTimeBase> sp2pre_iotime) 
  {
    this->sp2pre_iotime = sp2pre_iotime;
  }
  IOTimeBase GetSP2Pre(int idx) { return this->sp2pre_iotime[idx]; }
  std::vector<IOTimeBase> GetSP2PreV() { return this->sp2pre_iotime; }

  virtual void Calc()=0; 
};

#endif
