/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"

#include "TrigT1NSWSimTools/MMT_Fitter.h"

#include <stdexcept>

using std::set;
using std::vector;
using std::pair;
using std::string;


MMT_Fitter::MMT_Fitter(int nlg, double lgmin, double lgmax):
  AthMessaging(Athena::getMessageSvc(), "MMT_Fitter"),
  m_number_LG_regions(nlg), m_LG_min(lgmin), m_LG_max(lgmax)
{
  ATH_MSG_DEBUG("MMT_Fitter start building");
  m_last=0;
  m_n_fit=0;
  ATH_MSG_DEBUG("MMT_Fitter built");
}


evFit_entry MMT_Fitter::fit_event(int event, vector<Hit>& track, vector<hitData_entry>& hitDatas, int& nfit,vector<pair<double,double> >&mxmy, double& mxl, double& mv, double& mu, std::shared_ptr<MMT_Parameters> par) const{
  ATH_MSG_DEBUG("Begin fit event!");
  bool did_fit=false;
  int check=Filter_UV(track,par);
  vector<int> xpl=par->q_planes("x");
  vector<int> upl=par->q_planes("u");
  vector<int> vpl=par->q_planes("v");
  //---- Calc global slopes and local X slope -----
  double M_x_global = Get_Global_Slope(track,"x",par);
  double M_u_global = (check>=10?(-999.):Get_Global_Slope(track,"u",par));
  double M_v_global = (check%10==1?(-999.):Get_Global_Slope(track,"v",par));
  //----  Calc delta theta ----------
  //----- Calc ROI ----------
  ROI ROI = Get_ROI(M_x_global,M_u_global,M_v_global,track,par);
  mxmy.push_back(pair<double,double>(ROI.m_x,ROI.m_y));
  mu  = M_u_global;
  mv  = M_v_global;

  if(ROI.theta==-999){
    for(unsigned int i=0;i<track.size();i++)  track[i].print();
    ATH_MSG_WARNING("SOMETHING IS OFF!  fit_event, ROI has theta=-999 (bad reconstruction) \n");
    evFit_entry empty(event,-999,-999,-999,ROI.roi,-999,-999,-999,-999,-999,track_to_index(track, par));
    return empty;
    //throw std::runtime_error("MMT_Fitter::fit_event: invalid ROI.theta");
  }
  static_assert(std::is_trivially_copyable<double>::value);
  static_assert(std::is_trivially_destructible<double>::value);
  double M_x_local   = Get_Local_Slope(track, ROI.theta, ROI.phi, par);
  double Delta_Theta_division = Get_Delta_Theta_division(M_x_local, M_x_global, 1.);
  double Delta_Theta = Get_Delta_Theta(M_x_local, M_x_global, par);
  double dtheta_idl  = Get_Delta_Theta_division(ideal_local_slope(track,par), M_x_global, 1.);

  mxl = M_x_local;

  if(dtheta_idl-std::abs(Delta_Theta_division) > 2.e-3) par->fill0 = true;
  ATH_MSG_DEBUG("Mxg=" << M_x_global << ", Mug=" << M_u_global << ", Mvg=" << M_v_global << ", Mxl=" << M_x_local << ", dth=" <<Delta_Theta);
  //@@@@@@@@ Begin Info Storage for Later Analysis @@@@@@@@@@@@@@@@@@@@@@@
  vector<bool> planes_hit_tr(8,false), planes_hit_bg(8,false);
  for(unsigned int ihit=0; ihit<track.size(); ihit++){
    int hitData_pos = find_hitData(hitDatas,track[ihit].key);
    if(hitData_pos == -1) continue;
    if(hitDatas[hitData_pos].truth_nbg) planes_hit_tr[track[ihit].info.plane]=true;
    else planes_hit_bg[track[ihit].info.plane]=true;
  }
  int n_xpl_tr=0, n_xpl_bg=0, n_uvpl_tr=0, n_uvpl_bg=0;
  for(int xp=0; xp<(int)xpl.size(); xp++){
    int plane = xpl[xp];
    n_xpl_tr += planes_hit_tr[plane];
    n_xpl_bg += planes_hit_bg[plane];
  }
  if(check<10){
    for(unsigned int up=0; up<upl.size(); up++){
      int plane = upl[up];
      n_uvpl_tr += planes_hit_tr[plane];
      n_uvpl_bg += planes_hit_bg[plane];
    }
  }
  if(check%10==0){
    for(unsigned int vp=0; vp<vpl.size(); vp++){
      int plane = vpl[vp];
      n_uvpl_tr += planes_hit_tr[plane];
      n_uvpl_bg += planes_hit_bg[plane];
    }
  }

  //FINISH ME!!!!!!!
  double candtheta=ROI.theta,candphi=ROI.phi;
  /* I think this bit appears redundant but could end up being stupid; the fitter shouldn't care about CT stuff (beyond having min num hits to do fit, which the -999 or w/e is responsible for taking care of)
  bool xfail=(n_xpl_tr+n_xpl_bg<par->CT_x),uvfail= (n_uvpl_tr+n_uvpl_bg<par->CT_uv);
  msg(MSG::DEBUG) << n_xpl_tr+n_xpl_bg<<" x hits");
  if(xfail)candtheta=-999.;
  if(uvfail)candphi=-999.;
  */
  bool fitkill = (ROI.theta==-999 || Delta_Theta==-999||Delta_Theta==-4);// ||xfail||uvfail);
  ATH_MSG_DEBUG("HIT CODE: " << track_to_index(track, par));
  evFit_entry aemon(event,candtheta,candphi,Delta_Theta_division,ROI.roi,n_xpl_tr,n_uvpl_tr,n_xpl_bg,n_uvpl_bg,Delta_Theta,track_to_index(track, par));
  if(fitkill) return aemon;

  int nplanes = par->setup.size();
  for(int plane=0; plane<nplanes; plane++){
    if(track[plane].info.slope == -999) continue; //&& Delta_Theta_division~=-999
    int hitData_pos = find_hitData(hitDatas, track[plane].key);
    if(hitData_pos == -1) continue;
    did_fit = true;
    hitDatas[hitData_pos].fit_fill(ROI.theta,ROI.phi,Delta_Theta,M_x_global,M_u_global,M_v_global,M_x_local,ROI.m_x,ROI.m_y,ROI.roi);
    aemon.fit_hit_keys.push_back(track[plane].key);
    if(hitDatas[hitData_pos].truth_nbg) aemon.truth_planes_hit += std::pow(10,nplanes-plane-1);
     else aemon.bg_planes_hit += std::pow(10,nplanes-plane-1);
  }
  if(did_fit) nfit++;
  return aemon;
}

int MMT_Fitter::find_hitData(const vector<hitData_entry>& hitDatas, const hitData_key& key) const{
  for(unsigned int i=0;i<hitDatas.size();i++){
    if(hitDatas[i].BC_time==key.BC_time && hitDatas[i].time==key.time && hitDatas[i].gtime==key.gtime && hitDatas[i].VMM_chip==key.VMM_chip) return i;
  }
  return -1;
}


int MMT_Fitter::Filter_UV(vector<Hit>& track, std::shared_ptr<MMT_Parameters> par) const{
  return 0;
  double h=par->h, tolerance = h;//*2;  //Can be optimized...
  vector<int> u_planes = par->q_planes("u"), v_planes = par->q_planes("v");
  vector<Hit> u_hits = q_hits("u",track,par), v_hits = q_hits("v",track,par);
  bool pass_u =! u_hits.empty(), pass_v =! v_hits.empty();

  //if the difference in slope between the first and last u/v planes is too great don't pass, set track hits to zero
  if(pass_u){
    if(std::abs( double(u_hits.front().info.slope-u_hits.back().info.slope)) > tolerance) pass_u=false;
  }
  if(pass_v){
    if(std::abs( double(v_hits.front().info.slope-v_hits.back().info.slope)) > tolerance) pass_v=false;
  }
  int return_val = 0; //of form (bool ubad, bool vbad), so 10 would be bad u, good v
  if(!pass_u){
    for(unsigned int iup=0; iup<u_planes.size(); iup++) track[u_planes[iup]].info.slope = -999;
    return_val += 10;
  }
  if(!pass_v){
    for(unsigned int ivp=0; ivp<v_planes.size(); ivp++) track[v_planes[ivp]].info.slope = -999;
    return_val += 1;
  }
  return return_val;
}

double MMT_Fitter::Get_Global_Slope (const vector<Hit>& track, const string& type, std::shared_ptr<MMT_Parameters> par) const
{
  vector<Hit> qhits = q_hits(type,track,par);
  double sum = 0.;
  if(qhits.size()==0) return -999;
  double nhitdiv= 1./qhits.size();
  for(int ihit=0;ihit<(int)qhits.size();ihit++){
    sum+=(qhits[ihit].info.slope*nhitdiv);
  }
  return sum;
}

//CHANGE!
double MMT_Fitter::Get_Local_Slope (const vector<Hit>& Track,double theta,double phi,std::shared_ptr<MMT_Parameters> par) const
{
  vector<int> x_planes = par->q_planes("x"), ybin_hits(x_planes.size(),-1);
  int nxp=x_planes.size();
  for(int ipl=0; ipl<nxp; ipl++){
    ybin_hits[ipl]=(((Track[x_planes[ipl]].info.slope==-999) || (Track[x_planes[ipl]].info.slope==-4)) ? -1 : par->ybin(Track[x_planes[ipl]].info.y,x_planes[ipl]));
  }
  bool hit=false;
  double yzsum=0;
  double ysum=0;//added to try alternative calculus
  double zsum=0;//added to try alternative calculus
  float mxlf=0;
  int xdex=-1;
  int ybin=-1;
  int which=-1;
  par->key_to_indices(ybin_hits, xdex, ybin, which);
  if(xdex<0 || ybin<0 || which<0) return -999;

  double zbar=par->Ak_local_slim[xdex][ybin][which];
  double bk=par->Bk_local_slim[xdex][ybin][which];
  ATH_MSG_DEBUG("zbar is " << zbar << ", and bk is " << bk);
  int ebin = par->eta_bin(theta);
  int pbin = par->phi_bin(phi);
  for(int ipl=0; ipl<nxp; ipl++){
    double z = Track[x_planes[ipl]].info.z;
    double y = Track[x_planes[ipl]].info.y;
    double zfirst = Track[x_planes[0]].info.z;
    double yfirst = Track[x_planes[0]].info.y;
    if(ebin != -1){
      z = z+par->zmod[ebin][pbin][ipl];
      y = y+par->ymod[ebin][pbin][ipl];
    }
    if(Track[x_planes[ipl]].info.slope == -999 || Track[x_planes[ipl]].info.slope == -4) continue;
    hit=true;
    yzsum+=y*(z*zbar-1.);
    mxlf += bk*y*(z*zbar-1.);
    //alternative calculus
    ysum += y-yfirst;
    zsum += z-zfirst;
  }
  //double mxl=double(bk*yzsum); //old calculus
  double mxl=(ysum/zsum); //new calculus
  if(!hit) {return double(999);}
  return mxl;
}

int MMT_Fitter::track_to_index(const vector<Hit>&track, std::shared_ptr<MMT_Parameters> par)const{
  vector<bool>hits(par->setup.size(),false);
  for(int ihit=0; ihit<(int)track.size(); ihit++) hits[track[ihit].info.plane] = (hits[track[ihit].info.plane] ? true : track[ihit].info.slope>-2.);
  return par->bool_to_index(hits);
}

double MMT_Fitter::ideal_local_slope(const vector<Hit>& Track, std::shared_ptr<MMT_Parameters> par)const{
  vector<vector<double> > z_hit;
  for(unsigned int i = 0; i<par->z_large.size(); i++){
    vector<double> temp;
    for(unsigned int j = 0; j<par->z_large[i].size(); j++) temp.push_back(par->z_large[i][j]);
    z_hit.push_back(temp);
  }
  vector<int> x_planes=par->q_planes("x");
  int nxp=x_planes.size();
  bool hit=false;
  double sum_xy=0, sum_y=0;
  double ak_idl = ideal_ak(Track, par);
  double bk_idl = ak_idl*ideal_zbar(Track,par);
  for(int ipl=0; ipl<nxp; ipl++){
    double y = Track[x_planes[ipl]].info.y;
    double z = Track[x_planes[ipl]].info.z;
    if(y==-999) continue;
    hit=true;
    sum_xy += ak_idl*z*y;
    sum_y  += bk_idl*y;
  }
  if(!hit) return -10.;
  double ls_idl=sum_xy-sum_y;
  return ls_idl;
}

double MMT_Fitter::ideal_z(const Hit& hit, std::shared_ptr<MMT_Parameters> par)const{
  int plane=hit.info.plane;
  double tilt = (plane<4 ? par->correct.rotate.X() : 0);
  double dz   = (plane<4 ? par->correct.translate.Z() : 0);
  double nominal = par->z_nominal[plane];
  double y = hit.info.y - par->ybases[plane].front();
  double z = nominal+dz+y*std::tan(tilt);
  return z;
}

double MMT_Fitter::ideal_ak(const vector<Hit>& Track, std::shared_ptr<MMT_Parameters> par)const{
  vector<int> x_planes=par->q_planes("x");//this tells us which planes are x planes
  int n_xplanes=x_planes.size(),hits=0;
  double sum_x=0,sum_xx=0;
  for(int ip=0; ip<n_xplanes; ip++){
    if(Track[x_planes[ip]].info.slope==-999) continue;
    double addme=ideal_z(Track[x_planes[ip]],par);//z_hit[x_planes[plane]];
    hits++;//there's a hit
    sum_x  += addme;
    sum_xx += addme*addme;
    ATH_MSG_DEBUG("z[" << ip << "]=" << addme << ", sum_x=" << sum_x << ", sum_xx=" << sum_xx << ", hits=" << hits);
  }
  double diff = hits*sum_xx-sum_x*sum_x;
  return hits/diff;
}

double MMT_Fitter::ideal_zbar(const vector<Hit>& Track, std::shared_ptr<MMT_Parameters> par)const{
  vector<int> x_planes = par->q_planes("x");//this tells us which planes are x planes
  double ztot=0;
  int nhit=0;
  for(unsigned int ip=0; ip<x_planes.size(); ip++) {
    if(Track[x_planes[ip]].info.slope == -999) continue;
    nhit++;
    ztot += ideal_z(Track[x_planes[ip]],par);
  }
  return ztot/nhit;
}

double MMT_Fitter::Get_Delta_Theta(double M_local, double M_global, std::shared_ptr<MMT_Parameters> par) const{
  int region=-1;
  double LG = M_local * M_global;
  for(int j=0;j<m_number_LG_regions;j++){   //m_number_LG_regions
    if(LG <= DT_Factors_val(j,0,par)){
      region = j;
      break;
    }
  }
  if(region==-1) return -999;
  return DT_Factors_val(region,1,par)*(M_local - M_global);
}

double MMT_Fitter::DT_Factors_val(int i, int j, std::shared_ptr<MMT_Parameters> par) const{
  if(par->val_tbl){
    return par->DT_Factors[i][j];
  }
  if(j<0 || j>1){
    ATH_MSG_WARNING("DT_Factors only has two entries on the second index (for LG and mult_factor); you inputed an index of " << j );
    throw std::runtime_error("MMT_Fitter::DT_Factors_val: invalid index");
  }
  if(i<0 || i>=m_number_LG_regions){
    ATH_MSG_WARNING("There are " << m_number_LG_regions << " in DT_Factors(_val); you inputed an index of " << i );
    throw std::runtime_error("MMT_Fitter::DT_Factors_val: invalid index");
  }
  double a=1.;//not sure what this is for, so hard to choose fixed_point algebra
  if(j==0) return mult_factor_lgr(i,a,m_number_LG_regions,m_LG_min,m_LG_max);
  return LG_lgr(i,a,m_number_LG_regions,m_LG_min,m_LG_max);
}

double MMT_Fitter::LG_lgr(int ilgr, double a, int number_LG_regions, double min, double max) const{
  a+=0;
  return min+double(ilgr/number_LG_regions)*(max-min);
}

double MMT_Fitter::mult_factor_lgr(int ilgr, double a, int number_LG_regions, double min, double max) const{
  return double(1.) / double(  (   LG_lgr(ilgr,a,number_LG_regions,min,max)  /  a  + a  ) );
}

double MMT_Fitter::Get_Delta_Theta_division(double M_local, double M_global, double a) const{

  //we could use 2 bits for the numerator and 3 for the denominator, but then
  //fixed_point doesn't know how to do the algebra. Assume we know how to do
  //this division (we don't, efficiently, thus the method Get_Delta_Theta
  return double( (M_local - M_global) / ( (M_local*M_global) / a  +  a  ));
}

vector<Hit> MMT_Fitter::q_hits(const string& type,const vector<Hit>& track, std::shared_ptr<MMT_Parameters> par) const{
  string setup(par->setup);
  if(setup.length()!=track.size()){
    ATH_MSG_WARNING("Setup has length: "<<setup.length()<<", but there are "<<track.size()<<" hits in the track");
    throw std::runtime_error("MMT_Fitter::q_hits: inconsistent setup");
  }
  vector<int> qpl(par->q_planes(type));
  vector<Hit> q_hits;
  for(unsigned int ihit=0; ihit<qpl.size(); ihit++){
    if( !(track[qpl[ihit]].info.slope==-999) )  {q_hits.push_back(track[qpl[ihit]]);
    }
  }
  return q_hits;
}

//change this to take u and/or v out of the roi calculation
ROI MMT_Fitter::Get_ROI(double M_x, double M_u, double M_v, const vector<Hit>&track, std::shared_ptr<MMT_Parameters> par) const{
  //M_* are all global slopes
  ATH_MSG_DEBUG("\nGet_ROI(" << M_x << "," << M_u << "," << M_v << ") ");

  //--- calc constants ------
  double b=M_PI/180.0*(par->stereo_degree);
  double A=1./std::tan(b);
  double B=1./std::tan(b);

  //---  slope conversion equations ----
  double my = M_x;
  double mxu = ( A*M_u - B*my );
  double mxv = ( B*my - A*M_v );
  //--- which slopes are truly present ----
  //Note that bad slopes are not necessarily 0 as I often use -999 to denote something missing
  //we have -999 for M_u or M_v to denote that it didn't pass filtering
  int nu=1, nv=1;
  if(M_u<0 || M_u==-999){
    mxu=0;
    nu=0;
  }
  if(M_v<0 || M_v==-999){
    mxv=0;
    nv=0;
  }
  if(nu==0 && nv==0) return ROI(-999,-999,-999,-999,-999);

  //--- average of 2 mx slope values ----if both u and v were bad, give it a -999 value to know not to use m_x
  //*** check to see if U and V are necessary for fit
  double mx = (nu+nv==0 ? 0 : (mxv+mxu)/(nu+nv));
  if(par->correct.translate.X()!=0 && par->correct.type==2){
    mx += phi_correct_factor(track,par)*par->correct.translate.X()/par->z_nominal[3];
  }
  ATH_MSG_DEBUG("(b,A,B,my,mxu,mxv,mx)=(" << b << "," << A << "," << B << "," << my << "," << mxu << "," << mxv << "," << mx << ")\n");

  //Get mx and my in parameterized values
  int a_x = std::round((mx-par->m_x_min)/par->h_mx);
  int a_y = std::round((my-par->m_y_min)/par->h_my);
  // Generally, this offers a reality check or cut.  The only reason a slope
  // should be "out of bounds" is because it represents a weird UV combination
  // -- ie. highly background influenced
  if(a_y>par->n_y || a_y<0){
    ATH_MSG_DEBUG( "y slope (theta) out of bounds in Get_ROI....(a_x,a_y,par->n_x,par->n_y)=("<< a_x << "," << a_y << "," << par->n_x << "," << par->n_y << ")");
    return ROI(-999,-999,-999,-999,-999);
  }

  if(a_x>par->n_x || a_x<0){
    ATH_MSG_DEBUG( "x slope (phi) out of bounds in Get_ROI....(a_x,a_y,par->n_x,par->n_y)=(" << a_x << "," << a_y << "," << par->n_x << "," <<par->n_y << ")");
    return ROI(-999,-999,-999,-999,-999);
  }

  ATH_MSG_DEBUG("fv_angles...(a_x,a_y)=("<<a_x<<","<<a_y<<")");
  double phicor=0.;
  if(par->correct.rotate.Z()!=0 && par->correct.type==2){
    phicor=-0.2*par->correct.rotate.Z();
  }

  double fv_theta = Slope_Components_ROI_theta(a_y,a_x,par);
  double fv_phi = (mx==0) ? -999 : Slope_Components_ROI_phi(a_y,a_x,par)+phicor;
  ATH_MSG_DEBUG("fv_theta=" << fv_theta << ", fv_phi=" << fv_phi);

  //--- More hardware realistic approach but need fine tuning ----
  int roi = Rough_ROI_temp(fv_theta,fv_phi,par);

  //--- current "roi" which is not an actual roi but an approx phi and theta
  return ROI(fv_theta,fv_phi,mx,my,roi);
}

double MMT_Fitter::phi_correct_factor(const vector<Hit>&track, std::shared_ptr<MMT_Parameters> par)const{
  if((par->correct.rotate.Z()==0 && par->correct.translate.X()==0) || par->correct.type!=2) return 0.;
  int nxmis=0, nx=0, numis=0, nu=0, nvmis=0, nv=0;
  double xpart=0.5, upart=0.5, vpart=0.5;
  string set=par->setup;
  for(int ihit=0;ihit<(int)track.size();ihit++){
    int n_pln=track[ihit].info.plane;
    bool ismis=n_pln<4;
    char pln=set[n_pln];
    if(pln=='x'||pln=='X'){
      nx++;
      nxmis+=ismis;
    } else if(pln=='u'||pln=='U') {
      nu++;
      numis+=ismis;
    } else if(pln=='v'||pln=='V') {
      nv++;
      nvmis+=ismis;
    }
  }
  if(nu==0 && nv==0)return 0.;
  if(nu==0) upart=0.;
  else if(nv==0)vpart=0.;
  else xpart=0.;
  return xpart*1.*nxmis/nx+upart*1.*numis/nu+vpart*1.*nvmis/nv;
}

double MMT_Fitter::Slope_Components_ROI_val(int jy, int ix, int thetaphi, std::shared_ptr<MMT_Parameters> par) const{
  if(par->val_tbl){
    return par->Slope_to_ROI[jy][ix][thetaphi];
  }
  if(thetaphi<0 || thetaphi>1){
    ATH_MSG_WARNING("Slope_Components_ROI only has two entries on the third index (for theta and phi); you inputed an index of " << thetaphi);
    throw std::runtime_error("MMT_Fitter::Slope_Components_ROI_val: invalid number of entries");
  }
  if(thetaphi==0) return Slope_Components_ROI_theta(jy,ix,par);
  return Slope_Components_ROI_phi(jy,ix,par);
}

double MMT_Fitter::Slope_Components_ROI_theta(int jy, int ix, std::shared_ptr<MMT_Parameters> par) const{
  //get some parameter information
  if(jy<0 || jy>=par->n_y){
    ATH_MSG_WARNING("You picked a y slope road index of " << jy << " in Slope_Components_ROI_theta; there are only " << par->n_y << " of these.\n");
    if(jy >= par->n_y) jy=par->n_y-1;
    else jy=0;
  }
  if(ix<0||ix>=par->n_x){
    ATH_MSG_WARNING("You picked an x slope road index of " << ix << " in Slope_Components_ROI_theta; there are only " << par->n_x << " of these.\n");
    if(ix >= par->n_x) ix=par->n_x-1;
    else ix=0;
  }
  int xdex=ix, ydex=jy+1;
  if(xdex==0) xdex++;
  double mx = par->m_x_min+par->h_mx*xdex;
  double my = par->m_y_min+par->h_my*ydex;
  double theta = std::atan(sqrt( (mx*mx+my*my) ));
  if(theta<par->minimum_large_theta || theta>par->maximum_large_theta){
    theta=0;
  }
  return theta;
}

double MMT_Fitter::Slope_Components_ROI_phi(int jy, int ix, std::shared_ptr<MMT_Parameters> par) const{
  if(jy<0 || jy>=par->n_y){
    ATH_MSG_WARNING("You picked a y slope road index of " << jy << " in Slope_Components_ROI_phi; there are only " << par->n_y << " of these.\n");
    if(jy >= par->n_y) jy=par->n_y-1;
    else jy=0;
  }
  if(ix<0 || ix>=par->n_x){
    ATH_MSG_WARNING("You picked an x slope road index of " << ix << " in Slope_Components_ROI_phi; there are only " << par->n_x << " of these.\n");
    //right now we're assuming these are cases just on the edges and so put the values to the okay limits
    if(ix >= par->n_x) ix=par->n_x-1;
    else ix=0;
  }
  int xdex=ix, ydex=jy+1;
  double mx=par->m_x_min+par->h_mx*xdex;
  double my=par->m_y_min+par->h_my*ydex;
  ATH_MSG_DEBUG("par->m_x_min+par->h_mx*xdex=" << par->m_x_min << "+" << par->h_mx << "*" << xdex << "=" << mx << ", ");
  ATH_MSG_DEBUG("par->m_y_min+par->h_my*ydex=" << par->m_y_min << "+" << par->h_my << "*" << ydex << "=" << my << ", ");

  double phi = std::atan2(mx,my);//the definition is flipped from what you'd normally think
  ATH_MSG_DEBUG("for a phi of " << phi);
  if(phi < par->minimum_large_phi || phi > par->maximum_large_phi){
    ATH_MSG_DEBUG("Chucking phi of " << phi <<" which registers as not in [" << par->minimum_large_phi << "," << par->maximum_large_phi << "]");
    phi=999;
  }
  return phi;
}

int MMT_Fitter::Rough_ROI_temp(double theta, double phi, std::shared_ptr<MMT_Parameters> par) const{
  //temporary function to identify areas of the wedge.
  double minimum_large_theta = par->minimum_large_theta;
  double maximum_large_theta = par->maximum_large_theta;
  double minimum_large_phi = par->minimum_large_phi;
  double maximum_large_phi = par->maximum_large_phi;
  int n_theta_rois=32, n_phi_rois=16;//*** ASK BLC WHAT THESE VALUES OUGHT TO BE!

  double h_theta = (maximum_large_theta - minimum_large_theta)/n_theta_rois;
  double h_phi = (maximum_large_phi - minimum_large_phi)/n_phi_rois;
  //how is this done in the FPGA? No such division, for sure
  double roi_t = std::ceil( (  (theta - minimum_large_theta)/h_theta));
  double roi_p = std::ceil( (  (phi - minimum_large_phi)/h_phi));

  if(theta<minimum_large_theta || theta>maximum_large_theta) roi_t = 0;
  if(phi<minimum_large_phi || phi>maximum_large_phi) roi_p = 0;
  int ret_val=roi_t * 1000 + roi_p;
  return ret_val;
}
