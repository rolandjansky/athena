#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <TChain.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TMath.h>
#include <TStyle.h>
#include <TString.h>

#include "RoIBase.h"
#include "RoI.h"

int     binDis = 400;
double  minDis =0, maxDis = 1.001;

vector <std::string> m_stringVectorOfTaggers;

std::map <std::string, TH1D*> hbTag, huTag;

bool lverb = false;

void run(std::string);
void loopOver(std::string);
void rejVsEffPlot(TH1D*, TH1D*, TGraphErrors*);
void rejVsEffFile();


//** ----------------------------------------------------------------------------------------------------------------- **//


void run(std::string sample){

  TString htit;

  for (unsigned int itagmeth=0; itagmeth<m_stringVectorOfTaggers.size(); itagmeth++) {
   
    htit = m_stringVectorOfTaggers[itagmeth];

    std::cout << "Creating histogram: " << htit << std::endl;

    TH1D* h_bb = new TH1D("bb_"+htit,"bb_"+htit,binDis,minDis,maxDis);
    TH1D* h_uu = new TH1D("uu_"+htit,"uu_"+htit,binDis,minDis,maxDis);

    hbTag[m_stringVectorOfTaggers[itagmeth]] = h_bb;
    huTag[m_stringVectorOfTaggers[itagmeth]] = h_uu;

  }

  loopOver(sample);

  gStyle->SetOptStat(0);

  TCanvas *c = new TCanvas();
  c->SetWindowSize(800,600);
  c->Resize();

  TStyle* gStyle = new TStyle();

  for (unsigned int itagmeth=0; itagmeth<m_stringVectorOfTaggers.size(); itagmeth++) {

    htit = m_stringVectorOfTaggers[itagmeth];

    gStyle->SetOptFit(0);

    hbTag[m_stringVectorOfTaggers[itagmeth]]->SetFillColor(kBlue);
    hbTag[m_stringVectorOfTaggers[itagmeth]]->SetFillStyle(3003);
    hbTag[m_stringVectorOfTaggers[itagmeth]]->Draw();
    huTag[m_stringVectorOfTaggers[itagmeth]]->SetFillColor(kGreen);
    huTag[m_stringVectorOfTaggers[itagmeth]]->SetFillStyle(3003);
    huTag[m_stringVectorOfTaggers[itagmeth]]->Draw("SAME");

    TLegend *leg = new TLegend(0.7, 0.7, 0.85, 0.85);
    leg->AddEntry(hbTag[m_stringVectorOfTaggers[itagmeth]], " bb", "f");
    leg->AddEntry(huTag[m_stringVectorOfTaggers[itagmeth]], " uu", "f");
    leg->Draw();

    c->cd();
    c->SaveAs("distr_"+htit+".png");
    c->Clear();

    TGraphErrors *gr = new TGraphErrors(1);
    rejVsEffPlot(hbTag[m_stringVectorOfTaggers[itagmeth]], huTag[m_stringVectorOfTaggers[itagmeth]], gr);
    gr->Draw("AP");
    c->SaveAs("eff_"+htit+".png");
  }

  rejVsEffFile();

  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void loopOver(std::string sample) {

  TChain *chain = new TChain("CollectionTree","");
  chain->Add(sample.c_str(), TChain::kBigNumber);
  
  std::cout << "Chain pointer: " << chain << std::endl;

  Ntuple *nt = new Ntuple(chain);

  Int_t nEntries = (Int_t)chain->GetEntries();

  int start=0, end=nEntries;

  double deltaR = 0.1;

  for(Int_t ien=start; ien<end; ien++) {

    chain->GetEntry(ien);

    vector <RoI> m_RoIVector;     

    for (int iroi=0;iroi<nt->L1Jet_nRoI;iroi++){

      if ((*nt->L1Jet_ET8x8)[iroi] < 20000) continue;
      if (fabs((*nt->L1Jet_eta)[iroi]) > 2.3) continue;

      RoI m_RoI((*nt->L1Jet_phi)[iroi],(*nt->L1Jet_eta)[iroi]);

      int iflav = m_RoI.checkQuark(nt, deltaR);
      m_RoI.Flav(iflav);

      if (iflav!=5 && iflav !=0) continue;
      
      m_RoI.SetWeights(nt);
      m_RoIVector.push_back(m_RoI);      

    }

    if (lverb) std::cout <<" number of RoIs = " << m_RoIVector.size() << std::endl;

    for (unsigned int iroi=0; iroi<m_RoIVector.size(); iroi++) {
      
      if (lverb) {
	std::cout << "RoI" << std::endl;
	std::cout << "RoI phi : " << m_RoIVector[iroi].Phi() << std::endl;
	std::cout << "RoI eta : " << m_RoIVector[iroi].Eta() << std::endl;
	std::cout << "RoI Et  : " << m_RoIVector[iroi].Et() << std::endl;
      }

      double xtag = -1;      	  	  

      for (unsigned int itagmeth=0; itagmeth<m_stringVectorOfTaggers.size(); itagmeth++) {

	if (lverb)
	  std::cout << "Looking for tagging method " << m_stringVectorOfTaggers[itagmeth] << std::endl;

	if (m_stringVectorOfTaggers[itagmeth]=="L2_IP1D") xtag = m_RoIVector[iroi].l2Tag.xIP1D();
	if (m_stringVectorOfTaggers[itagmeth]=="L2_IP2D") xtag = m_RoIVector[iroi].l2Tag.xIP2D();
	if (m_stringVectorOfTaggers[itagmeth]=="L2_IP3D") xtag = m_RoIVector[iroi].l2Tag.xIP3D();
	if (m_stringVectorOfTaggers[itagmeth]=="L2_CHI2") xtag = m_RoIVector[iroi].l2Tag.xCHI2();
	if (m_stringVectorOfTaggers[itagmeth]=="L2_SV")   xtag = m_RoIVector[iroi].l2Tag.xSVtx();
	if (m_stringVectorOfTaggers[itagmeth]=="L2_MVTX") xtag = m_RoIVector[iroi].l2Tag.xMVtx();
	if (m_stringVectorOfTaggers[itagmeth]=="L2_EVTX") xtag = m_RoIVector[iroi].l2Tag.xEVtx();
	if (m_stringVectorOfTaggers[itagmeth]=="L2_NVTX") xtag = m_RoIVector[iroi].l2Tag.xNVtx();

	if (m_stringVectorOfTaggers[itagmeth]=="EF_IP1D") xtag = m_RoIVector[iroi].efTag.xIP1D();
	if (m_stringVectorOfTaggers[itagmeth]=="EF_IP2D") xtag = m_RoIVector[iroi].efTag.xIP2D();
	if (m_stringVectorOfTaggers[itagmeth]=="EF_IP3D") xtag = m_RoIVector[iroi].efTag.xIP3D();
	if (m_stringVectorOfTaggers[itagmeth]=="EF_CHI2") xtag = m_RoIVector[iroi].efTag.xCHI2();
	if (m_stringVectorOfTaggers[itagmeth]=="EF_SV")   xtag = m_RoIVector[iroi].efTag.xSVtx();
	if (m_stringVectorOfTaggers[itagmeth]=="EF_MVTX") xtag = m_RoIVector[iroi].efTag.xMVtx();
	if (m_stringVectorOfTaggers[itagmeth]=="EF_EVTX") xtag = m_RoIVector[iroi].efTag.xEVtx();
	if (m_stringVectorOfTaggers[itagmeth]=="EF_NVTX") xtag = m_RoIVector[iroi].efTag.xNVtx();

	if (xtag != -1) {
	  if (m_RoIVector[iroi].Flav() == 5) {

	    if (lverb)
	      std::cout << "Filling bb histogram " << m_stringVectorOfTaggers[itagmeth] << std::endl;

	    hbTag[m_stringVectorOfTaggers[itagmeth]]->Fill(xtag);

	  } else if (m_RoIVector[iroi].Flav() == 0) {

	    if (lverb)
	      std::cout << "Filling uu histogram " << m_stringVectorOfTaggers[itagmeth] << std::endl;

	    huTag[m_stringVectorOfTaggers[itagmeth]]->Fill(xtag);

	  }
	}
      }
    }
  }

  chain->Delete();

  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void rejVsEffPlot(TH1D* hb, TH1D* hu, TGraphErrors *gr) {

  double totb = hb->GetEntries();
  double totu = hu->GetEntries();
  
  double sumu=totu,sumb=totb;
  double effu,effb,seffu,seffb;
  int ind=0;

  for (int i=-1;i<hb->GetNbinsX()-1;i++){
    if (i>=0){
      sumb -= hb->GetBinContent(i+1);
      sumu -= hu->GetBinContent(i+1);
    }
    effb  = sumb/totb;
    effu  = sumu/totu;
    seffb = sqrt((effb)*(1-effb)/totb);
    seffu = sqrt((effu)*(1-effu)/totu);

    if (effu!=0 && effb!=0){ 
      seffu = seffu/pow(effu,2);
      gr->SetPoint(ind,effb,1./effu);
      gr->SetPointError(ind,seffb,seffu);
      ind++;
    }
  }

  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void rejVsEffFile() {

  ofstream m_file;
  m_file.open("summary.txt");
  m_file <<"----------------Summary-------------\n\n\n\n";

  for (unsigned int itagmeth=0; itagmeth<m_stringVectorOfTaggers.size(); itagmeth++) {

    m_file <<" ==>> tagger " << m_stringVectorOfTaggers[itagmeth] << "\n\n\n";

    double totb = hbTag[m_stringVectorOfTaggers[itagmeth]]->GetEntries();
    double totu = huTag[m_stringVectorOfTaggers[itagmeth]]->GetEntries();
  
    double sumu=totu,sumb=totb;
    double effu,effb,seffu,seffb;
    double reju, sreju;

    int j = 0;

    for (int i=-1; i<hbTag[m_stringVectorOfTaggers[itagmeth]]->GetNbinsX()-1; i++, j++) {

      if (i>=0) {
	sumb -= hbTag[m_stringVectorOfTaggers[itagmeth]]->GetBinContent(i+1);
	sumu -= huTag[m_stringVectorOfTaggers[itagmeth]]->GetBinContent(i+1);
      }

      effb  = std::max(1e-10, sumb/totb);
      effu  = std::max(1e-10, sumu/totu);
      seffb = sqrt((effb)*(1-effb)/totb);
      seffu = sqrt((effu)*(1-effu)/totu);

      reju  = 1/effu;
      sreju = seffu/sqrt(effu);

      if (j == binDis/10) {

	j = 0;

	m_file << "eff(b) " << effb << " +- " << seffb << " with cut at X > " 
	       << hbTag[m_stringVectorOfTaggers[itagmeth]]->GetBinCenter(i+1)+hbTag[m_stringVectorOfTaggers[itagmeth]]->GetBinWidth(i+1)/2 
	       << std::endl;
	m_file << "eff(u) " << effu << " +- " << seffu << " with cut at X > " 
	       << hbTag[m_stringVectorOfTaggers[itagmeth]]->GetBinCenter(i+1)+hbTag[m_stringVectorOfTaggers[itagmeth]]->GetBinWidth(i+1)/2 
	       << std::endl;
	m_file << "rej(u) " << reju << " +- " << sreju << " with cut at X > " 
	       << hbTag[m_stringVectorOfTaggers[itagmeth]]->GetBinCenter(i+1)+hbTag[m_stringVectorOfTaggers[itagmeth]]->GetBinWidth(i+1)/2 
	       << std::endl;
	m_file << "\n";

      }
    }

    m_file << "\n\n";
  }

  m_file.close();

  return;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


int main(int argc, char *argv[]){

  const char* dataFile = "ntuple.BjetSlice.root";
  
  //std::string dataFile((const char *)argv[1],strlen(argv[1]));

  if (dataFile == "") {

    std::cout << "Input file is missing" << std::endl;
    return 1;

  } else
    std::cout << "Data file name: " << dataFile << std::endl;

  m_stringVectorOfTaggers.push_back("L2_IP1D");
  m_stringVectorOfTaggers.push_back("L2_IP2D");
  m_stringVectorOfTaggers.push_back("L2_IP3D");
  m_stringVectorOfTaggers.push_back("L2_CHI2");
  m_stringVectorOfTaggers.push_back("EF_IP1D");
  m_stringVectorOfTaggers.push_back("EF_IP2D");
  m_stringVectorOfTaggers.push_back("EF_IP3D");
  m_stringVectorOfTaggers.push_back("EF_CHI2");

  std::cout << "Running with " << m_stringVectorOfTaggers.size() << " taggers" << std::endl;

  for (unsigned int itagmeth=0; itagmeth<m_stringVectorOfTaggers.size(); itagmeth++)
    std::cout << "    =>  " << m_stringVectorOfTaggers[itagmeth] << std::endl;

  run(dataFile);
} 



