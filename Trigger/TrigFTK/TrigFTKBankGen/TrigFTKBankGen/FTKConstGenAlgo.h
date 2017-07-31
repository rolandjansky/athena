/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKConstGenAlgo_h
#define FTKConstGenAlgo_h

#include "TrigFTKSim/FTKPMap.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TTree.h"

#include <string>
#include <vector>
#include <ostream>
#include <cassert>


class ITHistSvc;
class TH1F;
class TH2F;
#define NPARS 5

/** data structure that contain definition of geometrical
    constants for linear fits */
class geo_constants
{
 public:

  double *Vd; // impact paramers coefs
  double *Vc; // curvature coefs
  double *Vf; // phi coefs
  double *Vz0; // z0 coefs
  double *Vo; // cot(theta) coefs
  double **kernel; // kernel
  double *kaverages; // averages
  double Cd; // constant terms for: impact parameter
  double Cc; // curvature
  double Cf; // phi
  double Cz0; // z0
  double Co; // cot(theta)
  int real; // this values is greater than 0 if these constants are correctly evaulated
}; 


class FTKConstGenAlgo: public AthAlgorithm {
public:
  FTKConstGenAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTKConstGenAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
   
  class StoreGateSvc;
  
 private:

  ITHistSvc *m_thistSvc;
  StoreGateSvc*  m_storeGate;
  bool m_merging;
  bool m_genconst;
  bool m_subregion;
  bool m_extract_1stStage;
  std::string m_cfpath; 
  std::vector<std::string> m_fpath;  
  std::vector<std::string> m_gfpath;
  std::vector<std::string> m_sfpath;
  string m_extractpath;  
  
  std::string m_outfilename; // the algorithm mostly produce a single file, if set this is the name of the file, a default option is otherwise used
  TFile *file;
  TFile *good_file;
  TFile *cfile;
  TTree *m_ctree;

  bool m_ITkMode; // toggle ITk geometry
  bool m_allregion;
  int m_region;
  int m_nsubregion;
  int m_nbank;
  int m_nfile;
  int addPattReturnCode;
  float nsector;
  bool match;
 
  double pr;

  int nevent;

  int nplane;
  int nplane8;

  // variables for merge
  std::vector<short> *m_mergeintc;
  std::vector<short> *m_mergeintphi;
  std::vector<short> *m_mergeintd0;
  std::vector<short> *m_mergeintz0;
  std::vector<short> *m_mergeinteta;
  
  // variables for extract 1st stage
  int plane_index_1st_stage;
  int plane_index_2nd_stage;
  int coord_index_1st_stage;
  int coord_index_2nd_stage;
  int index_missing_plane;
  int index_missing_coord;

  std::vector<int> vec_plane_index_1st_stage;
  std::vector<int> vec_plane_index_2nd_stage;
  std::vector<int> vec_coord_index_1st_stage;
  std::vector<int> vec_coord_index_2nd_stage;
  std::vector<int> vec_doInvert;
  std::vector<int> vec_ndimension;

  // variables for 12L->8L
  bool doRelation; // if true, this layer/coords has/have missing layer/coords
  bool doInvert;// endcap inversion for some SCT layers
  int inversion;
  int inversion2;

  std::vector<short> *m_extract_intc;
  std::vector<short> *m_extract_intphi;
  std::vector<short> *m_extract_intd0;
  std::vector<short> *m_extract_intz0;
  std::vector<short> *m_extract_inteta;
  
  // Plane map pointer, to be set during the initialization
  FTKPlaneMap *m_pmap;
  FTKPlaneMap *m_pmap_8L;

  std::string m_pmap_path;
  std::string m_pmap_8L_path;

  bool *endcap_inversion; // true if endcap invert stereo and axial layer IDs

  // functions
  void merge();
  void constantgen();
  void extract_1stStage();
  void make_subregion();
  int mtx_eigen(int dim, double *mtx_v, double *eigval_v, double *eigvecs_v);
  int mtx_invert(int dim, double *mtx_v);
  void nullifyConstants(int sector,int ndim);
  void sigmad(double *geoconstvect/*, double **marray*/, double **marrayinv,double *v,int dim);
  float proj(double *vec1, double *vec2, int size);
  int failedConstants(int sector,int ndim);

};

#endif // FTKConstGenAlgo_h

