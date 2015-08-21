/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKTrack.h"

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TMath.h>

#include <getopt.h>

#ifdef HAVE_BOOST_IOSTREAMS
#include <boost/array.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/iostreams/device/file.hpp>
#endif
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
using namespace std;

/*
 * F. Volpi 15-Jan-2010
 *
 * Added the possibility to read bzip2 or gzip compressed files using
 * boost library.
 *
 * G. Volpi 12-May-2009
 *
 * This tool convert FTK data file in ASCII format in a ROOT
 * compatible way.
 *
 */

string rfname;
int num_events(-1);

int main(int argc, char *argv[]) {
   char c;
   while ( (c=getopt(argc,argv,"n:o:"))!=-1 ) { // option parsing loop
      switch (c) { // options selection
      case 'n': // set the number of events to convert
	 num_events = atoi(optarg);
	 break;
      case 'o': // set name of the output file
	 rfname = optarg;
	 break;
      } // end option selection
   } // end option parsing loop

   if (rfname=="") {
      cerr << "!!! Missing ROOT output file name" << endl;
      return -1;
   }

   // open the root file to store the TTree
   TFile *ofile = TFile::Open(rfname.c_str(),"recreate");


   // create the input TTree
   TTree *tree = new TTree("input_data","FTK input data");

   // declaration of the iPat branches and needed variables
   TClonesArray *iPatTracks = new TClonesArray("FTKTrack",10);
   FTKTrack *rectrack;
   int n_iPat(0);
   tree->Branch("iPat_tracks",&iPatTracks);
//    TRefArray *ipat_refs = new TRefArray();
//    tree->Branch("iPat_refs",&ipat_refs);

   // declaration of the truth branches and needed variables
   TClonesArray *TruthTracks = new TClonesArray("FTKTrack",10);
   int n_Truth(0);
   FTKTrack *truthtrack;
   tree->Branch("Truth_tracks",&TruthTracks);
//    TRefArray *truth_refs = new TRefArray();
//    tree->Branch("Truth_refs",&truth_refs);



   int itot_evt(0);
   for (int i=optind;i<argc&&(itot_evt<num_events||num_events<0);++i) { // loop over ASCII input files

#ifdef HAVE_BOOST_IOSTREAMS
     string filename(argv[i]);
     boost::iostreams::filtering_istream ascii_in;
     if (boost::algorithm::iends_with(filename,".gz"))
       ascii_in.push(boost::iostreams::gzip_decompressor());
     else if (boost::algorithm::iends_with(filename,".bz2"))
       ascii_in.push(boost::iostreams::bzip2_decompressor());
     boost::iostreams::file_source ifile(filename); // open the file
     ascii_in.push(ifile);
#else 
      ifstream ascii_in(argv[i]);
#endif
      // variables that identify the event
      int irun(-1);
      int ievt(-1);
      int eevt(-1);
      
      // temporary varible used to read the data
      double x, y, z, x0, y0;
      int charge, assoc;
      double px, py, pz, curv;
      double a0, z0, phi, cottheta, pt;
      //Int_t iPixel, iBarrel;
      //Int_t nLayer, PhiMod, EtaMod;
      //Int_t PhiIdx, EtaIdx, nStrips;
      Int_t evtindex;
      Int_t barcode;
      Int_t pdgcode;
      Int_t parentID;
      Float_t barcode_frac;

      //Int_t ObjectNumber;
      while(ascii_in&&(itot_evt<num_events||num_events<0)) { // main loop to read the events    
	 // define variable to read the row
	 string line;
	 if (!getline(ascii_in,line)) {
	   cout << "*** End of file reached after " << itot_evt << " evts" << endl;
	   break;
	 }
	 istringstream sin(line);

	 char charflag;
	 sin >> charflag;

	 switch(charflag) {
	 case 'R': // declare the run number
	    sin >> irun;
	    break;
	 case 'F': // event start
	    if (ievt != eevt) {
	       cerr << "*** Error: Event #" << ievt << " started before the #" << eevt << " ends" << endl;
	       return -2;
	    }
	    sin >> ievt;
	    

	    // start to populate the new event
	    iPatTracks->Delete();
	    TruthTracks->Delete();
//  	    ipat_refs->Clear();
//  	    truth_refs->Clear();
	    n_iPat = 0;
	    n_Truth = 0;
	    // TO FINISH
       
	    cout << "\r Evt #" << setw(7) << ievt << '(' << itot_evt << ')' << flush;
	    break;
	 case 'L': // end of the event block
	    sin >> eevt;
	    if (ievt != eevt) { // problem
	       cout << endl;
	       cerr << "*** Error: Event #" << eevt << " closed before the #" << ievt << endl;
	       return -2;
	    }
	    tree->Fill();
	    ++itot_evt;
	    break;
	 case 'H': // space-point hit
	    // TODO
	    break;
	 case 'S': // raw hit
	    // TODO
	    break;
	 case 'E': // Offline reconstruction tracks, or FTK tracks
	   sin >> a0 >> z0 >> phi >> cottheta >> pt;
	   sin >> evtindex >> barcode >> barcode_frac;

	    if (phi>TMath::Pi()) phi -= TMath::Pi();
	    else if (phi<-TMath::Pi()) phi += TMath::Pi();

	    rectrack = new((*iPatTracks)[n_iPat++]) FTKTrack();
	    rectrack->setIP(a0);
	    rectrack->setZ0(z0);
	    rectrack->setPhi(phi);
	    rectrack->setCotTheta(cottheta);
	    rectrack->setHalfInvPt(.5e-3/pt);
	    rectrack->setBarcode(barcode);
	    rectrack->setBarcodeFrac(barcode_frac);
	    rectrack->setTrackID(evtindex);
	    break;
	 case 'T': // truth tracks
	    sin >> x >> y >> z >> charge;
	    sin >> px >> py >> pz >> pdgcode;
	    sin >> assoc >> barcode_frac;
	    sin >> evtindex >> barcode >> parentID;
	    if (!sin) barcode = -1; // if has in error the filed doesn't exist

	    truthtrack = new((*TruthTracks)[n_Truth++]) FTKTrack();
	    if (!sin) { 
	       // this error is because the line don't have the pdgcode
	       pdgcode = 0;
	    }

	    // convertion from cartesian to ftksim parameters of track
	    x0 = x - charge*py;
	    y0 = y + charge*px;

	    curv = charge / TMath::Sqrt(px*px+py*py);

	    a0 = TMath::Sqrt(x0*x0+y0*y0)*abs(charge)/charge-1/curv;

	    // now redifine curvature as semi-curvature
	    curv /= 2;

	    phi = atan2(-x0*abs(charge)/charge,y0*abs(charge)/charge);

	    cottheta = pz/sqrt(px*px+py*py);

	    truthtrack->setIP(a0);
	    truthtrack->setHalfInvPt(curv);
	    truthtrack->setPhi(phi);
	    truthtrack->setCotTheta(cottheta);
	    truthtrack->setZ0(z);
	    truthtrack->setBankID(assoc);
	    truthtrack->setRoadID(parentID);
	    truthtrack->setBarcode(barcode);
	    truthtrack->setBarcodeFrac(barcode_frac);
	    truthtrack->setTrackID(evtindex);
	    break;
	 default:
	    cerr << "--- Warning: " << charflag << " informations are not converted" << endl;
	    break;
	 }

      } // end main loop to read the events    
   } // end loop over ASCII input files

   // save the tree and close the file
   tree->Write();

   ofile->Close();

   cout << endl;
   // exit
   return 0;
}
