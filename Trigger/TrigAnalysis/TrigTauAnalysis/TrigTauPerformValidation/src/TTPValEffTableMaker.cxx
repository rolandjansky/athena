/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;
#include "TrigTauPerformValidation/TTPValEffTableMaker.h"

TTPValEffTableMaker::TTPValEffTableMaker (TrigTauBugFix::POOLFILE poolfile, TrigTauBugFix::TTPVERSION version, bool debug1) 
  : bugfix(version , poolfile) {

  filenameSet = false;
  offlineType[0] = TrigTauMCTauList::NONE;
  offlineType[1]= TrigTauMCTauList::TAU1P3P;
  offlineType[2]= TrigTauMCTauList::TAUREC;
  offlineType[3]= TrigTauMCTauList::EITHERTAU;
  offlineType[4]= TrigTauMCTauList::BOTHTAU;
  offlineType[5] = TrigTauMCTauList::TAUCUTSAFELOOSE;
  offlineType[6] = TrigTauMCTauList::TAUCUTSAFEMEDIUM;
  offlineType[7] = TrigTauMCTauList::TAUCUTSAFETIGHT;
  offlineType[8] = TrigTauMCTauList::TAULLHLOOSE;
  offlineType[9] = TrigTauMCTauList::TAULLHMEDIUM;
  offlineType[10] = TrigTauMCTauList::TAULLHTIGHT;
  
  initialize(debug1);
  return;
}

TTPValEffTableMaker::TTPValEffTableMaker (TString filename1, TrigTauBugFix::POOLFILE poolfile, TrigTauBugFix::TTPVERSION version, bool debug1) 
  : bugfix(version , poolfile) {
  
  setFile (filename1);
  filenameSet = true;
  offlineType[0] = TrigTauMCTauList::NONE;
  offlineType[1]= TrigTauMCTauList::TAU1P3P;
  offlineType[2]= TrigTauMCTauList::TAUREC;
  offlineType[3]= TrigTauMCTauList::EITHERTAU;
  offlineType[4]= TrigTauMCTauList::BOTHTAU;
  offlineType[5] = TrigTauMCTauList::TAUCUTSAFELOOSE;
  offlineType[6] = TrigTauMCTauList::TAUCUTSAFEMEDIUM;
  offlineType[7] = TrigTauMCTauList::TAUCUTSAFETIGHT;
  offlineType[8] = TrigTauMCTauList::TAULLHLOOSE;
  offlineType[9] = TrigTauMCTauList::TAULLHMEDIUM;
  offlineType[10] = TrigTauMCTauList::TAULLHTIGHT;
  initialize(debug1);
  return;

}


void TTPValEffTableMaker::setFile ( TString filename1) {
  
  if (!filenameSet ) {
    filename = filename1;
    tree = new TChain("eventTree");
    tree->Add(filename);
  }
  else {
    cout << "WARNING: File already defined!! Overwriting old file settings...." << endl;
    filename = filename1;
    tree = new TChain("eventTree"); //memory leak
    tree->Add(filename);
  }
  filenameSet = true;
}


void TTPValEffTableMaker::setChain ( TChain * tree1 ) {

  if ( filenameSet ) {
    cout << "WARNING: Tree already defined!! Overwriting tree...." << endl;
    tree = tree1;
  }
  else {
    tree = tree1;
    if (debug) cout << " Chain is set..." << endl; 
 }
  filenameSet = true;
}

void TTPValEffTableMaker::initialize ( bool debug1)  {

  maxVisEta = 2.5;
  minLeadingPt = 6.0;
  setDebug(debug1);
  setSignal(true);
  menu = TrigTauFlags::TAU16;
  cutLevel = TrigTauSelectionManager::LOOSE;
  menuVersion = TrigTauSelectionManager::MENU1560;
  chainType = TrigTauFlags::SITRK;
  return;
}


void TTPValEffTableMaker::analyze() {

  if (!filenameSet) {
    cout << "ERROR: Please set filename! Use TTPValEffTableMaker::setFile(TString filename)." << endl;
    return;
  }

 //set counter to zero and initialize MCTruth analyzer
  for (unsigned int i =0 ; i!=nMatching ; ++i)
    for (unsigned int j=0; j!=nProngType; ++j) {
      for(unsigned int k=0; k!=5; ++k)
	  counter[i][j][k] = 0;
      analyzer[i][j].truthList.setMaxVisEta(maxVisEta);
      analyzer[i][j].truthList.setMinLeadingPt(minLeadingPt);
      analyzer[i][j].truthList.setMinVisPt(menu);
      if ( j == 0 ) analyzer[i][j].truthList.select1Prong();
      if ( j == 1 ) analyzer[i][j].truthList.select3Prong();
      analyzer[i][j].truthList.setRecoMatchdR(0.2);
      //set L1Met cut if desired
      analyzer[i][j].trigMan.setL1MET(0.);
      analyzer[i][j].trigMan.setMenuVersion(menuVersion );
      analyzer[i][j].trigMan.setTauMenu(menu, cutLevel);
      analyzer[i][j].chainList.setRoiWordMatching(false);
      analyzer[i][j].chainList.setHLTAuthor(chainType);
    }

  selmanager.setMenuVersion(menuVersion);
  selmanager.setTauMenu(menu, cutLevel);

  if ( debug ) cout << "Starting Analysis..." << endl;

   TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);
  //tree->SetBranchStatus("mcEvent*", 0);

  // Loop over all events
  nev = int(tree->GetEntries());
  if ( debug ) cout << "Starting eventloop..." << endl;
  //if ( nev > 10 ) nev = 10;
  for(int iev = 0; iev<nev; ++iev) {
    if ( debug ) cout << "============== Checking event " << iev << " ==================" <<  endl;
    event->reset();
    tree->GetEvent(iev);
    bugfix.fix(event);
    
    if ( debug ) cout << "Starting analysis of event for all matchings and types..." << endl;

    bool goodEvent = true;
    for (unsigned int i =0; i!=nMatching; ++i)
      for (unsigned int j=0; j!=nProngType; ++j) {
	if ( debug  && i==2 && j==2 ) cout << "Analyze event " << iev << " with matching type " << i << " and prong type " << j << "." << endl;
	goodEvent = analyzeEvent(event, offlineType[i], i, j);
      }
  }  // end loop over events
}


bool TTPValEffTableMaker::analyzeEvent (TrigTauEvent * event, TrigTauMCTauList::OFFLINESELECTION offlineType, int i, int j )   {

  if (debug && i==2 && j==2 ) cout << "Starting analyzeEvent...." << endl;

    analyzer[i][j].analyze(event, signal, offlineType);

    // ============ Select type of events (e.g. signal) ================
    if( signal ) {
      analyzer[i][j].truthList.fillPureList(&event->mcEvent);
      analyzer[i][j].truthList.fillGoodList();
      analyzer[i][j].truthList.requireRecoMatch(*event, offlineType);
      if ( debug && i==2 && j==2 && analyzer[i][j].truthList.nMCTauGood() ==0 ) cout << "Event contains no good taus! " << endl;
      if(  analyzer[i][j].truthList.nMCTauGood() ==0 ) return false;
    }
  
    // loop over taus after fiducial cuts

    if ( debug && i==2 && j==2 ) cout << "Event contains " << analyzer[i][j].truthList.nMCTauGood() << " good tau!" << endl;
    for( unsigned int iTrue = 0; iTrue <  analyzer[i][j].truthList.nMCTauGood(); ++iTrue )  //loop over all true taus
      {
	const TrigTauMCTau * truth = & analyzer[i][j].truthList.mcTauGoodEntry(iTrue);    //select tau
	counter[i][j][4]++;

	//if ( debug && analyzer[i][j].chainList == 0 ) cout << "ERROR: No chain list found!!" << endl;
	//if ( debug && i==2 && j==2 ) cout << "L1 chains: " << analyzer[i][j].chainList.nPassL1Chains() << endl; 
        unsigned int nTrigChains = analyzer[i][j].chainList.nTrigChains() ;
	if (debug && i==2 && j==2 ) cout << "Value of nTrigChains: " << nTrigChains << endl;
	if ( nTrigChains == 0 ) continue;
        


        for( unsigned int ichain = 0; ichain < nTrigChains; ++ichain) {
           if ( debug && i==2 && j==2 ) cout << "Starting chain " << ichain << "!" << endl;
           const TrigTauTrigChain *ch = &analyzer[i][j].chainList.trigChainEntry(ichain);
 	   const TrigTauL1Object* l1object = ch->L1;
           const TrigTauL2Object* l2object = ch->L2;
	   const TrigTauL2Object* l2calobject = ch->L2Cal;
           const TrigTauL2Object* l2trkobject = ch->L2Trk;
           const TrigTauRecEFObject* efobject = ch->EF;
	   unsigned int currentWord;
	   if ( l1object == 0 ) continue;

	   if( truth->DeltaR(*l1object)<0.2 ) {
	     currentWord = selmanager.l1Sel.cutWord(*l1object);
	     if (currentWord != 0 ) continue;
	     counter[i][j][0]++;
           }
	   else continue;

           //check Level 2 (including Trk and Cal)
           if (l2object == 0 ) continue;

           if( truth->DeltaR(*l2object)<0.2 ) {
		    currentWord = selmanager.l2CalSel.cutWord(*l2calobject);
		    if ( currentWord !=0 ) {
		      continue;
		    }
		    counter[i][j][1]++;
		    currentWord = selmanager.l2TrkSel.cutWord(*l2trkobject);
                    if ( currentWord !=0  ) continue;
		    counter[i][j][2]++;
           }
	   else continue;


           //check EF
	   if (efobject == 0 ) {
             if ( debug  && i==2 && j==2 ) cout << "EF object is empty; go to next chain" << endl;
             continue;
	   }
           if( truth->DeltaR(*efobject)<0.2 ) {
		   currentWord = selmanager.efSel.cutWord(*efobject);
                   if (currentWord != 0) continue;
		    counter[i][j][3]++;

           }
        } // end chain loop
      } // end loop over truth
    return true;
} //end function




void TTPValEffTableMaker::print (int which) {

  
  TrigTauFlags menu1;

  ofstream outfile;
  outfile.open("effTable.txt");

  switch (which) {

  case 0 :
    
    outfile << "Printing ABSOLUTE Efficiencies: " << endl;
    outfile << "Menu " << menu1.menuName(menu) <<  "\tNumber of total Events: " << nev <<  endl;

    outfile << "For matching: \n 0 - Tau1P3P \n 1 - TAUREC \n 2 - EITHERTAU \n 3 - BOTHTAU \n 4 - TauCutSafeLoose \n 5 - TauCutSafeMedium \n 6 - TauCutSafeTight \n 7 - TauLlhLoose \n 8 - TauLlhMedium \n 9 - TauLlhTight \n 10 - NONE " << endl;


    outfile << "\n Number of truth events: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][4]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }

    outfile << "\n For L1: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][0])/float(counter[i][j][4]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }

    outfile << "\n For L2Calo: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][1])/float(counter[i][j][4]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }

    outfile << "\n For L2Trk: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][2])/float(counter[i][j][4]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }
  

    outfile << "\n For EF: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][3])/float(counter[i][j][4]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }

    outfile << "\nSummary: \"All\" decays and \"TauCutSafeMedium\" offline matching: " << endl;
    outfile << "\n\t\t\t\t & Level1 \t\t & L2Calo \t\t & L2Trk \t\t & EF \\\\" << endl;
      outfile << "Relative Efficiencies:\t\t & "<< setprecision(3) << float(counter[6][2][0])/float(counter[6][2][4])
	 << "\t\t & " << float(counter[6][2][1])/float(counter[6][2][0])
	 << "\t\t & " << float(counter[6][2][2])/float(counter[6][2][1])
	   << "\t\t & " << float(counter[6][2][3])/float(counter[6][2][2]) << " \\\\" << endl;
    outfile << "Absolute Efficiencies: \t\t & "<< setprecision(3) << float(counter[6][2][0])/float(counter[6][2][4])
	 << "\t\t & " << float(counter[6][2][1])/float(counter[6][2][4])
	 << "\t\t & " << float(counter[6][2][2])/float(counter[6][2][4])
	 << "\t\t & " << float(counter[6][2][3])/float(counter[6][2][4]) << " \\\\" << endl;
							     
		     

    break;

  

  case 1:
    
    outfile << "Printing RELATIVE Efficiencies: " << endl;
    outfile << "Menu " << menu1.menuName(menu) <<  "\tNumber of total Events: " << nev  <<  endl;

    outfile << "For matching: \n 0 - Tau1P3P \n 1 - TAUREC \n 2 - EITHERTAU \n 3 - BOTHTAU \n 4 - TauCutSafeLoose \n 5 - TauCutSafeMedium \n 6 - TauCutSafeTight \n 7 - TauLlhLoose \n 8 - TauLlhMedium \n 9 - TauLlhTight \n 10 - NONE " << endl;

    outfile << "\n Number of truth events: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][4]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }

    outfile << "\n For L1: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][0])/float(counter[i][j][4]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }

    outfile << "\n For L2Calo: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][1])/float(counter[i][j][0]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }

    outfile << "\n For L2Trk: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][2])/float(counter[i][j][1]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }
  

    outfile << "\n For EF: " << endl;
    outfile << "\t\t & 1Pr \t\t\t & 3Pr \t\t\t & All \\\\" << endl;
    for (unsigned int i = 0; i!=nMatching; ++i) {
      outfile << offlineType[i];

      for (unsigned int j = 0; j!=nProngType; ++j) {
	if (counter[i][j][4]!=0) outfile << "\t\t & " << setprecision(3) << float(counter[i][j][3])/float(counter[i][j][2]);
	else outfile << "\t\t undef.";
      }
	outfile << " \\\\\n";
    }


    outfile << "\nSummary: \"All\" decays and \"TauCutSafeMedium\" offline matching: " << endl;
    outfile << "\n\t\t\t\t & Level1 \t\t & L2Calo \t\t & L2Trk \t\t & EF \\\\" << endl;
      outfile << "Relative Efficiencies:\t\t & "<< setprecision(3) << float(counter[6][2][0])/float(counter[6][2][4])
	 << "\t\t & " << float(counter[6][2][1])/float(counter[6][2][0])
	 << "\t\t & " << float(counter[6][2][2])/float(counter[6][2][1])
	   << "\t\t & " << float(counter[6][2][3])/float(counter[6][2][2]) << " \\\\" << endl;
    outfile << "Absolute Efficiencies: \t\t & "<< setprecision(3) << float(counter[6][2][0])/float(counter[6][2][4])
	 << "\t\t & " << float(counter[6][2][1])/float(counter[6][2][4])
	 << "\t\t & " << float(counter[6][2][2])/float(counter[6][2][4])
	 << "\t\t & " << float(counter[6][2][3])/float(counter[6][2][4]) << " \\\\"  << endl;


    break;
  }

  return;

}

void TTPValEffTableMaker::printCSC(TPostScript & ps, bool relEff) {

  TrigTauFlags menu1;

  TH1F * L1Eff = new TH1F("TTPValEffTableMaker_" + menu1.menuName(menu) + "_L1", "L1 Relative Efficiencies", nMatching*nProngType+1, 0, nMatching*nProngType+1);
  TH1F * L2CaloEff = new TH1F("TTPValEffTableMaker_" + menu1.menuName(menu) + "_L2Calo", "L2Calo Relative Efficiencies", nMatching*nProngType+1, 0, nMatching*nProngType+1);
  TH1F * L2TrkEff = new TH1F("TTPValEffTableMaker_" + menu1.menuName(menu) + "_L2Trk", "L2Trk Relative Efficiencies", nMatching*nProngType+1, 0, nMatching*nProngType+1);
  TH1F * EFEff = new TH1F("TTPValEffTableMaker_" + menu1.menuName(menu) + "_EF", "EF Relative Efficiencies", nMatching*nProngType+1, 0, nMatching*nProngType+1);


  switch (relEff) {

  case 1:

    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L1_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L1Eff->SetBinContent(i*nProngType+j+1, float(counter[i][j][0])/float(counter[i][j][4]));
	L1Eff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L1Eff->LabelsOption("d","X");
      }
    }
   
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L2Calo_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L2CaloEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][1])/float(counter[i][j][0]));
	L2CaloEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L2CaloEff->LabelsOption("d","X");
      }
    }
    
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L2Trk_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L2TrkEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][1])/float(counter[i][j][1]));
	L2TrkEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L2TrkEff->LabelsOption("d","X");
      }
    }

    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "EF_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) EFEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][3])/float(counter[i][j][2]));
	EFEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	EFEff->LabelsOption("d","X");
      }
    }
  
    break;

    case 0:

      L1Eff->SetName("L1 Absolute Efficiencies");
      for (unsigned int i = 0; i!=nMatching; ++i) {
	//outfile << offlineType[i];
	for (unsigned int j = 0; j!=nProngType; ++j) {
	  TString label = "L1_";
	  label += i;
	  if (j==0) label += "_1Prong";
	  if (j==1) label += "_3Prong";
	  if (j==2) label += "_AllDecay";
	  if (counter[i][j][4]!=0) L1Eff->SetBinContent(i*nProngType+j+1, float(counter[i][j][0])/float(counter[i][j][4]));
	  L1Eff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L1Eff->LabelsOption("d","X");
	}
      }

    L2CaloEff->SetName("L2Calo Absolute Efficiencies");
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L2Calo_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L2CaloEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][2])/float(counter[i][j][4]));
	L2CaloEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L2CaloEff->LabelsOption("d","X");
      }
    }

    L2TrkEff->SetName("L2Trk Absolute Efficiencies");
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L2Trk_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L2TrkEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][1])/float(counter[i][j][4]));
	L2TrkEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L2TrkEff->LabelsOption("d","X");
      }
    }

    EFEff->SetName("EF Absolute Efficiencies");
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "EF_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) EFEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][3])/float(counter[i][j][4]));
	EFEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	EFEff->LabelsOption("d","X");
      }
    }
    break;
  };

  TCanvas * c1 = (TCanvas *)gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);
  ps.On();

  L1Eff->GetYaxis()->SetTitle("L1 efficiency");
  L1Eff->Draw();
  c1->Update();

  L2CaloEff->GetYaxis()->SetTitle("L2Calo efficiency");
  L2CaloEff->Draw();
  c1->Update();

  L2TrkEff->GetYaxis()->SetTitle("L2Trk efficiency");
  L2TrkEff->Draw();
  c1->Update();

  EFEff->GetYaxis()->SetTitle("EF efficiency");
  EFEff->Draw();
  c1->Update();

  ps.Off();
}

void TTPValEffTableMaker::update(TString filename, bool relEff) {

  TFile f(filename, "update");

  f.cd();
  f.mkdir("EffTable");
  f.cd("EffTable");


  TrigTauFlags menu1;

  TH1F * L1Eff = new TH1F("TTPValEffTableMaker_" + menu1.menuName(menu) + "_L1", "L1 Relative Efficiencies", nMatching*nProngType+1, 0, nMatching*nProngType+1);
  TH1F * L2CaloEff = new TH1F("TTPValEffTableMaker_" + menu1.menuName(menu) + "_L2Calo", "L2Calo Relative Efficiencies", nMatching*nProngType+1, 0, nMatching*nProngType+1);
  TH1F * L2TrkEff = new TH1F("TTPValEffTableMaker_" + menu1.menuName(menu) + "_L2Trk", "L2Trk Relative Efficiencies", nMatching*nProngType+1, 0, nMatching*nProngType+1);
  TH1F * EFEff = new TH1F("TTPValEffTableMaker_" + menu1.menuName(menu) + "_EF", "EF Relative Efficiencies", nMatching*nProngType+1, 0, nMatching*nProngType+1);


  switch (relEff) {

  case 1:

    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L1_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L1Eff->SetBinContent(i*nProngType+j+1, float(counter[i][j][0])/float(counter[i][j][4]));
	L1Eff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L1Eff->LabelsOption("v","X");
      }
    }

   
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L2Calo_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L2CaloEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][1])/float(counter[i][j][0]));
	L2CaloEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L2CaloEff->LabelsOption("v","X");
      }
    }


    
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L2Trk_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L2TrkEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][1])/float(counter[i][j][1]));
	L2TrkEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L2TrkEff->LabelsOption("v","X");
      }
    }


    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "EF_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) EFEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][3])/float(counter[i][j][2]));
	EFEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	EFEff->LabelsOption("v","X");
      }
    }
  
    break;


    case 0:

      L1Eff->SetName("L1 Absolute Efficiencies");
      for (unsigned int i = 0; i!=nMatching; ++i) {
	//outfile << offlineType[i];
	for (unsigned int j = 0; j!=nProngType; ++j) {
	  TString label = "L1_";
	  label += i;
	  if (j==0) label += "_1Prong";
	  if (j==1) label += "_3Prong";
	  if (j==2) label += "_AllDecay";
	  if (counter[i][j][4]!=0) L1Eff->SetBinContent(i*nProngType+j+1, float(counter[i][j][0])/float(counter[i][j][4]));
	  L1Eff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L1Eff->LabelsOption("v","X");
	}
      }

    L2CaloEff->SetName("L2Calo Absolute Efficiencies");
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L2Calo_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L2CaloEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][2])/float(counter[i][j][4]));
	L2CaloEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L2CaloEff->LabelsOption("v","X");
      }
    }

    L2TrkEff->SetName("L2Trk Absolute Efficiencies");
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "L2Trk_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) L2TrkEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][1])/float(counter[i][j][4]));
	L2TrkEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	L2TrkEff->LabelsOption("v","X");
      }
    }

    EFEff->SetName("EF Absolute Efficiencies");
    for (unsigned int i = 0; i!=nMatching; ++i) {
      //outfile << offlineType[i];
      for (unsigned int j = 0; j!=nProngType; ++j) {
	TString label = "EF_";
	label += i;
	if (j==0) label += "_1Prong";
	if (j==1) label += "_3Prong";
	if (j==2) label += "_AllDecay";
	if (counter[i][j][4]!=0) EFEff->SetBinContent(i*nProngType+j+1, float(counter[i][j][3])/float(counter[i][j][4]));
	EFEff->GetXaxis()->SetBinLabel(i*nProngType+j+1, label);
	EFEff->LabelsOption("v","X");
      }
    }


    break;

  };

  L1Eff->Write();
  L2CaloEff->Write();
  L2TrkEff->Write();
  EFEff->Write();
  f.Close();
}
