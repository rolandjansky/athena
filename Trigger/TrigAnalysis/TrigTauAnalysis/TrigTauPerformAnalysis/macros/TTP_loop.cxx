/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

struct TauCuts
{

  // L1 
  float cut_L1_TauClus;
  float cut_L1_EmIsol;
  float cut_L1_HdIsol;
  
  // L2Calo
  float cut_L2_EMRadius;
  float cut_L2_isoFrac; 
  float cut_L2_stripWidth2;
  float cut_L2_EtCalib; 

  // L2ID
  float cut_L2_minPt;
  int   cut_L2_nTrackMultiplicities;
  int   cut_L2_trackMultiplicities[20];
  bool  cut_L2_charge;
  bool  cut_L2_nSlowTracks;
  float cut_L2_slowPtMax;
  float cut_L2_basicRoI;
  float cut_L2_nIsoTracks;
  float cut_L2_leadingPt;
  bool  cut_L2_centerLeadingPt;
  float cut_L2_sumPtRatio;

  // L2Match
  int cut_L2_nMatchedTrackMultiplicities;
  int cut_L2_matchedTrackMultiplicities[20];
  float cut_L2_minTauPt;


  // EF
  float cut_EF_EMEt;
  float cut_EF_EtCalib;
  float cut_EF_EMRadius;
  float cut_EF_isoFrac; 
  float cut_EF_nTracksMin;
  float cut_EF_nTracksMax;
  float cut_EF_EMFrac;
  float cut_EF_PtMaxTrack;
  

};

int TTP_loop(TTree * tree, TrigTauFlags::TRIGGERMENU menu = TrigTauFlags::TAU16I)//int id)
{
  TauCuts cuts;
  setCuts(menu, cuts);

  // -- Initialization ----------------------
  TrigTauEvent * event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch",&event) ;

  tree->SetBranchStatus("*",1) ;

  int nL1 = 0;
  int nL2Calo = 0;
  int nL2Track = 0;
  int nEF = 0;
  int nEvtL1 = 0;
  int nEvtL2Calo = 0;
  int nEvtL2Track = 0;
  int nEvtEF = 0;
  int nDecL1 = 0;
  int nDecL2 = 0;
  int nDecEF = 0;

  int nev = int(tree->GetEntries() );
  for(int iev = 0; iev<nev; ++iev)
    {
      event->reset() ;
      tree->GetEvent(iev) ;
      //=============================== Loop over event =================

      // Check TriggerDecision
      bool decL1 = event->decision.passedL1(menu);
      bool decL2 = event->decision.passedL2(menu);
      bool decEF = event->decision.passedEF(menu);

      nDecL1 += int(decL1);
      nDecL2 += int(decL2);
      nDecEF += int(decEF);

      // Start from  L1
      bool foundL1 = false;
      bool foundL2Calo = false;
      bool foundL2Track = false;
      bool foundEF = false;
      for(unsigned int iL1 = 0; iL1 < event->nTauL1(); ++iL1)
	{
	  const TrigTauL1Object * l1 = &event->tauL1Entry(iL1);


	  if( !passedL1(cuts,*l1) )
	    {
	      // if(decL1 ){ cout << "found trigDec for "; printL1(*l1);}//be aware of other good L1 in the event
	      continue;
	    }//else if( !decL1 ) { cout << "no trigDec for "; printL1(*l1); }
	  nL1++;
	  foundL1 = true;

	  // in C++ we can use bestMatch function, but not in the macro
	  //int iL2 = event->bestMatch(*l1, event->tauL2.begin() , event->tauL2.end() ); 
	  const TrigTauL2Object * l2(0);
	  bool found = false;
	  for(unsigned int iL2 = 0; !found && iL2 < event->nTauL2(); ++iL2)
	    {
	      l2 = &(event->tauL2Entry(iL2));
	      if( event->same(*l1, *l2) ) found = true; 
	    }
	  if( !found  ) continue;
	  if( !passedL2Calo(cuts,*l2) )    continue;
	  nL2Calo++;
	  foundL2Calo = true;

	  if( !passedL2Track(cuts,*l2) )
	    {
	      continue;
	    }//else if( !decL2 ) { cout << "no trigDec for :\n"; printL2Calo(*l2); printL2ID(cuts,*l2); }

	  nL2Track++;
	  foundL2Track = true;

	  const TrigTauRecEFObject * ef(0);
	  found = false;
	  for(unsigned int iEF = 0; !found && iEF < event->nTauEF(); ++iEF)
	    {
	      ef = &event->tauEFEntry(iEF);
	      if( ef->m_roiID  == l2->m_roiID ){ found = true ;}
	    }
	  if( !found ) continue;
	  if( !passedEF(cuts,*ef) )
	    {
	      //if( decEF ){ cout << "found trigDec for"; printEF(*ef);}
	      continue;
	    }//else if( !decEF ) { cout << "no trigDec for "; printEF(*ef); }
	  nEF++;
	  foundEF = true;
	} // end L1 loop

      if( foundL1 ) nEvtL1++;
      if( foundL2Calo ) nEvtL2Calo++;
      if( foundL2Track ) nEvtL2Track++;
      if( foundEF ) nEvtEF++;

    }// end loop over events


  cout << " Nevt   = "  << nev << endl;
  cout << " L1 eff/event           = " << float(nEvtL1)/float(nev) << " TriggerDecision : " << float(nDecL1)/float(nev)<< endl;
  cout << " L2Calo eff/event       = " << float(nEvtL2Calo)/float(nev) <<endl;
  cout << " L2Calo+Track eff/event = " << float(nEvtL2Track)/float(nev) << " TriggerDecision : " << float(nDecL2)/float(nev)<<endl;
  cout << " EF eff/event           = " << float(nEvtEF)/float(nev) << " TriggerDecision : " << float(nDecEF)/float(nev)<<endl;


  return 0;
}

//==================================================================
// yes, these functions can be added to TauCuts structure. 
bool passedL1(const TauCuts& cuts, const TrigTauL1Object & ll1)
{
  if( cuts.cut_L1_TauClus >= ll1.m_TauEnergy ) return false;
  if( cuts.cut_L1_EmIsol  < ll1.m_EmIsol ) return false;
  if( cuts.cut_L1_HdIsol  < ll1.m_HdIsol ) return false;
  return true;
}

bool passedL2Calo(const TauCuts& cuts, const TrigTauL2Object & ll2)
{
  if( cuts.cut_L2_EMRadius    <= ll2.m_EMRadius ) return false;
  if( cuts.cut_L2_isoFrac     <= ll2.m_isoFrac ) return false; 
  if( cuts.cut_L2_stripWidth2 <= ll2.m_stripWidth2 ) return false; ;
  if( cuts.cut_L2_EtCalib     > ll2.m_EtCalib ) return false; ; 
  return true;
}  
bool passedL2Track(const TauCuts& cuts, const TrigTauL2Object & ll2)
{
  int nIso = 0;
  int nSlow = 0;
  int nTrack = 0;
  int charge = 0;
  float leadPt = 0;
  
  float sumPtCore = 0;
  float sumPtIsol = 0;

  TLorentzVector base;
  if( cuts.cut_L2_centerLeadingPt )
    {
      for(unsigned int itr = 0; itr < ll2.nTracks(); ++itr )
	{
	  const TrigTauTrack * track = &ll2.track(itr);
	  float pt = track->Pt();
	  if(  pt <  cuts.cut_L2_minPt ) continue;
	  if( leadPt < pt ) leadPt = pt;
	  base = *track;
	}  

      if( leadPt <1 ) base = ll2;
    }



  for(unsigned int itr = 0; itr < ll2.nTracks(); ++itr )
    {
      const TrigTauTrack * track = &ll2.track(itr);
      float pt = track->Pt();
      if(  pt <  cuts.cut_L2_minPt ) continue;
      
      float dR = ll2.DeltaR(*track);
      if( dR > 0.3*sqrt(2) ) { // something must be wrong here plot it
	cout << " Strange track! dR="<< dR<<": TrigTau_phi= "<< ll2.Phi() << ", eta= "<<ll2.Eta() << endl
	     << "                           track  _phi= "<< track->Phi() << ", eta= "<< track->Eta() 
	     << ", pt="<< track->Pt() << endl;
      }
      if( dR > cuts.cut_L2_basicRoI  ) { nIso++; sumPtIsol += track->Pt(); continue; }
      else 
      nTrack++;
      charge += track->charge();
      sumPtCore += track->Pt();
      if( pt < cuts.cut_L2_slowPtMax ) nSlow++;
    }

  bool goodMultiplicity = false;
  if( cuts.cut_L2_charge )
    {
      if( nTrack %2 == 1 )
	{ if( abs(charge) == 1 ) goodMultiplicity  = true; }
      else
	{ if( abs(charge) == 0 || abs(charge) == 2 ) goodMultiplicity  = true; }
      if( !goodMultiplicity ) return false;
     }

   if( cuts.cut_L2_nSlowTracks < nSlow ) return false;
   if( cuts.cut_L2_nIsoTracks < nIso ) return false;
   if( cuts.cut_L2_leadingPt > leadPt ) return false; ;
   
   if( sumPtCore <1 ) sumPtCore = 40000;
   if(  cuts.cut_L2_sumPtRatio < sumPtIsol/sumPtCore ) return false;
  
  goodMultiplicity = false;
  for(unsigned int imult = 0; !goodMultiplicity && imult < cuts.cut_L2_nTrackMultiplicities; ++imult )
    if( cuts.cut_L2_trackMultiplicities[imult] == nTrack ) goodMultiplicity = true;
  if( !goodMultiplicity ) return false;
  
  goodMultiplicity = false;
  for(unsigned int imult = 0;!goodMultiplicity && imult < cuts.cut_L2_nMatchedTrackMultiplicities; ++imult )
    if( cuts.cut_L2_matchedTrackMultiplicities[imult] == ll2.m_nMatchedTracks ) goodMultiplicity = true;
  if( !goodMultiplicity ) return false;
  
   if( cuts.cut_L2_minTauPt > ll2.Pt() ) return false; ;
  
  return true;
}
  
bool passedEF(const TauCuts& cuts, const TrigTauRecEFObject & lef)
{
  if( cuts.cut_EF_EMRadius < lef.m_EMRadius ) return false;
  if( cuts.cut_EF_isoFrac  < lef.m_isoFrac ) return false; 
  if( cuts.cut_EF_nTracksMin  > lef.nTracks() ) return false; 
  if( cuts.cut_EF_nTracksMax  < lef.nTracks() ) return false; 
  if( cuts.cut_EF_EtCalib     > lef.Et() ) return false; 
  if(  lef.leadingTrack() && cuts.cut_EF_PtMaxTrack  > lef.leadingTrack()->Pt() ) return false; 
  float emet=lef.m_energy/cosh(lef.m_caloEta);
  if( cuts.cut_EF_EMEt       >  emet ) return false; 
 
  float EMFrac = lef.m_etEMCalib/(lef.m_etEMCalib+lef.m_etHadCalib);
  if( cuts.cut_EF_EMFrac > EMFrac ) return false;

 return true;
}

//==============================================================================

void setCuts(TrigTauFlags::TRIGGERMENU menu, TauCuts &cuts)
{
  if( menu == TrigTauFlags::TAU16I )
    {
      cuts.cut_L1_TauClus = 11000;
      cuts.cut_L1_EmIsol  = 6000;
      cuts.cut_L1_HdIsol  = 999000;
  
      cuts.cut_L2_EMRadius = 0.0160;
      cuts.cut_L2_isoFrac  = 0.65; 
      cuts.cut_L2_stripWidth2 = 0.051;
      cuts.cut_L2_EtCalib = 17000; 


      cuts.cut_L2_minPt = 1500;
      cuts.cut_L2_nTrackMultiplicities = 7;
      cuts.cut_L2_trackMultiplicities[0] = 1;
      cuts.cut_L2_trackMultiplicities[1] = 2;
      cuts.cut_L2_trackMultiplicities[2] = 3;
      cuts.cut_L2_trackMultiplicities[3] = 4;
      cuts.cut_L2_trackMultiplicities[4] = 5;
      cuts.cut_L2_trackMultiplicities[5] = 6;
      cuts.cut_L2_trackMultiplicities[6] = 7;
      cuts.cut_L2_charge      = false;
      cuts.cut_L2_nSlowTracks = 2;
      cuts.cut_L2_slowPtMax   = 7500;
      cuts.cut_L2_basicRoI    = 0.15;
      cuts.cut_L2_nIsoTracks  = 99;
      cuts.cut_L2_leadingPt   = 5000;
      cuts.cut_L2_centerLeadingPt = true;
      cuts.cut_L2_sumPtRatio  = 0.1;

      cuts.cut_L2_nMatchedTrackMultiplicities  = 7;
      cuts.cut_L2_matchedTrackMultiplicities[0] = 1;
      cuts.cut_L2_matchedTrackMultiplicities[1] = 2;
      cuts.cut_L2_matchedTrackMultiplicities[2] = 3;
      cuts.cut_L2_matchedTrackMultiplicities[3] = 4;
      cuts.cut_L2_matchedTrackMultiplicities[4] = 5;
      cuts.cut_L2_matchedTrackMultiplicities[5] = 6;
      cuts.cut_L2_matchedTrackMultiplicities[6] = 7;
      cuts.cut_L2_minTauPt = 0;


  
      cuts.cut_EF_EMEt = -10000;
      cuts.cut_EF_EtCalib = 22000;
      cuts.cut_EF_EMRadius = 0.1;
      cuts.cut_EF_isoFrac = 0.3; 
      cuts.cut_EF_nTracksMin = 1;
      cuts.cut_EF_nTracksMax = 8;
      cuts.cut_EF_EMFrac = -10;
      cuts.cut_EF_PtMaxTrack = 7000;

    }else{
    cout << " The cuts for this menu are not yet in this macro\n";
  }
}

//======================================================================================
// Print functions

void printL1(const TrigTauL1Object & ll1 )
{
  cout << "L1 : TauEn="<< ll1.m_TauEnergy<< ", EmIsol="<< ll1.m_EmIsol
       << ", HdIsol="<< ll1.m_HdIsol  << endl;
}
void printL2Calo(const TrigTauL2Object & ll2 )
{
  cout << "L2Calo : EMRad="<<ll2.m_EMRadius << ", IsoFrac="<<  ll2.m_isoFrac 
       << ", stripWidth2="<< ll2.m_stripWidth2 << ", EtCalib="<< ll2.m_EtCalib 
       << " " << ll2.m_eCalib/cosh(ll2.Eta())<< endl;
}

void printL2ID(const TauCuts& cuts, const TrigTauL2Object & ll2  )
{
  int nIso = 0;
  int nSlow = 0;
  int nTrack = 0;
  int charge = 0;
  float leadPt = 0;

  for(unsigned int itr = 0; itr < ll2.nTracks(); ++itr )
    {
      const TrigTauTrack * track = &ll2.track(itr);
      float pt = track->Pt();
      if(  pt <  cuts.cut_L2_minPt ) continue;
      float dR = ll2.dRWithCluster(*track);

      if( dR > cuts.cut_L2_basicRoI ) { nIso++; continue; }
      nTrack++;
      charge += track->charge();
      if( pt < cuts.cut_L2_slowPtMax ) nSlow++;
      if( leadPt < pt ) leadPt = pt;
    }

  cout << "L2ID : nTracks="<< nTrack << ", nSlow(pT<"<<cuts.cut_L2_slowPtMax<<")="<< nSlow<<", nIso="<< nIso 
       << ", leadPt="<<leadPt<< endl;
  cout << "L2Match : nMatchedTracks="<< ll2.m_nMatchedTracks<< endl;
}

void printEF(const TrigTauRecEFObject & lef)
{
  float emet=lef.m_energy/cosh(lef.m_caloEta);
  float EMFrac = lef.m_etEMCalib/(lef.m_etEMCalib+lef.m_etHadCalib);
  cout << "EF :  nTracks="<<lef.nTracks();
  if(lef.leadingTrack()) cout << ", leadPt="<<lef.leadingTrack()->Pt();
  cout << ", EtCalib="<<lef.Et()<<", EMEt="<<emet <<endl
       << ", EmRad="<< lef.m_EMRadius<<", IsoFrac="<< lef.m_isoFrac<< ", EMFrac="<< EMFrac << endl;  
}
