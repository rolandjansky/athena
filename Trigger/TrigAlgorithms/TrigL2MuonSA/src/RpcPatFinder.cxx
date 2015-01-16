/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/RpcPatFinder.h"

#include <math.h>
#include <bitset>
#include <iostream>

// Original author: Massimo Corradi

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcPatFinder::RpcPatFinder(MsgStream* msg) 
  : m_msg(msg)
{  
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::RpcPatFinder::~RpcPatFinder()
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcPatFinder::clear() {
  std::list<double> z;
  z.clear();
  m_hits_in_layer_eta.assign(8,z);
  m_hits_in_layer_phi.assign(8,z);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::RpcPatFinder::addHit(std::string stationName,
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
  // gasGap
  ilay+=gasGap-1;
  
  if (!measuresPhi){
    // if eta measurement then save Z/R
    double R=sqrt(gPosX*gPosX+gPosY*gPosY);
    double x=gPosZ/R;
    m_hits_in_layer_eta.at(ilay).push_back(x);
  }else{
    // if phi measurement then save phi
    double x=atan2(gPosY,gPosX);
    m_hits_in_layer_phi.at(ilay).push_back(x);
  }  
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonSA::RpcPatFinder::findPatternEta(double &ZoverR_middle, double &ZoverR_outer, unsigned int & pattern){
  double result_dMO;
  bool found=false;
  if (patfinder(false, pattern,  ZoverR_middle,ZoverR_outer, result_dMO)>=2) found=true;
  return found;
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
      msg() << MSG::DEBUG << "patfinder: l_start = "<< l_start
	    << " x= " << current_x
	    << " pat= "    << (std::bitset<8>) pat  << endreq; 

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
	msg() << MSG::DEBUG << "patfinder:  l_test = "<< l_test 
	      << " n_layer= "<< n_layer 
	      << " x= " << current_x 
	      << " pat= " << (std::bitset<8>)pat << endreq;
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
    msg() << MSG::DEBUG << "patfinder: BEST pat= " << (std::bitset<8>)result_pat
	  <<"  dMM= "<<result_dMM 
	  <<"  dMO= "<<result_dMO 
	  <<"  x0= "<<result_x 
	  <<"  x1= "<<result_x1 << endreq;
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
  delta=x2-x1;
  if (isphi){
    if (delta>M_PI) delta-=2*M_PI;
    if (delta<-M_PI) delta+=2*M_PI;
  }
  delta=fabs(x2-x1); 
  

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



