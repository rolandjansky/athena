// ---------------------------------------------------------------------- 
// Make histograms' average (histograms at 1 directory level are averaged)
// Command:
//      root -b make_avg.C+
// ---------------------------------------------------------------------- 

#include <TROOT.h>
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
// Entry point for ROOT. Note that the same 'directory structure' of all ROOT 
// files is assumed.
// ---------------------------------------------------------------------- 
void make_avg()
{
  cout << "======================================================================" << endl;
  cout << "Histograms averaging..." << endl;
  cout << "======================================================================" << endl;
  
  const string dirPath = "."; 
  
  string avgFileName( "avg_histos.root" );

  vector< string > fileNames = listFiles( dirPath, ".root", avgFileName );
  cout << "In total " << fileNames.size() << " ROOT file(s) found" << endl;
  if ( fileNames.empty() ) {
    return; 
  }
  
    // make a copy of one Root file overwriting it if necessary 
  if ( gSystem->CopyFile( fileNames[ 0 ].c_str(), avgFileName.c_str(), kTRUE ) != 0 ) {
    cerr << "Couldn't make file copy!" << endl;
    return;
  }

  vector< HistMap > histMapVec;

    // process Root files found in a given directory
  for ( size_t i = 0; i < fileNames.size(); ++i ) {
    cout << "- processing file " << fileNames[ i ] << "..." << endl;

    TFile *file = new TFile( fileNames[ i ].c_str(), "READ" );
    if ( file->IsZombie() ) {
      cerr << "Cannot open file! Exit." << endl;
      return;
    }
    
    HistMap histMap;

    TIter nextKey( file->GetListOfKeys() );
    TKey *key = 0;
    while( ( key = (TKey*)nextKey() ) ) {
      TClass *c1 = gROOT->GetClass( key->GetClassName() );

      if ( !c1->InheritsFrom( "TH1" ) ) {
        if ( c1->InheritsFrom( "TDirectory" ) ) {
          cout << " * entering directory ";

          TDirectory *d = (TDirectory*)key->ReadObj();
          cout << d->GetName() << endl;

          TIter nextKey2( d->GetListOfKeys() );

          TKey *key2;
          while( ( key2 = (TKey*)nextKey2() ) ) {
            TClass *c2 = gROOT->GetClass( key2->GetClassName() );
            if ( c2->InheritsFrom( "TH1" ) ) {
              TH1 *h = (TH1*)key2->ReadObj();

              //cout << "  - found histogram: " << h->GetName() << endl;

                // keep copy of found histogram
              //histMap.push_back( make_pair( string( h->GetName() ), (TH1*)h->Clone() ) );
              histMap.push_back( make_pair( string( h->GetName() ), h ) );
            }
          }
        }
      } else {    // histos in the 'main directory'
        //TH1 *h = (TH1*)key->ReadObj();
        //cout << " - found hlev histogram: " << h->GetName() << endl;

          // keep copy of found histogram
//        histMap.push_back( make_pair( string( h->GetName() ), (TH1*)h->Clone() ) );
      }

    }

      // created map to the vector
    histMapVec.push_back( histMap );

//    file->Close();
  }

  createAvgFile( avgFileName, histMapVec );
  cout << "Done!" << endl;
}

// ---------------------------------------------------------------------- 
// 
// ---------------------------------------------------------------------- 
void createAvgFile( const string &avgFileName, const vector< HistMap > &histMapVec )
{
  cout << "* updating avg file: " << endl;

  TFile *avgFile = new TFile( avgFileName.c_str(), "UPDATE" ); 
  if ( avgFile->IsZombie() ) {
    cerr << "Cannot open file! Exit." << endl;
    return;
  }

  TIter nextKey( avgFile->GetListOfKeys() );

  size_t histCnt = 0;

  TKey *key;
  while( ( key = (TKey*)nextKey() ) ) {
      TClass *c1 = gROOT->GetClass( key->GetClassName() );
      if ( !c1->InheritsFrom( "TH1" ) ) {
        if ( c1->InheritsFrom( "TDirectory" ) ) {
          cout << " * entering directory ";

          TDirectory *d = (TDirectory*)key->ReadObj();
          cout << d->GetName() << endl;
          d->cd();

          TIter nextKey2( d->GetListOfKeys() );

          TKey *key2;
          while( ( key2 = (TKey*)nextKey2() ) ) {
            TClass *c2 = gROOT->GetClass( key2->GetClassName() );
            if ( c2->InheritsFrom( "TH1" ) ) {
              TH1 *h = (TH1*)key2->ReadObj();

              //cout << " - found histogram: " << h->GetName() << endl;
          
              //cout << "# of root files: " << histMapVec.size() << endl;

              vector< TH1* > sameHist;
                // loop over other files
              for ( size_t ii = 0; ii < histMapVec.size(); ++ii ) {
                //cout << "ii= " << ii << ", histct: " << histCnt << endl;
                TH1 *currHist = (TH1*)( histMapVec[ ii ][ histCnt ].second );
                //cout << "currhist: " << currHist << endl;
                //cout << "currhist: " << currHist->GetName() << endl;


                sameHist.push_back( currHist );
              }

              makeAverage( h, sameHist );

              h->Write( h->GetName(), TObject::kWriteDelete );
              //h->Write( h->GetName(), TObject::kOverwrite );
              ++histCnt;
            }
          }

            // go up
         d->cd( ".." );
        }


      } else {    // histos in the 'main directory'
        TH1 *h = (TH1*)key->ReadObj();
        cout << " - found hlev histogram: " << h->GetName() << endl;

          // keep copy of found histogram
//        histMap.push_back( make_pair( string( h->GetName() ), (TH1*)h->Clone() ) );
      }

    }
  
//  avgFile->Write();
//  avgFile->Close();
}

// ---------------------------------------------------------------------- 
// Make average of a given vector of histograms.
// ---------------------------------------------------------------------- 
bool makeAverage( TH1 *avgHisto, const vector< TH1* > &histograms )
{
  const size_t nHistograms = histograms.size();
  if ( nHistograms == 0 ) {
    cerr << "No histogram given!" << endl;
    return false;
  }

    // validate pointers first
  for ( size_t i = 0; i < nHistograms; ++i ) {
    if ( histograms[ i ] == 0 ) {
      cerr << "Histogram # " << i << " is null!" << endl;
      return false;
    }
  }
  
    // validate number of bins
  const size_t nBins = histograms[ 0 ]->GetNbinsX();
  //cout << " nBins: " << nBins << endl;

  for ( size_t i = 1; i < nHistograms; ++i ) {
    if ( histograms[ i ]->GetNbinsX() != (int)nBins ) {
      cerr << "Incorrect number of bins for histogram '" << i << "'" << endl;
      cerr << " [i]: " << histograms[ i ]->GetNbinsX() << ", [0]: " << nBins << endl;
      return false;
    }
  }
  
  vector< double > averages;
  vector< double > means;

    // set average
  for ( size_t i = 0; i < nBins; ++i ) {
    double avg = 0.0;
    for ( size_t j = 0; j < nHistograms; ++j ) {
      avg += histograms[ j ]->GetBinContent( i ) / nHistograms;
    }
    //cout << "- avg: " << avg << endl;
    avgHisto->SetBinContent( i, avg );
  }

    // get sum of errors for all bins
  double errSum = 0.0;
  for ( size_t i = 0; i < nBins; ++i ) {
    errSum += TMath::Abs( avgHisto->GetBinError( i ) );
  }

  bool histoHasErrors = ( errSum != 0 );
  if ( histoHasErrors ) {
    //cout << "Histograms with errors" << endl;
  }

    // set mean value
  for ( size_t i = 0; i < nBins; ++i ) {
    double avg = avgHisto->GetBinContent( i );
    //cout << "read avg: " << avg << endl;

    double del = 0.0;
    for ( size_t j = 0; j < nHistograms; ++j ) {
      if ( histoHasErrors ) { 
        del += TMath::Power( histograms[ j ]->GetBinError( i ), 2 );
      } else {
        del += TMath::Power( histograms[ j ]->GetBinContent( i ) - avg, 2 );
      }
    }
    double dev = TMath::Sqrt( del / nHistograms );
    //cout << "- avg: " << avg << ", del: " << del << ", dev: " << dev << endl;
    avgHisto->SetBinError( i, dev );
  }

  return true;
}

// ---------------------------------------------------------------------- 
// Return vector of file names with extension 'ext' in a given directory 
// ---------------------------------------------------------------------- 
vector< string > listFiles( const string &dirName, const string &ext, const string &toExclude )
{
  vector< string > fileNames;
  cout << "Listing directory '" << dirName << "'" << endl;

  TSystemDirectory dir( dirName.c_str(), dirName.c_str() );
  TList *files = dir.GetListOfFiles(); 

  if ( files ) {
    TSystemFile *file = 0;
    TString fName;

      // loop over files in a given directory 
    TIter next( files );
    while ( ( file = (TSystemFile*)next() ) ) {
      fName = file->GetName();
      
      if ( !file->IsDirectory() && fName.EndsWith( ext.c_str() ) && toExclude != string( fName.Data() ) ) {
        fileNames.push_back( string( fName.Data() ) );
        cout << fName.Data() << endl;
      }
    }
  }

  return fileNames;
}

