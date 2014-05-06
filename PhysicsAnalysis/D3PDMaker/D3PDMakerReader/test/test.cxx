/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: test.cxx 354164 2011-03-24 17:10:36Z krasznaa $

// STL include(s):
#include <iostream>

// ROOT include(s):
#include <TFile.h>
#include <TTree.h>

// D3PDReader include(s):
#include "EventInfoD3PDObject.h"
#include "MuonROID3PDObject.h"

/**
 * Small example of how to use some of the generated classes for some simple
 * tasks. Used for code testing at the moment...
 *
 * To be able to compile the code, you first have to run the share/CodeGenerator_jobOptions.py
 * job.
 */
int main() {

   // Create the D3PDReader objects:
   Long64_t master = 0;
   D3PDReader::EventInfoD3PDObject event( master );
   D3PDReader::MuonROID3PDObject muroi( master );

   // And one which will only write information:
   D3PDReader::MuonROID3PDObject selmuroi( "sel_trig_L1_mu_" );

   // Open an input file:
   TFile* file = TFile::Open( "/pool0/data10_7TeV/D3PD/user.akraszna.data10_7TeV.periodF.physics_Muons.PhysCont.MuctpiD3PD.t0pro04_v01.ver01.100908135131/user.akraszna.000049.D3PD._00001.root" );
   if( ! file ) {
      std::cerr << "Couldn't open file!" << std::endl;
      return 1;
   }

   // Access the D3PD TTree inside of it:
   TTree* tree = dynamic_cast< TTree* >( file->Get( "muctpi" ) );
   if( ! tree ) {
      std::cerr << "Couldn't find TTree!" << std::endl;
      return 1;
   }

   // Connect the D3PDReader objects to this new TTree:
   event.ReadFrom( tree );
   muroi.ReadFrom( tree );

   // Open an output file:
   TFile* ofile = TFile::Open( "output_test.root", "RECREATE" );
   ofile->cd();

   // Create an output tree:
   TTree* otree1 = new TTree( "muctpiDiMu", "Selected MuctpiD3PD tree" );
   otree1->SetAutoSave( 100000000 );
   TTree::SetBranchStyle( 1 );
   otree1->SetDirectory( ofile );

   // Create another output tree:
   TTree* otree2 = new TTree( "muctpiTriMu", "Selected MuctpiD3PD tree" );
   otree2->SetAutoSave( 100000000 );
   TTree::SetBranchStyle( 1 );
   otree2->SetDirectory( ofile );

   // Activate some of the branches for writing:
   event.SetActive();
   muroi.SetActive();
   selmuroi.SetActive( kTRUE, "^thr.*|^n$|^pt$|^eta$|^phi$" );

   // Tell the objects that they should write themselves out into the two created
   // TTrees:
   event.WriteTo( otree1 );
   muroi.WriteTo( otree1 );
   selmuroi.WriteTo( otree1 );
   event.WriteTo( otree2 );
   muroi.WriteTo( otree2 );
   selmuroi.WriteTo( otree2 );

   // Loop over the events:
   for( master = 0; master < tree->GetEntries(); ++master ) {

      // Print some information:
      if( ! ( master % 5000 ) ) {
         std::cout << "Run/Event numbers: " << event.RunNumber() << " / "
                   << event.EventNumber() << std::endl;
         std::cout << "Number of muon RoIs in the event: " << muroi.n() << std::endl;

         //
         // There are two ways of accessing the individual muon ROIs:
         //

         // 1st method:
         std::cout << "Pt of the Muon ROIs (method 1): [";
         for( Int_t i = 0; i < muroi.n(); ++i ) {
            std::cout << ( *muroi.pt() )[ i ];
            if( i + 1 != muroi.n() ) {
               std::cout << ", ";
            }
         }
         std::cout << "]" << std::endl;

         // 2nd method:
         std::cout << "Pt of the Muon ROIs (method 2): [";
         for( Int_t i = 0; i < muroi.n(); ++i ) {
            std::cout << muroi[ i ].pt();
            if( i + 1 != muroi.n() ) {
               std::cout << ", ";
            }
         }
         std::cout << "]" << std::endl;
      }

      // Write all LVL1 muon RoIs passing threshold >=2 into the output trees:
      selmuroi.Clear(); // The container has to be cleared first...
      for( Int_t i = 0; i < muroi.n(); ++i ) {
         if( muroi[ i ].thrNumber() >= 2 ) {
            selmuroi += muroi[ i ];
         }
      }

      // Select the events with at least 2 LVL1 muon RoIs:
      if( muroi.n() > 1 ) {
         event.ReadAllActive();
         muroi.ReadAllActive();
         otree1->Fill();
      }
      // Select the events with at least 3 LVL1 muon RoIs:
      if( muroi.n() > 2 ) {
         event.ReadAllActive();
         muroi.ReadAllActive();
         otree2->Fill();
      }
   }

   // Close the input file:
   file->Close();
   delete file;

   // Write out the TTrees:
   otree1->Write();
   otree1->SetDirectory( 0 );
   delete otree1;
   otree2->Write();
   otree2->SetDirectory( 0 );
   delete otree2;

   // Close the output file:
   ofile->Close();
   delete ofile;

   return 0;
}
