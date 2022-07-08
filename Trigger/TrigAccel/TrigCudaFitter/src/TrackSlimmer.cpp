#include "RecTrack.h"
#include "TrackInfoStruct.h"
#include "TrackSlimmer.h"

TrackSlimmer::TrackSlimmer(void) {
	m_vpTIS.clear();
}

TrackSlimmer::~TrackSlimmer(void) {
	for(std::vector<TrackInfoStruct*>::iterator it = m_vpTIS.begin();it!=m_vpTIS.end();++it) {
		delete (*it);
	}
	m_vpTIS.clear();
}

void TrackSlimmer::processTracks(std::vector<const RecTrack*>& vTracks) {
	long geoOffset = 0;
	long hitOffset = 0;

	for(std::vector<const RecTrack*>::iterator it=vTracks.begin();it!=vTracks.end();++it) {
		TrackInfoStruct* pTIS = new TrackInfoStruct;
		pTIS->m_nHits = 0;
		for(int i=0;i<5;i++) pTIS->m_params[i]=(*it)->getParameters()[i];
		pTIS->m_nOffsetG=geoOffset;
		pTIS->m_nOffsetH=hitOffset;
		m_vpTIS.push_back(pTIS);
	}
}
