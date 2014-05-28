/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  BinsDiffFromStripMedianOnline.cxx is to pick out the problematic bins in 2D histogram assuming that y-axis(the phi direction) be symmetric. 
    Author: Feng TIAN (columbia university)
    Email:  Feng.Tian@cern.ch 
*/

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/BinsDiffFromStripMedianOnline.h>
#include "dqm_algorithms/BinsDiffFromStripMedian.h"
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/AlgorithmManager.h>

#include <TH1.h>
#include <TF1.h>
#include <TClass.h>
#include <math.h>

#include <iostream>
#include <string>

using namespace std;

//bool mySortfunc(binOnline i,binOnline j){return (i.m_value > j.m_value);}
//bool mySortfunc_ratio(binOnline i, binOnline j){return (i.m_outstandingRatio> j.m_outstandingRatio);}

static dqm_algorithms::BinsDiffFromStripMedianOnline myInstance;

dqm_algorithms::BinsDiffFromStripMedianOnline::BinsDiffFromStripMedianOnline( ){
  dqm_core::AlgorithmManager::instance().registerAlgorithm("BinsDiffFromStripMedianOnline", this);
}

dqm_algorithms::BinsDiffFromStripMedianOnline::~BinsDiffFromStripMedianOnline(){
}

dqm_algorithms::BinsDiffFromStripMedianOnline * dqm_algorithms::BinsDiffFromStripMedianOnline::clone(){
  return new BinsDiffFromStripMedianOnline();
}


dqm_core::Result * dqm_algorithms::BinsDiffFromStripMedianOnline::execute(const std::string &  name, 
									  const TObject& object, 
									  const dqm_core::AlgorithmConfig& config ){ 
  TH1* histogram;

  if( object.IsA()->InheritsFrom( "TH1" ) ) {
    histogram = (TH1*)&object;
    if (histogram->GetDimension() > 2 ){ 
      throw dqm_core::BadConfig( ERS_HERE, name, "dimension > 2 " );
    }
  } else {
    throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1" );
  }
  
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  const double minstatperstrip = dqm_algorithms::tools::GetFirstFromMap( "MinStatPerstrip", config.getParameters(), -1);
  const double ignoreval1 = dqm_algorithms::tools::GetFirstFromMap( "ignoreval1", config.getParameters(), -99999);
  const double ignoreval2 = dqm_algorithms::tools::GetFirstFromMap( "ignoreval2", config.getParameters(), 0.);
  const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 1); 
  const int Nmaxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20); 
  const bool VisualMode = (bool) dqm_algorithms::tools::GetFirstFromMap( "VisualMode", config.getParameters(), 1);
  const int NpublishRed = (int) dqm_algorithms::tools::GetFirstFromMap( "PublishRedBins",config.getParameters(), 0);
  const bool ClusterResult = (bool) dqm_algorithms::tools::GetFirstFromMap( "ClusterResult", config.getParameters(), 0);  
  const double suppressFactor = dqm_algorithms::tools::GetFirstFromMap("SuppressFactor", config.getParameters(), 0.05);
  const double suppressRedFactor = dqm_algorithms::tools::GetFirstFromMap("SuppressRedFactor", config.getParameters(), 0.01);
  const bool OnlineMode = (bool) dqm_algorithms::tools::GetFirstFromMap( "OnlineMode", config.getParameters(), 0);
  const double Nred_red = (double) dqm_algorithms::tools::GetFirstFromMap( "Nred_red", config.getParameters(), 5.);
  const double Nred_yellow = (double) dqm_algorithms::tools::GetFirstFromMap( "Nred_yellow", config.getParameters(), 3.);
  const double Nyellow_yellow = (double) dqm_algorithms::tools::GetFirstFromMap( "Nyellow_yellow", config.getParameters(), 5.);
  const double Nredyellow_yellow = (double) dqm_algorithms::tools::GetFirstFromMap( "Nredyellow_yellow", config.getParameters(), 4.);
  
  if ( histogram->GetEntries() < minstat ) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
  
  double gthreshold;
  double rthreshold;
  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "MaxDeviation", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "MaxDeviation", config.getGreenThresholds() );
  }catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  
  std::vector<int> range=dqm_algorithms::tools::GetBinRange(histogram, config.getParameters()); 
  vector<double> stripsSize;
  vector<double> stripsMedian;
  vector<double> stripsAvg; 
  vector<double> stripsVariance;
  double maxInMap=0;

  if ( (int)range.size() < 4 ){
    throw dqm_core::BadConfig( ERS_HERE, name, "BinRange vector <4 " );
  }

  for ( int i = range[0]; i <= range[1]; ++i ) {
    vector<double> onestrip;
    double stripSum=0;
    for ( int j = range[2]; j <= range[3]; ++j ) {
      float binvalue = histogram->GetBinContent(i,j);
      if (fabs(binvalue- ignoreval1)<0.0001 || fabs(binvalue - ignoreval2)<0.0001) continue;
      onestrip.push_back(binvalue);
      stripSum += binvalue;
      if(binvalue > maxInMap) {
	maxInMap = binvalue;
      }
    }
    if(onestrip.size()!=0 ) {
      stripsAvg.push_back(stripSum/onestrip.size());
    }else {
      stripsAvg.push_back(0);
    }
    FindStripMedianOnline(name,onestrip,stripsMedian);
    stripsSize.push_back(onestrip.size());

    float sumdiff2=0;
    int counter=0;
    if ((int)stripsAvg.size() <= i-range[0] ){
      throw dqm_core::BadConfig( ERS_HERE, name, "out of stripsAvg range " );
    }
    float strip_avg = stripsAvg[i-range[0]];
 
    for ( int j = range[2]; j <= range[3]; ++j ) {
      if (fabs(histogram->GetBinContent(i,j)-ignoreval1)<0.0001 || fabs(histogram->GetBinContent(i,j)-ignoreval2)<0.0001) continue;
      double binvalue = histogram->GetBinContent(i,j);
      double diff=binvalue-strip_avg;
      sumdiff2 +=pow(diff,2);
      counter++;
    }
    double variance=-1;
    if(counter!=0) variance = sumdiff2 / counter ;
    stripsVariance.push_back(variance);
  }

  dqm_core::Result* result = new dqm_core::Result();
  vector<binOnline> redbins;
  vector<binOnline> yellowbins;
  vector<binOnline> Allbins;
  int limit = range[1]-range[0];
  for ( int q=0; q <= limit; ++q ) {
    int k=q+range[0];
    if ((int)stripsSize.size() <= q ){
      throw dqm_core::BadConfig( ERS_HERE, name, "out of stripsSize range " );
    }
    if(stripsSize[q]<minstatperstrip) continue;
    if ((int)stripsMedian.size() <= q ){
      throw dqm_core::BadConfig( ERS_HERE, name, "out of stripsMedian range " );
    }
    if ((int)stripsVariance.size() <= q){
      throw dqm_core::BadConfig( ERS_HERE, name, "out of stripsVariance range " );
    }
    if ((int)stripsAvg.size() <= q ){
      throw dqm_core::BadConfig( ERS_HERE, name, "out of stripsAvg range " );
    }
    double strip_median = stripsMedian[q];
    double strip_variance = stripsVariance[q];
    double strip_avg = stripsAvg[q];

    if(fabs(strip_median)<0.0001 && fabs(strip_variance)<0.0001){ 
      continue; // skip empty strip
    }else {
      if(fabs(strip_median)<0.0001 && fabs(strip_variance)>0.0001 && fabs(strip_avg)>0.0001) {
	strip_median = strip_avg;
      } else{
	if(fabs(strip_median)<0.0001 && fabs(strip_variance)>0.0001 && fabs(strip_avg)<0.0001) continue;
      }
    }

 
    for ( int l = range[2]; l <= range[3]; ++l ) {
      double binvalue = histogram->GetBinContent(k,l);
      if (fabs(binvalue-ignoreval1)<0.0001 || fabs(binvalue-ignoreval2)<0.0001) continue;
      double outstandingRatio=0; 
      if(fabs(strip_median) > 0.0001 ){
	outstandingRatio= /*binvalue/strip_median*/ (binvalue-strip_median)/sqrt(fabs(strip_median));
      }else{
	continue;
      }
      double eta = histogram->GetXaxis()->GetBinCenter(k);
      double phi = histogram->GetYaxis()->GetBinCenter(l); 
      binOnline onebin = {eta,phi,k,l,binvalue,outstandingRatio};
      Allbins.push_back(onebin);
      //      if( VisualMode  && (binvalue / maxInMap < suppressFactor) ) continue;
      if(fabs(outstandingRatio) > rthreshold ) {
        if( VisualMode  && (binvalue / maxInMap < suppressRedFactor) ){
	  continue;
	}
        redbins.push_back(onebin);
      }else if(fabs(outstandingRatio) > gthreshold ){ 
	if( VisualMode  && (binvalue / maxInMap < suppressFactor) ){
	  continue;
	}
	yellowbins.push_back(onebin);
      }
    }    
  }
  int count_red_c = 0;
  int count_yellow_c = 0;
  vector<vector<colorbinOnline> > ColorBinMap;
  if(ClusterResult){
    // initialize ColorBinMap
    int limit = range[1]-range[0];
    for ( int q = 0; q <= limit; ++q ) {
      int k =q+range[0];
      vector<colorbinOnline> oneColorStrip;
      for ( int l = range[2]; l <= range[3]; ++l ) {
	//     colorbinOnline oneColorBin = {k,l,-1,-1,-1,green,1};
	colorbinOnline oneColorBin = {-1,-1,k,l,-1,green,1};
	oneColorStrip.push_back(oneColorBin);
      }
      ColorBinMap.push_back(oneColorStrip);
    } 

    // map redbins and yellowbins to ColorBinMap
    for(unsigned int i=0;i<redbins.size();i++){
      //  int k=redbins[i].m_ix;
      //  int l=redbins[i].m_iy;
      int q=redbins[i].m_ix - range[0];
      int p = redbins[i].m_iy-range[2];

      if ((int)ColorBinMap.size() <= q){
	throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
      }

      if ((int)ColorBinMap[q].size() <= p ){
	throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
      }

      ColorBinMap[q][p].m_eta = redbins[i].m_eta;

      ColorBinMap[q][p].m_phi = redbins[i].m_phi;
      ColorBinMap[q][p].m_value = redbins[i].m_value;
      ColorBinMap[q][p].m_color = red;

    }


    for(unsigned int i=0;i<yellowbins.size();i++){
      //  int k=yellowbins[i].m_ix;
      //  int l=yellowbins[i].m_iy;
      int q=yellowbins[i].m_ix - range[0];
      int p = yellowbins[i].m_iy-range[2];

      if ((int)ColorBinMap.size() <= q ){
	throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
      }

      if ((int)ColorBinMap[q].size() <= p ){
	throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
      }

      ColorBinMap[q][p].m_eta = yellowbins[i].m_eta;
      ColorBinMap[q][p].m_phi = yellowbins[i].m_phi;
      ColorBinMap[q][p].m_value = yellowbins[i].m_value;
      ColorBinMap[q][p].m_color = yellow;
    }


    // cluster bad bins
    vector<colorclusterOnline > clusterArray;
    for(unsigned int i=0;i<redbins.size();i++){
      //   const int k=redbins[i].m_ix;
      //   const int l=redbins[i].m_iy;
      int q=redbins[i].m_ix - range[0];
      int p = redbins[i].m_iy-range[2];

      if ((int)ColorBinMap.size() <= q ){
	throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
      }

      if ((int)ColorBinMap[q].size() <= p ){
	throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
      }

      if(ColorBinMap[q][p].m_color != green){
	colorclusterOnline onecluster = MakeClusterOnline(name,range[0],range[2],redbins[i],ColorBinMap);
	//   colorclusterOnline onecluster = {0,0,0,0,green,-1};
	if((int)onecluster.m_size > 1) clusterArray.push_back(onecluster);
      }
    }
    for(unsigned int i=0;i<yellowbins.size();i++){
      //   const int k=yellowbins[i].m_ix;
      //   const int l=yellowbins[i].m_iy;
      int q=yellowbins[i].m_ix - range[0];
      int p = yellowbins[i].m_iy-range[2];

      if ((int)ColorBinMap.size() <= q ){
	throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
      }

      if ((int)ColorBinMap[q].size() <= p ){
	throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
      }

      if(ColorBinMap[q][p].m_color != green){
	colorclusterOnline onecluster = MakeClusterOnline(name,range[0],range[2],yellowbins[i],ColorBinMap);
	//   colorclusterOnline onecluster = {0,0,0,0,green,-1};
	if((int)onecluster.m_size > 1) clusterArray.push_back(onecluster);
      }
    }

    // publish clusters here:
    for(unsigned int i=0;i<clusterArray.size();i++){
      char tmp[500];
      if(clusterArray[i].m_color==red){
	snprintf(tmp,500,"CR%i-(eta,phi)(r)(size)=(%0.3f,%0.3f)(%0.3f)(%i)",count_red_c,clusterArray[i].m_eta,clusterArray[i].m_phi,clusterArray[i].m_radius,clusterArray[i].m_size);
	count_red_c++;
      }else if(clusterArray[i].m_color==yellow){
	snprintf(tmp,500,"CY%i-(eta,phi)(r)(size)=(%0.3f,%0.3f)(%0.3f)(%i)",count_yellow_c,clusterArray[i].m_eta,clusterArray[i].m_phi,clusterArray[i].m_radius,clusterArray[i].m_size);
	count_yellow_c++;
      }
      std::string tag(tmp);
      result->tags_[tag] = clusterArray[i].m_value;
    }
    result->tags_["NRedClusters"] = count_red_c;  
    result->tags_["NYellowClusters"] = count_yellow_c; 

  }

  // std::sort(redbins.begin(),redbins.end(),mySortfunc);
  std::sort(redbins.begin(),redbins.end());
  // std::sort(yellowbins.begin(),yellowbins.end(),mySortfunc); 
  std::sort(yellowbins.begin(),yellowbins.end()); 
  // std::sort(Allbins.begin(),Allbins.end(),mySortfunc_ratio);
  std::sort(Allbins.begin(),Allbins.end());
  // publish red bins
  int count_red=0;
  for(unsigned int i=0;i<redbins.size();i++){
   
    int q = redbins[i].m_ix-range[0];
    int p = redbins[i].m_iy-range[2];

    if(ClusterResult){
      if(q<(int)ColorBinMap.size()){
	if(p<(int)ColorBinMap[q].size()){
	  if( ColorBinMap[q][p].m_status==0 ) continue;
	}
      }
    }

    if(publish){
      char tmp[500];
      snprintf(tmp,500,"R%i-(eta,phi)[OSRatio]=(%0.3f,%0.3f)[%0.2e]",count_red,redbins[i].m_eta,redbins[i].m_phi,redbins[i].m_outstandingRatio);
      std::string tag(tmp);
      result->tags_[tag] = redbins[i].m_value;
    }  
    count_red++;
    if(NpublishRed > 0){
      if(count_red > NpublishRed) break;
    }
  }

  // publish yellow bins
  int count_yellow=0;
  for(unsigned int i=0;i<yellowbins.size();i++){
    int q = yellowbins[i].m_ix-range[0];
    int p = yellowbins[i].m_iy-range[2];

    if(ClusterResult){
      if(q<(int)ColorBinMap.size()){
	if(p<(int)ColorBinMap[q].size()){
	  if(ColorBinMap[q][p].m_status==0) continue;
	}
      }
    }
    if(publish && (count_red+count_yellow) < Nmaxpublish ){
      char tmp[500];
      snprintf(tmp,500,"Y%i-(eta,phi)[OSRatio]=(%0.3f,%0.3f)[%.2e]",count_yellow,yellowbins[i].m_eta,yellowbins[i].m_phi,yellowbins[i].m_outstandingRatio);
      std::string tag(tmp);
      result->tags_[tag] = yellowbins[i].m_value;
    }
    count_yellow++;
  }
  result->tags_["NRedBins"] = count_red;  // count_red is the number of red bins printed
  result->tags_["NYellowBins"] = count_yellow; // count_yellow is the number of yellow bins printed

  if(count_red+count_yellow==0 && (int)Allbins.size()>=5 ){
    for(int i=0;i<5;i++){
      char tmptmp[500];
      snprintf(tmptmp,500,"LeadingBin%i-(eta,phi)=(%0.3f,%0.3f)",i,Allbins[i].m_eta,Allbins[i].m_phi);
      std::string tagtag(tmptmp);
      result->tags_[tagtag] = Allbins[i].m_value;
    }

  }
  

  if(!OnlineMode){
    if(count_red>0 || count_red_c>0) {
      result->status_ = dqm_core::Result::Red;
    } else {
      if (count_yellow>0||count_yellow_c>0) { 
	result->status_ = dqm_core::Result::Yellow;
      } else { result->status_ = dqm_core::Result::Green; }
    } 
  } else {
    if(count_red>=Nred_red){
      result->status_ = dqm_core::Result::Red;
    }else if (count_red>=Nred_yellow || count_yellow>=Nyellow_yellow || (count_red+count_yellow)>=Nredyellow_yellow){
      result->status_ = dqm_core::Result::Yellow;
    }else{
      result->status_ = dqm_core::Result::Green;
    }
  }

  return result;
  
}

void dqm_algorithms::BinsDiffFromStripMedianOnline::FindStripMedianOnline(const std::string &  name,vector<double> onestrip_tmp,vector<double>& stripsMedian){
  double median=0;
  //  if(onestrip_tmp.size()==0) stripsMedian.push_back(median);
  //  else{
  std::sort(onestrip_tmp.begin(),onestrip_tmp.end());
  int index1=(int)(onestrip_tmp.size()/4.);
  //   int index1=int(floor(onestrip_tmp.size()/4.));
  //   int index2=int(floor(onestrip_tmp.size()/2.));
  int index2=(int)(onestrip_tmp.size()/2.);
  int index3=(int) (3.*onestrip_tmp.size()/4.);
  //   int index3=int(floor(3*onestrip_tmp.size()/4.));
  if(onestrip_tmp.size()>0){
      if ((int)onestrip_tmp.size() <= index1 || (int)onestrip_tmp.size() <= index2 || (int)onestrip_tmp.size() <= index3){
	throw dqm_core::BadConfig( ERS_HERE, name, "out of onestrip_tmp range " );
      }

      median = (onestrip_tmp[index1]+onestrip_tmp[index2]+onestrip_tmp[index3])/3.0;
    }
  //   median = (onestrip_tmp[index2]);
  stripsMedian.push_back(median);
  //  }
}

void dqm_algorithms::BinsDiffFromStripMedianOnline::AddToList(const int r0,const int r2,int i,int j,vector<vector<colorbinOnline> > & ColorBinMap, vector<colorbinOnline>& LookAtList){

  if( i-r0<0 || i-r0>=(int)ColorBinMap.size())  return;
  if( j-r2<0 || j-r2>=(int)ColorBinMap[i-r0].size() ) return;

  vector<colorbinOnline> tmp;

  if(i-1-r0>=0 && i-1-r0<(int)ColorBinMap.size()){
    if(j-1-r2>=0 && j-1-r2<(int)ColorBinMap[i-1-r0].size()){
      if(ColorBinMap[i-1-r0][j-1-r2].m_status==1){
	tmp.push_back(ColorBinMap[i-1-r0][j-1-r2]);
	ColorBinMap[i-1-r0][j-1-r2].m_status=0;
      }
    }
  }
  if(i-r0>=0 && i-r0<(int)ColorBinMap.size()){
    if(j-1-r2>=0 && j-1-r2<(int)ColorBinMap[i-r0].size()){
      if(ColorBinMap[i-r0][j-1-r2].m_status==1){
	tmp.push_back(ColorBinMap[i-r0][j-1-r2]);
	ColorBinMap[i-r0][j-1-r2].m_status=0;
      }
    }
  }

  if(i+1-r0>=0 && i+1-r0<(int)ColorBinMap.size()){
    if(j-1-r2>=0 && j-1-r2<(int)ColorBinMap[i+1-r0].size()){
      if(ColorBinMap[i+1-r0][j-1-r2].m_status==1){
	tmp.push_back(ColorBinMap[i+1-r0][j-1-r2]); 
	ColorBinMap[i+1-r0][j-1-r2].m_status=0;
      }
    }
  }

  if(i-1-r0>=0 && i-1-r0<(int)ColorBinMap.size()){
    if(j-r2>=0 && j-r2<(int)ColorBinMap[i-1-r0].size()){
      if(ColorBinMap[i-1-r0][j-r2].m_status==1){
	tmp.push_back(ColorBinMap[i-1-r0][j-r2]);
	ColorBinMap[i-1-r0][j-r2].m_status=0;
      }
    }
  }


  if(i+1-r0>=0 && i+1-r0<(int)ColorBinMap.size()){
    if(j-r2>=0 && j-r2<(int)ColorBinMap[i+1-r0].size()){
      if(ColorBinMap[i+1-r0][j-r2].m_status==1){
	tmp.push_back(ColorBinMap[i+1-r0][j-r2]);
	ColorBinMap[i+1-r0][j-r2].m_status=0;
      }
    }
  }


  if(i-1-r0>=0 && i-1-r0<(int)ColorBinMap.size()){
    if(j+1-r2>=0 && j+1-r2<(int)ColorBinMap[i-1-r0].size()){
      if(ColorBinMap[i-1-r0][j+1-r2].m_status==1){
	tmp.push_back(ColorBinMap[i-1-r0][j+1-r2]);
	ColorBinMap[i-1-r0][j+1-r2].m_status=0;
      }
    }
  }

  if(i-r0>=0 && i-r0<(int)ColorBinMap.size()){
    if(j+1-r2>=0 && j+1-r2<(int)ColorBinMap[i-r0].size()){
      if(ColorBinMap[i-r0][j+1-r2].m_status==1){
	tmp.push_back(ColorBinMap[i-r0][j+1-r2]);
	ColorBinMap[i-r0][j+1-r2].m_status=0;
      }
    }
  }

  if(i+1-r0>=0 && i+1-r0<(int)ColorBinMap.size()){
    if(j+1-r2>=0 && j+1-r2<(int)ColorBinMap[i+1-r0].size()){
      if(ColorBinMap[i+1-r0][j+1-r2].m_status==1){
	tmp.push_back(ColorBinMap[i+1-r0][j+1-r2]);
	ColorBinMap[i+1-r0][j+1-r2].m_status=0;
      }
    }
  }

  for(unsigned int k=0;k<tmp.size();k++){
    if(tmp[k].m_color!=green){
      LookAtList.push_back(tmp[k]);
      AddToList(r0,r2,tmp[k].m_ix,tmp[k].m_iy,ColorBinMap,LookAtList);
    }
  }
  return;

}

double dqm_algorithms::BinsDiffFromStripMedianOnline::CalEta(vector<colorbinOnline>& LookAtList){
  double sumEta=0;
  double sumN=0;
  for(unsigned int i=0;i<LookAtList.size();i++){
    double eta = LookAtList[i].m_eta;
    double n = LookAtList[i].m_value;
    sumEta += n*eta;
    sumN +=n;
  }
  if(sumN!=0){
    return sumEta/sumN;
  }else{
    return -99.;
  }
}

double dqm_algorithms::BinsDiffFromStripMedianOnline::CalPhi(vector<colorbinOnline>& LookAtList){
  double sumPhi=0;
  double sumN=0;
  for(unsigned int i=0;i<LookAtList.size();i++){
    double phi = LookAtList[i].m_phi;
    double n = LookAtList[i].m_value;
    sumPhi += n*phi;
    sumN +=n;
  }
  if(sumN!=0){
    return sumPhi/sumN;
  }else{
    return -99.;
  }
}

double dqm_algorithms::BinsDiffFromStripMedianOnline::CalVal(vector<colorbinOnline>& LookAtList){
  double sumN=0;
  for(unsigned int i=0;i<LookAtList.size();i++){
    double n = LookAtList[i].m_value;
    sumN += n;
  }
  return sumN;
}
double dqm_algorithms::BinsDiffFromStripMedianOnline::CalR(vector<colorbinOnline>& LookAtList,double eta, double phi){
  if(LookAtList.size()<2) return 0;
  double maxR=0;
  for(unsigned int i=0;i<LookAtList.size();i++){
    double distance = sqrt(pow((LookAtList[i].m_eta-eta),2)+pow((LookAtList[i].m_phi-phi),2));
    maxR=distance>maxR?distance:maxR;
  }
  return maxR;
}

dqm_algorithms::BinsDiffFromStripMedianOnline::colorclusterOnline
dqm_algorithms::BinsDiffFromStripMedianOnline::MakeClusterOnline(const std::string &  name,const int r0,const int r2,binOnline& onebin, vector<vector<colorbinOnline> > & ColorBinMap){
  colorclusterOnline onecluster={0,0,0,0,green,-1};

  if ((int)ColorBinMap.size() <= (onebin.m_ix-r0) ){
    throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
  }

  if ((int)ColorBinMap[onebin.m_ix-r0].size() <= (onebin.m_iy-r2) ){
    throw dqm_core::BadConfig( ERS_HERE, name, "out of ColorBinMap range " );
  }

  if(ColorBinMap[onebin.m_ix-r0][onebin.m_iy-r2].m_status==0)
    return onecluster;
  vector<colorbinOnline> LookAtList;
  if(ColorBinMap[onebin.m_ix-r0][onebin.m_iy-r2].m_color!=green){
    LookAtList.push_back(ColorBinMap[onebin.m_ix-r0][onebin.m_iy-r2]);
    ColorBinMap[onebin.m_ix-r0][onebin.m_iy-r2].m_status=0;
    AddToList(r0,r2,onebin.m_ix,onebin.m_iy,ColorBinMap, LookAtList);
    if(LookAtList.size()>1){
      onecluster.m_size = LookAtList.size();
      onecluster.m_value = CalVal(LookAtList);
      if(ColorBinMap[onebin.m_ix-r0][onebin.m_iy-r2].m_color==red){
        onecluster.m_color = red;
      }else{
	onecluster.m_color = yellow;
      }
      onecluster.m_eta = CalEta(LookAtList);
      onecluster.m_phi = CalPhi(LookAtList);
      onecluster.m_radius = CalR(LookAtList,onecluster.m_eta,onecluster.m_phi);
    } else {
      ColorBinMap[onebin.m_ix-r0][onebin.m_iy-r2].m_status=1;
    }
  }
  return onecluster;
}

void dqm_algorithms::BinsDiffFromStripMedianOnline::printDescription(std::ostream& out){
  
  out<<"BinsDiffFromStripMedianOnline: Calculates smoothed strip median and then find out bins which are aliens "<<std::endl;
  
  out<<"Mandatory Green/Red Threshold is the value of outstandingRatio=(bin value)/(strip median) based on which to give Green/Red result\n"<<std::endl;
  
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: MinStatPerstrip: Minimum strip statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: ignoreval0: values to be ignored for being processed"<<std::endl; 
  out<<"Optional Parameter: ignoreval1: values to be ignored for being processed"<<std::endl; 
  out<<"Optional Parameter: PublishBins: Save bins which are different from average in Result (on:1,off:0,default is 1)"<<std::endl; 
  out<<"Optional Parameter: MaxPublish: Max number of bins to save (default 20)"<<std::endl; 
  out<<"Optional Parameter: VisualMode: is to make the evaluation process similar to the shift work, so one will get resonable result efficiently."<<std::endl;
  
  out<<"Optional Parameter: PublishRedBins:  Max number of red bins to save."<<std::endl;
  out<<"Optional Parameter: ClusterResult: to cluster close bad bins together."<<std::endl;
  out<<"Optional Parameter: SuppressFactor: if the ratio of the bin contennt to max one in the histogram is smaller than SuppressFactor, don't set the bin as red or yellow ."<<std::endl;
  out<<"Optional Parameter: SuppressRedFactor: if the ratio of the bin contennt to max one in the histogram is smaller than SuppressRedFactor, don't set the bin as red ."<<std::endl;
  out<<"Optional Parameter: OnlineMode: switch on when running online."<<std::endl;
  out<<"Optional Parameter: Nred_red: minimum number of red bins needed to label the histogram as red."<<std::endl;
  out<<"Optional Parameter: Nyellow_yellow: minimum number of yellow bins needed to label the histogram as yellow."<<std::endl;
  out<<"Optional Parameter: Nred_yellow: minimum number of red bins needed to label the histogram as yellow."<<std::endl;
  out<<"Optional Parameter: Nredyellow_yellow: minimum number of yellow+red bins needed to label the histogram as yellow."<<std::endl;
  
}
