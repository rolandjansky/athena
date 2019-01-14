/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FTK_RecTools/FTK_DuplicateTrackRemovalTool.h"
#include <vector>
#include "TVector2.h"
#include "CLHEP/Units/PhysicalConstants.h"

FTK_DuplicateTrackRemovalTool::FTK_DuplicateTrackRemovalTool(const std::string& t,
                                               const std::string& n,
                                               const IInterface*  p ):
  AthAlgTool(t,n,p),
  m_trks_nodups(NULL),
  m_HW_ndiff(6),
  m_dphi_roughmatch(0.3)
{
  declareInterface< IFTK_DuplicateTrackRemovalTool >( this );
  declareProperty("HW_ndiff",m_HW_ndiff);
  declareProperty("dphi_roughmatch",m_dphi_roughmatch);
}

StatusCode FTK_DuplicateTrackRemovalTool::initialize() {

  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  m_trks_nodups = new FTK_RawTrackContainer(SG::VIEW_ELEMENTS);//we don't own the tracks, we're just going to hold them

  athenaLog << MSG::INFO << "FTK_DuplicateTrackRemovalTool initialized "<< endmsg;
  return sc;
}

StatusCode FTK_DuplicateTrackRemovalTool::finalize() {
  StatusCode sc = AlgTool::finalize();
  m_trks_nodups->clear();
  delete m_trks_nodups;
  return sc;
}

//tell whether the two tracks match, based on number of matching hits and unmatching hits
bool FTK_DuplicateTrackRemovalTool::match(const FTK_RawTrack* track, const FTK_RawTrack* oldtrack) const {

	const std::vector<FTK_RawPixelCluster>& pixclus = track->getPixelClusters();
	const std::vector<FTK_RawSCT_Cluster>& sctclus = track->getSCTClusters();

	//Note to self: Clusters are made from sim in FTK_RDO_CreatorAlgo...

	int nclus = pixclus.size() + sctclus.size();
	int nmatchingclus=0;
	int nclusleft=12;// there's at most 12 hits, 4 pixel and 8 sct
	const std::vector<FTK_RawPixelCluster>& oldpixclus = oldtrack->getPixelClusters();
	for (auto clus : pixclus){
		//is this pixel clus matched by any on the old track?
		unsigned int id = clus.getModuleID();
		for (auto oldclus : oldpixclus){
			if (oldclus.getModuleID()==id){ // if the ID matches, it's on the same module // stored in WordA
				if (clus.getWordB()==oldclus.getWordB()) { // is it the same eta and phi position and width? // stored in WordB
					nmatchingclus++;

					//it matches if the number of unmatched clusters is <= 6 (or HW_diff)
					if ( (nclus-nmatchingclus) <= m_HW_ndiff){ //corresponding criteria in simulation
						return true; //return as soon as we know the answer!
					}
					break; //we already found a matching cluster to this cluster, so no need to look for more matches to this cluster
				}
			}
		}//loop over old tracks' clusters
		nclusleft--;
		if ((nclus-nmatchingclus-nclusleft)>m_HW_ndiff) return false; // no way we can get there with just nclusleft remaining
	}

	const std::vector<FTK_RawSCT_Cluster>& oldsctclus = oldtrack->getSCTClusters();
	for (auto clus : sctclus){
		//is this sct clus matched by any on the old track?
		unsigned int id = clus.getWord();
		for (auto oldclus : oldsctclus){
			if (oldclus.getWord()==id){ // if the word matches, it's the same dude
				nmatchingclus++;

				//it matches if the number of unmatched clusters is <= 6 (or HW_diff)
				if ( (nclus-nmatchingclus) <= m_HW_ndiff){ //corresponding criteria in simulation
					return true; //return as soon as we know the answer!
				}
				break; //we already found a matching cluster to this cluster, so no need to look for more matches to this cluster
			}
		}//loop over old tracks' clusters
		nclusleft--;
		if ((nclus-nmatchingclus-nclusleft)>m_HW_ndiff) return false; // no way we can get there with just nclusleft remaining
	}

	//ATH_MSG_DEBUG("Found "<<nmatchingpixclus<<" matching pix clus out of "<<pixclus.size());
	//ATH_MSG_DEBUG("Found "<<nmatchingsctclus<<" matching sct clus out of "<<sctclus.size());

	//ATH_MSG_VERBOSE("ACH888: "<<matching<<" "<<track->getSectorID()<<" "<<track->getPhi()<<" "<<track->getCotTh()<<" "<<track->getZ0()<<" "<<oldtrack->getSectorID()<<" "<<oldtrack->getPhi()<<" "<<oldtrack->getCotTh()<<" "<<oldtrack->getZ0());
	return false;
}

//return the better of two tracks, based on number of missing layers, and if those tie, chi2
const FTK_RawTrack* FTK_DuplicateTrackRemovalTool::besttrack(const FTK_RawTrack* track, const FTK_RawTrack* oldtrack) const {
	//return track;//ACH-temporary test to see if this is slow

	unsigned int trackhits = track->getPixelClusters().size()+track->getSCTClusters().size();
	unsigned int oldtrackhits = oldtrack->getPixelClusters().size()+oldtrack->getSCTClusters().size();
	if (trackhits > oldtrackhits) return track;
	if (oldtrackhits > trackhits) return oldtrack;

	//in case of a tie, use chi2
	if (track->getChi2() < oldtrack->getChi2()) return track; // smaller chi2 wins
	else return oldtrack;
}

#ifdef FTKDuplicateTrackRemovalUseMap
void FTK_DuplicateTrackRemovalTool::addtophimap(double trackphi, unsigned int pos){
	m_phimap[trackphi].push_back(pos);//add it to the map

	//check that track phi is within -pi to pi
	if (trackphi<(-CLHEP::pi-0.1)) ATH_MSG_WARNING("FTK track with phi < -pi! "<<trackphi);
	else if (trackphi>(CLHEP::pi+0.1)) ATH_MSG_WARNING("FTK track with phi > pi! "<<trackphi);

	//add extra copy in case of wraparound
	if (trackphi>(CLHEP::pi-m_dphi_roughmatch)) m_phimap[trackphi-CLHEP::twopi].push_back(pos);
	else if (trackphi<(-CLHEP::pi+m_dphi_roughmatch)) m_phimap[trackphi+CLHEP::twopi].push_back(pos);
}
void FTK_DuplicateTrackRemovalTool::removefromphimap(double oldtrackphi, unsigned int e){

	auto& vec = m_phimap[oldtrackphi];
	auto ind = std::find(vec.begin(),vec.end(),e);
	if (ind==vec.end()) ATH_MSG_WARNING("Wasn't in the map?! "<<oldtrackphi<<" "<<e);
	else vec.erase(ind);

	//take of removing wraparound entries
	if (oldtrackphi>(CLHEP::pi-m_dphi_roughmatch)) {
	  vec = m_phimap[oldtrackphi-CLHEP::twopi];
	  auto ind = std::find(vec.begin(),vec.end(),e);
	  if (ind==vec.end()) ATH_MSG_WARNING("Wasn't in the map?! "<<oldtrackphi<<" "<<e);
	  else vec.erase(ind);
	}
	else if (oldtrackphi<(-CLHEP::pi+m_dphi_roughmatch)) {
	  vec = m_phimap[oldtrackphi+CLHEP::twopi];
	  auto ind = std::find(vec.begin(),vec.end(),e);
	  if (ind==vec.end()) ATH_MSG_WARNING("Wasn't in the map?! "<<oldtrackphi<<" "<<e);
	  else vec.erase(ind);
	}
}
#endif

FTK_RawTrackContainer* FTK_DuplicateTrackRemovalTool::removeDuplicates(const FTK_RawTrackContainer* trks){

#ifdef FTKDuplicateTrackRemovalTiming
  clock_t tStart = clock();
  for (int tim=0;tim<1000;++tim){
#endif

#ifdef FTKDuplicateTrackRemovalUseMap
    m_phimap.clear();
    std::set<unsigned int> trackstokill;
#endif

  ATH_MSG_DEBUG("I'm in removeDuplicates!");
  m_trks_nodups->clear();
  m_trks_nodups->reserve(trks->size());
  for (unsigned int i = 0; i!=trks->size(); i++) {
    const FTK_RawTrack *track = trks->at(i);
    
    //now we should see whether this track overlaps with one (or more?) tracks already in the nodups container
    std::vector<unsigned int> matching_oldtracks;
    
#ifdef FTKDuplicateTrackRemovalUseMap
    //search just the range of phi in the map of old tracks near the phi of this new track
    //the map goes from -pi-dphi_roughmatch to pi+dphi_roughmatch, to handle wraparound
    double trackphi = track->getPhi();
    auto lower = m_phimap.lower_bound(trackphi-m_dphi_roughmatch);
    auto upper = m_phimap.upper_bound(trackphi+m_dphi_roughmatch);
    for (auto it=lower; it!=upper; it++){
      for (unsigned int e : it->second) {//these are the indices of the old tracks at each phi value in the phi range
	//ATH_MSG_DEBUG("Looking for match of track "<<i<<" with oldtrack "<<e);
	const FTK_RawTrack *oldtrack = m_trks_nodups->at(e);
	if (this->match(track,oldtrack)) {
	  matching_oldtracks.push_back(e);
	}
      }
    }
#else
    //loop over all old tracks and see if they match
    for (unsigned int e = 0; e!=m_trks_nodups->size(); e++) {
      const FTK_RawTrack *oldtrack = m_trks_nodups->at(e);
      
      //first check for a rough match in phi
      double dphi = TVector2::Phi_mpi_pi(track->getPhi()-oldtrack->getPhi());//make sure it's in -pi..pi
      dphi = fabs(dphi);//then take abs since we don't care about sign
      if (dphi>m_dphi_roughmatch) continue;//no match if dphi is larger than dphi_roughmatch, 0.3 by default
      
      if (this->match(track,oldtrack)) {
	matching_oldtracks.push_back(e);
      }
    }
#endif

    //ATH_MSG_DEBUG("Found "<<matching_oldtracks.size()<<" old tracks matching track "<<i);
    if (matching_oldtracks.size()==0){//if there's no match, just add the new track
#ifdef FTKDuplicateTrackRemovalTestMultiple
      for (int j=0; j<3; ++j){ // ACH - temporary test - add multiple times, to test multiple matches!
#endif
#ifdef FTKDuplicateTrackRemovalUseMap
	addtophimap(trackphi,m_trks_nodups->size());//so e.g. the first track will be index 0, since the size is 0 just before we push_back the first track
#endif
	m_trks_nodups->push_back((FTK_RawTrack*)track);
#ifdef FTKDuplicateTrackRemovalTestMultiple
      } // ACH - temporary test - add multiple times, to test multiple matches!
#endif
    }
    
    //if it does match, either replace the matching track(s) with this new track, or ignore this new track, depending on which track we like best
    else if (matching_oldtracks.size()==1){
      unsigned int e = matching_oldtracks[0];
      const FTK_RawTrack *oldtrack = m_trks_nodups->at(e);
      const FTK_RawTrack *besttrack = this->besttrack(track,oldtrack);
      if (besttrack==track){
#ifdef FTKDuplicateTrackRemovalUseMap
	removefromphimap(oldtrack->getPhi(),e);//remove the old track from the map
	addtophimap(trackphi,e);//add the new track to the map
#endif
	m_trks_nodups->at(e)=(FTK_RawTrack*)track;
      }
      else{
	//nothing to do - the better track was already in the output container
      }
    }
    
    else { // more than 1 matching existing track (yet the existing matching tracks did not match each other)
      ATH_MSG_WARNING("Found multiple tracks ("<<matching_oldtracks.size()<<") matching track "<<i);
      
      // is the new track better than all the matching old tracks?
      bool newisbest = true;//start with an optimistic attitude!
      for (unsigned int e : matching_oldtracks){
	const FTK_RawTrack *oldtrack = m_trks_nodups->at(e);
	const FTK_RawTrack *besttrack = this->besttrack(track,oldtrack);
	if (besttrack!=track){
	  newisbest=false; // guess we're not the best, give up!
	  break;
	}
      }
      
      // if the new track is better than all the matching old tracks, remove the old tracks and add this one, otherwise do nothing (the new track is dropped)
      if (newisbest){
			  //yikes, we're better than all the matching old tracks
	bool replacedfirsttrack = false;//I want to check that the algorithm really replaces the first old track with the new one, and just once
	for (unsigned int e : matching_oldtracks){
	  
#ifdef FTKDuplicateTrackRemovalUseMap
	  removefromphimap(m_trks_nodups->at(e)->getPhi(),e);
#endif
	  if (e==matching_oldtracks[0]) {//this should be a little faster than removing all the old matching tracks and then adding the new one
	    
#ifdef FTKDuplicateTrackRemovalUseMap
	    addtophimap(trackphi,e);
#endif
	    m_trks_nodups->at(e)=(FTK_RawTrack*)track; // replace the first matching track with this new track
	    if (replacedfirsttrack) ATH_MSG_WARNING("We already did replace the first matching track!");
	    replacedfirsttrack=true;//just check that we really did it!
	  }
	  else {
	    //remove the old matching tracks beyond the first  one
#ifdef FTKDuplicateTrackRemovalUseMap
	    trackstokill.insert(e);//we'll remove these from the tracks returned at the end of the day, so we don't screw up the map in the meantime
#else
	    m_trks_nodups->erase(m_trks_nodups->begin()+e); // yes this is really the way you remove an element from a vector, you have to pass in the iterator
#endif
	  }
	}
	if (!replacedfirsttrack)  ATH_MSG_WARNING("Why did I not replace the first track?!");

      } // new track is best one
      
    } // deciding what to do based on the number of matches
    
  } // loop over incoming tracks
  
#ifdef FTKDuplicateTrackRemovalUseMap
  //remove those tracks that we flagged for killing
  if (trackstokill.size()){
    ATH_MSG_WARNING("Killing an extra "<<trackstokill.size()<<" tracks from multiple matches");
    FTK_RawTrackContainer* trks_nodups_temp  = new FTK_RawTrackContainer(SG::VIEW_ELEMENTS);//we don't own the tracks, we're just going to hold them
    for (unsigned int e = 0; e!=m_trks_nodups->size(); e++) {
      if (std::find(trackstokill.begin(),trackstokill.end(),e)==trackstokill.end()) trks_nodups_temp->push_back((FTK_RawTrack*)m_trks_nodups->at(e));
    }
    m_trks_nodups->clear();
    delete m_trks_nodups;
    m_trks_nodups = trks_nodups_temp;
  }
#endif
  
#ifdef FTKDuplicateTrackRemovalTiming
  } // loop over doing the removal
  clock_t tEnd = clock();
  double elapsed_secs = double(tEnd - tStart) / CLOCKS_PER_SEC;
  ATH_MSG_INFO("Time taken: "<<elapsed_secs<<" ms");//it's "ms", not "s", since we did 1000 times
#endif
  
  return m_trks_nodups;
}
