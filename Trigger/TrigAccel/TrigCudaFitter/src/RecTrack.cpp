//#include "stdafx.h"
#include "BFieldPoint.h"
#include "SpacePoint.h"
#include "DkfTrack.h"

#include "RecTrack.h"
#include "TrkTrackState.h"
#include "TrkBaseNode.h"
#include "TrkPlanarSurface.h"

#include <algorithm>


RecTrack::RecTrack(int id)
  : m_algoId(id)
{
	for(int i=0;i<5;i++) m_params[i]=0.0;
	m_fieldMap.clear();m_spacepoints.clear();
}

RecTrack::~RecTrack(void)
{
	for(std::vector<const BFieldPoint*>::iterator bIt=m_fieldMap.begin();bIt!=m_fieldMap.end();++bIt)
		delete (*bIt);
	m_fieldMap.clear();
	for(std::vector<const SpacePoint*>::iterator it=m_spacepoints.begin();
		it!=m_spacepoints.end();++it)
		delete (*it);
	m_spacepoints.clear();
}

void RecTrack::setInitialParameters(float* par)
{
	for(int i=0;i<5;i++) m_params[i]=par[i];
	if(m_algoId==1) m_params[4]=-m_params[4]*0.001f;
}

void RecTrack::addBFieldPoint(double x, double y, double z, float* B)
{
	BFieldPoint* pP = new BFieldPoint(x,y,z,B);
	m_fieldMap.push_back(pP);
}

void RecTrack::addSpacePoint(const SpacePoint* psp)
{
	m_spacepoints.push_back(psp);
}



const std::vector<const SpacePoint*>& RecTrack::getSpacePoints(void) const
{
	//TODO: insert return statement here
	return m_spacepoints;
}

const double* RecTrack::getParameters(void) const
{
	return &m_params[0];
}

int RecTrack::getAlgoId(void) const
{
	return m_algoId;
}

const BFieldPoint* RecTrack::getField(int i) const
{
	//TODO: insert return statement here
	return m_fieldMap[i];
}

bool SpSortPredicat(const SpacePoint* pSP1, const SpacePoint* pSP2)
{
	return (pSP1->m_r<pSP2->m_r);
}

void RecTrack::sortSpacePoints(void)
{
	std::sort(m_spacepoints.begin(),m_spacepoints.end(),SpSortPredicat);
}
