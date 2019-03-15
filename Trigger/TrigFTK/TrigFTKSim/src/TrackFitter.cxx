/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/TrackFitter.h"
#include "TrigFTKSim/MultiTruth.h"
#include "TrigFTKSim/ftkdefs.h"

#include <TSystem.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

//#define DEBUG_HITEXTRAPOLATION
TrackFitter::TrackFitter() :
  m_ncoords(0), m_nplanes(0), m_npars(5),
  m_Chi2Cut(0), m_Chi2Cut_maj(0), m_Chi2Cut_vetomaj(-1), m_Chi2DofCutAux(-1), m_Chi2DofCutSSB(-1),
  m_HitWarrior(2), m_HW_ndiff(3), m_HW_dev(0),
  m_AuxDoctor(false),
  m_keep_rejected(0), m_fit_removed(0),
  m_max_ncomb(10000), m_max_nhitsperplane(-1), m_max_trkout(2000000), m_norecovery_nhits(-1),
  m_one_per_road(false), m_require_first(true), m_do_majority(1),
  // m_one_per_road(true), m_require_first(false), m_do_majority(1),
  m_roadinput(0), m_trackoutput(0), m_trackoutput_pre_hw(0),
  //m_fwoutput(0), m_maxsectors_fwo(0), m_nsectors_fwo(0), // output for firmware tests
  m_nregions(0), m_nsubregions(0),
  m_constant(0),
  m_goodRegion(0),
  m_noconstants_errlevel(ftk::sevr), m_pmap(0),
  m_ntracks(0),
  m_ntracks_pre_hw(0),
  m_ncombs(0),
  m_nfits(0),
  m_nfits_maj(0),
  m_nfits_maj_pix(0),
  m_nfits_maj_SCT(0),
  m_nfits_rec(0),
  m_nfits_addrec(0),
  m_nfits_bad(0),
  m_nfits_rej(0),
  m_nfits_badmaj(0),
  m_nfits_rejmaj(0),
  m_comb_id(0),
  combtrack(0x0),
  m_complete_mask(0),
  m_startlist(0),
  m_position(0),
  m_endlist(0),
  m_hitcnt(0),
  m_identify_badhit(false),
  m_saveStepByStepTracks(false)
{
  // nothing to do
#ifdef DEBUG_HITEXTRAPOLATION
  m_identify_badhit = true;
#endif
}


TrackFitter::~TrackFitter()
{
  // close the firmware output logfiles
  //m_fwoutput->endTrackFitterFiles();

  if (m_ncoords>0) {
    delete [] m_HW_dev;
  }

  for (int i=0;i<m_nregions;++i) {
    for (int j=0;j!=m_nsubregions;++j) {
      if (m_constant[i][j]) delete m_constant[i][j];
    }
    delete [] m_constant[i];
  }

  if (m_nregions>0) delete [] m_constant;

  if (combtrack) delete [] combtrack;
}


void TrackFitter::setBank(int ir, int isr, FTKConstantBank *banks)
{
  if (ir>=m_nregions)
    FTKSetup::PrintMessageFmt(ftk::sevr,"Region ID %d too large. Wrong number of region (%d)",ir,m_nregions);
  else if (isr>=m_nsubregions)
    FTKSetup::PrintMessageFmt(ftk::sevr,"Sub-region ID %d too large. Wrong number of sub-region (%d)",isr,m_nsubregions);

  m_constant[ir][isr] = banks;
}

void TrackFitter::setNCoordsPlanes(int nc, int np)
{
  if (m_ncoords>0) {
    delete [] m_HW_dev;
  }
  m_ncoords = nc;
  m_nplanes = np;
  m_HW_dev = new float[m_ncoords];
  for (int i=0;i<m_ncoords;++i) {
    m_HW_dev[i] = 0.;
  }
}


/** this function loads the HW distance used to decide if 2 coordinates
    represent a single real hit */
void TrackFitter::loadHWConf(const char *fname)
{
  ifstream fin(fname);
  if (!fin) {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Error loading HW configuration: %s",fname);
  }

  string line;

  // first line represents version, not used now
  getline(fin,line);

  // loop until an N in the first position is found,
  // this should change in the future, as in other map files
  while(getline(fin,line)) {
    if (line[0]=='N') break;
  }

  // start the real read of the file
  char flag;
  int nvals;
  int pos;
  float fval;

  fin >> flag >> nvals;
  if (flag!='N')
    FTKSetup::PrintMessage(ftk::sevr,"Error parsing HW configuration");
  else if (nvals!=m_ncoords) {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Error parsing HW configuration, wrong number of values %d -> %d",nvals,m_ncoords);
  }

  for (int i=0;i<nvals;++i) {
    fin >> pos >> fval;
    m_HW_dev[pos] = fval;
  }
}


void TrackFitter::prepareBanks(int nregions, int nsubregions)
{
  m_nregions = nregions;
  m_nsubregions = nsubregions;

  // propagate the number of the banks into the input and output modules
  m_roadinput->setNBanks(m_nregions);
  m_trackoutput->setNBanks(m_nregions);
  //  m_trackoutput_pre_hw->setNBanks(m_nregions);

  m_constant = new FTKConstantBank**[m_nregions];
  for (int i=0;i<m_nregions;++i) { // regions loop
    m_constant[i] = new FTKConstantBank*[m_nsubregions];

    for (int j=0;j<m_nsubregions;++j) {
      m_constant[i][j] = 0x0;
    }
  } // end regions loop
}

/** initialize the track-fitter */
void TrackFitter::init()
{
  // check which regions/towers don't have any good set of constants
  m_goodRegion = new bool[m_nregions];
  for (int i=0;i!=m_nregions;++i) {
    m_goodRegion[i] = false;

    for (int j=0;j!=m_nsubregions;++j) {
      if (m_constant[i][j]) {
	m_goodRegion[i] = true; // at least one good set of constants
	break;
      }
    }
  }

  // propagate the initialization
  m_roadinput->init(m_goodRegion);
  m_trackoutput->init(TREE_TRACK_BUFSIZE,m_goodRegion);
  //  m_trackoutput_pre_hw->init(TREE_TRACK_BUFSIZE,m_goodRegion);

  // initialize firmware output files
  //m_fwoutput->beginTrackFitterFiles();

  // open the first file
  if (!m_trackoutput->getMultiOut()) {
    // the only file in this mode has to be set before
    m_trackoutput->beginFile();
    //    m_trackoutput_pre_hw->beginFile();
  }
  else {
    TString ofname(gSystem->BaseName(m_roadinput->getCurrentFileName()));
    // add the prefix ftktracks_
    if (ofname.Contains("ftkroads")) {
      // the input file has the ftkroads prefix, it becomes ftkroads
      ofname.Replace(0,8,"ftktracks");
    }
    else
      ofname.Prepend("ftktracks_");
    m_trackoutput->beginFile(ofname.Data());
    //    m_trackoutput_pre_hw->beginFile(ofname.Data());
  }

  if (m_identify_badhit) {
    // prepare the constants to make the linear fit inversion
    for (int ireg=0;ireg!=m_nregions;++ireg) {
      for (int isub=0;isub!=m_nsubregions;++isub) {
	if (m_constant[ireg][isub]) {
	  m_constant[ireg][isub]->prepareInvConstants();
	}
      }
    }
  }
  // set the dimension of the temporary tracks
  newtrk.setNCoords(m_ncoords);
  newtrk.setNPlanes(m_nplanes);

  // iterators to loop to fill the combinations
  m_startlist = new vector<FTKHit>::const_iterator[m_nplanes];
  m_position = new vector<FTKHit>::const_iterator[m_nplanes];
  m_endlist = new vector<FTKHit>::const_iterator[m_nplanes];
  // counter that enumerates current hit in FTKHit array
  m_hitcnt = new int[m_nplanes];

  m_complete_mask = ~(~0<<m_ncoords);
  // allocate once the array of recovered tracks
  combtrack = new FTKTrack[m_ncoords];
  for (int ic=0;ic!=m_ncoords;++ic) {
    combtrack[ic].setNCoords(m_ncoords);
    combtrack[ic].setNPlanes(m_nplanes);
  }
}


int TrackFitter::nextEvent()
{
  if (!m_roadinput) {
    cerr << "*** input data module doesn't exist";
    return -1;
  }

  // move to the next event
  int res = m_roadinput->nextEvent();
  if (res<0) {
    cerr << "*** no more input data" << endl;
    return -1;
  }
  else if (res==1) {
    // syncronize the output
    if (m_trackoutput->getMultiOut()) {
       m_trackoutput->endFile();
       //       m_trackoutput_pre_hw->endFile();
      TString ofname(gSystem->BaseName(m_roadinput->getCurrentFileName()));
      // add the prefix ftktracks_
      if (ofname.Contains("ftkroads")) {
	// the input file has the ftkroads prefix, it becomes ftkroads
	ofname.Replace(0,8,"ftktracks");
      }
      else
	ofname.Prepend("ftktracks_");
      m_trackoutput->beginFile(ofname.Data());
      //      m_trackoutput_pre_hw->beginFile(ofname.Data());
    }
  }

  // clear the output module
  m_trackoutput->eventBegin();
  //  m_trackoutput_pre_hw->eventBegin();

  for (int ibank=0;ibank!=m_nregions;++ibank) { // regions loop

    processor_init(ibank);

    m_trackoutput->setRunNumber( ibank , m_roadinput->runNumber(ibank) );
    m_trackoutput->setEventNumber( ibank , m_roadinput->eventNumber(ibank) );
    // m_trackoutput_pre_hw->setRunNumber( ibank , m_roadinput->runNumber(ibank) );
    // m_trackoutput_pre_hw->setEventNumber( ibank , m_roadinput->eventNumber(ibank) );

    const FTKRoad *cur_road = m_roadinput->nextRoad(ibank);
    while (cur_road) {
      processor(*cur_road);
#if 0 // debug just a limited fraction
      if (m_nfits>=10000000)
	break;
#endif
      cur_road = m_roadinput->nextRoad(ibank);
    }
    // now that all roads in the bank are processed, associate geant
    // truth data with the resulting tracks.

    processor_end(ibank);

  } // end regions loop


  // call end event for the output module
  m_trackoutput->eventEnd();
  //  m_trackoutput_pre_hw->eventEnd();
  return res;
}


/** This method is called before the first call of the ::processor()
    method and resets the counters used to store, event-by-event
    the number of fitted tracks and other similar information */
void TrackFitter::processor_init(int /*ibank*/)
{
    // reset number of tracks, the list temporary stores
    // the tracks for a single bank
    m_ntracks = 0;
    m_comb_id = 0;
    m_tracks.clear();
    m_tracks_pre_hw.clear();
    m_tracks_hits.clear();
    m_tracks_pattern.clear();

    // reset counters
    m_ncombs = 0;
    m_nfits = 0;
    m_nfits_maj = 0;
    m_nfits_maj_pix = 0;
    m_nfits_maj_SCT = 0;
    m_nfits_rec = 0;
    m_nfits_addrec = 0;
    m_nfits_bad = 0;
    m_nfits_rej = 0;
    m_nfits_badmaj = 0;
    m_nfits_rejmaj = 0;
}


/** this method is called after the last road of a bank, in this event,
    is processed. It stores the tracks and the other global information
    on the track fitter load */
void TrackFitter::processor_end(int ibank)
{
  list<FTKTrack>::iterator itrack2 = m_tracks.begin();
  for (;itrack2!=m_tracks.end();itrack2 = m_tracks.erase(itrack2)) {
    m_trackoutput->addTrack(ibank,*itrack2);
  }

  if (m_saveStepByStepTracks) {
    itrack2 = m_tracks_pre_hw.begin();
    for (;itrack2!=m_tracks_pre_hw.end();itrack2 = m_tracks_pre_hw.erase(itrack2)) {
      m_trackoutput->addTrack_pre_hw(ibank,*itrack2);
    }

    // itrack2 = m_tracks_hits.begin();
    // for (;itrack2!=m_tracks_hits.end();itrack2 = m_tracks_hits.erase(itrack2)) {
    //   m_trackoutput->addTrack_hits(ibank,*itrack2);
    // }

    // itrack2 = m_tracks_pattern.begin();
    // for (;itrack2!=m_tracks_pattern.end();itrack2 = m_tracks_pattern.erase(itrack2)) {
    //   m_trackoutput->addTrack_pattern(ibank,*itrack2);
    // }
  }

  // itrack2 = m_tracks_pre_hw.begin();
  // for (;itrack2!=m_tracks_pre_hw.end();itrack2 = m_tracks.erase(itrack2)) {
  //   m_trackoutput_pre_hw->addTrack(ibank,*itrack2);
  // }
  m_trackoutput->addNCombs(ibank,m_ncombs);
  m_trackoutput->addNFits(ibank,m_nfits);
  m_trackoutput->addNFitsMajority(ibank,m_nfits_maj);
  m_trackoutput->addNFitsMajority_pix(ibank,m_nfits_maj_pix);
  m_trackoutput->addNFitsMajority_SCT(ibank,m_nfits_maj_SCT);
  m_trackoutput->addNFitsRecovery(ibank,m_nfits_rec);
  m_trackoutput->addNAddFitsRecovery(ibank,m_nfits_addrec);
  m_trackoutput->addNFitsBad(ibank,m_nfits_bad);
  m_trackoutput->addNFitsHWRejected(ibank,m_nfits_rej);
  m_trackoutput->addNFitsBadMajority(ibank,m_nfits_badmaj);
  m_trackoutput->addNFitsHWRejectedMajority(ibank,m_nfits_rejmaj);
  
  // m_trackoutput_pre_hw->addNCombs(ibank,m_ncombs);
  // m_trackoutput_pre_hw->addNFits(ibank,m_nfits);
  // m_trackoutput_pre_hw->addNFitsMajority(ibank,m_nfits_maj);
  // m_trackoutput_pre_hw->addNFitsMajority_pix(ibank,m_nfits_maj_pix);
  // m_trackoutput_pre_hw->addNFitsMajority_SCT(ibank,m_nfits_maj_SCT);
  // m_trackoutput_pre_hw->addNFitsRecovery(ibank,m_nfits_rec);
  // m_trackoutput_pre_hw->addNAddFitsRecovery(ibank,m_nfits_addrec);
  // m_trackoutput_pre_hw->addNFitsBad(ibank,m_nfits_bad);
  // m_trackoutput_pre_hw->addNFitsHWRejected(ibank,m_nfits_rej);
  // m_trackoutput_pre_hw->addNFitsBadMajority(ibank,m_nfits_badmaj);
  // m_trackoutput_pre_hw->addNFitsHWRejectedMajority(ibank,m_nfits_rejmaj);
}


/*******************************************
 * Function:  processor()
 * Purpose:   Finds all tracks within a given road
 * Arguments: FTKRoad
 * Comment:   Called once per road (aka pattern) from a loop in nextEvent()
*******************************************/
void TrackFitter::processor(const FTKRoad &road) {
  // check if the road was marked as rejected by the RW or HF
  if (road.getRWRejected()!=0 &&
      (m_fit_removed==0 ||
       (m_fit_removed>0 && road.getRWRejected()<m_fit_removed)) ) {
    // if asked rejected roads can be fitted for debugging reason
    return;
  }
  if (road.getHFRejected())
    return;

   // retrieve all the informations for the fits
  int region = road.getRegion(); // a.k.a. bank
  int subreg = road.getSubRegion();
  int sector = road.getSectorID();
  int nplanes = road.getNPlanes();

  // this is needed because 4L and 11L subregions are not correlated
  // given a 4L road in subA we use the lookup to get 11L sector in subB, where B not necessarily equal A
  if(FTKSetup::getFTKSetup().getSCTtrkMode()){
    subreg=sector%m_nsubregions;
    sector=sector/m_nsubregions;
  }

  // get the bank related to this road
  FTKConstantBank *current_bank = m_constant[region][subreg];
  if (!current_bank) { // no valid constants
    FTKSetup::PrintMessageFmt(m_noconstants_errlevel,
            "*** Constants for region %d - %d don't exists",
            region,subreg);
    return;
  }
  // check if the number of sector is compatible with this bank
  else if (sector>=current_bank->getNSectors()) {
    FTKSetup::PrintMessageFmt(m_noconstants_errlevel,
            "*** Constants for Sector %d in region %d - %d don't exists\n",
            sector,region,subreg);

    return;
  }
  // check if the sector has a valid constants set
  else if (!current_bank->getIsGood(sector)) {
    FTKSetup::PrintMessageFmt(m_noconstants_errlevel,
            "*** Constants for Sector %d in region %d - %d is not valid\n",
            sector,region,subreg);

    return;
  }

  // iterators to loop to fill the combinations
  vector<FTKHit>::const_iterator *startlist = new vector<FTKHit>::const_iterator[nplanes];
  vector<FTKHit>::const_iterator *position = new vector<FTKHit>::const_iterator[nplanes];
  vector<FTKHit>::const_iterator *endlist = new vector<FTKHit>::const_iterator[nplanes];

  // counter that enumerates current hit in FTKHit array
  int *hitcnt = new int[nplanes];

   // mask to prevent majority in planes with multiple hits
   unsigned int norecovery_mask(0);

   // number of combinations
   int ncomb(1);
   int nmissing(m_ncoords);
   bool missPix(false);
   bool missSCT(false);
   unsigned int bitmask(0);
   for (int p=0;p<nplanes;++p) {
     int nhits = road.getNHits(p);
     if (nhits==0) {
       // check if the planes has hits or not
       startlist[p] = endlist[p] = position[p] = vector<FTKHit>::const_iterator(); // empty iterator, this will flag empty lists
       hitcnt[p]=-1;
       // set the fake hit in the empty layer
       newtrk.setFTKHit(p,FTKHit());
       int iy = m_pmap->getDim(p,1); // use to determine if plane with no hits is PIX or SCT
       if (iy == -1) missSCT = true; // SCT
       else missPix = true; // Pix
     }
     else {
       // set the list iterators
       position[p] = startlist[p] = road.getHits(p).begin();
       endlist[p] = road.getHits(p).end();
       ncomb  *= nhits;
       hitcnt[p] = 0;
       // set the bitmask and nmissing, the same for all the combs
       // in this road

       // retrieve the coordinate in the final array
       int ix = m_pmap->getDim(p,0);
       int iy = m_pmap->getDim(p,1);
       // decide if a 1D or 2D hit
       int ndim = iy==-1 ?  1 : 2;
       if (ndim==1) {
   bitmask |= 1 << ix;
   nmissing -= 1;
       }
       else {
   bitmask |= ( (1<<ix) | (1<<iy));
   nmissing -= 2;
       }
       // set the mask to avoid recovery in crowded planes
       // if m_norecovery
       if (m_norecovery_nhits!=-1 && nhits>m_norecovery_nhits)
   norecovery_mask |= (1<<p);
     }
   }

   m_ncombs += ncomb;

   // output for firmware tests
   //bool dump_fwo = (nmissing == 0 && m_nsectors_fwo < m_maxsectors_fwo );
   //if( dump_fwo ) {
   //  m_fwoutput->beginRoad( m_nsectors_fwo , region , subreg , sector , road.getRoadID() );
   //  m_nsectors_fwo++;
   //  m_fwoutput->addFullRoad( road );
   //  m_fwoutput->addConstants( current_bank , sector );
   //}

   // newtrk is "global", the road related values can be set here
   // for all the combinations
   newtrk.setBankID(road.getBankID());
   // the pattern ID is more unique and simple to track that the road ID.
   // If the DB ID is set this is preferred.
   newtrk.setRoadID(road.getRoadID());
   newtrk.setPatternID(road.getPatternDBID()>0 ? road.getPatternDBID() : road.getPatternID());
   newtrk.setSectorID(road.getSectorID());
   newtrk.setBitmask(bitmask);
   newtrk.setNMissing(nmissing);

   // the base HW flag depends if the road is accepted by the RW
   int HWbase = road.getRWRejected()*10;

   // FlagJT: Vector to hold all of the combinations in order to pick
   // the best-chi2 fit with the most number of hits. If we're not
   // just doing one track/road, then this just holds all of the
   // combinations.
   std::vector<FTKTrack> theCombos;
   int min_nmissing = nplanes; // 11

   // FlagAK: don't process more than m_max_ncomb. Relevant for 10^34 events
   for (int comb=0;comb<(ncomb>m_max_ncomb ? m_max_ncomb : ncomb);++comb) {

     /* FlagAK:
  Decide if this hit combination survived hit filter (optional)
  If hit filter had not been run, this doesn't do anything
      */
     int hf_nmatches(0);
     for (int ip=0;ip<nplanes;++ip) {
       if(hitcnt[ip]==-1) { // this plane doesn't have any hits
   hf_nmatches++;
       }
       else { // this plane has some hits. Test if current hit passed hit filter
   if(!road.hit_failed_HF(ip,hitcnt[ip])) hf_nmatches++;
       }
     }
     int hf_rejected = (hf_nmatches != nplanes);

     /* Try to fit a track */
     if (m_ntracks>=m_max_trkout) {
       FTKSetup::PrintMessage(ftk::warn,"processor:too many tracks");
       break;
     } else if (!hf_rejected) {
       // this block uses the "newtrk" variable that is a field of the object

       for (int ip=0;ip<nplanes;++ip) {
   // retrieve the coordinate in the final array
   int ix = m_pmap->getDim(ip,0);
   int iy = m_pmap->getDim(ip,1);
   // decide if a 1D or 2D hit
   int ndim = iy==-1 ?  1 : 2;
   if (position[ip]!=endlist[ip]) {
     // set the FTK cluster associated to this layer
     newtrk.setFTKHit(ip,(*position[ip]));

     if (ndim==1) {
       newtrk.setCoord(ix,(*position[ip])[0]);
     }
     else {
       newtrk.setCoord(ix,(*position[ip])[0]);
       newtrk.setCoord(iy,(*position[ip])[1]);
     }
   }
       }
       newtrk.setTrackID(m_comb_id);
       newtrk.setCombinationID(comb);
       newtrk.setHWRejected(HWbase);
       newtrk.setHWTrackID(-1);

       // see if pixel b-layer is required
       bool blayer_condition = m_require_first ? (newtrk.getBitmask() & 1) : 1;
       // only keep this combination if there are enough hits,
       // otherwise go to next combination.
       if( blayer_condition &&
     (!m_one_per_road || (m_one_per_road && nmissing <= min_nmissing) ) ) {

   if( m_one_per_road && nmissing < min_nmissing ) {
     // Don't consider any combinations with more missing hits
     min_nmissing = nmissing;
     for( std::vector<FTKTrack>::iterator citer = theCombos.begin();
    citer != theCombos.end(); citer++ ) {
       if( (*citer).getNMissing() > min_nmissing ) {
         // Presumably this would be handled before this stage
         // in the real FTK hardware.
         citer = theCombos.erase(citer);
         citer--;
         m_nfits_maj--;
         m_nfits--;
         if (missSCT) m_nfits_maj_SCT--;
         if (missPix) m_nfits_maj_pix--;
       }
     }
   }

   // add one fit in the counters
   m_nfits++;
   if (nmissing>0) {
     m_nfits_maj++;
     if (missPix) m_nfits_maj_pix++;
     if (missSCT) m_nfits_maj_SCT++;
   }


   /* Do the actual fit - see code in FTKConstantBank::linfit  */
   current_bank->linfit(sector,newtrk);
   newtrk.setOrigChi2(newtrk.getChi2());

   // output for firware tests
   //if( dump_fwo ) m_fwoutput->addTrack( newtrk );

   if( newtrk.getChi2() != 0 )
     theCombos.push_back(newtrk);
       } // end if nmissing <= min_nmissing
     } // end if !hf_rejected
     /* compose the next combination */
     for (int curpl=0;curpl<nplanes;++curpl) {
       if (startlist[curpl]==endlist[curpl]) // empty hit
   continue;

       // move to the next
       position[curpl]++;  hitcnt[curpl]++;
       if (position[curpl]!=endlist[curpl] && hitcnt[curpl]!=m_max_nhitsperplane) {
   // possible combination
   break;
       }
       else {
   // restart on this plane, move the next plane
   position[curpl] = startlist[curpl];
   hitcnt[curpl] = 0;
       }
     }

     ++m_comb_id; // increment the combination counter
   } // loop over combinations

   delete [] startlist;
   delete [] endlist;
   delete [] position;
   delete [] hitcnt;

   if( m_one_per_road ) {
     if( theCombos.size() != 0 ) {

       // Find the best chi2 track
       float bestfullchi2(theCombos[0].getChi2());
       int idbestfull(0);

       for( unsigned int idx = 1; idx < theCombos.size(); idx++) {
   if( theCombos[idx].getChi2() < bestfullchi2 ) {
     bestfullchi2 =  theCombos[idx].getChi2();
     idbestfull = idx;
   }
       }
       newtrk = theCombos[idbestfull];
       theCombos.clear();
       theCombos.push_back(newtrk);
       // output for firmware tests
       //if( dump_fwo ) m_fwoutput->addBestTrack( newtrk );
     }
   }

   // output for firmware tests
   //if( dump_fwo ) m_fwoutput->endRoad();

   // In the case of m_do_majority > 1, we only do majority fits if
   // ALL full fits fail the chi2 cut
   bool fullpassed = false;
   if( m_do_majority > 1 ) {
     for( unsigned int idx = 0; idx < theCombos.size(); idx++ ) {
       newtrk = theCombos[idx];
       if( newtrk.getNMissing() != 0 ) continue;

       float dof = m_ncoords - m_npars - newtrk.getNMissing();
       if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

       if( newtrk.getChi2() < ( m_Chi2DofCutAux > -1 ? dof*m_Chi2DofCutAux : m_Chi2Cut ) ) {
   fullpassed = true;
   break;
       }
     }
   }

   for( std::vector<FTKTrack>::iterator citer = theCombos.begin();
  citer != theCombos.end(); citer = theCombos.erase(citer) ) {
     newtrk = *citer;

     float dof = m_ncoords - m_npars - newtrk.getNMissing();
     if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

#ifdef DEBUG_HITEXTRAPOLATION
     if (newtrk.getNMissing()==0) {
       FTKTrack invtrack(newtrk);
       current_bank->invlinfit(sector,invtrack); // perfect extrapolation
       // Print the original and extrapolated hits
       cout << "DBGHIT OR";
       for (int ix=0;ix!=m_ncoords;++ix) cout << " " << newtrk.getCoord(ix);
       cout << " " << newtrk.getChi2() << endl;
       cout << "DBGHIT EX";
       for (int ix=0;ix!=m_ncoords;++ix) cout << " " << invtrack.getCoord(ix);
       cout << endl;
     }
#endif
     // Try to recover majority if chi2 no good
     if (newtrk.getNMissing()==0 &&
   (m_do_majority==1 || (m_do_majority>1 && !fullpassed)) &&
   // Do recovery if chi2 or chi2/dof is above threshold
   (newtrk.getChi2() > ( m_Chi2DofCutAux > -1 ? dof*m_Chi2DofCutAux : m_Chi2Cut )) &&
   // Or veto majority if chi2 is too high
   (newtrk.getChi2() < m_Chi2Cut_vetomaj || m_Chi2Cut_vetomaj < 0) ) { // recover majority
       /* if the N/N track have a bad chi2 we try to evaluate
    the track using the "best" track within the N combination
    of N-1/N tracks */

       // in this block the used array of track combtrack[] is created
       // in the init block to save some time

       float bestchi2(newtrk.getChi2());
       int idbest(-1);
       m_nfits_rec += 1;
       for (int ip=(m_require_first?1:0);ip<nplanes;++ip) { // loop over the combinations

   // skip planes with multiple hits
   if (norecovery_mask&(1<<ip)) continue;

   // start for the complete track
   combtrack[ip] = newtrk;

   // zero the cluster for the missing layer
   combtrack[ip].setFTKHit(ip,FTKHit());

   // remove the hits related with this plane
   int ix = m_pmap->getDim(ip,0);
   int iy = m_pmap->getDim(ip,1);
   int ndim = iy==-1? 1 : 2;

   // set the number of missing points and the related bitmask
   combtrack[ip].setNMissing(newtrk.getNMissing()+ndim);
   unsigned int newbitmask = (1<<ix);
   if (iy!=-1) newbitmask |= (1<<iy);
   newbitmask = newtrk.getBitmask() & ~newbitmask;

   combtrack[ip].setBitmask(newbitmask);

   // increment the counter of additional fits due to the recovery
   m_nfits_addrec += 1;
   // attempt the fit
   current_bank->linfit(sector,combtrack[ip]);

   // check if the chi2 is better
   if (combtrack[ip].getChi2()<bestchi2) {
     bestchi2 = combtrack[ip].getChi2();
     idbest = ip;
   }
       } // end loop over the combinations

       if (idbest>=0) {
   // a majority track was found
   newtrk = combtrack[idbest];
#ifdef DEBUG_HITEXTRAPOLATION
       // Print the best recovery track
       cout << "DBGHIT RC";
       for (int ix=0;ix!=m_ncoords;++ix) cout << " " << newtrk.getCoord(ix);
       cout << " " << newtrk.getChi2() << " " << newtrk.getBitmask() << endl;
#endif
       }

     } // end block to recover complete tracks with bad chi2

     dof = m_ncoords - m_npars - newtrk.getNMissing();
     if( dof < 1 ) dof = 1e30; // Just pass all tracks with too few dof

     // check if the track pass the quality requirements
     if (newtrk.getChi2()< ( m_Chi2DofCutAux > -1 ? dof*m_Chi2DofCutAux :
           (newtrk.getNMissing() > 0 ? m_Chi2Cut_maj : m_Chi2Cut) )  &&
   newtrk.getChi2() != 0 ) {

       // to append the found track go trought the HW filter
       // add this track to track list only if
       // don't have common hits with another track,
       // in this case keep the best

       // remains 0 if the track has to be added
       // -1 means is worse than an old track (duplicated)

       int accepted(0);

       // Disable hitwarrior, auto-accept every track
       if (m_HitWarrior!=0)
   accepted = doHitWarriorFilter(newtrk,m_tracks);

       if (accepted>=0) { // track accepted, no hits shared with already fitted tracks
   // match the track to a geant particle using the channel-level
   // geant info in the superstrip data.
   compute_truth(region,road,newtrk);
   // copy newtrk after truth assignment.
   m_tracks.push_back(newtrk);
   if (accepted==1) {
     // this track removed an old one
     m_nfits_rej += 1;
     if (nmissing>0) m_nfits_rejmaj++;
   }
   else {
     ++m_ntracks;
   }
       }
       else if (accepted==-1) {
   // the track is candidate as ghost and is not the best
   // clean the list of the hits

   if (m_keep_rejected) {
     // match the track to a geant particle using the channel-level
     // geant info in the superstrip data.
     compute_truth(region,road,newtrk);
     // copy newtrk after truth assignment.
     m_tracks.push_back(newtrk);
   }

   m_nfits_rej += 1;
   if (nmissing>0) m_nfits_rejmaj++;
       }
     }
     else { // if the track fail the cut on chi-square clean the hit list
       if (m_keep_rejected>1) {
   newtrk.setHWRejected( newtrk.getHWRejected()+100 );

   // match the track to a geant particle using the channel-level
   // geant info in the superstrip data.
   compute_truth(region,road,newtrk);
   // copy newtrk after truth assignment.
   m_tracks.push_back(newtrk);
       }
       m_nfits_bad += 1;
       if (nmissing>0) m_nfits_badmaj++;
     }
   } // end insertion block

}


void
TrackFitter::compute_truth(const unsigned int& /*ibank*/,const FTKRoad& road,FTKTrack& newtrk) const
{
  const bool debug = false;
  // retrieve the superstrips for this road and recover the truth
  // data for the hit used on the superstrip.
  static vector<MultiTruth> mtv;
  mtv.clear();
  mtv.reserve( road.getNPlanes() );
  if( debug ) {
    // dump all ssid coordinates and all track coordinates.
    cout << " FINAL ROAD SS DUMP: pl ssid c0" << endl;
    for( unsigned int i=0, f=road.getNPlanes(); i!=f; ++i ) {
      const int ssid = road.getSSID(i);
      const std::vector<FTKHit>& hits(road.getHits(i));
      for( std::vector<FTKHit>::const_iterator ihit=hits.begin(), fhit=hits.end(); ihit!=fhit; ++ihit ) {
        const FTKHit& hit = *ihit;
        cout << i << " " << ssid << " " << hit[0] << endl;
        unsigned int index = std::distance( hits.begin() , ihit );
        road.getTruth(i,index).display();
      }
    }
    cout << " FINAL TRACK COORD DUMP: pl bitmask_bit c0" << endl;
    cout << " bitmask: " << newtrk.getBitmask() << endl;
    cout << " nmissing: " << newtrk.getNMissing() << endl;
    for( unsigned int i=0, f=road.getNPlanes(); i!=f; ++i ) {
      const int ix = m_pmap->getDim(i,0);
      const float xcoord = newtrk.getCoord(ix);
      const bool has_hit = (newtrk.getBitmask() & (1 << ix));
      cout << i << " " << has_hit << " " << xcoord << endl;
    }
  } // end if dump debugging info to stdout
  for( unsigned int i=0, f=road.getNPlanes(); i!=f; ++i ) {
    //const int ssid = road.getSSID(i);
    //FTKSS& ss = const_cast<FTKSS&>( m_roadinput->getSS(ibank,i,ssid) );
    //const std::vector<FTKHit>& hits(ss.getHits());
    const std::vector<FTKHit>& hits = road.getHits(i);
    // retrieve the coordinate in the final array
    const int ix = m_pmap->getDim(i,0);
    const float xcoord = newtrk.getCoord(ix);
    // find the hit index into the superstrip for this
    // coordinate. this is tricky---only the coordinate is
    // saved, so it is the only way we have of identifying
    // which geant truth data to use.
    std::vector<FTKHit>::const_iterator match_hit=hits.end();
    for( std::vector<FTKHit>::const_iterator ihit=hits.begin(), fhit=hits.end(); ihit!=fhit; ++ihit ) {
      const FTKHit& hit = *ihit;
      assert( hit.getDim()>=1 );
      if( true && (hit.getDim()==2) && (hit[0]==xcoord) && (hit[1]==newtrk.getCoord(m_pmap->getDim(i,1))) ) {
        match_hit = ihit;
        break;
      } else if( hit[0] == xcoord ) {
        match_hit = ihit;
        break;
      }
    }
    // by construction, superstrip must contain the hit used on this track, or
    // the track must not have a hit in this plane.
    if( (newtrk.getBitmask() & (1 << ix)) == 0 ) { continue; } // no hit in this plane
    assert( match_hit != hits.end() );
    //unsigned int index = std::distance( hits.begin() , match_hit );
    //assert( static_cast<int>(index)<=ss.getNHits() );
    // adjust weight for hits without (and also with) a truth match, so that each is counted with the same weight.
    //MultiTruth this_mt( ss.getTruth(index) );
    MultiTruth this_mt( (*match_hit).getTruth() );
    this_mt.assign_equal_normalization();
    mtv.push_back( this_mt );
    if( debug ) {
      unsigned int index = std::distance( hits.begin() , match_hit );
      cout << "added MultiTruth for " << index << " ";
      this_mt.display();
    }
  }
  // compute the best geant match, the barcode with the largest number of hits contributing to the track.
  // frac is then the fraction of the total number of hits on the track attributed to the barcode.
  MultiTruth mt( std::accumulate(mtv.begin(),mtv.end(),MultiTruth(),MultiTruth::AddAccumulator()) );
  // retrieve the best barcode and frac and store in the track.
  MultiTruth::Barcode tbarcode;
  MultiTruth::Weight tfrac;
  const bool ok = mt.best(tbarcode,tfrac);
  if( ok ) {
    newtrk.setEventIndex(tbarcode.first);
    newtrk.setBarcode(tbarcode.second);
    newtrk.setBarcodeFrac(tfrac);
  }
  // dump matching info to stdout
  if( debug ) {
    mt.display();
    cout << " TRACK MATCHED TO " << tbarcode.first << "," << tbarcode.second << " " << tfrac << " " << mtv.size() << endl;
    std::for_each( mtv.begin() , mtv.end() , mem_fun_ref( &MultiTruth::display ) );
  }
}

/** this method attempets to add a track into a list of tracks,
    before to do that applies the usual HW filter */
int TrackFitter::doHitWarriorFilter(FTKTrack &track_toadd,list<FTKTrack> &tracks_list, bool isfirst)
{
  // remains 0 if the track has to be added
  // -1 means is worse than an old track (duplicated)
  // 1 means has to be added and removed an old trakc
  int accepted(0);

  list<FTKTrack>::iterator itrack = tracks_list.begin();
  for (;itrack!=tracks_list.end();++itrack) { // loop over tracks of this bank

    // reference to an old track
    FTKTrack &track_old = *itrack;
    if (track_old.getHWRejected()/100 !=0 || ( (!isfirst) && track_old.getHWRejected()%10 !=0) ) //We still want to compare to duplicate tracks
    //if (track_old.getHWRejected()%10 !=0 || track_old.getHWRejected()/100!=0)
      // skip HW rejected tracks and tracks with bad chi2,
      // tracks in removed roads are used, but if a match is found
      // will be always rejected or marked as removed, if compared
      // to a combination in a good road
      continue;

    //Slightly different logic on first stage (AUX) vs. 2nd stage (SSB)
    int HWres = isfirst ? track_toadd.HWChoiceFirstStage(track_old,m_HW_dev,m_HW_ndiff,m_HitWarrior) : track_toadd.HWChoice(track_old,m_HW_dev,m_HW_ndiff,m_HitWarrior);

    if (HWres==-1) {
      accepted = -1;
      // passing the end of the list means remove current
      if (isfirst) { //for first stage processing, don't remove tracks from list yet as they're needed for duplicate removal comparisons
        removeTrackStage1(tracks_list,tracks_list.end(),track_toadd,track_old,true);
      } else {
        removeTrack(tracks_list,tracks_list.end(),track_toadd,track_old,true);
      }
    }
    else if (HWres==1) {
      accepted = 1;
      // return the new current position, it changes if the
      // rejected track is removed from the list
      if (isfirst) {
        itrack = removeTrackStage1(tracks_list,itrack,track_old,track_toadd);
      } else {
        itrack = removeTrack(tracks_list,itrack,track_old,track_toadd);
      }
    }

  } // end loop over tracks of this bank

  return accepted;
}

/** This is the newly added overlap removal method Aux Doctor. It checks the SSIDs,
    which are extrapolated Aux track coordinates, and performs the overlap removal. **/
int TrackFitter::doAuxDoctor(FTKTrack &track_toadd,list<FTKTrack> &tracks_list)
{
  // remains 0 if the track has to be added
  // -1 means it's a duplicate
  int accepted_AuxDoctor(0);

  list<FTKTrack>::iterator itrack = tracks_list.begin();

  for (;itrack!=tracks_list.end();++itrack) { // loop over tracks of this bank
    // reference to an old track
    FTKTrack &track_old = *itrack;

    bool toadd = false;
    for (int i=0; i<track_toadd.getNPlanesIgnored(); i++) {
      if (track_toadd.getSSID(i) != track_old.getSSID(i)) {
        toadd = true;
        break;
      }
    }

    if (!toadd) {
      accepted_AuxDoctor = -1;
      // passing the end of the list means remove current
      removeTrack(tracks_list,tracks_list.end(),track_toadd,track_old,true);
    }
  } // end loop over tracks of this bank

  return accepted_AuxDoctor;
}

/** rejected, or mark as rejected, the track in the list
    against  the second track */
list<FTKTrack>::iterator TrackFitter::removeTrack(list<FTKTrack> &tracks_list, list<FTKTrack>::iterator itrack,
						  FTKTrack &rejtrk, const FTKTrack &killer, bool rejnew)
{

  if (!rejnew) { // is rejecting a track in the list
    if (!m_keep_rejected) { // remove from the list the rejected
      // save the position before this
      list<FTKTrack>::iterator before = itrack;
      // move the to the previous track
      --before;

      // remove the old track from the list
      tracks_list.erase(itrack);

      // assign the itrack to the previous position before, to not invalidate the loop
      itrack = before;
    }
    else if  (rejtrk.getHWRejected()%10==0) { // mark only if not yet marked
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());
      //m_nfits_rej += 1;
    }

  }
  else {  // is rejecting the new track, not yet in the list
    if (rejtrk.getHWRejected()%10==0) { // mark only the first time
      // the new track is rejected
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());

      //m_nfits_rej += 1;
    }
  }

  return itrack;
}

//Track remover for first stage does not delete tracks, as they are still needed for HW duplciate comparison

list<FTKTrack>::iterator TrackFitter::removeTrackStage1(list<FTKTrack> &tracks_list, list<FTKTrack>::iterator itrack,
                          FTKTrack &rejtrk, const FTKTrack &killer, bool rejnew)
{

  if (!rejnew) { // is rejecting a track in the list
    if  (rejtrk.getHWRejected()%10==0) { // mark only if not yet marked
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());
      //m_nfits_rej += 1;
    }

  }
  else {  // is rejecting the new track, not yet in the list
    if (rejtrk.getHWRejected()%10==0) { // mark only the first time
      // the new track is rejected
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());

      //m_nfits_rej += 1;
    }
  }

  return itrack;
}
