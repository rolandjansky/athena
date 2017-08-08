/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKBankGenAlgo_h
#define FTKBankGenAlgo_h

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTK_SGHitInput.h"
#include "TrigFTKSim/FTKDataInput.h"
#include "TrigFTKSim/TrackFitter.h"

#include "TrigFTKBankGen/FTKConstGenAlgo.h"

#include "TTree.h"

#include <string>
#include <vector>
#include <ostream>
#include <cassert>
#include <fstream>
#include <bitset>

/////////////////////////////////////////////////////////////////////////////
class ITHistSvc;
class TH1F;
class TH2F;

//TODO prefer FTKHit
//originally in FTKBankGenAlgo.h
class filterSecHit{
 public:
  int count;
  int sector;
  int sector_HW;
  int plane;
  int hittype;
  float dimx;
  float dimy;
  int ndim;
  int CSide;
  int ASide;
  FTKHit originalhit;
};

class matrix{
 public:
  double C;
  double D;
  double Phi;
  double Coto;
  double Z;
  float  nhit;
};

class FTKBankGenAlgo: public AthAlgorithm {
public:
  FTKBankGenAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKBankGenAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  class StoreGateSvc;
  
private:
  
  ITHistSvc *m_thistSvc;
  StoreGateSvc*  m_storeGate;
  ToolHandle<FTK_SGHitInputI> m_hitInputTool; // input handler
  
  TFile *file;

  //for ConstGenTest  
  int count_match;
  int count_pass_filter;
  double coverage;

  //for event filter
  int nmuon;
  double pt,eta,truth_phi;

  int m_nevent;
  // Number of regions/banks
  int m_nbanks;
  int m_nsubregions;
  int m_nregions;
  
  int m_verbosity;
  bool m_BarrelOnly;
  int m_rawMode;
  int m_IBLMode; // flag to change the IBLMode
  bool m_fixEndcapL0;
  bool m_ITkMode; // flag to use ITk geometry

  // Plane map pointer, to be set during the initialization
  FTKPlaneMap *m_pmap;
  FTKPlaneMap *m_pmap_8L;
  
  // region map used in this session
  FTKRegionMap *m_rmap;
  
  int whichRegionAlt(int *sector, FTKRegionMap *rmap);

  std::string m_pmap_path;
  std::string m_pmap_8L_path;

  std::string m_rmap_path;

  /* the following flags were copied from TrigFTKSim/FTKDataInput.h on 11/02/14 check they are still up to date. Thanks, Guido*/
  bool m_Clustering; // flag to enable the clustering
  bool m_SaveClusterContent; // flag to allow the debug of the cluster content
  bool m_DiagClustering; // true if diagonal clustering is used
  bool m_SctClustering; // true if SCT clustering is performed
  unsigned int m_PixelClusteringMode; // 1 means ToT information is used
                                      //   && 400/600um pixels are accounted for
                                      // 0 is default used up to 2013
  bool m_Ibl3DRealistic;
  bool m_DuplicateGanged;
  bool m_GangedPatternRecognition;


  bool m_UseIdentifierHash; // if true define a sector as a sequence of identifier hashes
  
  std::string m_sector_dir_path;//for const test
  std::string m_gcon_dir_path;//for const test
  char c_sector_dir_path[200];
  char c_gcon_dir_path[200];

  int m_nplanes;
  int  TotalDim;
  int  TotalDim2;
  int *npatterns;
  int *ntracks;
  TTree *m_tree[100]; // expanded from 64-->100 to allow for more ITk regions

  TTree *m_slicetree;
  TTree *m_montree;
  ULong64_t m_monval[100];

  std::vector <short> int_c,int_phi,int_d0,int_z0,int_eta; 
  std::vector<short> *m_intc;
  std::vector<short> *m_intphi;
  std::vector<short> *m_intd0;
  std::vector<short> *m_intz0;
  std::vector<short> *m_inteta;

  int m_nc;
  int m_nphi;
  int m_nd0;
  int m_nz0;
  int m_neta;
      
  double m_par_c_max;
  double m_par_c_min;
  int m_par_c_slices;
  
  double m_par_phi_max;
  double m_par_phi_min;
  int m_par_phi_slices;

  double m_par_d0_max;
  double m_par_d0_min;
  int m_par_d0_slices;
  
  double m_par_z0_max;
  double m_par_z0_min;
  int m_par_z0_slices;

  double m_par_eta_max;
  double m_par_eta_min;
  int m_par_eta_slices; 

  double m_c_max;
  double m_c_min;
  int m_c_slices;

  double m_phi_max;
  double m_phi_min;
  int m_phi_slices;

  double m_d0_max;
  double m_d0_min;
  int m_d0_slices;
  
  double m_z0_max;
  double m_z0_min;
  int m_z0_slices;

  double m_eta_max;
  double m_eta_min;
  int m_eta_slices; 

  
  // hit information
  unsigned int nhits;
  int nsinglemu;
  std::vector<FTKRawHit> orig_hits;
  std::vector<FTKTruthTrack> truth_track;
  std::vector<FTKTruthTrack> m_trainingtracks; // list of good training tracks
  std::vector<FTKHit> hitslist;//use hit infomation
  std::map<int, std::vector<FTKHit> > m_maphits;// map of the hits according the barcode
  
  // definitions of truth track 
  float m_PT_THRESHOLD;
  int m_TRAIN_PDG;
  
  int *p_ss;
  int *p_hashss;
  double *tmphitc;
  
  double *tmpxC;
  double *tmpxD;
  double *tmpxPhi;
  double *tmpxCoto;
  double *tmpxZ;
  double *tmpcovx;
     
  int addPattReturnCode;

  //track parameter
  double x0,y0;
  double c,d,phi,z0,coto;
   
  int m_nsector;
  float m_nhit;
  // tmp sector and matrix elements
  
  //TODO vector<int> m_sectorID;
  int *m_sectorID;
  int *m_hashID;
  double*  m_tmpVec;
  double m_tmpC;
  double m_tmpD;
  double m_tmpPhi;
  double m_tmpCoto;
  double m_tmpZ;

  double *m_tmpxC;
  double *m_tmpxD;
  double *m_tmpxPhi;
  double *m_tmpxCoto;
  double *m_tmpxZ;
  double *m_tmpcovx;
  
  matrix Mtmp;

  //event definitions  
  bool GoodTrack;
  bool doPattgen;
    
  int tmp_sectorID;
  int the_sectorID;
  int *tmp_ssID; //TODO std::vector<int> tmp_SSID;
  int *tmp_hashID; //TODO std::vector<int> tmp_SSID;

  //for culcurate resolution
  double dC;
  double dD;
  double dPhi;
  double dZ0;
  double dCoto;

  ofstream file_resolution;
  ofstream file_truthpar;
  ofstream file_recpar;
  ofstream file_coverage;

  char str_gcon_file_name[200];
  char str_sector_file_name[200];
  string gcon_path;

   FTKTrack base_trk;
   FTKConstantBank **m_constant; // constant banks
   bool m_const_test_mode;

  double header[2];
  double dummy;

  std::bitset<128> m_compsecbitmask;
};

#endif // FTKBankGenAlgo_h
