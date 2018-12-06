/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/RpcPatFinder.h"

#include <math.h>
#include <bitset>
#include <iostream>

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// Original author: Massimo Corradi

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_RpcPatFinder("IID_RpcPatFinder", 1, 0);

const InterfaceID& TrigL2MuonSA::RpcPatFinder::interfaceID() { return IID_RpcPatFinder; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcPatFinder::RpcPatFinder(const std::string& type,
					 const std::string& name,
					 const IInterface*  parent):
  AthAlgTool(type, name, parent)  
{  
  declareInterface<TrigL2MuonSA::RpcPatFinder>(this);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcPatFinder::~RpcPatFinder()
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcPatFinder::initialize()
{
  ATH_MSG_DEBUG("Initializing RpcPatFinder - package version " << PACKAGE_VERSION) ;
   
  StatusCode sc;
  sc = AthAlgTool::initialize();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
    return sc;
  }

  // 
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcPatFinder::clear() {
  std::list<double> z;
  z.clear();
  m_hits_in_layer_eta.assign(8,z);
  m_hits_in_layer_phi.assign(8,z);
  std::vector<double> zz;
  zz.clear();
  m_hits_in_layer_R.assign(8,zz);
  m_hits_in_layer_Z.assign(8,zz);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcPatFinder::addHit(std::string stationName,
					int stationEta,
					bool  measuresPhi,
					unsigned  int gasGap,
					unsigned  int doubletR,
					double gPosX,
					double gPosY,
					double gPosZ ){
  
  int ilay=0;
  // BO 
  if (stationName.substr(0,2)=="BO") ilay=4;
  // doubletR
  ilay+=2*(doubletR-1);
  // BML7 special chamber with 1 RPC doublet (doubletR=1 but RPC2) :
  if (stationName.substr(0,3)=="BML" && stationEta==7) ilay+=2;
  // gasGap
  ilay+=gasGap-1;

  double R=sqrt(gPosX*gPosX+gPosY*gPosY);
  double Phi=atan2(gPosY,gPosX);

  if (!measuresPhi){
    // if eta measurement then save Z/R
    R = calibR(stationName,R, Phi);  
    double x=gPosZ/R;
    m_hits_in_layer_eta.at(ilay).push_back(x);
    m_hits_in_layer_R.at(ilay).push_back(R);//mod!
    m_hits_in_layer_Z.at(ilay).push_back(gPosZ);//mod!
   }else{
    // if phi measurement then save phi
    m_hits_in_layer_phi.at(ilay).push_back(Phi);
  }
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
bool TrigL2MuonSA::RpcPatFinder::findPatternPhi(double &phi_middle, double &phi_outer,   unsigned int &pattern){
  double result_dMO;
  bool found=false;
  if (patfinder(true, pattern, phi_middle,phi_outer, result_dMO)>=2) found=true;
  return found;
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonSA::RpcPatFinder::findPatternEta(double aw[], double bw[], unsigned int & pattern){
  double result_delta[3]={9999,9999,9999};
  bool found=false;
  if( patfinder_forEta(false, pattern,  aw, bw, result_delta)>=2) found=true;
  return found;
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

int  TrigL2MuonSA::RpcPatFinder::patfinder_forEta(bool iphi,
			     unsigned int &result_pat,
			     double result_aw[],
			     double result_bw[],
			     double result_dist[]){
  
  std::vector<std::list<double>> *  rpc_x;
  rpc_x = &m_hits_in_layer_eta;
  
  int  layer_end;
  if(rpc_x->at(6).size()+rpc_x->at(7).size() >0) layer_end = 7;//special "feet" towers
  else                                           layer_end = 5;
  
  // reset parameters
  result_pat=0;

  int n_max=0;
  size_t index[8]={};
  size_t result_index[8]={};

  // Loop on start layer
  for (int l_start=0; l_start<layer_end; l_start++){
    for(int i=0; i<8; i++) index[i]=0;

    // Loop on hits of start layer, for each hit try a new pattern
    for (std::list<double>::iterator i_start=rpc_x->at(l_start).begin(); i_start!=rpc_x->at(l_start).end(); i_start++){
      int n_hits=1;
      unsigned int pat=(1<<l_start); // bit pattern of hit layers
      double dMO=9999; // disstance middle-outer station
      double dMM=9999; // distance RPC1-RPC2 on middle stations
      double current_x =*i_start; // set current_x to the starting hit
      int l_current = l_start;

      index[0] = std::distance(rpc_x->at(l_start).begin(), i_start);//mod!
  
      ATH_MSG_DEBUG("patfinder: l_start = "<< l_start << " x= " << current_x
		    << " pat= "    << (std::bitset<8>) pat); 

      // ----- add compatible hits in other layers ----//
      // loop on test layers:
      for (int l_test=l_start+1; l_test<=layer_end; l_test++){
	int n_layer=0;
	double x_layer=0;
	double delta_layer=999;
	//  loop on hits of test layer and picks the one with smaller distance from current_x
	for (std::list<double>::iterator i_test=rpc_x->at(l_test).begin(); i_test!=rpc_x->at(l_test).end(); i_test++){ 
	  double delta=-1;
	  // check if within the road
	  if (deltaOK(l_current,l_test,current_x,*i_test,iphi,delta)){
	    n_layer++;
	    // if closest hit update x_layer
	    if (delta<delta_layer) {
	      delta_layer=delta;
	      x_layer=*i_test;	      
	      index[l_test]       = std::distance(rpc_x->at(l_test).begin(), i_test);//mod!
	    }
	  }
	}//for i_test
	if (n_layer>0) {// compatible hit found in this layer increase n_hits
	  n_hits+=1;
	  current_x=x_layer;
	  pat+=(1<<l_test);
	  l_current=l_test;
	  if (l_start<4&&l_test>=4&&delta_layer<dMO){
	    dMO=delta_layer;
	  }else if (l_start<2&&l_test>=2&&l_test<4&&delta_layer<dMM) {
	    dMM=delta_layer;
	  } else  if (l_start>=4&&l_start<5&&l_test>=6&&delta_layer<dMM) {
	    dMM=delta_layer;
	  }
	}// if (n_layer)
	
	ATH_MSG_DEBUG("patfinder:  l_test = "<< l_test << " n_layer= "<< n_layer 
		      << " x= " << current_x << " pat= " << (std::bitset<8>)pat);
      }//for l_test

      // if longest pattern found, update result
      if (n_hits>n_max) { 
	n_max=n_hits;
	result_pat=pat;
	result_dist[2]=dMO; 
	result_dist[1]=dMM; 
	for(int i=0; i<8; i++) result_index[i]=index[i];
      }else if (n_hits==n_max) { 
	// if same lenght but smallest dMM/dMO, update result
	if (dMM<result_dist[1]||(dMM==result_dist[1]&&dMO<result_dist[2])){
	  result_pat=pat;
	  result_dist[2]=dMO;
	  result_dist[1]=dMM;
	  for(int i=0; i<8; i++) result_index[i]=index[i];
	}
      }
    }//for i_start
  }//for l_start

  if (n_max>=2) {
    abcal(result_pat, result_index, result_aw, result_bw);
    ATH_MSG_DEBUG("patfinder: BEST pat= " << (std::bitset<8>)result_pat
		  <<"  dMM= "<<result_dist[1] <<"  dMO= "<<result_dist[2]);
  
  }//if(n_max>2)
  
  return n_max;
}//patfinder_forEta()

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

int  TrigL2MuonSA::RpcPatFinder::patfinder(bool iphi,
			     unsigned int &result_pat,
			     double &result_x,
			     double &result_x1,
			     double &result_dMO){
  
  const int N_layers=8;

  std::vector<std::list<double>> *  rpc_x;
  if (iphi){
    rpc_x = &m_hits_in_layer_phi;
  } else {
    rpc_x = &m_hits_in_layer_eta;
  }
  
  int l_start_max=2; //max layer of first hit
  if (rpc_x->at(6).size()+rpc_x->at(7).size()>0) l_start_max=5; // special "feet" towers

  // reset parameters
  result_x=0;
  result_x1=0;
  result_pat=0;
  double result_dMM=9999;
  result_dMO=9999;
  int n_max=0;


  // Loop on start layer
  for (int l_start=0; l_start<=l_start_max; l_start++){
    // Loop on hits of start layer, for each hit try a new pattern
    for (std::list<double>::iterator i_start=rpc_x->at(l_start).begin(); i_start!=rpc_x->at(l_start).end(); i_start++){
      int n_hits=1;
      unsigned int pat=(1<<l_start); // bit pattern of hit layers
      double dMO=9999; // disstance middle-outer station
      double dMM=9999; // distance RPC1-RPC2 on middle stations

      double current_x =*i_start; // set current_x to the starting hit
      int l_current = l_start;
      ATH_MSG_DEBUG("patfinder: l_start = "<< l_start << " x= " << current_x
		    << " pat= "    << (std::bitset<8>) pat); 

      // ----- add compatible hits in other layers ----//
      // loop on test layers:
      for (int l_test=l_start+1; l_test<N_layers; l_test++){
	int n_layer=0;
	double x_layer=0;
	double delta_layer=999;
	//  loop on hits of test layer and picks the one with smaller distance from current_x
	for (std::list<double>::iterator i_test=rpc_x->at(l_test).begin(); i_test!=rpc_x->at(l_test).end(); i_test++){ 
	  double delta=-1;
	  // check if within the road
	  if (deltaOK(l_current,l_test,current_x,*i_test,iphi,delta)){
	    n_layer++;
	    // if closest hit update x_layer
	    if (delta<delta_layer) {
	      delta_layer=delta;
	      x_layer=*i_test;	      
	    }
	  }
	}
	if (n_layer>0) {// compatible hit found in this layer increase n_hits
	  n_hits+=1;
	  current_x=x_layer;
	  pat+=(1<<l_test);
	  l_current=l_test;
	  if (l_start<4&&l_test>=4&&delta_layer<dMO){
	    dMO=delta_layer;
	  }else if (l_start<2&&l_test>=2&&l_test<4&&delta_layer<dMM) {
	    dMM=delta_layer;
	  } else  if (l_start>=4&&l_start<5&&l_test>=6&&delta_layer<dMM) {
	    dMM=delta_layer;
	  }
	}
	ATH_MSG_DEBUG("patfinder:  l_test = "<< l_test << " n_layer= "<< n_layer 
		      << " x= " << current_x << " pat= " << (std::bitset<8>)pat);
      }
      // if longest pattern found, update result
      if (n_hits>n_max) { 
	n_max=n_hits;
	result_x=*i_start;
	result_pat=pat;
	result_dMO=dMO; 
	result_dMM=dMM; 
	result_x1=current_x;
      }else if (n_hits==n_max) { 
	// if same lenght but smallest dMM/dMO, update result
	if (dMM<result_dMM||(dMM==result_dMM&&dMO<result_dMO)){
	  result_x=*i_start;
	  result_pat=pat;
	  result_dMO=dMO;
	  result_dMM=dMM;
	  result_x1=current_x;
	}
      }
    }
  }
  if (n_max>2) {
    ATH_MSG_DEBUG("patfinder: BEST pat= " << (std::bitset<8>)result_pat
		  <<"  dMM= "<<result_dMM <<"  dMO= "<<result_dMO 
		  <<"  x0= "<<result_x <<"  x1= "<<result_x1);
  }
  return n_max;
  
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool  TrigL2MuonSA::RpcPatFinder::deltaOK(int l1, int l2, double x1, double x2, int isphi, double &delta){
  
  
  // ROAD tuned for ~20 GeV 
  /*
  double delta_gasgap_eta = 0.004;
  double delta_lowpt_eta = 0.005;
  double delta_highpt_eta = 0.012;
  double delta_feet_eta = 0.02;

  double delta_gasgap_phi = 0.004;
  double delta_lowpt_phi = 0.005;
  double delta_highpt_phi = 0.008;
  double delta_feet_phi = 0.02;
  */

  //OPEN road 

  const double delta_gasgap_eta = 0.01;
  const double delta_lowpt_eta = 0.05;
  const double delta_highpt_eta = 0.1;
  const double delta_feet_eta = 0.05;

  const double delta_gasgap_phi = 0.01;
  const double delta_lowpt_phi = 0.03;
  const double delta_highpt_phi = 0.04;
  const double delta_feet_phi = 0.03;

  // calculate delta-eta or delta-phi 
  if(isphi) delta=fabs(acos(cos(x2-x1)));
  else delta=fabs(x2-x1); 

  double delta_max=0;
  if (l1>l2) {
    int tmp=l2;
    l2=l1;
    l1=tmp;
  }
  // calculate delta_max
  if (isphi){
    if (l2-l1==1&&(l1==0||l1==2||l1==4||l1==6)){
      delta_max=delta_gasgap_phi;
    } else if (l1<2&&l2<4) {
      delta_max=delta_lowpt_phi;
    }else if (l1<4&&l2>=4) {
      delta_max=delta_highpt_phi;
    }else if (l1<6&&l1>=4&&l2>=6) {
      delta_max=delta_feet_phi;
    }
  } else {
    if (l2-l1==1&&(l1==0||l1==2||l1==4||l1==6)){
      delta_max=delta_gasgap_eta;
    } else if (l1<2&&l2>=2&&l2<4) {
      delta_max=delta_lowpt_eta;
    }else if (l1<4&&l2>=4) {
      delta_max=delta_highpt_eta;
    }else if (l1<6&&l1>=4&&l2>=6) {
      delta_max=delta_feet_eta;
    }
  }

  // evaluate the result

  bool OK=false;
  if (delta<delta_max) OK=true;

  return OK;
  
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::RpcPatFinder::calibR(std::string stationName, double R, double Phi){
  double DeltaPhi, temp_phi;
  double calibPhi = acos(cos(Phi)); // 0 < Phi < 2PI
  
  if(std::string::npos != stationName.rfind("L")){//For Large , SP
    DeltaPhi= 999; temp_phi=9999;
    for(int inum=0;inum < 8;inum++){
      temp_phi = fabs((inum * M_PI/4.0 )- calibPhi);
      if(temp_phi < DeltaPhi)      DeltaPhi = temp_phi;
    }
  }else if(std::string::npos != stationName.rfind("S") ||
	   std::string::npos != stationName.rfind("F") ||
	   std::string::npos != stationName.rfind("G")   ){
    DeltaPhi= 999; temp_phi=9999;

    for(int inum=0;inum < 8;inum++){
      temp_phi = fabs(inum *(M_PI/4.0 )+(M_PI/8.0) - calibPhi);
      if(temp_phi < DeltaPhi)      DeltaPhi = temp_phi;
    }//for end                                                                                                                      
  }else return R;

  double calibR = R *cos(DeltaPhi);
  
  return calibR;
}//calbR()

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcPatFinder::abcal(unsigned int result_pat, size_t index[], double aw[], double bw[]){
  const float ZERO_LIMIT = 1.e-5;

  std::vector<std::vector<double> > * rpc_R;
  std::vector<std::vector<double> > * rpc_Z;
  rpc_R = &m_hits_in_layer_R;
  rpc_Z = &m_hits_in_layer_Z;
  double R[8]={0,0,0,0,0,0,0,0};
  double Z[8]={0,0,0,0,0,0,0,0};
  unsigned int bit=1;

  int hot_min[3]={999,999,999};
  int hot_max[3]={-999,-999,-999};
  
  int inn_counter=0;
  int out_counter=0;

  for(int i=0; i<8; i++){
    if(i != 0) bit = bit << 1;
    if((result_pat & bit)==false) continue;
    R[i] = rpc_R->at(i).at(index[i]);
    Z[i] = rpc_Z->at(i).at(index[i]);
    
    if(i < hot_min[0])          hot_min[0] = i;
    if(i < hot_min[1])          hot_min[1] = i;
    if(1 < i && out_counter <1) hot_min[2] = i;
    
    if(i < 4){
      hot_max[0] = i;
      hot_max[1] = i;
    }
    if(hot_max[2] < i )         hot_max[2] = i;

    inn_counter++;
    if(1 < i) out_counter++;
  }//for i 
 
  unsigned int inn_bit;
  inn_bit=0x3;//00000011
  if((result_pat & inn_bit)==inn_bit){
    R[hot_min[0]] = (R[0]+R[1])/2.0;
    Z[hot_min[0]] = (Z[0]+Z[1])/2.0;
  }
  inn_bit=0xC;//00001100
  if((result_pat & inn_bit)==inn_bit){
    R[hot_max[0]] = (R[2]+R[3])/2.0;
    Z[hot_max[0]] = (Z[2]+Z[3])/2.0;
  }
    
  unsigned int mid_bit;
  mid_bit=0x3;//00000011
  if((result_pat & mid_bit)==mid_bit){
    R[hot_min[1]] = (R[0]+R[1])/2.0;
    Z[hot_min[1]] = (Z[0]+Z[1])/2.0;
  }
  mid_bit=0xC;//00001100
  if((result_pat & mid_bit)==mid_bit){
    R[hot_max[1]] = (R[2]+R[3])/2.0;
    Z[hot_max[1]] = (Z[2]+Z[3])/2.0;
  }
  
  unsigned int out_bit;
  out_bit=0xC;//00001100
  if((result_pat & out_bit)==out_bit){
    R[hot_min[2]] = (R[2]+R[3])/2.0;
    Z[hot_min[2]] = (Z[2]+Z[3])/2.0;
  }
  
  out_bit=0x30;//00110000
  if((result_pat & out_bit)==out_bit){
    R[hot_max[2]] = (R[4]+R[5])/2.0;
    Z[hot_max[2]] = (Z[4]+Z[5])/2.0;
  }

  out_bit=0xC0;//11000000
  if((result_pat & out_bit)==out_bit){
    R[hot_max[2]] = (R[6]+R[7])/2.0;
    Z[hot_max[2]] = (Z[6]+Z[7])/2.0;
  }

  inn_bit=0xF;//00001111
  double theta_m,theta_t, theta_f;
  if((result_pat & inn_bit)==inn_bit){
    theta_m = atan2(R[hot_min[0]],Z[hot_min[0]]);
    theta_t = atan2(R[hot_max[0]]-R[hot_min[0]],Z[hot_max[0]]-Z[hot_min[0]]);
    theta_f = (theta_m+theta_t)/2.0;
      
    aw[0] = tan(theta_f);
    bw[0] = R[hot_min[0]] - Z[hot_min[0]]*aw[0];
    aw[0] = 1.0/aw[0];
  }else{
    if(hot_min[0]!=999){
      aw[0] = Z[hot_min[0]] / R[hot_min[0]];
      bw[0] = 0.0;
    }else{
      aw[0] = Z[hot_max[0]] / R[hot_max[0]];
      bw[0] = 0.0;
    }//else
  }//else 

  for(int i=1;i<3;i++){
    if(hot_max[i]!=-999 && hot_min[i]!=999){
      if(fabs(Z[hot_max[i]] - Z[hot_min[i]]) > ZERO_LIMIT) {
        aw[i] = (R[hot_max[i]]- R[hot_min[i]]) / (Z[hot_max[i]]-Z[hot_min[i]]);
        bw[i] = R[hot_max[i]] - Z[hot_max[i]]*aw[i];
        aw[i] = 1.0/aw[i];
      }else if(i<2){
        aw[i] = Z[hot_min[i]] / R[hot_min[i]];
        bw[i] = 0.0;
      } else{
        aw[i] = Z[hot_max[i]] / R[hot_max[i]];
        bw[i] = 0.0;
      }
    }else{
      if(i <2){
        if(hot_min[i]!=999){
          aw[i] = Z[hot_min[i]] / R[hot_min[i]];
          bw[i] = 0.0;
        }else if(hot_max[i]!=-999){
          aw[i] = Z[hot_max[i]] / R[hot_max[i]];
          bw[i] = 0.0;
        }
      }else{
        if(hot_max[i]!=-999){
          aw[i] = Z[hot_max[i]] / R[hot_max[i]];
          bw[i] = 0.0;
        }else if(hot_min[i]!=999){
          aw[i] = Z[hot_min[i]] / R[hot_min[i]];
          bw[i] = 0.0;
        }
      }    
    }
  }
}//abcal()

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::RpcPatFinder::finalize()
{
  ATH_MSG_DEBUG("Finalizing RpcPatFinder - package version " << PACKAGE_VERSION);
   
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
