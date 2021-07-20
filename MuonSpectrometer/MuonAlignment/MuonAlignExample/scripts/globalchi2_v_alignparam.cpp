/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <TAxis.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TDecompBK.h>
#include <TEventList.h>
#include <TF1.h>
#include <TGraph.h>
#include <TLorentzVector.h>
#include <TMatrixDSym.h>
#include <TMatrixTSparse.h>
#include <TPaveText.h>
#include <TTree.h>
#include <TVectorD.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "MuonFixedId.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLine.h"
#include "TStyle.h"
#include "TSystem.h"
#include "Tools.h"

using namespace MuonCalib;

void Init(TTree* tree);
void getDataFromTree(TChain* chain, std::map<long long int, double**>& fullArrayMap, std::map<long long int, double**>& xarrayMap,
                     std::map<long long int, long long int>& numTracks, std::map<long long int, string>& chamberNames,
                     std::map<long long int, bool*>& goodParams);

void getDataFromAlignParTree(TChain* chain, std::map<long long int, double*>& firstderivMap,
                             std::map<long long int, double*>& secndderivMap, std::map<long long int, double*>& initPosMap,
                             std::map<long long int, double*>& alignParMap, std::map<long long int, double*>& alignParErrMap);

bool DEBUG = 1;
const int NSHIFTS = 11;
const int NPARAMS = 6;
string xtitles[NPARAMS] = {"#Deltas", "#Deltaz", "#Deltat", "#Delta#alpha(s)", "#Delta#beta(z)", "#Delta#gamma(t)"};
string units[NPARAMS] = {" [mm]", " [mm]", " [mm]", " [mrad]", " [mrad]", " [mrad]"};

MuonFixedId* mid;

double* secderiv = new double[NPARAMS];
double* firstderiv = new double[NPARAMS];
double* initPos = new double[NPARAMS];
double* alignPar = new double[NPARAMS];
double* alignParErr = new double[NPARAMS];
double scale[NPARAMS] = {1., 1., 1., 1000., 1000., 1000.};
double nominalPos[NPARAMS] = {0., 0., 0., 0., 0., 0.};

bool goodpar[NPARAMS] = {0, 0, 0, 0, 0, 0};

int m_ntrees = 0;

TString outputdir("");
TString rootname("");
int iteration = 0;
int kindex = -1;
string chname = "";

void help(const char* argv0) {
    cerr << "Usage: " << argv0 << " [options] " << endl
         << "Options:" << endl
         << " -o :\tOutput path" << endl
         << " -i :\tInput files" << endl
         << " -r :\tRootname" << endl
         << " -j :\tIteration" << endl
         << " -a :\tDraw absolute lines" << endl
         << " -l :\tL1 alignment" << endl
         << " -d :\tDebug" << endl
         << " -h :\tShow this message and quit" << endl
         << endl;
    exit(1);
}

//_____________________________________________________________________________
int main(int argc, char** argv) {
    //----------------------------
    //-- Parse the command line --
    //----------------------------

    extern char* optarg;
    char c;
    TString inputdir("");

    bool L1ALIGNMENT = false;
    bool ABSLINES = false;

    while ((c = getopt(argc, argv, "-hdalo:i:r:j:k:")) != -1) {
        switch (c) {
            case 'o': outputdir = optarg; break;
            case 'i': inputdir = optarg; break;
            case 'r': rootname = optarg; break;
            case 'j': iteration = atoi(optarg); break;
            case 'k': kindex = atoi(optarg); break;
            case 'd': DEBUG = true; break;
            case 'a': ABSLINES = true; break;
            case 'l': L1ALIGNMENT = true; break;
            default:
                cerr << "Help or unknown argument!" << endl;
                help(argv[0]);
                break;
        }
    }
    if (rootname == "") {
        cerr << "no rootname passed, exiting..." << endl;
        exit(1);
    }
    if (inputdir == "") { inputdir = rootname; }
    if (outputdir == "") { outputdir = inputdir; }

    //----------------
    //-- Initialize --
    //----------------

    if (L1ALIGNMENT) {
        xtitles[0] = "#Deltax";
        xtitles[1] = "#Deltay";
        xtitles[2] = "#Deltaz";
        xtitles[3] = "#Delta#alpha(x)";
        xtitles[4] = "#Delta#beta(y)";
        xtitles[5] = "#Delta#gamma(z)";
    }

    Tools::setStyle();
    double tsize = 0.05;
    gStyle->SetTextSize(tsize);
    gStyle->SetLabelSize(tsize, "x");
    gStyle->SetTitleSize(tsize, "x");
    gStyle->SetLabelSize(tsize, "y");
    gStyle->SetTitleSize(tsize, "y");
    //  gStyle->SetFrameFillStyle(0);
    //   gStyle->SetTitleFillColor(0);
    //   gStyle->SetTitleBorderSize(1);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadRightMargin(0.04);
    gStyle->SetTitleYOffset(1.5);
    // gStyle->SetOptStat(1111111);
    // gStyle->SetOptFit(11111);
    gStyle->SetFrameFillStyle(0);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(1);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadRightMargin(0.04);
    gStyle->SetPalette(1);

    //  std::string directory="";
    //   if (argc>1) {
    //     directory=string(argv[1]);
    //   }
    //   std::string iteration="0";
    //   if (argc>2) {
    //     iteration=string(argv[2]);
    //   }
    //   std::cout<<"using input directory "<<directory<<std::endl;

    std::string asztfile = "";
    // if (argc>3) {
    //   asztfile=string(argv[3]);
    //}

    if (asztfile == "") {
        nominalPos[1] = 0.;
        nominalPos[2] = 0.;
        nominalPos[3] = 0.;
        nominalPos[4] = 0.;
        nominalPos[5] = 0.;
        //}
    }

    /*
      TMatrixD nomMatrix(4,3);
      for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
          if (i==j) nomMatrix[i][j]=1.;
          else      nomMatrix[i][j]=0.;
      nomMatrix[0][3]=0.;
      nomMatrix[1][3]=0.;
      nomMatrix[2][3]=5700.;
    */

    else {
        // read nomimal pos from asztfile
        // Tools:getPos has to be changed to return double[]
    }

    mid = new MuonFixedId(0);

    //-------------------------
    //-- Get data from trees --
    //-------------------------

    std::map<long long int, double**> fullArrayMap;
    std::map<long long int, double**> xarrayMap;
    std::map<long long int, long long int> numTracks;
    std::map<long long int, string> chamberNames;
    std::map<long long int, bool*> goodParams;

    void* directory = gSystem->OpenDirectory(inputdir);
    if (!directory) {
        cerr << "input directory does not exist, exiting..." << endl;
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
    cout << "multiplejobs: " << multiplejobs << endl;

    TString basefilename;
    if (multiplejobs) {
        TString iterdir = inputdir + Form("/iter%i/", iteration);
        directory = gSystem->OpenDirectory(iterdir);
        basefilename = iterdir;
        if (!directory) {
            cerr << "iteration: " << iteration << "does not exists, exiting..." << endl;
            exit(1);
        }
    } else {
        basefilename = inputdir + "/";
    }

    basefilename += "MuonAlign_" + rootname + Form("_iter%i", iteration);
    if (kindex > -1) basefilename += Form("_index%i", kindex);

    TString muonalign_filename = basefilename;
    TString alignpar_filename = basefilename;
    if (multiplejobs && kindex < 0) {
        muonalign_filename += "_index*";
        alignpar_filename += "_Solve";
    }
    muonalign_filename += ".root";
    alignpar_filename += ".root";

    std::map<long long int, double*> firstderivMap;
    std::map<long long int, double*> secndderivMap;
    std::map<long long int, double*> initPosMap;
    std::map<long long int, double*> alignParMap;
    std::map<long long int, double*> alignParErrMap;

    TChain* tree_alignPar = new TChain("alignParMDT");
    std::cout << "adding to alignpar chain: " << alignpar_filename << std::endl;
    tree_alignPar->Add(alignpar_filename);

    getDataFromAlignParTree(tree_alignPar, firstderivMap, secndderivMap, initPosMap, alignParMap, alignParErrMap);

    TChain* tree = new TChain("MuonAlign");
    std::cout << "adding to muonalign chain: " << muonalign_filename << std::endl;
    tree->Add(muonalign_filename);

    m_ntrees = tree->GetNtrees();
    std::cout << "have " << m_ntrees << " trees" << std::endl;

    getDataFromTree(tree, fullArrayMap, xarrayMap, numTracks, chamberNames, goodParams);

    std::cout << "alignpar_fiilename=" << alignpar_filename << std::endl;

    //------------------
    //-- Create plots --
    //------------------

    int totalnchambers = (int)fullArrayMap.size();
    std::cout << totalnchambers << " total chamber(s)" << std::endl;

    // now fit and plot results
    TGraph*** gr = new TGraph**[NPARAMS];
    TPaveText*** pave = new TPaveText**[NPARAMS];
    double** firstderivs = new double*[NPARAMS];
    double** secondderivs = new double*[NPARAMS];
    double** zeros = new double*[NPARAMS];
    double** sigmas = new double*[NPARAMS];

    for (int i = 0; i < NPARAMS; i++) {
        gr[i] = new TGraph*[totalnchambers];
        pave[i] = new TPaveText*[totalnchambers];
        firstderivs[i] = new double[totalnchambers];
        secondderivs[i] = new double[totalnchambers];
        zeros[i] = new double[totalnchambers];
        sigmas[i] = new double[totalnchambers];
    }
    long long int* identifiers = new long long int[totalnchambers];

    int ichamber(0);

    std::map<long long int, double**>::const_iterator it = fullArrayMap.begin();
    std::map<long long int, double**>::const_iterator itend = fullArrayMap.end();
    std::map<long long int, double**>::const_iterator xit = xarrayMap.begin();
    std::map<long long int, long long int>::const_iterator numTracksit = numTracks.begin();
    std::map<long long int, string>::const_iterator chamberNamesit = chamberNames.begin();
    std::map<long long int, bool*>::const_iterator goodParIt = goodParams.begin();

    int nPlotChambers(0);
    const int CHAMBERSPERPAGE = 1;

    TCanvas* canv = new TCanvas("canv", "canv");
    canv->SetFillColor(10);
    canv->Divide(3, 2 * CHAMBERSPERPAGE);
    int ipad(1);
    int nCanvas(0);
    bool* goodpar;
    int ntracks = -1;
    // double* xarray;
    bool saveplot(1);

    for (; it != itend; ++it, ++numTracksit, ++chamberNamesit, ++xit, ++goodParIt) {
        saveplot = 1;
        if (chname != "" && nPlotChambers == CHAMBERSPERPAGE) {
            nPlotChambers = 0;
            stringstream canvname;
            canvname << outputdir << "/" << rootname << "_chi2_v_alignparams_" << chname << "_iter" << iteration;
            if (kindex > -1) canvname << "_index" << kindex;
            canvname << ".eps";
            canv->Print(canvname.str().c_str());
            nCanvas++;
            canv->Clear();
            canv->SetFillColor(10);
            canv->Divide(3, 2 * CHAMBERSPERPAGE);
            ipad = 1;
        }

        chname = chamberNamesit->second;
        ntracks = numTracksit->second;
        goodpar = goodParIt->second;
        // xarray=xit->second;

        identifiers[ichamber] = chamberNamesit->first;

        int id = identifiers[ichamber];
        if (DEBUG) std::cout << "id = " << id << std::endl;
        alignPar = alignParMap[id];
        alignParErr = alignParErrMap[id];
        firstderiv = firstderivMap[id];
        secderiv = secndderivMap[id];
        initPos = initPosMap[id];
        if (0 == secderiv) {
            saveplot = 0;
            std::cout << "didn't find secderiv for id " << id << std::endl;
            continue;
        }
        std::cout << "found secderiv for id " << id << std::endl;

        TLine** vertlines = new TLine*[NPARAMS];
        TLine** vertlines2 = new TLine*[NPARAMS];

        // TLine** vertline_errlow=new TLine*[6];
        // TLine** vertline_errhigh=new TLine*[6];
        // TBox**  errbox=new TBox*[6];

        for (int iparam = 0; iparam < 6; iparam++) {
            if (!goodpar[iparam]) {
                ipad++;
                continue;
            }

            cout << "---------------------------------------------------" << endl;
            cout << "first/secderiv=" << firstderiv[iparam] << ", " << secderiv[iparam] << endl;

            double initShift(0.);
            if (ABSLINES) { initShift = initPos[iparam]; }
            double* x = new double[NSHIFTS];
            for (int i = 0; i < NSHIFTS; i++) {
                // x[i]=-4.*xarray[iparam]+(double)i*xarray[iparam];
                x[i] = (xit->second)[iparam][i];  //-ymin;
                x[i] += initShift;
            }
            std::cout << "alignPar[" << iparam << "]=" << alignPar[iparam] << std::endl;

            canv->cd(ipad);
            if (alignPar[iparam] == 0.) continue;

            double* y = new double[NSHIFTS];
            for (int i = 0; i < NSHIFTS; i++) {
                y[i] = (it->second)[iparam][i];  //-ymin;
            }
            // double* x = new double[NSHIFTS];
            // for (int i=0;i<NSHIFTS;i++) {
            //	x[i]=(xit->second)[iparam][i];//-ymin;
            //}

            gr[iparam][ichamber] = new TGraph(NSHIFTS, x, y);
            TGraph* thisgr = gr[iparam][ichamber];
            thisgr->SetMarkerSize(1);
            thisgr->SetMarkerStyle(20);
            thisgr->SetLineWidth(3);
            thisgr->Draw("AP");
            thisgr->GetXaxis()->SetTitle((xtitles[iparam] + units[iparam]).c_str());
            //   thisgr->GetXaxis()->SetTitleSize(0.045);
            thisgr->GetYaxis()->SetTitle("global #chi^{2}");
            //       thisgr->GetYaxis()->SetTitleSize(0.06);
            // thisgr->GetYaxis()->SetTitleOffset(1.6);
            thisgr->SetTitle((chname + string(" - ") + xtitles[iparam]).c_str());
            thisgr->Fit("pol2", "Q");

            TF1* fit = gr[iparam][ichamber]->GetFunction("pol2");

            stringstream fitname;
            fitname << "fit" << iparam;
            TF1* fit2 = new TF1(fitname.str().c_str(), "pol2", x[0], x[NSHIFTS - 1]);

            double a = 0.5 * secderiv[iparam];
            double b = firstderiv[iparam] - secderiv[iparam] * initShift;
            // double c=fit->GetMinimum() + b*b/a/4.;
            double c = fit->Eval(0.);
            // if (std::fabs(y[0]-y[4])<1.e-5) {
            //        c=y[0]; // special case where first deriv is negligible
            //        a=b=0.;
            //}

            fit2->SetParameter(2, a);
            fit2->SetParameter(1, b);
            fit2->SetParameter(0, c);
            fit2->SetLineColor(kRed);

            // double min=fit->GetMinimum();
            // if (fit2->GetMinimum()<min) {
            //  fit2->Draw();
            // gr[iparam][ichamber]->Draw("Psame");
            // fit->Draw("same");
            //}
            // else {

            fit->Draw("same");
            //}

            fit2->Draw("same");

            // if (DEBUG) std::cout<<"identifiers["<<ichamber<<"]="<<identifiers[ichamber]<<std::endl;

            double gr_Xmin = thisgr->GetXaxis()->GetXmin();
            double gr_Xmax = thisgr->GetXaxis()->GetXmax();
            double gr_Ymin = thisgr->GetYaxis()->GetXmin();
            double gr_Ymax = thisgr->GetYaxis()->GetXmax();

            double line1pos(0.);
            double line2pos(0.);

            if (ABSLINES) {
                line1pos = nominalPos[iparam];
                line2pos = initPos[iparam] + alignPar[iparam];
            } else {
                line1pos = nominalPos[iparam] + initPos[iparam];
                line2pos = nominalPos[iparam] + initPos[iparam] + alignPar[iparam];
            }
            if (line2pos > gr_Xmax) line2pos = gr_Xmax;
            if (line2pos < gr_Xmin) line2pos = gr_Xmin;

            // cout << "line1: " <<

            // vertical lines
            // if (identifiers[ichamber]==240131072) {
            //  std::cout<<"drawing vert line"<<std::endl;
            // cout << "min: " << thisgr->GetYaxis()->GetXmin() << endl;

            vertlines[iparam] = new TLine(line1pos, gr_Ymin, line1pos, gr_Ymax);
            vertlines[iparam]->SetLineColor(kBlue);
            vertlines[iparam]->SetLineWidth(2);
            vertlines[iparam]->Draw("same");
            //}

            vertlines2[iparam] = new TLine(line2pos, gr_Ymin, line2pos, gr_Ymax);
            vertlines2[iparam]->SetLineColor(kGreen);
            vertlines2[iparam]->SetLineWidth(2);
            vertlines2[iparam]->Draw("same");

            //   vertline_errlow[iparam]=new TLine(alignPar[iparam]-alignParErr[iparam],gr_Ymin,
            //                                 alignPar[iparam]-alignParErr[iparam],gr_Ymax);
            //       vertline_errlow[iparam]->SetLineColor(kGreen);
            //       vertline_errlow[iparam]->SetLineWidth(1);
            //       vertline_errlow[iparam]->Draw("same");
            //       vertline_errhigh[iparam]=new TLine(alignPar[iparam]+alignParErr[iparam],gr_Ymin,
            //                                  alignPar[iparam]+alignParErr[iparam],gr_Ymax);
            //       vertline_errhigh[iparam]->SetLineColor(kGreen);
            //       vertline_errhigh[iparam]->SetLineWidth(1);
            //       vertline_errhigh[iparam]->Draw("same");

            //       errbox[iparam]=new TBox(alignPar[iparam]-alignParErr[iparam],gr_Ymin,
            //                               alignPar[iparam]+alignParErr[iparam],gr_Ymax);
            //       errbox[iparam]->SetFillColor(kGreen);
            //       errbox[iparam]->SetFillStyle(3144);
            //       errbox[iparam]->SetLineWidth(0);
            //       errbox[iparam]->Draw("same");

            // pave[iparam][ichamber]=new TPaveText(.40,.65,.97,.92,"NDC");
            pave[iparam][ichamber] = new TPaveText(.40, .75, .98, .98, "NDC");

            TPaveText* thispave = pave[iparam][ichamber];
            thispave->SetFillColor(0);
            thispave->SetBorderSize(1);
            thispave->SetTextAlign(12);

            firstderivs[iparam][ichamber] = fit->GetParameter(1);
            secondderivs[iparam][ichamber] = 2. * fit->GetParameter(2);
            sigmas[iparam][ichamber] = std::sqrt(2. / fit->GetParameter(2));
            zeros[iparam][ichamber] = fit->GetMinimumX(x[0], x[NSHIFTS - 1]);
            // if (DEBUG) {
            // cout<<"\nfit: "<<endl;
            // fit->Print();
            //}

            // stringstream idstr; idstr<<numTracksit->first;
            // thispave->AddText(idstr.str().c_str());

            stringstream num;
            num << "Number of tracks: " << ntracks;  //<<" tracks";
            thispave->AddText(num.str().c_str());

            // std::cout<<"correct/fit="<<fit->GetParameter(1)<<"/"<<fit2->GetParameter(1)<<std::endl;

            stringstream first;
            first.setf(ios::scientific);
            first.precision(2);
            first << "1st deriv:  " << firstderivs[iparam][ichamber] << " #pm " << fit->GetParError(1);
            // std::cout<<"firstderivs["<<iparam<<"]["<<ichamber<<"]="<<firstderivs[iparam][ichamber]<<std::endl;
            // std::cout<<first.str()<<std::endl;
            thispave->AddText(first.str().c_str());
            stringstream secnd;
            secnd.setf(ios::scientific);
            secnd.precision(2);
            secnd << "2nd deriv: " << secondderivs[iparam][ichamber] << " #pm " << fit->GetParError(2);
            std::cout << "id 1stderiv 2ndderiv: " << identifiers[ichamber] << " " << firstderivs[iparam][ichamber] << " "
                      << secondderivs[iparam][ichamber] << std::endl;
            thispave->AddText(secnd.str().c_str());
            stringstream sigma;
            sigma.setf(ios::scientific);
            sigma.precision(2);
            sigma << "Minimum: " << zeros[iparam][ichamber] << " #pm " << sigmas[iparam][ichamber];
            thispave->AddText(sigma.str().c_str());
            thispave->Draw();
            canv->cd(ipad)->Update();
            delete[] y;
            delete[] x;

            // stringstream pcanvname;
            // pcanvname<<outputdir<<"/"<<rootname<<"_chi2_v_alignparams_"<<chname<<"_pad"<<ipad<<"_iter"<<iteration<<".eps";
            // if(saveplot)  canv->cd(ipad)->Print(pcanvname.str().c_str());

            ipad++;
        }

        nPlotChambers++;
        ichamber++;
    }
    stringstream canvname;
    canvname << outputdir << "/" << rootname << "_chi2_v_alignparams_" << chname << "_iter" << iteration;
    if (kindex > -1) canvname << "_index" << kindex;
    canvname << ".eps";
    if (saveplot) canv->Print(canvname.str().c_str());

    //---------------
    //-- Plot pull --
    //---------------

    gStyle->SetOptFit(1111);

    canv->Clear();
    canv->SetFillColor(10);
    canv->Divide(3, 2);
    int nplottedchambers = ichamber;
    TH1F** hist = new TH1F*[NPARAMS];
    for (int iparam = 0; iparam < 6; iparam++) {
        stringstream histname;
        histname << "pull_" << iparam;
        hist[iparam] = new TH1F(histname.str().c_str(), histname.str().c_str(), 15, -5., 5.);
        for (int ich = 0; ich < nplottedchambers; ich++) {
            double pull = zeros[iparam][ich] / sigmas[iparam][ich];
            hist[iparam]->Fill(pull);
        }
        canv->cd(iparam + 1);
        hist[iparam]->Draw();
        hist[iparam]->Fit("gaus", "Q");
    }

    if (saveplot) canv->Print(outputdir + "/" + rootname + "_pull" + Form("_iter%i", iteration) + ".eps");

    delete[] hist;
    delete canv;
}

//________________________________________________________________________
void Init(TTree* tree) {}

//________________________________________________________________________
void getDataFromTree(TChain* tree, std::map<long long int, double**>& fullArrayMap, std::map<long long int, double**>& xarrayMap,
                     std::map<long long int, long long int>& numTracks, std::map<long long int, string>& chamberNames,
                     std::map<long long int, bool*>& goodParams) {
    std::cout << "getting data from tree" << std::endl;
    // tree variables
    int run;
    int evt;
    int nchambers;
    long long int* chamberid = new long long int[10];

    TMatrixD* chi2_transx = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_transy = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_transz = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_rotx = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_roty = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_rotz = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_transx_X = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_transy_X = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_transz_X = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_rotx_X = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_roty_X = new TMatrixD(NSHIFTS, 10);
    TMatrixD* chi2_rotz_X = new TMatrixD(NSHIFTS, 10);

    tree->SetBranchAddress("run", &run);
    tree->SetBranchAddress("evt", &evt);
    tree->SetBranchAddress("nchambers", &nchambers);
    tree->SetBranchAddress("chamberid", chamberid);
    tree->SetBranchAddress("chi2_transx", &chi2_transx);
    tree->SetBranchAddress("chi2_transy", &chi2_transy);
    tree->SetBranchAddress("chi2_transz", &chi2_transz);
    tree->SetBranchAddress("chi2_rotx", &chi2_rotx);
    tree->SetBranchAddress("chi2_roty", &chi2_roty);
    tree->SetBranchAddress("chi2_rotz", &chi2_rotz);
    tree->SetBranchAddress("chi2_transx_X", &chi2_transx_X);
    tree->SetBranchAddress("chi2_transy_X", &chi2_transy_X);
    tree->SetBranchAddress("chi2_transz_X", &chi2_transz_X);
    tree->SetBranchAddress("chi2_rotx_X", &chi2_rotx_X);
    tree->SetBranchAddress("chi2_roty_X", &chi2_roty_X);
    tree->SetBranchAddress("chi2_rotz_X", &chi2_rotz_X);

    // double msid_ptDiff;
    double chi2VAlignParamQuality[6];
    // tree->SetBranchAddress("MSID_ptDiff",           &msid_ptDiff);
    tree->SetBranchAddress("chi2VAlignParamQuality", chi2VAlignParamQuality);

    std::cout << "in datatree, have " << tree->GetEntries() << " entries" << std::endl;

    /*
    TH2F** h2MSIDpTVTrackQual=new TH2F*[6];

     h2MSIDpTVTrackQual[0] =new TH2F("h2MSIDpTVTrackQualX",   "h2MSIDpTVTrackQualX",   30,-20.,10.,30,0.,0.2);
     h2MSIDpTVTrackQual[1] =new TH2F("h2MSIDpTVTrackQualY",   "h2MSIDpTVTrackQualY",   30,-20.,10.,30,0.,0.2);
     h2MSIDpTVTrackQual[2] =new TH2F("h2MSIDpTVTrackQualZ",   "h2MSIDpTVTrackQualZ",   30,-20.,10.,30,0.,0.2);
     h2MSIDpTVTrackQual[3] =new TH2F("h2MSIDpTVTrackQualRotX","h2MSIDpTVTrackQualRotX",30,-20.,10.,30,0.,0.2);
     h2MSIDpTVTrackQual[4] =new TH2F("h2MSIDpTVTrackQualRotY","h2MSIDpTVTrackQualRotY",30,-20.,10.,30,0.,0.2);
     h2MSIDpTVTrackQual[5] =new TH2F("h2MSIDpTVTrackQualRotZ","h2MSIDpTVTrackQualRotZ",30,-20.,10.,30,0.,0.2);
    */

    for (int ientry = 0; ientry < tree->GetEntries(); ientry++) {
        tree->GetEntry(ientry);

        for (int ich = 0; ich < nchambers; ich++) {
            bool* goodpar = new bool[6];
            for (int i = 0; i < 6; i++) goodpar[i] = true;

            // first make sure there are no zeros in the chi2
            for (int ish = 0; ish < NSHIFTS; ish++) {
                if ((*chi2_transx)[ish][ich] == 0.) goodpar[0] = false;
                if ((*chi2_transy)[ish][ich] == 0.) goodpar[1] = false;
                if ((*chi2_transz)[ish][ich] == 0.) goodpar[2] = false;
                if ((*chi2_rotx)[ish][ich] == 0.) goodpar[3] = false;
                if ((*chi2_roty)[ish][ich] == 0.) goodpar[4] = false;
                if ((*chi2_rotz)[ish][ich] == 0.) goodpar[5] = false;

                if (((*chi2_transx)[ish][ich] > 0. && ((*chi2_transx)[ish][ich] <= .001)) ||
                    ((*chi2_transy)[ish][ich] > 0. && ((*chi2_transy)[ish][ich] <= .001)) ||
                    ((*chi2_transz)[ish][ich] > 0. && ((*chi2_transz)[ish][ich] <= .001)) ||
                    ((*chi2_rotx)[ish][ich] > 0. && ((*chi2_rotx)[ish][ich] <= .001)) ||
                    ((*chi2_roty)[ish][ich] > 0. && ((*chi2_roty)[ish][ich] <= .001)) ||
                    ((*chi2_rotz)[ish][ich] > 0. && ((*chi2_rotz)[ish][ich] <= .001)))
                    continue;
            }

            // int hid=hitid[ich];
            long long int chamberId = chamberid[ich];

            stringstream chambername;
            if (chamberId < 3) {
                switch (chamberId) {
                    case 0: chambername << "endcapA"; break;
                    case 1: chambername << "endcapC"; break;
                    case 2: chambername << "barrel"; break;
                }
            } else {
                mid->setMuonIdentifier(chamberId);
                chambername << Tools::getChamberName(mid);
            }

            std::map<long long int, double**>::const_iterator it = fullArrayMap.find(chamberId);
            std::map<long long int, double**>::const_iterator xit = xarrayMap.find(chamberId);
            if (it == fullArrayMap.end()) {
                double** thisarray = new double*[NPARAMS];
                double** xarray = new double*[NPARAMS];
                for (int iparam = 0; iparam < NPARAMS; iparam++) {
                    thisarray[iparam] = new double[NSHIFTS];
                    xarray[iparam] = new double[NSHIFTS];
                }

                for (int ish = 0; ish < NSHIFTS; ish++) {
                    if (goodpar[0]) {
                        xarray[0][ish] = (*chi2_transx_X)[ish][ich];
                        thisarray[0][ish] = (*chi2_transx)[ish][ich];
                    }
                    thisarray[1][ish] = (*chi2_transy)[ish][ich];
                    thisarray[2][ish] = (*chi2_transz)[ish][ich];
                    thisarray[3][ish] = (*chi2_rotx)[ish][ich];
                    thisarray[4][ish] = (*chi2_roty)[ish][ich];
                    xarray[1][ish] = (*chi2_transy_X)[ish][ich];
                    xarray[2][ish] = (*chi2_transz_X)[ish][ich];
                    xarray[3][ish] = (*chi2_rotx_X)[ish][ich] * 1000;
                    xarray[4][ish] = (*chi2_roty_X)[ish][ich] * 1000;
                    if (goodpar[5]) {
                        xarray[5][ish] = (*chi2_rotz_X)[ish][ich] * 1000;
                        thisarray[5][ish] = (*chi2_rotz)[ish][ich];
                    }
                }
                fullArrayMap[chamberId] = thisarray;
                numTracks[chamberId] = 1;
                chamberNames[chamberId] = chambername.str();
                goodParams[chamberId] = goodpar;
                xarrayMap[chamberId] = xarray;
            } else {
                double** thisarray = it->second;
                for (int ish = 0; ish < NSHIFTS; ish++) {
                    if (goodpar[0]) thisarray[0][ish] += (*chi2_transx)[ish][ich];
                    thisarray[1][ish] += (*chi2_transy)[ish][ich];
                    thisarray[2][ish] += (*chi2_transz)[ish][ich];
                    thisarray[3][ish] += (*chi2_rotx)[ish][ich];
                    thisarray[4][ish] += (*chi2_roty)[ish][ich];
                    if (goodpar[5]) thisarray[5][ish] += (*chi2_rotz)[ish][ich];
                }
                numTracks[chamberId] = numTracks[chamberId] + 1;
                chamberNames[chamberId] = chambername.str();
            }
        }

        // for (int i=0;i<6;i++) {
        // h2MSIDpTVTrackQual[i]->Fill(log10(chi2VAlignParamQuality[i]),msid_ptDiff);
        //}
    }
    if (DEBUG) std::cout << "returning from getDataFromTree" << std::endl;

    TCanvas* canv = new TCanvas("canv", "canv");
    canv->Divide(3, 2);

    /*
    for (int i=0;i<6;i++) {
      canv->cd(i+1);
      h2MSIDpTVTrackQual[i]->Draw("colz");
      h2MSIDpTVTrackQual[i]->GetXaxis()->SetTitle("log(#chi^{2} of #chi^{2} v. align param)");
      h2MSIDpTVTrackQual[i]->GetYaxis()->SetTitle("|1-pT_{ID}/pT_{MS}|");
    }

    stringstream canvname;
    canvname<<outputdir<<"/"<<rootname<<"_msidPtVTrackQuality_iter"<<iteration<<".eps";
    canv->Print(canvname.str().c_str());
    */

    delete canv;
    std::cout << "returning from getDataFromTree" << std::endl;
    return;
}

void getDataFromAlignParTree(TChain* tree, std::map<long long int, double*>& firstderivMap, std::map<long long int, double*>& secndderivMap,
                             std::map<long long int, double*>& initPosMap, std::map<long long int, double*>& alignParMap,
                             std::map<long long int, double*>& alignParErrMap) {
    std::cout << "getting data from alignPar tree" << std::endl;

    long long int chamberId;
    double init_tras, init_traz, init_trat;
    double init_rots, init_rotz, init_rott;
    double err_tras, err_traz, err_trat;
    double err_rots, err_rotz, err_rott;
    double tras, traz, trat;
    double rots, rotz, rott;
    double firstderiv_tras;
    double firstderiv_traz;
    double firstderiv_trat;
    double firstderiv_rots;
    double firstderiv_rotz;
    double firstderiv_rott;
    double secndderiv_tras;
    double secndderiv_traz;
    double secndderiv_trat;
    double secndderiv_rots;
    double secndderiv_rotz;
    double secndderiv_rott;

    tree->SetBranchAddress("chamberId", &chamberId);
    tree->SetBranchAddress("init_tras", &init_tras);
    tree->SetBranchAddress("init_traz", &init_traz);
    tree->SetBranchAddress("init_trat", &init_trat);
    tree->SetBranchAddress("init_rots", &init_rots);
    tree->SetBranchAddress("init_rotz", &init_rotz);
    tree->SetBranchAddress("init_rott", &init_rott);
    tree->SetBranchAddress("err_tras", &err_tras);
    tree->SetBranchAddress("err_traz", &err_traz);
    tree->SetBranchAddress("err_trat", &err_trat);
    tree->SetBranchAddress("err_rots", &err_rots);
    tree->SetBranchAddress("err_rotz", &err_rotz);
    tree->SetBranchAddress("err_rott", &err_rott);
    tree->SetBranchAddress("delta_tras", &tras);
    tree->SetBranchAddress("delta_traz", &traz);
    tree->SetBranchAddress("delta_trat", &trat);
    tree->SetBranchAddress("delta_rots", &rots);
    tree->SetBranchAddress("delta_rotz", &rotz);
    tree->SetBranchAddress("delta_rott", &rott);
    tree->SetBranchAddress("firstderiv_tras", &firstderiv_tras);
    tree->SetBranchAddress("firstderiv_traz", &firstderiv_traz);
    tree->SetBranchAddress("firstderiv_trat", &firstderiv_trat);
    tree->SetBranchAddress("firstderiv_rots", &firstderiv_rots);
    tree->SetBranchAddress("firstderiv_rotz", &firstderiv_rotz);
    tree->SetBranchAddress("firstderiv_rott", &firstderiv_rott);
    tree->SetBranchAddress("secndderiv_tras", &secndderiv_tras);
    tree->SetBranchAddress("secndderiv_traz", &secndderiv_traz);
    tree->SetBranchAddress("secndderiv_trat", &secndderiv_trat);
    tree->SetBranchAddress("secndderiv_rots", &secndderiv_rots);
    tree->SetBranchAddress("secndderiv_rotz", &secndderiv_rotz);
    tree->SetBranchAddress("secndderiv_rott", &secndderiv_rott);

    if (DEBUG) std::cout << tree->GetEntries() << " entries in alignPar tree" << std::endl;
    double *firstderivs(0), *secndderivs(0);
    double *alignPar(0), *alignParErr(0);
    double* initPos(0);
    for (int ientry = 0; ientry < tree->GetEntries(); ientry++) {
        tree->GetEntry(ientry);

        std::map<long long int, double*>::iterator itFirst = firstderivMap.find(chamberId);

        if (itFirst == firstderivMap.end()) {
            firstderivs = new double[NPARAMS];
            secndderivs = new double[NPARAMS];
            alignPar = new double[NPARAMS];
            alignParErr = new double[NPARAMS];
            initPos = new double[NPARAMS];

            alignPar[0] = tras;
            alignPar[1] = traz;
            alignPar[2] = trat;
            alignPar[3] = rots * 1000.;
            alignPar[4] = rotz * 1000.;
            alignPar[5] = rott * 1000.;
            initPos[0] = init_tras;
            initPos[1] = init_traz;
            initPos[2] = init_trat;
            initPos[3] = (init_rots)*1000.;
            initPos[4] = (init_rotz)*1000.;
            initPos[5] = (init_rott)*1000.;
            for (int i = 0; i < 6; i++) cout << "initPos[" << i << "]=" << initPos[i] << endl;

            alignParErr[0] = sqrt(err_tras);
            alignParErr[1] = sqrt(err_traz);
            alignParErr[2] = sqrt(err_trat);
            alignParErr[3] = sqrt(err_rots) * 1000.;
            alignParErr[4] = sqrt(err_rotz) * 1000.;
            alignParErr[5] = sqrt(err_rott) * 1000.;
            firstderivs[0] = firstderiv_tras * 1.;
            firstderivs[1] = firstderiv_traz * 1.;
            firstderivs[2] = firstderiv_trat * 1.;
            firstderivs[3] = firstderiv_rots * .001;
            firstderivs[4] = firstderiv_rotz * .001;
            firstderivs[5] = firstderiv_rott * .001;
            secndderivs[0] = secndderiv_tras;
            secndderivs[1] = secndderiv_traz;
            secndderivs[2] = secndderiv_trat;
            secndderivs[3] = secndderiv_rots * .000001;
            secndderivs[4] = secndderiv_rotz * .000001;
            secndderivs[5] = secndderiv_rott * .000001;
            if (DEBUG) {
                for (int i = 0; i < 6; i++) std::cout << "firstderivs[" << i << "]=" << firstderivs[i] << std::endl;
                for (int i = 0; i < 6; i++) std::cout << "secndderivs[" << i << "]=" << secndderivs[i] << std::endl;
            }

            initPosMap[chamberId] = initPos;
            alignParMap[chamberId] = alignPar;
            alignParErrMap[chamberId] = alignParErr;
            firstderivMap[chamberId] = firstderivs;
            secndderivMap[chamberId] = secndderivs;
        } else {
            firstderivs[0] += firstderiv_tras * 1.;
            firstderivs[1] += firstderiv_traz * 1.;
            firstderivs[2] += firstderiv_trat * 1.;
            firstderivs[3] += firstderiv_rots * .001;
            firstderivs[4] += firstderiv_rotz * .001;
            firstderivs[5] += firstderiv_rott * .001;
            secndderivs[0] += secndderiv_tras;
            secndderivs[1] += secndderiv_traz;
            secndderivs[2] += secndderiv_trat;
            secndderivs[3] += secndderiv_rots * .000001;
            secndderivs[4] += secndderiv_rotz * .000001;
            secndderivs[5] += secndderiv_rott * .000001;
            if (DEBUG) {
                for (int i = 0; i < 6; i++) std::cout << "firstderivs[" << i << "]=" << firstderivs[i] << std::endl;
                for (int i = 0; i < 6; i++) std::cout << "secndderivs[" << i << "]=" << secndderivs[i] << std::endl;
            }
        }
    }
    return;
}
