//////////////////////////////////////////////////////////////
// Moore Validation/Performance Plots
//
// MooPerformanceMacros.C
//
// Nektarios Ch. Benekos     Thomas Moore, Stephane Willocq 
// CERN/ATC                  Univ of Massachusetts, Amherst
//
// Feb, 2005
//////////////////////////////////////////////////////////////

#include "TPad.h"
#include "TFile.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TText.h"
#include "TF1.h"
#include "TCanvas.h"
#include <cstring>  //for strcmp
//#include "TObject.h"

void allpTPage( const char* histo, const char* reco="", const char* legendPos="" ) {

  TSeqCollection* canvasList = gROOT->GetListOfCanvases();
  TCanvas* c1 = (TCanvas*)canvasList->First();
  c1->Clear();
  c1->Divide(2,2);
  //  c1->Divide(2,3);

 
  //TIter next(gROOT->GetListOfFiles());
  //TFile* f;
  char hTitle[256];

  Int_t iFile(0);  //while ((f = (TFile*)next())) {

  TDirectory* currentDir = gDirectory;
  TIter objIter(gDirectory->GetList());
  TObject* obj;
  while (obj = objIter() ) {

    if (obj->InheritsFrom("TDirectory")) {

      //printf("Looping over directory %s\n",((TDirectory*)obj)->GetTitle());

      ((TDirectory*)obj)->cd();

      //f->cd();
      //sprintf(hTitle,"p_{T} = %s GeV/c",((TDirectory*)obj)->GetTitle());
      sprintf(hTitle,"%s",((TDirectory*)obj)->GetTitle());

      c1->cd(iFile+1);

      if ( strcmp(reco,"all")==0 || strcmp(reco,"All")==0 ) {

        char nameMo[256];
        char nameId[256];
        char nameMu[256];
        char nameCb[256];

        sprintf(nameMo,"%sMo",histo);
        sprintf(nameId,"%sId",histo);
        sprintf(nameMu,"%sMu",histo);
        sprintf(nameCb,"%sCb",histo);

        TH1F* histMo = (TH1F*)gDirectory->Get(nameMo);
        if ( !histMo ){
          printf("allpTPage can't find histogram %s/n",nameMo);
          return;
        }
        TH1F* histId = (TH1F*)gDirectory->Get(nameId);
        if ( !histId ) {
          printf("allpTPage can't find histogram %s/n",nameId);
          return;
        }
        TH1F* histMu = (TH1F*)gDirectory->Get(nameMu);
        if ( !histMu ) {
          printf("allpTPage can't find histogram %s/n",nameMu);
          return;
        }
        TH1F* histCb = (TH1F*)gDirectory->Get(nameCb);
        if ( !histCb ) {
          printf("allpTPage can't find histogram %s/n",nameCb);
          return;
        }

        histMo->SetTitle(hTitle);
        histId->SetTitle(hTitle);
        histMu->SetTitle(hTitle);
        histCb->SetTitle(hTitle);

//         setStyle22(histMo);
//         setStyle22(histId);
//         setStyle22(histMu);
//         setStyle22(histCb);
        setStyle23(histMo);
        setStyle23(histId);
        setStyle23(histMu);
        setStyle23(histCb);

        setScheme(histMo,"Mo");
        setScheme(histId,"Id");
        setScheme(histMu,"Mu");
        setScheme(histCb,"Cb");
 
        histId->Draw();
        histMu->Draw("same");
        histCb->Draw("same");
        histMo->Draw("same");

        if ( strcmp(legendPos,"bl")==0 || strcmp(legendPos,"BL")==0 ) {
	  TLegend* leg = new TLegend(0.16,0.22,0.37,0.47);
	  leg->AddEntry(histMo,"Moore","p");
	  leg->AddEntry(histId,"Inner Det","p");
	  leg->AddEntry(histMu,"MuId Standalone","p");
	  leg->AddEntry(histCb,"MuId Combined","p");
	  leg->SetFillStyle(0);
	  leg->Draw();
        } else if ( strcmp(legendPos,"tl")==0 || strcmp(legendPos,"TL")==0 ) {
	  TLegend* leg = new TLegend(0.16,0.58,0.37,0.83);
	  leg->AddEntry(histMo,"Moore","p");
	  leg->AddEntry(histId,"Inner Det","p");
	  leg->AddEntry(histMu,"MuId Standalone","p");
	  leg->AddEntry(histCb,"MuId Combined","p");
	  leg->SetFillStyle(0);
	  leg->Draw();
        } 

      } else if ( strcmp(reco,"")==0 || strcmp(reco,"Mo")==0 || 
        strcmp(reco,"Id")==0 || strcmp(reco,"Mu")==0 || strcmp(reco,"Cb")==0 ) {

        char hName[256];
        sprintf(hName,"%s%s",histo,reco);
	//	printf("In macro allpTPage - histo hName %s %s\n",histo, hName);

        TH1F* h1D = (TH1F*)gDirectory->FindObject(hName);
        h1D->SetTitle(hTitle);
        setScheme(h1D,reco);
	//        setStyle22(h1D);
        setStyle23(h1D);
        h1D->Draw();
      }  
      iFile++;
    } 
  }
  c1->Update();
  currentDir->cd();
}

///////////////////////////////////////////////////////////////////////////////////////

void allpTPage2D( const char* histo, const char* reco="", const char* title="" ) {


  char pTSpec[256];

  TSeqCollection* canvasList = gROOT->GetListOfCanvases();
  TCanvas* c1 = (TCanvas*)canvasList->First();
  c1->Clear();
  c1->Divide(2,2);
  //  c1->Divide(2,3);

  //TIter next(gROOT->GetListOfFiles());
  //TFile* f;

  Int_t iFile(0);
  //while ((f = (TFile*)next())) {

  TDirectory* currentDir = gDirectory;
  TIter objIter(gDirectory->GetList());
  TObject* obj;
  while (obj = objIter() ) {

    if (obj->InheritsFrom("TDirectory")) {

      ((TDirectory*)obj)->cd();
      //f->cd();
      //sprintf(pTSpec,"pT = %s GeV/c",((TDirectory*)obj)->GetTitle());
      sprintf(pTSpec,"%s",((TDirectory*)obj)->GetTitle());

      //      TText* pTText = new TText(0.18,0.7,pTSpec);
      TText* pTText = new TText(0.20,0.9,pTSpec);
      pTText->SetNDC();
      pTText->SetTextSize(0.07);

      c1->cd(iFile+1);
      gPad->SetRightMargin(0.15);

      char hName[256];
      sprintf(hName,"%s%s",histo,reco);
      //      printf("In macro allpTPage2D - histo hName %s %s\n",histo, hName);
        
      TH2F* h2D = (TH2F*)gDirectory->FindObject(hName);
      //      setStyle22(h2D);
      setStyle23(h2D);
      h2D->Draw("COLZ");
      pTText->Draw();
 
      iFile++;
    }
  } 
  c1->Update();
  currentDir->cd();
}

////////////////////////////////////////////////////////////////////////////

void compRecoPage(const char* var )
{

  TSeqCollection* canvasList = gROOT->GetListOfCanvases();
  TCanvas* c1 = (TCanvas*)canvasList->First();

  char nameMo[256];
  char nameId[256];
  char nameMu[256];
  char nameCb[256];

  sprintf(nameMo,"%sMo",var);
  sprintf(nameId,"%sId",var);
  sprintf(nameMu,"%sMu",var);
  sprintf(nameCb,"%sCb",var);

  TH1F* histMo = (TH1F*)gDirectory->Get(nameMo);
  TH1F* histId = (TH1F*)gDirectory->Get(nameId);
  TH1F* histMu = (TH1F*)gDirectory->Get(nameMu);
  TH1F* histCb = (TH1F*)gDirectory->Get(nameCb);

  setStyle22(histMo);
  setStyle22(histId);
  setStyle22(histMu);
  setStyle22(histCb);

  setScheme(histMo,"Mo");
  setScheme(histId,"Id");
  setScheme(histMu,"Mu");
  setScheme(histCb,"Cb");

  c1->Clear();
  c1->Divide(2,2);

  TText* textMo = new TText(0.18,0.7,"Moore");
  textMo->SetNDC();
  textMo->SetTextSize(0.07);

  TText* textId = new TText(0.18,0.7,"Inner Det");
  textId->SetNDC();
  textId->SetTextSize(0.07);

  TText* textMu = new TText(0.18,0.7,"MuID Standalone");
  textMu->SetNDC();
  textMu->SetTextSize(0.07);

  TText* textCb = new TText(0.18,0.7,"MuID Combined");
  textCb->SetNDC();
  textCb->SetTextSize(0.07);

  c1->cd(1);
  histMo->Draw();
  textMo->Draw();
  c1->cd(2);
  histId->Draw();
  textId->Draw();
  c1->cd(3);
  histMu->Draw();
  textMu->Draw();
  c1->cd(4);
  histCb->Draw();
  textCb->Draw();

  c1->Update();

}

////////////////////////////////////////////////////////////////////////////

void compRecoPage2D(const char* var)
{

  TSeqCollection* canvasList = gROOT->GetListOfCanvases();
  TCanvas* c1 = (TCanvas*)canvasList->First();

  char nameMo[256];
  char nameId[256];
  char nameMu[256];
  char nameCb[256];

  sprintf(nameMo,"%sMo",var);
  sprintf(nameId,"%sId",var);
  sprintf(nameMu,"%sMu",var);
  sprintf(nameCb,"%sCb",var);

  TH2F* histMo = (TH2F*)gDirectory->Get(nameMo);
  TH2F* histId = (TH2F*)gDirectory->Get(nameId);
  TH2F* histMu = (TH2F*)gDirectory->Get(nameMu);
  TH2F* histCb = (TH2F*)gDirectory->Get(nameCb);

  c1->Clear();
  c1->Divide(2,2);

  setStyle22(histMo);
  setStyle22(histId);
  setStyle22(histMu);
  setStyle22(histCb);

  TText* textMo = new TText(0.18,0.7,"Moore");
  textMo->SetNDC();
  textMo->SetTextSize(0.07);

  TText* textId = new TText(0.18,0.7,"Inner Det");
  textId->SetNDC();
  textId->SetTextSize(0.07);

  TText* textMu = new TText(0.18,0.7,"MuID Standalone");
  textMu->SetNDC();
  textMu->SetTextSize(0.07);

  TText* textCb = new TText(0.18,0.7,"MuID Combined");
  textCb->SetNDC();
  textCb->SetTextSize(0.07);

  c1->cd(1);
  gPad->SetRightMargin(0.15);
  histMo->Draw("COLZ");
  textMo->Draw();
  c1->cd(2);
  gPad->SetRightMargin(0.15);
  histId->Draw("COLZ");
  textId->Draw();
  c1->cd(3);
  gPad->SetRightMargin(0.15);
  histMu->Draw("COLZ");
  textMu->Draw();
  c1->cd(4);
  gPad->SetRightMargin(0.15);
  histCb->Draw("COLZ");
  textCb->Draw();

  c1->Update();

}

////////////////////////////////////////////////////////////////////////////

void setStyle23(TH1* theHist) {

    theHist->GetXaxis()->CenterTitle();
    theHist->GetYaxis()->CenterTitle();
    theHist->SetLabelSize(0.08,"X");
    theHist->SetLabelSize(0.08,"Y");
    theHist->SetTitleSize(0.10,"X");
    theHist->SetTitleSize(0.10,"Y");
    theHist->SetTitleOffset(0.8,"X"); //1.2
    theHist->SetTitleOffset(0.6,"Y"); //1.2
}

////////////////////////////////////////////////////////////////////////////

void setStyle22(TH1* theHist) {

    theHist->GetXaxis()->CenterTitle();
    theHist->GetYaxis()->CenterTitle();
    theHist->SetLabelSize(0.06,"X");
    theHist->SetLabelSize(0.06,"Y");
    theHist->SetTitleSize(0.07,"X");
    theHist->SetTitleSize(0.07,"Y");
    theHist->SetTitleOffset(1.2,"X"); //1.2
    theHist->SetTitleOffset(1.0,"Y"); //1.2
}

////////////////////////////////////////////////////////////////////////////

void setScheme(TH1* theHist, const char* scheme) {

  // Set color/marker style for this recon:
  // Moore/InnerDet/Muid Standalone/Combined

  Int_t hColor;
  Int_t hMarker;

  if ( strcmp(scheme,"Mo")==0 ) {
    hColor = 4;
    hMarker = 4;
  } else if ( strcmp(scheme,"Id")==0 ) {
    hColor = 6;
    hMarker = 23;
  } else if ( strcmp(scheme,"Mu")==0 ) {
    hColor = 2;
    hMarker = 21;
  } else if ( strcmp(scheme,"Cb")==0 ) {
    hColor = 1;
    hMarker = 22;
  } else {
    hColor = 1;
    hMarker = 1;
  }
  theHist->SetLineColor(hColor);
  theHist->SetMarkerStyle(hMarker);
  theHist->SetMarkerColor(hColor);

}

/////////////////////////////////////////////////

void makepTPage() {
 
  gROOT->cd();
  gROOT->ls();

  TSeqCollection* canvasList = gROOT->GetListOfCanvases();
  TCanvas* c1 = (TCanvas*)canvasList->First();

  TGraphErrors* graphMo;
  TGraphErrors* graphMu;
  TGraphErrors* graphId;
  TGraphErrors* graphCb;

  c1->Clear();
  c1->Divide(2,2);

  c1->cd(1);
  gPad->SetLogx(1);
  graphMo = (TGraphErrors*)gDirectory->Get("pTResVspTMo");
  graphMo->Draw("AP");
  graphMu = (TGraphErrors*)gDirectory->Get("pTResVspTMu");
  graphMu->Draw("P");
  graphId = (TGraphErrors*)gDirectory->Get("pTResVspTId");
  graphId->Draw("P");
  graphCb = (TGraphErrors*)gDirectory->Get("pTResVspTCb");
  graphCb->Draw("P");

  c1->cd(2);
  gPad->SetLogx(1);
  graphMo = (TGraphErrors*)gDirectory->Get("effVsPhi1BinVspTMo");
  graphMo->Draw("AP");
  graphMu = (TGraphErrors*)gDirectory->Get("effVsPhi1BinVspTMu");
  graphMu->Draw("P");
  graphId = (TGraphErrors*)gDirectory->Get("effVsPhi1BinVspTId");
  graphId->Draw("P");
  graphCb = (TGraphErrors*)gDirectory->Get("effVsPhi1BinVspTCb");
  graphCb->Draw("P");

  c1->Update();

}

//////////////////////////////////////////////////////////////////////////////

void makepTGraphs(const char* histo, const char* yAxis) {  

  printf("makepTGraphs called for %s\n",histo);

  TSeqCollection* fList = gROOT->GetListOfFiles();
  Int_t nFiles = fList->GetSize();
  printf("makepTGraphs found %i TFiles\n",nFiles);
  const Int_t npTVals = nFiles;
  const Int_t nRec = 4;

  Float_t pTVals[npTVals];
  Float_t val[nRec][npTVals];
  Float_t sigY[nRec][npTVals];
  Float_t sigX[nRec][npTVals];

  TIter next(gROOT->GetListOfFiles());
  TFile* f;
  Int_t iFile(0);
  while ((f = (TFile*)next())) {

    printf("makepTGraphs looping over file %s\n",f->GetName());
    Float_t thepT(0.);
    sscanf(f->GetTitle(),"%f",&thepT);
    pTVals[iFile] = thepT;

    for ( Int_t iRec = 0; iRec < nRec; iRec++ ) {
      val[iRec][iFile] =  0.0;
      sigY[iRec][iFile] = 0.0;
      sigX[iRec][iFile] = 0.0;
    }
    iFile++;
  }

  char* recExt[nRec];
  recExt[0] = "Mo";
  recExt[1] = "Id";
  recExt[2] = "Mu";
  recExt[3] = "Cb";

  TGraphErrors* gr[nRec];
  Float_t theMax(-9999999.0);

  for ( Int_t iRec = 0; iRec < nRec; iRec++ ) {

    char hName[256];
    sprintf(hName,"%s%s",histo,recExt[iRec]);
    char grName[256];
    sprintf(grName,"%sVspT%s",histo,recExt[iRec]);

    next.Reset();
    iFile = 0;
    while ((f = (TFile*)next())) {
      
      f->cd();
      TH1F* aHist = (TH1F*)gDirectory->Get(hName);
      if ( aHist ) {
        if ( TString(histo) == TString("effVsPhi1Bin") ) {
	  val[iRec][iFile] = aHist->GetBinContent(1);
	  sigY[iRec][iFile] = aHist->GetBinError(1);
          printf("eff = %f +- %f\n",val[iRec][iFile],sigY[iRec][iFile]);
        } else {
	  TF1* aFit = aHist->GetFunction("gaus");
          if ( aFit ) {
	    val[iRec][iFile] = aFit->GetParameter(2);
	    sigY[iRec][iFile] = aFit->GetParError(2);
	    printf("width = %f +- %f\n",val[iRec][iFile],sigY[iRec][iFile]);
	  } else {
            gDirectory->pwd();
            printf("gauss fit failed for %s\n",hName);
	  }
        } 
      } else {
        printf("histogram %s could not be found!\n",hName);
      }
      iFile++;
    }
    gROOT->cd(); //make TGraphs in top directory
    gr[iRec] = new TGraphErrors(npTVals,pTVals,val[iRec],sigX[iRec],sigY[iRec]);
    gr[iRec]->SetTitle("");
    gr[iRec]->SetName(grName);
 
    for ( Int_t k = 0; k < npTVals; k++ ) {
      if ( val[iRec][k] > theMax ) theMax = val[iRec][k];
    }
  }

  gr[0]->SetMarkerStyle(4);
  gr[0]->SetMarkerColor(4);
  gr[0]->SetMaximum(1.25*theMax);
  gr[0]->SetMinimum(0.0);
  gr[0]->GetXaxis()->SetTitle("Muon p_{T} (GeV/c)");
  gr[0]->GetYaxis()->SetTitle(yAxis);
  gr[0]->GetXaxis()->SetLabelSize(0.06);
  gr[0]->GetYaxis()->SetLabelSize(0.06);
  gr[0]->GetXaxis()->SetTitleSize(0.07);
  gr[0]->GetYaxis()->SetTitleSize(0.07);
  gr[0]->GetXaxis()->CenterTitle();
  gr[0]->GetYaxis()->CenterTitle();;
  gr[0]->GetXaxis()->SetTitleOffset(1.2);
  gr[0]->GetXaxis()->SetLimits(8.,350.);

  gr[1]->SetMarkerStyle(23);
  gr[1]->SetMarkerColor(6);

  gr[2]->SetMarkerStyle(21);
  gr[2]->SetMarkerColor(2);

  gr[3]->SetMarkerStyle(22);
  gr[3]->SetMarkerColor(1);

  // TGraphs have to be manually added to the current directory!
  gDirectory->GetList()->Add(gr[0]);
  gDirectory->GetList()->Add(gr[1]);
  gDirectory->GetList()->Add(gr[2]);
  gDirectory->GetList()->Add(gr[3]);
  gDirectory->ls();

}

/////////////////////////////////////////////////////////////////

void makeSummaryPages() {

  printf("In makeSummaryPages...\n");

  TSeqCollection* canvasList = gROOT->GetListOfCanvases();
  TCanvas* c1 = (TCanvas*)canvasList->First();

  char nameMo[256];
  char nameId[256];
  char nameMu[256];
  char nameCb[256];

  //TIter next(gROOT->GetListOfFiles());
  //TFile* f;
  Int_t iFile(0);
  //while ((f = (TFile*)next())) {
  TDirectory* currentDir = gDirectory;
  TIter objIter(gDirectory->GetList());
  TObject* obj;
  while (obj = objIter() ) {

    if (obj->InheritsFrom("TDirectory")) {

      ((TDirectory*)obj)->cd();
      c1->Clear();
      c1->Divide(2,2);

      c1->cd(1);
      //f->cd();

      sprintf(nameMo,"%sMo","effVsEta");
      sprintf(nameId,"%sId","effVsEta");
      sprintf(nameMu,"%sMu","effVsEta");
      sprintf(nameCb,"%sCb","effVsEta");

      TH1F* histMo = (TH1F*)gDirectory->Get(nameMo);
      if ( !histMo ) {
        printf("makeSummaryPages can't find histogram %s/n",nameMo);
        return;  
      }
      TH1F* histId = (TH1F*)gDirectory->Get(nameId);
      if ( !histId ) {
        printf("makeSummaryPages can't find histogram %s/n",nameId);
        return;
      }
      TH1F* histMu = (TH1F*)gDirectory->Get(nameMu);
      if ( !histMu ) {
        printf("makeSummaryPages can't find histogram %s/n",nameMu);
        return;
      }
      TH1F* histCb = (TH1F*)gDirectory->Get(nameCb);
      if ( !histCb ) {
        printf("makeSummaryPages can't find histogram %s/n",nameCb);
        return;
      }

      setStyle22(histMo);
      setStyle22(histId);
      setStyle22(histMu);
      setStyle22(histCb);
 
      histId->Draw();
      histMu->Draw("same");
      histCb->Draw("same");
      histMo->Draw("same");

      TLegend* leg = new TLegend(0.16,0.22,0.37,0.47);
      leg->AddEntry(histMo,"Moore","p");
      leg->AddEntry(histId,"Inner Det","p");
      leg->AddEntry(histMu,"MuId Standalone","p");
      leg->AddEntry(histCb,"MuId Combined","p");
      leg->SetFillStyle(0);
      leg->Draw();
 
      c1->cd(2);
      gROOT->cd();

      TGraphErrors* graphMo;
      TGraphErrors* graphMu;
      TGraphErrors* graphId;
      TGraphErrors* graphCb;

//       gPad->SetLogx(1);
//       graphMo = (TGraphErrors*)gDirectory->Get("effVsPhi1BinVspTMo");
//       graphMo->Draw("AP");
//       graphMu = (TGraphErrors*)gDirectory->Get("effVsPhi1BinVspTMu");
//       graphMu->Draw("P");
//       graphId = (TGraphErrors*)gDirectory->Get("effVsPhi1BinVspTId");
//       graphId->Draw("P");
//       graphCb = (TGraphErrors*)gDirectory->Get("effVsPhi1BinVspTCb");
//      graphCb->Draw("P");

      c1->cd(3);
      //f->cd();
      ((TDirectory*)obj)->cd();
      sprintf(nameMo,"%sMo","pTResWidthVsEta");
      sprintf(nameId,"%sId","pTResWidthVsEta");
      sprintf(nameMu,"%sMu","pTResWidthVsEta");
      sprintf(nameCb,"%sCb","pTResWidthVsEta");

      TH1F* histMo2 = (TH1F*)gDirectory->Get(nameMo);
      if ( !histMo2 ) {
        printf("makeSummaryPages can't find histogram %s\n",nameMo);
        return;
        }
      TH1F* histId2 = (TH1F*)gDirectory->Get(nameId);
      if ( !histId2 ) {
        printf("makeSummaryPages can't find histogram %s\n",nameId);
        return;
      }
      TH1F* histMu2 = (TH1F*)gDirectory->Get(nameMu);
      if ( !histMu2 ) {
        printf("makeSummaryPages can't find histogram %s\n",nameMu);
        return;
      }
      TH1F* histCb2 = (TH1F*)gDirectory->Get(nameCb);
      if ( !histCb2 ) {
        printf("makeSummaryPages can't find histogram %s\n",nameCb);
        return;
      }

      setStyle22(histMo2);
      setStyle22(histId2);
      setStyle22(histMu2);
      setStyle22(histCb2);
 
      histId2->Draw();
      histMu2->Draw("same");
      histCb2->Draw("same");
      histMo2->Draw("same");

      c1->cd(4);
      gROOT->cd();

//       gPad->SetLogx(1);
//       graphMo = (TGraphErrors*)gDirectory->Get("pTResVspTMo");
//       graphMo->Draw("AP");
//       graphMu = (TGraphErrors*)gDirectory->Get("pTResVspTMu");
//       graphMu->Draw("P");
//       graphId = (TGraphErrors*)gDirectory->Get("pTResVspTId");
//       graphId->Draw("P");
//       graphCb = (TGraphErrors*)gDirectory->Get("pTResVspTCb");
//      graphCb->Draw("P");

      c1->Update();

      char filename[256];
      Float_t thepT(0.);
      sscanf(((TDirectory*)obj)->GetTitle(),"%f",&thepT);
      sprintf(filename,"summary_%iGeV.gif", (Int_t)thepT);
      c1->Print(filename);

      iFile++;
    }
  }
  currentDir->cd();
}
