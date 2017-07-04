/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*  KillBinsByStrip.cxx
    Selects out outlier bins from a 2D histogram, by strip, by gradually re-calculating the strip average without those bins.
    Assumes that y-axis (phi coordinates) is symmetric from detector design.
    Author: Olivier Simard (CEA-Saclay)
    Email:  Olivier.Simard@cern.ch
*/

#include <dqm_core/AlgorithmConfig.h>
#include <dqm_algorithms/KillBinsByStrip.h>
#include <dqm_algorithms/tools/AlgorithmHelper.h>
#include <dqm_core/AlgorithmManager.h>

#include <TH1.h>
#include <TH2.h>
#include <TH1D.h>
#include <TClass.h>

using namespace std;

bool mySortfunc(bin2 i,bin2 j){return (i.m_value > j.m_value);}
bool mySortfunc_ratio(bin2 i, bin2 j){return (i.m_deviation> j.m_deviation);}
static dqm_algorithms::KillBinsByStrip myInstance;

//_________________________________________________________________________________________
dqm_algorithms::KillBinsByStrip::KillBinsByStrip(){ dqm_core::AlgorithmManager::instance().registerAlgorithm("KillBinsByStrip", this); }
dqm_algorithms::KillBinsByStrip::~KillBinsByStrip(){ }
dqm_algorithms::KillBinsByStrip* dqm_algorithms::KillBinsByStrip::clone(){ return new KillBinsByStrip(); }

//_________________________________________________________________________________________
dqm_core::Result* dqm_algorithms::KillBinsByStrip::execute(const std::string& name,const TObject& object,const dqm_core::AlgorithmConfig& config)
{
  // Runs KillBinsByStrip algorithm on the 2D-histogram provided in 'object'.

  TH2* histogram = NULL;
  if( object.IsA()->InheritsFrom("TH2") ){
    histogram = (TH2*)&object;
    if(histogram->GetDimension() != 2 ){ throw dqm_core::BadConfig( ERS_HERE, name, "Not a 2D-histogram" ); }
  } else { throw dqm_core::BadConfig( ERS_HERE, name, "does not inherit from TH1/TH2" ); }
 
  const double minstat = dqm_algorithms::tools::GetFirstFromMap( "MinStat", config.getParameters(), -1);
  //const double ignoreval = dqm_algorithms::tools::GetFirstFromMap( "ignoreval", config.getParameters(), -99999);
  const bool publish = (bool) dqm_algorithms::tools::GetFirstFromMap( "PublishBins", config.getParameters(), 1);
  const int Nmaxpublish = (int) dqm_algorithms::tools::GetFirstFromMap( "MaxPublish", config.getParameters(), 20);
  const bool VisualMode = (bool) dqm_algorithms::tools::GetFirstFromMap( "VisualMode", config.getParameters(), 0);
  const int NpublishRed = (int) dqm_algorithms::tools::GetFirstFromMap( "PublishRedBins",config.getParameters(), 0);
 
  if (histogram->GetEntries() < minstat) {
    dqm_core::Result *result = new dqm_core::Result(dqm_core::Result::Undefined);
    result->tags_["InsufficientEntries"] = histogram->GetEntries();
    return result;
  }
 
  double gthreshold = 0., rthreshold = 0.;
  try {
    rthreshold = dqm_algorithms::tools::GetFromMap( "MaxDeviation", config.getRedThresholds() );
    gthreshold = dqm_algorithms::tools::GetFromMap( "MaxDeviation", config.getGreenThresholds() );
  } catch( dqm_core::Exception & ex ) {
    throw dqm_core::BadConfig( ERS_HERE, name, ex.what(), ex );
  }
 
  // get bin limits
  std::vector<int> range = dqm_algorithms::tools::GetBinRange(histogram,config.getParameters());
  dqm_core::Result* result = new dqm_core::Result();
  vector<bin2> redbins;
  vector<bin2> yellowbins;
  vector<bin2> Allbins;
  
  // objects needed before loop
  TH1D* projected_strip = NULL;
  char hname[56];
  int ix,iy;
  int binphi,biny,binz;
  int nmax = 0, nbins = 0;
  double binval = 0., maxval = 0.;
  //double average = 0.
  double stripavg = 0.,striperr = 0.,striperr2 = 0.,testval = 0.;

  // Some parameters set by hand: move to jobOption
  int poissonLimit = 5;
  

  // loop over all strips (eta or x-axis)
  for(ix = range[0]; ix <= range[1]; ++ix){

    double eta = histogram->GetXaxis()->GetBinCenter(ix);

    // make up some name for the 1D-projection so that ROOT::TName does not complain
    sprintf(hname,"%s_py_bin%d",histogram->GetName(),ix);
    projected_strip = histogram->ProjectionY(hname,ix,ix); // single strip projection
    nmax = 0;
    nbins = 0;


    // then within each strip remove bins until the deviation stabilizes.
    // note that because the maximum can be anywhere in the strip, one cannot
    // loop systematically over all bins. we rather search for the maximum and test it directly.
    // Here "TH1::GetMaximumBin()" is used but one can replace it by anything that
    // would be more efficient or faster.
    while(true){

      striperr = 0.;
      striperr2 = 0.;

      // number of bins with data in the strip (does not include previous maxima, if nmax>0)
      if(nmax==0) for(iy=1;iy<=projected_strip->GetXaxis()->GetNbins();iy++) if(projected_strip->GetBinContent(iy)>0) nbins += 1;

      // identify current maximum in the strip
      projected_strip->GetMaximumBin(binphi,biny,binz); // projected x-axis means the bin in phi (y-axis)
      maxval = projected_strip->GetBinContent(binphi); 
      binval = maxval;

      // get out of this loop if the strip is empty or when we have emptied the strip
      if(nbins<=0) break;

      // arithmetic mean for this strip
      stripavg = projected_strip->Integral()/(double)nbins;

      // error on the mean: we might have to consider switching to Poisson if the number of remaining bins is low
      if(nbins<=poissonLimit){

        // Poisson error calculated from mean
        striperr = sqrt(stripavg);

      } else {

        // calculate sigma^2
        for(iy=1;iy<=projected_strip->GetXaxis()->GetNbins();iy++) if(projected_strip->GetBinContent(iy)>0) striperr2 += pow((projected_strip->GetBinContent(iy)-stripavg),2);
        // calculate error on the mean
        striperr = sqrt(striperr2)/sqrt((double)nbins);

      }
        
      // calculate deviation from stripavg
      if(striperr>0.) testval = fabs(maxval-stripavg)/striperr;
      else testval = 0.;

      // decision
      bool die = false;
      double phi = projected_strip->GetXaxis()->GetBinCenter(binphi);
      bin2 onebin = {eta,phi,ix,binphi,binval,testval};
      if(testval > rthreshold) redbins.push_back(onebin);
      else if(testval > gthreshold) yellowbins.push_back(onebin);
      else { // no problem, write that bin but then exits
        Allbins.push_back(onebin);
        die = true;
      }

      // if the while-loop is not broken, keep looking for secondary maxima after
      // removing the maximum that was just found.
      // if there are any other reason to stop testing one should implement it here
      if(die) break;

      // apply KillBin method - remove the content of latest offending-bin
      projected_strip->SetBinContent(binphi,0); // kill bin
      nmax += 1;  // register one more maximum in the strip
      nbins -= 1; // remove one count from strip

    } // while

    delete projected_strip;

  } // for(ix)

  
  // The following is more or less the same.
  std::sort(redbins.begin(),redbins.end(),mySortfunc);
  std::sort(yellowbins.begin(),yellowbins.end(),mySortfunc);
  std::sort(Allbins.begin(),Allbins.end(),mySortfunc_ratio);
  char tmpstr[500];
  int count_red=0,count_yellow=0;  

  // publish red bins
  for(unsigned int i=0;i<redbins.size();i++){
    if(VisualMode) continue;
    if(publish){
      sprintf(tmpstr,"R%i-(eta,phi)[OSRatio]=(%0.3f,%0.3f)[%0.2e]",count_red,redbins[i].m_eta,redbins[i].m_phi,redbins[i].m_deviation);
      std::string tag = tmpstr;
      result->tags_[tag] = redbins[i].m_value;
    }  
    count_red++;
    if(NpublishRed > 0){
      if(count_red > NpublishRed) break;
    }
  }

  // publish yellow bins
  for(unsigned int i=0;i<yellowbins.size();i++){
    if(VisualMode) continue;
    if(publish && (count_red+count_yellow) < Nmaxpublish ){
      sprintf(tmpstr,"Y%i-(eta,phi)[OSRatio]=(%0.3f,%0.3f)[%.2e]",count_yellow,yellowbins[i].m_eta,yellowbins[i].m_phi,yellowbins[i].m_deviation);
      std::string tag = tmpstr;
      result->tags_[tag] = yellowbins[i].m_value;
    }
    count_yellow++;
  }
  result->tags_["NRedBins"] = count_red;  // count_red is the number of red bins printed
  result->tags_["NYellowBins"] = count_yellow; // count_yellow is the number of yellow bins printed

  if(count_red+count_yellow==0 && Allbins.size()>0){
    for(unsigned int i=0;i<Allbins.size();i++){
      sprintf(tmpstr,"LeadingBin%i-(eta,phi)=(%0.3f,%0.3f)",i,Allbins[i].m_eta,Allbins[i].m_phi);
      std::string tagtag = tmpstr;
      result->tags_[tagtag] = Allbins[i].m_value;
    }
  }
 
  if(count_red>0) result->status_ = dqm_core::Result::Red;
  else if(count_yellow>0) result->status_ = dqm_core::Result::Yellow;
  else result->status_ = dqm_core::Result::Green;

  return result;
}

//_________________________________________________________________________________________
void dqm_algorithms::KillBinsByStrip::printDescription(std::ostream& out)
{
  out<<"KillBinsByStrip: Selects out outlier bins from a 2D histogram, by strip, by gradually re-calculating the strip average without those bins."<<std::endl;
  out<<"                 Assumes that y-axis (phi coordinates) is symmetric from detector design."<<std::endl;
 
  out<<"Optional Parameter: MinStat: Minimum histogram statistics needed to perform Algorithm"<<std::endl;
  out<<"Optional Parameter: ignoreval: valued to be ignored for being processed"<<std::endl;
  out<<"Optional Parameter: PublishBins: Save bins which are different from average in Result (on:1,off:0,default is 1)"<<std::endl;
  out<<"Optional Parameter: MaxPublish: Max number of bins to save (default 20)"<<std::endl;
  out<<"Optional Parameter: VisualMode: is to make the evaluation process similar to the shift work, so one will get resonable result efficiently."<<std::endl;
  return;
}

