#define NSWstudies_cxx
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TH3.h>
#include <cmath>
#include <iostream>

#include "../efficiency.C"

#include "NSWstudies_match.h"
#include "NSWstudies.h"

using namespace std;

void validateHits(Flocalize_collection& Hits, vector<bool>* insideBounds, vector<int>* pdgCodes);
void init_hists (vector< TH1I* >& hist_vec, bool isMM, string datatype, string matchedwith);
bool doEvt (int evtnr);
void write_and_delete (vector< TH1I* > vec);
void init_hist_pull (vector< TH1F*>& hist_pull);

//Inputs
int min_mismatched_channel = 0;
int max_diff = 3;

// log settings
bool quiet = true;
bool printDigits = false;
bool printHits = false;

// Helpers in development:
// Validate hits beforehand
bool digi_test = false;
// Does the electron check do anything?
bool doElectronCheck = true;


// To test two data objects versus each other change the input of match_Hits_Digits and fillHists functions!!
void NSWstudies::Loop()
{
   bool doMM = 1;
   bool dosTGC = 1;

   TFile *outFile = new TFile("RootFiles/NSWVal_Hists.root", "recreate");
   vector< TH1I* > hist_MM_digits;
   vector< TH1I* > hist_MM_hits;
   vector< TH1I* > hist_sTGC_digits;
   vector< TH1I* > hist_sTGC_hits;
   TH2D *hist_MM_global_digits = new TH2D ("MM_Global_pos_mismatched_digits", "MM_Global_pos_mismatched_digits", 50, -6000., 6000., 50, -6000., 6000.);
   TH2D *hist_MM_global_hits = new TH2D ("MM_Global_pos_mismatched_hits", "MM_Global_pos_mismatched_hits", 50, -6000., 6000., 50, -6000., 6000.);
   TH2D *hist_sTGC_global_digits = new TH2D ("sTGC_Global_pos_mismatched_digits", "sTGC_Global_pos_mismatched_digits", 50, -6000., 6000., 50, -6000., 6000.);
   TH2D *hist_sTGC_global_hits = new TH2D ("sTGC_Global_pos_mismatched_hits", "sTGC_Global_pos_mismatched_hits", 50, -6000., 6000., 50, -6000., 6000.);

   vector< TH1F* > hist_pull;
   init_hist_pull(hist_pull);

//   In a ROOT session, you can do:
//      root> .L NSWstudies.C
//      root> NSWstudies t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entriess
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   //DEBUG: look only at n events
   Long64_t firstentry = 0;
   //nentries = 100;

   Long64_t jentry;
   Long64_t nbytes = 0, nb = 0;
   for (jentry=firstentry; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (!doEvt(eventNumber)) {continue;}
      // if (Cut(ientry) < 0) continue;
     
      //only muon events:
      bool allMu = true;
      int mu_pdg = 13;
      for (int pdg : *TruthParticle_Pdg) { allMu &= (abs(pdg) == mu_pdg); }
      if (!allMu) { continue; }
      
      if (!quiet) {
         printf("\n**** Event: %d ****\n", eventNumber);
       /*  printf("\nMM's\n");printf("number of truthparticles: %lu\n", TruthParticle_Pdg->size());
            for (int pdg : *TruthParticle_Pdg) { printf("   namely: %d\n", pdg);}
            for (double Pt : *TruthParticle_Pt) { printf("  Pt: %f\n", Pt); }*/
      }

      if (dosTGC) {
         Flocalize_collection oHits_sTGC ("Hits", Hits_sTGC_off_stationName, Hits_sTGC_off_stationEta, Hits_sTGC_off_stationPhi, Hits_sTGC_off_multiplet, Hits_sTGC_off_gas_gap, Hits_sTGC_off_channel, Hits_sTGC_off_channel_type);
         if (digi_test) validateHits(oHits_sTGC, Hits_sTGC_isInsideBounds, Hits_sTGC_particleEncoding);
         Flocalize_collection oDigits_sTGC ("Digits", Digits_sTGC_stationName, Digits_sTGC_stationEta, Digits_sTGC_stationPhi, Digits_sTGC_multiplet, Digits_sTGC_gas_gap, Digits_sTGC_channel, Digits_sTGC_channel_type);
         Flocalize_collection oSDO_sTGC ("SDOs", SDO_sTGC_stationName, SDO_sTGC_stationEta, SDO_sTGC_stationPhi, SDO_sTGC_multiplet, SDO_sTGC_gas_gap, SDO_sTGC_channel, SDO_sTGC_channel_type);
         Flocalize_collection oRDO_sTGC ("RDOs", RDO_sTGC_stationName, RDO_sTGC_stationEta, RDO_sTGC_stationPhi, RDO_sTGC_multiplet, RDO_sTGC_gas_gap, RDO_sTGC_channel, RDO_sTGC_channel_type);
         Flocalize_collection oPRD_sTGC ("PRDs", PRD_sTGC_stationName, PRD_sTGC_stationEta, PRD_sTGC_stationPhi, PRD_sTGC_multiplet, PRD_sTGC_gas_gap, PRD_sTGC_channel, PRD_sTGC_channel_type);
         match_Hits_Digits(oRDO_sTGC, oPRD_sTGC);
         fillHists(oRDO_sTGC, hist_sTGC_hits);
         fillHists(oPRD_sTGC, hist_sTGC_digits);

         plotError (oPRD_sTGC, hist_pull);
         
         /*/Digits to Hits 2D:
         for (unsigned int k = 0; k < oDigits_sTGC.size(); ++k) {
            int diff = oDigits_sTGC.matchedchannel.at(k) - oDigits_sTGC.channel->at(k);
            if ((oDigits_sTGC.matchedchannel.at(k) < 0 || abs(diff) > 3) && oDigits_sTGC.channel->at(k) > min_mismatched_channel) {
               hist_sTGC_global_digits->Fill(Digits_sTGC_globalPosX->at(k), Digits_sTGC_globalPosY->at(k));
            }
         }
         //Hits to Digits 2D:
         for (unsigned int k = 0; k < oHits_sTGC.size(); ++k) {
            int diff = oHits_sTGC.matchedchannel.at(k) - oHits_sTGC.channel->at(k);
            if ((oHits_sTGC.matchedchannel.at(k) < 0 || abs(diff) > 3) && oHits_sTGC.channel->at(k) > min_mismatched_channel) {
               hist_sTGC_global_hits->Fill(Hits_sTGC_detector_globalPositionX->at(k), Hits_sTGC_detector_globalPositionY->at(k));
            }
         }*/
      }
      if (doMM) {
         // No channel type for MM!
         Flocalize_collection oHits_MM ("Hits", Hits_MM_off_stationName, Hits_MM_off_stationEta, Hits_MM_off_stationPhi, Hits_MM_off_multiplet, Hits_MM_off_gas_gap, Hits_MM_off_channel);
         if (digi_test) validateHits(oHits_MM, Hits_MM_isInsideBounds, Hits_MM_particleEncoding);
         Flocalize_collection oDigits_MM ("Digits", Digits_MM_stationName, Digits_MM_stationEta, Digits_MM_stationPhi, Digits_MM_multiplet, Digits_MM_gas_gap, Digits_MM_channel);
         Flocalize_collection oSDO_MM ("SDOs", SDO_MM_stationName, SDO_MM_stationEta, SDO_MM_stationPhi, SDO_MM_multiplet, SDO_MM_gas_gap, SDO_MM_channel);
         Flocalize_collection oRDO_MM ("RDOs", RDO_MM_stationName, RDO_MM_stationEta, RDO_MM_stationPhi, RDO_MM_multiplet, RDO_MM_gas_gap, RDO_MM_channel);
         Flocalize_collection oPRD_MM ("PRDs", PRD_MM_stationName, PRD_MM_stationEta, PRD_MM_stationPhi, PRD_MM_multiplet, PRD_MM_gas_gap, PRD_MM_channel);
         match_Hits_Digits(oRDO_MM, oPRD_MM);
         fillHists(oRDO_MM, hist_MM_hits);
         fillHists(oPRD_MM, hist_MM_digits);

         plotError (oPRD_MM, hist_pull);

         /*/Digits to Hits 2D:
         for (unsigned int k = 0; k < oDigits_MM.sizes(); ++k) {
            int diff = oDigits_MM.matchedchannel.at(k) - oDigits_MM.channel->at(k);
            if ((oDigits_MM.matchedchannel.at(k) < 0 || abs(diff) > 3) && oDigits_MM.channel->at(k) > min_mismatched_channel) {
               hist_MM_global_digits->Fill(Digits_MM_globalPosX->at(k), Digits_MM_globalPosY->at(k));
            }
         }/
         // //Hits to Digits 2D:
         for (unsigned int k = 0; k < oHits_MM.size(); ++k) {
            int diff = oHits_MM.matchedchannel.at(k) - oHits_MM.channel->at(k);
            if ((oHits_MM.matchedchannel.at(k) < 0 || abs(diff) > 3) && oHits_MM.channel->at(k) > min_mismatched_channel) {
               hist_MM_global_hits->Fill(Hits_MM_detector_globalPositionX->at(k), Hits_MM_detector_globalPositionY->at(k));
            }
         }*/
      }
   }
   // Print efficiencies
   float mm_digits, mm_hits, stgc_digits, stgc_hits, mm_hit_miss, stgc_hit_miss, mm_digit_miss, stgc_digit_miss;
   if (doMM) {
   	mm_digits = hist_MM_digits[1]->GetEntries();
   	mm_hits = hist_MM_hits[1]->GetEntries();
   	mm_hit_miss = hist_MM_hits[3]->GetEntries();
   	mm_digit_miss = hist_MM_digits[3]->GetEntries();
   	std::cout << "MM: Conversion forwards: ";
   	efficiency(mm_hit_miss, mm_hits);
   	std::cout << "MM: Conversion backwards: ";
   	efficiency(mm_digit_miss, mm_digits);
   } 
   if (dosTGC) {
	   stgc_digits = hist_sTGC_digits[1]->GetEntries();
	   stgc_hits = hist_sTGC_hits[1]->GetEntries();
	   stgc_hit_miss = hist_sTGC_hits[3]->GetEntries();
	   stgc_digit_miss = hist_sTGC_digits[3]->GetEntries();
   	std::cout << "sTGC: Conversion forwards: ";
   	efficiency(stgc_hit_miss, stgc_hits);
   	std::cout << "sTGC: Conversion backwards: ";
   	efficiency(stgc_digit_miss, stgc_digits);
   }
   // Bookkeeping: MM: Digits
   write_and_delete(hist_MM_digits);
   if (doMM) hist_MM_global_digits->Write();
   delete hist_MM_global_digits;
   //Hits
   write_and_delete(hist_MM_hits);
   if (doMM) hist_MM_global_hits->Write();
   delete hist_MM_global_hits;
   // sTGC: Digits
   write_and_delete(hist_sTGC_digits);
   if (dosTGC) hist_sTGC_global_digits->Write();
   delete hist_sTGC_global_digits;
   // Hits
   write_and_delete(hist_sTGC_hits);
   if (dosTGC) hist_sTGC_global_hits->Write();
   delete hist_sTGC_global_hits;
   // 
   for (TH1F* _h: hist_pull) {
      _h->Write();
      delete _h;
   }
   // File
   delete outFile;
}

void validateHits(Flocalize_collection& Hits, vector<bool>* insideBounds, vector<int>* pdgCodes) {
	//Function to take out hits which should not be digitized
	bool accept_hit;
	for (unsigned int i = 0; i < Hits.size(); ++i) {
		accept_hit = true;
		// check if hit is inside active volume
		accept_hit &= insideBounds->at(i);
		// check if hit is a muon hit
		accept_hit &= abs(pdgCodes->at(i)) == 13;
		// Alex: For the sTGC wire digits, channel numbers can only go up to 59. However I made it so that hits placed in the dead region are given the channelnumber 63. As 63 isn't a valid channel number, the digit isn't added.
		if (!Hits.isMM) accept_hit &= (Hits.channel_type->at(i) != 2 || Hits.channel->at(i) != 63);
		// Flag hits which should not be digitized
		if (!accept_hit) { Hits.matchedchannel.at(i) = -100; }
	}
}


void NSWstudies::match_Hits_Digits (Flocalize_collection& Hits, Flocalize_collection& Digits) {
	static bool done_once = 0;
	if (!done_once) { printf("Converting %s to %s\n", Hits.name.data(), Digits.name.data()); done_once = 1; } 
   //first level check: sizes
   if (!quiet) printf("About to match, size of Hits: %lu, size of Digits:%lu \n", Hits.size(), Digits.size());

   for (unsigned int i = 0; i < Hits.size(); ++i)
   {
   	if (Hits.matchedchannel.at(i) == -100) { continue; }
      //printf("current i: %d\n", i);
      int nMatch = 0;
      for (unsigned int j = 0; j < Digits.size(); ++j)
      {
         //printf("current j: %d\n", j);
         if (Hits.at(i).isEqual( Digits.at(j) ) )
         {
            nMatch++;
            Hits.update_match(i, Digits.channel->at(j), j);
            Digits.update_match(j, Hits.channel->at(i), i);
            //printf("Found Hits in Digits at Hits index: %d, and Digits index: %d\n", i, j);
         }
      }
      if (quiet) continue;
      //Check for other matches
      if (nMatch > 1 && Digits.isMM) {
         printf("\nWARNING: More than 1 match for MM!, namely: %d matches\n", nMatch);
      }
      if (nMatch == 0) {
         printf("\nWARNING: No match found!\n");
         printf("Hits info: \n");
         Hits.at(i).printInfo();
      }
      //debug
      if (nMatch == 1 ) { 
         //printf("Found single Digit for this Hit!\n");
         //(Hits.at(i).channel == Digits.at(jfound).channel) ? printf("Channels are the same: %d\n", Hits.at(i).channel) : printf("Channels are different: %d for Hit, %d for Digit\n", Hits.at(i).channel, Digits.at(jfound).channel);
      }
   }
   Hits.matchedwith = Digits.name;
	Digits.matchedwith = Hits.name;
   if (printDigits) { printf("\nFull digits info (event: %d): \n", eventNumber); Digits.printInfo(); }
   if (printHits) { printf("\nFull hits info (event: %d): \n", eventNumber); Hits.printInfo(); }
 }

void NSWstudies::fillHists (Flocalize_collection& oData, vector< TH1I* >& hist_vec) {
   if (hist_vec.empty()) { init_hists(hist_vec, oData.isMM, oData.name, oData.matchedwith); }
   TH1I* hist_diff = hist_vec[0];
   TH1I* hist_ndigits = hist_vec[1];
   TH1I* hist_match = hist_vec[2];
   TH1I* hist_missmatch = hist_vec[3];
   TH1I* hist_missmatched_evt = hist_vec[4];
   TH1I* hist_missmatched_chc = hist_vec[5];
   bool hasMissed = false;
   for (unsigned int i = 0; i < oData.size(); ++i) { 
      int diff = oData.matchedchannel.at(i) - oData.channel->at(i);
      if (abs(diff) > 19 ) { diff = -20; /*printf("Matchedchannel more then 20 strips away! Matchedchannel: %d\n", oData.matchedchannel.at(i));*/ }
      //Check if hit inside volume & only muon:
      if (oData.matchedchannel.at(i) == -100) { continue; }
      if (oData.name == "Hits" && !digi_test && doElectronCheck) {
         if (oData.isMM) { 
            bool accept_hit = Hits_MM_isInsideBounds->at(i) * (abs(Hits_MM_particleEncoding->at(i)) == 13);
            //diff = accept_hit * diff; 
            if (!accept_hit) { continue; }
         } else { 
         	// For the wire digits, channel numbers can only go up to 59. However I made it so that hits placed in the dead region are given the channelnumber 63. As 63 isn't a valid channel number, the digit isn't added.
            bool accept_hit = Hits_sTGC_isInsideBounds->at(i) * 
            						(abs(Hits_sTGC_particleEncoding->at(i)) == 13) * 
            						(oData.channel_type->at(i) != 2 || oData.channel->at(i) != 63);
            //diff = accept_hit * diff; 
            if (!accept_hit) { continue; }
         }
      }
      //
      hist_diff->Fill(diff); 
      int nextbin = oData.localize(i);
      const char* eta_sign = (oData.stationEta->at(i) > 0) ? "+" : "-" ;
      int cht = oData.isMM ? -1 : oData.channel_type->at(i);
      TString binName;
      binName.Form("#eta:%s,%s,mp:%d,gg:%d,cht:%d", eta_sign, oData.stationName->at(i).data(), oData.multiplet->at(i), oData.gas_gap->at(i), cht);
      // Note bin numbering for setbinlabel starts at 1
      hist_match->GetXaxis()->SetBinLabel(nextbin + 1, binName);
      hist_missmatch->GetXaxis()->SetBinLabel(nextbin + 1, binName);
      hist_ndigits->GetXaxis()->SetBinLabel(nextbin + 1, binName);
      hist_ndigits->Fill(nextbin);
      //Increment hist by 1 for each matched hit in matchedindices
      //printf("size matchedindices: %d, empty: %d\n", oData.matchedindices.at(i).size() , 1 * oData.matchedindices.at(i).empty());
      for (unsigned int j = 0; j < oData.matchedindices.at(i).size(); ++j) { 
         hist_match->Fill(nextbin);
      }
      if (abs(diff) > max_diff && oData.channel->at(i) > min_mismatched_channel) { 
         hist_missmatch->Fill(nextbin);
         hist_missmatched_chc->Fill(oData.channel->at(i));
         hasMissed = true;
         if (!quiet) { printf("\nFound mismatched digit in event %d. Digit info:\n", eventNumber); oData.at(i).printInfo(); }
      }
   }
   if (hasMissed) {
      hist_missmatched_evt->Fill(eventNumber);
   }
}

void init_hists (vector< TH1I* >& hist_vec, bool isMM, string datatype, string matchedwith) {
   int ndigits;
   const char* obj = datatype.c_str();
   const char* type;
   TString title;
   if (isMM) {
      ndigits = 34;
      type = "MM";
   } else {
      ndigits = 100;
      type = "sTGC";
   }
   title.Form("%s_Nearest_matched(%s)channel_minus_Channel_%s", type, matchedwith.c_str(), obj);
   hist_vec.push_back( new TH1I (title.Data(), title.Data(), 40, -20, 20) );
   hist_vec[0]->GetXaxis()->SetTitle("Difference(strips) (-20 = out of range)");
   title.Form("%s_Occurence_of_%s", type, obj);
   hist_vec.push_back( new TH1I (title.Data(), title.Data(), ndigits, 0, ndigits) );
   title.Form("%s_%s_per_%s",type, obj, matchedwith.c_str());
   hist_vec.push_back( new TH1I (title.Data(), title.Data(), ndigits, 0, ndigits) );
   title.Form("%s_Nr_mismatched_%s", type, obj);
   hist_vec.push_back( new TH1I (title.Data(), title.Data(), ndigits, 0, ndigits) );
   title.Form("%s_Mismatched_events_%s", type, obj);
   hist_vec.push_back( new TH1I (title.Data(), title.Data(), 1000, 0, 1000) );
   title.Form("%s_Mismatched_channel_%s", type, obj);
   hist_vec.push_back( new TH1I (title.Data(), title.Data(), 100, 0, 500) );
}

void NSWstudies::plotError (Flocalize_collection oPRD, vector<TH1F*> hists_pull) {
	bool isMM = oPRD.isMM;

   double sTGC_error, sTGC_locX, sTGC_truthX, sTGC_pull;
   double MM_error, MM_locX, MM_truthX, MM_pull;
   int i, j ,chTy, gg;
   for (i = 0; i < oPRD.size(); ++i) {
	   // assume only one SDO per PRD, if not, select the first one
	   j = oPRD.matchedindices[i][0];
	   if (!isMM) {
	   	// sTGC part
	      sTGC_locX = PRD_sTGC_localPosX->at(i);
	      sTGC_error = sqrt (PRD_sTGC_covMatrix_1_1->at(i));
	      sTGC_truthX = SDO_sTGC_localPosX->at(j);
	      sTGC_pull = (sTGC_locX - sTGC_truthX) / sTGC_error;

	      printf("PRD sTGC channel: %d, matched with SDO channel: %d\n", PRD_sTGC_channel->at(i), SDO_sTGC_channel->at(j));
	      printf("PRD sTGC locX: %f, SDO locx: %f\n\n", sTGC_locX, sTGC_truthX);

	      //printf("sTGC1\n");
	      chTy = oPRD.channel_type->at(i);
	      //printf("sTGC2\n");
	      hists_pull[chTy]->Fill((sTGC_locX - sTGC_truthX));
	      //printf("sTGC3\n");
	      hists_pull[chTy + 3]->Fill(sTGC_pull);

   	} else {

	      // MM part
	      MM_locX = PRD_MM_localPosX->at(i);
	      MM_error = sqrt (PRD_MM_covMatrix_1_1->at(i));
	      MM_truthX = SDO_MM_localPosX->at(j);
	      MM_pull = (MM_locX - MM_truthX) / MM_error;

	      printf("PRD MM channel: %d, matched with SDO channel: %d\n", PRD_MM_channel->at(i), SDO_MM_channel->at(j));
	      printf("PRD MM locX: %f, SDO locx: %f\n\n", MM_locX, MM_truthX);

	      gg = oPRD.gas_gap->at(i) + 5;
	      hists_pull[gg]->Fill((MM_locX - MM_truthX));
	      hists_pull[gg + 4]->Fill(MM_pull);
   	}
   }
}

bool doEvt (int evtnr) {
   //do all events:
   return true;
   //int events_to_investigate[] = { 11, 19, 21, 22, 25 };
   int events_to_investigate[] = {11};
   bool keep_event = false;
   int size = sizeof(events_to_investigate)/sizeof(events_to_investigate[0]);
   for (int i = 0; i < size; ++i) { keep_event |= events_to_investigate[i] == evtnr; }
   return keep_event;
}

void write_and_delete (vector< TH1I* > vec) {
   for (TH1I* _h: vec) {
      _h->Write();
      delete _h;
   }
}


void init_hist_pull (vector< TH1F*>& hist_pull) {
	hist_pull.push_back (new TH1F("sTGC_pad_distance", "sTGC_pad_distance", 200, -200, 200));
   hist_pull.push_back (new TH1F("sTGC_strip_distance", "sTGC_strip_distance", 200, -200, 200));
   hist_pull.push_back (new TH1F("sTGC_wire_distance", "sTGC_wire_distance", 200, -200, 200));
   hist_pull.push_back (new TH1F("sTGC_pad_pull", "sTGC_pad_pull", 200, -100, 100));
   hist_pull.push_back (new TH1F("sTGC_strip_pull", "sTGC_strip_pull", 200, -100, 100));
   hist_pull.push_back (new TH1F("sTGC_wire_pull", "sTGC_wire_pull", 200, -100, 100));

   hist_pull.push_back (new TH1F("MM_gg1_distance", "MM_gg1_distance", 200, -20, 20));
   hist_pull.push_back (new TH1F("MM_gg2_distance", "MM_gg2_distance", 200, -20, 20));
   hist_pull.push_back (new TH1F("MM_gg3_distance", "MM_gg3_distance", 200, -20, 20));
   hist_pull.push_back (new TH1F("MM_gg4_distance", "MM_gg4_distance", 200, -20, 20));
   hist_pull.push_back (new TH1F("MM_gg1_pull", "MM_gg1_pull", 200, -20, 20));
   hist_pull.push_back (new TH1F("MM_gg2_pull", "MM_gg2_pull", 200, -20, 20));
   hist_pull.push_back (new TH1F("MM_gg3_pull", "MM_gg3_pull", 200, -20, 20));
   hist_pull.push_back (new TH1F("MM_gg4_pull", "MM_gg4_pull", 200, -20, 20));

}










