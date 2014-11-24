/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUncertainties/MultijetJESUncertaintyProvider.h"
#include "JetUncertainties/UJUncertaintyProvider.h"
#include "JetUncertainties/JESUtils.h"
#include "atlasstyle/AtlasStyle.C"
#include "atlasstyle/AtlasLabels.C"


#include <stdio.h>
//#include "UJUncertaintyProvider.h"

#include <TCanvas.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TLegend.h>

#include <stdlib.h>

//#include "AtlasStyle.C"

TCanvas* c = 0;
TString inputfile = "../share/UJ_2012/JMS2012_Aug22.root";

TH1* getSlice(UJUncertaintyProvider& ujp, float eta, float pt, UJUncertaintyProvider::e_prop p) {

	float mmin = 0.0;
	float mmax = 500.0;

	int nbins = 100;

	TH1F* ret = new TH1F("ret", "", nbins, mmin, mmax);

	for (int i = 1; i <= nbins; i++) {
		float m = ret->GetBinCenter(i);
		float unc = ujp.getRelUncert(pt*1000., m*1000, eta, p);
		ret->SetBinContent(i, unc);
		printf("getSlice: eta=%f:m=%f:pt=%f:result=%f (%i)\n", eta, m, pt, unc, p);
	}

	ret->GetXaxis()->SetTitle("Jet Mass [GeV]");
	ret->GetYaxis()->SetTitle("Relative Uncertainty");
	ret->GetXaxis()->SetLabelSize(0.03);
	ret->GetYaxis()->SetLabelSize(0.03);
	ret->GetYaxis()->SetRangeUser(0.0, 0.25);

	return ret;
}

TH1* getSlicePt(UJUncertaintyProvider& ujp, float eta, float mopt, UJUncertaintyProvider::e_prop p) {

	float mmin = 0.0;
	float mmax = 1000.0;

	int nbins = 100;

	TH1F* ret = new TH1F("ret", "", nbins, mmin, mmax);

	for (int i = 1; i <= nbins; i++) {
		float pt = ret->GetBinCenter(i);
		float unc = ujp.getRelUncert(pt*1000., mopt*pt*1000, eta, p);
		ret->SetBinContent(i, unc);
		printf("getSlice: %f:%f:%f:%f\n", eta, mopt*pt, pt, unc);
	}

	ret->GetXaxis()->SetTitle("Jet Pt [GeV]");
	ret->GetYaxis()->SetTitle("Relative Uncertainty");
	ret->GetXaxis()->SetLabelSize(0.03);
	ret->GetYaxis()->SetLabelSize(0.03);
	ret->GetYaxis()->SetRangeUser(0.0, 0.25);

	return ret;
}

void printSingle(const char* jetalg, UJUncertaintyProvider::e_prop p) {

  //UJUncertaintyProvider ujp("../share/UJ_2012/UJInput2012.root", jetalg, "MC12a");
  UJUncertaintyProvider ujp(inputfile, jetalg, "MC12a");
	
	static const float meta[2] = {0.0, 1.0};
	static const float mpt[6] = {100.0, 200.0, 300.0, 500.0, 750.0, 1000.0};
	//corresponding to the center of the bin, to help direct comparison with tables
	//static const float mpt[6] = {50.0, 150.0, 350.0, 550.0, 700.0, 1000.0};


	TLegend* leg = new TLegend(0.55, 0.55, 0.9, 0.9);
	leg->SetBorderSize(0);
	leg->SetFillColor(kWhite);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++) {
			//TH1* slice = getSlice(ujp, meta[i], mpt[j], UJUncertaintyProvider::JMS);
			TH1* slice = getSlice(ujp, meta[i], mpt[j], p);
			slice->SetLineColor(j+1);
			slice->SetLineStyle(i+1);
			TH1* copy = 0;
			if (!i && !j) {
				copy = slice->DrawCopy();
			} else {
				copy = slice->DrawCopy("SAME");
			}
			//leg->AddEntry(copy, "hello");
			leg->AddEntry(slice, TString::Format("#eta = %.1f pt = %.0f GeV", meta[i], mpt[j]), "L");
		}
	}
	leg->Draw();
	static const char* propstr[6] = {"pT", "m", "tau21", "tau32", "d12", "d23"};
	c->Print(TString::Format("slices_%s_%s.pdf", jetalg, propstr[p]));	
	
}

void printSinglePt(const char* jetalg, UJUncertaintyProvider::e_prop p) {

  //UJUncertaintyProvider ujp("../share/UJ_2012/UJInput2012.root", jetalg, "MC12a");
  UJUncertaintyProvider ujp(inputfile, jetalg, "MC12a");
	
	static const float meta[2] = {0.0, 1.0};
	static const float mmopt[6] = {0.1, 0.2, 0.3, 0.4, 0.6, 0.8};


	TLegend* leg = new TLegend(0.55, 0.55, 0.9, 0.9);
	leg->SetBorderSize(0);
	leg->SetFillColor(kWhite);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++) {
			//TH1* slice = getSlice(ujp, meta[i], mpt[j], UJUncertaintyProvider::JMS);
			TH1* slice = getSlicePt(ujp, meta[i], mmopt[j], p);
			slice->SetLineColor(j+1);
			slice->SetLineStyle(i+1);
			TH1* copy = 0;
			if (!i && !j) {
				copy = slice->DrawCopy();
			} else {
				copy = slice->DrawCopy("SAME");
			}
			//leg->AddEntry(copy, "hello");
			leg->AddEntry(slice, TString::Format("#eta = %.1f mopt = %.1f GeV", meta[i], mmopt[j]), "L");
		}
	}
	leg->Draw();
	static const char* propstr[6] = {"pT", "m", "tau21", "tau32", "d12", "d23"};
	c->Print(TString::Format("slices2_%s_%s.pdf", jetalg, propstr[p]));		
}

void allAlg(const char* jetalg) {

	printSingle(jetalg, UJUncertaintyProvider::JPTS);
	printSingle(jetalg, UJUncertaintyProvider::JMS);
	printSingle(jetalg, UJUncertaintyProvider::D12);
	printSingle(jetalg, UJUncertaintyProvider::D23);
	printSingle(jetalg, UJUncertaintyProvider::TAU21);
	printSingle(jetalg, UJUncertaintyProvider::TAU32);
	/*
	printSinglePt(jetalg, UJUncertaintyProvider::JPTS);
	printSinglePt(jetalg, UJUncertaintyProvider::JMS);
	printSinglePt(jetalg, UJUncertaintyProvider::D12);
	printSinglePt(jetalg, UJUncertaintyProvider::D23);
	printSinglePt(jetalg, UJUncertaintyProvider::TAU21);
	printSinglePt(jetalg, UJUncertaintyProvider::TAU32);
	*/
}

void gammajet_shift(const char* jetalg){
  UJUncertaintyProvider ujp(inputfile, jetalg, "MC12a");

  float pt_min = 0.0;
  float pt_max = 500.0;
  int nbins = 200;
  TH1F* ret1 = new TH1F("ret1", ";p_{T}^{jet};relative shift", nbins, pt_min, pt_max);
  TH1F* ret2 = new TH1F("ret2", "", nbins, pt_min, pt_max);
  TH1F* ret1b = new TH1F("ret1b", ";p_{T}^{jet};relative shift", nbins, pt_min, pt_max);
  TH1F* ret2b = new TH1F("ret2b", "", nbins, pt_min, pt_max);
  for (int i = 1; i <= nbins; i++) {
    float pt = ret1->GetBinCenter(i);
    float shiftNPV = ujp.getRelUncert_GammaJet_ShiftNPV(pt, 11.8+1);
    float shiftMu = ujp.getRelUncert_GammaJet_ShiftMu(pt, 20.7+1);
    ret1->SetBinContent(i, shiftNPV);
    ret2->SetBinContent(i, shiftMu);
    float shiftNPVb = ujp.getRelUncert_GammaJet_ShiftNPV(pt, 11.8+2);
    float shiftMub = ujp.getRelUncert_GammaJet_ShiftMu(pt, 20.7+2);
    ret1b->SetBinContent(i, shiftNPVb);
    ret2b->SetBinContent(i, shiftMub);
  }

  TLegend* leg = new TLegend(0.55, 0.7, 0.9, 0.9);
  leg->SetBorderSize(0);
  leg->SetFillColor(kWhite);

  ret1->SetLineColor(kRed);
  ret2->SetLineColor(kGreen);
  ret1b->SetLineColor(kRed);
  ret2b->SetLineColor(kGreen);
  ret1b->SetLineStyle(3);
  ret2b->SetLineStyle(3);

  ret1->Draw();
  ret1b->Draw("same");
  ret2->Draw("same");
  ret2b->Draw("same");

  leg->AddEntry(ret1, "N_{PV} (N_{PV}=11.8+1)", "L");
  leg->AddEntry(ret1b, "N_{PV} (N_{PV}=11.8+2)", "L");
  leg->AddEntry(ret2, "<#mu> (<#mu>=20.7+1)", "L");
  leg->AddEntry(ret2b, "<#mu> (<#mu>=20.7+2)", "L");
  leg->Draw();

  c->Print(TString::Format("shift_%s.pdf", jetalg));

}

int main(int argc, char** argv) {

  if (argc>1) inputfile = argv[1];


	//UJUncertaintyProvider ujp("UJInput2012.root", "AntiKt10LCTopo", "MC12a");
	//UJUncertaintyProvider ujp("UJInput2012.root", "AntiKt10LCTopoTrimmedPtFrac5SmallR30", "MC12a");

	//printf("%f\n", ujp.getRelUncert(220000., 40000., 0.5, UJUncertaintyProvider::JMS));
	//exit(0);

	SetAtlasStyle();
	c = new TCanvas("c", "c", 600, 600);

	//allAlg("AntiKt10LCTopo");
	allAlg("AntiKt10LCTopoTrimmedPtFrac5SmallR30");
	//allAlg("CamKt12LCTopo");
	//allAlg("CamKt12LCTopoSplitFilteredmassFraction67minSplitR0");

	if (TString(inputfile).Contains("GammaJet"))
	  gammajet_shift("AntiKt10LCTopoTrimmedPtFrac5SmallR30");	  

	return 0;
}
