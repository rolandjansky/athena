/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <TAxis.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TEventList.h>
#include <TF1.h>
#include <TGraph.h>
#include <TPaveText.h>
#include <TTree.h>

#include "ChamberDerivs.h"
#include "ChamberNames.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "Tools.h"
//#include "TBox.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "TBranch.h"
#include "TDecompBK.h"
#include "TDecompSVD.h"
#include "TGraphErrors.h"
#include "TH2F.h"
#include "TMatrix.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TMatrixTUtils.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TSystem.h"
#include "TVectorD.h"

using namespace CLHEP;

void initSDTree();
void initMTPPTree();
void makeNewTree();

bool getSDTreeEntry(int ientry, int chid);
bool getMTPPTreeEntry(int& ientrySD, int run, int evt, int ntrk, int chid = -1);
const int getNumberMatrixIndices();
void getChamberIdMap(string filename);
int getChIdFromString(const string& chname);
void addDerivatives();
void fillTree();
void make2DAlignHists(const ChamberDerivs& chDerivs, TPad** pads);
void makeHitmap(const ChamberDerivs& chDerivs, TPad* pad);
void plotDiagonalCutHists(bool offdiag = false);

int getXcoordinate(const string& chambername);
int getYcoordinate(const string& chambername);
int get1Dcoordinate(const string& chambername);
int hitMapCoord(int coord, const string& chambername);

void getAllSingularValues(const ChamberDerivs& chDerivs, vector<double>& svArray);

void addConstraints(ChamberDerivs* chDerivs);
void addSoftConstraints(ChamberDerivs* chDerivs);
void addExternalConstraints(ChamberDerivs* chDerivs, const std::string& filename);

// double* fixAlongTrack(double* secDerivs) const;

void makeSomePlots(const ChamberDerivs& chDerivs);
TH1F** alignParDist(const ChamberDerivs& chDerivs, const string& histname, double* range = 0);

void getFinalPars(double* initPar, double* deltas, double* finalPars);

bool trackSelection();

void decomposeTransform(const HepGeom::Transform3D& transform, double* values);

double m_rotscale = 1.;

bool DEBUG = true;

bool L1ALIGNMENT = true;

bool FIX_T = !L1ALIGNMENT;
bool FIX_ROTZ = !L1ALIGNMENT;
bool FIX_ROTT = !L1ALIGNMENT;

bool DO_EC_ONLY = false;
bool DO_ASIDE_ONLY = false;
bool CONSTRAIN_EM = false;
bool CONSTRAIN_EI = false;
bool EIGENVALUE_CUT = false;
bool DO_SOFT_CONSTRAINT = false;
bool CONSTRAIN_ROTZ = false;
bool CONSTRAIN_LOWESTEV = false;
bool DO_EE_ONLY = false;
bool SET_RANGE = true;
const double RANGE_TRA = .5;
const double RANGE_ROTS = 1.5;
const double RANGE_ROTZT = .5;
// const double RANGE_TRA  =    3.;
// const double RANGE_ROTS =    4.;
// const double RANGE_ROTZT=    1.;
// const double RANGE_TRA  =    1.;
// const double RANGE_ROTS =    2.;
// const double RANGE_ROTZT=    5.;
// const double RANGE_TRA  =    6.;
// const double RANGE_ROTS =    4.;
// const double RANGE_ROTZT=    3.;

string parstr[6] = {"align_tras", "align_traz", "align_trat", "align_rots", "align_rotz", "align_rott"};
string unitstr[6] = {"mm", "mm", "mm", "mrad", "mrad", "mrad"};
// double range_tra=(CONSTRAIN_EM||EIGENVALUE_CUT)?2.:20.;
// double range_rot=(CONSTRAIN_EM||EIGENVALUE_CUT)?2.:20.;
double range_tra = 2.;
double range_rot = 2.;

const int NPAR = 6;
std::string m_chname = "";

const int NBARREL = 336;
const int NENDCAP = 256;

const int CUTNCHAMBERS = 0;

const int NMAXCHAMBERS = 50;
const int NMAXINDICES = NMAXCHAMBERS * 6;

ChamberDerivs* m_allChDerivs;
ChamberNames* m_chNames;

TChain* m_sdTree;
// TChain* m_sdFriendTree;
TChain* m_mtppTree;

// SD tree variables
int m_sd_run, m_sd_evt, m_sd_nChambers, m_sd_nMatrixIndices;
int *m_sd_chamberIds(new int[NMAXCHAMBERS]), *m_sd_matrixIndices(new int[NMAXINDICES]);
double* m_sd_firstDeriv(new double[NMAXINDICES]);
double* m_sd_secndDeriv(new double[NMAXINDICES * NMAXINDICES]);
double* m_adjSecndDeriv(new double[NMAXINDICES * NMAXINDICES]);

double m_sd_momentum;

TBranch* m_sd_brNChambers;
TBranch* m_sd_brChambers;
TBranch* m_sd_brRun;
TBranch* m_sd_brEvt;

// MuonTrackPreProcessor tree variables
int m_mtpp_run;
int m_mtpp_evt;
double trackChi2;
double trackPt;
int nresiduals;
double* residuals;
int nInnerLayersHit;
int nMiddleLayersHit;
int nOuterLayersHit;

/*
int     nresiduals;
double  residuals[50];
int     nInnerLayersHit;
int     nMiddleLayersHit;
int     nOuterLayersHit;
//int     nOtherLayersHit;
int     nTubeLayersHitInner;
int     nTubeLayersHitMiddle;
int     nTubeLayersHitOuter;
//int     nTubeLayersHitOther;
int     nChambersHit;
int     nLargeChambersHit;
int     nSmallChambersHit;
int     nInnerLayerDriftSigns;
int     nMiddleLayerDriftSigns;
int     nOuterLayerDriftSigns;
//int     nOtherLayerDriftSigns;
int     nInnerLayerOutliers;
int     nMiddleLayerOutliers;
int     nOuterLayerOutliers;
//int     nOtherLayerOutliers;
int     nInnerLayerHoles;
int     nMiddleLayerHoles;
int     nOuterLayerHoles;
//int     nOtherLayerHoles;
*/

TBranch* m_mtpp_brRun;
TBranch* m_mtpp_brEvt;

// output tree
TFile* m_outputFile;
TTree* m_outputTree;

int m_nAlignChambers;
double** m_alignPar = new double*[NMAXCHAMBERS];
double** m_alignErr = new double*[NMAXCHAMBERS];
double** m_firstderiv = new double*[NMAXCHAMBERS];
double** m_secndderiv = new double*[NMAXCHAMBERS];
int* m_chamberIds = new int[NMAXCHAMBERS];

int m_nInnerChambers, m_nMiddleChambers, m_nOuterChambers, m_nOtherChambers;

void help(const char* argv0) {
    cerr << "Usage: " << argv0 << " [options] " << endl
         << "Options:" << endl
         << " -o :\tOutput path" << endl
         << " -i :\tInput files" << endl
         << " -r :\tRootname" << endl
         << " -j :\tIteration" << endl
         << " -a :\tDraw absolute lines" << endl
         << " -d :\tDebug" << endl
         << " -c :\tChamber name" << endl
         << " -t :\tMinimum number of tracks" << endl
         << " -f :\tMinimum number of tracks in off-diagonal" << endl
         << " -h :\tShow this message and quit" << endl
         << endl;
    exit(1);
}

//_____________________________________________________________________________
int main(int argc, char** argv) {
    int nTracksProcessed = 0;
    int nTracksBeforeCut = 0;
    int nTracks = 0;
    int nTracksBEEA = 0;
    int nTracksBEEC = 0;
    int nTracksEELA = 0;
    int nTracksEELC = 0;
    int nTracksBIS8A = 0;
    int nTracksBIS8C = 0;
    double totalTrackChi2 = 0.;
    double totalTrackChi2BEEA = 0.;
    double totalTrackChi2BEEC = 0.;
    double totalTrackChi2EELA = 0.;
    double totalTrackChi2EELC = 0.;
    double totalTrackChi2BIS8A = 0.;
    double totalTrackChi2BIS8C = 0.;
    int nTracksECA = 0;
    int nTracksECC = 0;
    int nTracksBAR = 0;
    double totalTrackChi2ECA = 0.;
    double totalTrackChi2ECC = 0.;
    double totalTrackChi2BAR = 0.;

    for (int i = 0; i < NMAXCHAMBERS; i++) {
        m_alignPar[i] = new double[NPAR];
        m_alignErr[i] = new double[NPAR];
        m_firstderiv[i] = new double[NPAR];
        m_secndderiv[i] = new double[NPAR];
    }

    //----------------------------
    //-- Parse the command line --
    //----------------------------

    extern char* optarg;
    char c;
    TString inputdir("/afs/cern.ch/user/r/roberth/scratch0/L1_1file_noscat_results/");

    TString outputdir("");
    // TString rootname("group.det-muon");
    TString rootname("");

    int iteration = 0;
    bool ABSLINES = false;
    int diagcut(1), offdiagcut(0), maxnevents(-1);

    while ((c = getopt(argc, argv, "-hdaloc:i:r:j:t:f:n:")) != -1) {
        switch (c) {
            case 'o': outputdir = optarg; break;
            case 'i': inputdir = optarg; break;
            case 'r': rootname = optarg; break;
            case 'j': iteration = atoi(optarg); break;
            case 'd': DEBUG = true; break;
            case 'a': ABSLINES = true; break;
            case 't': diagcut = atoi(optarg); break;
            case 'f': offdiagcut = atoi(optarg); break;
            case 'n': maxnevents = atoi(optarg); break;
            case 'c': m_chname = optarg; break;
            default:
                cerr << "Help or unknown argument!" << endl;
                help(argv[0]);
                break;
        }
    }

    stringstream namestr;
    namestr << "MuonAlign_L1_1file_noscat_iter" << iteration;
    rootname = namestr.str();

    // getChamberIdMap("chambers.txt");
    m_chNames = (L1ALIGNMENT) ? new ChamberNames("l1chambers.txt") : new ChamberNames("chambers.txt");

    cout << "diagcut: " << diagcut << ", offdiagcut: " << offdiagcut << endl;

    if (rootname == "") {
        cerr << "no rootname passed, exiting..." << endl;
        exit(1);
    }
    if (inputdir == "") { inputdir = rootname; }
    if (outputdir == "") { outputdir = inputdir; }

    int chid = m_chNames->chamberIdFromString(m_chname);
    cout << "chamber ID: " << chid << endl;

    gStyle->SetOptStat(1111111);
    gStyle->SetOptFit(11111);
    gStyle->SetFrameFillStyle(0);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(1);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadRightMargin(0.15);
    gStyle->SetPalette(1);

    void* directory = gSystem->OpenDirectory(inputdir);
    if (!directory) {
        cerr << "input directory " << inputdir << " does not exist, exiting..." << endl;
        exit(1);
    }
    directory = gSystem->OpenDirectory(outputdir);
    if (!directory) {
        cerr << "create output directory: " << outputdir << endl;
        int status = gSystem->mkdir(outputdir, false);
        if (status != 0) {
            cerr << "could not create output directory, exiting..." << endl;
            exit(1);
        }
    }

    TString iterdir = inputdir + "/iter0";
    directory = gSystem->OpenDirectory(inputdir + "/iter0");
    bool multiplejobs = directory ? true : false;

    TString basefilename;
    if (multiplejobs) {
        TString iterdir = inputdir + Form("/iter%i/", iteration);
        directory = gSystem->OpenDirectory(iterdir);
        basefilename = iterdir;
        if (!directory) {
            cerr << "directory: " << iterdir << " does not exist" << endl;
            cerr << "iteration: " << iteration << " exiting..." << endl;
            exit(1);
        }
    } else {
        basefilename = inputdir + "/";
    }

    // basefilename += "MuonAlign_"+rootname+Form("_iter%i",iteration);
    basefilename += rootname;

    TString muonalign_filename = basefilename;
    TString alignpar_filename = basefilename;
    if (multiplejobs) {
        // muonalign_filename += "_index*";
        muonalign_filename += "*";
        alignpar_filename += "_solve";
    }
    muonalign_filename += ".root";
    alignpar_filename += ".root";
    if (multiplejobs) { muonalign_filename += "*"; }

    cout << "muonalign_filename: " << muonalign_filename << endl;

    // data from globalChi2Deriv tree

    m_sdTree = new TChain("globalChi2Deriv");
    m_sdTree->Add(muonalign_filename);
    cout << "have " << m_sdTree->GetNtrees() << " globalChi2Deriv trees" << endl;

    // m_sdFriendTree=new TChain("globalChi2DerivFriend");
    // m_sdFriendTree->Add("/tmp/roberth/newGlobalChi2Deriv.root");
    // m_sdTree->AddFriend(m_sdFriendTree);

    // get number of matrix indices and create matrix/vector
    const int NMATINDICES = getNumberMatrixIndices();
    const int NCHAMBERS = NMATINDICES / NPAR;
    cout << "nchambers: " << NCHAMBERS << endl;
    m_allChDerivs = new ChamberDerivs(NPAR, NCHAMBERS);

    // set branch addresses for SDTree
    initSDTree();

    // get MuonTrackPreProcessor tree
    m_mtppTree = new TChain("MuonTrackPreProcessor");
    m_mtppTree->Add(muonalign_filename);
    cout << "have " << m_mtppTree->GetNtrees() << " MuonTrackPreProcessor trees" << endl;
    initMTPPTree();

    // create new tree with combined results
    makeNewTree();

    int ientryMTPP(0);
    int ntrks(0), lastrun(0), lastevt(0);
    int nevts(0);
    for (int ientrySD = 0; ientrySD < (int)m_sdTree->GetEntries(); ientrySD++) {
        // cout<<"ientrySD: "<<ientrySD<<endl;

        if (nevts % 100000 == 0 && nevts > 0) cout << "nevts: " << nevts << endl;
        nevts++;

        if (maxnevents > -1 && nevts > maxnevents) {
            // cout<<"maxnevents: "<<maxnevents<<", nevts: "<<nevts<<endl;
            break;
        }

        // get next entry in derivatives tree
        bool goodSDentry = getSDTreeEntry(ientrySD, chid);

        // increment or reset ntrks counter
        if (m_sd_run != lastrun || m_sd_evt != lastevt) {
            lastrun = m_sd_run;
            lastevt = m_sd_evt;
            ntrks = -1;
        }
        ntrks++;

        nTracksProcessed++;

        if (!goodSDentry) {
            cout << "not goodSDentry" << endl;
            continue;
        }
        // cout<<"m_sd_nMatrixIndices="<<m_sd_nMatrixIndices<<endl;

        // cut on negative second derivative
        int index0 = -1;
        for (int imat = 0; imat < m_sd_nMatrixIndices; imat++) {
            for (int jmat = imat; jmat < m_sd_nMatrixIndices; jmat++) {
                index0++;
                if (imat != jmat) continue;
                double sd = m_sd_secndDeriv[index0];
                // cout<<"sd["<<imat<<"]["<<imat<<"]="<<sd<<endl;
                if (sd < 0.) goodSDentry = false;
            }
        }
        if (!goodSDentry) {
            // cout<<"bad sd entry"<<endl;
            continue;
        }
        // else
        // cout<<"good sd entry"<<endl;

        // get corresponding entry in MuonTrackPreProcessor tree
        bool goodMTPPEntry = getMTPPTreeEntry(ientryMTPP, m_sd_run, m_sd_evt, ntrks, chid);

        nTracksBeforeCut++;
        if (!goodMTPPEntry) continue;
        // cout<<"goodMTPPEntry"<<endl;

        if (!trackSelection()) continue;

        totalTrackChi2 += trackChi2;

        for (int ich = 0; ich < m_sd_nChambers; ich++) {
            int chid = m_sd_chamberIds[ich];
            string chname = m_chNames->chamberName(chid);
            if (chname == "") continue;
            if (chname.substr(0, 3) == "BEE") {
                if (chname.substr(4, 1) == "A") {
                    totalTrackChi2BEEA += trackChi2;
                    nTracksBEEA++;
                } else {
                    totalTrackChi2BEEC += trackChi2;
                    nTracksBEEC++;
                }
                break;
            } else if (chname.substr(0, 3) == "EEL") {
                if (chname.substr(4, 1) == "A") {
                    totalTrackChi2EELA += trackChi2;
                    nTracksEELA++;
                } else {
                    totalTrackChi2EELC += trackChi2;
                    nTracksEELC++;
                }
                break;
            } else if (chname.substr(0, 4) == "BIS8") {
                if (chname.substr(4, 1) == "A") {
                    totalTrackChi2BIS8A += trackChi2;
                    nTracksBIS8A++;
                } else {
                    totalTrackChi2BIS8C += trackChi2;
                    nTracksBIS8C++;
                }
                break;
            } else if (chname.substr(0, 3) == "ECA") {
                totalTrackChi2ECA += trackChi2;
                nTracksECA++;
                break;
            } else if (chname.substr(0, 3) == "ECC") {
                totalTrackChi2ECC += trackChi2;
                nTracksECC++;
                break;
            } else if (chname.substr(0, 3) == "BAR") {
                totalTrackChi2BAR += trackChi2;
                nTracksBAR++;
                break;
            }
        }

        nTracks++;

        // add derivatives
        addDerivatives();

        // fill tree
        fillTree();
    }

    cout << "---------------------------------------------" << endl;
    cout << "-    number tracks            : " << nTracks << endl;
    cout << "-    track chi2/NDoF          : " << totalTrackChi2 / (double)nTracks << endl;

    if (!L1ALIGNMENT) {
        cout << "-    BEEA  # tracks / track chi2/NDoF         : " << nTracksBEEA << "/" << totalTrackChi2BEEA / (double)nTracksBEEA
             << endl;
        cout << "-    BEEC  # tracks / track chi2/NDoF         : " << nTracksBEEC << "/" << totalTrackChi2BEEC / (double)nTracksBEEC
             << endl;
        cout << "-    EELA  # tracks / track chi2/NDoF         : " << nTracksEELA << "/" << totalTrackChi2EELA / (double)nTracksEELA
             << endl;
        cout << "-    EELC  # tracks / track chi2/NDoF         : " << nTracksEELC << "/" << totalTrackChi2EELC / (double)nTracksEELC
             << endl;
        cout << "-    BIS8A # tracks / track chi2/NDoF         : " << nTracksBIS8A << "/" << totalTrackChi2BIS8A / (double)nTracksBIS8A
             << endl;
        cout << "-    BIS8C # tracks / track chi2/NDoF         : " << nTracksBIS8C << "/" << totalTrackChi2BIS8C / (double)nTracksBIS8C
             << endl;
    } else {
        cout << "-    ECA  # tracks / track chi2/NDoF         : " << nTracksECA << "/" << totalTrackChi2ECA / (double)nTracksECA << endl;
        cout << "-    ECC  # tracks / track chi2/NDoF         : " << nTracksECC << "/" << totalTrackChi2ECC / (double)nTracksECC << endl;
        cout << "-    BAR  # tracks / track chi2/NDoF         : " << nTracksBAR << "/" << totalTrackChi2BAR / (double)nTracksBAR << endl;
    }
    cout << "---------------------------------------------" << endl;

    // cout<<"printing"<<endl;
    // m_allChDerivs->PrintChambers();
    // m_allChDerivs->PrintFirstDerivs();
    // m_allChDerivs->PrintSecondDerivs();

    cout << "printing info for all chambers" << endl;
    for (int ich = 0; ich < m_allChDerivs->nchambers(); ich++) {
        if (m_allChDerivs->chamberId(ich) < 0) continue;

        string chname = m_chNames->chamberName(m_allChDerivs->chamberId(ich));
        cout << "ich: " << ich << ", (CLHEP) ch: " << m_allChDerivs->chamberId(ich) << ", ntracks: " << m_allChDerivs->nhits(ich, ich)
             << " (" << chname << ")" << endl;
    }

    // if (CONSTRAIN_LOWESTEV) m_allChDerivs->constrainLowestEigenvector(m_chamberIdMap);

    // add 300 to all diagonal terms
    if (DO_SOFT_CONSTRAINT) addSoftConstraints(m_allChDerivs);

    // add constraints
    addConstraints(m_allChDerivs);

    /*
    // get condition numbers for various cuts
    for (int i=0;i<5;i++) {
      double idiagcut=2000.+2000.*(double)i;
      //for (int j=0;j<5;j++) {
      //double joffdiagcut=1000.*(double)j;
      ChamberDerivs tmpChDerivs=
        m_allChDerivs->trimmedChamberDerivs(idiagcut,400);
      double tmpcond=tmpChDerivs.condition(-1);
      cout<<"diag/offdiag/cond: "<<idiagcut<<"/4000/"<<tmpcond<<endl;
      //}
    }
    */

    // get plots for different diagonal cuts
    /*
    plotDiagonalCutHists();
    plotDiagonalCutHists(true);
    */

    // trim ChamberDerivs
    // cout<<"getting trimmed ChamberDerivs"<<endl;
    ChamberDerivs trimmedChDerivs = m_allChDerivs->trimmedChamberDerivs(diagcut, offdiagcut);
    cout << "printing trimmed chamber" << endl;
    trimmedChDerivs.PrintChambers();
    trimmedChDerivs.PrintFirstDerivs();
    trimmedChDerivs.PrintSecondDerivs();

    cout << "printing info for trimmed chambers" << endl;
    for (int ich = 0; ich < trimmedChDerivs.nchambers(); ich++) {
        if (trimmedChDerivs.chamberId(ich) < 0) continue;
        string chname = m_chNames->chamberName(trimmedChDerivs.chamberId(ich));
        cout << "(CLHEP) ch: " << trimmedChDerivs.chamberId(ich) << ", ntracks: " << trimmedChDerivs.nhits(ich, ich) << " (" << chname
             << ")" << endl;
    }

    cout << "=======================================================" << endl;
    cout << "             external constraints" << endl;
    cout << "=======================================================" << endl;

    // external constraints
    string asztFileNameLastIter = "";
    if (iteration > 0) {
        stringstream ss;
        ss << "outputAszt_iter" << iteration - 1 << ".txt";
        asztFileNameLastIter = ss.str();
    }
    cout << "asztFileNameLastIter: " << asztFileNameLastIter << endl;

    addExternalConstraints(&trimmedChDerivs, asztFileNameLastIter);

    TH2F* h_tVSV0 = new TH2F("h_tVSV0", "align_trat vs. log10(min singular value)", 100, 1., 2.5, 50, -20., 20.);
    vector<double> svArray;
    for (int ich = 0; ich < trimmedChDerivs.nchambers(); ich++) {
        cout << "ch: " << m_chNames->chamberName(trimmedChDerivs.chamberId(ich)) << endl;
        vector<double> sv;
        const TMatrixD *u, *vinv;
        trimmedChDerivs.singularValues(sv, u, vinv, ich);
        delete u;
        delete vinv;
        double minval(1.e10);
        for (int i = 0; i < (int)sv.size(); i++) {
            if (sv[i] < minval) minval = sv[i];
            cout << "sv[" << i << "]=" << sv[i] << endl;
        }
        svArray.push_back(minval);
    }

    // solve for global parameters/write to file
    string dummy;
    std::ofstream asztFile;
    stringstream asztFileName;
    asztFileName << "outputAszt_iter" << iteration;
    asztFile.open((asztFileName.str() + ".txt").c_str());
    if (L1ALIGNMENT)
        asztFile << "/Muon/Align/L1\n";
    else
        asztFile << "/Muon/Align/L3\n";

    std::ofstream asztFileDelta;
    asztFileDelta.open((asztFileName.str() + "_delta.txt").c_str());
    asztFileDelta << "/Muon/Align/L3\n";

    string origType;
    int origJff, origJzz, origJob;

    std::ifstream asztFileLastIter;
    if (iteration > 0) {
        cout << "opening " << asztFileNameLastIter << endl;
        asztFileLastIter.open(asztFileNameLastIter.c_str());
        asztFileLastIter >> dummy;
    }

    cout << "-----------------------------------------------" << endl;
    cout << "Solving for global parameters" << endl;
    double* err(0);
    double* initPars = new double[6];
    double* finalPars = new double[6];
    double* deltas = trimmedChDerivs.solve(err, -1, ChamberDerivs::ROOT);
    for (int i = 0; i < trimmedChDerivs.nindices(); i++) {
        int ipar = i % NPAR;
        if (!L1ALIGNMENT) {
            if (ipar >= NPAR - 6 + 3) {}
        } else {
            cout << "deltas[" << i << "]=" << deltas[i] << "(before)";
            deltas[i] *= m_rotscale;
            cout << ", " << deltas[i] << "(after)" << endl;
        }
    }
    for (int ich = 0; ich < trimmedChDerivs.nchambers(); ich++) {
        string chname = m_chNames->chamberName(trimmedChDerivs.chamberId(ich));
        cout << "(CLHEP) ch: " << trimmedChDerivs.chamberId(ich) << ", ntracks: " << trimmedChDerivs.nhits(ich, ich) << " (" << chname
             << ")" << endl;

        int jzz = atoi(chname.substr(3, 1).c_str());
        if (chname.substr(4, 1) == "C") jzz *= -1;
        int detphi = atoi(chname.substr(5, 2).c_str());
        if (chname.substr(2, 1) == "L") detphi++;
        int jff = detphi / 2;

        if (L1ALIGNMENT) {
            if (chname.substr(0, 3) == "ECA") {
                jff = jzz = 0;
            } else if (chname.substr(0, 3) == "ECC") {
                jff = jzz = 1;
            } else if (chname.substr(0, 3) == "Bar") {
                jff = jzz = 2;
            }
        }

        // get original values
        if (iteration > 0) {
            asztFileLastIter >> dummy >> origType >> origJff >> origJzz >> origJob;
            for (int i = 0; i < 6; i++) {
                double val;
                asztFileLastIter >> val;
                if (NPAR < 6 && i == 0) continue;
                int ipar = (NPAR == 6) ? i : i - 1;
                initPars[ipar] = val;
            }

            if (!L1ALIGNMENT && (origType != chname.substr(0, 3) || jff != origJff || jzz != origJzz)) {
                cout << "origType: " << origType << ", chname: " << chname << endl;
                cout << "origJff: " << origJff << ", jff: " << jff << endl;
                cout << "origJzz: " << origJzz << ", jzz: " << jzz << endl;
                cout << "bad entry" << endl;
                continue;
            }
        } else
            for (int i = 0; i < 6; i++) initPars[i] = 0.;

        // write deltas to file
        asztFileDelta << "A " << chname.substr(0, 3) << " " << jff << " " << jzz << " 0 ";
        if (!L1ALIGNMENT) asztFileDelta << "0.0 ";
        for (int ipar = 0; ipar < NPAR; ipar++) {
            int i = ich * NPAR + ipar;
            // cout<<"   "<<i<<": "<<deltas[i]<<" +/- "<<err[i]<<endl;
            asztFileDelta << deltas[i] << " ";
        }
        cout << endl;
        asztFileDelta << "\n";

        // get final values
        getFinalPars(initPars, &deltas[ich * NPAR], finalPars);
        asztFile << "A " << chname.substr(0, 3) << " " << jff << " " << jzz << " 0 ";
        if (!L1ALIGNMENT) asztFile << "0.0 ";
        for (int ipar = 0; ipar < NPAR; ipar++) {
            int i = ich * NPAR + ipar;
            int thisipar = (L1ALIGNMENT) ? ipar : ipar + 1;
            cout << "   " << i << ": " << finalPars[thisipar] << " +/- " << err[i] << endl;
            asztFile << finalPars[thisipar] << " ";
        }
        cout << endl;
        asztFile << "\n";

        // fill histogram with determinant for local 2nd deriv matrix
        // if (chname[4]=='A' || !DO_EC_ONLY) {
        int ipar = ich * NPAR + NPAR - 6 + 2;  // z-parameter
        h_tVSV0->Fill(log10(svArray[ich]), deltas[ipar]);
        //}
    }
    asztFile.close();
    asztFileDelta.close();
    asztFileLastIter.close();

    delete[] deltas;
    delete[] initPars;
    delete[] finalPars;

    TCanvas* c2 = new TCanvas("c2", "c2");
    h_tVSV0->Draw("colz");
    c2->Print("c2.eps");

    if (EIGENVALUE_CUT) {
        ChamberDerivs newTrimmedChDerivs = trimmedChDerivs.trimmedChamberDerivsBySV(80.);
        cout << "printing newly trimmed chamber" << endl;
        newTrimmedChDerivs.PrintChambers();
        makeSomePlots(newTrimmedChDerivs);
    } else
        makeSomePlots(trimmedChDerivs);

    // solve for local parameters
    cout << "-----------------------------------------------" << endl;
    cout << "Solving for local parameters" << endl;
    for (int ich = 0; ich < trimmedChDerivs.nchambers(); ich++) {
        trimmedChDerivs.PrintSecondDerivs(ich);

        double* locerr(0);
        double* locres = trimmedChDerivs.solve(locerr, ich, ChamberDerivs::CLHEP);
        cout << "(CLHEP) ch: " << trimmedChDerivs.chamberId(ich) << ", ntracks: " << trimmedChDerivs.nhits(ich, ich) << " ("
             << m_chNames->chamberName(trimmedChDerivs.chamberId(ich)) << ")" << endl;
        for (int ipar = 0; ipar < NPAR; ipar++) { cout << "   (loc) " << ipar << ": " << locres[ipar] << " +/- " << locerr[ipar] << endl; }
        delete[] locerr;
        delete[] locres;
    }

    m_outputFile->cd();
    m_outputTree->Write();
}

//________________________________________________________________________
void initSDTree() {
    m_sdTree->ResetBranchAddresses();

    m_sdTree->SetBranchStatus("*", 0);

    m_sdTree->SetBranchStatus("run", 1);
    m_sdTree->SetBranchStatus("event", 1);
    m_sdTree->SetBranchStatus("momentum", 1);
    m_sdTree->SetBranchStatus("nChambers", 1);
    m_sdTree->SetBranchStatus("chamberIds", 1);
    m_sdTree->SetBranchStatus("nMatrixIndices", 1);
    m_sdTree->SetBranchStatus("matrixIndices", 1);
    // m_sdTree->SetBranchStatus("newSecndDeriv", 1);
    m_sdTree->SetBranchStatus("secndDeriv", 1);
    m_sdTree->SetBranchStatus("firstDeriv", 1);

    m_sdTree->SetBranchAddress("run", &m_sd_run, &m_sd_brRun);
    m_sdTree->SetBranchAddress("event", &m_sd_evt, &m_sd_brEvt);
    m_sdTree->SetBranchAddress("momentum", &m_sd_momentum);
    m_sdTree->SetBranchAddress("nChambers", &m_sd_nChambers, &m_sd_brNChambers);
    m_sdTree->SetBranchAddress("chamberIds", m_sd_chamberIds, &m_sd_brChambers);
    m_sdTree->SetBranchAddress("nMatrixIndices", &m_sd_nMatrixIndices);
    m_sdTree->SetBranchAddress("matrixIndices", m_sd_matrixIndices);
    // m_sdTree->SetBranchAddress("newSecndDeriv",   m_sd_secndDeriv);
    m_sdTree->SetBranchAddress("secndDeriv", m_sd_secndDeriv);
    m_sdTree->SetBranchAddress("firstDeriv", m_sd_firstDeriv);
}

//________________________________________________________________________
void initMTPPTree() {
    m_mtppTree->SetBranchStatus("*", 0);

    m_mtppTree->SetBranchStatus("run", 1);
    m_mtppTree->SetBranchStatus("evt", 1);
    m_mtppTree->SetBranchStatus("trackChi2", 1);
    m_mtppTree->SetBranchStatus("trackPt", 1);

    m_mtppTree->SetBranchStatus("nresiduals", 1);
    m_mtppTree->SetBranchStatus("residuals", 1);
    m_mtppTree->SetBranchStatus("nInnerLayersHit", 1);
    m_mtppTree->SetBranchStatus("nMiddleLayersHit", 1);
    m_mtppTree->SetBranchStatus("nOuterLayersHit", 1);
    /*
    m_mtppTree->SetBranchStatus("nChambersHit",           1);
    m_mtppTree->SetBranchStatus("nLargeChambersHit",      1);
    m_mtppTree->SetBranchStatus("nSmallChambersHit",      1);
    m_mtppTree->SetBranchStatus("nTubeLayersHitInner",    1);
    m_mtppTree->SetBranchStatus("nTubeLayersHitMiddle",   1);
    m_mtppTree->SetBranchStatus("nTubeLayersHitOuter",    1);
    m_mtppTree->SetBranchStatus("DriftSignsInnerLayer",   1);
    m_mtppTree->SetBranchStatus("DriftSignsMiddleLayer",  1);
    m_mtppTree->SetBranchStatus("DriftSignsOuterLayer",   1);
    //m_mtppTree->SetBranchStatus("trackEta",               1);
    m_mtppTree->SetBranchStatus("nInnerLayerOutliers",    1);
    m_mtppTree->SetBranchStatus("nMiddleLayerOutliers",   1);
    m_mtppTree->SetBranchStatus("nOuterLayerOutliers",    1);
    m_mtppTree->SetBranchStatus("nInnerLayerHoles",       1);
    m_mtppTree->SetBranchStatus("nMiddleLayerHoles",      1);
    m_mtppTree->SetBranchStatus("nOuterLayerHoles",       1);
    */

    m_mtppTree->SetBranchAddress("run", &m_mtpp_run, &m_mtpp_brRun);
    m_mtppTree->SetBranchAddress("evt", &m_mtpp_evt, &m_mtpp_brEvt);
    m_mtppTree->SetBranchAddress("trackChi2", &trackChi2);
    m_mtppTree->SetBranchAddress("trackPt", &trackPt);

    m_mtppTree->SetBranchAddress("nresiduals", &nresiduals);
    m_mtppTree->SetBranchAddress("residuals", residuals);
    m_mtppTree->SetBranchAddress("nInnerLayersHit", &nInnerLayersHit);
    m_mtppTree->SetBranchAddress("nMiddleLayersHit", &nMiddleLayersHit);
    m_mtppTree->SetBranchAddress("nOuterLayersHit", &nOuterLayersHit);

    /*
    m_mtppTree->SetBranchAddress("nChambersHit",           &nChambersHit);
    m_mtppTree->SetBranchAddress("nLargeChambersHit",      &nLargeChambersHit);
    m_mtppTree->SetBranchAddress("nSmallChambersHit",      &nSmallChambersHit);
    m_mtppTree->SetBranchAddress("nTubeLayersHitInner",    &nTubeLayersHitInner);
    m_mtppTree->SetBranchAddress("nTubeLayersHitMiddle",   &nTubeLayersHitMiddle);
    m_mtppTree->SetBranchAddress("nTubeLayersHitOuter",    &nTubeLayersHitOuter);
    m_mtppTree->SetBranchAddress("DriftSignsInnerLayer",   &nInnerLayerDriftSigns);
    m_mtppTree->SetBranchAddress("DriftSignsMiddleLayer",  &nMiddleLayerDriftSigns);
    m_mtppTree->SetBranchAddress("DriftSignsOuterLayer",   &nOuterLayerDriftSigns);
    */

    /*
    //m_mtppTree->SetBranchAddress("trackEta",               &trackEta);
    m_mtppTree->SetBranchAddress("nInnerLayerOutliers",    &nInnerLayerOutliers);
    m_mtppTree->SetBranchAddress("nMiddleLayerOutliers",   &nMiddleLayerOutliers);
    m_mtppTree->SetBranchAddress("nOuterLayerOutliers",    &nOuterLayerOutliers);
    m_mtppTree->SetBranchAddress("nInnerLayerHoles",       &nInnerLayerHoles);
    m_mtppTree->SetBranchAddress("nMiddleLayerHoles",      &nMiddleLayerHoles);
    m_mtppTree->SetBranchAddress("nOuterLayerHoles",       &nOuterLayerHoles);
    */
    return;
}

//________________________________________________________________________
bool getSDTreeEntry(int ientry, int chid) {
    // cout<<"loading ientry "<<ientry<<endl;

    m_sdTree->LoadTree(ientry);

    int treenum = m_sdTree->GetTreeNumber();
    int treeentry = ientry - m_sdTree->GetTreeOffset()[treenum];

    m_sd_brRun->GetEntry(treeentry);
    m_sd_brEvt->GetEntry(treeentry);
    m_sd_brNChambers->GetEntry(treeentry);
    m_sd_brChambers->GetEntry(treeentry);

    // cout<<"chid: "<<chid<<endl;
    bool goodchamber = (chid == -1);
    if (!goodchamber) {
        for (int ich = 0; ich < m_sd_nChambers; ich++) {
            // cout<<"m_sd_chamberIds["<<ich<<"]="<<m_sd_chamberIds[ich]<<endl;
            if (m_sd_chamberIds[ich] == chid) {
                goodchamber = true;
                break;
            }
        }
    }
    if (!goodchamber) {
        cout << "returning false" << endl;
        return false;
    }

    m_sdTree->GetEntry(ientry);
    // cout<<"get entry "<<ientry<<endl;

    return true;
}

//________________________________________________________________________
bool getMTPPTreeEntry(int& ientryMTPP, int run, int evt, int ntrkSD, int chid) {
    int ntrksMTPP = ntrkSD - 1;

    while (m_mtpp_run != run || m_mtpp_evt != evt || ntrksMTPP < ntrkSD) {
        // cout<<"getting entry "<<ientryMTPP<<endl;
        m_mtppTree->LoadTree(ientryMTPP);
        int treenum = m_mtppTree->GetTreeNumber();
        int treeentry = ientryMTPP - m_mtppTree->GetTreeOffset()[treenum];
        m_mtpp_brRun->GetEntry(treeentry);
        m_mtpp_brEvt->GetEntry(treeentry);

        if (m_mtpp_run == run && m_mtpp_evt == evt) ntrksMTPP++;

        ++ientryMTPP;
    }

    m_mtppTree->GetEntry(ientryMTPP);

    // get numbers of chambers
    m_nInnerChambers = m_nMiddleChambers = m_nOuterChambers = m_nOtherChambers = 0;
    if (chid != -1) {
        for (int ich = 0; ich < m_sd_nChambers; ich++) {
            int tchid = m_sd_chamberIds[ich];
            string nchname = m_chNames->chamberName(tchid);
            if (nchname.substr(0, 2) == "EI")
                m_nInnerChambers++;
            else if (nchname.substr(0, 2) == "EM")
                m_nMiddleChambers++;
            else if (nchname.substr(0, 2) == "EO")
                m_nOuterChambers++;
            else
                m_nOtherChambers++;
        }
    }

    int& nChambers = m_nInnerChambers;
    if (chid != -1) {
        if (m_chname.substr(1, 1) == "I")
            //do nothing, the assignment remains valid
            //the following is effectively a self-assignment
            //nChambers = m_nInnerChambers;
        else if (m_chname.substr(1, 1) == "M")
            nChambers = m_nMiddleChambers;
        else if (m_chname.substr(1, 1) == "O")
            nChambers = m_nOuterChambers;
        else
            nChambers = m_nOtherChambers;
    }

    bool goodentry = chid == -1;
    if (!goodentry) {
        goodentry = (nChambers > CUTNCHAMBERS);
        if (CUTNCHAMBERS == 1) goodentry = (nChambers == 1);
        if (CUTNCHAMBERS == 2) goodentry = (nChambers >= 2);
    }

    // cout<<"returning "<<goodentry<<endl;

    return goodentry;
}

//________________________________________________________________________
const int getNumberMatrixIndices() {
    TBranch* brNMatIndices;
    TBranch* brMatIndices;
    int nMatIndices;
    int* matIndices = new int[50];

    m_sdTree->SetBranchAddress("nMatrixIndices", &nMatIndices, &brNMatIndices);
    m_sdTree->SetBranchAddress("matrixIndices", matIndices, &brMatIndices);

    int maxIndex(0);
    int minIndex(-1);
    for (int ientry = 0; ientry < (int)m_sdTree->GetEntries(); ientry++) {
        m_sdTree->LoadTree(ientry);
        int treenum = m_sdTree->GetTreeNumber();
        int treeentry = ientry - m_sdTree->GetTreeOffset()[treenum];

        brNMatIndices->GetEntry(treeentry);
        brMatIndices->GetEntry(treeentry);

        for (int imat = 0; imat < nMatIndices; imat++) {
            if (minIndex < 0 || matIndices[imat] < minIndex) minIndex = matIndices[imat];
            if (matIndices[imat] > maxIndex) maxIndex = matIndices[imat];
        }
    }

    return maxIndex + 1;
}

//________________________________________________________________________
void addDerivatives() {
    // cout<<"addDerivatives"<<endl;

    // fix 2nd derivatives to constrain along track
    int sdIndex(0);
    TMatrixDSym secDerivMatrix(m_sd_nMatrixIndices);
    for (int imat = 0; imat < m_sd_nMatrixIndices; imat++) {
        for (int jmat = imat; jmat < m_sd_nMatrixIndices; jmat++) {
            double sd = m_sd_secndDeriv[sdIndex++];
            if (imat % NPAR >= NPAR - 6 + 3) sd *= m_rotscale;
            if (jmat % NPAR >= NPAR - 6 + 3) sd *= m_rotscale;
            secDerivMatrix[imat][jmat] = sd;
            if (jmat != imat) secDerivMatrix[jmat][imat] = sd;
        }
    }

    if (CONSTRAIN_LOWESTEV) {
        if (DEBUG) std::cout << "constraining lowest eigenvalues" << std::endl;

        for (int ich = 0; ich < m_sd_nChambers; ich++) {
            TMatrixDSym secDerivCh(NPAR);
            for (int ipar = 0; ipar < NPAR; ipar++) {
                int oldiindex = ich * NPAR + ipar;
                for (int jpar = ipar; jpar < NPAR; jpar++) {
                    int oldjindex = ich * NPAR + jpar;
                    secDerivCh[ipar][jpar] = secDerivMatrix[oldiindex][oldjindex];
                    if (jpar != ipar) secDerivCh[jpar][ipar] = secDerivMatrix[oldiindex][oldjindex];
                }
            }

            if (DEBUG) {
                int chid = m_sd_chamberIds[ich];
                cout << "secDeriv (ich=" << ich << ") " << m_chNames->chamberName(chid) << endl;
                secDerivCh.Print();
            }

            TDecompSVD decompMat(secDerivCh);
            decompMat.Decompose();
            const TVectorD& svV = decompMat.GetSig();

            vector<double> sv;
            for (int i = 0; i < svV.GetNoElements(); i++) {
                sv.push_back(svV[i]);
                if (DEBUG) cout << "sv[" << i << "]=" << sv[i] << endl;
            }

            TMatrixD u = decompMat.GetU();
            if (DEBUG) {
                cout << "U:" << endl;
                u.Print();
            }

            TMatrixD v = decompMat.GetV();
            TMatrixD vinv(v.Invert());
            // cout<<"Vinv:"<<endl;
            // vinv.Print();

            TMatrixDSym newSD(NPAR);
            double constraintval(0.);
            for (int ipar = 0; ipar < NPAR; ipar++) {
                if (ipar == 0) {
                    // double sd=secDerivCh[NPAR-1][NPAR-1];
                    // constraintval=.03;//2.*sd;
                    // constraintval=100.;
                    constraintval = 5.;
                    if (DEBUG) cout << "constraintval: " << constraintval << endl;
                }
                if (ipar < NPAR - 6 + 3)
                    newSD[ipar][ipar] = sv[ipar];
                else
                    newSD[ipar][ipar] = sv[ipar] + constraintval;
            }
            TMatrixD D = u * newSD * vinv;
            // D.Print();

            for (int ipar = 0; ipar < NPAR; ipar++) {
                int oldiindex = ich * NPAR + ipar;
                for (int jpar = ipar; jpar < NPAR; jpar++) {
                    int oldjindex = ich * NPAR + jpar;
                    secDerivMatrix[oldiindex][oldjindex] = D[ipar][jpar];
                    if (jpar != ipar) secDerivMatrix[oldjindex][oldiindex] = D[ipar][jpar];
                }
            }
        }
        if (DEBUG) {
            cout << "after" << endl;
            secDerivMatrix.Print();
        }
    }

    // add derivatives
    int nChambers = m_allChDerivs->nchambers();
    int ich = -1;

    int index(0);

    int lastChId = -1;
    // cout<<"m_sd_nMatrixIndices: "<<m_sd_nMatrixIndices<<endl;

    for (int imat = 0; imat < m_sd_nMatrixIndices; imat++) {
        // cout<<"imat: "<<imat<<endl;

        if (imat % NPAR == 0) {
            // cout<<"setting chamberId"<<endl;
            int ii_matIndex = m_sd_matrixIndices[imat];
            ich++;
            int allChId = ii_matIndex / NPAR;
            // cout<<"allChId: "<<allChId<<endl;
            if (m_allChDerivs->chamberId(allChId) < 0) { m_allChDerivs->setChamberId(allChId, m_sd_chamberIds[ich]); }
            // cout<<"chamberId: "<<m_allChDerivs->chamberId(allChId)<<endl;
            lastChId = m_sd_chamberIds[ich];
        }

        string chname = m_chNames->chamberName(lastChId);
        if (DO_EE_ONLY) {
            if (chname.substr(0, 3) != "BEE" && chname.substr(0, 3) != "EEL") continue;
            // std::cout<<"chname="<<chname<<std::endl;
        }
        // cout<<"chname: "<<chname<<endl;

        // if (m_sd_nMatrixIndices<15) cout<<"imat: "<<imat<<endl;
        int i_matIndex = m_sd_matrixIndices[imat];

        double fd = m_sd_firstDeriv[imat];
        if (imat % NPAR >= NPAR - 6 + 3) fd *= m_rotscale;
        // cout<<"adding "<<fd<<" to "<<i_matIndex<<endl;
        m_allChDerivs->addFirstDeriv(i_matIndex, fd);

        for (int jmat = imat; jmat < m_sd_nMatrixIndices; jmat++) {
            int j_matIndex = m_sd_matrixIndices[jmat];
            double sd = secDerivMatrix[imat][jmat];

            // cout<<"adding "<<sd<<" to "<<i_matIndex<<","<<j_matIndex<<endl;
            if (j_matIndex >= i_matIndex)
                m_allChDerivs->addSecondDeriv(i_matIndex, j_matIndex, sd);
            else
                m_allChDerivs->addSecondDeriv(j_matIndex, i_matIndex, sd);

            if (imat % NPAR == 0 && jmat % NPAR == 0) {
                int j_matIndex = m_sd_matrixIndices[jmat];
                int i_allChId = i_matIndex / NPAR;
                int j_allChId = j_matIndex / NPAR;
                // cout<<"increment hitmap"<<endl;
                if (j_matIndex >= i_matIndex)
                    m_allChDerivs->incrementHitmap(i_allChId * nChambers + j_allChId);
                else
                    m_allChDerivs->incrementHitmap(j_allChId * nChambers + i_allChId);
            }
            index++;
        }
    }

    return;
}

//________________________________________________________________________
void fillTree() {
    int m_nChambers = -1;
    int index = -1;
    for (int imat = 0; imat < m_sd_nMatrixIndices; imat++) {
        int ipar = imat % NPAR;
        if (ipar == 0) {
            m_nChambers++;
            m_chamberIds[m_nChambers] = m_sd_chamberIds[m_nChambers];
        }

        for (int jmat = imat; jmat < m_sd_nMatrixIndices; jmat++) {
            index++;
            if (jmat != imat) continue;
            double sd = m_sd_secndDeriv[index];
            double fd = m_sd_firstDeriv[imat];
            // cout<<"ft: sd["<<imat<<"]="<<sd<<", fd="<<fd<<endl;
            double alignPar = -fd / sd;
            double alignErr = std::sqrt(2. / fabs(sd));
            m_alignPar[m_nChambers][ipar] = alignPar;
            m_alignErr[m_nChambers][ipar] = alignErr;
            m_firstderiv[m_nChambers][ipar] = fd;
            m_secndderiv[m_nChambers][ipar] = sd;
        }
    }
    m_outputTree->Fill();
}

//________________________________________________________________________
void makeNewTree() {
    m_outputFile = new TFile("outputfile.root", "RECREATE");
    m_outputTree = m_mtppTree->CloneTree(0);

    m_outputTree->Branch("nAlignChambers", &m_nAlignChambers, "nAlignChambers");
    m_outputTree->Branch("chamberIds", m_chamberIds, "chamberIds[nAlignChambers]");

    stringstream alignParStr;
    alignParStr << "alignPar[nAlignChambers][" << NPAR << "]";
    stringstream alignErrStr;
    alignErrStr << "alignErr[nAlignChambers][" << NPAR << "]";
    stringstream fdStr;
    fdStr << "fd[nAlignChambers][" << NPAR << "]";
    stringstream sdStr;
    sdStr << "sd[nAlignChambers][" << NPAR << "]";

    m_outputTree->Branch("alignPar", m_alignPar, alignParStr.str().c_str());
    m_outputTree->Branch("alignErr", m_alignErr, alignErrStr.str().c_str());
    m_outputTree->Branch("firstderiv", m_firstderiv, fdStr.str().c_str());
    m_outputTree->Branch("secndderiv", m_secndderiv, sdStr.str().c_str());

    m_outputTree->Branch("nInnerChambers", &m_nInnerChambers, "nInnerChambers/I");
    m_outputTree->Branch("nMiddleChambers", &m_nMiddleChambers, "nMiddleChambers/I");
    m_outputTree->Branch("nOuterChambers", &m_nOuterChambers, "nOuterChambers/I");
    m_outputTree->Branch("nOtherChambers", &m_nOtherChambers, "nOtherChambers/I");

    return;
}

//________________________________________________________________________
void makeHitmap(const ChamberDerivs& chDerivs, TPad* pad) {
    int optfit = gStyle->GetOptFit();
    int optstat = gStyle->GetOptStat();
    gStyle->SetOptFit(000000);
    gStyle->SetOptStat(000000);

    if (DO_EC_ONLY && !DO_ASIDE_ONLY) pad->Divide(2, 1);

    // int nchambers=chDerivs.nchambers();
    int nchambers = (DO_EC_ONLY) ? NENDCAP : NENDCAP + NBARREL;
    if (!DO_ASIDE_ONLY && !DO_EC_ONLY) nchambers *= 2;

    int minbin = (DO_ASIDE_ONLY) ? 0 : -nchambers / 2;
    int maxbin = (DO_ASIDE_ONLY) ? nchambers : nchambers / 2;
    TH2F* hitmap = new TH2F("hitmap", "log_{10}(# tracks) for each pair of chambers", maxbin - minbin, minbin, maxbin, maxbin - minbin,
                            minbin, maxbin);

    for (int ich = 0; ich < chDerivs.nchambers(); ich++) {
        int ichid = chDerivs.chamberId(ich);
        string ichambername = m_chNames->chamberName(ichid);
        if (ichambername == "") continue;

        if (ichambername.substr(4, 1) == 'C' && (DO_ASIDE_ONLY)) continue;

        int icoord = hitMapCoord(get1Dcoordinate(ichambername), ichambername) - minbin + 1;

        for (int jch = ich; jch < chDerivs.nchambers(); jch++) {
            // if (jch!=ich) continue;

            int jchid = chDerivs.chamberId(jch);
            string jchambername = m_chNames->chamberName(jchid);
            if (jchambername == "") continue;
            if (jchambername.substr(4, 1) == 'C' && DO_ASIDE_ONLY) continue;

            int jcoord = hitMapCoord(get1Dcoordinate(jchambername), jchambername) - minbin + 1;

            double val = std::log10(chDerivs.nhits(ich, jch));
            // cout<<"setting bin "<<icoord<<endl;
            hitmap->SetBinContent(icoord, jcoord, val);
            // cout<<"setting bin "<<jcoord<<endl;
            hitmap->SetBinContent(jcoord, icoord, val);
        }
    }

    // set bin labels
    string chnames[16] = {"EIL1A1", "EIL2A1", "EIL3A1", "EIL4A1", "EML1A1", "EML2A1", "EML3A1", "EML4A1",
                          "EML5A1", "BEE1A1", "EOL1A1", "EOL2A1", "EOL3A1", "EOL4A1", "EOL5A1", "EOL6A1"};
    string labels[16] = {"EI_1A", "EI_2A", "EI_3A", "EI_4A", "EM_1A", "EM_2A", "EM_3A", "EM_4A",
                         "EM_5A", "EE",    "EO_1A", "EO_2A", "EO_3A", "EO_4A", "EO_5A", "EO_6A"};

    for (int i = 0; i < 16; i++) {
        int ibin = hitMapCoord(get1Dcoordinate(chnames[i]), chnames[i]) - minbin + 1;
        hitmap->GetXaxis()->SetBinLabel(ibin, labels[i].c_str());
        hitmap->GetYaxis()->SetBinLabel(ibin, labels[i].c_str());
    }

    pad->cd();
    hitmap->Draw("COLZ");
    // hitmap->Draw("LEGO2");
    gStyle->SetOptFit(optfit);
    gStyle->SetOptStat(optstat);

    return;
}

//________________________________________________________________________
void make2DAlignHists(const ChamberDerivs& chDerivs, TPad** pad1, TPad** pad2) {
    int optfit = gStyle->GetOptFit();
    int optstat = gStyle->GetOptStat();
    gStyle->SetOptFit(000000);
    gStyle->SetOptStat(000000);

    TH2D** h2_align = new TH2D*[NPAR];
    TLegend** h2_align_leg = new TLegend*[NPAR];
    TH2D** h2_alignVNtrks = new TH2D*[NPAR];

    int NXBINS = (DO_EC_ONLY && DO_ASIDE_ONLY) ? 8 : 35;
    double xmin = (DO_EC_ONLY && DO_ASIDE_ONLY) ? 9.5 : -17.5;
    double xmax = 17.5;
    for (int ipar = 0; ipar < NPAR; ipar++) {
        string thisparstr = parstr[6 - NPAR + ipar];

        h2_align[ipar] = new TH2D(thisparstr.c_str(), thisparstr.c_str(), NXBINS, xmin, xmax, 51, 0.5, 51.5);
        string histname = thisparstr + "VNtrks";
        double range = (ipar < NPAR - 3) ? range_tra : range_rot;
        range *= 10.;
        h2_alignVNtrks[ipar] = new TH2D(histname.c_str(), histname.c_str(), 100, 1000., 20000., 100, -range, range);
    }

    double histmin[NPAR];
    double histmax[NPAR];
    for (int i = 0; i < NPAR; i++) {
        histmin[i] = 1e8;
        histmax[i] = 0.;
    }

    double *err, *res;
    res = chDerivs.solve(err, -1, ChamberDerivs::ROOT);

    for (int ich = 0; ich < chDerivs.nchambers(); ich++) {
        int chid = chDerivs.chamberId(ich);
        string chambername = m_chNames->chamberName(chid);
        int ntrks = chDerivs.nhits(ich, ich);

        for (int ipar = 0; ipar < NPAR; ipar++) {
            double alignpar = res[ich * NPAR + ipar];

            if (chambername != "") {
                h2_align[ipar]->Fill(getXcoordinate(chambername), 51 - getYcoordinate(chambername), alignpar);
                h2_alignVNtrks[ipar]->Fill(ntrks, alignpar);

                if (alignpar > histmax[ipar]) histmax[ipar] = alignpar;
                if (alignpar < histmin[ipar]) histmin[ipar] = alignpar;

                string Ytitle, Xtitle;
                if (chambername[3] != '0') {
                    if (chambername[0] == 'E') Xtitle = "Endcap" + chambername.substr(3, 2);
                    if (chambername[0] == 'B') Xtitle = "Barrel" + chambername.substr(3, 2);
                    if (chambername[1] == 'E') Xtitle = "EE/BEE" + chambername.substr(3, 2);
                }

                if (chambername[1] == 'I' || chambername.substr(0, 3) == "BEE") Ytitle = "Inner" + chambername.substr(5, 2);
                if (chambername[1] == 'M' || chambername.substr(0, 3) == "EEL") Ytitle = "Middle" + chambername.substr(5, 2);
                if (chambername[1] == 'O') Ytitle = "Outer" + chambername.substr(5, 2);

                cout << "chambername: " << chambername << ", Ytitle: " << Ytitle << endl;
                h2_align[ipar]->GetYaxis()->SetBinLabel(51 - getYcoordinate(chambername), Ytitle.c_str());

                int firstbin = int(-xmin);
                if (firstbin > 0) firstbin++;
                if (chambername.substr(4, 1) == "C" && DO_ASIDE_ONLY) continue;

                // cout<<"setting bin label x"<<firstbin+getXcoordinate(chambername)<<endl;
                h2_align[ipar]->GetXaxis()->SetBinLabel(firstbin + getXcoordinate(chambername), Xtitle.c_str());
            }
            int thispar = 6 - NPAR + ipar;
            h2_align[ipar]->GetYaxis()->SetTitle((parstr[thispar] + " (" + unitstr[thispar] + ")").c_str());
        }
    }

    delete[] res;
    delete[] err;

    Double_t contours_tra[16] = {-2., 1.5, -1., -.5, -.4, -.3, -.2, -.1, .1, .2, .3, .4, .5, 1., 1.5, 2.};
    Double_t contours_rot[16] = {-2., 1.5, -1., -.5, -.4, -.3, -.2, -.1, .1, .2, .3, .4, .5, 1., 1.5, 2.};

    for (int i = 0; i < 16; i++) contours_rot[i] *= .01;

    for (int ipar = 0; ipar < NPAR; ipar++) {
        TH2D* h = h2_align[ipar];

        h2_align_leg[ipar] = new TLegend(.75, .8, .85, .9);
        TLegend* leg = h2_align_leg[ipar];

        if (SET_RANGE) {
            for (int ix = 1; ix <= NXBINS; ix++) {
                for (int iy = 1; iy <= 51; iy++) {
                    if (h->GetBinContent(ix, iy) == 0.)
                        // cout<<"setting bin "<<ix<<" "<<iy<<endl;
                        h->SetBinContent(ix, iy, -999.);
                }
            }
        }

        pad1[ipar]->cd();
        cout << "histmin: " << histmin[ipar] << ", histmax: " << histmax[ipar] << endl;

        if (SET_RANGE) {
            if (0) {
                if (ipar < NPAR - 3) {
                    h->SetAxisRange(-range_tra, range_tra, "Z");
                    if (CONSTRAIN_EM || EIGENVALUE_CUT) h->SetContour(16, contours_tra);
                } else {
                    h->SetAxisRange(-range_rot, range_rot, "Z");
                    if (CONSTRAIN_EM || EIGENVALUE_CUT) h->SetContour(16, contours_rot);
                }
            } else {
                double range(0.);
                if (ipar < NPAR - 3) {
                    cout << "setting axis range to -2,2" << endl;
                    range = RANGE_TRA;
                } else {
                    cout << "setting axis range to -10,10" << endl;
                    range = RANGE_ROTZT;
                }
                if (ipar == NPAR - 6 + 3) {
                    cout << "setting axis range to -2,2" << endl;
                    range = RANGE_ROTS;
                }
                h->SetAxisRange(-range, range, "Z");
            }
        }

        h->GetXaxis()->SetTickLength(0);
        h->GetYaxis()->SetTickLength(0);
        h->GetXaxis()->SetLabelSize(.02);
        h->GetYaxis()->SetLabelSize(.02);
        h->Draw("COLZ");

        leg->SetFillColor(10);
        int thispar = 6 - NPAR + ipar;
        leg->AddEntry(h, (parstr[thispar] + " (" + unitstr[thispar] + ")").c_str(), "");
        // leg->Draw();

        // pad2[ipar]->cd();
        pad2[ipar]->cd()->SetLogx(1);
        TH2D* h2 = h2_alignVNtrks[ipar];
        h2->GetXaxis()->SetTitle("# tracks");
        h2->GetYaxis()->SetTitle((parstr[thispar] + " (" + unitstr[thispar] + ")").c_str());
        h2->Draw("COLZ");
    }

    gStyle->SetOptFit(optfit);
    gStyle->SetOptStat(optstat);

    return;
}

//________________________________________________________________________
int hitMapCoord(int coord, const string& chambername) {
    int newcoord = coord;
    if (!DO_EC_ONLY && (chambername[0] == 'E' || chambername.substr(0, 2) == "BE")) newcoord += NBARREL;
    if (chambername.substr(4, 1) == 'C') newcoord *= -1;
    return newcoord;
}

//________________________________________________________________________
int get1Dcoordinate(const string& chambername) {
    if (chambername == "") return -999;

    int eta = atoi(chambername.substr(3, 1).c_str());
    int phi = atoi(chambername.substr(5, 2).c_str());

    if (chambername[1] == 'I' && phi > 10) {
        if (phi == 11 && chambername[2] == 'R') phi = 17;
        if (phi == 15 && chambername[2] == 'R') phi = 18;
    }

    eta--;
    phi--;

    // inner
    bool isBarrel = chambername[0] == 'B' && chambername[1] != 'E';

    int icoord = 0;
    int nphi = isBarrel ? 18 : 16;
    int neta = isBarrel ? 8 : 4;

    // inner layer
    if (chambername[1] == 'I') return icoord + eta * nphi + phi;

    if (chambername[0] == 'E' || chambername.substr(0, 2) == "BE") {
        icoord += nphi * neta;
        nphi = 8;
        neta = 2;
        if (chambername.substr(0, 2) == "BE") return icoord + eta * nphi + phi;
    }

    icoord += nphi * neta;
    nphi = 16;
    neta = (isBarrel) ? 6 : 5;

    // middle layer
    if (chambername[1] == 'M') {
        // cout<<"eta: "<<eta<<", phi: "<<phi<<", icoord: "<<icoord<<endl;
        return icoord + (eta * nphi) + phi;
    }

    icoord += nphi * neta;
    nphi = 16;
    neta = 6;

    if (chambername[1] == 'O') return icoord + eta * nphi + phi;

    return -1;
}

//________________________________________________________________________
int getXcoordinate(const string& chambername) {
    if (chambername == "") return -999;

    Int_t eta = atoi(chambername.substr(3, 1).c_str());
    Int_t Xcor = (chambername[4] == 'A') ? eta : (eta * (-1));
    // if(chambername[0]=='B') return Xcor;
    if ((chambername[0] == 'E') && (chambername[4] == 'A')) Xcor = Xcor + 10;             // case Endcap A
    if ((chambername[0] == 'E') && (chambername[4] == 'C')) Xcor = Xcor - 10;             // case Endcap C
    if ((chambername[1] == 'E') && (chambername[4] == 'A')) Xcor = Xcor - 2;              // case EELA and EESA
    if ((chambername[1] == 'E') && (chambername[4] == 'C')) Xcor = Xcor + 2;              // case EELC adn EESC
    if ((chambername.substr(0, 2) == "BE") && (chambername[4] == 'A')) Xcor = Xcor + 10;  // case BEEA
    if ((chambername.substr(0, 2) == "BE") && (chambername[4] == 'C')) Xcor = Xcor - 10;  // case BEEC

    return Xcor;
}

//________________________________________________________________________
int getYcoordinate(const string& chambername) {
    if (chambername == "") return -999;
    cout << "chname: " << chambername << endl;

    Int_t Ycor = atoi(chambername.substr(5, 2).c_str());
    cout << "Ycor: " << Ycor << endl;

    if ((chambername[1] == 'I') && (Ycor > 10)) {
        if (Ycor == 11 && chambername[2] == 'R') Ycor = 17;
        if (Ycor == 15 && chambername[2] == 'R') Ycor = 18;
    }
    if (chambername[1] == 'M') Ycor = Ycor + 18;
    if (chambername[1] == 'O') Ycor = Ycor + 34;
    if ((chambername[0] == 'E') && (chambername[1] == 'E')) Ycor = Ycor + 18;

    return Ycor;
}

//________________________________________________________________________
void addSoftConstraints(ChamberDerivs* chDerivs) {
    std::cout << "adding soft constraint" << std::endl;
    // add constraints
    for (int ich = 0; ich < chDerivs->nchambers(); ich++) {
        for (int ipar = 0; ipar < NPAR; ipar++) {
            // chDerivs->addConstraint(ich,ipar,1000.);
            // chDerivs->addConstraint(ich,ipar,100.);
            chDerivs->addConstraint(ich, ipar, 10.);
        }
    }

    return;
}

//________________________________________________________________________
void addExternalConstraints(ChamberDerivs* chDerivs, const std::string& filename) {
    std::ifstream asztFileLastIter;
    string dummy;
    if (filename != "") {
        asztFileLastIter.open(filename.c_str());
        asztFileLastIter >> dummy;
    }

    string origType;
    int origJff, origJzz, origJob;

    double* initPars = new double[NPAR];
    for (int ipar = 0; ipar < NPAR; ipar++) initPars[ipar] = 0.;

    double sigma = 1.;
    for (int ich = 0; ich < chDerivs->nchambers(); ich++) {
        if (chDerivs->chamberId(ich) < 0) continue;

        string chname = m_chNames->chamberName(chDerivs->chamberId(ich));
        cout << "in externalconstraints, ich " << ich << ", chname: " << chname << endl;

        int jzz = atoi(chname.substr(3, 1).c_str());
        if (chname.substr(4, 1) == "C") jzz *= -1;
        int detphi = atoi(chname.substr(5, 2).c_str());
        if (chname.substr(2, 1) == "L") detphi++;
        int jff = detphi / 2;

        if (filename != "") {
            asztFileLastIter >> dummy >> origType >> origJff >> origJzz >> origJob;
            for (int i = 0; i < 6; i++) {
                double val;
                asztFileLastIter >> val;
                if (NPAR < 6 && i == 0) continue;
                int ipar = (NPAR == 6) ? i : i - 1;
                initPars[ipar] = val;
                if (ipar > NPAR - 6 + 2) initPars[ipar] /= m_rotscale;
            }
            if (!L1ALIGNMENT && (origType != chname.substr(0, 3) || jff != origJff || jzz != origJzz)) {
                cout << "origType: " << origType << ", chname: " << chname << endl;
                cout << "origJff: " << origJff << ", jff: " << jff << endl;
                cout << "origJzz: " << origJzz << ", jzz: " << jzz << endl;
                cout << "bad entry" << endl;
                exit(2);
            }
        }

        /*
        for (int ipar=0;ipar<NPAR-3;ipar++) {
          chDerivs->addConstraint(ich,ipar,sigma);
          cout<<"ich: "<<ich<<", initPars["<<ipar<<"]="<<initPars[ipar]<<endl;
          chDerivs->addFirstDerivConstraint(ich,ipar,sigma*initPars[ipar]);
        }
        */

        std::cout << "adding constraints0" << std::endl;
        if (FIX_T) {
            double sigma_t = 1.e22;
            std::cout << "fixing t" << std::endl;
            chDerivs->addConstraint(ich, 1, sigma_t);
            chDerivs->addFirstDerivConstraint(ich, 1, sigma_t * initPars[1]);
        }
        if (FIX_ROTZ) {
            double sigma_rotz = 1.e30;
            std::cout << "fixing rotz" << std::endl;
            chDerivs->addConstraint(ich, 3, sigma_rotz);
            chDerivs->addFirstDerivConstraint(ich, 3, sigma_rotz * initPars[3]);
        }
        if (FIX_ROTT) {
            double sigma_rott = 1.e30;
            std::cout << "fixing rott" << std::endl;
            chDerivs->addConstraint(ich, 4, sigma_rott);
            chDerivs->addFirstDerivConstraint(ich, 4, sigma_rott * initPars[4]);
        }
    }
    delete[] initPars;
}

//________________________________________________________________________
void addConstraints(ChamberDerivs* chDerivs) {
    std::cout << "adding constraints1" << std::endl;

    // add constraints
    if (CONSTRAIN_ROTZ) {
        if (DEBUG) cout << "constrain ROTZ" << endl;
        for (int ich = 0; ich < chDerivs->nchambers(); ich++) chDerivs->addConstraint(ich, NPAR - 6 + 4, 50000.);
    }

    if (CONSTRAIN_EM) {
        if (DEBUG) cout << "constrain EM" << endl;

        // EML3A05
        int ich_EML3A05 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EML3A05"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EML3A05")) << endl;
        chDerivs->addConstraint(ich_EML3A05, 0, 14325. / 2.);  // z
        chDerivs->addConstraint(ich_EML3A05, 1, 12900. / 2.);  // t

        // EML3A09
        int ich_EML3A09 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EML3A09"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EML3A09")) << endl;
        chDerivs->addConstraint(ich_EML3A09, 0, 14325. / 2.);  // z
        chDerivs->addConstraint(ich_EML3A09, 1, 12900. / 2.);  // t

        // EML3A13
        int ich_EML3A13 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EML3A13"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EML3A13")) << endl;
        chDerivs->addConstraint(ich_EML3A13, 1, 12900. / 2.);  // t

        // EML3C05
        int ich_EML3C05 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EML3C05"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EML3C05")) << endl;
        chDerivs->addConstraint(ich_EML3C05, 0, 14325. / 2.);  // z
        chDerivs->addConstraint(ich_EML3C05, 1, 12900. / 2.);  // t

        // EML3C09
        int ich_EML3C09 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EML3C09"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EML3C09")) << endl;
        chDerivs->addConstraint(ich_EML3C09, 0, 14325. / 2.);  // z
        chDerivs->addConstraint(ich_EML3C09, 1, 12900. / 2.);  // t

        // EML3C13
        int ich_EML3C13 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EML3C13"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EML3C13")) << endl;
        chDerivs->addConstraint(ich_EML3C13, 1, 12900. / 2.);  // t
    }

    if (CONSTRAIN_EI) {
        if (DEBUG) cout << "constrain EI" << endl;

        // EIL1A05
        int ich_EIL1A05 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EIL1A05"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EIL1A05")) << endl;
        chDerivs->addConstraint(ich_EIL1A05, 0, 14325. / 2.);  // z
        chDerivs->addConstraint(ich_EIL1A05, 1, 12900. / 2.);  // t

        // EIL1A09
        int ich_EIL1A09 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EIL1A09"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EIL1A09")) << endl;
        chDerivs->addConstraint(ich_EIL1A09, 0, 14325. / 2.);  // z
        chDerivs->addConstraint(ich_EIL1A09, 1, 12900. / 2.);  // t

        // EIL1A13
        int ich_EIL1A13 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EIL1A13"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EIL1A13")) << endl;
        chDerivs->addConstraint(ich_EIL1A13, 1, 12900. / 2.);  // t

        // EIL1C05
        int ich_EIL1C05 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EIL1C05"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EIL1C05")) << endl;
        chDerivs->addConstraint(ich_EIL1C05, 0, 14325. / 2.);  // z
        chDerivs->addConstraint(ich_EIL1C05, 1, 12900. / 2.);  // t

        // EIL1C09
        int ich_EIL1C09 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EIL1C09"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EIL1C09")) << endl;
        chDerivs->addConstraint(ich_EIL1C09, 0, 14325. / 2.);  // z
        chDerivs->addConstraint(ich_EIL1C09, 1, 12900. / 2.);  // t

        // EIL1C13
        int ich_EIL1C13 = chDerivs->getChIndex(m_chNames->chamberIdFromString("EIL1C13"));
        cout << "chamber: " << m_chNames->chamberName(m_chNames->chamberIdFromString("EIL1C13")) << endl;
        chDerivs->addConstraint(ich_EIL1C13, 1, 12900. / 2.);  // t
    }

    return;
}

//________________________________________________________________________
void getAllSingularValues(const ChamberDerivs& chDerivs, vector<double>& svArray) {
    svArray.clear();

    for (int ich = 0; ich < chDerivs.nchambers(); ich++) {
        vector<double> sv;
        const TMatrixD *u, *vinv;
        chDerivs.singularValues(sv, u, vinv, ich);
        delete u;
        delete vinv;

        double minval = 1.e20;
        for (int ipar = 0; ipar < (int)sv.size(); ipar++) {
            if (sv[ipar] < minval) minval = sv[ipar];
        }
        svArray.push_back(minval);
    }
    return;
}

//________________________________________________________________________
void makeSomePlots(const ChamberDerivs& chDerivs) {
    vector<double> svArray;
    getAllSingularValues(chDerivs, svArray);

    // histogram to store t vs. determinant
    TH2F* h_tVCond = new TH2F("h_tVCond", "t vs. condition number", 100, 2.2, 4.2, 50, -20., 20.);
    TH2F* h_tVSV = new TH2F("h_tVSV", "log10(t) vs. log10(min singular value)", 100, 1., 9., 50, -6., 3.);

    // solve for global parameters
    cout << "-----------------------------------------------" << endl;
    cout << "Solving for global parameters" << endl;
    double* err(0);
    double* res = chDerivs.solve(err, -1, ChamberDerivs::ROOT);
    for (int ich = 0; ich < chDerivs.nchambers(); ich++) {
        string chname = m_chNames->chamberName(chDerivs.chamberId(ich));
        cout << "(ROOT) ch: " << chDerivs.chamberId(ich) << ", ntracks: " << chDerivs.nhits(ich, ich) << " (" << chname << ")" << endl;
        for (int ipar = 0; ipar < NPAR; ipar++) {
            int i = ich * NPAR + ipar;
            cout << "   " << i << ": " << res[i] << " +/- " << err[i] << endl;
        }
        cout << endl;

        // fill histogram with determinant for local 2nd deriv matrix
        if (chname[4] == 'A' || !DO_EC_ONLY) {
            double cond = chDerivs.condition(ich);
            int ipar = ich * NPAR + NPAR - 6 + 2;  // z-parameter
            cout << "ich: " << ich << ", cond: " << cond << ", res: " << res[ipar] << endl;
            h_tVCond->Fill(log10(cond), res[ipar]);
            h_tVSV->Fill(log10(svArray[ich]), log10(std::fabs(res[ipar])));
        }
    }
    delete[] res;
    delete[] err;

    TCanvas* canv_tvcond = new TCanvas("canv_tvcond", "align t vs. condition number");
    h_tVCond->Draw("colz");
    canv_tvcond->Print("tvcond.eps");
    delete canv_tvcond;

    TCanvas* canv_tvsv = new TCanvas("canv_tvsv", "align t vs. singular value");
    h_tVSV->Draw("colz");
    canv_tvsv->Print("tvsv.eps");
    delete canv_tvsv;

    int xsize = 7000;
    if (DO_EC_ONLY)
        xsize = 3000;
    else if (DO_ASIDE_ONLY)
        xsize = 4000;

    // TCanvas* canv=new TCanvas("canv","canv");
    // canv->SetFillColor(10);
    // makeHitmap(chDerivs,canv);
    // canv->Print("hitmap.eps");

    TCanvas** canvAP = new TCanvas*[NPAR];
    TCanvas** canvAP2 = new TCanvas*[NPAR];
    for (int ipar = 0; ipar < NPAR; ipar++) {
        stringstream canvname;
        canvname << "alignPar_" << ipar;
        canvAP[ipar] = new TCanvas(canvname.str().c_str(), canvname.str().c_str());
        canvAP[ipar]->SetFillColor(10);

        stringstream canvname2;
        canvname2 << "alignPar_" << ipar << "_2";
        canvAP2[ipar] = new TCanvas(canvname2.str().c_str(), canvname2.str().c_str());
        canvAP2[ipar]->SetFillColor(10);
    }

    make2DAlignHists(chDerivs, (TPad**)canvAP, (TPad**)canvAP2);
    for (int ipar = 0; ipar < NPAR; ipar++) {
        stringstream epsname;
        epsname << "alignPar_" << ipar << ".eps";
        canvAP[ipar]->Print(epsname.str().c_str());

        stringstream epsname2;
        epsname2 << "alignParVNTrks_" << ipar << ".eps";
        canvAP2[ipar]->Print(epsname2.str().c_str());
    }

    TCanvas* canv = new TCanvas("canv", "canv");
    canv->SetFillColor(10);
    double ranges[NPAR] = {RANGE_TRA, RANGE_TRA, RANGE_ROTS, RANGE_ROTZT, RANGE_ROTZT};
    TH1F** alignParHists = alignParDist(chDerivs, "alignParDist", ranges);
    TF1** alignParFits = new TF1*[NPAR];
    for (int ipar = 0; ipar < NPAR; ipar++) {
        canv->Clear();
        alignParHists[ipar]->Draw();
        alignParHists[ipar]->SetLineWidth(.5);

        stringstream fitstr;
        fitstr << "gaus_" << ipar;
        alignParFits[ipar] = new TF1(fitstr.str().c_str(), "gaus", -1.5, 1.5);
        alignParHists[ipar]->Fit(alignParFits[ipar], "QR");
        alignParFits[ipar]->SetLineWidth(1.);

        int thispar = ipar + 6 - NPAR;
        alignParHists[ipar]->GetXaxis()->SetTitle((parstr[thispar] + " (" + unitstr[thispar] + ")").c_str());
        stringstream histname;
        histname << "alignParDist_" << ipar << ".eps";
        canv->Print(histname.str().c_str());
    }
    delete[] alignParHists;
    delete[] alignParFits;
}

//________________________________________________________________________
TH1F** alignParDist(const ChamberDerivs& chDerivs, const string& histname, double* ranges) {
    double* err(0);
    double* res = chDerivs.solve(err, -1, ChamberDerivs::ROOT);

    TH1F** hists = new TH1F*[NPAR];
    for (int ipar = 0; ipar < NPAR; ipar++) {
        double range = 10.;
        if (ranges) range = ranges[ipar];
        stringstream name;
        name << histname << "_" << ipar;
        hists[ipar] = new TH1F(name.str().c_str(), name.str().c_str(), 30, -range, range);

        for (int ich = 0; ich < chDerivs.nchambers(); ich++) {
            int i = ich * NPAR + ipar;
            double result = res[i];
            hists[ipar]->Fill(result);
        }
    }
    return hists;
}

//________________________________________________________________________
void plotDiagonalCutHists(bool offdiag) {
    const int NTRKHISTS = 5;
    TH1F*** trkHists = new TH1F**[NTRKHISTS];
    TF1*** trkHistFits = new TF1**[NTRKHISTS];
    TPaveStats*** trkHistStats = new TPaveStats**[NTRKHISTS];

    int *ntrksdiag(new int[NTRKHISTS]), *ntrksoffdiag(new int[NTRKHISTS]);
    for (int itrk = 0; itrk < NTRKHISTS; itrk++) {
        ntrksdiag[itrk] = (offdiag) ? 5000 : 1000 + 2000 * itrk;
        ntrksoffdiag[itrk] = (offdiag) ? 400 * 400 * itrk : 400;

        ChamberDerivs trkChDerivs = m_allChDerivs->trimmedChamberDerivs(ntrksdiag[itrk], ntrksoffdiag[itrk]);
        stringstream histname;
        histname << "ntrks_" << ntrksdiag[itrk] << "_" << ntrksoffdiag[itrk] << "_alignPar";
        trkHists[itrk] = alignParDist(trkChDerivs, histname.str());
        trkHistFits[itrk] = new TF1*[NPAR];
        trkHistStats[itrk] = new TPaveStats*[NPAR];
    }

    TLegend** legs = new TLegend*[NPAR];
    TCanvas* ctmp = new TCanvas("ctmp", "ctmp");
    int colors[NTRKHISTS] = {kRed, kGreen, kBlue, kMagenta, kCyan};
    TCanvas* canvTrkHists = new TCanvas("canvTrkHists", "canvTrkHists");
    int optfit = gStyle->GetOptFit();
    int optstat = gStyle->GetOptStat();
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(11);
    canvTrkHists->SetFillColor(10);
    // canvTrkHists->Divide(3,2);
    for (int ipar = 0; ipar < NPAR; ipar++) {
        if (ipar != 1) continue;
        stringstream fitname;
        fitname << "trkHistFit_" << ipar;
        double y2pos(0.), deltay(0.), x2pos(0), deltax(0.);

        legs[ipar] = new TLegend(.5, .7, .7, .85);
        for (int itrk = 0; itrk < NTRKHISTS; itrk++) {
            // fit to gaussian
            ctmp->cd();
            trkHistFits[itrk][ipar] = new TF1(fitname.str().c_str(), "gaus", -8., 8.);
            trkHists[itrk][ipar]->Fit(trkHistFits[itrk][ipar], "QR");
            trkHistFits[itrk][ipar]->SetLineColor(colors[itrk]);
            trkHistFits[itrk][ipar]->SetLineWidth(1);

            // canvTrkHists->cd(ipar+2);
            canvTrkHists->cd();
            trkHists[itrk][ipar]->SetLineColor(colors[itrk]);
            trkHists[itrk][ipar]->SetLineWidth(.5);
            if (itrk == 0)
                trkHists[itrk][ipar]->Draw();
            else
                trkHists[itrk][ipar]->Draw("sames");
            trkHistFits[itrk][ipar]->Draw("sames");

            canvTrkHists->Update();
            trkHistStats[itrk][ipar] = (TPaveStats*)trkHists[itrk][ipar]->GetFunction("stats");
            TPaveStats* p = trkHistStats[itrk][ipar];
            if (!p) {
                cout << "no TPaveStats!" << endl;
                exit(3);
            }
            if (itrk == 0) {
                x2pos = p->GetX2NDC();
                y2pos = p->GetY2NDC();
                deltax = p->GetX2NDC() - p->GetX1NDC();
                deltay = p->GetY2NDC() - p->GetY1NDC();
                deltax *= .6;
                deltay *= .6;
            } else
                y2pos -= deltay;

            p->SetY2NDC(y2pos);
            p->SetY1NDC(y2pos - deltay);
            if (itrk > NTRKHISTS / 2 - 1) x2pos = .2 + deltax;
            p->SetX1NDC(x2pos - deltax);
            p->SetX2NDC(x2pos);

            p->SetTextColor(colors[itrk]);
            canvTrkHists->Modified();

            cout << "x1/x2: " << p->GetX1NDC() << "/" << p->GetX2NDC() << endl;
            cout << "y1/y2: " << p->GetY1NDC() << "/" << p->GetY2NDC() << endl << endl;
            // p->Draw();
            // canvTrkHists->Modified();

            // stringstream legentry; legentry<<ntrksdiag[itrk]<<" "<<ntrksoffdiag[itrk];
            // legs[ipar]->AddEntry(trkHistFits[itrk][ipar],legentry.str().c_str(),"l");
        }
        // legs[ipar]->Draw();
    }
    if (offdiag)
        canvTrkHists->Print("trkHistsOffDiag.eps");
    else
        canvTrkHists->Print("trkHistsDiag.eps");

    delete[] trkHists;
    delete[] trkHistFits;
    delete[] trkHistStats;

    delete[] ntrksdiag;
    delete[] ntrksoffdiag;

    delete ctmp;
    gStyle->SetOptFit(optfit);
    gStyle->SetOptStat(optstat);

    return;
}

//________________________________________________________________________
bool trackSelection() {
    if (L1ALIGNMENT) return true;

    if (!DO_EE_ONLY) {
        if (m_sd_nChambers < 3) return false;

        for (int ich = 0; ich < m_sd_nChambers; ich++) {
            int chid = m_sd_chamberIds[ich];
            string chname = m_chNames->chamberName(chid);

            // remove BEE tracks
            if (chname.substr(0, 4) == "EIS2" || chname.substr(0, 4) == "EIS1") {
                for (int jch = 0; jch < m_sd_nChambers; jch++) {
                    int jid = m_sd_chamberIds[jch];
                    string jname = m_chNames->chamberName(jid).substr(0, 4);
                    if (jname == "EOS3" || jname == "EOS4" || jname == "EOS5" || jname == "EOS6") return false;
                }
            }

            // remove CSC tracks
            if (chname.substr(0, 2) == "EM" && chname.substr(3, 1) == "1") return false;
            if (chname.substr(0, 2) == "EO" && (chname.substr(3, 1) == "1" || chname.substr(3, 1) == "2")) return false;
        }
    } else {
        // get number of inner, middle, and outer layer hits

        // cout<<"nInnerLayerHits:"<<nInnerLayersHit<<", nMiddleLayerHits:"<<nMiddleLayersHit
        // <<", nOuterLayerHits:"<<nOuterLayersHit<<endl;
        if (nInnerLayersHit < 8 || nMiddleLayersHit < 6 || nOuterLayersHit < 6) return false;

        bool hasEEL = false;
        bool hasEE = false;
        for (int ich = 0; ich < m_sd_nChambers; ich++) {
            int chid = m_sd_chamberIds[ich];
            string chname = m_chNames->chamberName(chid);
            if (chname.substr(0, 3) == "BEE" || chname.substr(0, 3) == "EEL") {
                hasEE = true;
                if (chname.substr(0, 3) == "EEL") hasEEL = true;
                break;
            }
        }
        if (!hasEE) return false;
        /*if (hasEEL)
          cout<<"nInnerLayerHits:"<<nInnerLayersHit<<", nMiddleLayerHits:"<<nMiddleLayersHit
              <<", nOuterLayerHits:"<<nOuterLayersHit<<endl;
        */
    }

    return true;
}

/*
//________________________________________________________________________
void getFinalPars(double* initPar, double* deltas, double* finalPars)
{
  double tras(0.), traz(0.), trat(0.), rots(0.), rotz(0.), rott(0.);
  double dtras(0.),dtraz(0.),dtrat(0.),drots(0.),drotz(0.),drott(0.);
  int firsti=0;
  if (NPAR==6) {
    tras=initPar[0];
    dtras=deltas[0];
    firsti=1;
  }
  traz=initPar[firsti]; dtraz=deltas[firsti++];
  trat=initPar[firsti]; dtrat=deltas[firsti++];
  rots=initPar[firsti]; drots=deltas[firsti++];
  rotz=initPar[firsti]; drotz=deltas[firsti++];
  rott=initPar[firsti]; drott=deltas[firsti++];

  HepGeom::Transform3D init(TranslateX3D(trat)*TranslateY3D(tras)*
                            TranslateZ3D(traz)*RotateX3D(rott)*
                            RotateY3D(rots)*RotateZ3D(rotz));

  HepGeom::Transform3D delta(TranslateX3D(dtrat)*TranslateY3D(dtras)*
                             TranslateZ3D(dtraz)*RotateX3D(drott)*
                             RotateY3D(drots)*RotateZ3D(drotz));

  //cout<<"init: "<<init[0][3]<<" "<<init[1][3]<<" "<<init[2][3]<<endl;
  //cout<<"delta: "<<delta[0][3]<<" "<<delta[1][3]<<" "<<delta[2][3]<<endl;

  double* vals=new double[6];
  HepGeom::Transform3D prod=delta*init;
  //cout<<"prod: "<<prod[0][3]<<" "<<prod[1][3]<<" "<<prod[2][3]<<endl;

  decomposeTransform(prod,vals);
  //for (int i=0;i<6;i++) cout<<"vals["<<i<<"]="<<vals[i]<<endl;

  if (NPAR==6) finalPars[0]=vals[1];
  else finalPars[0]=0.;        // s
  finalPars[1]=vals[2]; // z
  finalPars[2]=vals[0]; // t
  finalPars[3]=vals[4]; // rots
  finalPars[4]=vals[5]; // rotz
  finalPars[5]=vals[3]; // rott
  for (int i=0;i<6;i++)
    cout<<"finalPars["<<i<<"]="<<finalPars[i]<<endl;

  //delete [] vals;

  return;
}
*/

//________________________________________________________________________
void getFinalPars(double* initPar, double* deltas, double* finalPars) {
    double tras(0.), traz(0.), trat(0.), rots(0.), rotz(0.), rott(0.);
    double dtras(0.), dtraz(0.), dtrat(0.), drots(0.), drotz(0.), drott(0.);
    int firsti = 0;

    cout << "s/z/t: " << tras << "/" << traz << "/" << trat << endl;
    cout << "rot s/z/t: " << rots << "/" << rotz << "/" << rott << endl;

    if (NPAR == 6) {
        tras = initPar[0];
        dtras = deltas[0];
        firsti = 1;
    }
    traz = initPar[firsti];
    dtraz = deltas[firsti++];
    trat = initPar[firsti];
    dtrat = deltas[firsti++];
    rots = initPar[firsti];
    drots = deltas[firsti++];
    rotz = initPar[firsti];
    drotz = deltas[firsti++];
    rott = initPar[firsti];
    drott = deltas[firsti++];

    cout << "d s/z/t: " << dtras << "/" << dtraz << "/" << dtrat << endl;
    cout << "d rot s/z/t: " << drots << "/" << drotz << "/" << drott << endl;

    tras += dtras;
    traz += dtraz;
    trat += dtrat;
    rots += drots;
    rotz += drotz;
    rott += drott;

    cout << "s/z/t: " << tras << "/" << traz << "/" << trat << endl;
    cout << "rot s/z/t: " << rots << "/" << rotz << "/" << rott << endl;

    if (NPAR == 6)
        finalPars[0] = tras;
    else
        finalPars[0] = 0.;
    finalPars[1] = traz;
    finalPars[2] = trat;
    finalPars[3] = rots;
    finalPars[4] = rotz;
    finalPars[5] = rott;

    return;
}

//________________________________________________________________________
void decomposeTransform(const HepGeom::Transform3D& transform, double* values) {
    // copied from AlignModule
    HepGeom::Scale3D scale;
    HepGeom::Rotate3D rotation;
    HepGeom::Translate3D translation;
    transform.getDecomposition(scale, rotation, translation);

    values[0] = translation.dx();
    values[1] = translation.dy();
    values[2] = translation.dz();

    // a12 = cosbeta singamma
    // a13 = -sinbeta
    // a23 =  sinalpha cosbeta

    double sinbeta = -rotation[0][2];
    double cosbeta = std::sqrt(1. - sinbeta * sinbeta);

    double sinalpha = rotation[1][2] / cosbeta;
    double singamma = rotation[0][1] / cosbeta;

    values[3] = -std::asin(sinalpha);
    values[4] = -std::asin(sinbeta);
    values[5] = -std::asin(singamma);
}

/*
//________________________________________________________________________
double* fixAlongTrack(double* secDerivs) const
{
  TMatrixDSym mat(5);
  int index(0);
  for (int i=0;i<5;i++) {
    for (int j=0;j<5;j++) {
      mat[i][j]=(*secDerivs)[index++];
    }
  }

  TDecompSVD decompMat(mat);
  decompMat.Decompose();
  const TVectorD& svV=decompMat.GetSig();

  vector<double> sv;
  for (int i=0;i<svV.GetNoElements();i++)
    sv.push_back(svV[i]);

  u=new TMatrixD(decompMat.GetU());
  u->Print();

  TMatrixD v=decompMat.GetV();
  vinv=new TMatrixD(v.Invert());
  cout<<"Vinv:"<<endl;
  vinv->Print();

  TMatrixDSym sd(5);
  for (int i=0;i<3;i++)
    sd[i][i]=sv[i];
  sd[3][3]=sv[0];
  sd[4][4]=sv[0];
  TMatrixD D=(*u)*sd*(*vinv);
  D.Print();

  double* vals=new double[5];
  index=0;
  for (int i=0;i<5;i++) {
    for (int j=i;j<5;j++) {
      vals[index++]=D[i][j];
    }
  }
  return vals;
}
*/
