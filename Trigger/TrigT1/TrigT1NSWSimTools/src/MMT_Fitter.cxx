/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Athena/Gaudi includes
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "MMT_Fitter.h"



MMT_Fitter::MMT_Fitter(MMT_Parameters *par, int nlg, double lgmin, double lgmax): /*m_par(par),*/ number_LG_regions(nlg), LG_min(lgmin), LG_max(lgmax), m_msg("MMT_Fitter")
{
  msg(MSG::DEBUG) << "MMT_F::building fitter"<< endmsg;
  m_par=par;
  last=0;
  n_fit=0;
  msg(MSG::DEBUG) << "MMT_F::built fitter"<< endmsg;
}


evFit_entry MMT_Fitter::fit_event(int event, vector<Hit>& track, vector<hitData_entry>& hitDatas, int& nfit,vector<pair<double,double> >&mxmy, double& mxl, double& mv, double& mu) const{
  msg(MSG::DEBUG) << "Begin fit event!"<< endmsg;
  bool did_fit=false;
  int check=Filter_UV(track);
  vector<int> xpl=m_par->q_planes("x");
  vector<int> upl=m_par->q_planes("u");
  vector<int> vpl=m_par->q_planes("v");
  //---- Calc global slopes and local X slope -----
  float32fixed<2> M_x_global = Get_Global_Slope(track,"x");
  float32fixed<2> M_u_global = (check>=10?float32fixed<2>(-999.):Get_Global_Slope(track,"u"));
  float32fixed<2> M_v_global = (check%10==1?float32fixed<2>(-999.):Get_Global_Slope(track,"v"));
  //----  Calc delta theta ----------
  //----- Calc ROI ----------
  ROI ROI = Get_ROI(M_x_global,M_u_global,M_v_global,track);
  mxmy.push_back(pair<double,double>(ROI.m_x.getFloat(),ROI.m_y.getFloat()));
  mu  = M_u_global.getValue();
  mv  = M_v_global.getValue();

  if(ROI.theta==-999){
    for(unsigned int i=0;i<track.size();i++)  track[i].print();
    ATH_MSG_WARNING("SOMETHING IS OFF!  fit_event\n");
    exit(-999);
  }

  float32fixed<2> M_x_local = Get_Local_Slope(track,ROI.theta.getFloat(),ROI.phi.getFloat()),Delta_Theta_division = Get_Delta_Theta_division(M_x_local,M_x_global,1.), Delta_Theta = Get_Delta_Theta(M_x_local,M_x_global), dtheta_idl=Get_Delta_Theta_division(ideal_local_slope(track),M_x_global);

  mxl = M_x_local.getValue();

  if(abs(dtheta_idl-Delta_Theta_division)>2.e-3)m_par->fill0=true;
  msg(MSG::DEBUG) << "Mxg="<<M_x_global.getValue()<<",Mug="<<M_u_global.getValue()<<",Mvg="<<M_v_global.getValue()<<",Mxl="<<M_x_local.getValue()<<",dth="<<Delta_Theta.getValue()<< endmsg;
  //@@@@@@@@ Begin Info Storage for Later Analysis @@@@@@@@@@@@@@@@@@@@@@@
  vector<bool> planes_hit_tr(8,false),planes_hit_bg(8,false);
  for(unsigned int ihit=0; ihit<track.size(); ihit++){
    int hitData_pos=find_hitData(hitDatas,track[ihit].key);
    if(hitData_pos==-1) continue;
    if(hitDatas[hitData_pos].truth_nbg) planes_hit_tr[track[ihit].info.plane]=true;
    else planes_hit_bg[track[ihit].info.plane]=true;
  }
  int n_xpl_tr=0,n_xpl_bg=0,n_uvpl_tr=0,n_uvpl_bg=0;
  for(int xp=0; xp<(int)xpl.size(); xp++){
    int plane=xpl[xp];
    n_xpl_tr+=planes_hit_tr[plane];n_xpl_bg+=planes_hit_bg[plane];
  }
  if(check<10){
    for(unsigned int up=0; up<upl.size(); up++){
      int plane=upl[up];
      n_uvpl_tr+=planes_hit_tr[plane];
      n_uvpl_bg+=planes_hit_bg[plane];
    }
  }
  if(check%10==0){
    for(unsigned int vp=0; vp<vpl.size(); vp++){
      int plane=vpl[vp];
      n_uvpl_tr+=planes_hit_tr[plane];
      n_uvpl_bg+=planes_hit_bg[plane];
    }
  }
//   return evFit_entry(event,ROI.theta,ROI.phi,Delta_Theta_division,ROI.roi,n_xpl_tr,n_uvpl_tr,n_xpl_bg,n_uvpl_bg,Delta_Theta);

  //FINISH ME!!!!!!!
  float32fixed<4> candtheta=ROI.theta,candphi=ROI.phi;
  /* I think this bit appears redundant but could end up being stupid; the fitter shouldn't care about CT stuff (beyond having min num hits to do fit, which the -999 or w/e is responsible for taking care of)
  bool xfail=(n_xpl_tr+n_xpl_bg<m_par->CT_x),uvfail= (n_uvpl_tr+n_uvpl_bg<m_par->CT_uv);
  msg(MSG::DEBUG) << n_xpl_tr+n_xpl_bg<<" x hits");
  if(xfail)candtheta=-999.;
  if(uvfail)candphi=-999.;
  */
  bool fitkill=(ROI.theta==-999 || Delta_Theta==-999||Delta_Theta==-4);// ||xfail||uvfail);
  msg(MSG::DEBUG) << "HIT CODE: "<<track_to_index(track)<< endmsg;
  evFit_entry aemon(event,candtheta,candphi,Delta_Theta_division,ROI.roi,n_xpl_tr,n_uvpl_tr,n_xpl_bg,n_uvpl_bg,Delta_Theta,track_to_index(track));
  if(fitkill) return aemon;
//   msg(MSG::DEBUG) << " aemon fit_theta="<<aemon.fit_theta.getFloat()<<", fit_phi="<<aemon.fit_phi.getFloat()<<"...";
  int nplanes=m_par->setup.size();
  for(int plane=0; plane<nplanes; plane++){
    if(track[plane].info.slope==-999) continue; //&& Delta_Theta_division~=-999
    int hitData_pos=find_hitData(hitDatas,track[plane].key);
    if(hitData_pos==-1) continue;
    did_fit=true;
    hitDatas[hitData_pos].fit_fill(ROI.theta,ROI.phi,Delta_Theta,M_x_global,M_u_global,M_v_global,M_x_local,ROI.m_x,ROI.m_y,ROI.roi);
    aemon.fit_hit_keys.push_back(track[plane].key);
//     msg(MSG::DEBUG) << "hitData fit_theta="<<hitDatas[hitData_pos].fit_theta<<"...";
    if(hitDatas[hitData_pos].truth_nbg) aemon.truth_planes_hit+=pow(10,nplanes-plane-1);
    else aemon.bg_planes_hit+=pow(10,nplanes-plane-1);
  }
  if(did_fit) nfit++;
//   msg(MSG::DEBUG) << "aemon has "<<aemon.fit_hit_keys.size()<<" keys");
//   if(event==2997||event==2800) aemon.print();
  return aemon;
}

int MMT_Fitter::find_hitData(const vector<hitData_entry>& hitDatas, const hitData_key& key) const{
  for(unsigned int i=0;i<hitDatas.size();i++){
    if(hitDatas[i].BC_time==key.BC_time&&hitDatas[i].time==key.time&&hitDatas[i].gtime==key.gtime&&hitDatas[i].VMM_chip==key.VMM_chip) return i;
  }
  return -1;
}


int MMT_Fitter::Filter_UV(vector<Hit>& track) const{
  return 0;
  float32fixed<2> h=m_par->h, tolerance = h;//*2;  //Can be optimized...
  vector<int> u_planes=m_par->q_planes("u"),v_planes=m_par->q_planes("v");
  vector<Hit> u_hits=q_hits("u",track),v_hits=q_hits("v",track);
  bool pass_u=!u_hits.empty(),pass_v=!v_hits.empty();
  //if the difference in slope between the first and last u/v planes is too great don't pass, set track hits to zero
  if(pass_u){
    if((float32fixed<2>(u_hits.front().info.slope-u_hits.back().info.slope)).fabs()>tolerance) pass_u=false;
  }
  if(pass_v){
    if((float32fixed<2>(v_hits.front().info.slope-v_hits.back().info.slope)).fabs()>tolerance) pass_v=false;
  }
  int return_val=0;//of form (bool ubad, bool vbad), so 10 would be bad u, good v
  if(!pass_u){
    for(unsigned int iup=0;iup<u_planes.size();iup++) track[u_planes[iup]].info.slope=-999;
    return_val+=10;
  }
  if(!pass_v){
    for(unsigned int ivp=0;ivp<v_planes.size();ivp++) track[v_planes[ivp]].info.slope=-999;
    return_val+=1;
  }
  return return_val;
}

float32fixed<2> MMT_Fitter::Get_Global_Slope (const vector<Hit>& track, const string& type) const
{
  vector<Hit> qhits = q_hits(type,track);
  float32fixed<2> sum = 0.;
  if(qhits.size()==0) return -999;
  float32fixed<2> nhitdiv= 1./qhits.size();
//   msg(MSG::DEBUG) << "Calculating global slope of type "<<type<<"---adding "<< qhits.size() <<" slopes: ";
  for(int ihit=0;ihit<(int)qhits.size();ihit++){
    sum+=(qhits[ihit].info.slope*nhitdiv);
  }
  return sum;
}

//CHANGE!
float32fixed<2> MMT_Fitter::Get_Local_Slope (const vector<Hit>& Track,double theta,double phi) const
{
  vector<int> x_planes=m_par->q_planes("x"),ybin_hits(x_planes.size(),-1);
  int nxp=x_planes.size();
  for(int ipl=0; ipl<nxp; ipl++){
    ybin_hits[ipl]=((Track[x_planes[ipl]].info.slope==-999||Track[x_planes[ipl]].info.slope==-4)?-1:m_par->ybin(Track[x_planes[ipl]].info.y));
  }
  bool hit=false;
  float32fixed<yzdex> yzsum=0;
  float mxlf=0;
  int xdex=-1;
  int ybin=-1;
  int which=-1;
  m_par->key_to_indices(ybin_hits,xdex,ybin,which);
  if(xdex<0||ybin<0||which<0) return -999;
  float32fixed<zbardex> zbar=m_par->Ak_local_slim[xdex][ybin][which];
  float32fixed<bkdex>bk=m_par->Bk_local_slim[xdex][ybin][which];
  msg(MSG::DEBUG) << "zbar is "<<zbar.getValue()<<", and bk is "<<bk.getValue()<< endmsg;
  int ebin=m_par->eta_bin(theta),pbin=m_par->phi_bin(phi);
  for(int ipl=0; ipl<nxp; ipl++){
    float32fixed<yzdex> z=Track[x_planes[ipl]].info.z,y=Track[x_planes[ipl]].info.y;
    if(ebin!=-1){
      z=z+m_par->zmod[ebin][pbin][ipl];
      y=y+m_par->ymod[ebin][pbin][ipl];
    }
    if(Track[x_planes[ipl]].info.slope==-999||Track[x_planes[ipl]].info.slope==-4) continue;
    hit=true;
    yzsum+=y*(z*zbar-1.);
    mxlf += bk.getFloat()*y.getFloat()*(z.getFloat()*zbar.getFloat()-1.);
  }
  float32fixed<2> mxl=float32fixed<2>(bk.getValue()*yzsum.getValue());
  if(!hit) {return float32fixed<2>(999);}
  // if(log10(abs(mxl.getValue()-mxlf))>-3.&&xdex!=5&&xdex!=10)cout<<setprecision(20)<<"*****AVENGE ME! *******fixed: "<<mxl.getValue()<<", float:"<<mxlf<<",    (ERROR: "<<mxlf-mxl.getValue()<<")");
  return mxl;
}

int MMT_Fitter::track_to_index(const vector<Hit>&track)const{
  vector<bool>hits(m_par->setup.size(),false);
  for(int ihit=0;ihit<(int)track.size();ihit++)hits[track[ihit].info.plane]=(hits[track[ihit].info.plane]?true:track[ihit].info.slope>-2.);
  return m_par->bool_to_index(hits);
}

double MMT_Fitter::ideal_local_slope(const vector<Hit>& Track)const{
  vector<vector<double> > z_hit;
  for(unsigned int i = 0; i<m_par->z_large.size(); i++){
    vector<double> temp;
    for(unsigned int j = 0; j<m_par->z_large[i].size(); j++)
      temp.push_back(m_par->z_large[i][j].getFloat());
    z_hit.push_back(temp);
  }
  vector<int> x_planes=m_par->q_planes("x");
  int nxp=x_planes.size();
  bool hit=false;
  double sum_xy=0,sum_y=0;
  double ak_idl=ideal_ak(Track),bk_idl=ak_idl*ideal_zbar(Track);
  for(int ipl=0; ipl<nxp; ipl++){
    double y=Track[x_planes[ipl]].info.y.getFloat(),z=Track[x_planes[ipl]].info.z.getFloat();
    if(y==-999) continue;
    hit=true;
    sum_xy += ak_idl*z*y;
    sum_y  += bk_idl*y;
//     msg(MSG::DEBUG) << "...z="<<z<<",y="<<y<<",sum_y="<<sum_y<<",sum_xy="<<sum_xy;
  }
  if(!hit) return -10.;
  double ls_idl=sum_xy-sum_y;
//   msg(MSG::DEBUG) << endl<<"ak_idl="<<ak_idl<<",bk_idl="<<bk_idl<<",sum_xy"<<sum_xy<<",sum_y="<<sum_y<<",ls_idl="<<ls_idl);
  return ls_idl;
}

double MMT_Fitter::ideal_z(const Hit& hit)const{
  int plane=hit.info.plane;
  double tilt=(plane<4?m_par->correct.rotate.X():0),dz=(plane<4?m_par->correct.translate.Z():0),
    nominal=m_par->z_nominal[plane].getFloat(),y=hit.info.y.getFloat()-m_par->ybases[plane].front().getFloat(),z=nominal+dz+y*tan(tilt);
  return z;
}

double MMT_Fitter::ideal_ak(const vector<Hit>& Track)const{
  vector<int> x_planes=m_par->q_planes("x");//this tells us which planes are x planes
  int n_xplanes=x_planes.size(),hits=0;
  double sum_x=0,sum_xx=0;
  for(int ip=0; ip<n_xplanes; ip++){
    if(Track[x_planes[ip]].info.slope==-999) continue;
    double addme=ideal_z(Track[x_planes[ip]]);//z_hit[x_planes[plane]];
    hits++;//there's a hit
    sum_x  += addme;
    sum_xx += addme*addme;
    msg(MSG::DEBUG) << "z["<<ip<<"]="<<addme<<", sum_x="<<sum_x<<", sum_xx="<<sum_xx<<", hits="<<hits<< endmsg;
  }
  double diff = hits*sum_xx-sum_x*sum_x;
  return hits/diff;
}

double MMT_Fitter::ideal_zbar(const vector<Hit>& Track)const{
  vector<int> x_planes=m_par->q_planes("x");//this tells us which planes are x planes
  double ztot=0;int nhit=0;
  for(unsigned int ip=0;ip<x_planes.size();ip++){
    if(Track[x_planes[ip]].info.slope==-999)continue;
    nhit++;ztot+=ideal_z(Track[x_planes[ip]]);
  }
  return ztot/nhit;
}

float32fixed<2> MMT_Fitter::Get_Delta_Theta(float32fixed<2> M_local, float32fixed<2> M_global) const{
  int region=-1;
  // if(div_hack)return Get_Delta_Theta_division(M_local,M_global);
  float32fixed<2> LG = M_local * M_global;
  for(int j=0;j<number_LG_regions;j++){   //number_LG_regions
    if(LG <= DT_Factors_val(j,0)){
      region = j;
      break;
    }
  }
  if(region==-1) return -999;
  return DT_Factors_val(region,1)*(M_local - M_global);
}

float32fixed<2> MMT_Fitter::DT_Factors_val(int i, int j) const{
  if(m_par->val_tbl){
    return m_par->DT_Factors[i][j];
  }
  if(j<0||j>1){
    ATH_MSG_WARNING("DT_Factors only has two entries on the second index (for LG and mult_factor); you inputed an index of " << j );
    exit(1);
  }
  if(i<0||i>=number_LG_regions){
    ATH_MSG_WARNING("There are " << number_LG_regions << " in DT_Factors(_val); you inputed an index of " << i );
    exit(1);
  }
  double a=1.;//not sure what this is for, so hard to choose fixed_point algebra
  if(j==0) return mult_factor_lgr(i,a,number_LG_regions,LG_min,LG_max);
  return LG_lgr(i,a,number_LG_regions,LG_min,LG_max);
}

float32fixed<2> MMT_Fitter::LG_lgr(int ilgr, double a, int number_LG_regions, float32fixed<2> _min, float32fixed<2> _max) const{
  a+=0;
  return _min+float32fixed<2>(ilgr/number_LG_regions)*(_max-_min);
}

float32fixed<2> MMT_Fitter::mult_factor_lgr(int ilgr, double a, int number_LG_regions, float32fixed<2> _min, float32fixed<2> _max) const{
  return float32fixed<2>(1./(a+LG_lgr(ilgr,a,number_LG_regions,_min,_max)/a));
}

float32fixed<2> MMT_Fitter::Get_Delta_Theta_division(float32fixed<2> M_local, float32fixed<2> M_global, float32fixed<4> a) const{
//delta_theta = theta_local_slope - theta_global_fit
// a=1;  // Really sin(phi), but I use small angles about phi=pi/2


  //we could use 2 bits for the numerator and 3 for the denominator, but then
  //fixed_point doesn't know how to do the algebra. Assume we know how to do
  //this division (we don't, efficiently, thus the method Get_Delta_Theta
  return float32fixed<2>((M_local - M_global)/(a + (M_local*M_global)/a.getFloat()));
}

vector<Hit> MMT_Fitter::q_hits(const string& type,const vector<Hit>& track) const{
  string setup(m_par->setup);
  if(setup.length()!=track.size()){
    ATH_MSG_WARNING("Setup has length: "<<setup.length()<<", but there are "<<track.size()<<" hits in the track");
    exit(2);
  }
  vector<int> qpl(m_par->q_planes(type));
  vector<Hit> q_hits;
  for(unsigned int ihit=0; ihit<qpl.size(); ihit++){
    if(track[qpl[ihit]].info.slope!=-999) {q_hits.push_back(track[qpl[ihit]]);
    }
  }
  return q_hits;
}

//change this to take u and/or v out of the roi calculation
ROI MMT_Fitter::Get_ROI(float32fixed<2> M_x,float32fixed<2> M_u,float32fixed<2> M_v,const vector<Hit>&track) const{
  //M_* are all global slopes
  msg(MSG::DEBUG) << "\nGet_ROI("<<M_x.getValue()<<","<<M_u.getValue()<<","<<M_v.getValue()<<") "<< endmsg;

  //--- calc constants ------
  float32fixed<2> b=TMath::DegToRad()*(m_par->stereo_degree.getFloat());
  float32fixed<7> A=1./tan(b.getFloat()),B=1./tan(b.getFloat());

  //---  slope conversion equations ----
  float32fixed<2> m_y = M_x;
  float32fixed<2> m_xu = A*M_u.getFloat() - B*m_y.getFloat(),m_xv = B*m_y.getFloat() - A*M_v.getFloat();
  //--- which slopes are truly present ----
  //Note that bad slopes are not necessarily 0 as I often use -999 to denote something missing
  //we have -999 for M_u or M_v to denote that it didn't pass filtering
  int nu=1,nv=1;
  if(M_u<0||M_u==float32fixed<2>(-999)){
    m_xu = 0;nu=0;
  }
  if(M_v<0||M_v==float32fixed<2>(-999)){
    m_xv=0;nv=0;
  }
  if(nu==0&&nv==0) return ROI(-999,-999,-999,-999,-999);

  //--- average of 2 mx slope values ----if both u and v were bad, give it a -999 value to know not to use m_x
  //*** check to see if U and V are necessary for fit
  float32fixed<2> m_x = (nu+nv==0?0:(m_xv+m_xu)/(nu+nv));
  if(m_par->correct.translate.X()!=0&&m_par->correct.type==2){
    m_x+=phi_correct_factor(track)*m_par->correct.translate.X()/m_par->z_nominal[3].getFloat();
  }
  //if(debug)
    msg(MSG::DEBUG) << "(b,A,B,my,mxu,mxv,mx)=("<<b.getFloat()<<","<<A.getFloat()<<","<<B.getFloat()<<","<<m_y.getFloat()<<","<<m_xu.getFloat()<<","<<m_xv.getFloat()<<","<<m_x.getValue()<<")\n"<< endmsg;
  //mfits.push_back(pair<double,double>(m_x.getFloat(),m_y.getFloat()));

  //Get m_x and m_y in parameterized values
  int a_x = round((m_x.getValue()-m_par->m_x_min.getValue())/m_par->h_mx.getValue()), a_y = round((m_y.getValue()-m_par->m_y_min.getValue())/m_par->h_my.getValue());
  // Generally, this offers a reality check or cut.  The only reason a slope
  // should be "out of bounds" is because it represents a weird UV combination
  // -- ie. highly background influenced
  if(a_y>m_par->n_y || a_y<0){
    msg(MSG::DEBUG) << "y slope (theta) out of bounds in Get_ROI....(a_x,a_y,m_par->n_x,m_par->n_y)=("<<a_x<<","<<a_y<<","<<m_par->n_x<<","<<m_par->n_y<<")"<< endmsg;
    return ROI(-999,-999,-999,-999,-999);
  }

  if(a_x>m_par->n_x || a_x<0){
    msg(MSG::DEBUG) << "x slope (phi) out of bounds in Get_ROI....(a_x,a_y,m_par->n_x,m_par->n_y)=("<<a_x<<","<<a_y<<","<<m_par->n_x<<","<<m_par->n_y<<")"<< endmsg;
    return ROI(-999,-999,-999,-999,-999);
  }

//   xent.push_back(a_x);yent.push_back(a_y);
  msg(MSG::DEBUG) << "fv_angles...(a_x,a_y)=("<<a_x<<","<<a_y<<")"<< endmsg;
  double phicor=0.;
  if(m_par->correct.rotate.Z()!=0&&m_par->correct.type==2){
//     phicor=-1.*phi_correct_factor(track)*m_par->correct.rotate.Z();
    phicor=-0.2*m_par->correct.rotate.Z();
  }

  float32fixed<4> fv_theta=Slope_Components_ROI_theta(a_y,a_x), fv_phi=(m_x.getValue()==0?-999:Slope_Components_ROI_phi(a_y,a_x).getFloat()+phicor);
  msg(MSG::DEBUG) << "fv_theta="<<fv_theta.getValue()<<", fv_phi="<<fv_phi.getValue()<< endmsg;

  //--- More hardware realistic approach but need fine tuning ----
  int roi = Rough_ROI_temp(fv_theta,fv_phi);

  //--- current "roi" which is not an actual roi but an approx phi and theta
  return ROI(fv_theta.getValue(),fv_phi.getValue(),m_x.getValue(),m_y.getValue(),roi);
}

double MMT_Fitter::phi_correct_factor(const vector<Hit>&track)const{
  if((m_par->correct.rotate.Z()==0&&m_par->correct.translate.X()==0)||m_par->correct.type!=2)return 0.;
  int nxmis=0,nx=0,numis=0,nu=0,nvmis=0,nv=0;
  double xpart=0.5,upart=0.5,vpart=0.5;
  string set=m_par->setup;
  for(int ihit=0;ihit<(int)track.size();ihit++){
    int n_pln=track[ihit].info.plane;
    bool ismis=n_pln<4;
    char pln=set[n_pln];
    if(pln=='x'||pln=='X'){nx++;nxmis+=ismis;}
    else if(pln=='u'||pln=='U'){nu++;numis+=ismis;}
    else if(pln=='v'||pln=='V'){nv++;nvmis+=ismis;}
  }
  if(nu==0&&nv==0)return 0.;
  if(nu==0)upart=0.;
  else if(nv==0)vpart=0.;
  else xpart=0.;
  return xpart*1.*nxmis/nx+upart*1.*numis/nu+vpart*1.*nvmis/nv;
}

float32fixed<4> MMT_Fitter::Slope_Components_ROI_val(int jy, int ix, int thetaphi) const{
  if(m_par->val_tbl){
    return m_par->Slope_to_ROI[jy][ix][thetaphi];
  }
  if(thetaphi<0||thetaphi>1){
    ATH_MSG_WARNING("Slope_Components_ROI only has two entries on the third index (for theta and phi); you inputed an index of " << thetaphi);
    exit(2);
  }
  if(thetaphi==0) return Slope_Components_ROI_theta(jy,ix);
  return Slope_Components_ROI_phi(jy,ix);
}

float32fixed<4> MMT_Fitter::Slope_Components_ROI_theta(int jy, int ix) const{
  //get some parameter information
  if(jy<0||jy>=m_par->n_y){
    ATH_MSG_WARNING("You picked a y slope road index of " << jy << " in Slope_Components_ROI_theta; there are only " << m_par->n_y << " of these.\n");
    if(jy>=m_par->n_y)jy=m_par->n_y-1;
    else jy=0;
//     exit(2);
  }
  if(ix<0||ix>=m_par->n_x){
    ATH_MSG_WARNING("You picked an x slope road index of " << ix << " in Slope_Components_ROI_theta; there are only " << m_par->n_x << " of these.\n");
    if(ix>=m_par->n_x)ix=m_par->n_x-1;
    else ix=0;
//     exit(2);
  }
  int xdex=ix,ydex=jy+1;
  if(xdex==0)xdex++;
  float32fixed<2> m_x=m_par->m_x_min+m_par->h_mx*xdex, m_y=m_par->m_y_min+m_par->h_my*ydex;
  float32fixed<4> theta=atan(sqrt(m_x*m_x+m_y*m_y));
//   cout<<"in slope componets roi theta, theta must be in ["<<m_par->minimum_large_theta<<","<<m_par->maximum_large_theta<<"]");
  if(theta<m_par->minimum_large_theta || theta>m_par->maximum_large_theta){
//     cout << "Our theta of "<<theta<<" is not in ["<<m_par->minimum_large_theta<<","<<m_par->maximum_large_theta<<"]");
    theta=0;
  }
  return theta;
}

float32fixed<4> MMT_Fitter::Slope_Components_ROI_phi(int jy, int ix) const{
  if(jy<0||jy>=m_par->n_y){
    ATH_MSG_WARNING("You picked a y slope road index of " << jy << " in Slope_Components_ROI_phi; there are only " << m_par->n_y << " of these.\n");
    if(jy>=m_par->n_y)jy=m_par->n_y-1;
    else jy=0;
//    exit(2);
  }
  if(ix<0||ix>=m_par->n_x){
    ATH_MSG_WARNING("You picked an x slope road index of " << ix << " in Slope_Components_ROI_phi; there are only " << m_par->n_x << " of these.\n");
    //right now we're assuming these are cases just on the edges and so put the values to the okay limits
    if(ix>=m_par->n_x)ix=m_par->n_x-1;
    else ix=0;
//     exit(2);
  }
  int xdex=ix,ydex=jy+1;
  float32fixed<2> m_x=m_par->m_x_min+m_par->h_mx*xdex, m_y=m_par->m_y_min+m_par->h_my*ydex;
  //if(debug)
    msg(MSG::DEBUG) << "m_par->m_x_min+m_par->h_mx*xdex="<<m_par->m_x_min.getValue()<<"+"<<m_par->h_mx.getValue()<<"*"<<xdex<<"="<<m_x.getValue()<<", "<< endmsg;
  //if(debug)
    msg(MSG::DEBUG) << "m_par->m_y_min+m_par->h_my*ydex="<<m_par->m_y_min.getValue()<<"+"<<m_par->h_my.getValue()<<"*"<<ydex<<"="<<m_y.getValue()<<", "<< endmsg;
  float32fixed<4> phi(atan2(m_x.getValue(),m_y.getValue()));//the definition is flipped from what you'd normally think
  msg(MSG::DEBUG) << "for a phi of "<<phi.getValue()<< endmsg;
  if(phi<m_par->minimum_large_phi || phi>m_par->maximum_large_phi){
    msg(MSG::DEBUG) << "Chucking phi of " << phi.getValue()<<" which registers as not in ["<<m_par->minimum_large_phi.getValue()<<","<<m_par->maximum_large_phi.getValue()<<"]"<< endmsg;
    phi=999;
  }
  return phi;
}

int MMT_Fitter::Rough_ROI_temp(float32fixed<4> theta, float32fixed<4> phi) const{
  //temporary function to identify areas of the wedge.
  float32fixed<4> minimum_large_theta=m_par->minimum_large_theta, maximum_large_theta=m_par->maximum_large_theta,minimum_large_phi=m_par->minimum_large_phi, maximum_large_phi=m_par->maximum_large_phi;
  int n_theta_rois=32, n_phi_rois=16;//*** ASK BLC WHAT THESE VALUES OUGHT TO BE!

  float32fixed<4> h_theta = (maximum_large_theta - minimum_large_theta)/n_theta_rois;
  float32fixed<4> h_phi = (maximum_large_phi - minimum_large_phi)/n_phi_rois;
  //how is this done in the FPGA? No such division, for sure
  double roi_t = ceil((theta - minimum_large_theta)/h_theta.getFloat());
  double roi_p = ceil((phi - minimum_large_phi)/h_phi.getFloat());

  if(theta<minimum_large_theta || theta>maximum_large_theta) roi_t = 0;
  if(phi<minimum_large_phi || phi>maximum_large_phi) roi_p = 0;
  int ret_val=roi_t * 1000 + roi_p;
  /*
  cout<<"ret_val:"<<ret_val);
  if(ret_val<1){
    cout << "ROI theta of "<<theta<<" is not in ["<<m_par->minimum_large_theta<<","<<m_par->maximum_large_theta<<"]");
    cout << "ROI phi of "<<phi<<" is not in ["<<m_par->minimum_large_phi<<","<<m_par->maximum_large_phi<<"]");
  }
  */
  return ret_val;
}

