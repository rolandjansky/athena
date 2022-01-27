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
	//      out << "Completely dead chamber " << chamber_name << std::endl;
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
  char char16 = histo->GetName()[16];
  std:: string hname=histo->GetName();
  std::size_t found = hname.find("ontrack");
  std::string crate;
  if (found!=std::string::npos){
    char16 = histo->GetName()[24];
    crate = hname.substr(22,4);        
  }  else        crate = hname.substr(14,4);	
  if(m_name == "MDT" && char16 != '0' ) return getMDTChamberName(histo, biny);
  if(m_name == "MDT" && char16 == '0' ) return getMDTChamberNameByCrate(biny, crate);
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
        if(phiStat%2==0) stat_type = 'S';
        if(phiStat%2==1) stat_type = 'L';
        if(etaStat==6 && phiStat>12) phiStat = phiStat+1; //there is no phi=13 in eta=6, therefore phi=14,15,16
                                                          //was moved one bin down in the histogram we are looking at 
        if(phiStat==12 || phiStat==14){ //BMF and BMG
            if(etaStat%2==0) stat_type='G';
            //BMF naming does not correspond to the actual eta station. BMF eta station is 1,3,5 but the chambers are named
            //BMF1, BMF2, BMF3. We set etaStat such that the proper name comes out at the end.
            if(etaStat%2==1) {
                stat_type = 'F';
                if(etaStat==3) etaStat=2;
                if(etaStat==5) etaStat=3;
            }
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
std::string
dqm_algorithms::OccupancyHoleFinder::getMDTChamberNameByCrate(int biny, std::string crate){

  int phiStat = -99;
  int etaStat = -99;
  std::string chamber_str = "xxx";

  if(crate.substr(0,1) == 'B' && crate.substr(2,2) == "01"){
	if( (biny-61) > 0){ // BOS
		chamber_str = "BOS";
		etaStat = (biny+1 - 61)/2;
		phiStat =  2*(biny - 2*etaStat - 61) +4;
	} else if( (biny-49) > 0){ //BOL
		chamber_str = "BOL";
		etaStat = (biny+1 - 49)/2;
		phiStat =  2*(biny - 49 - 2*etaStat)+ 3;
	} else if( (biny-37) > 0){ //BMS
		chamber_str = "BMS";
		etaStat = (biny+1 - 37)/2;
		phiStat =  2*(biny - 2*etaStat - 37) + 4;
	} else if ( (biny-25) > 0){ //BML
		chamber_str = "BML";
		etaStat = (biny+1 - 25)/2;
		phiStat =  2*(biny - 25 - 2*etaStat) +3;
	} else if ( (biny-24) > 0){ //BME
		chamber_str = "BME";
		etaStat = 1;
		phiStat = 13;
	} else if ( (biny-12) > 0){ //BIS
		chamber_str = "BIS";
		etaStat = (biny+1 - 12)/2;
		phiStat =  2*(biny - 2*etaStat - 12) + 4;
	} else if ( (biny) > 0){ //BIL
		chamber_str = "BIL";
		etaStat = (biny+1)/2;
		phiStat =  2*(biny - 2*etaStat) + 3;
    } 
  }else if (crate.substr(0,1) == "B" && crate.substr(2,2) == "02"){
	if( (biny-60) > 0){ // BOS
		chamber_str = "BOS";
		etaStat = (biny+1 - 60)/2;
		phiStat =  2*(biny - 2*etaStat - 60) + 8;
	} else if ( (biny-48) > 0){ //BOL
		chamber_str = "BOL";
		etaStat = (biny+1 - 48)/2;
		phiStat =  2*(biny - 2*etaStat - 48) + 7;
	} else if ( (biny-36) > 0){ //BMS
		chamber_str = "BMS";
		etaStat = (biny+1 - 36)/2;
		phiStat =  2*(biny - 2*etaStat - 36) + 8;
	} else if ( (biny-24) > 0){ //BML
		chamber_str = "BML";
		etaStat = (biny+1 - 24)/2;
		phiStat =  2*(biny - 2*etaStat - 24) + 7;
	} else if ( (biny-12) > 0){ //BIS
		chamber_str = "BIS";
		etaStat = (biny+1 - 12)/2;
		phiStat =  2*(biny - 2*etaStat - 12) + 8;
	} else if ( (biny) > 0){ //BIL
		chamber_str = "BIL";
		etaStat = (biny+1)/2;
		phiStat =  2*(biny - 2*etaStat) + 7;
    }
  } else if(crate.substr(0,1) == "B" && crate.substr(2,2) == "03"){
	int cOffset = 0;
	if (crate.substr(1,1) == "C"){ cOffset = 1;}
	if( (biny-74 + cOffset) > 0) { // BOS
		chamber_str = "BOS";
		etaStat = (biny - 74 + cOffset);
		phiStat =  10;
	} else if ( (biny-62 + cOffset) > 0){ //BOL
		chamber_str = "BOL";
		etaStat = (biny+1 - 62 + cOffset)/2;
		phiStat =  2*(biny - 2*etaStat - 62 + cOffset) + 11;
	} else if ( (biny - 57 ) > 0){
		chamber_str = "BOG";
		etaStat = 2*(biny - 57) - 2 + 2*cOffset;
		phiStat = 12;
	} else if ( (biny-53) > 0){
		chamber_str = "BOF";
		etaStat = 2*(biny - 53) - 1;
		phiStat = 12;
	} else if( (biny-47) > 0){ // BMS
		chamber_str = "BMS";
		etaStat = biny - 47;
		phiStat =  10;
	} else if ( (biny-35) > 0){ //BML
		chamber_str = "BML";
		etaStat = (biny+1 - 35)/2;
		phiStat =  2*(biny - 2*etaStat - 35) + 11;
	} else if ( (biny - 32) > 0){ //BMG
		chamber_str = "BMG";
		etaStat = (biny - 32)*2 ; //because its BMG2, BMG4, BMG6
		phiStat = 12;
	} else if ( (biny - 29) > 0){ //BMF
		chamber_str = "BMF";
		etaStat = biny - 29;
		phiStat = 12;
	} else if ( (biny-17) > 0){ //BIS
		chamber_str = "BIS";
		etaStat = (biny+1 - 17)/2;
		phiStat =  2*(biny - 2*etaStat - 17) + 12;
	} else if ( (biny-11) > 0){ //BIR
		chamber_str = "BIR";
		etaStat = biny-11;
		phiStat =  11;
	} else if ( (biny-6) > 0){ //BIM
		chamber_str = "BIM";
		etaStat = biny-6;
		phiStat =  11;
	} else if ( (biny) > 0){ //BIL
		chamber_str = "BIL";
		etaStat = biny;
		phiStat =  9;
    }
  } else if(crate.substr(0,1) == "B" && crate.substr(2,2) == "04"){
	int cOffset = 0;
	if (crate.substr(1,1) == "C"){ cOffset = 1;}
	if( (biny-74 + cOffset) > 0) { // BOS
		chamber_str = "BOS";
		etaStat = (biny - 74 + cOffset);
		phiStat =  16;
	} else if ( (biny-61 + cOffset) > 0){ //BOL
		chamber_str = "BOL";
		etaStat = (biny+1 - 61 + cOffset)/2;
		phiStat =  2*(biny - 2*etaStat - 61 + cOffset) + 15;
	} else if ( (biny - 56 ) > 0){
		chamber_str = "BOG";
		etaStat = 2*(biny - 56) - 2 + 2*cOffset;
		phiStat = 14;
	} else if ( (biny-52) > 0){
		chamber_str = "BOF";
		etaStat = 2*(biny - 52) - 1;
		phiStat = 14;
	} else if( (biny-46) > 0){ // BMS
		chamber_str = "BMS";
		etaStat = biny - 43;
		phiStat =  16;
	} else if ( (biny - 45) > 0){
		chamber_str = "BML";
		etaStat = 6;
		phiStat = 15;
	} else if ( (biny-35) > 0){ //BML
		chamber_str = "BML";
		etaStat = (biny+1 - 35)/2;
		phiStat =  2*(biny - 2*etaStat - 35) + 15;
	} else if ( (biny - 32) > 0){ //BMG
		chamber_str = "BMG";
		etaStat = (biny - 32)*2;
		phiStat = 14;
	} else if ( (biny - 29) > 0){ //BMF
		chamber_str = "BMF";
		etaStat = biny - 29;
		phiStat = 14;
	} else if ( (biny-17) > 0){ //BIS
		chamber_str = "BIS";
		etaStat = (biny+1 - 17)/2;
		phiStat =  2*(biny - 2*etaStat - 17) + 16;
	} else if ( (biny-11) > 0){ //BIR
		chamber_str = "BIR";
		etaStat = biny-11;
		phiStat =  15;
	} else if ( (biny-6) > 0){ //BIM
		chamber_str = "BIM";
		etaStat = biny-6;
		phiStat =  15;
	} else if ( (biny) > 0){ //BIL
		chamber_str = "BIL";
		etaStat = biny;
		phiStat =  13;
    }
  } else if(crate.substr(0,1) == "E" && (crate.substr(2,2) == "01" || crate.substr(2,2) == "03") ){
	int cOffset = 0;
	if (crate.substr(2,2) == "03"){ cOffset = 8;}
	if( (biny-61) > 0) { //EOS
		chamber_str = "EOS";
		etaStat = (biny+1 - 61)/2;
		phiStat =  2*(biny - 2*etaStat - 61) + 4 + cOffset;
	} else if ( (biny-49) > 0){ //EOL
		chamber_str = "EOL";
		etaStat = (biny+1 - 49)/2;
		phiStat =  2*(biny - 2*etaStat - 49) + 3 + cOffset;
	} else if ( (biny - 39 ) > 0){ //EMS
		chamber_str = "EMS";
		etaStat = (biny+1 - 39)/2;
		phiStat =  2*(biny - 2*etaStat - 39) + 4 + cOffset;
	} else if ( (biny-29) > 0){ //EML
		chamber_str = "EML";
		etaStat = (biny+1 - 29)/2;
		phiStat =  2*(biny - 2*etaStat - 29) + 3 + cOffset;
	} else if( (biny-25) > 0){ // EIS
		chamber_str = "EIS";
		etaStat = (biny+1 - 25)/2;
		phiStat =  2*(biny - 2*etaStat - 25) + 4 + cOffset;
	} else if ( (biny - 16) > 0){ //EIL
		chamber_str = "EIL";
		etaStat = (biny+1 - 16)/2;
		phiStat =  2*(biny - 2*etaStat - 16) + 3 + cOffset;
	} else if ( (biny-12) > 0){ //EES
		chamber_str = "EES";
		etaStat = (biny+1 - 12)/2;
		phiStat =  2*(biny - 2*etaStat - 12) + 4 + cOffset;
	} else if ( (biny - 8) > 0){ //EEL
		chamber_str = "EEL";
		etaStat = (biny+1 - 8)/2;
		phiStat =  2*(biny - 2*etaStat - 8 ) + 3 + cOffset;
	} else if ( (biny-4) > 0){ //BIS
		chamber_str = "BIS";
		etaStat = (biny+1 - 4)/2 + 6;
		phiStat =  2*(biny - 2*(etaStat-6) - 4) + 4 + cOffset;
	} else if ( (biny) > 0){ //BEE
		chamber_str = "BEE";
		etaStat = (biny+1)/2;
		phiStat =  2*(biny - 2*etaStat) + 4 + cOffset;
    }
  } else if(crate.substr(0,1) == "E" && (crate.substr(2,2) == "02" || crate.substr(2,2) == "04") ){
	int cOffset = 0; int phiOffset = 0;
	if (crate.substr(2,2) == "04"){
	        cOffset = 1; phiOffset = 8;
	}
	if( (biny-59 - cOffset) > 0) { //EOS
		chamber_str = "EOS";
		etaStat = (biny+1 - 59 - cOffset)/2;
		phiStat =  2*(biny - 2*etaStat - 59 - cOffset) + 8 + phiOffset;
	} else if ( (biny-47 - cOffset) > 0){ //EOL
		chamber_str = "EOL";
		etaStat = (biny+1 - 47 - cOffset)/2;
		phiStat =  2*(biny - 2*etaStat - 47 - cOffset) + 7 + phiOffset;
	} else if ( (biny - 37 - cOffset ) > 0){ //EMS
		chamber_str = "EMS";
		etaStat = (biny+1 - 37 - cOffset)/2;
		phiStat =  2*(biny - 2*etaStat - 37 - cOffset) + 8 + phiOffset;
	} else if ( (biny-27 - cOffset) > 0){ //EML
		chamber_str = "EML";
		etaStat = (biny+1 - 27 - cOffset)/2;
		phiStat =  2*(biny - 2*etaStat - 27 - cOffset) + 7 + phiOffset;
	} else if( (biny-23 - cOffset) > 0){ // EIS
		chamber_str = "EIS";
		etaStat = (biny+1 - 23 - cOffset)/2;
		phiStat =  2*(biny - 2*etaStat - 23 - cOffset) + 8 + phiOffset;
	} else if ( (biny - 15 - cOffset) > 0){ //EIL
		chamber_str = "EIL";
		etaStat = (biny+1 - 15 - cOffset)/2;
		phiStat =  2*(biny - 2*etaStat - 15 - cOffset) + 7 + phiOffset;
	} else if ( (biny-11 - cOffset) > 0){ //EES
		chamber_str = "EES";
		etaStat = (biny+1 - 11 - cOffset)/2;
		phiStat =  2*(biny - 2*etaStat - 11 - cOffset) + 8 + phiOffset;
	} else if ( (biny - 8) > 0){ //EEL
		chamber_str = "EEL";
		etaStat = (biny+1 - 8)/2;
		phiStat =  2*(biny - 2*etaStat - 8 ) + 7 + phiOffset;
		if(cOffset == 0 && etaStat == 2){phiStat = 7;}
	} else if ( (biny-4) > 0){ //BIS
		chamber_str = "BIS";
		etaStat = (biny+1 - 4)/2 + 6;
		phiStat =  2*(biny - 2*(etaStat-6) - 4) + 8 + phiOffset;
	} else if ( (biny) > 0){ //BEE
		chamber_str = "BEE";
		etaStat = (biny+1)/2;
		phiStat =  2*(biny - 2*etaStat) + 8 + phiOffset;
	}
  }
  std::string phiStat_str = TString::Format("%i", phiStat).Data();
  if(phiStat_str.size() == 1) phiStat_str = std::string("0")+phiStat_str;
  std::string etaStat_c = TString::Format("%i", etaStat).Data();
  std::string chamber_name = chamber_str + etaStat_c + crate.substr(1,1) + phiStat_str;
  if(chamber_str == "BOG" && etaStat == 0){
	chamber_name = chamber_str + etaStat_c + "B" + phiStat_str;
  }
  return chamber_name;
}
