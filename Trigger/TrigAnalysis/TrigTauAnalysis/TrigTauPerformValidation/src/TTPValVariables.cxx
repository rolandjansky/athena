/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
using namespace std;
#include "TrigTauPerformValidation/TTPValVariables.h"



TTPValVariables::TTPValVariables(TrigTauBugFix::TTPVERSION /*version*/, bool debug1) { 
  cout << "WARNING: Filename not specified!! " << endl; 
  initialize(debug1);
  fileDefined = false;
  return; 
}

TTPValVariables::TTPValVariables ( char * filename1, TrigTauBugFix::TTPVERSION version,  bool debug1) 
  : bugfix(version, TrigTauBugFix::RDO)  {
    
  if (debug1) cout << " Creating validation4 object..." << endl; 
  filename = filename1;  
  chain = new TChain("eventTree");
  chain->Add(filename);
  initialize(debug1);
  fileDefined = true;

}

void TTPValVariables::initialize(bool debug1) {

  cout << "INFO: No cuts on events. If cuts are needed, please set them." << endl;
  cout << "INFO: Signal set to false by default." << endl;
  cout << "INFO: plotAllMatchedEvents set to true by default." << endl;

  debug = debug1;
  signal = false;
  maxVisEta = 0.0;
  minLeadingPt = 0.0;
  plotAllMatchedEvents = true;
  nProng = 0;
  menu = TrigTauFlags::TAU20;
  cutLevel = TrigTauSelectionManager::LOOSE;
  offlineType =  TrigTauMCTauList::TAUCUTSAFEMEDIUM;
  menuVersion = TrigTauSelectionManager::MENU1560;
  chainType = TrigTauFlags::SITRK; 
}


void TTPValVariables::setPlotAllEvents (bool newValue) {

  ignoreMatching = newValue;
  if (newValue == true) plotAllMatchedEvents = true;
  return;
}



void TTPValVariables::setFile( char * filename1) {
  filename = filename1;
  chain = new TChain("eventTree"); //potential memory leak
  chain->Add(filename);
  fileDefined = true;
  return;
}


void TTPValVariables::analyze(int number, bool chains, bool l1, bool l2 , bool efid, bool ef) {
  
  if (!fileDefined) {
    cout << "File was not defined and variables not initialized!!" << endl;
    return;
  }
 
  selmanager.setMenuVersion(menuVersion);
  selmanager.setTauMenu(menu, cutLevel);
  if (debug) cout << "Validation object created!!" << endl;


  // MC truth
  analyzer.truthList.setMaxVisEta(maxVisEta);
  //analyzer.truthList.setMinVisPt(menu); // in GeV
  analyzer.truthList.setMinLeadingPt(minLeadingPt); // pt of the leading pt of track is 
  if ( nProng == 1 ) analyzer.truthList.select1Prong();
  else if ( nProng == 2 ) analyzer.truthList.select3Prong();
  analyzer.truthList.setRecoMatchdR(0.2);
  analyzer.trigMan.setL1MET(0.);
  analyzer.trigMan.setMenuVersion(menuVersion );
  analyzer.trigMan.setTauMenu(menu, cutLevel);
  analyzer.chainList.setRoiWordMatching(false);
  analyzer.chainList.setHLTAuthor(chainType);
  
  
  event  = new TrigTauEvent();
  chain->SetBranchAddress("EventBranch", &event);
  chain->SetBranchStatus("*", 1);
  
  // Also set an L1 MET cut if desired
  
  int nev = int(chain->GetEntries());
  if ( nev > number ) nev = number;
  for(int iev = 0; iev<nev; ++iev) {
    analyzeEvent( iev, chains, l1, l2, efid, ef );
  }
}


void TTPValVariables::analyzeEvent(int eventNumber, bool chains, bool l1, bool l2 , bool efid, bool ef) {
  
  event->reset();
  chain->GetEvent(eventNumber);
  bugfix.fix(event);

  cout << "\nAnalyzing EVENT NUMBER " << event->eventNumber << endl;
  cout << "RUN NUMBER\t" << event->runNumber << endl;

  cout << "Number of Objects:" << "\tL1:\t" << event->tauL1.size() << "\tL2:\t" << event->tauL2.size() << "\tEF:\t" << event->tauEF.size();

  if( chains ){
//   analyzer.analyze(event, signal, offlineType);

//   // ============ Select type of events (e.g. signal) ================
//   if( signal ) {
//     if ( analyzer.truthList.nMCTauGood() ==0 ) cout << "Event contains no good taus! " << endl;
//     if(  analyzer.truthList.nMCTauGood() ==0 ) return;
//   }
  
//   // loop over taus after fiducial cuts
  
//   //const TrigTauMCTau * truth = & analyzer.truthList.mcTauGoodEntry(iTrue);    //select tau
  
//   unsigned int nTrigChains = analyzer.chainList.nTrigChains() ;
//   cout << "Number of Chains: " << nTrigChains << endl;
  //if (nTrigChains == 0) return;
  
//   vector<TrigTauTrigChain> chainlist;
//   for (unsigned int ichain = 0; ichain !=nTrigChains; ++ichain) {
//     chainlist.push_back(analyzer.chainList.trigChainEntry(ichain));
//   }
  
//   //sorting algorithm
//   for (int pass=0; pass!=nTrigChains; pass++) {
//     int potentialSmallest = pass;  // assume this is smallest
//     if (debug) cout << "\nPotential Smallest: " << potentialSmallest << " with " << chainlist[potentialSmallest].L1->Phi() << endl;
//     //--- Look over remaining elements to find smallest.
//     for (int i=pass+1; i<nTrigChains; i++) {
//       if (chainlist[i].L1->Phi() < chainlist[potentialSmallest].L1->Phi() ) {
// 	//--- Remember index for latter swap.
// 	potentialSmallest = i;
// 	if (debug) cout << "New Potential Smallest: " << potentialSmallest << " with " << chainlist[potentialSmallest].L1->Phi() << endl;
//       }
//     }
    
//     //--- Swap smallest remaining element
//     TrigTauTrigChain temp = chainlist[pass];
//     chainlist[pass] = chainlist[potentialSmallest];
//     chainlist[potentialSmallest] = temp;
//   }
  //starting printing of individual chains
//   for( unsigned int ichain = 0; ichain != nTrigChains; ++ichain) {
//     cout << "\nStarting chain " << ichain << "!" << endl;
//     TrigTauTrigChain *ch = &chainlist[ichain];
//     const TrigTauL1Object* l1object = ch->L1;
//     const TrigTauL2Object* l2object = ch->L2;
//     const TrigTauL2Object* l2calobject = ch->L2Cal;
//     const TrigTauL2Object* l2trkobject = ch->L2Trk;
//     const TrigTauRecEFObject* efobject = ch->EF;
  
    
//     //check Level 1
//     if (l1object == 0 && !plotAllMatchedEvents) {
//       if ( debug ) cout << "L1 Object is empty; go to next chain" << endl;
//       continue;
//     }
//     if( !l1object == 0 ) {
//       unsigned int currentWord = printL1(l1object);	  
//       if (!plotAllMatchedEvents && currentWord != 0 ) continue;
//     }
//     else if (!plotAllMatchedEvents) continue;
    
//     //check Level 2 (including Trk and Cal)
//     if (l2object == 0 && !plotAllMatchedEvents) {
//       if ( debug ) cout << "L2 object is empty; go to next chain" << endl;
//       continue;
//     }
//     if( !(l2object == 0))//  && ((truth->DeltaR(*l2object)<0.2) || ignoreMatching) )
//       {
	
// 	unsigned int currentWord1 = printL2Calo(l2caloobject);
// 	unsigned int currentWord2 =  printL2Trk(l2object);
// 	printL2Combined(l2object);
	
// 	if ( (currentWord1 !=0 || currentWord2 !=0) && !plotAllMatchedEvents ) continue;
//       }
//     else if (!plotAllMatchedEvents) continue;
    
    
//     //check EF
//     if (efobject == 0) {
//       if ( debug ) cout << "EF object is empty; go to next chain" << endl;
//       continue;
//     }
//     //if ( truth->DeltaR(*efobject)<0.2 || ignoreMatching ) {
//     unsigned int currentWord = printEF(efobject); 
//     // } //only uncomment with offline matching
    
    
//     if (debug ) cout << "All levels for chain " << ichain  << " checked." << endl;
//   } // end chain loop
  
  }



  //implement: loop over all tauObjects and see if the are attached to TrigChain

  /***************************************************************************
   DO NOT USE CHAINS AFTER THIS POINT. sorting MODIFIES POINTERS in EVENT
                   or re-do chains after sorting!
   ***************************************************************************/
  if( l1 ){
    sort(event->tauL1.begin(), event->tauL1.end(), less_roiWordL1());
    for(unsigned int iL1 = 0; iL1 < event->nTauL1(); ++iL1)
      printL1(&event->tauL1Entry(iL1));
  }

  if( l2 ) {
    sort(event->tauL2.begin(), event->tauL2.end(), less_Et());
    for(vector<TrigTauL2Object*>::iterator itau = event->tauL2.begin(); itau !=  event->tauL2.end(); ++itau)
      sort((*itau)->tracks.begin(), (*itau)->tracks.end(),  less_Et());
    for(unsigned int iL2 = 0; iL2 < event->tauL2.size(); ++iL2)
      {
	printL2Combined(event->tauL2[iL2]);
	printL2Calo(event->tauL2[iL2]);
	printL2Trk(event->tauL2[iL2]);
      }
  }

  if ( ef ) {
     sort(event->tauEF.begin(), event->tauEF.end(), less_EF());
     for(unsigned int iEF = 0; iEF < event->tauEF.size(); ++iEF)
       printEF(event->tauEF[iEF]);
     
  }

  if( efid) {

    // DO NOT sort EF tracks = they are saved in TRefArray, and are pointed to
    // IF you use following, do not use EF tracks anymore!!
    
    // if roiWord is correct : 
    //sort(event->tracksRoIEF.begin(), event->tracksRoIEF.end(), less_roiWordEFID());
    for(vector<TrigTauTrackCollection*>::iterator itau = event->tracksRoIEF.begin();
	itau !=  event->tracksRoIEF.end(); ++itau)
      sort((*itau)->tracks.begin(), (*itau)->tracks.end(),  less_Et());
       
     for(unsigned int iEF = 0; iEF < event->tracksRoIEF.size(); ++iEF)
       printEFID(event->tracksRoIEF[iEF]);
  }


return;
}



int TTPValVariables::printL1(const TrigTauL1Object * l1object)
{
  unsigned int currentWord = selmanager.l1Sel.cutWord(*l1object);
  cout << "\n==========LEVEL 1 ========= \nWord:\t" << currentWord ;
  cout << "\t\tChainPosition: Eta:\t" << l1object->Eta() << "\tPhi:\t" << l1object->Phi() << endl;
  cout << "RoIWord:\t" << setprecision(4) << l1object->m_roiWord;
  cout << "\tEnergy:\t" << setprecision(4) << l1object->m_TauEnergy;
  cout << "\tEmIsol:\t" << setprecision(4) << l1object->m_EmIsol << endl;
  //cout << "\tHadIsol:\t" << setprecision(4) << l1object->m_HdIsol;
  //cout << "\tEmCore:\t" << setprecision(4) << l1object->m_EmCore;
  //cout << "\tHadCore:\t" << setprecision(4) << l1object->m_HdCore;
  //cout << "\tEmEnergy:\t" << setprecision(4) << l1object->m_EmEnergy << endl;
  //histograms[0*3+j]->Fill(l1object->m_EmEnergy);
  //histograms[0*3+j]->Fill(l1object->m_EmEnergy);
  return currentWord;
}
  
int TTPValVariables::printL2Calo(const TrigTauL2Object * l2calobject)
{
  unsigned int currentWord1 = selmanager.l2CalSel.cutWord(*l2calobject);
  cout << "\n\n=========Level 2 Calo ==========\nWord: \t" << currentWord1;
  cout << "\t\tChainPosition: Eta:\t" << l2calobject->Eta() << "\tPhi:\t" << l2calobject->Phi();
  cout << "\nEtCluster:\t" << setprecision(4) << l2calobject->etClus();
  cout << "\tEtCalib:\t" << setprecision(4) << l2calobject->etClusCalib();
  cout << "\tnStrips:\t" << setprecision(4) << l2calobject->m_nStrips;
  cout << "\tStripWidth:\t" << setprecision(4) << l2calobject->m_stripWidth2;
  cout << "\nEmRadius:\t" << setprecision(4) << l2calobject->m_EMRadius;
  cout << "\tIsoFrac:\t" << setprecision(4) << l2calobject->m_isoFrac;
  cout << "\tEmEnergyWidth:\t" << setprecision(4) << l2calobject->m_EMEnergyWidth[2];
  cout << "\tEtNarrow:\t" << setprecision(4) << l2calobject->etNarrow();
  cout << "\tCaloEta:\t" << setprecision(4) << l2calobject->m_caloEta;
  cout << "\nCaloPhi:\t" << setprecision(4) << l2calobject->m_caloPhi;
  cout << "\tEmEnergyWide:\t" << setprecision(4) << l2calobject->m_EMEnergyWide[0]+l2calobject->m_EMEnergyWide[1]+l2calobject->m_EMEnergyWide[2]+l2calobject->m_EMEnergyWide[3];
  cout << "\tHadEnergyWide:\t" << setprecision(4) << l2calobject->m_HadEnergyWide[0]+l2calobject->m_HadEnergyWide[1]+l2calobject->m_HadEnergyWide[2];
  return currentWord1;
}

int TTPValVariables::printL2Trk(const TrigTauL2Object * l2object)
{
  unsigned int currentWord2 = selmanager.l2TrkSel.cutWord(*l2object);
  
  cout << "\n\n=========Level 2 Track ==========\nWord: \t" << currentWord2;
  cout << "\t\tChainPosition: Eta:\t" << l2object->Eta() << "\tPhi:\t" << l2object->Phi();
  cout << "\nnCoreTracks:\t" << setprecision(4) << l2object->m_nCoreTracks;
  cout << "\tnSlowTracks:\t" << setprecision(4) << l2object->m_nSlowTracks;
  cout << "\tnIsoTracks:\t" << setprecision(4) << l2object->m_nIsoTracks;
  cout << "\tnMatchedTracks:\t" << setprecision(4) << l2object->m_nMatchedTracks;
  cout << "\tCharge:\t" << setprecision(4) << l2object->m_charge;
  cout << "\nPtLeadingTrack:\t" << setprecision(4) << l2object->m_leadingTrackPt;
  cout << "\tPtSumRatio:\t" << setprecision(4) << l2object->m_scalarPtSumIso/l2object->m_scalarPtSumCore;
  cout << "\tTotal pt:\t" << setprecision(4) << l2object->ptTot();
  
  unsigned int ntracks = l2object->nTracks();
  cout << "\n\nLooping over all tracks:\t"<< ntracks<< endl;
  const TrigTauTrack * track = 0;
  for (unsigned int iTrk = 0; iTrk!=ntracks; ++iTrk) {
    track =l2object->tracks[iTrk];
    cout << "Printing Track number " << iTrk << endl;
    cout << "Pt:\t" << track->Pt();
    cout << "\tEta:\t" << track->Eta();
    cout << "\tPhi:\t" << track->Phi();
    cout << "\ta0:\t" << track->m_a0 << endl;
  }
  cout << "\nPtSumCore:\t" <<l2object->m_scalarPtSumCore;
  cout << "\tPtSumIsol:\t" <<l2object->m_scalarPtSumIso;
  return currentWord2;
}

int TTPValVariables::printL2Combined(const TrigTauL2Object * l2object)
{
	  cout << "\n\n=========Level 2 Combined ==========" << endl;;
	  //cout << "\nRoIWord:\t" << setprecision(4) << l2object->m_roiWord;
	  cout << "\tEtaCombined:\t" << l2object->Eta();
	  cout << "\tPhiCombined:\t" << l2object->Phi();
	  cout << "\nEtCombined:\t" << l2object->m_EtCalib << endl;
	  cout << "\tm_tags:\t" << endl; l2object->m_tags.print();
	  return 0;
}

int TTPValVariables::printEF(const TrigTauRecEFObject * efobject)
{
	  unsigned int currentWord = selmanager.efSel.cutWord(*efobject); 

	  cout << "\n\n========= Event Filter ==========\nWord: \t" << currentWord;
	  cout << "\t\tChainPosition: Eta:\t" << efobject->Eta() << "\tPhi:\t" << efobject->Phi();
	  cout << "\nEt:\t"<<efobject->Et();
	  cout << "\tSeedType:\t" << efobject->m_seedType;
	  cout << "\nRoIWord:\t" << setprecision(4) << efobject->m_roiWord;
	  //cout << "\nnTracks:\t" << setprecision(4) << efobject->nTracks();			
	  cout << "\tEmRadius:\t" << setprecision(4) << efobject->m_EMRadius; 
	  cout << "\tEmRadius1p3p:\t" << setprecision(4) << efobject->m_EMRadius_1p3p; 
	  cout << "\tIsoFrac:\t" << setprecision(4) << efobject->m_isoFrac; 
	  float emFrac=0;						
	  if (efobject->m_etEMCalib>0 ) emFrac = efobject->m_etEMCalib/(efobject->m_etEMCalib + efobject->m_etHadCalib); 
	  cout << "\tEmFrac:\t" << setprecision(4) << emFrac;				
	  float ptMax = 999999999.;					
	  if (efobject->nTracks() ) ptMax = efobject->leadingTrack()->Pt(); 
	  cout << "\tMaxPt:\t" << setprecision(4) << ptMax;		
	  cout << "\nEt\t" << setprecision(4) << efobject->Et();			
	  cout << "\tEmEt:\t" << setprecision(4) << efobject->m_energy/cosh(efobject->m_caloEta); 
	  //histograms[30*3+j]->Fill(efobject->mass()); mass is more complicated; will be added later. 
	  cout << "\tMass:\t" << "not implemented, yet";					
	  if (debug) cout << "All histograms filled..." << endl;	
	  if ( currentWord == 0 && debug ) cout << "Tau passed trigger!!!" << endl;

	  cout << "\tEmCalib:\t" << efobject->m_etEMCalib;
	  cout << "\nHadCalib:\t" << efobject->m_etHadCalib;
	  cout << "\tSumHadCellEt:\t" << efobject->m_sumHadCellEt;
	  cout << "\tSumEmCellEt:\t" << efobject->m_sumEmCellEt;
	  cout << "\tHadRadius:\t" << efobject->m_hadRadius;
	  cout << "\nMTrk3P:\t" << efobject->m_MTrk3P;
	  cout << "\tEmRadius1P3P:\t" << efobject->m_EMRadius_1p3p;
	  cout << "\tEfEFlow:\t" << efobject->m_etEFlow;
	  cout << "\tMVisEFlow:\t" << efobject->m_MvisEflow;


	  cout << "\n\nLooping over all tracks:";
	  //unsigned int nTracks = efobject->nTracks();
// 	  const TrigTauTrack * track = 0;  //possible memory leak
// 	  for (unsigned int iTrk = 0; iTrk!=nTracks; ++iTrk) {

// 	    track = &efobject->track(iTrk);
// 	    cout << "\nPrinting Track number " << iTrk << endl;

// 	    cout << "Pt:\t" << track->Pt();
// 	    cout << "\tEta:\t" << track->Eta();
// 	    cout << "\tPhi:\t" << track->Phi();
// 	    cout << "\ta0:\t" << track->m_a0 << endl;
// 	  }
    cout << "\tm_tags:\t" << endl; efobject->m_tags.print();

	    return currentWord;
}


int TTPValVariables::printEFID(const TrigTauTrackCollection * object)
{
  
  cout << "\n\n=========Level EF Tracks ==========\n";
  cout << "\nRoIWord:\t" << setprecision(4) << object->m_roiWord;
  
  unsigned int ntracks = object->nTracks();
  cout << "\n\nLooping over all tracks:\t"<< ntracks<< endl;
  const TrigTauTrack * track = 0;
  for (unsigned int iTrk = 0; iTrk!=ntracks; ++iTrk) {
    track =object->tracks[iTrk];
    cout << "Printing Track number " << iTrk << endl;
    cout << "Pt:\t" << track->Pt();
    cout << "\tEta:\t" << track->Eta();
    cout << "\tPhi:\t" << track->Phi();
    cout << "\ta0:\t" << track->m_a0 << endl;
  }
  return 0;
}
