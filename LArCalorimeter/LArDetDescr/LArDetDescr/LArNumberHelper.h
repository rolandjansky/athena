/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArNumberHelper_H
#define LArNumberHelper_H

#include "Identifier/Identifier.h"

class StoreGateSvc;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;

class IRDBAccessSvc;
class IRDBRecordset;
class IRDBRecord;
class LArCellVolumes;
class IGeoModelSvc;

/**

    This class reads all numbers traditionnaly needed and used 
    to build the read-out geometry part of the LArDetDescrManager :

     - grannularity and region size in eta/phi are taken from the
       Identifier dictionnary. These are in "ideal" geometry.

     - the cell depth (in R or Z) comes from the official geometry DB

    The LArMgrInitializer uses these numbers and takes care of the translation 
    between cylindric/cartesian coordinates if needed.

    Now that the position of each region is taken from the GeoModel, the HepTransforms
    are provided by another specific class : LArSubdetPosHelper

    The name Helper was choosen to differentiate such simple classes, which are 
    exclusively used within the LArDetDescr package, from Tools possibly used by
    reconstrcution clients.

*/

class LArNumberHelper
{
public :
  
  LArNumberHelper(const std::string geometry);
  ~LArNumberHelper();
 
  // fill in first, will take care of the user interface after...
  
  // Warning: 
  //          phi_min is the LAr conventional origin and stays at 0
  //          phi_max is the region size in phi and stays equal to 2 pi .
  //          It is for individual channels that the -pi,pi convention
  //          is applied.

  int nb_of_em_regions();
  int nb_of_hec_regions();
  int nb_of_fcal_regions();
  
  void print_em();
  void print_hec();
  void print_fcal();

  // em :
  Identifier region_id_em(int i);
  int     n_eta_em(int i);
  int     n_phi_em(int i);
  double  deta_em(int i);
  double  dphi_em(int i);
  int     sign_em(int i);
  double  eta_min_em(int i);
  double  eta_max_em(int i);
  double  phi_min_em(int i);
  double  phi_max_em(int i);
  int     n_depth_em(int i);
  double  depth_em_in(int i, int j);
  double  depth_em_out(int i, int j);

  // hec :
  Identifier region_id_hec(int i);
  int       n_eta_hec(int i);
  int       n_phi_hec(int i);
  double    deta_hec(int i);
  double    dphi_hec(int i);
  int       sign_hec(int i);
  double    eta_min_hec(int i);
  double    eta_max_hec(int i);
  double    phi_min_hec(int i);
  double    phi_max_hec(int i);
  int       n_depth_hec(int i);
  double    depth_hec_in(int i, int j);
  double    depth_hec_out(int i, int j);

  // fcal : geometry in in x/y, but FCAL_ID works in eta/phi !
  Identifier  region_id_fcal(int i);
  int      n_eta_fcal(int i);
  int      n_phi_fcal(int i);
  double   deta_fcal(int i);
  double   dphi_fcal(int i);
  int      sign_fcal(int i);

  double   x_min_fcal(int i);
  double   x_max_fcal(int i);
  double   dx_fcal(int i);
  double   y_min_fcal(int i);
  double   y_max_fcal(int i);
  double   dy_fcal(int i);
  double   z_min_fcal(int i);
  double   z_max_fcal(int i);

  double    eta_min_fcal(int i);
  double    eta_max_fcal(int i);
  double    phi_min_fcal(int i);
  double    phi_max_fcal(int i);

  int     n_depth_fcal(int i);
  double  depth_fcal_in(int i, int j);
  double  depth_fcal_out(int i, int j);

  LArCellVolumes* VolumeHelper();

  // Parameters needed for sagging
  void sagging_param( std::vector<double>& Rhocen, std::vector<double>& Sag );
  
 private:

  const LArEM_ID* m_em_id;
  const LArHEC_ID* m_hec_id;
  const LArFCAL_ID* m_fcal_id;
  std::string m_geometry;

  StoreGateSvc* m_pDetStore;   

  IRDBAccessSvc* m_iAccessSvc;
  const IRDBRecordset* m_lar;
  const IRDBRecord* m_rec;
  const IGeoModelSvc* m_geoModelSvc;

  std::string m_tag;
  std::string m_node;

  LArCellVolumes* m_cellVolumes;

  void prepare_arrays();

  void hard_em();
  void db_nb_em();

  void hard_hec();
  void db_nb_hec();

  void hard_fcal();
  void db_nb_fcal();

  void cart_to_cyl (  double x,    double y,    double z,
		      double& eta, double& phi, double& r );

  double eta ( double r, double z );

  // std::vector <double> z_array;  // max dim will be 44

  // em : info coming from Identifiers
  std::vector <Identifier>    m_region_id_em;
  std::vector <int>           m_n_eta_em;
  std::vector <int>           m_n_phi_em;
  std::vector <double>        m_deta_em;
  std::vector <double>        m_dphi_em;
  std::vector <int>           m_sign_em;

  // em : info coming from the geometry DB
  std::vector <double>        m_eta_min_em;
  std::vector <double>        m_eta_max_em;
  std::vector <double>        m_phi_min_em;
  std::vector <double>        m_phi_max_em;
  std::vector <int>           m_n_depth_em;
  std::vector < std::vector <double> > m_depth_em_in;
  std::vector < std::vector <double> > m_depth_em_out;

  // hec : info coming from Identifiers
  std::vector <Identifier>    m_region_id_hec;
  std::vector <int>           m_n_eta_hec;
  std::vector <int>           m_n_phi_hec;
  std::vector <double>        m_deta_hec;
  std::vector <double>        m_dphi_hec;
  std::vector <int>           m_sign_hec;

  // hec : info coming from the geometry DB
  std::vector <double>        m_eta_min_hec;
  std::vector <double>        m_eta_max_hec;
  std::vector <double>        m_phi_min_hec;
  std::vector <double>        m_phi_max_hec;
  std::vector <int>           m_n_depth_hec;
  std::vector < std::vector <double> > m_depth_hec_in;
  std::vector < std::vector <double> > m_depth_hec_out;
 
  // fcal : info coming from Identifiers
  std::vector <Identifier>    m_region_id_fcal;
  std::vector <int>           m_n_eta_fcal;
  std::vector <int>           m_n_phi_fcal;
  std::vector <double>        m_deta_fcal;
  std::vector <double>        m_dphi_fcal;
  std::vector <int>           m_sign_fcal;

  // fcal : info coming from FCAL ChannelMap
  std::vector <double>        m_x_min_fcal;
  std::vector <double>        m_x_max_fcal;
  std::vector <double>        m_dx_fcal;
  std::vector <double>        m_y_min_fcal;
  std::vector <double>        m_y_max_fcal;
  std::vector <double>        m_dy_fcal;
  std::vector <double>        m_z_min_fcal;
  std::vector <double>        m_z_max_fcal;
  std::vector <double>        m_eta_min_fcal;
  std::vector <double>        m_eta_max_fcal;
  std::vector <double>        m_phi_min_fcal;
  std::vector <double>        m_phi_max_fcal;
  std::vector <int>           m_n_depth_fcal;
  std::vector < std::vector <double> > m_depth_fcal_in;
  std::vector < std::vector <double> > m_depth_fcal_out;

  // these arrays are read in the geometry DB and used internally

  // EMB
  double m_emb_psin;
  double m_accg_rin_ac;
  double m_accg_rout_ac;
  std::vector <double> m_acco_rmx12;
  std::vector <double> m_acco_ee12;
  std::vector <double> m_acco_rmx23;

  // EMEC
  double m_endg_zorig;
  std::vector <double> m_esep_iw23;
  double m_emb_iwout;
  double m_emec_psin;
  double m_esep_zsep12;
  std::vector <double> m_esep_zsep23;
  double m_emec_out;

  // HEC
  double m_hec_in0;
  double m_hec_in1;
  double m_hec_in2;
  double m_hec_in3;
  double m_hec_out;
  double m_hec_gap;

  // FCAL: none, its all in the FCALChannelMaps

  // Avoid coverity warnings.
  LArNumberHelper (const LArNumberHelper&);
  LArNumberHelper& operator= (const LArNumberHelper&);

};

#endif
