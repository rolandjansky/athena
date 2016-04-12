/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "HistoPile.h"

using namespace std;

//______________________________________________________________________________
void HistoPile::Push(const char *str) 
{
    m_path.push_back(str);
    m_path.push_back(".");
    m_currHist = 0;
}

//______________________________________________________________________________
void HistoPile::Pop() 
{
    m_path.pop_back();
    m_path.pop_back(); 
    m_currHist = 0;
}

//______________________________________________________________________________
const char * HistoPile::Path() 
{
    static char line[1024];
    Int_t len = 0;
  
    for( list<string>::iterator p=m_path.begin(); p!=m_path.end(); ++p) {
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
  if ( !m_currHist) {  
  
    const char *path = Path();
  
    m_currHist = (TH1F*)m_histos->FindObject(path);

    // not yet in list - try to take def from reference file
    if (!m_currHist && m_reffile) {
        TH1F* refHist = (TH1F*)m_reffile->Get(path);
        if (!refHist) {
          cout << "WARNING Could not find "<<path<<" in reference file!" << endl;        
        } else {
//          cout << ">>>CLONE HISTO<<< " << path << endl;
          m_currHist = (TH1F*)refHist->Clone(path);
          m_currHist->Reset();
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)        
          m_currHist->SetCanExtend(TH1::kNoAxis);    // don't allow rebinning binning
#else
          m_currHist->SetBit(TH1::kCanRebin,0);
#endif
//          m_currHist->SetDirectory(0);
        //cout << m_currHist->Hash() << " " << refHist->Hash() << endl; 
          m_histos->AddLast(m_currHist);
        }
    }

    if (!m_currHist) {  
        // some default - tbc
        m_currHist =  new TH1F(path,path,40,f/2.,f*2.); 
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)        
        m_currHist->SetCanExtend(TH1::kAllAxes);
#else
        m_currHist->SetBit(TH1::kCanRebin);
#endif
        m_currHist->SetDirectory(0);
        m_histos->AddLast(m_currHist);
//        cout << ">>>NEW HISTO<<< " << path << endl;
    }
  }
  m_currHist->Fill(f);
}

//______________________________________________________________________________
void HistoPile::Save() 
{
	if (!m_filename) return;
    
    TFile* file = TFile::Open(m_filename,"RECREATE");

	TIter next(m_histos);
	TH1F* hist;
	while ((hist = (TH1F *)next() ))
	{
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)        
          hist->SetCanExtend(TH1::kNoAxis);    // don't allow rebinning binning
#else
          hist->SetBit(TH1::kCanRebin,0);
#endif
	  hist->SetDirectory(file);
	  hist->Write();
	  //cout << hist->GetName() << endl;
	  //hist->Print();
	  //  if (obj->GetUniqueID()) nbins++;
      
      //Remove(hist); // clean up
	}

	delete file;	   

	cout << "Histograms created: about "
		<< m_histos->AverageCollisions()*m_histos->GetEntries() << endl;

}

