/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// MSPlotLooper.C
// Massimiliano Bellomo (INFN), Jul 2007
// <massimiliano.bellomo@pv.infn.it>
//////////////////////////////////////////////////////////
/**
modifications to have histograms for eff/resol analysis both in MuonRecValidator and MuonInsituPerformace.

MSPlotLooper.C which contains following functions :

AddLooper : to sum histograms and (not implemented yet) trees among several root files. it could be useful for batch jobs.
PlotLooper: to loop and plot interactively all histograms in a file, looping inside each directory. 
For each plot you can interact with the canvas and the press <enter> for next plot, "n" for next dir, "s" to save in gif format or "q" to quit.
CompareLooper : to loop and compare  histograms for two different runs, usefull for RTT histos comparisons.
*/
 
#include <vector>

//----------------------------------------------------------------------
gROOT->Reset();

//______________________________________________________________________
// give the list of files below. Last file must be a NULL string
const char *cfiles[] = {
  "../run/MuonRecValidationAOD1.aan.root",
  "../run/MuonRecValidationAOD2.aan.root",
  ""
};

const char *outfile="file.root";
//______________________________________________________________________

TFile   *fnew;
TList   *flist;
TFile   *afile, *file1, *file2;

TH1     *h1, *h2;
TTree   *t1, *t2;
TObject *obj;
TKey    *key;

vector<TDirectory*> paths;

void AddRecursive(TDirectory *root,TDirectory* node); 
//______________________________________________________________________
//
//
//  
//______________________________________________________________________
void AddLooper() {

  // create the result file
  fnew = new TFile(outfile,"RECREATE");
  
  //create a support list for the input files
  flist = new TList();
  
  //open all input files and insert them in the list of files
  Int_t nfiles = 0;
  while (strlen(cfiles[nfiles])) {
    afile = new TFile(cfiles[nfiles]);
    flist->Add(afile);
    nfiles++;
  }
  
  //Get a pointer to the first file
  afile = file1 = (TFile*)flist->First();

  AddRecursive(fnew,file1);
  
  cout<<"Done, clean up"<<endl;
  fnew->ls();
  fnew->Write();
  fnew->Close();
  delete fnew;
  flist->Delete();
  delete flist;
}

//______________________________________________________________________
//
//
//  
//______________________________________________________________________
void AddRecursive(TDirectory *root,TDirectory* node) {

  static TDirectory *dact;

  //We create an iterator to loop on all objects(keys) of first file
  TIter nextkey(node->GetListOfKeys());
  while (key = (TKey*)nextkey()) {
    node->cd();
    obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom("TTree")) { //case of a TTree or TNtuple
      t1 = (TTree*)obj;
      // this part still to be implemented
      // use TChain::Merge instead
    } elseif(obj->IsA()->InheritsFrom("TH1")) { //case of TH1 or TProfile
      h1 = (TH1*)obj;
      afile = (TFile*)flist->After(node);
      while (afile) { //loop on all files starting at second file
	h2 = (TH1*)afile->Get(h1->GetName());
	if (h2) { // here we should check that we can add
	  h1->Add(h2);
	  delete h2;
	}
	afile = (TFile*)flist->After(afile);
      }
    } elseif(obj->IsA()->InheritsFrom("TDirectory")) { //case of TDirectory
      root->cd();
      dact=root->mkdir(obj->GetName(),obj->GetTitle());
      dact->cd();
      AddRecursive(dact,(TDirectory*)obj);
    } else { //another object
      printf("anotherobjname=%s, title=%s\n",obj->GetName(),obj->GetTitle());
    }

    // write node object, modified or not into fnew
    if (obj) {
      root->cd();
      obj->Write(key->GetName());
      delete obj;
      obj=NULL; 
    }
  }
  root->cd();
}
//______________________________________________________________________ 


int PlotRecursive(TDirectory* node); 
//______________________________________________________________________
//
//
//  
//______________________________________________________________________
void PlotLooper (const char* filename) {
 
  gStyle->SetPalette(1);
  // input file
  file1 = new TFile(filename);
  
  PlotRecursive(file1);
  
}

//______________________________________________________________________
//
//
//  
//______________________________________________________________________
int PlotRecursive(TDirectory* node) 
{
  printf("Current dir --> %s",node->GetName());

  TTimer  *timer = new TTimer("gSystem->ProcessEvents();", 100, kFALSE);

  //We create an iterator to loop on all objects(keys) of file
  TIter nextkey(node->GetListOfKeys());
  while (key = (TKey*)nextkey()) {
    node->cd();
    obj = key->ReadObj();
    
    if (obj->IsA()->InheritsFrom("TTree")) { //case of a TTree or TNtuple
      t1 = (TTree*)obj;
      // this part still to be implemented
      // t1->Draw("something");
    
    }elseif(obj->IsA()->InheritsFrom("TH1")) { //case of TH1 or TProfile
      h1 = (TH1*)obj;
      Bool_t done = kFALSE;
      do {
	TCanvas *c0 = new TCanvas("c0","c0",600,600);
	if(obj->IsA()->InheritsFrom("TH2")) 
	  h1->Draw("colz");
	else
	  h1->Draw();
	  printf("\n TH1 --> %s \n",h1->GetName());
				
	c0->Modified();
	c0->Update(); 
		
	char input[40];
	timer->TurnOn();
	timer->Reset();
	printf("Type <enter> to continue, s:save, n:next_dir, q:exit \n");
	gets(input); 
	timer->TurnOff();
	if (!strncmp(input,"q",1)) return 0;
	else if(!strncmp(input,"s",1)) {
	  TString name = h1->GetName();
	  name.Append(".gif");
	  c0->SaveAs(name.Data());
	  done = kTRUE;
	}else if(!strncmp(input,"n",1)) {
	  return 1;
	}
	else
	  done = kTRUE;
      } while (!done);
      
    }elseif(obj->IsA()->InheritsFrom("TDirectory")) { //case of TDirectory
      
      int flag = PlotRecursive((TDirectory*)obj);
      if(!flag) return 0;

    }else { //another object
      
      printf("\n anotherobjname=%s, title=%s\n",obj->GetName(),obj->GetTitle());
    }
    
  }
  return 1;
}
//______________________________________________________________________ 

int ComapareRecursive(TDirectory* file1, TFile* file2);

void CompareLooper(const char* filename1, const char* filename2) {
 
  gStyle->SetPalette(1);
  // input file
  file1 = new TFile(filename1);
  file2 = new TFile(filename2);
  
  CompareRecursive(file1, file2);
  
}

//______________________________________________________________________
//
//
//  
//______________________________________________________________________
int CompareRecursive(TDirectory* node, TFile* ref) 
{
  printf("Compare in current dir --> %s",node->GetName());

  TTimer  *timer = new TTimer("gSystem->ProcessEvents();", 100, kFALSE);

  //We create an iterator to loop on all objects(keys) of file
  TIter nextkey(node->GetListOfKeys());
  while (key = (TKey*)nextkey()) {
    node->cd();
    obj = key->ReadObj();
    
    if (obj->IsA()->InheritsFrom("TTree")) { //case of a TTree or TNtuple
      t1 = (TTree*)obj;
      // this part still to be implemented
      // t1->Draw("something");
    
    }elseif(obj->IsA()->InheritsFrom("TH1")) { //case of TH1 or TProfile
      h1 = (TH1*)obj;
      Bool_t done = kFALSE;
      do {
	TCanvas *c0 = new TCanvas("c0", "c0",504,47,750,920);
	c0->Range(0,0,1,1);
	c0->SetFillColor(0);
	c0->SetBorderMode(0);
	c0->SetBorderSize(2);

	// ------------>Primitives in pad: c0_1
	TPad *c0_1 = new TPad("c0_1", "c0_1",0.0080429,0.394619,0.989276,0.991031);
	c0_1->Draw();
	c0_1->cd();
	c0_1->Range(-21214.7,-1548.21,110719,8317.49);
	c0_1->SetFillColor(0);
	c0_1->SetBorderSize(2);
	c0_1->SetLeftMargin(0.160798);
	c0_1->SetRightMargin(0.0812447);
	c0_1->SetTopMargin(0.0417362);
	c0_1->SetBottomMargin(0.156928);
	
	h1->GetYaxis()->SetTitleOffset(1.31);
	
	if(obj->IsA()->InheritsFrom("TH2")) 
	  h1->Draw("colz");
	else
	  h1->Draw();
	
	TString path;
	for(int i=0; i<(int)paths.size(); i++) {
	  path.Append(paths[i]->GetName());
	  path.Append("/");
	}
	path.Append(h1->GetName());
		  
	printf("\n TH1 path --> %s \n",path.Data());

	TH1* href = (TH1*)ref->Get(path.Data());
	href->SetMarkerColor(2);
	href->Draw("same");
	
	TLegend *l = new TLegend(0.465847,0.537594,0.990437,0.676692,NULL,"brNDC");
	l->AddEntry(href,"Reference Athena vXXX - XXX", "p");
	l->AddEntry(h1,"Athena vXXX - XXX", "p");
	l->SetFillColor(0);
	l->Draw("same");

	c0_1->Modified();
	c0->cd();
	
	// ------------>Primitives in pad: c0_2
	c0_2 = new TPad("c0_2", "c0_2",0.00670241,0.00784753,0.987936,0.408072);
	c0_2->Draw();
	c0_2->cd();
	c0_2->Range(37349,-45.8763,41320.1,29.5252);
	c0_2->SetFillColor(0);
	c0_2->SetBorderSize(2);
	c0_2->SetLeftMargin(0.163934);
	c0_2->SetRightMargin(0.0806011);
	c0_2->SetTopMargin(0.0308123);
	c0_2->SetBottomMargin(0.2493);

	TH1* dif = (TH1*)h1->Clone();
	dif->Reset();
	dif->Add(href, h1, 1, -1);
	
	dif->SetTitle("Difference");
	dif->GetYaxis()->SetTitle("Ref - Test");
	dif->GetXaxis()->SetLabelSize(0.08);
	dif->GetXaxis()->SetTitleSize(0.09);
	dif->GetYaxis()->SetLabelSize(0.08);
	dif->GetYaxis()->SetTitleSize(0.08);
	dif->GetYaxis()->SetTitleOffset(0.8);
	dif->SetStats(0);
	dif->SetMarkerColor(1);
	dif->SetLineColor(2);
	dif->Draw();

	c0_2->Modified(); 
	c0->Modified();
	c0->Update(); 
		
	char input[40];
	timer->TurnOn();
	timer->Reset();
	printf("Type <enter> to continue, s:save, n:next_dir, q:exit \n");
	gets(input); 
	timer->TurnOff();
	if (!strncmp(input,"q",1)) return 0;
	else if(!strncmp(input,"s",1)) {
	  TString name = h1->GetName();
	  name.Append(".gif");
	  c0->SaveAs(name.Data());
	  done = kTRUE;
	}else if(!strncmp(input,"n",1)) { 
	  paths.pop_back();
	  return 1;
	}
	else
	  done = kTRUE;
      } while (!done);
      
    }elseif(obj->IsA()->InheritsFrom("TDirectory")) { //case of TDirectory

      paths.push_back((TDirectory*)obj);
      int flag = CompareRecursive((TDirectory*)obj, ref); 
      if(!flag) {
	paths.clear();
	return 0;
      }
    }else { //another object
      
      printf("\n anotherobjname=%s, title=%s\n",obj->GetName(),obj->GetTitle());
    }    
  }
  cout << "going to return" << endl;
  paths.pop_back();
  return 1;
}
//______________________________________________________________________ 
  
 

void MSCanvasLooper(const char* file) {
  
  TTimer  *timer = new TTimer("gSystem->ProcessEvents();", 100, kFALSE);
  TFile f(file);

  TIter nextkey(gDirectory->GetListOfKeys());
  while (key = (TKey*)nextkey()) {
    obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom("TDirectory")) {
      dir = (TDirectory*)obj;
      
      cout << "dir --> " << dir->GetName() << endl;
     
      TIter nextkey2(dir->GetListOfKeys());
      while (key = (TKey*)nextkey2()) {
	obj = key->ReadObj();
	if (obj->IsA()->InheritsFrom("TDirectory")) {
	  dir = (TDirectory*)obj;

	  cout << "dir --> " << dir->GetName() << endl;

	  TIter nextkey3(dir->GetListOfKeys());
	  while (key = (TKey*)nextkey3()) {

	    obj = key->ReadObj();
	    
	    if (obj->IsA()->InheritsFrom("TH1"))
	      cout << "is histo" << endl;
	    
	    if (obj->IsA()->InheritsFrom("TCanvas")) {
	      
	      TCanvas *c = (TCanvas*)obj; 
	      
	      Bool_t done = kFALSE;
	      do {
		
		c->Draw();
		cout << "Canvas --> " << c->GetName() << endl;
		
		c->Modified();
		c->Update(); 
		
		char input[40];
		timer->TurnOn();
		timer->Reset();
		cout << "Type <enter> to continue, q to exit" << endl;
		gets(input); 
		timer->TurnOff();
		if (!strncmp(input,"q",1)) return;
		else
		  done = kTRUE;
	      } while (!done);
	    }
	  }
	}
      }
    }
  }
}


