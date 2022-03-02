#include "eflowRec/PFTrackFiller.h"

#include "eflowRec/eflowCaloObject.h"

void PFTrackFiller::fillTracksToRecover(PFData &data) const
{
  
  for (auto thisEflowCaloObject : *data.caloObjects){

    unsigned int nTrk = thisEflowCaloObject->nTracks();

    if (0 == thisEflowCaloObject->nClusters()){
      //If there are no matched clusters we mark each track in thisEflowCaloObject has one that has been processed ("subtracted")
      //and continue to the next eflowCaloObject
      for (unsigned int iTrk = 0; iTrk < nTrk; ++iTrk) {
        eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrk);
        if (!thisEfRecTrack->isSubtracted()) thisEfRecTrack->setSubtracted();
      }//loop on tracks on thisEflowCaloObject
      continue;
    }//if thisEflowCaloObject has zero CaloCluster     

    //ignore cluster only systems and go to the next eflowCaloObject
    if(nTrk == 0) continue;

    //Else process systems with only tracks or combinations of tracks and clusters

    //Create a vector to keep a list of tracks we don't want to consider
    std::vector<eflowRecTrack*> updatedTracks; 
      
    for (unsigned int iTrack = 0; iTrack < nTrk; ++iTrack){
      eflowRecTrack* thisEfRecTrack = thisEflowCaloObject->efRecTrack(iTrack);
      //If this track was already processed ("subtracted") we continue on to the next track
      if (thisEfRecTrack->isSubtracted()){
        updatedTracks.push_back(thisEfRecTrack);
        continue;
      }
      //remove existing matched CaloCluster and add this track to our list that we want to consider
      //for charged hadron subtraction
      thisEfRecTrack->clearClusterMatches();
      data.tracks.push_back(thisEfRecTrack);
    }//loop on tracks in thisEflowCaloObject

    //Now remove all tracks from thisEflowCaloObject
    thisEflowCaloObject->clearTracks();
    //Add back the tracks we won't consider for charged hadron subtraction (if any)
    if (!updatedTracks.empty()) thisEflowCaloObject->addTracks(updatedTracks);
    //SHOULD WE ALWAYS DO THIS? TO BE TESTED
    else thisEflowCaloObject->clearLinks();
  }

  std::sort(data.tracks.begin(),data.tracks.end(),eflowRecTrack::SortDescendingPt());

}

void PFTrackFiller::fillTracksToConsider(PFData &data, eflowRecTrackContainer &recTrackContainer) const
{
  for (unsigned int count = 0; count < recTrackContainer.size(); count++) data.tracks.push_back(recTrackContainer[count]);
}