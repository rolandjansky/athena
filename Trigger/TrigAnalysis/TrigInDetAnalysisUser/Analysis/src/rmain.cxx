
#include <cstdio>

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <set>

//#include "Cintex/Cintex.h"

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackEvent.h"
#include "TrigInDetAnalysis/TrackSelector.h"

#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
#include "Filters.h"
#include "TrigInDetAnalysisExample/NtupleTrackSelector.h"
#include "TrigInDetAnalysisUtils/Associator_TruthMatch.h"

#include "TrigInDetAnalysis/Efficiency.h"

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"


#include "ConfAnalysis.h"
#include "PurityAnalysis.h"

#include "ReadCards.h"
#include "utils.h"

#include "lumiList.h"
#include "dataset.h"

#include "event_selector.h"

#include "BinConfig.h"



// useful function to return a string with the 
// current date   
std::string time_str() { 
  time_t _t;
  time(&_t);

  std::string s(ctime(&_t));

  //  std::string::size_type pos = s.find("\n");
  // if ( pos != std::string::npos ) 

  return s.substr(0,s.find("\n"));
  //  return s;
}

int Nvtx = 0;
int NvtxCount = 0;

bool hipt     = false;

bool dumpflag = false;

extern bool PRINT_BRESIDUALS;



int NMod = 14;


int r = 0;
int lb = 0;
int ts = 0;
int ev = 0;

double a0 = 1.5;


/// Return selected author, expects format
/// L2_e20_medium:TrigL2SiTrackFinder:3
/// where 3 is the track author
/// NB: this isn't actually needed at all
int GetChainAuthor(std::string chainName) {

  /// can parse the entire name properly with this technique - 
  /// NB: should move to some meaningful name for strategies "A", "B", etc 
  std::string s1 = chop(chainName,":"); /// chops tokens off the front, up to the ":"
  std::string s2 = chop(chainName,":");
  std::string s3 = chop(chainName,":");

  //  if only s3=="" then no third token, if s2=="", no second token etc 
  // return atoi(s3.c_str());

  //  std::cout << ">" << s1 << "< >" << s2 << "< >" << s3 << "<" << std::endl;
  if ( s3=="0" ) return 0;
  if ( s3=="1" ) return 1;
  if ( s3=="2" ) return 2; 
  // }
  return -1;

}



template<class T>
std::ostream& operator<<(std::ostream& s, const std::vector<T>& v ) { 
  for ( unsigned i=0 ; i<v.size() ; i++ ) s << "\t" << v[i] << std::endl;
  return s;
}

template<class T>
std::ostream& operator<<(std::ostream& s, const std::vector<T*>& v ) { 
  for ( unsigned i=0 ; i<v.size() ; i++ ) s << "\t" << *v[i] << std::endl;
  return s;
}

	


struct event_list { 

  event_list(const std::string& filename) { 

    std::cout << "event_list::event_list() "; 
    std::cout << filename << std::endl;
    
    std::ifstream file(filename.c_str());
    
    int evnt;
    
    while ( file>>evnt ) { 
      mevents.insert(evnt);
      //      std::cout << "evnt " << evnt << std::endl; 
    }
    
    std::cout << "event_list::event_list() ";
    std::cout << mevents.size() << std::endl;
    
  }
  
  bool find(int i) { 
    std::set<int>::iterator it = mevents.find(i);
    if ( it!=mevents.end() ) return true;
    return false;
  }

  std::set<int> mevents;

};


TrackEvent* event = 0;




int usage(const std::string& name, int status) { 
  std::ostream& s = std::cout;
  
  s << "Usage: " << name << " <config filename> [OPTIONS]" << std::endl;
  s << "\nOptions: \n";
  s << "    -f, --file      value\toutput filename, \n";
  s << "    -b, --binConfig value\tconfig file for histogram configuration, \n";
  s << "    -r, --refChain  value\treference chain, \n";
  s << "    -t, --testChain value\ttest chain, \n";
  //  s << "    -a, --all     \tadd all grids (default)\n";
  s << "    -p, --pdgId     value\tpdg ID of truth particle if requiring truth particle processing,\n";
  s << "    -h, --help           \tthis help\n";
  //  s << "\nSee " << PACKAGE_URL << " for more details\n"; 
  s << "\nReport bugs to sutt@cern.ch";
  s << std::endl;

  
  return status;
}


/// global to grant access to the roi descriptor
TIDARoiDescriptor* groi = 0;


extern BinConfig binConfig;

extern BinConfig electronBinConfig;
extern BinConfig muonBinConfig;
extern BinConfig tauBinConfig;
extern BinConfig bjetBinConfig;
extern BinConfig cosmicBinConfig;


// #define DATA

int main(int argc, char** argv) 
{

  //  ROOT::Cintex::Cintex::Enable();

  if ( argc<2 ) { 
    std::cerr << "Error: no config file specified\n" << std::endl;
    return usage(argv[0], -1);
  }

  //  event_list elist("events.txt");

  /// set some configuration parameters that can be set from
  /// the command line - these over-ride those from the 
  /// configuration file
  /// Fixme: which is best? set them from the command line first, 
  ///        and then ignore them in the config file, or set from 
  ///        the config file, and then override them with command 
  ///        line arguments later? Theres only one way to find out...
  

  /// get a filename from the command line if present

  std::string histofilename("");

  std::string datafile = "";

  std::string refChain = "";

  int pdgId = 0;

  std::vector<std::string> testChains;

  std::string binningConfigFile = "";

  for ( int i=1 ; i<argc ; i++ ) { 
    if ( std::string(argv[i])=="-h" || std::string(argv[i])=="--help" ) {
      return usage(argv[0], 0);
    } 
    else if ( std::string(argv[i])=="-f" || std::string(argv[i])=="--file" ) { 
      if ( ++i>=argc ) return usage(argv[0], -1);
      histofilename = argv[i];
      if ( histofilename.find(".root")==std::string::npos ) histofilename += ".root";
    }
    else if ( std::string(argv[i])=="-r" || std::string(argv[i])=="--refChain" ) { 
      if ( ++i>=argc ) return usage(argv[0], -1);
      refChain = argv[i];
    }
    else if ( std::string(argv[i])=="-t" || std::string(argv[i])=="--testChain" ) { 
      if ( ++i>=argc ) return usage(argv[0], -1);
      testChains.push_back(argv[i]);
    }
    else if ( std::string(argv[i])=="-p" || std::string(argv[i])=="--pdgId" ) { 
      if ( ++i>=argc ) return usage(argv[0], -1);
      pdgId = atoi(argv[i]);
    }
    else if ( std::string(argv[i])=="-b" || std::string(argv[i])=="--binConfig" ) { 
      if ( ++i>=argc ) return usage(argv[0], -1);
      binningConfigFile = std::string(argv[i]);
    }
    else if ( std::string(argv[i]).find("-")==0 ) { 
      /// unknown option 
      std::cerr << "unknown option " << argv[i] << std::endl; 
      return usage(argv[0], -1);
    } 
    else { 
      datafile = argv[i];
    }
  }
  
  if ( datafile=="" ) { 
    std::cerr << "no config file specifed\n" << endl;
    return usage(argv[0], -1);
  }

  std::cout << time_str() << std::endl;

  

  ReadCards inputdata(datafile);

  inputdata.print();


#if 0
  bool oldrms95 = true;
  inputdata.declareProperty( "OldRMS95", oldrms95 );
  Resplot::setoldrms95( oldrms95 );
#endif


  unsigned nfiles = 0;
  inputdata.declareProperty( "NFiles", nfiles );


  if ( histofilename=="" ){ 
    if ( inputdata.isTagDefined("outputFile") )  histofilename = inputdata.GetString("outputFile");
    else  {
      std::cerr << "Error: no output file defined\n" << std::endl;
      return usage(argv[0], -1);
    }
  }

  /// open output file
  TFile foutput( histofilename.c_str(), "recreate" );
  if (!foutput.IsOpen()) {
    std::cerr << "Error: could not open output file\n" << std::endl;
    return -1;
  }

  TDirectory* foutdir = gDirectory; 

  std::cout << "writing output to " << histofilename << std::endl;

  TH1D* hevent = new TH1D( "event", "event", 1000, 10000, 80000 );
  Resplot* hcorr = new Resplot( "correlation", 21, -0.5, 20.5, 75, 0, 600); 

  /// set up the filters etc

  double pT     = 1000;
  double eta    = 2.5;
  double zed    = 200;

  int npix = 1;
  int nsct = 6;
  int nbl = -1;

  //int npix_rec = 1; // JK removed (unused)
  //int nsct_rec = -1;  // JK removed (unused)

  //double pT_rec = 0;  // JK removed (unused)
  double eta_rec = 5;

  double Rmatch = 0.1;

  double pTmatchLim_2 = 1.0;

  //bool printflag = false;  // JK removed (unused)

  bool truthMatch = false;

  bool pTMatch_2 = false;

  if ( inputdata.isTagDefined("pT") )   pT   = inputdata.GetValue("pT");


  if ( inputdata.isTagDefined("NMod") )   NMod   = inputdata.GetValue("NMod");


  if ( inputdata.isTagDefined("eta") )  eta  = inputdata.GetValue("eta");
  if ( inputdata.isTagDefined("zed") )  zed  = inputdata.GetValue("zed");
  if ( inputdata.isTagDefined("npix") ) npix = inputdata.GetValue("npix");
  if ( inputdata.isTagDefined("nsct") ) nsct = inputdata.GetValue("nsct");
  if ( inputdata.isTagDefined("nbl") ) nbl = inputdata.GetValue("nbl");

  /// only if not set from the command line
  if ( pdgId==0 && inputdata.isTagDefined("pdgId") ) pdgId = inputdata.GetValue("pdgId");
  

  //if ( inputdata.isTagDefined("npix_rec") ) npix_rec = inputdata.GetValue("npix_rec");  // JK removed (unused)
  //if ( inputdata.isTagDefined("nsct_rec") ) nsct_rec = inputdata.GetValue("nsct_rec");  // JK removed (unused)

  //if ( inputdata.isTagDefined("pT_rec") )  pT_rec  = inputdata.GetValue("pT_rec");  // JK removed (unused)
  if ( inputdata.isTagDefined("eta_rec") ) eta_rec = inputdata.GetValue("eta_rec");

  if ( inputdata.isTagDefined("a0") )           a0     = inputdata.GetValue("a0");
  if ( inputdata.isTagDefined("Rmatch") )       Rmatch = inputdata.GetValue("Rmatch");
  if ( inputdata.isTagDefined("pTmatchLim_2") ) pTmatchLim_2 = inputdata.GetValue("pTmatchLim_2");
  if ( inputdata.isTagDefined("pTMatch_2") )    pTMatch_2 = inputdata.GetValue("pTMatch_2");
  if ( inputdata.isTagDefined("truthMatch") )    truthMatch = ( inputdata.GetValue("truthMatch") ? 1 : 0);

  std::string useMatcher = "DeltaR";
  if ( inputdata.isTagDefined("UseMatcher") ) useMatcher = inputdata.GetString("UseMatcher");  

 
  /// only if not set from the command line
  if ( refChain=="" ) { 
    if ( inputdata.isTagDefined("refChain") )  refChain = inputdata.GetString("refChain"); 
    else { 
      std::cerr << "Error: no reference chain defined\n" << std::endl;
      //  return usage(argv[0], -1);
      return -1;
    }
  }

  if ( testChains.size()==0 ) { 
    if ( inputdata.isTagDefined("testChains") ) { 
      /// a whole list of chains
      testChains = inputdata.GetStringVector("testChains");
    }
    else { 
      if ( inputdata.isTagDefined("testChain") ) {
	/// a single chain
	testChains.push_back( inputdata.GetString("testChain") );
      }
    }
  }

  /// print soime debugging output
  //if ( inputdata.isTagDefined("printflag") )  printflag = ( inputdata.GetValue("printflag") ? 1 : 0 );  // JK removed (unused)

  /// select only tracks within the roi?
  bool select_roi = true;
  if ( inputdata.isTagDefined("SelectRoi") )  { 
    select_roi = ( inputdata.GetValue("SelectRoi")!=0 ? true : false );
  }

  //  bool selectfake_roi = false;  // JK removed (unused)
  //  if ( inputdata.isTagDefined("SelectFakeRoi") )  { 
  //     selectfake_roi = ( inputdata.GetValue("SelectFakeRoi")!=0 ? true : false );  // JK removed (unused)
  //  }
  

  std::vector<double> _lumiblocks;
  lumiList  goodrunslist;

  if ( inputdata.isTagDefined("LumiBlocks") )  { 
    _lumiblocks = inputdata.GetVector("LumiBlocks");

    for (unsigned int i=0 ; i<_lumiblocks.size()-2 ; i+=3 ){ 
      goodrunslist.addRange( _lumiblocks[i],  _lumiblocks[i+1],  _lumiblocks[i+2] );  
    }
  }

  bool useBestVertex = false;
  if ( inputdata.isTagDefined("useBestVertex") ) useBestVertex = ( inputdata.GetValue("useBestVertex") ? 1 : 0 );

  bool useSumPtVertex = true;
  if ( inputdata.isTagDefined("useSumPtVertex") ) useSumPtVertex = ( inputdata.GetValue("useSumPtVertex") ? 1 : 0 );

  bool vetoBestVertex = false;
  if ( inputdata.isTagDefined("vetoBestVertex") ) vetoBestVertex = ( inputdata.GetValue("vetoBestVertex") ? 1 : 0 );


  int NVtxTrackCut = 2;
  if ( inputdata.isTagDefined("NVtxTrackCut") ) NVtxTrackCut = inputdata.GetValue("NVtxTrackCut");

  int MinVertices = 1;
  if ( inputdata.isTagDefined("MinVertices") ) MinVertices = inputdata.GetValue("MinVertices");


  std::vector<double> event_list;
  bool event_selector_flag = false;

  if ( inputdata.isTagDefined("EventSelector") ) event_list = inputdata.GetVector("EventSelector");

  event_selector es(event_list);

  if ( es.size() ) event_selector_flag = true;


  std::vector<double> beamTest;
  std::vector<double> beamRef;

  if ( inputdata.isTagDefined("BeamTestx") )     beamTest.push_back(inputdata.GetValue("BeamTestx"));
  if ( inputdata.isTagDefined("BeamTesty") )     beamTest.push_back(inputdata.GetValue("BeamTesty"));
  
  else {
    beamTest.push_back(0);    beamTest.push_back(0);
  }
  if ( inputdata.isTagDefined("BeamRefx") )    beamRef.push_back(inputdata.GetValue("BeamRefx"));
  if ( inputdata.isTagDefined("BeamRefy") )    beamRef.push_back(inputdata.GetValue("BeamRefy"));
  
  else {
    beamRef.push_back(0);    beamRef.push_back(0);
  }
  
  
  if ( beamTest.size()!=2 || beamRef.size()!=2 ) { 
    std::cerr << "incorrectly specified beamline position" << std::endl;
    return (-1);
  }
  
  double a0v = 1000;
  double z0v = 2000;

  if ( inputdata.isTagDefined("a0v") )  a0v = inputdata.GetValue("a0v");
  if ( inputdata.isTagDefined("z0v") )  z0v = inputdata.GetValue("z0v");


  double a0vrec = 1000;
  double z0vrec = 2000;

  if ( inputdata.isTagDefined("a0vrec") )  a0vrec = inputdata.GetValue("a0vrec");
  if ( inputdata.isTagDefined("z0vrec") )  z0vrec = inputdata.GetValue("z0vrec");

  bool initialiseFirstEvent = false;
  if ( inputdata.isTagDefined("InitialiseFirstEvent") )  initialiseFirstEvent = inputdata.GetValue("InitialiseFirstEvent");


  // set the flag to prinout the missing track list in ConfAnalysis
  if ( inputdata.isTagDefined("dumpflag") )  dumpflag = ( inputdata.GetValue("dumpflag")==0 ? false : true );  


  bool doPurity = false;
  if ( inputdata.isTagDefined("doPurity") )  doPurity = ( inputdata.GetValue("doPurity")==0 ? false : true );

  bool debugPrintout = false;
  if ( inputdata.isTagDefined("DebugPrintout") )  debugPrintout = ( inputdata.GetValue("DebugPrintout")==0 ? false : true );



  ReadCards* binningConfig = &inputdata;

  /// read the binning config from a separate file if required

  if ( binningConfigFile!="" ) binningConfig = new ReadCards( binningConfigFile );
    
  /// set the tags in front of the histogram stuff 
  binConfig.set(         *binningConfig, "" );
  electronBinConfig.set( *binningConfig, "e_" );
  muonBinConfig.set(     *binningConfig, "mu_" );
  tauBinConfig.set(      *binningConfig, "tau_" );
  bjetBinConfig.set(     *binningConfig, "bjet_" );
  cosmicBinConfig.set(   *binningConfig, "cosmic_" );


  /// clean up
  if ( binningConfig!=&inputdata ) delete binningConfig;


  if ( inputdata.isTagDefined("PRINT_BRESIDUALS") ) PRINT_BRESIDUALS = ( inputdata.GetValue("PRINT_BRESIDUALS")==0 ? false : true );


  std::cout << "using reference " << refChain << std::endl;
  if ( refChain=="Truth" ) std::cout << "using pdgId " << pdgId << std::endl;

  /// track filters 

  /// truth articles

  Filter_Track filter_kine( eta, 1000,  zed, pT, -1, -1,   -1, -1,  -2, -2);


  /// filters for true selection for efficiency

  Filter_Vertex filter_vertex(a0v, z0v);

  Filter_Track filter_offline( eta, 1000,  2000, pT, npix, nsct, -1, nbl,  -2, -2);
  Filter_etaPT filter_etaPT(eta,pT);
  //  Filter_True filter_passthrough;
  // use an actual filter requiring at least 1 silicon hit
  // to get rid of the EF trt only tracks 
  Filter_Track filter_passthrough( 10, 1000,  2000, 0, -2, -2, 1, -2,  -2, -2);


  //  std::cout << "pdgId " << pdgId << std::endl;
  //Select only true particles matching pdgId
  Filter_pdgId filter_pdgtruth(pdgId); //BP

  // select inside-out offline tracking

  //Filter_TruthParticle filter_passthrough(&filter_offline);
  Filter_Track filter_track( eta, 1000,  2000,     pT, npix, nsct,   -1, -1,  -2, -2);

  Filter_Author    filter_inout(0);

  int author = 0;

  Filter_Author    filter_auth(author);

  Filter_Combined  filter_kineoff( &filter_inout, &filter_offline );
  //Filter_Combined  filter_off (&filter_passthrough, &filter_vertex);
  Filter_TrackQuality filter_q(0.01);  
  //Filter_Combined  filter_c(&filter_q, &filter_offline);
  //Filter_Combined  filter_off (&filter_c, &filter_vertex);
  Filter_Combined   filter_off (&filter_offline, &filter_vertex);
  //  Filter_Combined   filter_off (&filter_offline, &filter_auth);
  //Filter_Combined   filter_off (&filter_offline, &filter_auth);
  Filter_Track      filter_onlinekine(  eta_rec, 1000, 2000, pT,    -1,  npix,  nsct, -1,  -2, -2);
  /// filter to select all tracks without filtering
  //Filter_Combined  filter_off( &filter_kineoff, &filter_vertex);
  //Filter_Combined  filter_truth( &filter_kineoff, &filter_passthrough);
  //Filter_Combined  filter_truth( &filter_passthrough, &filter_kineoff);
  //Filter_Combined  filter_truth( &filter_passthrough, &filter_etaPT);
  Filter_Combined  filter_truth( &filter_pdgtruth, &filter_etaPT);




  Filter_Combined  filter_muon( &filter_offline, &filter_vertex);


  Filter_Vertex     filter_onlinevertex(a0vrec, z0vrec);
  //Filter_Track      filter_onlinekine(  eta_rec, 1000, 2000, pT,    -1,  npix,  nsct, -1,  -2, -2);
  Filter_Combined   filter_online( &filter_onlinekine, &filter_onlinevertex ); 


  Filter_Track      filter_offkinetight(  5, 1000, 2000, pT,    -1,  0,  0, -1,  -2, -2);
  Filter_Combined   filter_offtight( &filter_offkinetight, &filter_inout ); 



  /// track selectors so we can select multiple times with different 
  /// filters if we want (simpler then looping over vectors each time 

  //  NtupleTrackSelector  offlineTracks(&filter_offline);
  // NtupleTrackSelector  refTracks(&filter_off); 
  TrackFilter* refFilter;
  if      ( refChain=="Offline" )           refFilter = &filter_off;
  else if ( refChain=="Electrons" )         refFilter = &filter_off;
  else if ( refChain=="ElectronsMedium" )   refFilter = &filter_off;
  else if ( refChain=="Muons" )             refFilter = &filter_muon;
  else if ( refChain=="Taus" )              refFilter = &filter_off;
  else if ( refChain=="Taus3" )              refFilter = &filter_off;
  else if ( refChain=="Truth" && pdgId!=0 ) refFilter = &filter_truth;
  else if ( refChain=="Truth" && pdgId==0 )  refFilter = &filter_off;
  else { 
    std::cerr << "unknown reference chain defined" << std::endl;
    return (-1);
  }

  TrackFilter* testFilter = &filter_passthrough;


  std::cout << "using tracks: " << refChain  << " for reference sample" << std::endl;


  /// strings for which chains to analyse 
  // std::vector<std::string> chains;

  std::vector<std::string>& test_chains = testChains;


  /// create a map of the name to analysis string for selecting
  /// the correct analysis from the chain name
  //smh: TrackAnalysis is purely abstract, analysis will contain ConfAnalysis
  std::map<std::string,TrackAnalysis*> analysis;

  /// create the analyses and initialise them - 
  /// this will create a root directory and 
  /// book all the histograms    

  std::vector<TrackAnalysis*> analyses;
  analyses.reserve(test_chains.size());


  std::cout << "booking " << test_chains.size() << " analyses" << std::endl;
  
  for (unsigned int i=0; i < test_chains.size(); i++ ) {

    std::string chainname = test_chains[i];

    std::vector<std::string> chainnames;

    chainnames.push_back(chainname);

    ConfAnalysis* analy_conf = new ConfAnalysis(chainnames[0]);
    analy_conf->initialiseFirstEvent(initialiseFirstEvent);
    analy_conf->initialise();
    analy_conf->setprint(false);
    // analy_conf->setprint(true);
    
    //    for (unsigned int ic=0 ; ic<chainnames.size() ; ic++ )  analysis[chainnames[ic]] = analy_conf;
    
    analysis[chainname] = analy_conf;

    analyses.push_back(analy_conf);

    std::cout << "analysis: " << chainname << "\t" << analy_conf 
	      << "\n" 
	      << "---------------------------------" 
	      << "---------------------------------" 
	      << "---------------------------------" << std::endl;

    if ( doPurity ) {
      PurityAnalysis* analp = new PurityAnalysis(chainnames[0]+"-purity"); 
      analp->initialise();
      analp->setprint(false);
      // analp->setprint(true);
      analysis[chainnames[0]+"-purity"] = analp;
      analyses.push_back(analp);
    }

  }

  /// track selectors for efficiencies

  dynamic_cast<Filter_Combined*>(refFilter)->setDebug(debugPrintout);

  NtupleTrackSelector  refTracks( refFilter );
  NtupleTrackSelector  offTracks( testFilter );
  NtupleTrackSelector testTracks( testFilter);

  //  	NtupleTrackSelector  refTracks( &filter_passthrough );
  //  	NtupleTrackSelector  testTracks( refFilter );


  //NtupleTrackSelector  refTracks(  &filter_pdgtruth);
  //NtupleTrackSelector  testTracks( &filter_off );

  
  //NtupleTrackSelector testTracks(&filter_roi);

  TrackAssociator* _matcher = 0;

  if      ( useMatcher == "Sigma" )  _matcher = new Associator_BestSigmaMatcher("sigma", Rmatch); 
  else if ( useMatcher == "DeltaRZ" || useMatcher == "DeltaRZSinTheta" )  { 
    double deta = 0.05;
    double dphi = 0.05;
    double dzed = 25;
    if ( inputdata.isTagDefined("Matcher_deta" ) ) deta = inputdata.GetValue("Matcher_deta"); 
    if ( inputdata.isTagDefined("Matcher_dphi" ) ) dphi = inputdata.GetValue("Matcher_dphi"); 
    if ( inputdata.isTagDefined("Matcher_dzed" ) ) dzed = inputdata.GetValue("Matcher_dzed"); 

    if ( useMatcher == "DeltaRZ" ) _matcher = new Associator_BestDeltaRZMatcher(         "deltaRZ", deta, dphi, dzed ); 
    else                           _matcher = new Associator_BestDeltaRZSinThetaMatcher( "deltaRZ", deta, dphi, dzed ); 
  }
  else { 
  /// track matcher for best fit deltaR matcher
    _matcher = new Associator_BestDeltaRMatcher("deltaR", Rmatch); 
  }

  TrackAssociator& dRmatcher = *_matcher;

  //  Associator_BestDeltaPhiMatcher matcher("deltaPhi", 0.1);
  Associator_TruthMatcher truthMatcher;
  //Associator_BestDeltaRMatcher_OLD matcher("deltaR", Rmatch); 
  
  // track matcher for experimental pT matcher
  Associator_SecondBestpTMatcher SecpTmatcher("SecpT", pTmatchLim_2);

  //NtupleTrackSelector  roiTracks( refFilter );



  /// track selectors for purities

  //  NtupleTrackSelector  refPurityTracks( &filter_offtight );
  //  NtupleTrackSelector  testPurityTracks( &filter_online );

  NtupleTrackSelector  refPurityTracks( &filter_inout );
  NtupleTrackSelector  testPurityTracks( &filter_online );

  // get the list of input files

  std::vector<std::string> filenames;


  if ( inputdata.isTagDefined("DataSets") ) {  

    std::vector<std::string> datasets = inputdata.GetStringVector("DataSets");
    for (unsigned int ids=0 ; ids<datasets.size() ; ids++ ) { 
      dataset d( datasets[ids] );
      std::vector<std::string> _filenames = d.datafiles();
      filenames.insert(filenames.end(),_filenames.begin(),_filenames.end());
    }
  }
  else if ( inputdata.isTagDefined("DataFiles") ) filenames = inputdata.GetStringVector("DataFiles");
  else {
    std::cerr << "no input data specified" << std::endl;
    return (-1);
  }


  /// copy the release data to the output file
  
  //  TString* releaseMetaData = 0;
  //  data->SetBranchAddress("ReleaseMetaData",&releaseMetaData);

  bool show_release = true;

  std::vector<std::string> release_data;

  if ( show_release ){

    for ( unsigned i=0 ; i<filenames.size() ; i++ ) {
    
      TFile finput( filenames[i].c_str() );
      if (!finput.IsOpen()) {
	std::cerr << "Error: could not open input file" << filenames[i] << std::endl;
	exit(-1);
      }
  
      TTree*   dataTree    = (TTree*)finput.Get("dataTree");
      TString* releaseData = new TString("");
      
      if ( dataTree ) { 
	dataTree->SetBranchAddress( "ReleaseMetaData", &releaseData);
	
	for (unsigned int i=0; i<dataTree->GetEntries() ; i++ ) {
	  dataTree->GetEntry(i);      
	  release_data.push_back( releaseData->Data() );
	  std::cout << "main() release data: " << release_data.back() << " " << *releaseData << std::endl;
	}
      }
    }      

    for ( unsigned ird=0 ; ird<release_data.size() ; ird++ ) std::cout << "presort  " << ird << " " << release_data[ird] << std::endl;

    if ( !release_data.empty() ) { 
      std::sort(release_data.begin(), release_data.end());
      release_data.erase(std::unique(release_data.begin(), release_data.end()), release_data.end());

      for ( unsigned ird=0 ; ird<release_data.size() ; ird++ ) std::cout << "postsort " << ird << " " << release_data[ird] << std::endl;
    
      foutdir->cd();
      
      TTree*   dataTree    = new TTree("dataTree", "dataTree");
      TString* releaseData = new TString("");

      if ( dataTree ) { 
	dataTree->Branch( "ReleaseMetaData", "TString", &releaseData);
	
	for ( unsigned ird=0 ; ird<release_data.size() ; ird++ ) {  
	  *releaseData = release_data[ird];
	  dataTree->Fill();
	}
	
	dataTree->Write("", TObject::kOverwrite);
	delete dataTree;
      }
    }   
  
  }

  //  foutput.Write();
  //  foutput.Close();
  
  //  exit(0);


  TChain *data = new TChain("tree");

  TrackEvent* track_ev = new TrackEvent();

  data->SetBranchAddress("TrackEvent",&track_ev);



  int addedfiles = 0;

  event = track_ev;
  for ( unsigned i=0 ; i<filenames.size() && ( nfiles==0 || i<nfiles ) ; i++ ) { 
    addedfiles++;  
    data->AddFile(filenames[i].c_str());
  }

  std::cout << "added " << addedfiles << " files" << std::endl;
  //smh If no files can be opened, exit
  if (addedfiles == 0) {
    std::cout << "Cannot read any files, exiting" << std::endl;
    return (-1);
  }



  /// so we can specify the number of entries 
  /// we like, rather than run on all of them

  unsigned Nentries = data->GetEntries();
  if ( inputdata.isTagDefined("Nentries") ) { 
    unsigned _Nentries = unsigned(inputdata.GetValue("Nentries"));
    if ( _Nentries<Nentries ) Nentries = _Nentries;
  }

  std::cout << "file: " << argv[1] << "\tentries=" << data->GetEntries() << "\t( running over " << Nentries << " )" << std::endl;

  std::cout << "starting event loop " << time_str() << std::endl;

  unsigned event_counter = 0;




  int maxtime = track_ev->time_stamp();
  int mintime = track_ev->time_stamp();



  //  int Nentries = data->GetEntries();
  
  /// so we can specify the number of entries 
  /// we like, rather than run on all of them
  for (unsigned int i=0; i<Nentries ; i++ ) {
    data->GetEntry(i);
    //    if (i==0) {
    //      std::cout << "TrkNtuple generated with: " << *releaseMetaData << std::endl;//Only necessary for first event
    //    }
    
    //    if ( r!=h->run_number() ) std::cout <<* h << std::endl;

    r         = track_ev->run_number();
    ev        = track_ev->event_number();
    lb        = track_ev->lumi_block();
    ts        = track_ev->time_stamp();

    int event = track_ev->event_number();    
    //int bc    = track_ev->bunch_crossing_id();


    hipt = false;

    /// check whether in good lumi block range
    if ( !goodrunslist.inRange( r, lb ) ) continue;

    /// check whether it's in the event selector list
    if ( event_selector_flag && !es.in( event ) ) continue;

    if ( mintime>ts ) mintime = ts;
    if ( maxtime<ts ) maxtime = ts;

    //    if ( !elist.find(event) ) continue;

    //    std::cout << "run " << r << "\tevent " << event << "\tlb " << lb << std::endl;

    hevent->Fill( event );

    if ( event_counter>Nentries ) break;

    event_counter++;

    if ( (Nentries<10) || i%(Nentries/10)==0 || i%1000==0 || debugPrintout )  { 
      std::cout << "run "      << track_ev->run_number() 
                << "\tevent "  << track_ev->event_number() 
                << "\tlb "     << track_ev->lumi_block() 
                << "\tchains " << track_ev->chains().size()
                << "\ttime "   << track_ev->time_stamp();
      std::cout << "\t : processed " << i << " events so far (" << (100*i)/Nentries << "%)\t" << time_str() << std::endl;
      //   std::cerr << "\tprocessed " << i << " events so far \t" << time_str() << std::endl;
    }
  
    //		if ( printflag )  std::cout << *track_ev << std::endl;   
    
    r = track_ev->run_number();

    /// get the reference tracks (or perhaps even the true tracks !!!

    offTracks.clear();
    refTracks.clear();
    refPurityTracks.clear();
    //    offlineTracks.clear();

    /// set the vertex in the vertex filter
    /// this takes the first "good" vertex
    /// NB: probably need to use the vertex  
    ///     with most tracks
    int good_vertex = -1;
    
    Nvtx = 0;

    const std::vector<TrackChain>& chains = track_ev->chains();

    for (unsigned int ic=0 ; ic<chains.size() ; ic++ ) {
      if ( chains[ic].name()==refChain ) {
        offTracks.selectTracks( chains[ic].rois()[0].tracks() );
	break;
      }
    }

    std::vector<TrackVertex> vertices;
    
    NvtxCount = 0;

    const std::vector<TrackVertex>& mv = track_ev->vertices();

    int SumPtBest = -1;
    double highestSumPt = 0.0;

    for ( unsigned iv=0 ; iv<mv.size() ; iv++ ) {
      int Ntracks = mv[iv].Ntracks();
      if ( Ntracks>NVtxTrackCut ) {
	Nvtx += Ntracks;
	vertices.push_back( mv[iv] );
        NvtxCount++;
	if ( good_vertex == -1 || Ntracks>mv[good_vertex].Ntracks() ) { 
	  good_vertex = iv;
	}
      }
      double SumPt = 0.0;
      for (unsigned itr=0; itr<offTracks.tracks().size(); itr++){
	TrigInDetAnalysis::Track* tr = offTracks.tracks().at(itr);
	if(std::fabs(mv.at(iv).position()[2]-tr->z0())<1.5) SumPt += std::fabs(tr->pT());
      }
      if(SumPt>highestSumPt){
	highestSumPt = SumPt;
	SumPtBest = iv;
      }
    }//loop over vertices
    
    
    
    // all vertices *except* the best one
    std::vector<TrackVertex> vvtx;
    if ( vetoBestVertex ) { 
      for (unsigned int i=0 ; i<vertices.size() ; i++ ) { 
	if ( good_vertex!=int(i) ) vvtx.push_back(vertices[i]);
      }
    }
    else { 
      for (unsigned int i=0 ; i<vertices.size() ; i++ ) vvtx.push_back(vertices[i]);
    }
    
    if ( int(vvtx.size())<MinVertices ) continue;
    
    NvtxCount = vvtx.size();
    if(useSumPtVertex) good_vertex = SumPtBest;
    if ( MinVertices>0 ) { 
      /// if no good vertex, skip this event
      if ( good_vertex==-1 ) continue;
      else { 
	if ( useBestVertex )  filter_vertex.setVertex( mv[good_vertex] );
	else                  filter_vertex.setVertex( vvtx );
      }
    }

    //    filter_vertex.setVertex( vvtx ) ;
    hcorr->Fill( vvtx.size(), Nvtx ); 

    dynamic_cast<Filter_Combined*>(refFilter)->setRoi(0);

    /// get the tracks from the reference chain - shouldn't be one
    /// of the test chains, since there will be a 1-to-1 mapping
    /// but might be a useful check
    bool foundReference = false;

    const TrackChain* refchain = 0;

    for (unsigned int ic=0 ; ic<chains.size() ; ic++ ) { 
      if ( chains[ic].name()==refChain ) { 

	refchain = &chains[ic];
	
	foundReference = true;
	
	//	if ( debugPrintout ) { 
	//	  std::cout << "reference tracks:\n" << chains[ic].rois()[0].tracks() << std::endl;
	//	}

	//	std::cout << "using chain " << chains[ic].name() 
	//	  << " with " << chains[ic].rois()[0].tracks().size() << " tracks" << std::endl;
	//Get tracks from within reference roi
	refTracks.selectTracks( chains[ic].rois()[0].tracks() );

	//	std::cout << "refTracks " << refTracks.size() << " tracks from " << chains[ic].rois()[0].tracks().size() << std::endl;

	break;
      }
    }

    if ( !foundReference ) continue;
    
    if ( debugPrintout ) { 
      std::cout << "vertices:\n" << mv << std::endl;
      std::cout << "reference chain:\n" << *refchain << std::endl;
    }
    

    //    std::cout << " chains size " << track_ev->chains().size() << std::endl;

    for (unsigned int ic=0 ; ic<track_ev->chains().size() ; ic++ ) { 

      TrackChain& chain = track_ev->chains()[ic];

      //      std::cout << ic << " chain " << chain.name() << " size " << chain.size() << std::endl;

      /// find the analysis for this chain - is there a matching analysis?
      std::map<std::string,TrackAnalysis*>::iterator analitr = analysis.find(chain.name());

      /// no matching analysis so continue 
      if ( analitr==analysis.end() ) continue;
      // std::cout << "\t" << ic << " chain " << chain.name() << " size " << chain.size() << "\t: processing" << std::endl;
      // cout << "\tNumber of rois for " << chain.name() << " " << chain.size() << endl;

      if ( debugPrintout ) { 
	std::cout << "test chain:\n" << chain << std::endl;
      }


      for (unsigned int ir=0 ; ir<chain.size() ; ir++ ) { 

	//	std::cout << "\troi " << ir << std::endl;

	TrackRoi& troi = chain.rois()[ir]; 
        TIDARoiDescriptor roi( troi.roi() );
	
	//extract beamline position values from rois
	const std::vector<double>& beamline = chain.rois()[ir].user();

	//set values of track analysis to these so can access elsewhere
	for ( int i=analyses.size() ; i-- ; ) {
          TrackAnalysis* analy_track = analyses[i];
          if ( !inputdata.isTagDefined("BeamRef") && beamline.size()>=2) {
            analy_track->setBeamRef(  beamline[0], beamline[1] );
            analy_track->setBeamTest( beamline[0], beamline[1] );
	  }
	}

	testTracks.clear();

	testTracks.selectTracks( troi.tracks() );

	/// trigger tracks already restricted by roi 
	std::vector<TrigInDetAnalysis::Track*> testp = testTracks.tracks();

	/// here we set the roi for the filter so we can request only those tracks 
	/// inside the roi 

	if ( select_roi ) dynamic_cast<Filter_Combined*>(refFilter)->setRoi(&roi);

	//	if ( debugPrintout ) { 
	//	  std::cout << "refTracks.size() " << refTracks.size() << " before roi filtering" << std::endl; 
	//	  std::cout << "filter with roi " << roi << std::endl; 
	//	}	    

  const std::vector<TrigInDetAnalysis::Track*>&  refp  = refTracks.tracks( refFilter );

	//	if ( debugPrintout ) { 
	//	  std::cout << "refp.size() " << refp.size() << " after roi filtering" << std::endl; 
	//	}	    


	//	std::cout << "ref tracks refp.size() "    << refp.size() << "\n" << refp  << std::endl;
	//	std::cout << "test tracks testp.size() " << testp.size() << "\n" << testp << std::endl;

	groi = &roi;

	if (truthMatch) {
	  truthMatcher.match( refp, testp );
	  analitr->second->execute( refp, testp, &truthMatcher );
	}
	else if (pTMatch_2) {
	  SecpTmatcher.match(refp, testp);
	  analitr->second->execute( refp, testp, &SecpTmatcher);
	}
	else {
	  dRmatcher.match(refp, testp);
	  analitr->second->execute( refp, testp, &dRmatcher );
	}


  if ( debugPrintout ) { 
    //	std::cout << "-----------------------------------\n\nselected tracks:" << chain.name() << std::endl;
    std::cout << "\nselected tracks:" << chain.name() << std::endl;
    std::cout << "ref tracks refp.size() "    << refp.size() << "\n" << refp  << std::endl;
    std::cout << "test tracks testp.size() " << testp.size() << "\n" << testp << std::endl;

    TrackAssociator::track_map::const_iterator titr = dRmatcher.TrackAssociator::matched().begin();
    TrackAssociator::track_map::const_iterator tend = dRmatcher.TrackAssociator::matched().end();
    int im=0;
    std::cout << "track matches:\n";
    while (titr!=tend) { 
      std::cout << "\t" << im++ << "\t" << *titr->first << " ->\n\t\t" << *titr->second << std::endl;
      ++titr;
    }


    std::cout << "completed : " << chain.name() << "\n";
    std::cout << "-----------------------------------" << std::endl;

  }

#if 0
	if ( dRmatcher.size()<refp.size() ) { 

	  if ( refp.size()==1 && testp.size()==0 ) { 
	    std::cout << track_ev->chains()[ic] <<std::endl;
	    std::cout << "roi\n" << track_ev->chains()[ic].rois()[ir] << endl; 
	  }
 
	}
#endif

        /// run the analysis for this chain

	if ( doPurity ) { 

	  const std::vector<TrigInDetAnalysis::Track*>&  refpp = refPurityTracks.tracks( refFilter );

	  testPurityTracks.clear();

	  testPurityTracks.selectTracks( troi.tracks() );
	  std::vector<TrigInDetAnalysis::Track*> testpp = testPurityTracks.tracks();

	  /// match the tracks	
	  if (truthMatch) {
	    truthMatcher.match( refp, testp );
	  }
	  else if (pTMatch_2){
	    SecpTmatcher.match( refp, testp);
	  }
	  else {
	    dRmatcher.match(refp, testp);
	  }

	  std::map<std::string,TrackAnalysis*>::iterator analitrp = analysis.find(chain.name()+"-purity");

	  if ( analitrp == analysis.end() ) continue;

	  /// run the analysis for this chain
	  if (truthMatch) {
	    analitrp->second->execute( refpp, testpp, &truthMatcher );
	  }
	  else if (pTMatch_2) {
	    analitrp->second->execute( refpp, testpp, &SecpTmatcher );
	  }
	  else {
	    analitrp->second->execute( refpp, testpp, &dRmatcher );
	  }
					



	  static int ecounter = 0;
	  ecounter++;
	}
      }
      
    }
  }


  std::cout << "done " << time_str() << "\tprocessed " << event_counter << " events\ttimes " << mintime << " " << maxtime << std::endl;

  //  hcorr->Finalise(Resplot::FitPoisson);

  hcorr->Finalise(); 
  hcorr->Write();

  for ( int i=analyses.size() ; i-- ; ) { 
    // std::cout << "finalise analysis chain " << analyses[i]->name() << std::endl;
    analyses[i]->finalise();
    delete analyses[i];
  }
  /// write out the histograms
  foutput.Write();
  foutput.Close();

  std::cout << "done" << std::endl;

  return 0;
}

