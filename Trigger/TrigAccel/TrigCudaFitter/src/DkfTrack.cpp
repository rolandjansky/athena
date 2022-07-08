#include "TrkTrackState.h"
#include "TrkBaseNode.h"
#include "TrkPlanarSurface.h"
#include "RecTrack.h"
#include "DkfTrack.h"

DkfTrack::DkfTrack(TrkTrackState* pTS, const RecTrack* pR) : m_pTrackState(pTS), m_pRecTrack(pR)
  , m_dChi2(0)
  , m_nNDOF(-5)
{
	m_vpNodes.clear();m_vpSurfaces.clear();
	m_vpTrackStates.clear();
}

DkfTrack::~DkfTrack(void)
{
	delete m_pTrackState;
	for(std::vector<TrkBaseNode*>::iterator it=m_vpNodes.begin();it!=m_vpNodes.end();++it)
		delete (*it);
	m_vpNodes.clear();
	for(std::vector<const TrkPlanarSurface*>::iterator sIt=m_vpSurfaces.begin();sIt!=m_vpSurfaces.end();++sIt)
		delete (*sIt);
	m_vpSurfaces.clear();
	for(std::vector<TrkTrackState*>::iterator tsIt = m_vpTrackStates.begin();tsIt!=m_vpTrackStates.end();++tsIt)
		delete (*tsIt);
	m_vpTrackStates.clear();
}

double DkfTrack::getChi2(void)
{
	return m_dChi2;
}

void DkfTrack::addChi2(double d, int n)
{
	m_dChi2+=d;m_nNDOF+=n;
}

int DkfTrack::getNDOF(void)
{
	return m_nNDOF;
}
