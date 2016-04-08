/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "cmdline.h"
#include "LumiCalc/LumiCalculator.h"
#include "LumiCalc/CoolQuery.h"
#include "LumiCalc/LumiBlockRangeContainerConverter.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGRLCollection.h"
#include "GoodRunsLists/TMsgLogger.h"
#include "DBDataModel/CollectionMetadata.h"
#include <iomanip>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <stdio.h>
#include <sys/stat.h> 
#include <sys/types.h>

static std::vector<std::string> tagfile;
static std::vector<std::string> aodfile;
static std::vector<std::string> xmlfile;
static std::vector<std::string> rootfile;
static std::string m_treename = "tree";
static std::string m_d3pddirname = "Lumi";
static std::string m_xmloutfile = "out.xml";
static std::vector<std::string> m_triggerchain;
static std::string m_livetrigger;
static Root::TMsgLogger m_logger( "iLumiCalc" );
static std::vector<uint32_t> runnumber;
static std::vector<uint32_t> lbstart;
static std::vector<uint32_t> lbend;
static uint32_t minlbstart = cool::ValidityKeyMin & 0xFFFFFFFF;
static uint32_t maxlbend = (cool::ValidityKeyMax & 0xFFFFFFFF)/2;
static uint32_t minrunnum = (cool::ValidityKeyMin >> 32);
static uint32_t maxrunnum = (cool::ValidityKeyMax >> 32);

//______________________________________________________________
// Mini progress bar implementation, reduced version of
// the one from TMVA/Timer class implementation
class ProgressBar : public TStopwatch {

 public:
  ProgressBar( Int_t, Int_t, const char * prefix = "" );
  ~ProgressBar();
  
  void Reset();
  void DrawProgressBar( Int_t );
  TString SecToText( Double_t );
  Double_t ElapsedSeconds();
  TString GetElapsedTime();
  TString GetLeftTime( Int_t );

  Int_t   pbNbins; 
  Int_t   Ncounts;
  TString Prefix;

};

//_______________________________________________________________________
ProgressBar::ProgressBar(Int_t ncounts, Int_t nbins, const char * prefix)
  : pbNbins(nbins), Ncounts(ncounts), Prefix(TString(prefix))
{
  Reset();
}

//_______________________________________________________________________
ProgressBar::~ProgressBar()
{}

//_______________________________________________________________________
void ProgressBar::Reset() 
{
  TStopwatch::Start( kTRUE );
}

//_______________________________________________________________________
Double_t ProgressBar::ElapsedSeconds() 
{
  // computes elapsed time in seconds
  Double_t rt = TStopwatch::RealTime(); TStopwatch::Start( kFALSE );
  return rt;
}
//_______________________________________________________________________

TString ProgressBar::GetElapsedTime() 
{
  // returns string with elapsed time
  return SecToText( ElapsedSeconds());
}

//_______________________________________________________________________
TString ProgressBar::GetLeftTime( Int_t icounts ) 
{
  // returns pretty string with time left
  Double_t leftTime = ( icounts <= 0 ? -1 :
			icounts > Ncounts ? -1 :
			Double_t(Ncounts - icounts)/Double_t(icounts)*ElapsedSeconds() );

  return SecToText( leftTime );
}

//_______________________________________________________________________
TString ProgressBar::SecToText(  Double_t seconds )
{

  TString out = "";
  if (seconds <  0  ) out = "unknown";
  else if (seconds <= 300) out = Form( "%i sec", Int_t(seconds) );
  else {
    if (seconds > 3600) {
      Int_t h = Int_t(seconds/3600);
      if (h <= 1) out = Form( "%i hr : ", h );
      else        out = Form( "%i hrs : ", h );
      
      seconds = Int_t(seconds)%3600;
    }
    Int_t m = Int_t(seconds/60);
    if (m <= 1) out += Form( "%i min", m );
    else        out += Form( "%i mins", m );
  }

  return out;
}

//_______________________________________________________________________
void ProgressBar::DrawProgressBar( Int_t i )
{

  // check for consistency:
  if (i > Ncounts-1) i = Ncounts-1;
  if (i < 0         ) i = 0;
  Int_t ic = Int_t(Float_t(i)/Float_t(Ncounts)*pbNbins);
  
  std::clog << Prefix << " ";
  
  // re-print the actual bar
  std::clog << "[";
  for (Int_t it = 0; it<ic; it++){
    std::clog << ">";
  }
  
  for (Int_t it = ic+1; it<pbNbins; it++){
    std::clog << " ";
  }
  std::clog << "]";

  // print timing information
  std::clog <<  "(" << Int_t((100*(i+1))/Float_t(Ncounts)) << "%" 
	    << ", " << "time left: " << this->GetLeftTime( i ) << ") ";
  std::clog << "\r" << std::flush;
}


//______________________________________________________________
bool FileExists(std::string strFilename) { 
  struct stat stFileInfo; 
  bool blnReturn; 
  int intStat; 

  // Attempt to get the file attributes 
  intStat = stat(strFilename.c_str(),&stFileInfo); 
  if(intStat == 0) { 
    // We were able to get the file attributes 
    // so the file obviously exists. 


    blnReturn = true; 
  } else { 
    // We were not able to get the file attributes. 
    // This may mean that we don't have permission to 
    // access the folder which contains this file. If you 
    // need to do that level of checking, lookup the 
    // return values of stat which will give you 
    // more details on why stat failed. 
    blnReturn = false; 
  } 
   
  return(blnReturn); 
}

//______________________________________________________________
void print_usage(){

  std::cout << std::endl;
  std::cout << " --->>> i L U M I C A L C . E X E <<<---"<< std::endl;
  std::cout << std::endl;
  std::cout << "iLumiCalc.exe: this c++ program calculates integrated luminosity" << std::endl ;
  std::cout << "by looping over a list of lumiblocks, given a set of input options." << std::endl;
  std::cout << "Input file can be either an xml file, or a TAG file." << std::endl;
  std::cout << std::endl;
  std::cout << "Type: iLumiCalc.exe --help for the complete list of options" << std::endl ;
  std::cout << std::endl;
  std::cout << "Further help: https://twiki.cern.ch/twiki/bin/view/Atlas/CoolLumiCalc"<< std::endl;
  std::cout << std::endl;
  exit(-1);

}


//______________________________________________________________
void print_credits(){
  std::cout << "ATLAS - A Toroidal LHC Aparatus" << std::endl;
  exit(-1);
}


