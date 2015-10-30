/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TRootCompare.cxx
 * @brief  TRootCompare implementation
 * @author Frank Winklmeier
 *
 * $Id: TRootCompare.cxx,v 1.3 2008-11-06 12:05:07 fwinkl Exp $
 */

#include "TrigValTools/TRootCompare.h"

#include "TClass.h"
#include "TKey.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TVirtualPad.h"
#include "TPaveStats.h"
#include "TError.h"
#include "TText.h"
#include "TCollection.h"
#include "THashList.h"
#include "TAxis.h"

#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;

TRootCompare::TRootCompare() :
  _refFile(0),
  _outFile(0),
  _refRootDir(""),
  _psFile(""),
  _can(0),
  _alg(BIN),
  _threshold(1e-6),
  _histMatch(0),
  _histTotal(0),
  _histMissing(0),
  _drawNormalized(kFALSE),
  _drawDiff(kTRUE)
{
}

TRootCompare::~TRootCompare()
{
  delete _refFile;
}

void TRootCompare::beginJob()
{
  _histMatch = 0;
  _histTotal = 0;
  _histMissing = 0;
  
  Int_t canvasWidth = 1000;
  Int_t canvasHeight = 580;

  if (!_drawDiff) canvasWidth = canvasHeight;
  
  _can = new TCanvas("can","can",canvasWidth,canvasHeight);
  if (_psFile!="") printCanvas(_psFile+"[");  
}

void TRootCompare::endJob()
{  
  cout << endl;
  cout << "Summary:" << endl;
  cout << "========" << endl;
  cout << "Total histograms compared: " << _histTotal << endl;
  cout << "Missing references: " << _histMissing << endl;
  cout << "Matching histograms: " << _histMatch << endl;
  cout << "Different histograms: " << _histTotal-_histMatch << endl;
  cout << "Ignored histograms: " << _skippedObjects.size() << endl;
  
  if (_histMatch!=_histTotal) {
    cout << "List of different histograms:" << endl;
    vector<string>::iterator iter;
    int i = 0;
    for (iter=_noMatch.begin(); iter!=_noMatch.end(); iter++) {
      i++;
      cout << setw(2) << i << ") " << *iter << endl;
    }
  }

  if (verbose() && !_skippedObjects.empty()) {
    cout << endl << "List of ignored histograms:" << endl;
    vector<string>::iterator iter;
    int i = 0;
    for (iter=_skippedObjects.begin(); iter!=_skippedObjects.end(); iter++) {
      i++;
      cout << setw(2) << i << ") " << *iter << endl;
    }
  }
  cout << endl;

   if (_outFile) {
    cout << "Comparison results written to " << _outFile->GetName() << endl;
    delete _outFile;
  }  

  if (_psFile!="") {
    printCanvas(_psFile+"]");
    cout << "Comparison results written to " << _psFile << endl;
  }

  delete _can;
}

void TRootCompare::processKey(TDirectory& dir, TKey& key)
{
  dir.cd();
    
  if (_refFile==0) return;

  if (TString(key.GetName()).Contains("/")) {
    cout << "IGNORE: " << key.GetName() << " contains '/'" << endl;
    return;    
  }

  TObject* obj = key.ReadObj();
  if (obj->IsA()->InheritsFrom("TH1")) {
    
    // Extract directory name
    TString dirName(dir.GetPath());
    dirName.Replace(0,dirName.First(":")+2,0);
    if (rootDir()!="") dirName.ReplaceAll(rootDir(),_refRootDir);
    else dirName = _refRootDir+"/"+dirName;
    TString keyPath(dirName+"/"+key.GetName());

    if (_refFile->cd(dirName)) {

      if (TObject* refObj = _refFile->Get(keyPath)) {
        //  Check if class types agree
        if (obj->Class()!=refObj->Class()) {
          cout << key.GetName()
               << " is of different type in file and reference file." << endl;
          return;
        }

        TH1& h = *((TH1*)obj);
        TH1& href = *((TH1*)refObj);
        Bool_t match = compareHist(h,href);
        _histTotal++;
        if (match) {
          _histMatch++;
        }
        else { // histograms do not match
          _noMatch.push_back(keyPath.Data());

          _can->Clear();
          _can->Divide(2,1);
          _can->cd(1)->SetPad(0,1,1,0.90);
          _can->cd(2)->SetPad(0,0.90,1,0);
          TVirtualPad* pad = _can->cd(2);
          
          if (_drawDiff) pad->Divide(2,1);
          _can->SetName(h.GetName());
          _can->SetTitle(h.GetTitle());
          // Overlayed
          pad->cd(1);
          if (_drawNormalized) {
            if (href.Integral()) href.Scale(1/href.Integral());
            if (h.Integral()) h.Scale(1/h.Integral());
          }

          Double_t ymax = 1.05*max(h.GetMaximum(),href.GetMaximum());
          h.SetMaximum(ymax);
          h.SetLineColor(kBlue);
          h.Draw();
          _can->Update();
          TPaveStats* st1 = (TPaveStats*)gPad->GetPrimitive("stats");
          if (st1) {
            st1->SetName("stats1");
            st1->SetLineColor(kBlue);
          }

          href.SetLineColor(kRed);
          href.Draw("sames");
          _can->Update();
          TPaveStats* st2 = (TPaveStats*)gPad->GetPrimitive("stats");
          if (st1 && st2) {
            // Move stat box
            Double_t x1 = st1->GetX1NDC()-0.01;
            st2->SetName("stats2");
            Double_t w = st2->GetX2NDC()-st2->GetX1NDC();
            st2->SetX1NDC(x1-w);
            st2->SetX2NDC(x1);
            st2->SetLineColor(kRed);
            _can->Modified();
          }

          TH1* hdiff = 0;
          if (_drawDiff) {
            hdiff = (TH1*)h.Clone();
            // Too many problems with difference of 2D histograms
            if (hdiff->GetDimension()==1 &&
                hdiff->GetNbinsX()==h.GetNbinsX()) {
              // Difference
              pad->cd(2);
              hdiff->SetName(TString(href.GetName())+" (diff)");
              hdiff->SetTitle(TString(href.GetTitle())+" (diff)");
              hdiff->SetLineColor(kBlack);
              hdiff->Add(&href,-1);
              hdiff->Draw();
              TPaveStats* st = (TPaveStats*)gPad->GetPrimitive("stats1");
              if (st) st->SetLineColor(kBlack);
            }
            if(hdiff->GetDimension()==2 && 
               hdiff->GetNbinsX()==h.GetNbinsX() && 
               hdiff->GetNbinsY()==h.GetNbinsY()) {
              pad->cd(2);
              hdiff->SetName(TString(href.GetName())+" (diff)");
              hdiff->SetTitle(TString(href.GetTitle())+" (diff)");
              hdiff->SetLineColor(kBlack);
              hdiff->Add(&href,-1);
              if(hdiff->GetXaxis()->GetLabels()!=0 && hdiff->GetNbinsX()>100) {
                 TH1 * hdiffred = (TH1*)hdiff->Clone(); 
                 hdiffred->GetXaxis()->GetLabels()->Delete();
                 hdiffred->Reset();
                 hdiffred->SetName(TString(href.GetName())+" (diff reduced)");
                 hdiffred->SetTitle(TString(href.GetTitle())+" (diff reduced)");
                 int targetbin=1;
                 for(int x=1; x<=hdiff->GetNbinsX(); ++x) {
                    bool isEmpty(true);
                    for(int y=1;y<=hdiff->GetNbinsY();++y) {
                       if(hdiff->GetBinContent(x,y)!=0) { isEmpty=false; break; }
                    }
                    if(!isEmpty) {
                       for(int y=1;y<=hdiff->GetNbinsY();++y) {
                          if(hdiff->GetBinContent(x,y)!=0)
                             hdiffred->SetBinContent(targetbin,y,hdiff->GetBinContent(x,y));
                       }
                       hdiffred->GetXaxis()->SetBinLabel(targetbin,hdiff->GetXaxis()->GetBinLabel(x));
                       targetbin++;
                    }
                 }
                 hdiffred->LabelsDeflate();
                 hdiffred->Draw("text");
              } else {
                 hdiff->Draw("text");
              }
              TPaveStats* st = (TPaveStats*)gPad->GetPrimitive("stats1");
              if (st) st->SetLineColor(kBlack);
            }
          }
          
          // Some more cosmetics before saving to ps file
          _can->cd(0);
          TText text;
          text.SetTextSize(0.03);
          text.SetTextAlign(22);
          TString page("page ");
          page += _noMatch.size();          
          text.DrawTextNDC(0.5,0.03,page);

          TString title(dir.GetName());
          title+="/";
          title+=href.GetName();          
          text.DrawTextNDC(0.5,0.99,title);

          if (_file) {
            text.SetTextColor(kBlue);
            text.DrawTextNDC(0.5,0.96,_file->GetName());
          }
          if (_refFile) {
            text.SetTextColor(kRed);
            text.DrawTextNDC(0.5,0.93,_refFile->GetName());
          }
                                 
          if (_psFile!="") printCanvas(_psFile);

          // Save canvas to root file
          if (_outFile) {
            createDirectory(_outFile,dirName);  // now we are in dirName
            _can->Write();
          }          
          
          if (hdiff) delete hdiff;
        }
      }
      else {  // histogram not found
        cout << "Cannot find " << keyPath << " in reference file" << endl;
        _histMissing++;
      }
    }    
    else { // could not cd() into directory of histogram
      _histMissing++;
      // TFile::cd() is already printing a similar message
      //cout << "Cannot find directory " << dirName << " in reference file" << endl;
    }
  }
  delete obj;
}


Bool_t TRootCompare::setReferenceFile(const char* filename,
                                    const char* baseDir)
{
  if (filename==0) {
    cout << "Invalid file name" << endl;
    return kFALSE;
  }
  
  _refFile = new TFile(filename);
  if (_refFile->IsZombie()) {
    cout << "Cannot open reference file " << filename << endl;
    delete _refFile;
    _refFile = 0;
    return kFALSE;
  }

  _refRootDir = baseDir;
  return kTRUE;
}


Bool_t TRootCompare::setOutputFile(const char* filename)
{
  if (filename==0) {
    cout << "Invalid file name" << endl;
    return kFALSE;
  }

  _outFile = new TFile(filename,"recreate");
  if (_outFile->IsZombie()) {
    cout << "Cannot open file " << filename << endl;
    delete _outFile;
    _outFile = 0;
    return kFALSE;
  }
  return kTRUE;
}

Bool_t TRootCompare::setPsFile(const char* filename)
{
  if (filename==0) {
    cout << "Invalid file name" << endl;
    return kFALSE;
  }
  _psFile = filename;
  return kTRUE;
}

void TRootCompare::printCanvas(const char* filename)
{
  if (_can==0) return;
  if (filename==0) return;

  // Suppress the info message when saving file
  Int_t oldIgnoreLevel = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kWarning;
  if (TString(filename).EndsWith(".ps"))  
    _can->Print(filename,"Landscape");
  else
    _can->Print(filename);
  gErrorIgnoreLevel = oldIgnoreLevel;  
}


Bool_t TRootCompare::compareHist(TH1& h, TH1& href)
{
  Bool_t result = kTRUE;
  
  if (verbose()) {
    cout << "Comparing " << h.GetName() << " using ";
  }
  
  if (_alg==TRootCompare::BIN) {
    if (verbose()) cout << "BIN: ";

    // This will work for histograms of all dimensions
    for (Int_t z=1; z<=h.GetNbinsZ() && result; z++) {
      for (Int_t y=1; y<=h.GetNbinsY() && result; y++) {
        for (Int_t x=1; x<=h.GetNbinsX() && result; x++) {
          Double_t binDiff = fabs(h.GetBinContent(x,y,z)-href.GetBinContent(x,y,z));
          if (binDiff>_threshold) {
            result = kFALSE;
          }
        }
      }
    }
  }
  else if (_alg==TRootCompare::AXIS) {
    if (verbose()) cout << "AXIS: ";
    TAxis *xa(h.GetXaxis()), *xaref(href.GetXaxis());
    TAxis *ya(h.GetXaxis()), *yaref(href.GetXaxis());
    TAxis *za(h.GetXaxis()), *zaref(href.GetXaxis());
    if( xa->GetNbins() != xaref->GetNbins() ) result = kFALSE;
    if( result && (ya->GetNbins() != yaref->GetNbins()) ) result = kFALSE;
    if( result && (za->GetNbins() != zaref->GetNbins()) ) result = kFALSE;
    if( result && (fabs( xa->GetBinLowEdge(0) - xaref->GetBinLowEdge(0) ) > _threshold ) ) result = kFALSE;
    if( result && (fabs( ya->GetBinLowEdge(0) - yaref->GetBinLowEdge(0) ) > _threshold ) ) result = kFALSE;
    if( result && (fabs( za->GetBinLowEdge(0) - zaref->GetBinLowEdge(0) ) > _threshold ) ) result = kFALSE;
    if( result ) for (Int_t i=0; i<=xa->GetNbins() && result; i++)
       if( fabs (xa->GetBinUpEdge(i) - xaref->GetBinUpEdge(i)) > _threshold ) result = kFALSE;
    if( result ) for (Int_t i=0; i<=ya->GetNbins() && result; i++)
       if( fabs (ya->GetBinUpEdge(i) - yaref->GetBinUpEdge(i)) > _threshold ) result = kFALSE;
    if( result ) for (Int_t i=0; i<=za->GetNbins() && result; i++)
       if( fabs (za->GetBinUpEdge(i) - zaref->GetBinUpEdge(i)) > _threshold ) result = kFALSE;
  }
  else if (_alg==TRootCompare::CHI2) {
    if (verbose()) cout << "CHI2: ";
    
    // Don't compare empty histograms
    if (h.GetEntries()==0 && href.GetEntries()==0) result = kTRUE;  // both empty
    else if (h.Integral()==0 && href.Integral()==0) result = kTRUE; // both empty
    else if (h.Integral()*href.Integral()==0) result = kFALSE;      // one empty
    else {
      Double_t chi2;
      Int_t igood;
      Int_t ndf;
      Double_t p = h.Chi2TestX(&href,chi2,ndf,igood);
      // this is because of a bug in root
      if (ndf==0) result = kTRUE;
      else if (p>_threshold) result = kTRUE;
      else result = kFALSE;
    }
  }  
  else {
    cout << "ERROR: Invalid algorithm." << endl;
  }

  if (verbose()) cout << result << endl;
  return result;
}


// Create all directories in dirpath
void TRootCompare::createDirectory(TFile* f, const char* dirpath)
{
  if ((f==0) || (dirpath==0)) return;

  f->cd();
  TString s(dirpath);
  TObjArray* a = s.Tokenize("/");
  for (int i=0; i<a->GetEntries(); i++) {
    const char* dir = a->At(i)->GetName();
    if (gDirectory->GetDirectory(dir)==0) gDirectory->mkdir(dir); // create if it doesn't exist
    gDirectory->cd(dir);
  }
  
}
