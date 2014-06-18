/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKSectorMap.h"
#include "TrigFTKSim/FTKConstantBank.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/PatternBank.h"
#include "TrigFTKSim/FTKTrack.h"
#include "common_fcn.h"

#include <TSystem.h>
#include <TFile.h>
#include <TFitter.h>
#include <TRandom3.h>
#include <TMath.h>
#include <Math/PdfFuncMathCore.h>
#include <TH1F.h>
#include <TFoam.h>
#include <TFoamIntegrand.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

// verbosity level for the random walk
unsigned int debugWalk(0);

// burnin cicles for the random  walk
unsigned nburn(1000);
// total number of attemptes
unsigned long long nattempts(10000000);
// number of cells in the TFoam objects
Int_t NCells(100000);
// number of MC events used to build the foam
Int_t NSamples(200000);

// Plane map pointer, to be set during the initialization
FTKPlaneMap *pmap(0);
// Super-strip map, this object transform an hit in a SS index
FTKSSMap *ssmap(0);
// use hashmap internally in AM simulation (allows super-small SS widths)
bool force_am_hashmap(false);
// region map used in this session
FTKRegionMap *rmap(0);


FTKConstantBank *fitconst(0x0);
string bankpath;
string sectors_path;

// shared objects for the initialization
FTKTrack *curtrack;
double *maxcoords;

double minpar[5] = {-1/800,-2,-100,-30,-3};
double maxpar[5] = {1/800,2,100,30,3};
int read_commands();

typedef map<vector<int>,int> PatternMap;

class TFFoamDistr : public TFoamIntegrand {
protected:
  const FTKConstantBank *m_fitconst; //!
  int m_sector;

  FTKTrack m_tmptrack; //

  unsigned int m_ncoords;
  double *m_maxcoords; //[m_ncoords]

public:
  TFFoamDistr(const FTKConstantBank*, int, const FTKTrack&, double*);
  virtual ~TFFoamDistr() {;}

  virtual Double_t Density(Int_t, Double_t*);

};

TFFoamDistr::TFFoamDistr(const FTKConstantBank *fc, int secid, 
			 const FTKTrack &trk, double *maxs) : 
  TFoamIntegrand(), m_fitconst(fc), m_sector(secid), 
  m_tmptrack(trk), m_ncoords(trk.getNCoords())
{
  m_maxcoords = new double[m_ncoords];
  for (unsigned int ix=0;ix!=m_ncoords;++ix) m_maxcoords[ix] = maxs[ix];
}


Double_t TFFoamDistr::Density(Int_t ndim, Double_t *pos) {
  // set the coordinates
  for (unsigned int ix=0;ix!=m_ncoords;++ix) {
    m_tmptrack.setCoord(ix,pos[ix]*m_maxcoords[ix]);
  }      
  m_fitconst->linfit(m_sector,m_tmptrack);

  return ROOT::Math::chisquared_pdf(m_tmptrack.getChi2(),m_fitconst->getNConstr());
}

/** main function */
int main(int argc, char *argv[]) {
  cout << "Sector random walk, in patterns" << endl;
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  ftkset.setVerbosity(0);
  
  common_fcn_init(argc, argv);

  // preliminary parsing of the input file
  ifstream optionfile;
  streambuf *oldrdbuf = cin.rdbuf();
  if (argc > 1) {
    // input option in an external file
    optionfile.open(argv[1]);
    if (!optionfile) {
      cerr << "*** Error reading input file: " << argv[1] << endl;
      return -1;
    }
    // input option parsed from stdin
    cin.rdbuf(optionfile.rdbuf());
  }
  // call the parser
  if (read_commands() < 0) {
    cerr << "*** Error parsing the options" << endl;
    return -1;
  }
  cin.rdbuf(oldrdbuf);

  PatternBank sectors(sectors_path.c_str());
  sectors.InitPreload();

  PatternMap newbank; // map for the new patterns

  PatternBank bank;
  const int nplanes(pmap->getNPlanes());
  bank.setNPlanes(nplanes);
  const int ncoords(pmap->getTotalDim());
  maxcoords = new double[ncoords];
  curtrack = new FTKTrack(ncoords,nplanes);

  // set eventual random walk debugging
  TFile *debugfile = TFile::Open("debugfoam.root","recreate");

  for (int ipl=0;ipl!=nplanes;++ipl) {
    int ix = pmap->getDim(ipl,0);
    int iy = pmap->getDim(ipl,1);

    if (iy==-1) { // 1-D
      maxcoords[ix] = ssmap->getMap(ipl,0,0).m_phiwidth;
    }
    else {
      maxcoords[ix] = ssmap->getMap(ipl,0,0).m_phiwidth;
      maxcoords[iy] = ssmap->getMap(ipl,0,0).m_etawidth;
    }
  }


  int npars = fitconst->getNPars();
  float *pars = new float[npars];
  float *x = new float[ncoords];
  Double_t *mcx = new Double_t[ncoords];

  TRandom *rndgen = new TRandom3(0);
  for (int isec=0;isec!=fitconst->getNSectors()&&isec<5;++isec) { // sector loop

    // prepare the directory in the debug file
    TDirectory *curdir = debugfile->mkdir(Form("Sec%d",isec));
    curdir->cd();

    cout << "Initializing the TFoam object for sector " << isec << endl;
    // instance the Foam and its integrand
    TFoam *curfoam = new TFoam(Form("FoamSec%d",isec));
    TFoamIntegrand *integrand = new TFFoamDistr(fitconst,isec,*curtrack,maxcoords);
    // setup the main parameters
    curfoam->SetkDim(ncoords);
    curfoam->SetRho(integrand);
    curfoam->SetPseRan(rndgen);
    // custom parameters (from the example)
    Int_t nBin = 8;   // Number of bins in build-up
    Int_t OptRej = 1;   // Wted events for OptRej=0; wt=1 for OptRej=1 (default)
    Int_t OptDrive = 2;   // (D=2) Option, type of Drive =0,1,2 for TrueVol,Sigma,WtMax
    Int_t EvPerBin = 25;   // Maximum events (equiv.) per bin in buid-up
    Int_t Chat = 1;   // Chat level
    curfoam->SetnCells(NCells);    // optional
    curfoam->SetnSampl(NSamples);    // optional
    curfoam->SetnBin(nBin);      // optional
    curfoam->SetOptRej(OptRej);    // optional
    curfoam->SetOptDrive(OptDrive);  // optional
    curfoam->SetEvPerBin(EvPerBin);  // optional
    curfoam->SetChat(Chat);      // optional
    // initialization and registration
    curfoam->Initialize(); // make the foam structures
    curfoam->Write("Foam"); // save the foam for debugging reasons
    long nCalls = curfoam->GetnCalls();
    if (curfoam->GetPrimary()==0.) {
      cerr << "Primary is 0, skip this sector" << endl;
      continue;
    }
    cout << "Start generation sector " << isec << endl;

    
    TH1F *histowgt = new TH1F("histo_wgt","Wgt distr",50,0,1.25);
    TH1F **histox = new TH1F*[ncoords];
    TH1F **histomcx = new TH1F*[ncoords];
    for (int ix=0;ix!=curtrack->getNCoords();++ix) {
      histomcx[ix] = new TH1F(Form("histo_mcx%d",ix),Form("MC X[%d]",ix),100,0,1);
      histox[ix] = new TH1F(Form("histo_x%d",ix),Form("X[%d]",ix),maxcoords[ix],0,maxcoords[ix]);
    }

    TH1F *histochi2 = new TH1F("histochi2","Chi2",50,0,50);

    // get the current pattern definition
    const PatternDefinition &secdef = sectors.getPatternFast(isec);

    unsigned int naccepted(0);
    unsigned long long ngenerations = nattempts*secdef.getNEvents();
    unsigned long long steps = (ngenerations+9)/10;
    for (unsigned long long itry=0;itry!=ngenerations;++itry) { // extraction step
      if (itry%steps==0) cout << itry << " / " << ngenerations << endl;


      curfoam->MakeEvent();      
      curfoam->GetMCvect(mcx);
      Double_t wgt = curfoam->GetMCwt();
      histowgt->Fill(wgt);

      // set the coordinates
      for (int ix=0;ix!=curtrack->getNCoords();++ix) {
	x[ix] = mcx[ix]*maxcoords[ix];
	curtrack->setCoord(ix,x[ix]);
	histomcx[ix]->Fill(mcx[ix]);
	histox[ix]->Fill(x[ix]);
      }      
      fitconst->linfit(isec,*curtrack);

      // check the track parameters
      if (curtrack->getChi2()>50) continue;
      //if (TMath::Abs(curtrack->getPt())<.8e3) continue;
      //if (TMath::Abs(curtrack->getIP())>2.) continue;
      //if (TMath::Abs(curtrack->getZ0())>110.) continue;


      naccepted += 1;
      histochi2->Fill(curtrack->getChi2());
      
      vector<int> ssids;
      ssids.reserve(nplanes+1);
      // use the configuration
      for (int ipl=0;ipl!=nplanes;++ipl) {
	int ix = pmap->getDim(ipl,0);
	int iy = pmap->getDim(ipl,1);
	int ndim = (iy==-1 ? 1 : 2);
	FTKHit tmphit(ndim);	    
	tmphit.setSector(secdef.getSSID(ipl));
	tmphit.setPlane(ipl);
	tmphit.setCoord(0,curtrack->getCoord(ix));
	if (ndim==2) tmphit.setCoord(1,curtrack->getCoord(iy));

	ssids.push_back(ssmap->getSSGlobal(tmphit));
      }
      ssids.push_back(isec);
      
      // insert a new pattern or increase the coverage of an existing one
      PatternMap::iterator ipatt = newbank.find(ssids);
      if (ipatt==newbank.end()) {
	newbank[ssids] = 1;
      }
      else {
	(*ipatt).second += 1;
      }
    } // end generation loop

    curdir->Write();
    cout << "Number of good tracks: " << naccepted << " (" << static_cast<float>(naccepted)/ngenerations << ')' << endl;
    cout << "End generation sector " << isec << endl;

    Double_t eps = 0.0005;
    Double_t Effic, WtMax, AveWt, Sigma;
    Double_t IntNorm, Errel;
    Double_t MCresult, MCerror;
    curfoam->Finalize(IntNorm, Errel);     // final printout
    curfoam->GetIntegMC( MCresult, MCerror);  // get MC intnegral
    curfoam->GetWtParams(eps, AveWt, WtMax, Sigma); // get MC wt parameters
    Effic=0; if(WtMax>0) Effic=AveWt/WtMax;
    
    cout << "================================================================" << endl;
    cout << " MCresult= " << MCresult << " +- " << MCerror << " RelErr= "<< MCerror/MCresult << endl;
    cout << " Dispersion/<wt>= " << Sigma/AveWt << endl;
    cout << "      <wt>/WtMax= " << Effic <<",    for epsilon = "<<eps << endl;
    cout << " nCalls (initialization only) =   " << nCalls << endl;
    cout << "================================================================" << endl;
    
    delete integrand;
  } // end sector loop

  debugfile->Close();

  cout << "Number of patterns: " << newbank.size() << endl;
  PatternMap::iterator ipatt = newbank.begin();
  for (int id=0;ipatt!=newbank.end();++id) {
    PatternDefinition newpatt(id,nplanes,(*ipatt).first[nplanes],(*ipatt).second);
    cout << id;
    for (int ipl=0;ipl!=nplanes;++ipl) {
      newpatt.setSSID(ipl,(*ipatt).first[ipl]);
      cout << " " << (*ipatt).first[ipl];
    }
    cout << " " << (*ipatt).first[nplanes];
    cout << " " << (*ipatt).second << endl;
    bank.addPattern(newpatt);
    ++ipatt;    
  }
  bank.SaveAs(bankpath.c_str());

  delete [] x;
  delete [] mcx;
  delete [] pars;
  
  return 0;
}

/** this function parse the input commands and prepare a simple structure
    used by the main function */
int read_commands() {
  // ftk environemnt
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  
  const char prompt[] = "PARSE> ";
  
  string line;
  while (replace_getline(cin, line) != 0) {
    if (line[0] == '#' || line.size() == 0) {
      continue;
    }
    
    cout << prompt << line << endl;
    
    // extraxt the pair (key,value)
    istringstream sline(line);
    string key;
    sline >> key;
    if (key == "IGNORE_SIGSEGV") {
      // disable ROOT signal handler (it's buggy and may cause deadlocks after segfault)
      int ignore_sigsegv;
      sline >> ignore_sigsegv;
      if (ignore_sigsegv)
	gSystem->IgnoreSignal(kSigSegmentationViolation, true);
    } else if (key == "VERBOSITY") {
      int ival;
      sline >> ival;
      ftkset.setVerbosity(ival);
    } else if (key == "SS_FILE") {
      string sval;
      sline >> sval;
      cout << prompt << key << " = " << sval << endl;
      ssmap = new FTKSSMap(rmap, sval.c_str(), force_am_hashmap == false);
    } else if (key == "PMAP_FILE") {
      // load the plane map configuration file
      string sval;
      sline >> sval;
      pmap = new FTKPlaneMap(sval.c_str());
      if (!(*pmap)) {
	cerr << "*** error loading plane map in: " << sval << endl;
	return -1;
      }
    } else if (key == "RMAP_FILE") {
      // set rmap file
      string sval;
      sline >> sval;
      rmap = new FTKRegionMap(pmap, sval.c_str());
      if (!(*rmap)) {
	cerr << "*** error using map from: " << sval << endl;
	return -1;
      }
    } else if (key == "BURNIN") {
      sline >> nburn;
    } else if (key == "NATTEMPTS") {
      sline >> nattempts;
    } else if (key == "NCELLS") {
      sline >> NCells;
    } else if (key == "NSAMPLES") {
      sline >> NSamples;
    } else if (key == "DEBUGRW") {
      sline >> debugWalk;
    } else if (key == "BANK") {
      string fcpath;
      replace_getline(cin, fcpath);
      replace_getline(cin, sectors_path);
      replace_getline(cin, bankpath);
      fitconst = new FTKConstantBank(pmap->getTotalDim(), fcpath.c_str());
    }
  }

  return 0;
}
