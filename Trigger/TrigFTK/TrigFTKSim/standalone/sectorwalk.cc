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
#include <TFitter.h>
#include <TRandom3.h>
#include <TMath.h>
#include <Math/PdfFuncMathCore.h>

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
string besttrack_path;
string sectors_path;

// shared objects for the initialization
FTKTrack *curtrack;
double *maxcoords;

double minpar[5] = {-1/800,-2,-100,-30,-3};
double maxpar[5] = {1/800,2,100,30,3};
int read_commands();

typedef map<vector<int>,int> PatternMap;

void chisquare_setup(Int_t &npar, Double_t *, Double_t &res, Double_t *coords, int);

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
  ofstream fdebugwalk;
  if (debugWalk) {
    fdebugwalk.open("walkdebug.dat");
    fdebugwalk << "secid chi2";
    for (int ix=0;ix!=ncoords;++ix) {
      fdebugwalk << " x" << ix;
    }
    fdebugwalk << " pT phi d0 eta z0";
    for (int ipl=0;ipl!=nplanes;++ipl) {
      fdebugwalk << " SS" << ipl;
    }
    fdebugwalk << endl;
  }

  float *walksd = new float[ncoords];
  for (int ipl=0;ipl!=nplanes;++ipl) {
    int ix = pmap->getDim(ipl,0);
    int iy = pmap->getDim(ipl,1);

    if (iy==-1) { // 1-D
      maxcoords[ix] = ssmap->getMap(ipl,0,0).m_phiwidth;
      walksd[ix] = 8.*0;
    }
    else {
      maxcoords[ix] = ssmap->getMap(ipl,0,0).m_phiwidth;
      maxcoords[iy] = ssmap->getMap(ipl,0,0).m_etawidth;
      walksd[ix] = 5.*0;
      walksd[iy] = 3.*0;
    }
  }

  walksd[0] = 5.;
  walksd[1] = 3.;
  walksd[2] = 5.;
  walksd[3] = 3.;
  walksd[4] = 5.;
  walksd[5] = 3.;
  walksd[6] = 8.;
  walksd[7] = 8.;
  walksd[8] = 8.;
  walksd[9] = 8.;

  // initialize besttrack
  ifstream besttrackf(besttrack_path.c_str());
  // skip the first line with the header
  string btline;
  getline(besttrackf,btline);

  int npars = fitconst->getNPars();
  int nconstr = fitconst->getNConstr();
  int secid;
  float *pars = new float[npars];
  float oldchi2;
  float *x = new float[ncoords];

  gRandom->SetSeed(0);
  for (int isec=0;isec!=fitconst->getNSectors()&&isec<5;++isec) { // sector loop
    cout << "Start generation sector " << isec << endl;

    // get the starting point
    getline(besttrackf,btline);
    cout << "Starting point: " << btline << endl;
    istringstream inputbt(btline);
    inputbt >> secid;
    if (secid!=isec) {
      cerr << "Synchronization problem " << secid << "!=" << isec << endl;
      assert(0);
    }

    // read the coordinates
    for (int ix=0;ix!=ncoords;++ix) {
      inputbt >> x[ix];
    }
    inputbt >> oldchi2; // chi2
    for (int ip=0;ip!=npars;++ip) {
      inputbt >> pars[ip];
    }

    // check the starting point consistency
    for (int ix=0;ix!=curtrack->getNCoords();++ix) {
      curtrack->setCoord(ix,x[ix]);
    }
    fitconst->linfit(secid,*curtrack);
    if (TMath::Abs(oldchi2-curtrack->getChi2())>1e-3) {
      cerr << "Large Chi2 difference, possible problem in the constants: |" << oldchi2 << " - " << curtrack->getChi2() << "| = " << TMath::Abs(oldchi2-curtrack->getChi2()) << endl;
      curtrack->Print();
    }

    // start a random walk starting from the best track
    unsigned int naccepted(0);
    unsigned long long steps = (nattempts+9)/10;
    for (unsigned long long itry=0;itry!=nattempts;++itry) { // extraction step
      if (itry%steps==0) cout << itry << " / " << nattempts << endl;

      // get the current pattern definition
      const PatternDefinition &secdef = sectors.getPatternFast(isec);

      // Extract the possible track
      for (int ix=0;ix!=curtrack->getNCoords();++ix) {
	double tempx(-1);
	do {
	  if (walksd[ix]>=1) 
	    tempx = static_cast<double>(static_cast<int>(gRandom->Gaus(x[ix],walksd[ix])));
	  else tempx = x[ix];
	} while (tempx<0 || tempx>=maxcoords[ix]);

	curtrack->setCoord(ix,tempx);
      }      
      fitconst->linfit(secid,*curtrack);
      // check the track parameters
      //if (TMath::Abs(curtrack->getPt())<.8e3) continue;
      //if (TMath::Abs(curtrack->getIP())>2.) continue;
      //if (TMath::Abs(curtrack->getZ0())>110.) continue;

      Double_t alpha = ROOT::Math::chisquared_pdf(curtrack->getChi2(),nconstr)/ROOT::Math::chisquared_pdf(oldchi2,nconstr);
      Double_t refalpha(gRandom->Uniform());
      
      if (refalpha<alpha) {
	naccepted += 1;
	// move the values
	for (int ix=0;ix!=ncoords;++ix) x[ix] = curtrack->getCoord(ix);
	oldchi2 = curtrack->getChi2();

	//cout << "Good track" << endl;
	// curtrack->Print();
	if (itry>=nburn) {

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

	  PatternMap::iterator ipatt = newbank.find(ssids);
	  if (ipatt==newbank.end()) {
	    newbank[ssids] = 1;
	  }
	  else {
	    (*ipatt).second += 1;
	  }

	  if (debugWalk) {
	    fdebugwalk << isec << " " << curtrack->getChi2();
	    for (int ix=0;ix!=ncoords;++ix) {
	      fdebugwalk << " " << curtrack->getCoord(ix);
	    }
	    fdebugwalk << " " << curtrack->getPt() \
		       << " " << curtrack->getPhi() \
		       << " " << curtrack->getIP() \
		       << " " << curtrack->getEta() \
		       << " " << curtrack->getZ0();
	    for (int ipl=0;ipl!=nplanes;++ipl) {
	      fdebugwalk << " " << ssids[ipl];
	    }
	    fdebugwalk << endl;
	  }


	}
      }
    } // end extraction step

    cout << "Number of good tracks: " << naccepted << " (" << static_cast<float>(naccepted)/(nattempts-nburn) << ')' << endl;
    cout << "End generation sector " << isec << endl;
  } // end sector loop

  if (debugWalk) fdebugwalk.close();

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
  delete [] pars;
  delete [] walksd;
  
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
    } else if (key == "DEBUGRW") {
      sline >> debugWalk;
    } else if (key == "BANK") {
      string fcpath;
      replace_getline(cin, fcpath);
      replace_getline(cin, sectors_path);
      replace_getline(cin, besttrack_path);
      replace_getline(cin, bankpath);
      fitconst = new FTKConstantBank(pmap->getTotalDim(), fcpath.c_str());
    }
  }

  return 0;
}
