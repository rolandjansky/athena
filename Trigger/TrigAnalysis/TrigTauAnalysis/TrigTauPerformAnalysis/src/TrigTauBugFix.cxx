/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauBugFix.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence, Olya Igonkina
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to manage the individual //EF selectors.
///           Can also be used to define complete trigger menus
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformAnalysis/TrigTauBugFix.h"
#include "TrigTauPerformNtuple/TrigTauEvent.h"

#include <math.h>
#include <iostream>
using namespace std;

ClassImp(TrigTauBugFix)  

//
// -----------------------------------------------------------------------
//

int TrigTauBugFix::setTTPVersion(const TString & fileName) // sets both m_TTP and m_fileType
{
  // default
   m_TTP = TTP12; m_fileType = RDO;
  // --------------- 1 ----------------------------------
  // specific cases
  //if( fileName.Contains("generic_part_of_name_of_the_set") ){
  // m_TTP =  ; m_fileType = AOD;  return 0;
  // }else if( fileName.Contains("generic_part_of_name_of_the_set") ){
  // m_TTP =  ; m_fileType = AOD;  return 0;
  // }

  
  // --------------- 2 ----------------------------------
  // generic determination
   if( fileName.Contains("TTP15",TString::kIgnoreCase) ) {
     m_TTP = TTP15; 
   } else if( fileName.Contains("TTP14",TString::kIgnoreCase) ) {
     m_TTP = TTP14; 
   } else if( fileName.Contains("TTP13",TString::kIgnoreCase) ) {
     m_TTP = TTP13; 
   } else if( fileName.Contains("TTP12",TString::kIgnoreCase) ) {
     m_TTP = TTP12; 
   } else if( fileName.Contains("TTP11",TString::kIgnoreCase) ) {
     m_TTP = TTP11; 
   } else if( fileName.Contains("TTP10",TString::kIgnoreCase) ) {
     m_TTP = TTP10; 
   } else if( fileName.Contains("TTP09",TString::kIgnoreCase) ||  
       fileName.Contains("TTP9",TString::kIgnoreCase) ) {
     m_TTP = TTP9; 
   } else if( fileName.Contains("TTP07",TString::kIgnoreCase) ||  
       fileName.Contains("TTP7",TString::kIgnoreCase) ) {
     m_TTP = TTP7; 
   } else if( fileName.Contains("TTP06",TString::kIgnoreCase) ||  
	      fileName.Contains("TTP6",TString::kIgnoreCase) ) {
    m_TTP = TTP6; 
   } else if( fileName.Contains("TTP05",TString::kIgnoreCase) ||  
       fileName.Contains("TTP5",TString::kIgnoreCase) ) {
     m_TTP = TTP5; 
   }else if ( fileName.Contains("TTP04",TString::kIgnoreCase) || 
	      fileName.Contains("TTP4", TString::kIgnoreCase) ){
     m_TTP = TTP4; m_fileType = AOD; return 0;
   }

   if      (  fileName.Contains("RDO") )  m_fileType = RDO;
   else if (  fileName.Contains("ESD") )  m_fileType = ESD;
   else if (  fileName.Contains("AOD") )  m_fileType = AOD;
   else  {
     m_fileType = RDO;
     cout << " TrigTauBugFix WARNING : can not identify file type of  " << fileName << endl;
   }

  
 return 0;
}

void
TrigTauBugFix::fix(TrigTauEvent* event) {

  if (event == 0) {
    cout << "TrigTauBugFix::analyze - event pointer is NULL!" << endl;
    return;
  }
  m_nTotal++;


  //=====================================
  if( m_TTP == TTP15 )
  //=====================================
    {
      if( m_fileType == RDO )
	{
	  static bool first=true;
	  if (first)
	    {
	      cout <<" TrigTauBugFix::fix for TTP15, RDO - L1 units was GeV, now is MeV; \n";
	      first=false;
	    }
	  correctL1GeVtoMev(event);
	}
    }

  //=====================================
  else if( m_TTP == TTP14 )
  //=====================================
    {
      if( m_fileType == RDO )
	{
	  static bool first=true;
	  if (first)
	    {
	      cout <<" TrigTauBugFix::fix for TTP14, RDO - L1 units was GeV, now is MeV; \n";
	      first=false;
	    }
	  correctL1GeVtoMev(event);
	}
    }

  //=====================================
  else if( m_TTP == TTP13 )
  //=====================================
    {
      if( m_fileType == RDO )
	{
	  static bool first=true;
	  if (first)
	    {
	      cout <<" TrigTauBugFix::fix for TTP13, RDO - L1 units was GeV, now is MeV; \n";
	      first=false;
	    }
	  correctL1GeVtoMev(event);
	}
    }

  //=====================================
  else if( m_TTP == TTP12 )
  //=====================================
    {
      if( m_fileType == RDO )
	{
	  static bool first=true;
	  if (first)
	    {
	      cout <<" TrigTauBugFix::fix for TTP12, RDO - L1 units was GeV, now is MeV; \n";
	      first=false;
	    }
	  correctL1GeVtoMev(event);
	  attachTRTOnlyTracks(event);
	}
    }

  //=====================================
  else if( m_TTP == TTP11 )
  //=====================================
    {
      if( m_fileType == RDO )
	{
	  static bool first=true;
	  if (first)
	    {
	      cout <<" TrigTauBugFix::fix for TTP11, RDO - L1 units was GeV, now is MeV; \n"
		   <<"   Events with ZeroTrack pointers are removed - might be an issue for high occupancy events\n";
	      first=false;
	    }
	  correctL1GeVtoMev(event);
	  event->mcEvent.recalculateEtMissTrue();
	  resetIfZeroPointerTracks(event);
	}
    }

  //=====================================
  else  if( m_TTP == TTP10 )
  //=====================================
    {
      if( m_fileType == RDO )
	{
	  static bool first=true;
	  if (first)
	    {
	      cout <<" TrigTauBugFix::fix for TTP10, RDO - L1 units was GeV, now is MeV\n";
	      first=false;
	    }
	  correctL1GeVtoMev(event);
	}
    }

  //=====================================
  else if( m_TTP == TTP9 )
  //=====================================
    {
      if( m_fileType == RDO )
	{
	  static bool first=true;
	  if (first)
	    {
	      cout <<" TrigTauBugFix::fix for TTP9, RDO - L1 units was GeV, now is MeV\n";
	      //cout << " TEST!!! Correct EF EMRadius by 1./1.13!! (to conver dR=0.4 to dR=0.3 EMRad) \n";
	      first=false;
	    }
	  correctL1GeVtoMev(event);
	  //removeDuplicateTracks(event);
	  //resetIfl2roiWordIs0(event);

	  // TEST !!!!
// 	  for(unsigned int iEF = 0;  iEF < event->nTauEF(); ++iEF)
// 	    {
// 	     TrigTauRecEFObject * ef = &event->tauEFEntry(iEF);
// 	     ef->m_EMRadius /= 1.13;
// 	   }



	}

	  
  //=====================================
    }else if( m_TTP == TTP7 )
  //=====================================
    {
      if( m_fileType == RDO )
	{
	  static bool first=true;
	  if (first)
	    {
	      cout <<" TrigTauBugFix::fix for TTP7, RDO - L1 units was GeV, now is MeV, track duplicates are removed\n";
	      first=false;
	    }
	  correctL1GeVtoMev(event);
	  removeDuplicateTracks(event);
	  //resetIfl2roiWordIs0(event);
	}

	  
  //=====================================
    }else  if( m_TTP == TTP5 || m_TTP == TTP6 || m_TTP == TTP4 )
  //=====================================
    {
      cout << " This version of TrigTauperformAnalysis is not compatible with ntuples before TTP08\n"
	   << " please use TrigTauPerformAnalysis-00-02-17 instead \n";

    }

}




float TrigTauBugFix::tauRec_et_corr_TTP4(double et_in, double eta_in, int ntrk) const
{

  double m_ptetacorrectionsntr1[9] = { 0.8339,   0.8125,   0.8254,
				       0.8747,   0.8583,   0.8594,
				       0.9088,   0.9013,   0.8922};
  double m_ptetacorrectionsntr23[9]= { 0.9000,   0.8593,   0.9034,
				       0.9208,   0.8791,   0.9132,
				       0.9359,   0.9231,   0.9033};

  const int m_nptbins  = 3;
  const int m_netabins = 3;

  double m_ptpoints[m_nptbins] = {15000, 35000, 150000};
  double m_etapoints[m_netabins] = {0.25, 1.0, 2.0};

  double et = et_in;
  double eta = fabs( eta_in );
  double corr = 1, fudge = 1.011;

  unsigned int lowpt_idx = 0;
  unsigned int loweta_idx = 0;
  
  double lowpt_frac = 0;
  double loweta_frac = 0;
  
  while ( lowpt_idx < m_nptbins-1 && et > m_ptpoints[lowpt_idx+1] )
    lowpt_idx++;
  
  if( lowpt_idx < m_nptbins-1)
      lowpt_frac = ( m_ptpoints[lowpt_idx+1] - et ) /
          ( m_ptpoints[lowpt_idx+1] - m_ptpoints[lowpt_idx] );
  
  if ( lowpt_frac > 1 )
    lowpt_frac =  1;
  if( lowpt_frac < 0 ) {   //should never happen, only if ptNumberOfBins is set wrong (which is checked now)
    std::cerr << "FIXME: lowpt_frac < 0 !!" << std::endl;
  }
  
  while ( loweta_idx < m_netabins-1 && eta > m_etapoints[loweta_idx+1] )
    loweta_idx++;
  
  if( loweta_idx < m_netabins-1)
      loweta_frac = ( m_etapoints[loweta_idx+1] - eta ) /
          ( m_etapoints[loweta_idx+1] - m_etapoints[loweta_idx] );
  
  if ( loweta_frac > 1)
    loweta_frac = 1;
  if( loweta_frac < 0 ) {    //should never happen, only if etaNumberOfBins is set wrong
    std::cerr << "FIXME: loweta_frac < 0 !!" << std::endl;
  }
    
  double coeff_matrix[2][2] = { {0, 0}, {0, 0} };
  
  if ( ntrk <= 1 ) {
    coeff_matrix[0][0] = m_ptetacorrectionsntr1[lowpt_idx*m_netabins+loweta_idx];
    if( lowpt_idx < m_nptbins-1 )
      coeff_matrix[1][0] = m_ptetacorrectionsntr1[(lowpt_idx+1)*m_netabins+loweta_idx];
    if( loweta_idx < m_netabins-1 )
      coeff_matrix[0][1] = m_ptetacorrectionsntr1[lowpt_idx*m_netabins+(loweta_idx+1)];
    if( lowpt_idx < m_nptbins-1 && loweta_idx < m_netabins-1 )
      coeff_matrix[1][1] = m_ptetacorrectionsntr1[(lowpt_idx+1)*m_netabins+(loweta_idx+1)];
  } else {
    coeff_matrix[0][0] = m_ptetacorrectionsntr23[lowpt_idx*m_netabins+loweta_idx];
    if( lowpt_idx < m_nptbins-1 )
      coeff_matrix[1][0] = m_ptetacorrectionsntr23[(lowpt_idx+1)*m_netabins+loweta_idx];
    if( loweta_idx < m_netabins-1 )
      coeff_matrix[0][1] = m_ptetacorrectionsntr23[lowpt_idx*m_netabins+(loweta_idx+1)];
    if( lowpt_idx < m_nptbins-1 && loweta_idx < m_netabins-1 )
      coeff_matrix[1][1] = m_ptetacorrectionsntr23[(lowpt_idx+1)*m_netabins+(loweta_idx+1)];
  }
        
  corr = ( coeff_matrix[0][0] * lowpt_frac * loweta_frac ) + ( coeff_matrix[1][0] * (1-lowpt_frac) * loweta_frac );
  corr += ( coeff_matrix[0][1] * lowpt_frac * (1-loweta_frac) ) + ( coeff_matrix[1][1] * (1-lowpt_frac) * (1-loweta_frac) );

  et *= corr*fudge;

  return et;
}


bool TrigTauBugFix::correctL1GeVtoMev(TrigTauEvent* event ) 
{
  bool corrected = false;

  for(std::vector<TrigTauL1Object*>::iterator
	l2 = event->tauL1.begin(); l2 != event->tauL1.end(); ++l2) {
    
    if( (*l2)->m_TauEnergy > 3000. ) continue; // this was correct
    corrected  = true;
    (*l2)->m_EmEnergy *= 1000.;
    (*l2)->m_TauEnergy *= 1000.;
    (*l2)->m_EmIsol *= 1000.;
    (*l2)->m_HdIsol *= 1000.;
    (*l2)->m_EmCore *= 1000.;
    (*l2)->m_HdCore *= 1000.;
    
  }

  if( corrected )  m_nL1GeVtoMeV_Error++;
  return corrected; 
}


bool TrigTauBugFix::removeDuplicateTracks(TrigTauEvent* event)
{
  // fix of duplicate tracks
  bool corrected = false;
  for(unsigned int iL2 = 0;  iL2 < event->nTauL2(); ++iL2)
    corrected |= removeDuplicateTracks(&(event->tauL2Entry(iL2) ));
				       
  //for(unsigned int iL3 = 0;  iL3< event->nTauEF(); ++iL3)
  //corrected |= removeDuplicateTracks(&(event->tauL2Entry(iL3) ));
  
  if( corrected ) m_nDuplicateTrack_Error++;

  return corrected;
}

bool TrigTauBugFix::removeDuplicateTracks(TrigTauL2Object * l2)
{
  vector<TrigTauTrack> newList;
  
  unsigned int nTracks =  l2->nTracks() ;
  newList.reserve(nTracks);
  for(unsigned int itr = 0; itr < nTracks; ++itr )
    {
      const TrigTauTrack * track = &l2->track(itr);
      bool ok = true;
      for(std::vector<TrigTauTrack>::iterator jtrack = newList.begin();
	  ok && jtrack != newList.end(); jtrack++ )
	{
	  if( fabs(jtrack->Px() - track->Px())<1.e-5 &&
	      fabs(jtrack->Py() - track->Py())<1.e-5 &&
	      fabs(jtrack->Pz() - track->Pz())<1.e-5 )
	    {
	      ok = false;
	    }
	}
      if( ok ) newList.push_back(*track);
    }
  bool corrected = false;
  if( newList.size() != nTracks ) // duplicates found
    {
      corrected = true;
      l2->resetTracks();
      for(std::vector<TrigTauTrack>::iterator jtrack = newList.begin(); 
	  jtrack != newList.end(); jtrack++)
	l2->tracks.push_back(new TrigTauTrack(*jtrack));
    }
  
  return corrected;

}

bool TrigTauBugFix::resetIfl2roiWordIs0(TrigTauEvent* event)
{
  bool corrected = false;

  for(unsigned int iL2 = 0;  !corrected && iL2 < event->nTauL2(); ++iL2)
    {
      TrigTauL2Object * l2 = &event->tauL2Entry(iL2);
      if( l2->m_roiWord == 0 )
	{
	  corrected = true;
	  event->reset();
	}
    }
  

  if( corrected ) m_nDuplicateTrack_Error++;
  return corrected;

}


bool TrigTauBugFix::resetIfZeroPointerTracks(TrigTauEvent* event)
{
  bool corrected = false;

  for(unsigned int iEF = 0;  !corrected && iEF < event->nTauEF(); ++iEF)
    {
      TrigTauRecEFObject * ef = &event->tauEFEntry(iEF);

      for(unsigned int itrk = 0; !corrected && itrk < ef->nTracks(); ++ itrk)
	{
	  if( &ef->track(itrk) == 0 ){ // should be non-zero pointer, but..
	    corrected = true;
	    event->reset();
	  }
	}
    }
  

  if( corrected ) m_nZeroPointerTracks_Error++;
  return corrected;

}

bool TrigTauBugFix::attachTRTOnlyTracks(TrigTauEvent* event)
{
  bool corrected = false;

  for(unsigned int iEF = 0;   iEF < event->nTauEF(); ++iEF)
    {
      TrigTauRecEFObject * ef = &event->tauEFEntry(iEF);
      if( !ef->m_tags.isTau_TRTONLY() )continue;

      for(unsigned int itrRoI = 0; itrRoI < event->nTracksRoIEF(); ++ itrRoI)
	{
	  TrigTauTrackCollection * trkRoI = &event->trackRoIEFEntry(itrRoI);
	  if( trkRoI->m_roiWord != ef->m_roiWord ) continue;
	  for(unsigned int itr = 0; itr < trkRoI->nTracks(); ++itr)
	    {
	       TrigTauTrack * track = &trkRoI->track(itr) ;
	      if( track->m_nTRTHits &&track->m_nStrawHits  == 0 ) continue;
	      if( track->m_nPixelHits != 0 ) continue;
	      if( track->m_nSCTHits != 0 ) continue;
	      ef->r_tracks.Add(track);
	      ef->r_looseTracks.Add(track);
	      corrected = true;
	    }

	}
    }
  

  if( corrected ) m_nTRTOnlyTracks++;
  return corrected;

}




void TrigTauBugFix::printSummary() const
{
  cout << " The correction was applied for ";
  switch (m_TTP) {
  case TTP12 : cout << " TTP12 "; break;
  case TTP11 : cout << " TTP11 "; break;
  case TTP10 : cout << " TTP10 "; break;
  case TTP9 : cout << " TTP09 "; break;
  case TTP7 : cout << " TTP07 "; break;
  case TTP6 : cout << " TTP06 "; break;
  case TTP5 : cout << " TTP05 "; break;
  case TTP4 : cout << " TTP04 "; break;
  default : cout << " unknown TTP type"; break;
  }

  switch (m_fileType){
  case RDO : cout << " RDO "; break;
  case AOD : cout << " AOD "; break;
  case ESD : cout << " ESD "; break;
  }

  cout << "\nFound number of events with :\n"
       << "  L1 with MeV units                      - " <<  m_nL1GeVtoMeV_Error << endl
       << "  L2 with 0 roiWord                      - " <<  m_nL2roiWordIs0_Error << endl
       << "  with duplicated tracks                 - " << m_nDuplicateTrack_Error << endl
       << "  with EF pointing to non-existing track - " <<  m_nZeroPointerTracks_Error << endl
       << "  events where TRTOnly is attached to tauRec - " <<m_nTRTOnlyTracks  << endl
       << "out of total " << m_nTotal << " events "<< endl;

}
