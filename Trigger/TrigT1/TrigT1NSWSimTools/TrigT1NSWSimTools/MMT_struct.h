/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_STRUCT_H
#define MM_STRUCT_H

#include <fstream>

#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonDigitContainer/MmDigit.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"

#include "AthenaKernel/MsgStreamMember.h"

#include "TLorentzVector.h"
#include "TMath.h"

//flags
const double crep_pt=200.;
/*
  Potential indexing problems that might slip through the cracks:
  --athena_event/event (hitData_entry)
  --VMM_chip (hitData_entry)
  --road (Finder in MMT_Finder)
*/

//constants and conversions
const int yzdex=2,bkdex=13,zbardex=2;

template<unsigned char T> class float32fixed
{

private:
  float m_fixp_content;

public:
  float32fixed(){ m_fixp_content=0; }
  float32fixed(int value){  m_fixp_content=value; }
  float32fixed(float value){  m_fixp_content=value; }
  float32fixed(double value){  m_fixp_content=value; }
  ~float32fixed(){}




  float getFixed(int S=T) const{

    int nBits = S;
    nBits -= 1;
    //=== get scale
    int   scale  = 0;
    float absVal = std::fabs(m_fixp_content);
    if(absVal != 0.){
      scale = static_cast<int>( ::truncf(std::log((std::pow(2., static_cast<int>(nBits)) - 1.)/absVal)*(1./std::log(2.))) );
    }
    //=== return input value with fixed point precision
    return ::roundf(m_fixp_content * std::pow(2., scale)) / std::pow(2., scale);

  }



  // assignment
  float32fixed<T> operator=(float other){ this->m_fixp_content = other; return *this;  }
  template<unsigned char S>
  float32fixed<T> operator=(float32fixed<S> other){ this->m_fixp_content = other.getFixed(); return *this;  }

  // Float operators
  float32fixed<T> operator+(float other) const {return float32fixed<T>( this->m_fixp_content + float32fixed<T>(other).getFixed() );  }
  float32fixed<T> operator-(float other) const {return float32fixed<T>( this->m_fixp_content - float32fixed<T>(other).getFixed() );  }
  float32fixed<T> operator*(float other) const {return float32fixed<T>( this->m_fixp_content * float32fixed<T>(other).getFixed() );  }
  float32fixed<T> operator/(float other) const {return float32fixed<T>( this->m_fixp_content / float32fixed<T>(other).getFixed() );  }
  float32fixed<T> operator+=(float other) { *this = float32fixed<T>( (*this).m_fixp_content +  float32fixed<T>(other).getFixed() );   return *this;  }
  float32fixed<T> operator-=(float other) { *this = float32fixed<T>( (*this).m_fixp_content -  float32fixed<T>(other).getFixed() );   return *this;  }
  float32fixed<T> operator*=(float other) { *this = float32fixed<T>( (*this).m_fixp_content *  float32fixed<T>(other).getFixed() );   return *this;  }
  float32fixed<T> operator/=(float other) { *this = float32fixed<T>( (*this).m_fixp_content /  float32fixed<T>(other).getFixed() );   return *this;  }
  bool operator<(double other) const {return this->m_fixp_content<   float32fixed<T>(other).getFixed();}
  bool operator>(double other) const {return this->m_fixp_content>   float32fixed<T>(other).getFixed();}
  bool operator<=(double other) const {return this->m_fixp_content<= float32fixed<T>(other).getFixed();}
  bool operator>=(double other) const {return this->m_fixp_content>= float32fixed<T>(other).getFixed();}
  bool operator==(double other) const {return this->m_fixp_content== float32fixed<T>(other).getFixed();}


  // float32fixed operators
  float32fixed<T> operator+ (float32fixed<T> other) const {return float32fixed<T>( this->m_fixp_content + other.getFixed()  );  }
  float32fixed<T> operator- (float32fixed<T> other) const {return float32fixed<T>( this->m_fixp_content - other.getFixed()  );  }
  float32fixed<T> operator* (float32fixed<T> other) const {return float32fixed<T>( this->m_fixp_content * other.getFixed()  );  }
  float32fixed<T> operator/ (float32fixed<T> other) const {return float32fixed<T>( this->m_fixp_content / other.getFixed()  );  }
  float32fixed<T> operator+=(float32fixed<T> other) { *this = float32fixed<T>( (*this).m_fixp_content + other.getFixed()  );   return *this;  }
  float32fixed<T> operator-=(float32fixed<T> other) { *this = float32fixed<T>( (*this).m_fixp_content - other.getFixed()  );   return *this;  }
  float32fixed<T> operator*=(float32fixed<T> other) { *this = float32fixed<T>( (*this).m_fixp_content * other.getFixed()  );   return *this;  }
  float32fixed<T> operator/=(float32fixed<T> other) { *this = float32fixed<T>( (*this).m_fixp_content / other.getFixed()  );   return *this;  }
  bool operator< (float32fixed<T> other) const {return this->m_fixp_content<other.getFixed() ;}
  bool operator> (float32fixed<T> other) const {return this->m_fixp_content>other.getFixed() ;}
  bool operator<=(float32fixed<T> other) const {return this->m_fixp_content<=other.getFixed() ;}
  bool operator>=(float32fixed<T> other) const {return this->m_fixp_content>=other.getFixed() ;}
  bool operator==(float32fixed<T> other) const {return this->m_fixp_content==other.getFixed() ;}

  float32fixed<T> fabs() const{ return ((*this)>0.) ? *this : (*this)*-1.0 ; }

  explicit operator float () {return (double) this->m_fixp_content; }


};

struct std_align{
  //match alignments in the standard documentation;
  //note that we're working in a wedge with \phi_0 in the misalignment document of pi/2; hence,
  //the par_cor_mis (X,Y,Z)-->(T,S,Z)--(end cap side A, (Z>0))-->(s,z,t) rotations (gamma,beta,alpha)
  //so let's do show the correspondence between par_cor_mis and this new std_align, struct (we only do the back quadruplet)
  //(--,-dy,dz;dt,--,dp)=(s,z,t;gamma,beta,alpha)
  //our internal stuff will still match the (x,y,z) coordinate scheme used in the algorithm; we will just translate results into the standard coordinates for plots, etc.
  std_align(int qcm=0,const TVector3& trans=TVector3(),const TVector3& ang=TVector3());
  std::string par_title(int par_num,bool small_unit=false) const;
  std::string par_name(int par_num) const;
  std::string par_title_val(int par_num)const;
  std::string par_name_val(int par_num)const;
  std::string print() const;
  double par_val(int par_num) const;
  void set_val(int par_num,double par_val);
  int parmax() const { return 6; }
  std::string detail() const;
  bool is_nominal()const;

  //members
  int type;//corresponds to qcm; 0 (nominal case: no misal/correction), 1 (misalignment), 2 (correction), 3 (sim_correction; do corrections based on simulation results)
  TVector3 translate,rotate;

};

struct gcm_key{
  gcm_key(int the_pt=0,int the_ct=0,int the_mis=0,int the_corr=0,int the_eta=-1,int the_qt=0,int the_bg=0);
  bool operator==(const gcm_key& merp) const;
  bool operator!=(const gcm_key& merp) const;
  bool operator<(const gcm_key& merp) const;
  bool operator<=(const gcm_key& merp) const;
  bool operator>(const gcm_key& merp) const;
  bool operator>=(const gcm_key& merp) const;

  gcm_key ak()const {return gcm_key(pt,ct,mis,correct,-1,qt,bgcode);}//access key (ak for short; I'm a lazy typist); eta doesn't matter for access so set it to default so we access the map ok
  int varmax()const{return 7;}
  std::string print()const;
  void set_var(int var,int val);
  int get_var(int var)const;

  //these are the original variables....
  int pt,ct,mis;
  int correct;
  int eta;
  //these are the new ones--we will stack
  //qt is charge threshold 0, 1 (default), or 2
  //bgcode 0--3: std (y/n), wide (y/n)
  int qt,bgcode;
};

struct par_par{
  par_par(double the_h,
          int xct=0,
          int uvct=0,
          double uver=0,
          const std::string& setup="",
          bool ql=true,
          bool q_dlm=false,
          bool qbg=0,
          double the_qt=1.,
          std_align mis=std_align(0),
          std_align cor=std_align(0),
          bool fill_tab=true,
          int cs=0,
          const std::string&pd="",
          const std::string&tg="");

  std::string print_pars(const std::vector<int>&hide=std::vector<int>()) const;
  std::string detail()const;

  //members
  double h;
  int ctx,ctuv;
  double uverr;
  std::string setup;
  bool islarge,q_dlm,genbg;
  double qt;

  std_align misal,corr;
  std::vector<TString> misal_str;
  bool fill_val;
  int colskip;
  std::string pcrep_dir,tag;



};

class MMT_Parameters{
 public:
  MMT_Parameters(par_par inputParams,char wedgeSize, const MuonGM::MuonDetectorManager* detManager);

  std::vector<Amg::Vector3D> MM_firststrip_positions(const MuonGM::MuonDetectorManager* detManager, const std::string& wedge, int eta);
  int is_x(int plane);
  int is_u(int plane);
  int is_v(int plane);

  unsigned int n_multiplets = 2;
  unsigned int n_layers     = 4;

  // start counting at 1 to be consistent with mmIdHelper.
  std::vector<unsigned int> planes_x = {1, 2, 7, 8};
  std::vector<unsigned int> planes_u = {3, 5};
  std::vector<unsigned int> planes_v = {4, 6};

  std::vector<int> q_planes(const std::string& type) const;
  par_par param_par() const;
  double y_from_eta_wedge(double eta,int plane)const;
  double eta_wedge_from_y(double y,int plane)const;
  int ybin(float32fixed<18> y,int plane=0)const;
  int ybin(float32fixed<yzdex> y,int plane=0)const;
  int ybin(double y,int plane=0)const;

  //fill the tables
  void Local_Slope_A_B();
  void Slope_Components_ROI();
  void Delta_theta_optimization_LG();
  void fill_full_Ak_Bk();
  void fill_slims();
  void fill_crep_table(const std::string&dir,const std::string&tag);
  void fill_yzmod();
  void index_key_test();

  //eta-phi stuff
  int eta_bin(double theta) const;
  std::string eta_str(int eta) const;
  std::string phi_str(int eta) const;
  int phi_bin(double theta) const;

  //table calculations
  std::pair<double,double> ak_bk_hit_bins(const std::vector<int>& hits)const;

  std::vector<bool> lcl_int_to_xhits(int lcl_int)const;
  int xhits_to_lcl_int(const std::vector<bool>& xhits) const;
  std::string lcl_int_to_xhit_str(int lcl_int)const;
  bool toggle_key(std::vector<int>& key,int lo_value,int hi_value)const;
  void key_to_indices(const std::vector<int>& key,int& xdex,int& ybin,int& which)const;
  std::vector<int> indices_to_key(int xdex,int ybin,int which)const;

  //sim corr indexing stuff
  int nsimmax_1d()const;//number of sim cases
  int bool_to_index(const std::vector<bool>&track)const;
  std::vector<bool> index_to_bool(int index)const;
  std::string index_to_hit_str(int index)const;
  std::string bool_to_hit_str(const std::vector<bool>&track)const;

  //table
  std::map<std::vector<int>,std::pair<float32fixed<2>,float32fixed<2> > > AB_k_local;
  std::vector<std::vector<std::vector<float32fixed<zbardex> > > > Ak_local_slim;//[x_hit combo][ybin][case #]
  std::vector<std::vector<std::vector<float32fixed<bkdex> > > > Bk_local_slim;//[x_hit combo][ybin][case #]
  std::vector<std::vector<std::vector<float32fixed<4> > > > Slope_to_ROI;
  std::vector<std::vector<float32fixed<2> > > DT_Factors;
  //theta, phi, hit code, theta/phi/dtheta
  //hit code: binary stuff....
  //old hit code...%mis X, %mis UV: 2-4 X, 1-4 UV possible fractions: 0,1/2,1/3,2/3,1/4,(3/4,not possible with only one misaligned multiplet), 1: 0,3,4,6,8,12
  std::vector<std::vector<std::vector<std::vector<float> > > >crep_table;
  std::vector<std::vector<std::vector<float32fixed<yzdex> > > >ymod,zmod;

  //a toggle
  bool fill0;

  //how many etabins (of equal size) for algorithm results
  int n_etabins,n_phibins;
  std::vector<double>m_etabins,m_phibins;
  //currently configurable parameters
  bool diag,dlm_new;
  float32fixed<2> h;
  int CT_x,CT_uv;
  float32fixed<2> uv_error;
  double dtheta_cut;
  std::string setup;
  bool islarge,genbg;
  double chargeThreshold;
  //new, standardized, misalignment and correction information
  std_align misal,correct;

  //misalignment parameters
  int ybins,n_stations_eta;
  bool misalign,val_tbl;

  //dimensions
  float32fixed<18> w1, w2, w3, h1, h2, h3, H, Hnom, L, wedge_opening_angle;
  float32fixed<4> strip_width;
  float32fixed<4> stereo_degree;
  double stereo_strip_separation_top;
  double stereo_strip_separation_bottom;

  std::vector<float32fixed<18> > z_nominal;
  std::vector<std::vector<float32fixed<18> > > z_large;//[y bin][plane]
  std::vector<std::vector<float32fixed<18> > > ybases;//by stationEta--saved from file, hardcoded, alternative is equally spaced, in MMT_Loader::Get_Strip_Id
  float32fixed<2> m_x_min,m_x_max,m_y_min,m_y_max,h_mx, h_my;
  int n_x,n_y;

  float32fixed<3> slope_min, slope_max;
  float32fixed<2> x_error;
  int CT, CT_u, CT_v;

  float32fixed<4> minimum_large_theta, maximum_large_theta;
  float32fixed<4> minimum_large_phi, maximum_large_phi;

  int n_theta_rois, n_phi_rois, BC_window;

  float32fixed<18> mid_plane_large_X, mid_plane_large, mid_plane_large_UV;
  float32fixed<4> vertical_strip_width_UV;

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;


};

struct mm_digit_entry{
  int multiplet,gas_gap;
  double global_time,time;
  TVector2 local_pos, stripl_pos;
  TVector3 stripg_pos;
  double charge;
};

struct evInf_entry{
  evInf_entry(int event=0,int pdg=0,double e=0,double p=0,double ieta=0,double peta=0,double eeta=0,double iphi=0,double pphi=0,double ephi=0,
	      double ithe=0,double pthe=0,double ethe=0,double dth=0,int trn=0,int mun=0,const TVector3& tex=TVector3(),
	      int troi=0,int antev=0,int postv=0,int nxh=0,int nuvh=0,int nxbg=0,int nuvbg=0,double adt=0,int difx=0,
	      int difuv=0,bool cut=false,bool bad=false);
  void print() const;

  int athena_event,pdg_id;
  double E,pt,eta_ip,eta_pos,eta_ent,phi_ip,phi_pos,phi_ent,theta_ip,theta_pos,theta_ent,dtheta;
  int truth_n,mu_n;
  TVector3 vertex;
  int truth_roi;
  int N_hits_preVMM,N_hits_postVMM/*<--true*/;
  int N_X_hits,N_UV_hits;//signal pre vmm; the X and UV hits that count in efficiency denominator
  int NX_bg_preVMM,NUV_bg_preVMM;
  double avg_drift_time;
  int max_less_min_hitsteps_X,max_less_min_hitsteps_UV;
  bool pass_cut,bad_wedge;


};

struct hitData_key{
  hitData_key(int bct=0, double t=0, double gt=0, int vmm=-1,int ev=-1);
  hitData_key(const hitData_key& key){BC_time=key.BC_time;time=key.time;gtime=key.gtime;VMM_chip=key.VMM_chip;event=key.event;}
  //for these operators, "less" means what you think it means
  //with precedence of quantities the same as order in the constructor
  //in the context of "earlier," it might make sense to say that a hit is earlier if,
  //when all times tied, the larger plane is earlier, but this assumes a everything is signal like
  bool operator==(const hitData_key& merp) const;
  bool operator!=(const hitData_key& merp) const;
  bool operator<(const hitData_key& merp) const;
  bool operator>(const hitData_key& merp) const;
  bool operator<=(const hitData_key& merp) const;
  bool operator>=(const hitData_key& merp) const;
  std::string hdr()const;
  std::string str()const;
  void print() const;

  int BC_time;
  double time,gtime;
  int VMM_chip,event;


};

struct evFit_entry{
  evFit_entry(int event=0,float32fixed<4> fthe=0.,float32fixed<4> fphi=0.,
	      float32fixed<2> fdth=0.,int roi=-1,int xhit=false,int uvhit=false,
	      int bgx=false,int bguv=false,float32fixed<2> dth_nd=0.,int hc=0,int tph=0,
	      int bgph=0);

  void print() const;

  int athena_event;
  float32fixed<4> fit_theta,fit_phi;
  float32fixed<2> fit_dtheta;
  int fit_roi,X_hits_in_fit,UV_hits_in_fit,bg_X_fit,bg_UV_fit;
  float32fixed<2> dtheta_nodiv;
  int hcode,truth_planes_hit,bg_planes_hit;
  std::vector<hitData_key> fit_hit_keys;


};

struct evAna_entry{
  int athena_event;
  double theta_err,phi_err,dtheta_err;
  bool qualified_event, fit, good_fit;
  int X_hits_in_fit,UV_hits_in_fit;
  int bg_X_fit,bg_UV_fit;
};

struct hitData_info{
  hitData_info(int plane,int station_eta,int strip,MMT_Parameters *par,const TVector3& tru,double tpos,double ppos);
  hitData_info(int the_pl=0,double the_y=0,double the_z=-999);
  hitData_info(const hitData_info& info){plane=info.plane;y=info.y;z=info.z;slope=info.slope;}
  double mis_dy(int pl,MMT_Parameters *m_par,double tpos,double ppos)const;
  std::string hdr()const;
  std::string str()const;
  void print()const;
  bool operator==(const hitData_info& rhs) const;

  //members
  int plane;
  float32fixed<yzdex> y,z;//actual values divided by store_const() to make fixed point calculations doable--all this stuff is dimensionless in the end, so it's okay.
  float32fixed<2> slope;

};

struct Hit{
  //make a well-behaved constructor
  Hit(const hitData_key&k=hitData_key(),const hitData_info&i=hitData_info());
  bool operator==(const Hit& rhs) const;
  void print_track(const std::vector<Hit>& track) const;
  void print() const;

  //the members:
  hitData_key key;
  hitData_info info;
};

struct hitData_entry{
  hitData_entry(int ev=0, double gt=0, double q=0, int vmm=0, int pl=0, int st=0, int est=0, double tr_the=0, double tru_phi=0,
	     bool q_tbg=0, int bct=0, double time=0,const TVector3& tru=TVector3(), const TVector3& rec=TVector3(),
	     double fit_the=0, double fit_phi=0, double fit_dth=0, double tru_dth=0,// double tru_thl=0, double tru_thg=0,
	     double mxg=0, double mug=0, double mvg=0, double mxl=0, double the_mx=0, double the_my=0, int the_roi=0);

  Hit entry_hit(MMT_Parameters *m_par)const;
  hitData_key entry_key() const;
  hitData_info entry_info(MMT_Parameters *m_par)const;
  void fit_fill(float32fixed<4> fthe,float32fixed<4> fphi, float32fixed<2> fdth, float32fixed<2> mxg=0., float32fixed<2> mug=0., float32fixed<2> mvg=0., float32fixed<2> mxl=0., float32fixed<2> m_x=0., float32fixed<2> m_y=0., int king=0);
  void print() const;

  int event;
  double gtime,charge;
  int VMM_chip,plane,strip,station_eta;
  double tru_theta_ip,tru_phi_ip;
  bool truth_nbg;//truth (i.e. not bg) if true,
  int BC_time;/*fit theta, phi, dtheta originally here*/
  double time;
  TVector3 truth,recon;
  float32fixed<4> fit_theta,fit_phi;
  float32fixed<2> fit_dtheta;
  double tru_dtheta;//,tru_theta_local,tru_theta_global;
  float32fixed<2> M_x_global,M_u_global,M_v_global,M_x_local,mx,my;
  int roi;


};

struct finder_entry{
  //make a well-behaved constructor
  finder_entry(bool the_is_hit=0, int the_clock=-1,const Hit& the_k=Hit());
  bool operator==(const finder_entry& merp) const;
  bool operator!=(const finder_entry& merp) const;

  //Finder stuff:
  bool is_hit,is_ct;
  int clock;
  //Finder_Detail stuff:
  Hit hit;
};

struct ROI{
  //make a well-behaved constructor
  ROI(double the_theta, double the_phi, double the_m_x, double the_m_y, int the_roi);

  //the members:
  float32fixed<4> theta,phi;
  float32fixed<2> m_x,m_y;
  int roi;
};

struct athena_header{
  //make a well-behaved constructor
  athena_header(const TLorentzVector& par=TLorentzVector(), int tpn=0, double etp=0, double ete=0, double php=0, double phe=0, int mun=0, const TVector3& ver=TVector3());

  //the members:
  TLorentzVector the_part;
  int trupart_n;
  double etapos,etaent,phipos,phient;
  int muent_n;
  TVector3 vertex;
};

struct digitWrapper{
  digitWrapper(const MmDigit* digit=0,
               double tmpGTime=0,
               const TVector3& truthLPos=TVector3(),
               const TVector3& stripLPos=TVector3(),
               const TVector3& stripGPos=TVector3()
               );


  const MmDigit* digit;
  double gTime;

  TVector3 truth_lpos;//4,5
  TVector3 strip_lpos;
  TVector3 strip_gpos;//6-11

  inline Identifier id(){ return digit->identify(); };

};

struct track_address{//cartesian_hit_rot entry
  track_address(int bct=0,bool big=false,int wed=0,int pl=-1,int sh=0,const TVector3& chr=TVector3());

  int BC;
  bool islarge;
  int wedge,plane,strip_hit;
  TVector3 cart_hit;
};

#endif
