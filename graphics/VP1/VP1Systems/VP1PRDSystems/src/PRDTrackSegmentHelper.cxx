/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class PRDTrackSegmentHelper             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: December 2007                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1PRDSystems/PRDTrackSegmentHelper.h"
#include "VP1PRDSystems/PRDCollHandleBase.h"
#include "VP1PRDSystems/PRDHandleBase.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "DataModel/DataVector.h"
#include "TrkTrack/Track.h"
#include "TrkSegment/Segment.h"

#include <set>

//____________________________________________________________________
class PRDTrackSegmentHelper::Imp {
public:
  PRDTrackSegmentHelper * theclass;
  std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> >* prd2handles;

  std::map< const Trk::Track*, const SoMaterial* > tracks2mat;
  std::map< const Trk::Segment*, const SoMaterial* > segments2mat;


  void addTrackToPRDs(const Trk::Track *, bool outliers );
  void removeTrackFromPRDs(const Trk::Track *, bool outliers);
  void updateMaterialOfPRDs(const Trk::Track *, bool outliers );

  void addSegmentToPRDs(const Trk::Segment * );
  void removeSegmentFromPRDs(const Trk::Segment *);
  void updateMaterialOfPRDs(const Trk::Segment * );

  template <class T>
  void removeEntryFromVector(std::vector<T*>&v,T*t);//Removes first occurrence of t from v (does nothing if not found).
                                                    //Element order is not preserved. FIXME: To utility class.

  //static inline const Trk::PrepRawData* measurementToPRD(const Trk::MeasurementBase*);//Might return null.
  static inline std::vector<const Trk::PrepRawData*> measurementToPRDs(const Trk::MeasurementBase*);//Might return empty vector.

  //We keep this map around in order to give track associations for
  //prds (in particular those whose handles are created after the
  //change in visible tracks):
  //
  //NB (TK): I guess a track association tool could do this too - but
  //why add that dependency for something which is dead simple?
  std::map< const Trk::PrepRawData*,TracksAndSegments > prdsOnTracksAndSegments;

};

//____________________________________________________________________
template <class T>
void PRDTrackSegmentHelper::Imp::removeEntryFromVector(std::vector<T*>&v,T*t)
{
  const unsigned n(v.size());
  for (unsigned i(0); i<n;++i) {
    if (v[i]==t) {
      if (i==n-1) {
	v.resize(n-1);
	return;
      } else {
	v[i]=v[n-1];
	v.resize(n-1);
	return;
      }
    }
  }
}

//____________________________________________________________________
PRDTrackSegmentHelper::PRDTrackSegmentHelper(std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> >* prd2handles,
					     IVP1System* sys, QObject * parent)
  : QObject(parent), VP1HelperClassBase(sys,"PRDTrackSegmentHelper"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->prd2handles = prd2handles;
}

//____________________________________________________________________
PRDTrackSegmentHelper::~PRDTrackSegmentHelper()
{
  delete m_d;
}

//____________________________________________________________________
void PRDTrackSegmentHelper::visibleTracksChanged(const std::vector< std::pair<const Trk::Track*, const SoMaterial*> >& tracks)
{
  //NB: Code here is very similar to code in visibleSegmentsChanged!!
  messageVerbose("visibleTracksChanged start (old ntracks = "+QString::number(m_d->tracks2mat.size())
		 +", new ntracks = "+QString::number(tracks.size())+")");

  if (tracks.empty()) {
    //Special case #1 - loop over previous track's, remove their pointers from the relevant prd handles + update their materials.
    messageVerbose("special case #1 - new track list is empty");
    std::map< const Trk::Track*, const SoMaterial* >::iterator it, itE = m_d->tracks2mat.end();
    for (it=m_d->tracks2mat.begin();it!=itE;++it) {
      m_d->removeTrackFromPRDs(it->first,false);//measurements
      m_d->removeTrackFromPRDs(it->first,true);//outliers
    }
    m_d->tracks2mat.clear();
    messageVerbose("visibleTracksChanged end");
    return;
  }

  if (m_d->tracks2mat.empty()) {
    //special case #2 - no previous tracks, so just loop over incoming
    //tracks, and add their pointers to the relevant prd handles + update their materials.
    messageVerbose("special case #2 - old track list is empty");
    std::vector< std::pair<const Trk::Track*, const SoMaterial*> >::const_iterator it(tracks.begin()), itE(tracks.end());
    for (;it!=itE;++it) {
      m_d->tracks2mat.insert(*it);
      m_d->addTrackToPRDs(it->first, false );//measurements
      m_d->addTrackToPRDs(it->first, true );//outliers
    }
    messageVerbose("visibleTracksChanged end");
    return;
  }

  //Normal case - need to remove some trackpointers, add some, and for others possibly just update their materials.
  messageVerbose("normal case - neither old, nor new, track lists are empty");

  //First check if any previously visible tracks simply disappeared.
  // --> Create std::set for faster searches.
  std::set<const Trk::Track*> newtracksset;
    std::vector< std::pair<const Trk::Track*, const SoMaterial*> >::const_iterator it(tracks.begin()), itE(tracks.end());
    for (;it!=itE;++it)
      newtracksset.insert(it->first);
  std::set<const Trk::Track*>::const_iterator newtrackssetEnd(newtracksset.end());

  // --> Check old tracks versus this set - remove if no match:
  std::map< const Trk::Track*, const SoMaterial* >::iterator it2, it2E = m_d->tracks2mat.end();
  for (it2=m_d->tracks2mat.begin();it2!=it2E;) {
    if (newtracksset.find(it2->first)==newtrackssetEnd) {
      m_d->removeTrackFromPRDs(it2->first,false);//measurements
      m_d->removeTrackFromPRDs(it2->first,true);//outliers
      m_d->tracks2mat.erase(it2++);//postfix ++ operator must be used as here (due to the erase call)
    } else {
      it2++;
    }
  }

  // Next, check all tracks that are now visible - if not previously
  // visible we add their pointers to the relevant handles. If just
  // the material changed, we make sure the prd handle updates its
  // material.
  it2E=m_d->tracks2mat.end();
  for (it=tracks.begin();it!=itE;++it) {
    it2 = m_d->tracks2mat.find(it->first);
    if (it2==it2E) {
      m_d->tracks2mat[it->first] = it->second;
      m_d->addTrackToPRDs(it->first, false );//measurements
      m_d->addTrackToPRDs(it->first, true );//outliers
    } else {
      //we need to update the track material - but only if it changed of course.
      if (it->second!=it2->second) {
	m_d->tracks2mat[it->first] = it->second;
	m_d->updateMaterialOfPRDs(it->first, false );//measurements
	m_d->updateMaterialOfPRDs(it->first, true );//outliers
      }
    }
  }
  messageVerbose("visibleTracksChanged end");

}

//____________________________________________________________________
void PRDTrackSegmentHelper::visibleSegmentsChanged(const std::vector< std::pair<const Trk::Segment*, const SoMaterial*> >& segments)
{
  //NB: Code here is very similar to code in visibleTracksChanged!!
  messageVerbose("visibleSegmentsChanged start (old nsegments = "+QString::number(m_d->segments2mat.size())
		 +", new nsegments = "+QString::number(segments.size())+")");

  if (segments.empty()) {
    //Special case #1 - loop over previous segment's, remove their pointers from the relevant prd handles + update their materials.
    messageVerbose("special case #1 - new segment list is empty");
    std::map< const Trk::Segment*, const SoMaterial* >::iterator it, itE = m_d->segments2mat.end();
    for (it=m_d->segments2mat.begin();it!=itE;++it) {
      m_d->removeSegmentFromPRDs(it->first);
    }
    m_d->segments2mat.clear();
    messageVerbose("visibleSegmentsChanged end");
    return;
  }

  if (m_d->segments2mat.empty()) {
    //special case #2 - no previous segments, so just loop over incoming
    //segments, and add their pointers to the relevant prd handles + update their materials.
    messageVerbose("special case #2 - old segment list is empty");
    std::vector< std::pair<const Trk::Segment*, const SoMaterial*> >::const_iterator it(segments.begin()), itE(segments.end());
    for (;it!=itE;++it) {
      m_d->segments2mat.insert(*it);
      m_d->addSegmentToPRDs(it->first );
    }
    messageVerbose("visibleSegmentsChanged end");
    return;
  }

  //Normal case - need to remove some segmentpointers, add some, and for others possibly just update their materials.
  messageVerbose("normal case - neither old, nor new, segment lists are empty");

  //First check if any previously visible segments simply disappeared.
  // --> Create std::set for faster searches.
  std::set<const Trk::Segment*> newsegmentsset;
    std::vector< std::pair<const Trk::Segment*, const SoMaterial*> >::const_iterator it(segments.begin()), itE(segments.end());
    for (;it!=itE;++it)
      newsegmentsset.insert(it->first);
  std::set<const Trk::Segment*>::const_iterator newsegmentssetEnd(newsegmentsset.end());

  // --> Check old segments versus this set - remove if no match:
  std::map< const Trk::Segment*, const SoMaterial* >::iterator it2, it2E = m_d->segments2mat.end();
  for (it2=m_d->segments2mat.begin();it2!=it2E;) {
    if (newsegmentsset.find(it2->first)==newsegmentssetEnd) {
      m_d->removeSegmentFromPRDs(it2->first);
      m_d->segments2mat.erase(it2++);//postfix ++ operator must be used as here (due to the erase call)
    } else {
      it2++;
    }
  }

  // Next, check all segments that are now visible - if not previously
  // visible we add their pointers to the relevant handles. If just
  // the material changed, we make sure the prd handle updates its
  // material.
  it2E=m_d->segments2mat.end();
  for (it=segments.begin();it!=itE;++it) {
    it2 = m_d->segments2mat.find(it->first);
    if (it2==it2E) {
      m_d->segments2mat[it->first] = it->second;
      m_d->addSegmentToPRDs(it->first);
    } else {
      //we need to update the segment material - but only if it changed of course.
      if (it->second!=it2->second) {
	m_d->segments2mat[it->first] = it->second;
	m_d->updateMaterialOfPRDs(it->first);
      }
    }
  }
  messageVerbose("visibleSegmentsChanged end");
}

//____________________________________________________________________
void PRDTrackSegmentHelper::Imp::addSegmentToPRDs(const Trk::Segment * seg)
{
  // if (verbose())
  //   theclass->messageVerbose("addSegmentToPRDs start");
  if (!seg) {
    theclass->message("ERROR: Received null segment pointer!");
    return;
  }
  std::map< const Trk::PrepRawData*,TracksAndSegments >::iterator itInfo;
  std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> >::const_iterator itHandle, itHandleEnd(prd2handles->end());
  std::vector<const Trk::MeasurementBase*>::const_iterator
    it (seg->containedMeasurements().begin()), itE(seg->containedMeasurements().end());
  const Trk::PrepRawData * prd;
  for (;it!=itE;++it) {
    std::vector<const Trk::PrepRawData*> prds = Imp::measurementToPRDs(*it); // need to handle compound measurements too.
    std::vector<const Trk::PrepRawData*>::const_iterator itPrd = prds.begin(), itEnd=prds.end();
    for (;itPrd!=itEnd;itPrd++){
      prd = *itPrd;
      itInfo = prdsOnTracksAndSegments.find(prd);
      if (itInfo==prdsOnTracksAndSegments.end()) {
        prdsOnTracksAndSegments.insert(std::pair<const Trk::PrepRawData*,TracksAndSegments>(prd,
          TracksAndSegments(std::vector<const Trk::Track*>(),
          std::vector<const Trk::Track*>(),
          std::vector< const Trk::Segment* >(1,seg))));

      } else {
        itInfo->second.segments.push_back(seg);
      }
      itHandle = prd2handles->find(prd);
      if (itHandle!=itHandleEnd) {
        QList<PRDHandleBase*> handles = itHandle->second;
        foreach (PRDHandleBase*handle,handles)
          if (handle->collHandle()->colourBySegments())
          handle->updateMaterial();
      }

    }
  }
  // theclass->messageVerbose("addSegmentToPRDs end");
}

//____________________________________________________________________
void PRDTrackSegmentHelper::Imp::removeSegmentFromPRDs(const Trk::Segment * seg)
{
  // if (verbose())
  //   theclass->messageVerbose("removeSegmentFromPRDs start");
  if (!seg) {
    theclass->message("ERROR: Received null segment pointer!");
    return;
  }
  std::map< const Trk::PrepRawData*,TracksAndSegments >::iterator itInfo;
  std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> >::const_iterator itHandle, itHandleEnd(prd2handles->end());
  std::vector<const Trk::MeasurementBase*>::const_iterator
    it (seg->containedMeasurements().begin()), itE(seg->containedMeasurements().end());
  const Trk::PrepRawData * prd;
  for (;it!=itE;++it) {
    std::vector<const Trk::PrepRawData*> prds = Imp::measurementToPRDs(*it); // need to handle compound measurements too.
    std::vector<const Trk::PrepRawData*>::const_iterator itPrd = prds.begin(), itEnd=prds.end();
    for (;itPrd!=itEnd;itPrd++){
      prd = *itPrd;
      itInfo = prdsOnTracksAndSegments.find(prd);
      if (itInfo!=prdsOnTracksAndSegments.end()) {
        removeEntryFromVector(itInfo->second.segments,seg);
        itHandle = prd2handles->find(prd);
        if (itHandle!=itHandleEnd) {
          QList<PRDHandleBase*> handles = itHandle->second;
          foreach (PRDHandleBase*handle,handles)
            if (handle->collHandle()->colourBySegments())
            handle->updateMaterial();
        }
      }
    }
  }

  // if (verbose())
  //   theclass->messageVerbose("removeSegmentToPRDs end");
}

//____________________________________________________________________
void PRDTrackSegmentHelper::Imp::updateMaterialOfPRDs(const Trk::Segment * seg)
{
  // if (verbose())
  //   theclass->messageVerbose("updateMaterialOfPRDs(segment) start");
  if (!seg) {
    theclass->message("ERROR: Received null segment pointer!");
    return;
  }
  std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> >::const_iterator itHandle, itHandleEnd(prd2handles->end());
  std::vector<const Trk::MeasurementBase*>::const_iterator
    it (seg->containedMeasurements().begin()), itE(seg->containedMeasurements().end());
  const Trk::PrepRawData * prd;
  for (;it!=itE;++it) {
    std::vector<const Trk::PrepRawData*> prds = Imp::measurementToPRDs(*it); // need to handle compound measurements too.
    std::vector<const Trk::PrepRawData*>::const_iterator itPrd = prds.begin(), itEnd=prds.end();
    for (;itPrd!=itEnd;itPrd++){
      prd = *itPrd;
      itHandle = prd2handles->find(prd);
      if (itHandle!=itHandleEnd) {
        QList<PRDHandleBase*> handles = itHandle->second;
        foreach (PRDHandleBase*handle,handles)
          if (handle->collHandle()->colourByTracks())
          handle->updateMaterial();
      }
    }
  }

  // if (verbose())
  //   theclass->messageVerbose("updateMaterialOfPRDs(segment) end");
}

//____________________________________________________________________
void PRDTrackSegmentHelper::Imp::addTrackToPRDs(const Trk::Track * trk, bool outliers )
{
  // if (verbose())
  //   theclass->messageVerbose("addTrackToPRDs start");
  if (!trk) {
    theclass->message("ERROR: Received null track pointer!");
    return;
  }
  std::map< const Trk::PrepRawData*,TracksAndSegments >::iterator itInfo;
  std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> >::const_iterator itHandle, itHandleEnd(prd2handles->end());
  DataVector<const Trk::MeasurementBase>::const_iterator
    it ( outliers ? trk->outliersOnTrack()->begin() : trk->measurementsOnTrack()->begin() ),
    itE( outliers ? trk->outliersOnTrack()->end() : trk->measurementsOnTrack()->end() );
  const Trk::PrepRawData * prd;
  for (;it!=itE;++it) {
    std::vector<const Trk::PrepRawData*> prds = Imp::measurementToPRDs(*it); // need to handle compound measurements too.
    std::vector<const Trk::PrepRawData*>::const_iterator itPrd = prds.begin(), itEnd=prds.end();
    for (;itPrd!=itEnd;itPrd++){
      prd = *itPrd;
      if (prd) {
        itInfo = prdsOnTracksAndSegments.find(prd);
        if (itInfo==prdsOnTracksAndSegments.end()) {
          prdsOnTracksAndSegments.insert(std::pair<const Trk::PrepRawData*,TracksAndSegments>(prd,
            TracksAndSegments((outliers?std::vector<const Trk::Track*>():std::vector<const Trk::Track*>(1,trk)),
            (outliers?std::vector<const Trk::Track*>(1,trk):std::vector<const Trk::Track*>()),
            std::vector< const Trk::Segment* >())));
        } else {
          if (outliers)
            itInfo->second.tracks_outliers.push_back(trk);
          else
            itInfo->second.tracks.push_back(trk);
        }
        itHandle = prd2handles->find(prd);
        if (itHandle!=itHandleEnd) {
          QList<PRDHandleBase*> handles = itHandle->second;
          foreach (PRDHandleBase*handle,handles)
            if (handle->collHandle()->colourByTracks())
            handle->updateMaterial();
        }
      }
    }
  }
  // if (verbose())
  //   theclass->messageVerbose("addTrackToPRDs end");
}

//____________________________________________________________________
void PRDTrackSegmentHelper::Imp::removeTrackFromPRDs(const Trk::Track * trk, bool outliers)
{
  // if (verbose())
  //   theclass->messageVerbose("removeTrackFromPRDs start");
  if (!trk) {
    theclass->message("ERROR: Received null track pointer!");
    return;
  }
  std::map< const Trk::PrepRawData*,TracksAndSegments >::iterator itInfo;
  std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> >::const_iterator itHandle, itHandleEnd(prd2handles->end());
  DataVector<const Trk::MeasurementBase>::const_iterator
    it ( outliers ? trk->outliersOnTrack()->begin() : trk->measurementsOnTrack()->begin() ),
    itE( outliers ? trk->outliersOnTrack()->end() : trk->measurementsOnTrack()->end() );
  const Trk::PrepRawData * prd;
  for (;it!=itE;++it) {
    std::vector<const Trk::PrepRawData*> prds = Imp::measurementToPRDs(*it); // need to handle compound measurements too.
    std::vector<const Trk::PrepRawData*>::const_iterator itPrd = prds.begin(), itEnd=prds.end();
    for (;itPrd!=itEnd;itPrd++){
      prd = *itPrd;
      itInfo = prdsOnTracksAndSegments.find(prd);
      if (itInfo!=prdsOnTracksAndSegments.end()) {
        removeEntryFromVector((outliers?itInfo->second.tracks_outliers:itInfo->second.tracks),trk);
        itHandle = prd2handles->find(prd);
        if (itHandle!=itHandleEnd) {
          QList<PRDHandleBase*> handles = itHandle->second;
          foreach (PRDHandleBase*handle,handles)
            if (handle->collHandle()->colourByTracks())
            handle->updateMaterial();
        }
      }
    }
  }

  // if (verbose())
  //   theclass->messageVerbose("removeTrackToPRDs end");
}

//____________________________________________________________________
void PRDTrackSegmentHelper::Imp::updateMaterialOfPRDs(const Trk::Track * trk, bool outliers )
{
  // if (verbose())
  //   theclass->messageVerbose("updateMaterialOfPRDs(track) start");
  if (!trk) {
    theclass->message("ERROR: Received null track pointer!");
    return;
  }
  std::map< const Trk::PrepRawData *, QList<PRDHandleBase *> >::const_iterator itHandle, itHandleEnd(prd2handles->end());
  DataVector<const Trk::MeasurementBase>::const_iterator
    it ( outliers ? trk->outliersOnTrack()->begin() : trk->measurementsOnTrack()->begin() ),
    itE( outliers ? trk->outliersOnTrack()->end() : trk->measurementsOnTrack()->end() );
  const Trk::PrepRawData * prd;
  for (;it!=itE;++it) {
    std::vector<const Trk::PrepRawData*> prds = Imp::measurementToPRDs(*it); // need to handle compound measurements too.
    std::vector<const Trk::PrepRawData*>::const_iterator itPrd = prds.begin(), itEnd=prds.end();
    for (;itPrd!=itEnd;itPrd++){
      prd = *itPrd;
      itHandle = prd2handles->find(prd);
      if (itHandle!=itHandleEnd) {
        QList<PRDHandleBase*> handles = itHandle->second;
        foreach (PRDHandleBase*handle,handles)
          if (handle->collHandle()->colourByTracks())
          handle->updateMaterial();
      }
    }
  }

  // if (verbose())
  //   theclass->messageVerbose("updateMaterialOfPRDs(track) end");
}

//____________________________________________________________________
const PRDTrackSegmentHelper::TracksAndSegments * PRDTrackSegmentHelper::tracksAndSegments(const Trk::PrepRawData* prd)
{
  std::map< const Trk::PrepRawData*,TracksAndSegments >::iterator itInfo = m_d->prdsOnTracksAndSegments.find(prd);
  return itInfo == m_d->prdsOnTracksAndSegments.end() ? 0 : &(itInfo->second);
}


//____________________________________________________________________
inline  std::vector<const Trk::PrepRawData *> PRDTrackSegmentHelper::Imp::measurementToPRDs(const Trk::MeasurementBase* measbase)
{
  std::vector<const Trk::PrepRawData *>prds;
  const Trk::RIO_OnTrack * rio = dynamic_cast<const Trk::RIO_OnTrack *>(measbase);
  if (rio) prds.push_back( rio->prepRawData() );
  const Trk::CompetingRIOsOnTrack * crot = dynamic_cast<const Trk::CompetingRIOsOnTrack *>(measbase);
  if (crot) {

	//    for (unsigned int i=0; i< crot->numberOfContainedROTs (); ++i) prds.push_back( crot->rioOnTrack(i) ? crot->rioOnTrack(i)->prepRawData() : 0 );
    for (unsigned int i=0; i< crot->numberOfContainedROTs (); ++i) {
    	if (crot->rioOnTrack(i).identify().getString() != "") { // FIXME: search for a better way to see if the rioOnTrack is present
    		prds.push_back(crot->rioOnTrack(i).prepRawData());
    	}
    	else {
    		prds.push_back(0);
    	}
    }
  }
  return prds;
}

//____________________________________________________________________
inline PRDTrackSegmentHelper::TracksAndSegments::TracksAndSegments(const std::vector<const Trk::Track*>& t,
								   const std::vector<const Trk::Track*>& to,
								   const std::vector< const Trk::Segment* >& s )
  : tracks(t), tracks_outliers(to), segments(s)
{
}

//____________________________________________________________________
SoMaterial * PRDTrackSegmentHelper::trackMaterial(const Trk::Track* t) const
{
  std::map< const Trk::Track*, const SoMaterial* >::const_iterator it = m_d->tracks2mat.find(t);
  return it == m_d->tracks2mat.end() ? 0 : const_cast<SoMaterial *>(it->second);//fixme; const_cast is temporary hack. Remove const from materials!
}

//____________________________________________________________________
SoMaterial * PRDTrackSegmentHelper::segmentMaterial(const Trk::Segment* s) const
{
  std::map< const Trk::Segment*, const SoMaterial* >::const_iterator it = m_d->segments2mat.find(s);
  return it == m_d->segments2mat.end() ? 0 : const_cast<SoMaterial *>(it->second);//fixme; const_cast is temporary hack. Remove const from materials!
}
