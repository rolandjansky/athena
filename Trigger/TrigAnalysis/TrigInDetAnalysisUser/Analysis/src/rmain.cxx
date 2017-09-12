
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
#include "TrigInDetAnalysis/TIDAEvent.h"
#include "TrigInDetAnalysis/TrackSelector.h"

#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
#include "TrigInDetAnalysisUtils/Filters.h"
#include "TrigInDetAnalysisUtils/Filter_Offline2017.h"
#include "TrigInDetAnalysisExample/NtupleTrackSelector.h"
#include "TrigInDetAnalysisExample/ChainString.h"
#include "TrigInDetAnalysisUtils/Associator_TruthMatch.h"

#include "TrigInDetAnalysis/Efficiency.h"

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"


#include "ReadCards.h"
#include "utils.h"

#include "RoiFilter.h"

#include "ConfAnalysis.h"
#include "PurityAnalysis.h"

#include "ConfVtxAnalysis.h"

#include "lumiList.h"
#include "lumiParser.h"
#include "dataset.h"

#include "event_selector.h"

#include "BinConfig.h"

#include "zbeam.h"

#include "computils.h"

///  globals for communicating with *Analyses
#include "globals.h"

// in ConfAnalysis

extern bool PRINT_BRESIDUALS;

// in BinConfig.cxx
extern BinConfig binConfig;

extern BinConfig electronBinConfig;
extern BinConfig muonBinConfig;
extern BinConfig tauBinConfig;
extern BinConfig bjetBinConfig;
extern BinConfig cosmicBinConfig;

void copyReleaseInfo( TTree* tree, TFile* foutdir );



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



/// convert string to integer with check if successful
int atoi_check( const std::string& s ) {
  int i = std::atoi( s.c_str() );
  char check[128];
  std::sprintf( check, "%d", i );
  if ( std::string(check)!=s ) return -1;
  else return i;
}




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

// Function to allow creation of an RoI for reference tracks, with custom widths, and which defaults to trigger roi values if none are specifed
TIDARoiDescriptor makeCustomRefRoi( const TIDARoiDescriptor& roi, 
				    double etaHalfWidth=-999, 
				    double phiHalfWidth=-999, 
				    double zedHalfWidth=-999 ) { 
				    
  double roi_eta = roi.eta();
  double roi_phi = roi.phi();
  double roi_zed = roi.zed();
  
  double etaMinus = roi.etaMinus();
  double etaPlus  = roi.etaPlus();

  double zedMinus = roi.zedMinus();
  double zedPlus  = roi.zedPlus();

  double phiMinus = roi.phiMinus();
  double phiPlus  = roi.phiPlus();

  if ( etaHalfWidth != -999 ) { 
    etaMinus = roi_eta - etaHalfWidth;
    etaPlus  = roi_eta + etaHalfWidth;
  }

  if ( phiHalfWidth != -999 ) { 
    phiMinus = roi_phi - phiHalfWidth; // !!! careful! will this always wrap correctly?
    phiPlus  = roi_phi + phiHalfWidth; // !!! careful! will this always wrap correctly?
  }
   
  if ( zedHalfWidth != -999 ) { 
    zedMinus = roi_zed - zedHalfWidth;
    zedPlus  = roi_zed + zedHalfWidth;
  }

  return TIDARoiDescriptor( roi_eta, etaMinus, etaPlus,
			    roi_phi, phiMinus, phiPlus, // do wrap phi here (done within TIDARoiDescriptor already)			   
			    roi_zed, zedMinus, zedPlus );
}


// Small function to be used in sorting tracks in track vectors
bool trackPtGrtr( TIDA::Track* trackA, TIDA::Track* trackB) { return ( std::fabs(trackA->pT()) > std::fabs(trackB->pT()) ); }


template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T*>& v ) { 
  for ( size_t i=0 ; i<v.size() ; i++ ) s << "\t" << *v[i] << "\n";
  return s;
}


template<typename T>
std::ostream& operator<<(std::ostream& s, const std::vector<T>& v ) {
  if ( v.size()<5 ) for ( unsigned i=0 ; i<v.size() ; i++ ) s << "\t" << v[i];
  else              for ( unsigned i=0 ; i<v.size() ; i++ ) s << "\n\t" << v[i];
  return s;
}


const std::vector<TIDA::Track> ibl_filter( const std::vector<TIDA::Track>& tv ) { 

  static TH2D* h  = 0;
  static TH2D* h2 = 0;

  static int ic = 0;


  if ( h==0 ) { 
    h  = new TH2D( "hzvphi",  "hzvphi", 150, -300, 300, 150, -M_PI, M_PI ); 
    h2 = new TH2D( "hzvphi2", "hzvphi", 150, -300, 300, 150, -M_PI, M_PI ); 
  }

  for ( size_t i=tv.size() ; i-- ; ) { 
    
    if ( tv[i].author()!=5 ) break;

    double eta = tv[i].eta();
      
    double theta = 2*std::atan(-std::exp(eta));

    double ribl = 34.2; // actually 32.26 - 36.21 mm

    double z = tv[i].z0() + ribl/std::tan(theta);

    if ( !tv[i].expectBL() ) { 
      std::cout << "missing IBL: phi: " << tv[i].phi() << "\tz: " << z << " (" << eta << " " << theta*180/3.14159 << ")" << std::endl; 
      if ( h ) h->Fill( z, tv[i].phi() );
    }
    else { 
      if ( h2 ) h2->Fill( z, tv[i].phi() );
    }
  }

  if ( ic>=500 ) { 
    ic = 0;
    if ( h ) { 
      h->DrawCopy();
      gPad->Print("zphimap.pdf");
      h2->DrawCopy();
      gPad->Print("zphimap2.pdf");
    }
  }

  ic++;

  return tv;
}


const std::vector<TIDA::Track> replaceauthor( const std::vector<TIDA::Track>& tv, int a0=5, int a1=4 ) { 

  if ( a0==a1 ) return tv;

  std::vector<TIDA::Track> tr;
  tr.reserve(tv.size());
  
  for ( size_t i=0 ; i<tv.size() ; i++ ) {
    const TIDA::Track& t = tv[i];
    int a = t.author();
    if ( a==a0 ) a=a1;
    tr.push_back( TIDA::Track( t.eta(),   t.phi(),  t.z0(),  t.a0(),  t.pT(),  t.chi2(), t.dof(), 
                               t.deta(),  t.dphi(), t.dz0(), t.da0(), t.dpT(),
                               t.bLayerHits(), t.pixelHits(), t.sctHits(), t.siHits(), 
                               t.strawHits(),  t.trHits(), 
                               t.hitPattern(), t.multiPattern(), a, t.hasTruth(),
                               t.barcode(), t.match_barcode(), t.expectBL(), t.id() ) );

  }

  return tr;

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




int usage(const std::string& name, int status) { 
  std::ostream& s = std::cout;
  
  s << "Usage: " << name << " <config filename> [OPTIONS]" << std::endl;
  s << "\nOptions: \n";
  s << " -o, -f, --file      value\toutput filename, \n";
  s << "     -b, --binConfig value\tconfig file for histogram configuration, \n";
  s << "     -r, --refChain  value\treference chain, \n";
  s << "     -t, --testChain value\ttest chain, \n";
  s << "     -p, --pdgId     value\tpdg ID of truth particle if requiring truth particle processing,\n";
  s << "     -n, --nofit           \ttest do not fit resplots, \n";
  s << "         --rms             \ttest force new rms95 errors, \n";
  //  s << "    -a, --all     \tadd all grids (default)\n";
  s << "     -h, --help           \tthis help\n";
  //  s << "\nSee " << PACKAGE_URL << " for more details\n"; 
  s << "\nReport bugs to sutt@cern.ch";
  s << std::endl;

  
  return status;
}




// #define DATA


// bool contains( const std::string& s, const std::string& r ) { 
//   return s.find(r)!=std::string::npos;
// }

template<typename T>
std::vector<T*> pointers( std::vector<T>& v ) {
  /// this is slow - all this copying
  std::vector<T*> _v(v.size());
  for ( unsigned i=0 ; i<v.size() ; i++ ) _v.push_back( &v[i] );
  return _v;
}


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

  bool useoldrms = true;
  bool nofit     = false;

  for ( int i=1 ; i<argc ; i++ ) { 
    if ( std::string(argv[i])=="-h" || std::string(argv[i])=="--help" ) {
      return usage(argv[0], 0);
    } 
    else if ( std::string(argv[i])=="-o" || std::string(argv[i])=="-f" || std::string(argv[i])=="--file" ) { 
      if ( ++i>=argc ) return usage(argv[0], -1);
      histofilename = argv[i];
      if ( histofilename.find(".root")==std::string::npos ) histofilename += ".root";
    }
    else if ( std::string(argv[i])=="-r" || std::string(argv[i])=="--refChain" ) { 
      if ( ++i>=argc ) return usage(argv[0], -1);
      refChain = argv[i];
    }
    else if ( std::string(argv[i])=="--rms" )   useoldrms = false;
    else if ( std::string(argv[i])=="-n" || std::string(argv[i])=="--nofit" ) nofit = true;
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

  /// true by default - command line options sets to false
  /// so should override what is in the config file 
  if ( useoldrms ) { 
      bool oldrms95 = true;
      inputdata.declareProperty( "OldRMS95", oldrms95 );
      std::cout << "setting Resplot old rms95 " << oldrms95 << std::endl;
      Resplot::setoldrms95( oldrms95 );
  }
  else {
    std::cout << "setting Resplot  old rms95 " << useoldrms << std::endl;
    Resplot::setoldrms95( useoldrms );
  }


  if ( nofit ) { 
    std::cout << "Not fitting resplots " << std::endl;
    Resplot::setnofit( nofit );
  }


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
  double zed    = 2000;

  int npix = 1;
  int nsct = 6;
  int nbl = -1;

  int nsiholes = 2;

  bool expectBL = false;

  double chi2prob = 0;

  //int npix_rec = 1; // JK removed (unused)
  //int nsct_rec = -1;  // JK removed (unused)

  //double pT_rec = 0;  // JK removed (unused)
  double eta_rec = 5;

  double Rmatch = 0.1;


  //bool printflag = false;  // JK removed (unused)

  bool truthMatch = false;

  if ( inputdata.isTagDefined("TruthMatch") )   truthMatch = ( inputdata.GetValue("TruthMatch") ? true : false );

  // CK: Option to specify which ref tracks to use from ref track vector, ordered by pT
  //     User parameter is a vector of ref track vector indices
  //     e.g. specifyPtOrderedRefTracks = {0, 1, 5} will use the first leading, second leading, and sixth leading track
  std::vector<int> refPtOrd_indices;
  bool use_pt_ordered_ref = false;
  
  if ( inputdata.isTagDefined("specifyPtOrderedRefTracks") ) {
    std::vector<double> refPtOrd_indices_tmp;

    use_pt_ordered_ref = true;
    refPtOrd_indices_tmp = ( inputdata.GetVector("specifyPtOrderedRefTracks") );
    
    for (unsigned int i = 0; i < refPtOrd_indices_tmp.size(); ++i) {
      refPtOrd_indices.push_back( refPtOrd_indices_tmp.at(i) );
    }
  }

  if ( inputdata.isTagDefined("pT") )      pT   = inputdata.GetValue("pT");

  /// here we set a pTMax value less than pT, then only set the max pT in the 
  /// filter if we read a pTMax value *greater* than pT
  double pTMax = pT-1;
  if ( inputdata.isTagDefined("pTMax") )   pTMax = inputdata.GetValue("pTMax");


  if ( inputdata.isTagDefined("NMod") )   NMod   = inputdata.GetValue("NMod");


  if ( inputdata.isTagDefined("eta") )      eta      = inputdata.GetValue("eta");
  if ( inputdata.isTagDefined("zed") )      zed      = inputdata.GetValue("zed");
  if ( inputdata.isTagDefined("npix") )     npix     = inputdata.GetValue("npix");
  if ( inputdata.isTagDefined("nsiholes") ) nsiholes = inputdata.GetValue("nsiholes");
  if ( inputdata.isTagDefined("expectBL") ) expectBL = ( inputdata.GetValue("expectBL") > 0.5 ? true : false );
  if ( inputdata.isTagDefined("nsct") )     nsct     = inputdata.GetValue("nsct");
  if ( inputdata.isTagDefined("nbl") )      nbl      = inputdata.GetValue("nbl");
  if ( inputdata.isTagDefined("chi2prob") ) chi2prob = inputdata.GetValue("chi2prob");

  /// only if not set from the command line
  if ( pdgId==0 && inputdata.isTagDefined("pdgId") ) pdgId = inputdata.GetValue("pdgId");
  

  //if ( inputdata.isTagDefined("npix_rec") ) npix_rec = inputdata.GetValue("npix_rec");  // JK removed (unused)
  //if ( inputdata.isTagDefined("nsct_rec") ) nsct_rec = inputdata.GetValue("nsct_rec");  // JK removed (unused)

  //if ( inputdata.isTagDefined("pT_rec") )  pT_rec  = inputdata.GetValue("pT_rec");  // JK removed (unused)
  if ( inputdata.isTagDefined("eta_rec") ) eta_rec = inputdata.GetValue("eta_rec");

  if ( inputdata.isTagDefined("a0") )           a0     = inputdata.GetValue("a0");
  if ( inputdata.isTagDefined("Rmatch") )       Rmatch = inputdata.GetValue("Rmatch");

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

  /// get the test chains 
  if ( testChains.size()==0 ) { ///NB: don't override command line args
    if      ( inputdata.isTagDefined("testChains") ) testChains = inputdata.GetStringVector("testChains");
    else if ( inputdata.isTagDefined("testChain") )  testChains.push_back( inputdata.GetString("testChain") );
  }

  /// new code - can extract vtx name, pt, any extra options that we want, 
  /// but also chop off everythiung after :post 

  std::vector<ChainString> chainConfig;

  for ( unsigned ic=0 ; ic<testChains.size() ; ic++ ) { 
    chainConfig.push_back( ChainString( testChains[ic] ) );
    testChains[ic] =  chainConfig.back().pre(); 
  }


  /// now any additional config parameters for the chain are available
  

  /// print soime debugging output
  //if ( inputdata.isTagDefined("printflag") )  printflag = ( inputdata.GetValue("printflag") ? 1 : 0 );  // JK removed (unused)

  /// select only tracks within the roi?
  bool select_roi = true;
  

  // CK: use a custom filter RoI for ref tracks using refFilter?
  //     Note that if the reference chain uses a non-full-scan RoI, the custom filter RoI could be wider than the
  //     ref chain RoI
  bool      use_custom_ref_roi = false;
  const int custRefRoi_nParams = 3;

  double custRefRoi_params[custRefRoi_nParams] = {-999., -999., -999.};
  std::vector<std::string> custRefRoi_chainList; // Vector of chain names to apply the custom roi to
  std::set<std::string>    customRoi_chains;

  if ( inputdata.isTagDefined("customRefRoi_etaHalfWidth") ) custRefRoi_params[0] = inputdata.GetValue("customRefRoi_etaHalfWidth");
  if ( inputdata.isTagDefined("customRefRoi_phiHalfWidth") ) custRefRoi_params[1] = inputdata.GetValue("customRefRoi_phiHalfWidth");
  if ( inputdata.isTagDefined("customRefRoi_zedHalfWidth") ) custRefRoi_params[2] = inputdata.GetValue("customRefRoi_zedHalfWidth");

  if ( inputdata.isTagDefined("customRefRoi_chainList") ) custRefRoi_chainList = inputdata.GetStringVector("customRefRoi_chainList");
  
  for ( unsigned ic=0 ; ic<custRefRoi_chainList.size() ; ic++ ) customRoi_chains.insert( custRefRoi_chainList[ic] );

  for ( int param_idx=0 ; param_idx<custRefRoi_nParams ; param_idx++ ) {
    if ( custRefRoi_params[param_idx] != -999 ) {
      select_roi         = true; // In case select_roi is ever set to default to false
      use_custom_ref_roi = true;
    }
  }

  // Clean this up, it's a right mess and could surely be automated
  if ( use_custom_ref_roi ) {
    std::cout << "****                                              \t****" << std::endl;
    std::cout << "**** Custom RoI will be used to filter ref. tracks\t****" << std::endl;
    if ( !custRefRoi_chainList.empty()  ) {
      std::cout << "****    Applying custom RoI only to specified chains\t****" << std::endl;
 
      if ( custRefRoi_params[0] != -999. ) std::cout << "****    etaHalfWidth = " << custRefRoi_params[0] << "\t\t\t\t****" << std::endl;
      else                                 std::cout << "****    etaHalfWidth = value used in trigger RoI\t****" << std::endl;
      
      if ( custRefRoi_params[1] != -999. ) std::cout << "****    phiHalfWidth = " << custRefRoi_params[1] << "\t\t\t\t****" << std::endl;
      else                                 std::cout << "****    phiHalfWidth = value used in trigger RoI\t****" << std::endl;
      
      if ( custRefRoi_params[2] != -999. ) std::cout << "****    zedHalfWidth = " << custRefRoi_params[2] << "\t\t\t\t****" << std::endl;
      else                                 std::cout << "****    zedHalfWidth = value used in trigger RoI\t****" << std::endl;
    }
  }
  
  // Checking for SelectRoi after any other options that will set select_roi, to ensure that the value set
  // here will be used
  if ( inputdata.isTagDefined("SelectRoi") )  { 
    select_roi = ( inputdata.GetValue("SelectRoi")!=0 ? true : false );
  }

  if ( !select_roi ) { 
    std::cout << "****                                               ****" << std::endl;
    std::cout << "**** RoI filtering of reference tracks is disabled ****" << std::endl;
    std::cout << "****                                               ****" << std::endl;
  }

  //  bool selectfake_roi = false;  // JK removed (unused)
  //  if ( inputdata.isTagDefined("SelectFakeRoi") )  { 
  //     selectfake_roi = ( inputdata.GetValue("SelectFakeRoi")!=0 ? true : false );  // JK removed (unused)
  //  }
  

  std::vector<double> _lumiblocks;
  lumiParser  goodrunslist;

  
  if ( inputdata.isTagDefined("GRL") )  { 
    /// read the (xml?) GRL 
    std::cout << "Reading GRL from: " <<  inputdata.GetString("GRL") << std::endl;
    goodrunslist.read( inputdata.GetString("GRL") );
    //    std::cout << goodrunslist << std::endl;
  }
  else if ( inputdata.isTagDefined("LumiBlocks") )  { 
    /// else get the list from the dat file directly
    _lumiblocks = inputdata.GetVector("LumiBlocks");
    
    for (unsigned int i=0 ; i<_lumiblocks.size()-2 ; i+=3 ){ 
      goodrunslist.addRange( _lumiblocks[i],  _lumiblocks[i+1],  _lumiblocks[i+2] );  
    }
  }
  

  /// reference vertex selection 
  
  std::string vertexSelection = "";
  if ( inputdata.isTagDefined("VertexSelection") ) vertexSelection = inputdata.GetString("VertexSelection");
  
  
  bool bestPTVtx  = false;
  bool bestPT2Vtx = false;
  int  vtxind     = -1;
  
  if ( vertexSelection!="" ) { 
    if      ( vertexSelection=="BestPT" )  bestPTVtx  = true;
    else if ( vertexSelection=="BestPT2" ) bestPT2Vtx = true;
    else vtxind = atoi_check( vertexSelection );
  }
  
  
  
  
  /// test vertex selection 
  
  std::string vertexSelection_rec = "";
  if ( inputdata.isTagDefined("VertexSelectionRec") ) vertexSelection_rec = inputdata.GetString("VertexSelectionRec");
  
  bool bestPTVtx_rec  = false;
  bool bestPT2Vtx_rec = false;
  int  vtxind_rec     = -1;
  
  if ( vertexSelection_rec!="" ) { 
    if      ( vertexSelection_rec=="BestPT" )  bestPTVtx_rec  = true;
    else if ( vertexSelection_rec=="BestPT2" ) bestPT2Vtx_rec = true;
    else vtxind_rec = atoi_check( vertexSelection_rec ); 
  }
  
  


#if 0

  //////////////////////////////////////////////////////////////////
  /// don't use these three options any longer 

  bool useBestVertex = false;
  if ( inputdata.isTagDefined("useBestVertex") ) useBestVertex = ( inputdata.GetValue("useBestVertex") ? 1 : 0 );

  bool useSumPtVertex = true;
  if ( inputdata.isTagDefined("useSumPtVertex") ) useSumPtVertex = ( inputdata.GetValue("useSumPtVertex") ? 1 : 0 );

  int MinVertices = 1;
  if ( inputdata.isTagDefined("MinVertices") ) MinVertices = inputdata.GetValue("MinVertices");

  /////////////////////////////////////////////////////////////////

#endif


  
  /// is this option needed any longer ???
  int NVtxTrackCut = 2;
  if ( inputdata.isTagDefined("NVtxTrackCut") ) NVtxTrackCut = inputdata.GetValue("NVtxTrackCut");


  std::vector<double> event_list;
  bool event_selector_flag = false;

  if ( inputdata.isTagDefined("EventSelector") ) event_list = inputdata.GetVector("EventSelector");

  event_selector es(event_list);

  if ( es.size() ) event_selector_flag = true;


  std::vector<double> beamTest;
  std::vector<double> beamRef;


  bool correctBeamlineRef  = false;
  bool correctBeamlineTest = false;

  if ( inputdata.isTagDefined("CorrectBeamlineRef") )   correctBeamlineRef  = ( inputdata.GetValue("CorrectBeamlineRef") == 0 ? false : true );
  if ( inputdata.isTagDefined("CorrectBeamlineTest") )  correctBeamlineTest = ( inputdata.GetValue("CorrectBeamlineTest") == 0 ? false : true );


  if ( inputdata.isTagDefined("BeamTest") )     beamTest = inputdata.GetVector("BeamTest");
  else { 
    if ( inputdata.isTagDefined("BeamTestx") )  beamTest.push_back(inputdata.GetValue("BeamTestx"));
    if ( inputdata.isTagDefined("BeamTesty") )  beamTest.push_back(inputdata.GetValue("BeamTesty"));
  }
  
  
  if ( inputdata.isTagDefined("BeamRef") )      beamRef = inputdata.GetVector("BeamRef");
  else { 
    if ( inputdata.isTagDefined("BeamRefx") )   beamRef.push_back(inputdata.GetValue("BeamRefx"));
    if ( inputdata.isTagDefined("BeamRefy") )   beamRef.push_back(inputdata.GetValue("BeamRefy"));
  }
  


  if ( ( beamTest.size()!=0 && beamTest.size()!=2 && beamTest.size()!=3 ) || 
       (  beamRef.size()!=0 &&  beamRef.size()!=2 &&  beamRef.size()!=3 ) ) {
    std::cerr << "incorrectly specified beamline position" << std::endl;
    return (-1);
  }

  if ( beamTest.size()>0 ) correctBeamlineTest = true;
  if (  beamRef.size()>0 ) correctBeamlineRef  = true;

  if ( correctBeamlineRef )  std::cout << "main() correcting beamline for reference tracks" << std::endl;
  if ( correctBeamlineTest ) std::cout << "main() correcting beamline for test tracks"      << std::endl;
  
  

  if ( beamRef.size()>0 )  std::cout << "beamref  " << beamRef   << std::endl;
  if ( beamTest.size()>0 ) std::cout << "beamtest " << beamTest << std::endl;

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


  bool monitorZBeam = false;
  if ( inputdata.isTagDefined("MonitorinZBeam") )  monitorZBeam = ( inputdata.GetValue("MonitorZBeam")==0 ? false : true );

  std::cout << "dbg " << __LINE__ << std::endl;

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

  int selectcharge = 0;
  if ( inputdata.isTagDefined("Charge") ) selectcharge = inputdata.GetValue("Charge");


  std::cout << "using reference " << refChain << std::endl;
  if ( refChain=="Truth" ) std::cout << "using pdgId " << pdgId << std::endl;

  /// track filters 

  /// truth articles

  /// reminder: this is the *new* constructor - it now has too many parameters 
  //  Filter_Track( double etaMax,  double d0Max,  double z0Max,   double  pTMin,
  //                int  minPixelHits, int minSctHits, int minSiHits, int minBlayerHits,
  //                int minStrawHits, int minTrHits, double prob=0 ) :

  /// filters for true selection for efficiency

  Filter_Vertex filter_vertex(a0v, z0v);

  Filter_Track filter_offline( eta, 1000, zed, pT, 
                               npix, nsct, -1, nbl, 
                               -2, -2, chi2prob, 
                               20, 20, nsiholes, expectBL ); /// include chi2 probability cut 

  if ( selectcharge!=0 ) filter_offline.chargeSelection( selectcharge );
  if ( pTMax>pT )        filter_offline.maxpT( pTMax );

  Filter_etaPT filter_etaPT(eta,pT);


  //  std::cout << "pdgId " << pdgId << std::endl;
  //Select only true particles matching pdgId
  Filter_pdgId filter_pdgtruth(pdgId); //BP

  // select inside-out offline tracking

  //  Filter_TruthParticle filter_passthrough(&filter_offline);
  //  Filter_Track filter_track( eta, 1000,  2000,     pT, npix, nsct,   -1, -1,  -2, -2);

  Filter_Author    filter_inout(0);

  int author = 0;

  Filter_Author    filter_auth(author);

  Filter_TrackQuality filter_q(0.01);  
  Filter_Combined   filter_off(&filter_offline, &filter_vertex);

  Filter_Combined  filter_truth( &filter_pdgtruth, &filter_etaPT);

  Filter_Combined  filter_muon( &filter_offline, &filter_vertex);

  Filter_Track      filter_onlinekine(  eta_rec, 1000, 2000, pT,    -1,  npix,  nsct, -1,  -2, -2);
  Filter_Vertex     filter_onlinevertex(a0vrec, z0vrec);
  Filter_Combined   filter_online( &filter_onlinekine, &filter_onlinevertex ); 

  Filter_Track      filter_offkinetight(  5, 1000, 2000, pT,    -1,  0,  0, -1,  -2, -2);
  Filter_Combined   filter_offtight( &filter_offkinetight, &filter_inout ); 



  Filter_Offline2017* filter_offline2017 = 0;
  Filter_Combined*    filter_off2017     = 0;
  /// track selectors so we can select multiple times with different 
  /// filters if we want (simpler then looping over vectors each time 

  TrackFilter* refFilter;
  TrackFilter* truthFilter;


  if ( inputdata.isTagDefined("Filter" ) ) { 
    ChainString filter = inputdata.GetString("Filter");
    std::cout << "Filter: " << inputdata.GetString("Filter") << " : " << filter << std::endl;
    if ( filter.head()=="Offline2017" ) {
      std::string filter_type = filter.tail();
      filter_offline2017 = new Filter_Offline2017( pT, filter_type ); 
      filter_off2017     = new Filter_Combined ( filter_offline2017, &filter_vertex);
      refFilter = filter_off2017;
    }
    else { 
      std::cerr << "unimplemented Filter requested: " << filter.head() << std::endl; 
      return -1; 
    }
  }
  else { 
    if      ( refChain=="Offline" )             refFilter = &filter_off;
    else if ( contains( refChain,"Electrons") ) refFilter = &filter_off;
    else if ( contains( refChain, "Muons" ) )   refFilter = &filter_muon;
    else if ( contains( refChain,"1Prong" ) )   refFilter = &filter_off;  // tau ref chains
    else if ( contains( refChain,"3Prong" ) )   refFilter = &filter_off;  // tau ref chains
    else if ( refChain=="Truth" && pdgId!=0 )   refFilter = &filter_truth;
    else if ( refChain=="Truth" && pdgId==0 )   refFilter = &filter_off;
    else { 
      std::cerr << "unknown reference chain defined" << std::endl;
      return (-1);
    }
  }

  if (pdgId==0) truthFilter = &filter_off;
  else truthFilter = &filter_truth;
 

  // use an actual filter requiring at least 1 silicon hit
  // to get rid of the EF trt only tracks 

  std::cout << "filter_passthrough" << std::endl;

  Filter_Track filter_passthrough( 10, 1000,  2000, 0, -2, -2, 1, -2,  -2, -2);

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
  
  for ( unsigned  i=0 ; i<test_chains.size() ; i++ ) {

    std::string chainname = test_chains[i];

    std::vector<std::string> chainnames;

    chainnames.push_back(chainname);

    //    std::cout << "chain name " << chainname << "\t:" << chainnames.back() << " : " << chainnames.size() << std::endl;

    ConfAnalysis* analy_conf = new ConfAnalysis(chainnames.back(), chainConfig[i] );
    analy_conf->initialiseFirstEvent(initialiseFirstEvent);
    analy_conf->initialise();
    analy_conf->setprint(false);

    std::string vtxTool = chainConfig[i].postvalue("rvtx");

    if ( vtxTool!="" ) { 
      ///  don't use the vertex name as the directory, since then we cannot 
      ///  easily plot on the same plot  
      ConfVtxAnalysis* anal_confvtx = new ConfVtxAnalysis( vtxTool );
      // ConfVtxAnalysis* anal_confvtx = new ConfVtxAnalysis( "vertex" );
      analy_conf->store().insert( anal_confvtx, "rvtx" );
    }


    // analy_conf->setprint(true);

    /// hooray !! Set any additional aspects to the analysis up here
    /// so should be able to set individual, chain related variables
    /// eg set individual pt limits, different selection of reference
    /// objects, whether to run a vertex analysis ...

    /// When setting different pt cuts, then an additional track selector 
    /// will need to be created, this can then be added to the 
    /// TIDA::FeatireStore of the analysis, and retrieved each time 
    /// it is required 

    if ( chainConfig[i].values().size()>0 ) { 
      std::cout << "chain:: " << chainname << "\t(" << chainConfig[i] << " : size " << chainConfig[i].values().size() << ")" << std::endl; 
      for ( unsigned ik=chainConfig[i].values().size() ; ik-- ; ) {
	std::cout << "\tchainconfig: " << ik << "\tkey " << chainConfig[i].keys()[ik] << " " << chainConfig[i].values()[ik] << std::endl; 
      }
    }
  

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
      std::cout << "purity " << (chainnames[0]+"-purity") << std::endl;
      analp->initialise();
      analp->setprint(false);
      // analp->setprint(true);
      analysis[chainnames[0]+"-purity"] = analp;
      analyses.push_back(analp);
    }

  }

  std::cout << "main() finished looping" << std::endl;

  /// track selectors for efficiencies

  bool fullyContainTracks = true;

  if ( inputdata.isTagDefined("FullyContainTracks") ) { 
    fullyContainTracks = ( inputdata.GetValue("FullyContainTracks")==0 ? false : true ); 
  }

  dynamic_cast<Filter_Combined*>(refFilter)->setDebug(debugPrintout);
  dynamic_cast<Filter_Combined*>(refFilter)->containtracks(fullyContainTracks);

  NtupleTrackSelector  refTracks( refFilter );
  NtupleTrackSelector  offTracks( testFilter );
  NtupleTrackSelector testTracks( testFilter);

  NtupleTrackSelector truthTracks( truthFilter );

  //  	NtupleTrackSelector  refTracks( &filter_passthrough );
  //  	NtupleTrackSelector  testTracks( refFilter );
  //    NtupleTrackSelector  refTracks(  &filter_pdgtruth);
  //    NtupleTrackSelector  testTracks( &filter_off );
  //    NtupleTrackSelector testTracks(&filter_roi);


  /// do we want to filter the RoIs ? 

  bool filterRoi = false;

  double roieta       = 0;
  bool   roicomposite = false;
  size_t roimult       = 1;
  
  if ( inputdata.isTagDefined( "FilterRoi" ) ) { 

    filterRoi = true;

    std::vector<double> filter_values = inputdata.GetVector( "FilterRoi" );

    if ( filter_values.size()>0 ) roieta       =   filter_values[0];
    if ( filter_values.size()>1 ) roicomposite = ( filter_values[1]==0 ? false : true );
    if ( filter_values.size()>2 ) roimult      = int(filter_values[2]+0.5);
 
  }

  RoiFilter roiFilter( roieta, roicomposite, roimult );

  /// Determine what sort of matching is required ...

  TrackAssociator* _matcher = 0;

  if      ( useMatcher == "Sigma" )    _matcher = new Associator_BestSigmaMatcher("sigma", Rmatch); 
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
  else if ( useMatcher == "pT_2" ) { 
    double pTmatchLim_2 = 1.0;
    if ( inputdata.isTagDefined("Matcher_pTLim_2") ) pTmatchLim_2 = inputdata.GetValue("Matcher_pTLim_2");
    _matcher = new Associator_SecondBestpTMatcher("SecpT", pTmatchLim_2);
  }
  else if ( useMatcher == "Truth" ) {  
    _matcher = new Associator_TruthMatcher();
  }
  else { 
    /// default to deltaR matcher
    /// track matcher for best fit deltaR matcher
    _matcher = new Associator_BestDeltaRMatcher("deltaR", Rmatch); 
  }
  
  /// extra matcher for additionally matching reference to truth 
  TrackAssociator* truth_matcher = 0;
  if ( truthMatch ) { 
    if      ( useMatcher == "Sigma" )    truth_matcher = new Associator_BestSigmaMatcher("sigma_truth", Rmatch); 
    else if ( useMatcher == "DeltaRZ" || useMatcher == "DeltaRZSinTheta" )  { 
      double deta = 0.05;
      double dphi = 0.05;
      double dzed = 25;
      if ( inputdata.isTagDefined("Matcher_deta" ) ) deta = inputdata.GetValue("Matcher_deta"); 
      if ( inputdata.isTagDefined("Matcher_dphi" ) ) dphi = inputdata.GetValue("Matcher_dphi"); 
      if ( inputdata.isTagDefined("Matcher_dzed" ) ) dzed = inputdata.GetValue("Matcher_dzed"); 
  
      if ( useMatcher == "DeltaRZ" ) truth_matcher = new Associator_BestDeltaRZMatcher(         "deltaRZ_truth", deta, dphi, dzed ); 
      else                           truth_matcher = new Associator_BestDeltaRZSinThetaMatcher( "deltaRZ_truth", deta, dphi, dzed ); 
    }
    else if ( useMatcher == "pT_2" ) { 
      double pTmatchLim_2 = 1.0;
      if ( inputdata.isTagDefined("Matcher_pTLim_2") ) pTmatchLim_2 = inputdata.GetValue("Matcher_pTLim_2");
      truth_matcher = new Associator_SecondBestpTMatcher("SecpT_truth", pTmatchLim_2);
    }
    else if ( useMatcher == "Truth" ) {  
      truth_matcher = new Associator_TruthMatcher();
    }
    else { 
      /// default to deltaR matcher
      /// track matcher for best fit deltaR matcher
      truth_matcher = new Associator_BestDeltaRMatcher("deltaR_truth", Rmatch); 
    }
  }
  

  // NtupleTrackSelector  roiTracks( refFilter );


  /// track selectors for purities

  NtupleTrackSelector  refPurityTracks( &filter_inout );
  NtupleTrackSelector  testPurityTracks( &filter_online );

  // get the list of input files

  std::vector<std::string> filenames;


  if ( inputdata.isTagDefined("DataSets") ) {  

    std::cout << "fetching dataset details" << std::endl;
    std::vector<std::string> datasets = inputdata.GetStringVector("DataSets");
    for (unsigned int ids=0 ; ids<datasets.size() ; ids++ ) {
      std::cout << "\tdataset " << datasets[ids] << std::endl; 
      dataset d( datasets[ids] );
      std::vector<std::string> filenames_ = d.datafiles();
      std::cout << "\tdataset contains " << filenames_.size() << " files" << std::endl; 
      filenames.insert(filenames.end(), filenames_.begin(),filenames_.end());
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

  std::string release_data_save = "";

  if ( show_release ){

    bool first = true;

    for ( unsigned i=0 ; first && i<filenames.size() ; i++ ) {
    
      TFile* finput = TFile::Open( filenames[i].c_str() );


      if ( finput==0 || !finput->IsOpen() || finput->IsZombie() ) {
	std::cerr << "Error: could not open input file: " << filenames[i] << std::endl;
	exit(-1);
      }
  
      TTree*   dataTree    = (TTree*)finput->Get("dataTree");
      TString* releaseData = new TString("");
      
      if ( dataTree ) { 
	dataTree->SetBranchAddress( "ReleaseMetaData", &releaseData);
	
	for (unsigned int i=0; i<dataTree->GetEntries() ; i++ ) {
	  dataTree->GetEntry(i);      
	  release_data.push_back( releaseData->Data() );
	  if (  release_data_save != release_data.back() ) { 
	    std::cout << "main() release data: " << release_data.back() << " : " << *releaseData << std::endl;
	  }
	  first = false;
	  release_data_save = release_data.back();
	}
      }

      if ( finput ) delete finput;
    }      
    
    //    for ( unsigned ird=0 ; ird<release_data.size() ; ird++ ) std::cout << "presort  " << ird << " " << release_data[ird] << std::endl;

    if ( !release_data.empty() ) { 
      std::sort(release_data.begin(), release_data.end());
      release_data.erase(std::unique(release_data.begin(), release_data.end()), release_data.end());

      //      for ( unsigned ird=0 ; ird<release_data.size() ; ird++ ) std::cout << "postsort " << ird << " " << release_data[ird] << std::endl;
    
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



  int addedfiles = 0;



  unsigned _Nentries = 0;

  if ( inputdata.isTagDefined("Nentries") ) { 
    _Nentries = unsigned(inputdata.GetValue("Nentries"));
  }



  unsigned Nentries = 0; //data->GetEntries();

  unsigned event_counter = 0;
    

  typedef std::pair<int,double> zpair; 
  std::vector<zpair>  refz;
  std::vector<zpair>  testz;
  
  std::vector<double> beamline_ref;
  std::vector<double> beamline_test;
  
  int maxtime = 0;
  int mintime = 0;

  std::cout << "opening files" << std::endl;

  bool run = true;

  int  pregrl_events = 0;
  int  grl_counter   = 0;


  std::cout << "starting event loop " << time_str() << std::endl;
    

  for ( unsigned ifile=0 ; run && ifile<filenames.size() && ( nfiles==0 || ifile<nfiles ) ; ifile++ ) { 

    bool newfile = true;

    addedfiles++;  

    TFile*  finput = TFile::Open( filenames[ifile].c_str() );

    if ( finput==0 || !finput->IsOpen() || finput->IsZombie() ) {
      std::cerr << "Error: could not open output file " << filenames[ifile] << std::endl;
      continue;
    }
  
    TTree* data = (TTree*)finput->Get("tree");

    TIDA::Event* track_ev = new TIDA::Event();

    gevent = track_ev;

    data->SetBranchAddress("TIDA::Event",&track_ev);

    /// TChain *data = new TChain("tree");
    /// data->AddFile(filenames[ifile].c_str());
    //  }

    //    std::cout << "added " << addedfiles << " files" << std::endl;

    //  if (addedfiles == 0) {
    //  std::cout << "Cannot read any files, exiting" << std::endl;
    //  return (-1);
    // }



    /// so we can specify the number of entries 
    /// we like, rather than run on all of them
    
    //  unsigned Nentries = data->GetEntries();
    //  if ( inputdata.isTagDefined("Nentries") ) { 
    //   unsigned _Nentries = unsigned(inputdata.GetValue("Nentries"));
    //    if ( _Nentries<Nentries ) Nentries = _Nentries;
    //  }
    
    
    maxtime = track_ev->time_stamp();
    mintime = track_ev->time_stamp();
    
    unsigned cNentries = data->GetEntries();
    
    bool skip = true;
    
    /// so we can specify the number of entries 
    /// we like, rather than run on all of them
    for (unsigned int i=0; skip && run && i<cNentries ; i++ ) {

      
      //      if ( _Nentries<Nentries ) { 
      //	run = false;
      //	break;
      //     }
      //    Nentries++;

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


    
    bool ingrl = goodrunslist.inRange( r, lb );

    pregrl_events++;

    /// check whether in good lumi block range
    if ( !ingrl ) continue;
 
    grl_counter++;


    /// check whether it's in the event selector list
    if ( event_selector_flag && !es.in( event ) ) continue;

    if ( mintime>ts ) mintime = ts;
    if ( maxtime<ts ) maxtime = ts;

    if ( _Nentries>0 && event_counter>_Nentries ) { 
      run = false;
      std::cout << "breaking out " << run << std::endl;
      break;
    }

    event_counter++;
    Nentries++;

    //    if ( !elist.find(event) ) continue;

    //    std::cout << "run " << r << "\tevent " << event << "\tlb " << lb << std::endl;

    hevent->Fill( event );

 

    if ( filenames.size()<2 ) { 
      if ( (Nentries<10) || i%(Nentries/10)==0 || i%1000==0 || debugPrintout )  { 
	std::cout << "run "      << track_ev->run_number() 
		  << "\tevent "  << track_ev->event_number() 
		  << "\tlb "     << track_ev->lumi_block() 
		  << "\tchains " << track_ev->chains().size()
		  << "\ttime "   << track_ev->time_stamp();
	std::cout << "\t : processed " << i << " events so far (" << int((1000*i)/Nentries)*0.1 << "%)\t" << time_str() << std::endl;
	//   std::cerr << "\tprocessed " << i << " events so far \t" << time_str() << std::endl;
      }
    }  

    if ( newfile ) { 

      int pfiles = filenames.size();
      if ( nfiles>0 ) pfiles = nfiles; 

      std::cout << "file entries=" << data->GetEntries();

      if ( data->GetEntries()<100 )   std::cout << " ";
      if ( data->GetEntries()<1000 )  std::cout << " ";
      if ( data->GetEntries()<10000 ) std::cout << " ";
	
      std::cout << "\t";


      std::cout << "run "      << track_ev->run_number() 
		<< "\tevent "  << track_ev->event_number() 
		<< "\tlb "     << track_ev->lumi_block() 
		<< "\tchains " << track_ev->chains().size()
		<< "\ttime "   << track_ev->time_stamp();
      
      std::cout << "\t : processed " << ifile << " files so far (" << int((1e3*ifile)/pfiles)*0.1 << "%)\t" << time_str() << std::endl;

      newfile = false;
    }


    //		if ( printflag )  std::cout << *track_ev << std::endl;   
    
    r = track_ev->run_number();

    /// get the reference tracks (or perhaps even the true tracks !!!

    offTracks.clear();
    refTracks.clear();
    truthTracks.clear();
    refPurityTracks.clear();
    //    offlineTracks.clear();

    
    Nvtxtracks = 0;

    const std::vector<TIDA::Chain>& chains = track_ev->chains();


    dynamic_cast<Filter_Combined*>(truthFilter)->setRoi(0);
    //// get the truth tracks if required
    if ( truthMatch ) { 
      for (unsigned int ic=0 ; ic<chains.size() ; ic++ ) {
	if ( chains[ic].name()=="Truth" ) {
 	  truthTracks.selectTracks( chains[ic].rois()[0].tracks() );
	  break;
	}
      }
    }

    //// get the reference tracks
    for (unsigned int ic=0 ; ic<chains.size() ; ic++ ) {
      if ( chains[ic].name()==refChain ) {
        offTracks.selectTracks( chains[ic].rois()[0].tracks() );
	//extract beamline position values from rois
	beamline_ref = chains[ic].rois()[0].user();
	// std::cout << "beamline: " << chains[ic].name() << "  " << beamline_ref << std::endl;
	break;
      }
    }

    
    /// select the reference offline vertices
    
    std::vector<TIDA::Vertex> vertices;
    
    const std::vector<TIDA::Vertex>& mv = track_ev->vertices();

    int     selectvtx = -1;
    double  selection = 0;
    
    //    std::vector<TIDA::Vertex>& vertices = vertices;
     
    if ( bestPTVtx || bestPT2Vtx )  {  
      for ( unsigned iv=0 ; iv<mv.size() ; iv++ ) {
	if ( mv[iv].Ntracks()==0 ) continue;
	double selection_ = 0.0;
	for (unsigned itr=0; itr<offTracks.tracks().size(); itr++){
	  TIDA::Track* tr = offTracks.tracks().at(itr);
	  if( std::fabs(mv[iv].z()-tr->z0()) < 1.5 ) { 
	    if      ( bestPTVtx  ) selection_ += std::fabs(tr->pT());
	    else if ( bestPT2Vtx ) selection_ += std::fabs(tr->pT())*std::fabs(tr->pT()); 
	  }
	}
	if( selection_>selection){
	  selection = selection_;
	  selectvtx = iv;
	}
      }
      if ( selectvtx!=-1 ) vertices.push_back( mv[selectvtx] );
    }
    else if ( vtxind>=0 ) {
      if ( unsigned(vtxind)<mv.size() ) vertices.push_back( mv[vtxind] );
    }
    else { 
      for ( unsigned iv=0 ; iv<mv.size() ; iv++ ) vertices.push_back( mv[iv] );
    }
    
    
    if ( vertices.size()>0 ) std::cout << "vertex " << vertices[0] << std::endl;
    else                     std::cout << "NO vertex !!!" << std::endl;

    /// always push back the vector - if required there will be only one vertex on it
    filter_vertex.setVertex( vertices );

    /// calculate number of "vertex tracks"

    NvtxCount = 0;

    for ( unsigned iv=0 ; iv<mv.size() ; iv++ ) {
      int Ntracks = mv[iv].Ntracks();
      if ( Ntracks>NVtxTrackCut ) { /// do we really want this cut ???
	Nvtxtracks += Ntracks;
	//	vertices.push_back( mv[iv] );
        NvtxCount++;
      }
    }

    //    filter_vertex.setVertex( vvtx ) ;
    hcorr->Fill( vertices.size(), Nvtxtracks ); 




    dynamic_cast<Filter_Combined*>(refFilter)->setRoi(0);

    /// get the tracks from the reference chain - shouldn't be one
    /// of the test chains, since there will be a 1-to-1 mapping
    /// but might be a useful check
    bool foundReference = false;

    const TIDA::Chain* refchain = 0;

    for (unsigned int ic=0 ; ic<chains.size() ; ic++ ) { 
      if ( chains[ic].name()==refChain ) { 
	refchain = &chains[ic];
     	foundReference = true;
	//Get tracks from within reference roi
        //        ibl_filter( chains[ic].rois()[0].tracks() ); 
	refTracks.selectTracks( chains[ic].rois()[0].tracks() );
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

      TIDA::Chain& chain = track_ev->chains()[ic];

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
	

	/// get the rois and filter on them if required 

	//	std::cout << "\troi " << ir << std::endl;

	TIDA::Roi& troi = chain.rois()[ir]; 
        TIDARoiDescriptor roi( troi.roi() );

	/// do we want to filter on the RoI properties? 
	/// If so, if the RoI fails the cuts, then skip this roi

	if ( filterRoi && !roiFilter.filter( roi ) ) continue; 
	
	
	/// select the test sample (trigger) vertices
	const std::vector<TIDA::Vertex>& mvt = troi.vertices();





	std::vector<TIDA::Vertex> vertices_test;
	
	int     selectvtx = -1;
	double  selection = 0;
	
	if ( bestPTVtx_rec || bestPT2Vtx_rec )  {  
	  
	  const std::vector<TIDA::Track>& recTracks = troi.tracks();
	  
	  for ( unsigned iv=0 ; iv<mvt.size() ; iv++ ) {
	    double selection_ = 0.0;
	    for (unsigned itr=0; itr<recTracks.size(); itr++){
	      const TIDA::Track* tr = &recTracks[itr];
	      if( std::fabs(mvt[iv].z()-tr->z0()) < 1.5 ) { 
		if      ( bestPTVtx_rec  ) selection_ += std::fabs(tr->pT());
		else if ( bestPT2Vtx_rec ) selection_ += std::fabs(tr->pT())*std::fabs(tr->pT()); 
	      }
	    }
	    if( selection_>selection){
	      selection = selection_;
	      selectvtx = iv;
	    }
	  }
	  if ( selectvtx!=-1 ) vertices_test.push_back( mvt[selectvtx] );
	}
	else if ( vtxind_rec!=-1 ) {
	  if ( unsigned(vtxind_rec)<mvt.size() )       vertices_test.push_back( mvt[vtxind] );
	}
	else { 
	  for ( unsigned iv=0 ; iv<mvt.size() ; iv++ ) vertices_test.push_back( mvt[iv] );
	}




	
	
	//extract beamline position values from rois
	//	const std::vector<double>& beamline = chain.rois()[ir].user();
	beamline_test = chain.rois()[ir].user();
	
	//	std::cout << "beamline: " << chain.name() << "  " << beamline_test << std::endl; 

	//set values of track analysis to these so can access elsewhere
	for ( int i=analyses.size() ; i-- ; ) {

          TrackAnalysis* analy_track = analyses[i];
	  
	  if ( correctBeamlineTest ) { 
	    if      ( beamTest.size()==2 ) analy_track->setBeamTest( beamTest[0], beamTest[1] );
	    //	    else if ( beamTest.size()==3 ) analy_track->setBeamTest( beamTest[0], beamTest[1], beamTest[2] );
	    else { 
	      if ( !inputdata.isTagDefined("BeamTest") ) {
		if      ( beamline_test.size()==2 ) analy_track->setBeamTest( beamline_test[0], beamline_test[1] );
		//		else if ( beamline_test.size()==3 ) analy_track->setBeamTest( beamline_test[0], beamline_test[1], beamline_test[2] );
	      }
	    }
	  }
	  
	  if ( correctBeamlineRef ) { 
	    if      ( beamRef.size()==2 ) analy_track->setBeamRef( beamRef[0], beamRef[1] );
	    //	    else if ( beamRef.size()==3 ) analy_track->setBeamRef( beamRef[0], beamRef[1], beamRef[2] );
	    else { 
	      if ( !inputdata.isTagDefined("BeamRef") ) { 
		if      ( beamline_ref.size()==2 ) analy_track->setBeamRef( beamline_ref[0], beamline_ref[1] );
		//		else if ( beamline_ref.size()==3 ) analy_track->setBeamRef( beamline_ref[0], beamline_ref[1], beamline_ref[2] );
	      }
	    }
	  }
	  
	}
	
	testTracks.clear();

	testTracks.selectTracks( troi.tracks() );
	
	/// trigger tracks already restricted by roi - so no roi filtering required 
        std::vector<TIDA::Track*> testp = testTracks.tracks();
	
	/// here we set the roi for the filter so we can request only those tracks 
	/// inside the roi

	TIDARoiDescriptor refRoi;
	
	if ( select_roi ) {

	  bool customRefRoi_thisChain = false;

	  if ( use_custom_ref_roi ) { // Ideally just want to say ( use_custom_ref_roi && (chain.name() in custRefRoi_chain]sist) )
	    if ( customRoi_chains.size() ) { 
	      if ( customRoi_chains.find( chain.name() )!=customRoi_chains.end() ) customRefRoi_thisChain = true;
	    }
	    else customRefRoi_thisChain = true;  // Apply custom ref roi to all chains
	  }
	  
	  if ( use_custom_ref_roi && customRefRoi_thisChain ) { 
	    refRoi = makeCustomRefRoi( roi, custRefRoi_params[0], custRefRoi_params[1], custRefRoi_params[2] ); 
	  }
	  else refRoi = roi;

	  dynamic_cast<Filter_Combined*>(refFilter)->setRoi(&refRoi);
	}
	
	//	if ( debugPrintout ) { 
	//	  std::cout << "refTracks.size() " << refTracks.size() << " before roi filtering" << std::endl; 
	//	  std::cout << "filter with roi " << roi << std::endl; 
	//	}

	
	// Now filterng ref tracks by refFilter, and performing any further filtering and selecting,
	// before finally creating the const reference object refp

	std::vector<TIDA::Track*>  refp_vec = refTracks.tracks( refFilter );
	
	// Selecting only truth matched reference tracks
	if ( truthMatch ) { 	  
	  /// get the truth particles ...
	  if ( select_roi ) dynamic_cast<Filter_Combined*>(truthFilter)->setRoi(&roi);
	  const std::vector<TIDA::Track*>&  truth = truthTracks.tracks(truthFilter);
	  const std::vector<TIDA::Track*>&  refp_tmp = refp_vec;

	  /// truth_matcher match against current reference selection 
	  truth_matcher->match( refp_tmp, truth );

	  std::vector<TIDA::Track*>  refp_matched;

	  /// which truth tracks have a matching reference track ?
	  for ( unsigned i=0 ; i<refp_vec.size() ; i++ ) { 
	    if ( truth_matcher->matched( refp_vec[i] ) ) refp_matched.push_back( refp_vec[i] );
	  }

	  refp_vec.clear();
	  refp_vec = refp_matched;
	}
	
	// Choose the pT ordered refp tracks that have been asked for by the user
	if ( use_pt_ordered_ref ) {
	  std::sort(refp_vec.begin(), refp_vec.end(), trackPtGrtr); // Should this sorting be done to a temporary copied object, instead of the object itself?
	  
	  int nRefTracks = refp_vec.size();

	  std::vector<TIDA::Track*> refp_chosenPtOrd;

	  // Checking if any user specifed track indices are out of bounds for this event
	  for (unsigned int indexIdx = 0; indexIdx < refPtOrd_indices.size(); ++indexIdx) {
	    if (refPtOrd_indices.at(indexIdx) > nRefTracks) {
	      std::cout << "WARNING: for event " << event << ", pT ordered reference track at vector position " << refPtOrd_indices.at(indexIdx) << " requested but not found" << std::endl;
	    }
	  }
	  
	  for (int trackIdx = 0; trackIdx < nRefTracks; ++trackIdx) {
	    for (unsigned int indexIdx = 0; indexIdx < refPtOrd_indices.size(); ++indexIdx) {
	      if ( trackIdx == refPtOrd_indices.at(indexIdx) ) {
	        refp_chosenPtOrd.push_back(refp_vec.at(trackIdx));
		break;
	      }
	    }
	  }

	  refp_vec.clear();
	  refp_vec = refp_chosenPtOrd; // Note: not necessarily pT ordered.
	                                     //       Ordered by order of indices the user has passed
	                                     //       (which is ideally pT ordered e.g. 0, 1, 3)
	}


	/// remove any tracks below the pt threshold if one is specifed for the analysis

	ConfAnalysis* cf = dynamic_cast<ConfAnalysis*>( analitr->second );

	if ( cf ) { 
	  std::string ptconfig = cf->config().postvalue("pt");
	  if ( ptconfig!="" ) { 
	    double pt = std::atof( ptconfig.c_str() );
	    if ( pt>0 ) { 
	      std::vector<TIDA::Track*> reft; reft.reserve(refp_vec.size());
	      for ( std::vector<TIDA::Track*>::const_iterator itr=refp_vec.begin() ; itr!=refp_vec.end() ; itr++ ) { 
		if ( std::fabs((*itr)->pT())>=pt ) reft.push_back( *itr );
	      }
	      refp_vec = reft;
	    }
 	  }
	}

	
	const std::vector<TIDA::Track*>&  refp  =  refp_vec;
	
	//	if ( debugPrintout ) { 
	//	  std::cout << "refp.size() " << refp.size() << " after roi filtering" << std::endl; 
	//	}	    


	//	std::cout << "ref tracks refp.size() "    << refp.size() << "\n" << refp  << std::endl;
	//	std::cout << "test tracks testp.size() " << testp.size() << "\n" << testp << std::endl;

	groi = &roi;




	/// now found all the tracks within the RoI - *now* if required find the 
	/// the count how many of these reference tracks are on each of the 
	/// offline vertices


	std::vector<TIDA::Vertex> vertices_roi;


	if ( chain.name().find("SuperRoi") ) { 
	  
	  /// select the reference offline vertices
	  
	  vertices_roi.clear();
	  
	  const std::vector<TIDA::Vertex>& mv = vertices;
	    
	  //	  std::cout << "vertex filtering " << mv.size() << std::endl;  

	  for ( unsigned iv=0 ; iv<mv.size() ; iv++ ) {

	    const TIDA::Vertex& vx = mv[iv];

	    //	    std::cout << "\t" << iv << "\t" << vx << std::endl;

	    int trackcount = 0;

	    for (unsigned itr=0; itr<refp.size(); itr++){
	      TIDA::Track* tr = refp[itr];
	      double theta_     = 2*std::atan(std::exp(-tr->eta())); 
	      double dzsintheta = std::fabs(vx.z()-tr->z0()) * std::sin(theta_);
	      if( dzsintheta < 1.5 ) trackcount++;
	    }

	    vertices_roi.push_back( TIDA::Vertex( vx.x(), vx.y(), vx.z(),  
						  vx.dx(), vx.dy(), vx.dz(),
						  trackcount, 
						  vx.chi2(), vx.ndof() ) ); // ndof not valid for only Roi tracks 

	    //	    std::cout << "\t \t" << vertices_roi.back() << std::endl;

	  }
	  
	}
	else vertices_roi = vertices;


	foutdir->cd();

	// do analysing
	
	if ( monitorZBeam ) { 
	  if ( beamline_ref.size()>2 && beamline_test.size()>2 ) { 
	    refz.push_back(  zpair( lb, beamline_ref[2]) );
	    testz.push_back( zpair( lb, beamline_test[2]) );
	  }
	}

	_matcher->match( refp, testp);
	
	analitr->second->execute( refp, testp, _matcher );

	ConfVtxAnalysis* vtxanal = 0;
	analitr->second->store().find( vtxanal, "rvtx" );

	if ( vtxanal ) {	  
	  /// AAAAAARGH!!! because you cannot cast vector<T> to const vector<const T>
	  ///  we first need to copy the actual elements from the const vector, to a normal
	  ///  vector. This is because if we take the address of elements of a const vector<T>
	  ///  they will by of type const T*, so we can only add them to a vector<const T*>
	  ///  and all our functions are using const vector<T>&, so we would need to duplicate
	  ///  all the functions to allow over riding with vector<T*> *and* vector<const T*>
	  ///  to get this nonsense to work
	  
	  std::vector<TIDA::Vertex> vtxcck = vertices_roi;           
	  std::vector<TIDA::Vertex*> vtxp; // = pointers( vtxcck );
	  
	  vtxp.reserve( vtxcck.size() );
	  for ( unsigned iv=0 ; iv<vtxcck.size() ; iv++ ) vtxp.push_back( &vtxcck[iv] );
	  
	  std::vector<TIDA::Vertex> vtxcck_test = vertices_test;
	  std::vector<TIDA::Vertex*> vtxp_test; // = pointers( vtxcck_test );
	  
	  vtxp_test.reserve( vtxcck_test.size() );
	  for ( unsigned iv=0 ; iv<vtxcck_test.size() ; iv++ ) vtxp_test.push_back( &vtxcck_test[iv] );
	  
	  //	  std::cout << "vertex size :" << vtxp_test.size() << "\tvertex key " << vtxanal->name() << std::endl;

	  vtxanal->execute( vtxp, vtxp_test, track_ev );

	}

	
	if ( debugPrintout ) { 
	  //	std::cout << "-----------------------------------\n\nselected tracks:" << chain.name() << std::endl;
	  std::cout << "\nselected tracks:" << chain.name() << std::endl;
	  std::cout << "ref tracks refp.size() "    << refp.size() << "\n" << refp  << std::endl;
	  std::cout << "test tracks testp.size() " << testp.size() << "\n" << testp << std::endl;
	  
	  TrackAssociator::map_type::const_iterator titr = _matcher->TrackAssociator::matched().begin();
	  TrackAssociator::map_type::const_iterator tend = _matcher->TrackAssociator::matched().end();
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
	if ( _matcher->size()<refp.size() ) { 
	  
	  if ( refp.size()==1 && testp.size()==0 ) { 
	    std::cout << track_ev->chains()[ic] <<std::endl;
	    std::cout << "roi\n" << track_ev->chains()[ic].rois()[ir] << endl; 
	  }
 
	}
#endif

        /// run the analysis for this chain

	if ( doPurity ) { 
	  
	  const std::vector<TIDA::Track*>&  refpp = refPurityTracks.tracks( refFilter );

	  testPurityTracks.clear();

	  testPurityTracks.selectTracks( troi.tracks() );
	  std::vector<TIDA::Track*> testpp = testPurityTracks.tracks();

	  _matcher->match(refpp, testpp); /// ???
	    

	  std::map<std::string,TrackAnalysis*>::iterator analitrp = analysis.find(chain.name()+"-purity");
	  
	  if ( analitrp == analysis.end() ) continue;


	  analitrp->second->execute( refpp, testpp, _matcher );
	  

	  static int ecounter = 0;
	  ecounter++;
	}
      }
      
    }
  }

  delete track_ev;
  delete data;
  delete finput;

  //  std::cout << "run: " << run << std::endl;

  }

  std::cout << "done " << time_str() << "\tprocessed " << event_counter << " events"
	    << "\ttimes "  << mintime << " " << maxtime 
	    << "\t( grl: " << grl_counter << " / " << pregrl_events << " )" << std::endl;

  if ( monitorZBeam ) zbeam _zbeam( refz, testz );

  foutdir->cd();
  
  //  hcorr->Finalise(Resplot::FitPoisson);

  hcorr->Finalise(); 
  hcorr->Write();

  for ( int i=analyses.size() ; i-- ; ) { 
    // std::cout << "finalise analysis chain " << analyses[i]->name() << std::endl;
    analyses[i]->finalise();
    
    ConfVtxAnalysis* vtxanal = 0;
    analyses[i]->store().find( vtxanal, "rvtx" );
    if ( vtxanal ) vtxanal->finalise();
    
    delete analyses[i];
  }
  /// write out the histograms
  foutput.Write();
  foutput.Close();

  std::cout << "done" << std::endl;

  return 0;
}

