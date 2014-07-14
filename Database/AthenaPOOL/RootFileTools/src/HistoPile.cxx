/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "HistoPile.h"

//______________________________________________________________________________
void HistoPile::Push(const char *str) 
{
    fPath.push_back(str);
    fPath.push_back(".");
    fCurrHist = 0;
}

//______________________________________________________________________________
void HistoPile::Pop() 
{
    fPath.pop_back();
    fPath.pop_back(); 
    fCurrHist = 0;
}

//______________________________________________________________________________
const char * HistoPile::Path() 
{
    static char line[1024];
    Int_t len = 0;
  
    for( list<string>::iterator p=fPath.begin(); p!=fPath.end(); ++p) {
      strcpy( line + len, (*p).c_str() );
      len += p->size();
    }
    // replace trailing dot
    *(line + len-1) = 0; //' '; 
    return line; 
}

//______________________________________________________________________________
void HistoPile::Fill(Float_t f) 
{  
  // only look for current histogram again if not set
  if ( !fCurrHist) {  
  
    const char *path = Path();
  
    fCurrHist = (TH1F*)fHistos->FindObject(path);

    // not yet in list - try to take def from reference file
    if (!fCurrHist && fReffile) {
        TH1F* refHist = (TH1F*)fReffile->Get(path);
        if (!refHist) {
          cout << "WARNING Could not find "<<path<<" in reference file!" << endl;        
        } else {
//          cout << ">>>CLONE HISTO<<< " << path << endl;
          fCurrHist = (TH1F*)refHist->Clone(path);
          fCurrHist->Reset();
          fCurrHist->SetBit(TH1::kCanRebin,0);    // don't allow rebinning binning
//          fCurrHist->SetDirectory(0);
        //cout << fCurrHist->Hash() << " " << refHist->Hash() << endl; 
          fHistos->AddLast(fCurrHist);
        }
    }

    if (!fCurrHist) {  
        // some default - tbc
        fCurrHist =  new TH1F(path,path,40,f/2.,f*2.); 
        fCurrHist->SetBit(TH1::kCanRebin);
        fCurrHist->SetDirectory(0);
        fHistos->AddLast(fCurrHist);
//        cout << ">>>NEW HISTO<<< " << path << endl;
    }
  }
  fCurrHist->Fill(f);
}

//______________________________________________________________________________
void HistoPile::Save() 
{
	if (!fFilename) return;
    
    TFile* file = TFile::Open(fFilename,"RECREATE");

	TIter next(fHistos);
	TH1F* hist;
	while ((hist = (TH1F *)next() ))
	{
	  hist->SetBit(TH1::kCanRebin,0);
	  hist->SetDirectory(file);
	  hist->Write();
	  //cout << hist->GetName() << endl;
	  //hist->Print();
	  //  if (obj->GetUniqueID()) nbins++;
      
      //Remove(hist); // clean up
	}

	delete file;	   

	cout << "Histograms created: about "
		<< fHistos->AverageCollisions()*fHistos->GetEntries() << endl;

}

