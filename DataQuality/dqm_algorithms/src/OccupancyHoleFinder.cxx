/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file 
 * \author Justin Griffiths
 */

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/OccupancyHoleFinder.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <TH1.h>
#include <TH2.h>
#include <TClass.h>
#include <ers/ers.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include <sstream>
#include <cstdlib>

#include <dqm_core/AlgorithmManager.h>


namespace {
  dqm_algorithms::OccupancyHoleFinder MDT( "MDT" );
  dqm_algorithms::OccupancyHoleFinder Default("");
}

dqm_algorithms::OccupancyHoleFinder::OccupancyHoleFinder(const std::string &n)
  : m_name(n)
{

  if(m_name.size()) dqm_core::AlgorithmManager::instance().registerAlgorithm( m_name+"_OccupancyHoleFinder", this );
  else dqm_core::AlgorithmManager::instance().registerAlgorithm( "OccupancyHoleFinder", this );
}

dqm_algorithms::OccupancyHoleFinder::~OccupancyHoleFinder()
{
}

dqm_algorithms::OccupancyHoleFinder * 
dqm_algorithms::OccupancyHoleFinder::clone()
{
  return new OccupancyHoleFinder(m_name);
}


dqm_core::Result *
dqm_algorithms::OccupancyHoleFinder::execute(	const std::string & name, 
						const TObject & object, 
                                                const dqm_core::AlgorithmConfig & config )
{

  const TH2* histo = dynamic_cast<const TH2*> (&object);
  if (!histo) {
    throw dqm_core::BadConfig(ERS_HERE, name, "Input object is not a TH2");
  }
  TH1* histo_medians = getMedian(histo);
  int active_lbns = 0;
  //map of chamber bin number by number of lbns that it is dead 
  //more precisely the key is a y-axis bin number, while
  //the value is a list of lbns in which the chamber was dead
  std::map<int, std::vector<int> > dead_chambers_map;
  //map of lbn by number of dead chambers
  std::map<int, int> lbn_occupancy;

  double median_threshold;
  double min_median;
  //double gthreshold; // currently unused
  double rthreshold;
  try {
    median_threshold = dqm_algorithms::tools::GetFirstFromMap( "MedianThreshold", config.getParameters() );
    min_median = dqm_algorithms::tools::GetFirstFromMap( "MinMedian", config.getParameters() );
    rthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getRedThresholds() );
    //gthreshold = dqm_algorithms::tools::GetFromMap( "NBins", config.getGreenThresholds() );
  }
  catch ( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
  //if percentage_alive > suppress_thresh, don't print to display
  double suppress_thresh=0;
  //this should be a bool, if true, then don't print info on  web-display
  double suppress_chamber=0;
  try{
    suppress_thresh = dqm_algorithms::tools::GetFirstFromMap( "SuppressThresh", config.getParameters() );
    suppress_chamber = dqm_algorithms::tools::GetFirstFromMap( "SuppressChamber", config.getParameters() );
  }
  catch ( dqm_core::Exception & ex ) {
    suppress_thresh=0;
    suppress_chamber=0;
  }

  for(int ibinx = 1; ibinx != histo->GetNbinsX()+1; ++ibinx){
    float median = histo_medians->GetBinContent(ibinx);
    //ignore strips where there are very few statistics
    //can configure 'min_median'
    if(median < min_median) {
      lbn_occupancy[ibinx] = histo->GetNbinsY();
      active_lbns++;
      continue;
    }
    active_lbns++;
    int ndead_chambers = 0;
    for(int ibiny = 1; ibiny != histo->GetNbinsY()+1; ++ibiny){
      float content = histo->GetBinContent(ibinx, ibiny);
      //median_threshold is the percentage of the median strip 
      //content that an individual bin should satisfy
      if(content < median*median_threshold) {
	ndead_chambers++;
	std::map<int, std::vector<int> >::iterator itr = dead_chambers_map.find(ibiny);
	if(itr == dead_chambers_map.end()) {
	  std::vector<int> lbns;
	  lbns.push_back(ibinx);
	  dead_chambers_map.insert( std::make_pair(ibiny, lbns) );
	}
	else (*itr).second.push_back( ibinx );
      }
      lbn_occupancy[ibinx] = ndead_chambers;
    }//ybins
  }//xbins
      
  //Analyze chambers that died during run
  dqm_core::Result* result = new dqm_core::Result();
  std::map<std::string,double> tags;
  //ensure that we do not write too much info to the webpage
  int writes = 0;
  if(suppress_chamber==false){
    for(std::map<int, std::vector<int> >::const_iterator itr = dead_chambers_map.begin(); itr != dead_chambers_map.end(); ++itr){
      const std::vector<int> &lbns = (*itr).second;
      std::string chamber_name = getChamberName(histo, (*itr).first);
      if(writes>100) break;
      if( (int) lbns.size() == active_lbns ){
	//      out << "Completely dead chamber " << chamber_name << std::endl;;
	tags[TString::Format("Dead %s", chamber_name.c_str()).Data()] = -1;
	writes++;
      }
      else {
	writes++;
	tags[TString::Format("N_lbns %s Dead", chamber_name.c_str()).Data()] = (*itr).second.size()*histo->GetXaxis()->GetBinWidth(1);
	std::stringstream ss;
	int start = -1;
	int last_binx = start;
	for(unsigned int ii = 0; ii != lbns.size(); ++ii){
	  int ibinx = lbns[ii];
	  if(start == -1) {
	    start = lbns[ii];
	    last_binx = start;
	    ss << histo->GetXaxis()->GetBinLowEdge(ibinx) << " - ";
	    continue;
	  }
	  if(ibinx > last_binx+1){
	    ss << histo->GetXaxis()->GetBinLowEdge(last_binx)+histo->GetXaxis()->GetBinWidth(last_binx) << ", ";
	    ss << histo->GetXaxis()->GetBinLowEdge(ibinx) << " - ";
	    start = ibinx;
	    last_binx = start;
	    continue;
	  }
	  if(ii+1 == lbns.size()){
	    //last element
	    ss << histo->GetXaxis()->GetBinLowEdge(ibinx)+histo->GetXaxis()->GetBinWidth(ibinx) << ", ";
	  }
	  last_binx = ibinx;
	}
	//out << "Chamber " << chamber_name << " : " << ss.str() << std::endl;
      }//else 
    }
  }//suppress_chamber
  
  int dead_lbns = 0;//number of strips where occupancy < 90% (configurable)
  int start_bin_id = -1;
  //int last_bin_id = start_bin_id; // currently unused (because last_lbn is currently unused)
  int last_ndead = 0;
  int counter = 0;//current entry in lbn_occupancy
  for(std::map<int, int>::const_iterator itr = lbn_occupancy.begin(); itr != lbn_occupancy.end(); ++itr, ++counter){
    int bin_id = (*itr).first;
    int current_lbn = histo->GetXaxis()->GetBinLowEdge(bin_id);
    int start_lbn = 0;
    if(start_bin_id > -1) start_lbn = histo->GetXaxis()->GetBinLowEdge(start_bin_id);
    else start_lbn = 1;
    //int last_lbn = 0; // currently unused
    //if(last_bin_id > -1) last_lbn = histo->GetXaxis()->GetBinLowEdge(last_bin_id);
    //else last_bin_id = 1;
    int lbn_width = histo->GetXaxis()->GetBinWidth(bin_id);

    float percentage_alive = 100*(1-(*itr).second*1./histo->GetNbinsY());
    float last_percentage_alive = 100*(1-last_ndead*1./histo->GetNbinsY());
    //bool good_lbn(true); // currently unused
    //A value of dead_lbns>0 causes a red flag.
    if(percentage_alive < rthreshold){
      if((*itr).second != histo->GetNbinsY()) dead_lbns++; //do not flag red when entire strip is empty--likely this is not a problem
      //good_lbn = false;
    //tags[TString::Format("%i-%i", current_lbn, current_lbn+bin_width).Data()] = percentage_alive;
    }

    //Attempt to write out in format:
    //lbn_i-lbn_j XX% live
    if(start_bin_id == -1) {
      start_bin_id = bin_id;
      //last_bin_id = start_bin_id;
      last_ndead = (*itr).second;
      continue;
    }   

    if(last_ndead != (*itr).second || counter+1 == (int) lbn_occupancy.size()){
      if(writes<100){
	if(counter+1 == (int) lbn_occupancy.size() && percentage_alive < suppress_thresh) {
	  //This should happen on the last strip
	  tags[TString::Format("%i-%i", start_lbn, current_lbn+lbn_width).Data()] = percentage_alive;
	}
	else if(last_percentage_alive < suppress_thresh) {
	  //This happens when the current number of dead bins differs from the last number of dead bins
	  //Thus we write out the previous percentage at this point
	  tags[TString::Format("%i-%i", start_lbn, current_lbn-1).Data()] = last_percentage_alive;
	}
	start_bin_id = bin_id;
	//last_bin_id = bin_id;
	last_ndead = (*itr).second;
	writes++;
      }
    }
    //last_bin_id = bin_id;
  }
  if(dead_lbns) {
    result->status_ = dqm_core::Result::Red;
  }
  else result->status_ = dqm_core::Result::Green;
  
  result->tags_ = tags;
  return result;
  
}

void
dqm_algorithms::OccupancyHoleFinder::printDescription(std::ostream& /*out*/)
{
  
//   out<<"OccupancyHoleFinder: Checks number of bins N sigma away from reference histogram bin value or given Value\n"<<std::endl;
//   out<<"Simply Dump the Bin Number/Name of Bin with bin contents as result" << std::endl;
}

//return histogram with median in each vertical strip
TH1* 
dqm_algorithms::OccupancyHoleFinder::getMedian(const TH2* histo)
{
  TH1F* h = new TH1F( TString::Format("%s_median", histo->GetName()),"", histo->GetNbinsX(), histo->GetXaxis()->GetXmin(), histo->GetXaxis()->GetXmax() );
  
  for(int ibinx = 1; ibinx != histo->GetNbinsX()+1; ++ibinx){
    std::vector<float> y_bin_vals;
    for(int ibiny = 1; ibiny != histo->GetNbinsY()+1; ++ibiny){
      y_bin_vals.push_back(histo->GetBinContent(ibinx, ibiny));
      //out << "THIS CHAMBER :" << ibiny << ": " << getChamberName(histo, ibiny) << std::endl;
    }
    std::sort( y_bin_vals.begin(), y_bin_vals.end());
    int size = y_bin_vals.size();
    if(size == 0) {
      h->SetBinContent(ibinx, 0);
      continue;
    }
    float median = (size%2 == 0 ? (y_bin_vals[size/2-1]+y_bin_vals[size/2])/2 : y_bin_vals[size/2]);
    h->SetBinContent(ibinx, median);
  }
  return h;
}

std::string
dqm_algorithms::OccupancyHoleFinder::getChamberName(const TH2* histo, int biny){

  if(m_name == "MDT") return getMDTChamberName(histo, biny);
  if(std::string(histo->GetYaxis()->GetBinLabel(biny)).size()) return histo->GetYaxis()->GetBinLabel(biny);
  std::stringstream ss;
  ss << biny;
  return ss.str();
}

std::string
dqm_algorithms::OccupancyHoleFinder::getMDTChamberName(const TH2* histo, int biny){

  std::string name = histo->GetYaxis()->GetBinLabel(biny);
  
  int count = 0;
  while(name.size() == 0 && biny-count >= 1){
    name = histo->GetYaxis()->GetBinLabel(biny-count);
    if(name.size() == 0) count++;
  }

  std::string hname = histo->GetName();
  char side = hname[15];
  if(name == "BO0" && count == 2){
    //hack around aesthetic choice
    name = "BO1";
    count = 0;
  }
  else if(name == "BO1" && side == 'A'){
    //hack around aesthetic choice
    count++;
  }

  int phiStat = count+1;
  char stat_type = '0';
  int etaStat = atoi(&name[2]);
  if(name[0] == 'B'){
    if(name[1] == 'E') {
      stat_type = 'E';
      phiStat*=2;
    }
    else if(name[1] == 'I') {
      if( etaStat <= 5){
	if(phiStat%2 == 0 && phiStat <= 10) stat_type = 'S';
	else if(phiStat%2 == 1 && phiStat <= 10) stat_type = 'L';
	else if(phiStat == 11 || phiStat == 16) stat_type = 'R';
	else if(phiStat == 12 || phiStat == 17) stat_type = 'M';
	else if(phiStat == 13 || phiStat == 15 || phiStat == 18) stat_type = 'S';
	else if(phiStat == 14) stat_type = 'L';
	//fix phiStat
	if (phiStat > 16) phiStat-=2;
	else if (phiStat <=16 && phiStat > 11) phiStat--;
      }
      else if(etaStat == 6){
	if(phiStat%2 == 0) stat_type = 'S';
	else if(phiStat%2 == 1) stat_type = 'L';
	if( phiStat == 11 || phiStat == 15) stat_type = 'R';
      }
      else {
	stat_type = 'S';
	phiStat*=2;
      }
    }
    else if(name[1] == 'M') {
      if(etaStat <= 3){
	if(phiStat%2==0) stat_type = 'S';
	else stat_type = 'L';
	if(phiStat == 12 || phiStat == 14) stat_type = 'F';
      }
      else if(etaStat <= 5){
	if(phiStat%2 == 0) stat_type = 'S';
	else if(phiStat%2 == 1) stat_type = 'L';
	//fix phiStat
	if(phiStat>=13) phiStat+=2;
	if(phiStat==12) { stat_type = 'L'; phiStat++; }
      }
      else {
	if(phiStat%2 == 0) stat_type = 'S';
	else if(phiStat%2 == 1) stat_type = 'L';
	if(phiStat == 12) {stat_type = 'L'; phiStat=15;}
	if(phiStat == 13) {stat_type = 'S'; phiStat=16;}
      }
    }
    else {//'O'
      if( etaStat == 0 ) {
	stat_type = 'G';
	phiStat = (phiStat == 1? 12 : 14);
	side = 'B';
      }
      else if(etaStat <=6){
	if(phiStat%2==0) stat_type='S';
	else stat_type = 'L';

	if(etaStat%2==0 && (phiStat==12||phiStat==14) ) stat_type = 'G';
	else if(etaStat%2==1 && (phiStat==12||phiStat==14)) stat_type = 'F';
      }
      else {
	etaStat = (phiStat > 2? 8 : 7);
	stat_type = (etaStat== 7 ? 'F' : 'G');
	phiStat = (phiStat%2 == 1? 12 : 14);
      }
    }
    
  }
  else{//Endcap
    if(name[1] == 'E'){
      stat_type = 'L';
      etaStat = (phiStat > 3? 2: 1);
      if(side == 'A'){
	if(phiStat == 1) phiStat = 5;
	else if(phiStat%2 == 0) phiStat = 11;
	else phiStat = 13;
      }
      else{
	if(phiStat == 1) phiStat = 5;
	else if(phiStat%2==0) phiStat = 13;
	else phiStat = 15;
      }
    }
    else if(name[1] == 'I'){
      if(etaStat<=2){
	if(phiStat%2==0) stat_type = 'S';
	else if(phiStat%2==1) stat_type = 'L';	
      }
      else if(etaStat<=4){
	stat_type='L';
	phiStat*=2;
	phiStat--;
      }
      else{
	stat_type='L';
	phiStat = (phiStat == 1? 1 : 9);
      }
    }
    else {// 'M' or 'O'
      if(phiStat%2==0) stat_type = 'S';
      else stat_type = 'L';
    }
  }
  
  std::string phiStat_str = TString::Format("%i", phiStat).Data();
  if(phiStat_str.size() == 1) phiStat_str = std::string("0")+phiStat_str;
  std::string etaStat_c = TString::Format("%i", etaStat).Data();
  name = name.substr(0,2);
  name+=stat_type;
  name+=etaStat_c;
  name+=side;
  name+=phiStat_str;
  return name;
}
