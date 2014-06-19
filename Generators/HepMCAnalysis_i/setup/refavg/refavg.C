/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ---------------------------------------------------------------------- 
// average histograms in directory given as firt argument (default is current directory)
// ---------------------------------------------------------------------- 

#include <TROOT.h>
#include <TClass.h>
#include <TH1.h>
#include <TFile.h>
#include <TSystem.h>
#include <TList.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TString.h>
#include <TKey.h>
#include <TMath.h>

#include <iostream>
#include <string>
#include <vector>
#include <utility>

using namespace std;

typedef vector< pair< string, TH1* > > HistMap;

vector< string > listFiles( const string &dirName, const string &ext, const string &toExclude );
void createAvgFile( const string &avgFileName, const vector< HistMap > &histMapVec );
bool makeAverage( TH1 *avgHisto, const vector< TH1* > &histograms );

// ---------------------------------------------------------------------- 
// The same 'directory structure' of all ROOT files is assumed.
// ---------------------------------------------------------------------- 
int main(int argNum, char *args[])
  //arguments:
  //First argument is the directory where root files are searched
  //Second argument is the name of the output file
{
  cout << "Histograms averaging..." << endl;

  string dirPath = "."; 
  if (argNum > 1)
    dirPath = args[1];

  cout << "Average of histograms in " << dirPath << endl;

  string avgFileName = "avg_histos.root";
  if (argNum > 2)
    avgFileName = args[2];

  cout << "Output file: " << avgFileName << endl;

  //list of files in given directory, except output file
  vector< string > fileNames = listFiles(dirPath, ".root", avgFileName);
  cout << "Averaging " << fileNames.size() << " ROOT file(s)" << endl;
  if (fileNames.empty()) 
    {
      cout << "No files to average found" << endl;
      return -1;
    }
  
  // make a copy of one Root file overwriting it if necessary 
  if (gSystem->CopyFile(fileNames[0].c_str(), avgFileName.c_str(), kTRUE) != 0) 
    {
      cerr << "Couldn't make file copy!" << endl;
      return -1;
    }

  vector <HistMap> histMapVec;

  // process Root files
  for (size_t i = 0; i < fileNames.size(); ++i) 
    {
      cout << "- processing file " << fileNames[ i ] << "..." << endl;

      TFile *file = new TFile( fileNames[ i ].c_str(), "READ" );
      if (file->IsZombie())
	{
	  cerr << "Cannot open file! Exit." << endl;
	  return -1;
	}
    
      HistMap histMap;

      TIter nextKey(file->GetListOfKeys());
      TKey *key = 0;
      while (key = (TKey*)nextKey()) 
	{
	  TClass *c1 = gROOT->GetClass(key->GetClassName());

	  if (!c1->InheritsFrom("TH1"))
	    if (c1->InheritsFrom("TDirectory"))
	      {
		cout << " * entering directory ";

		TDirectory *d = (TDirectory*)key->ReadObj();
		cout << d->GetName() << endl;

		TIter nextKey2(d->GetListOfKeys());

		TKey *key2;
		while (key2 = (TKey*)nextKey2()) 
		  {
		    TClass *c2 = gROOT->GetClass(key2->GetClassName());
		    if (c2->InheritsFrom("TH1")) 
		      {
			TH1 *h = (TH1*)key2->ReadObj();
			histMap.push_back( make_pair( string( h->GetName() ), h ) );
		      }
		  }
	      }
	}
      // created map to the vector
      histMapVec.push_back( histMap );
      //    file->Close();
    }

  createAvgFile(avgFileName, histMapVec);
  cout << "Done!" << endl;
}

// ---------------------------------------------------------------------- 
// 
// ---------------------------------------------------------------------- 
void createAvgFile( const string &avgFileName, const vector< HistMap > &histMapVec )
{
  cout << "* updating avg file: " << endl;

  TFile *avgFile = new TFile(avgFileName.c_str(), "UPDATE"); 
  if (avgFile->IsZombie()) 
    {
      cerr << "Cannot open file! Exit." << endl;
      return;
    }

  TIter nextKey(avgFile->GetListOfKeys());
  size_t histCnt = 0;

  TKey *key;
  while (key = (TKey*)nextKey()) 
    {
      TClass *c1 = gROOT->GetClass(key->GetClassName());
      if (!c1->InheritsFrom("TH1")) 
        if (c1->InheritsFrom("TDirectory"))
	  {
	    cout << " * entering directory ";

	    TDirectory *d = (TDirectory*)key->ReadObj();
	    cout << d->GetName() << endl;
	    d->cd();

	    TIter nextKey2(d->GetListOfKeys());

	    TKey *key2;
	    while (key2 = (TKey*)nextKey2()) 
	      {
		TClass *c2 = gROOT->GetClass(key2->GetClassName());
		if (c2->InheritsFrom("TH1")) 
		  {
		    TH1 *h = (TH1*)key2->ReadObj();
		    vector< TH1* > sameHist;
		    // loop over other files
		    for (size_t ii = 0; ii < histMapVec.size(); ++ii) 
		      {
			TH1 *currHist = (TH1*)( histMapVec[ ii ][ histCnt ].second );
			sameHist.push_back( currHist );
		      }
		    makeAverage(h, sameHist);
		    h->Write( h->GetName(), TObject::kWriteDelete );
		    ++histCnt;
		  }
	      }

            // go up
	    d->cd( ".." );
	  }
    }
  
  //  avgFile->Write();
  //  avgFile->Close();
}

// ---------------------------------------------------------------------- 
// Make average of a given vector of histograms.
// ---------------------------------------------------------------------- 
bool makeAverage(TH1 *avgHisto, const vector <TH1*> &histograms)
{
  const size_t nHistograms = histograms.size();
  if (nHistograms == 0) 
    {
      cerr << "No histogram given!" << endl;
      return false;
    }

  // validate pointers first
  for (size_t i = 0; i < nHistograms; ++i)
    if (histograms[i] == 0) 
      {
	cerr << "Histogram # " << i << " is null!" << endl;
	return false;
      }
  
  // validate number of bins
  const size_t nBins = histograms[0]->GetNbinsX();

  for (size_t i = 0; i < nHistograms; ++i) 
    if (histograms[i]->GetNbinsX() != (int)nBins) 
      {
	cerr << "Incorrect number of bins for histogram '" << i << "'" << endl;
	cerr << " [i]: " << histograms[i]->GetNbinsX() << ", [0]: " << nBins << endl;
	return false;
      }

  avgHisto->Reset();
  for (size_t i = 0; i < histograms.size(); ++i) 
//  for (size_t i = 0; i < 1; ++i) //use only one histogram, because KS test is done with options NDX, and is sensitive to different statistic
    {
      histograms[i]->Sumw2();
      avgHisto->Add(histograms[i]);
    }
  avgHisto->Scale(1./(double)histograms.size());

  return true;
}

// ---------------------------------------------------------------------- 
// Return vector of file names with extension 'ext' in a given directory 
// ---------------------------------------------------------------------- 
vector <string> listFiles(const string &dirName, const string &ext, const string &toExclude)
{
  vector <string> fileNames;
  cout << "Listing directory '" << dirName << "'" << endl;

  TSystemDirectory dir(dirName.c_str(), dirName.c_str());
  TList *files = dir.GetListOfFiles();

  if (files) 
    {
      TSystemFile *file = 0;
      TString fName;

      // loop over files in a given directory 
      TIter next(files);
      while (file = (TSystemFile*)next()) 
	{
	  fName = file->GetName();
      	  if (!file->IsDirectory() && fName.EndsWith(ext.c_str()) && toExclude != string(fName.Data())) 
	    {
	      fileNames.push_back(dirName + "/" + string(fName.Data()));
	      cout << fName.Data() << endl;
	    }
	}
    }
  return fileNames;
}

