/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ClusterPatFinder.h"

#include <math.h>
#include <bitset>
#include <iostream>

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::ClusterPatFinder::ClusterPatFinder(const std::string& type,
					 const std::string& name,
					 const IInterface*  parent):
  AthAlgTool(type, name, parent)  
{  
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::ClusterPatFinder::addCluster(std::string stationName,
					        int stationEta,
                                                bool  measuresPhi,
                                                unsigned  int gasGap,
                                                unsigned  int doubletR,
                                                double gPosX,
                                                double gPosY,
                                                double gPosZ,
                                                TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const
{
  
  int ilay=0;
  // BO 
  if (stationName.substr(0,2)=="BO") ilay=4;
  // doubletR
  ilay+=2*(doubletR-1);
  // BML7 special chamber with 1 RPC doublet (doubletR=1 but RPC2) :
  if (stationName.substr(0,3)=="BML" && stationEta==7) ilay+=2;
  // gasGap
  ilay+=gasGap-1;

  double R=std::sqrt(gPosX*gPosX+gPosY*gPosY);
  double Phi=std::atan2(gPosY,gPosX);

  if (!measuresPhi){
    // if eta measurement then save Z/R
    R = calibR(stationName,R, Phi);  
    double x=gPosZ/R;
    rpcLayerClusters.clusters_in_layer_eta.at(ilay).push_back(x);
    rpcLayerClusters.clusters_in_layer_R.at(ilay).push_back(R);//mod!
    rpcLayerClusters.clusters_in_layer_Z.at(ilay).push_back(gPosZ);//mod!
   }else{
    // if phi measurement then save phi
    rpcLayerClusters.clusters_in_layer_phi.at(ilay).push_back(Phi);
  }
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
bool TrigL2MuonSA::ClusterPatFinder::findPatternPhi(std::vector<double>& phi_middle, 
                                                    std::vector<double>& phi_outer, 
                                                    TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const
{
  std::vector<TrigL2MuonSA::ClusterPattern> crPatterns;
  
  if( !patfinder(crPatterns, rpcLayerClusters) ) return false;
  removeSimilarRoad(crPatterns);
  for(TrigL2MuonSA::ClusterPattern& crPat : crPatterns){
    if( !crPat.isGoodFit ) continue;
    phi_middle.push_back(crPat.phi_middle);
    phi_outer.push_back(crPat.phi_outer);
    ATH_MSG_DEBUG("phi_middle/phi_outer = " << crPat.phi_middle << "/" << crPat.phi_outer);
  }
  return true;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
bool TrigL2MuonSA::ClusterPatFinder::patfinder(std::vector<TrigL2MuonSA::ClusterPattern>& crPatterns,
                                               TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const
{
  const int N_layers=8;
  bool iphi = true;

  std::vector<std::list<double>> *  rpc_x;
  rpc_x = &rpcLayerClusters.clusters_in_layer_phi;

  int l_start_max=2; //max layer of first hit
  if (rpc_x->at(6).size()+rpc_x->at(7).size()>0) l_start_max=5; // special "feet" towers

  for (int l_start=0; l_start<=l_start_max; l_start++){
    size_t index[8] = {};
    for(int i=0; i<8; i++) index[i]=-1;
    // Loop on hits of start layer, for each hit try a new pattern
    for (std::list<double>::iterator i_start=rpc_x->at(l_start).begin(); i_start!=rpc_x->at(l_start).end(); i_start++){
      TrigL2MuonSA::ClusterPattern crPat;
      crPat.Clear();
      int n_hits=1;
      unsigned int pat=(1<<l_start); // bit pattern of hit layers
      double dMO=9999; // disstance middle-outer station
      double dMM=9999; // distance RPC1-RPC2 on middle stations
      double current_x =*i_start; // set current_x to the starting hit
      int l_current = l_start;

      index[l_start] = std::distance(rpc_x->at(l_start).begin(), i_start);//mod!
      ATH_MSG_DEBUG("patfinder_cluster: l_start = "<< l_start << " x= " << current_x
          << " pat= "    << (std::bitset<8>) pat); 

      // ----- add compatible hits in other layers ----//
      // loop on test layers:
      bool skipLayer = false;
      for (int l_test=l_start+1; l_test<N_layers; l_test++){
        ATH_MSG_DEBUG("start searching the clusters in RPC plane, l_test = " << l_test);
        if(l_test % 2 == 0){
          int n_layer=0; //number of found clusters in sets
          int n_layer_upper=0;
          double x_layer=0; // position of the best cluster at test layer
          double delta_layer=999; //minimum d(Z/R) 
          int layerID = 0;
          for(int ilayer = 0; ilayer < 2; ilayer++){
            for (std::list<double>::iterator i_test=rpc_x->at(l_test + ilayer).begin(); i_test!=rpc_x->at(l_test + ilayer).end(); i_test++){ 
              double delta=-1;
              // check if within the road
              if (!deltaOK( l_current,l_test + ilayer,current_x,*i_test,iphi,delta)) continue;
              if(ilayer == 0)n_layer++; //in the layer,if there is the cluster which pass th deltaOK
              else if(ilayer == 1)n_layer_upper++;
              // if closest hit update x_layer
              if (delta<delta_layer) {
                delta_layer=delta;
                x_layer=*i_test;
                layerID = ilayer;	      
                index[l_test+ilayer]       = std::distance(rpc_x->at(l_test+ilayer).begin(), i_test);//mod! 
                //"index" is the ID of the best matched cluster in "l_test" layer 
                if(ilayer == 1){
                  ATH_MSG_DEBUG("the minimum delta was found in upper layer, update info");
                  index[l_test] = 0;
                  skipLayer = true;
                }
              }
            }//for i_test
          }//for ilayer
          if (n_layer>0 || n_layer_upper>0) {// compatible hit found in this layer increase n_hits
            int l_result = l_test + layerID;
            n_hits+=1;
            current_x=x_layer;
            pat+=(1<<l_result);
            l_current=l_result;
            if (l_start<4&&l_result>=4&&delta_layer<dMO){
              dMO=delta_layer;
            }else if (l_start<2&&l_result>=2&&l_result<4&&delta_layer<dMM) {
              dMM=delta_layer;
            } else  if (l_start>=4&&l_start<5&&l_result>=6&&delta_layer<dMM) {
              dMM=delta_layer;
            }
          }// if (n_layer)
          if(layerID == 0){
            ATH_MSG_DEBUG(" l_test = "<< l_test+layerID << " n_layer= "<< n_layer 
                << " x= " << current_x << " pat= " << (std::bitset<8>)pat);
          }
          else if(layerID == 1){
            ATH_MSG_DEBUG(" l_test = "<< l_test+layerID << " n_layer_upper= "<< n_layer_upper 
                << " x= " << current_x << " pat= " << (std::bitset<8>)pat);
          }
        }//test layer is 2 || 4 || 6
        else if(l_test % 2 == 1){
          int n_layer=0; //number of found clusters in sets
          double x_layer=0; // position of the best cluster at test layer
          double delta_layer=999; //minimum d(Z/R) 
          //  loop on hits of test layer and picks the one with smaller distance from current_x
          for (std::list<double>::iterator i_test=rpc_x->at(l_test).begin(); i_test!=rpc_x->at(l_test).end(); i_test++){ 
            double delta=-1;
            // check if within the road
            if (!deltaOK( l_current,l_test,current_x,*i_test,iphi,delta)) continue;
            n_layer++; //in the layer,if there is the cluster which pass th deltaOK
            // if closest hit update x_layer
            if (delta<delta_layer) {
              delta_layer=delta;
              x_layer=*i_test;	      
              index[l_test]       = std::distance(rpc_x->at(l_test).begin(), i_test);//mod! 
              //"index" is the ID of the best matched cluster in "l_test" layer 
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

          ATH_MSG_DEBUG(" l_test = "<< l_test << " n_layer= "<< n_layer 
              << " x= " << current_x << " pat= " << (std::bitset<8>)pat);
          ATH_MSG_DEBUG(" dMM/dMO = " << dMM << "/" << dMO);
        }
        if(skipLayer){
          l_test++;
          skipLayer = false;
        }


      }//for l_test
      //////////if the number of clusters in set is less than 3, we ignore the set////// 
      if(n_hits >= 2){
        crPat.phi_middle = *i_start;
        crPat.phi_outer = current_x;
        crPat.dMM = dMM;
        crPat.dMO = dMO;
        for(int i = 0; i < 8; i++){
          crPat.clustersID[i] = index[i];
        }
        crPatterns.push_back(crPat);
      }
    }//for i_start
  }//for l_start
  return crPatterns.size() > 0;
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonSA::ClusterPatFinder::findPatternEta(std::vector<std::vector<double>>& aw, 
                                                    std::vector<std::vector<double>>& bw, 
                                                    TrigL2MuonSA::RpcLayerClusters&   rpcLayerClusters) const
{
  std::vector<TrigL2MuonSA::ClusterPattern> crPatterns;

  if( !patfinder_forEta(crPatterns, rpcLayerClusters) ) return false;
  removeSimilarRoad(crPatterns);
  for(TrigL2MuonSA::ClusterPattern& crPat : crPatterns){
    if( !crPat.isGoodFit ) continue;
    for(int i=0; i<3; i++){
      aw[i].push_back(crPat.aw[i]);
      bw[i].push_back(crPat.bw[i]);

      ATH_MSG_DEBUG("aw[" << i << "]/bw[" << i << "] = " << crPat.aw[i] << "/" << crPat.bw[i]);
    }
  }
  return true;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonSA::ClusterPatFinder::patfinder_forEta(std::vector<TrigL2MuonSA::ClusterPattern>& crPatterns, 
                                                      TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const
{
  bool iphi = false;
  std::vector<std::list<double>> *  rpc_x;
  rpc_x = &rpcLayerClusters.clusters_in_layer_eta;
  int  layer_end;
  if(rpc_x->at(6).size()+rpc_x->at(7).size() >0) layer_end = 7;//special "feet" towers
  else                                           layer_end = 5;

  // Loop on start layer
  ATH_MSG_DEBUG("=== start to find rpc clusters pattern ===");
  for (int l_start=0; l_start<layer_end; l_start++){
    size_t index[8]={};
    for(int i=0; i<8; i++) index[i]=-1;
    TrigL2MuonSA::ClusterPattern crPat;
    crPat.Clear();

    // Loop on hits of start layer, for each hit try a new pattern
    for (std::list<double>::iterator i_start=rpc_x->at(l_start).begin(); i_start!=rpc_x->at(l_start).end(); i_start++){
      int n_hits=1;
      unsigned int pat=(1<<l_start); // bit pattern of hit layers
      double dMO=9999; // disstance middle-outer station
      double dMM=9999; // distance RPC1-RPC2 on middle stations
      double aw[3] = {0., 0., 0. };
      double bw[3] = {0., 0., 0. };
      double current_x =*i_start; // set current_x to the starting hit
      int l_current = l_start;
      index[l_start] = std::distance(rpc_x->at(l_start).begin(), i_start);//mod!

      ATH_MSG_DEBUG("l_start = "<< l_start << " x= " << current_x
          << " pat= "    << (std::bitset<8>) pat); 

      // ----- add compatible hits in other layers ----//
      // loop on test layers:
      bool skipLayer = false;
      for (int l_test=l_start+1; l_test<=layer_end; l_test++){
        ATH_MSG_DEBUG("start searching the clusters in RPC plane, l_test = " << l_test);
        if(l_test % 2 == 0){
          int n_layer=0; //number of found clusters in sets
          int n_layer_upper=0;
          double x_layer=0; // position of the best cluster at test layer
          double delta_layer=999; //minimum d(Z/R) 
          int layerID = 0;
          for(int ilayer = 0; ilayer < 2; ilayer++){
            for (std::list<double>::iterator i_test=rpc_x->at(l_test + ilayer).begin(); i_test!=rpc_x->at(l_test + ilayer).end(); i_test++){ 
              double delta=-1;
              // check if within the road
              if (!deltaOK( l_current,l_test + ilayer,current_x,*i_test,iphi,delta)) continue;
              if(ilayer == 0) n_layer++; //in the layer,if there is the cluster which pass th deltaOK
              else if(ilayer == 1) n_layer_upper++;
              // if closest hit update x_layer
              if (delta<delta_layer) {
                delta_layer=delta;
                x_layer=*i_test;
                layerID = ilayer;	      
                index[l_test+ilayer]       = std::distance(rpc_x->at(l_test+ilayer).begin(), i_test);//mod! 
                //"index" is the ID of the best matched cluster in "l_test" layer 
                if(ilayer == 1){
                  ATH_MSG_DEBUG("the minimum delta was found in upper layer, update info");
                  index[l_test] = 0;
                  skipLayer = true;
                }
              }
            }//for i_test
          }//for ilayer
          if (n_layer>0 || n_layer_upper>0) {// compatible hit found in this layer increase n_hits
            int l_result = l_test + layerID;
            n_hits+=1;
            current_x=x_layer;
            pat+=(1<<l_result);
            l_current=l_result;
            if (l_start<4&&l_result>=4&&delta_layer<dMO){
              dMO=delta_layer;
            }else if (l_start<2&&l_result>=2&&l_result<4&&delta_layer<dMM) {
              dMM=delta_layer;
            } else  if (l_start>=4&&l_start<5&&l_result>=6&&delta_layer<dMM) {
              dMM=delta_layer;
            }
          }// if (n_layer)
          if(layerID == 0){
            ATH_MSG_DEBUG(" l_test = "<< l_test+layerID << " n_layer= "<< n_layer 
                << " x= " << current_x << " pat= " << (std::bitset<8>)pat);
          }
          else if(layerID == 1){
            ATH_MSG_DEBUG(" l_test = "<< l_test+layerID << " n_layer_upper= "<< n_layer_upper 
                << " x= " << current_x << " pat= " << (std::bitset<8>)pat);
          }
        }//test layer is 2 || 4 || 6
        else if(l_test % 2 == 1){
          int n_layer=0; //number of found clusters in sets
          double x_layer=0; // position of the best cluster at test layer
          double delta_layer=999; //minimum d(Z/R) 
          //  loop on hits of test layer and picks the one with smaller distance from current_x
          for (std::list<double>::iterator i_test=rpc_x->at(l_test).begin(); i_test!=rpc_x->at(l_test).end(); i_test++){ 
            double delta=-1;
            // check if within the road
            if (!deltaOK( l_current,l_test,current_x,*i_test,iphi,delta)) continue;
            n_layer++; //in the layer,if there is the cluster which pass th deltaOK
            // if closest hit update x_layer
            if (delta<delta_layer) {
              delta_layer=delta;
              x_layer=*i_test;	      
              index[l_test]       = std::distance(rpc_x->at(l_test).begin(), i_test);//mod! 
              //"index" is the ID of the best matched cluster in "l_test" layer 
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
            }else if(l_start>=4&&l_start<5&&l_test>=6&&delta_layer<dMM) {
              dMM=delta_layer;
            }
          }// if (n_layer)

          ATH_MSG_DEBUG(" l_test = "<< l_test << " n_layer= "<< n_layer 
              << " x= " << current_x << " pat= " << (std::bitset<8>)pat);
          ATH_MSG_DEBUG(" dMM/dMO = " << dMM << "/" << dMO);
        }
        if(skipLayer){
          l_test++;
          skipLayer = false;
        }
      }//for l_test

      //////////if the number of clusters in set is less than 3, we ignore the set////// 
      if (n_hits>=2) {
        crPat.dMM = dMM;
        crPat.dMO = dMO;
        abcal(pat, index, aw, bw, rpcLayerClusters);
        for(int i = 0; i < 3; i++){
          crPat.aw[i] = aw[i];
          crPat.bw[i] = bw[i];
        }
        for(int i = 0; i < 8; i++){
          crPat.clustersID[i] = index[i];
        } 
        crPatterns.push_back(crPat);
      }
    }//for i_start
  }//for l_start
  return crPatterns.size() > 0;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool  TrigL2MuonSA::ClusterPatFinder::deltaOK(int l1, int l2, double x1, double x2, int isphi, double &delta) const
{
  
  
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
  if(isphi) delta=std::abs(std::acos(std::cos(x2-x1)));
  else delta=std::abs(x2-x1); 

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
  return delta<delta_max;
  
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::ClusterPatFinder::calibR(std::string stationName, double R, double Phi) const
{
  double DeltaPhi, temp_phi;
  double calibPhi = std::acos(std::cos(Phi)); // 0 < Phi < 2PI
  
  if(std::string::npos != stationName.rfind('L')){//For Large , SP
    DeltaPhi= 999; temp_phi=9999;
    for(int inum=0;inum < 8;inum++){
      temp_phi = std::abs((inum * M_PI/4.0 )- calibPhi);
      DeltaPhi = std::min(temp_phi, DeltaPhi);
    }
  }else if(std::string::npos != stationName.rfind('S') ||
	   std::string::npos != stationName.rfind('F') ||
	   std::string::npos != stationName.rfind('G')   ){
    DeltaPhi= 999; temp_phi=9999;

    for(int inum=0;inum < 8;inum++){
      temp_phi = std::abs(inum *(M_PI/4.0 )+(M_PI/8.0) - calibPhi);
      DeltaPhi = std::min(temp_phi, DeltaPhi);
    }//for end
  }else return R;

  return R *std::cos(DeltaPhi);
}//calbR()

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::ClusterPatFinder::abcal(unsigned int result_pat, size_t index[], double aw[], double bw[], TrigL2MuonSA::RpcLayerClusters& rpcLayerClusters) const
{
  const float ZERO_LIMIT = 1.e-5;

  std::vector<std::vector<double> > * rpc_R;
  std::vector<std::vector<double> > * rpc_Z;
  rpc_R = &rpcLayerClusters.clusters_in_layer_R;
  rpc_Z = &rpcLayerClusters.clusters_in_layer_Z;
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
  //
  double R_refit = 0;
  double Z_refit = 0;
  bool refitFlag = false;
  if((hot_max[1]-hot_min[1] == 1) && (hot_min[1] == 0 || hot_min[1] == 2)){ 
    refitFlag = true;
    R_refit = (R[hot_max[1]]+R[hot_min[1]])/2;
    Z_refit = (Z[hot_max[1]]+Z[hot_min[1]])/2;
  }
 
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
    theta_m = std::atan2(R[hot_min[0]],Z[hot_min[0]]);
    theta_t = std::atan2(R[hot_max[0]]-R[hot_min[0]],Z[hot_max[0]]-Z[hot_min[0]]);
    theta_f = (theta_m+theta_t)/2.0;
      
    aw[0] = std::tan(theta_f);
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
      if(std::abs(Z[hot_max[i]] - Z[hot_min[i]]) > ZERO_LIMIT) {
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
  //==========================================
  //If the clusters only in RPC1 or RPC2 are used for fitting,   
  //then refit by adding vertex (R, Z) = (0, 0) will be done
  //
  if(refitFlag){
    aw[1] = Z_refit/R_refit;
    bw[1] = 0.0;
    ATH_MSG_DEBUG("the result of refit : aw/bw = " << aw[1] << "/" << bw[1]);
  }
  //==========================================

}//abcal()

// --------------------------------------------------------------------------------                  
// --------------------------------------------------------------------------------                  
void TrigL2MuonSA::ClusterPatFinder::removeSimilarRoad(std::vector<TrigL2MuonSA::ClusterPattern>& crPatterns) const
{
  int nGroup = 1;

  ATH_MSG_DEBUG("============= setGroup start ========================");
  setGroup(nGroup, crPatterns); 

  ATH_MSG_DEBUG("============= selectGoodFit start ========================");
  selectGoodFit(nGroup, crPatterns);
}

// --------------------------------------------------------------------------------                  
// --------------------------------------------------------------------------------                  

void TrigL2MuonSA::ClusterPatFinder::setGroup(int& nGroup, 
                                              std::vector<TrigL2MuonSA::ClusterPattern>& crPatterns) const
{
  nGroup = 1;
  crPatterns.at(0).group = 0; //initial index is defined as first group (No.0)
  if(crPatterns.size() > 1){
    for(unsigned int iClus_start = 1; iClus_start < crPatterns.size(); iClus_start++){
      ATH_MSG_DEBUG("checked road : clusterID = {" << crPatterns.at(iClus_start).clustersID[0] << "," << crPatterns.at(iClus_start).clustersID[1] << "," << crPatterns.at(iClus_start).clustersID[2] << "," << crPatterns.at(iClus_start).clustersID[3] << "," << crPatterns.at(iClus_start).clustersID[4] << "," << crPatterns.at(iClus_start).clustersID[5] << "," << crPatterns.at(iClus_start).clustersID[6] << "," << crPatterns.at(iClus_start).clustersID[7] << "}");
      for(int igroup = 0; igroup < nGroup; igroup++){
        bool isDiffGroupFlag = false;
        int countDiffId_min = 9999;
        for(unsigned int iClus_test = 0; iClus_test < crPatterns.size(); iClus_test++){
          if(crPatterns.at(iClus_test).group != igroup) continue;
          int countDiffId = 0;
          bool isDiffFlag = false;
          ATH_MSG_DEBUG("the compared road : clusterID = {" << crPatterns.at(iClus_test).clustersID[0] << "," << crPatterns.at(iClus_test).clustersID[1] << "," << crPatterns.at(iClus_test).clustersID[2] << "," << crPatterns.at(iClus_test).clustersID[3] << "," << crPatterns.at(iClus_test).clustersID[4] << "," << crPatterns.at(iClus_test).clustersID[5] << "," << crPatterns.at(iClus_test).clustersID[6] << "," << crPatterns.at(iClus_test).clustersID[7] << "}");
          for(int iLay = 0; iLay < 8; iLay++){
            if(crPatterns.at(iClus_test).clustersID[iLay] > -1 && crPatterns.at(iClus_start).clustersID[iLay] > -1){
              if(crPatterns.at(iClus_test).clustersID[iLay] != crPatterns.at(iClus_start).clustersID[iLay]) countDiffId++;
            }
          }
          ATH_MSG_DEBUG("the number of different id's clusters = " << countDiffId);
          if(countDiffId > 1){ isDiffFlag = true;}
          else{ if(countDiffId_min > countDiffId) countDiffId_min = countDiffId;}
          if(isDiffFlag) isDiffGroupFlag = true;
        }//iClus_test
        if(!isDiffGroupFlag){
          ATH_MSG_DEBUG("this set may be in this group");
          crPatterns.at(iClus_start).groupCand.emplace(igroup, countDiffId_min);
        }// if isDiffGroupFlag
      }// igroup
      if(crPatterns.at(iClus_start).groupCand.empty()){
        ATH_MSG_DEBUG("this road is accepted, Group No. " << nGroup);
        crPatterns.at(iClus_start).group = nGroup;
        nGroup++;
      }//if groupCand
      else{
        ATH_MSG_DEBUG("this road is denied, searching the appropriate group......");
        int theGroup = 0;
        int minDiff = 2;
        for(auto itGrp = crPatterns.at(iClus_start).groupCand.begin(); itGrp != crPatterns.at(iClus_start).groupCand.end(); ++itGrp){
          ATH_MSG_DEBUG("group No." << itGrp->first << ", number of different ID." << itGrp->second);
          if(minDiff > (itGrp->second)){
            theGroup = itGrp->first;
            minDiff = itGrp->second;
          }
        }
        ATH_MSG_DEBUG("the group of this road is defined as No." << theGroup);
        crPatterns.at(iClus_start).group = theGroup;
      }// else groupCand
    }//iClus_start
  }
}
// --------------------------------------------------------------------------------                  
// --------------------------------------------------------------------------------                 
void TrigL2MuonSA::ClusterPatFinder::selectGoodFit(int nGroup,
                                                   std::vector<TrigL2MuonSA::ClusterPattern>& crPatterns) const
{

  for(int iGroup = 0; iGroup < nGroup; iGroup++){
    std::vector<TrigL2MuonSA::ClusterPattern> crpat_group;
    crpat_group.clear();
    int Nclus_max = -999;
    for(TrigL2MuonSA::ClusterPattern& pat : crPatterns ){
      if(pat.group != iGroup) continue;
      for(int ilay = 0; ilay < 8; ilay++){
        if(pat.clustersID[ilay] > -1) pat.nclusters++;
      }
      if(Nclus_max < pat.nclusters) Nclus_max = pat.nclusters;
      crpat_group.push_back(pat);
      ATH_MSG_DEBUG("the number of clusters = " << pat.nclusters);
    }
    ATH_MSG_DEBUG("max number of clusters = " << Nclus_max);
    double smallestdMM = 999999;
    double smallestdMO = 999999;

    TrigL2MuonSA::ClusterPattern bestPat;
    bestPat.Clear();
    for(TrigL2MuonSA::ClusterPattern& grpat : crpat_group){
      if(Nclus_max == grpat.nclusters){
        if(grpat.dMM < smallestdMM || 
           (grpat.dMM == smallestdMM && grpat.dMO < smallestdMO)){
            bestPat = grpat;
            smallestdMM = grpat.dMM;
            smallestdMO = grpat.dMO;
        }//dMM/dMO selection
      }
    }
    for(TrigL2MuonSA::ClusterPattern& pat : crPatterns){
      if(bestPat == pat){
        ATH_MSG_DEBUG("find best clusterPattern!");
        ATH_MSG_DEBUG("bestPattern aw[1]/bw[1] = " << bestPat.aw[1] << "/" << bestPat.bw[1] << ", pat aw[1]/bw[1] = " << pat.aw[1] << "/" << pat.bw[1]);
        pat.isGoodFit = true;
      }
    }
    ATH_MSG_DEBUG("clusterID = {" << bestPat.clustersID[0] << "," << bestPat.clustersID[1] << "," << bestPat.clustersID[2] << "," << bestPat.clustersID[3] << "," << bestPat.clustersID[4] << "," << bestPat.clustersID[5] << "," << bestPat.clustersID[6] << "," << bestPat.clustersID[7] << "}");
  }
}
// --------------------------------------------------------------------------------                  
// --------------------------------------------------------------------------------                  

