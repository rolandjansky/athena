/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// AlgTool used for trigger chamber MuonClusterOnTrack object creation
//
// (c) ATLAS muon software
///////////////////////////////////////////////////////////////////////

#include "TriggerChamberClusterOnTrackCreator.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"

namespace Muon
{

TriggerChamberClusterOnTrackCreator::TriggerChamberClusterOnTrackCreator (
	const std::string&	type,
	const std::string&	name, 
	const IInterface*	parent)
  :	AthAlgTool			(type, name, parent),
	m_clusterCreator		("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
	m_chooseBroadestCluster		(true)
{
    declareInterface<Muon::IMuonCompetingClustersOnTrackCreator>(this);
    declareProperty("ClusterCreator",  		m_clusterCreator);
    declareProperty("ChooseBroadestCluster", 	m_chooseBroadestCluster);

}

TriggerChamberClusterOnTrackCreator::~TriggerChamberClusterOnTrackCreator()
{}

StatusCode
TriggerChamberClusterOnTrackCreator::initialize()
{

    const MuonGM::MuonDetectorManager*  muonManager;
    ATH_CHECK( detStore()->retrieve(muonManager) );
    m_rpcIdHelper = muonManager->rpcIdHelper();
    m_tgcIdHelper = muonManager->tgcIdHelper();

    ATH_CHECK( m_clusterCreator.retrieve() );
    return StatusCode::SUCCESS;
}

StatusCode
TriggerChamberClusterOnTrackCreator::finalize()
{
    return StatusCode::SUCCESS;
}
  
const CompetingMuonClustersOnTrack*
TriggerChamberClusterOnTrackCreator::createBroadCluster(const std::list<const Trk::PrepRawData*>& prds,
							const double) const
{
    ATH_MSG_VERBOSE("enter createBroadCluster: number of prds " << prds.size() );

    // make some PRD consistency checks
    if (! prds.size())
    {
	ATH_MSG_WARNING("fails: empty PRD list ");
	return 0;
    }

    const Trk::TrkDetElementBase* detectorElement = (**prds.begin()).detectorElement();
    Identifier channelId	= (**prds.begin()).identify();
    const MuonIdHelper*	helper	= m_rpcIdHelper;
    m_isRpc			= helper->is_rpc(channelId);
    if (! m_isRpc)
    {
	if (m_tgcIdHelper->is_tgc(channelId))
	{
	    helper = m_tgcIdHelper;
	}
	else
	{
	    ATH_MSG_WARNING("fails: PRD must be from rpc or tgc ");
	    return 0;
	}
    }

    bool measuresPhi = m_isRpc && m_rpcIdHelper->measuresPhi(channelId);
    if (! m_isRpc) measuresPhi = m_tgcIdHelper->isStrip(channelId);
    for (std::list<const Trk::PrepRawData*>::const_iterator p = prds.begin();
	 p != prds.end();
	 ++p)
    {
	channelId = (**p).identify();
	if ((m_isRpc		&& m_rpcIdHelper->measuresPhi(channelId)	!= measuresPhi)
	    || (! m_isRpc	&& m_tgcIdHelper->isStrip(channelId)		!= measuresPhi))
	{
	    ATH_MSG_WARNING("fails: PRDs must measure same coordinate ");
	    return 0;
	}
	if ((**p).detectorElement() != detectorElement)
	{
		ATH_MSG_WARNING("fails: PRDs must be from same detectorElement ");
		return 0;
	}
    }

    // create a rot for each prd (which gets weight zero)
    std::vector<const Muon::MuonClusterOnTrack*>* rots = createPrdRots(prds);
    std::vector<double>* assProbs = new std::vector<double>;
    for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator m = rots->begin();
	 m != rots->end();
	 ++m)
	assProbs->push_back(0.);

    // for each surface, find the first and last rot forming the cluster
    std::list<int>				limitingChannels;
    std::list<const Muon::MuonClusterOnTrack*>	limitingRots;
    makeClustersBySurface(limitingChannels,limitingRots,prds,*rots);

    // cluster consistency - discard any surfaces not contributing to the final cluster
    applyClusterConsistency(limitingChannels,limitingRots);

    // overall localPosition, error matrix and surface
    Trk::LocalParameters* parameters	= 0;
    Amg::MatrixX* errorMatrix 	= 0;
    Trk::Surface* surface		= 0;
    makeOverallParameters(parameters,errorMatrix,surface,limitingChannels,limitingRots);
    
    // clear lists
    limitingChannels.clear();
    
    // return the competingMuonClusterOnTrack object containing the final parameters,
    // error matrix, surface, list of rots and weights
    return new CompetingMuonClustersOnTrack(parameters,errorMatrix,surface,rots,assProbs);
}

void
TriggerChamberClusterOnTrackCreator::applyClusterConsistency(
	std::list<int>&						limitingChannels,
	std::list<const Muon::MuonClusterOnTrack*>&		limitingRots) const
{
    // remove any clusters that will NOT contribute to the final cluster
    int numClusters	= limitingChannels.size()/2;
    int sizeMax		= 0;
    int sizeMin 	= 999;
    for (std::list<int>::iterator l = limitingChannels.begin();
	 l != limitingChannels.end();
	 ++l)
    {
	int size = abs(*l - *(++l));
	if (size > sizeMax) sizeMax = size;
	if (size < sizeMin) sizeMin = size;
    }
    
    std::list<int>::iterator discard	= limitingChannels.end();
    for (std::list<int>::iterator l = limitingChannels.begin();
	 l != limitingChannels.end();
	 ++l)
    {
	std::list<int>::iterator first = l;
	int size = abs(*l - *(++l));
	if (m_chooseBroadestCluster	&& size < sizeMax) discard = first;
	if (! m_chooseBroadestCluster	&& size > sizeMin) discard = first;
    }
    if (discard == limitingChannels.begin())
    {
	ATH_MSG_VERBOSE(" discard cluster #" << 1 );
	limitingRots.pop_front();
	limitingRots.pop_front();
	limitingChannels.pop_front();
	limitingChannels.pop_front();
    }
    else if (discard != limitingChannels.end())
    {
	ATH_MSG_VERBOSE(" discard cluster #" << numClusters );
	limitingRots.pop_back();
	limitingRots.pop_back();
	limitingChannels.pop_back();
	limitingChannels.pop_back();
    }
}

std::vector<const Muon::MuonClusterOnTrack*>*
TriggerChamberClusterOnTrackCreator::createPrdRots(
    const std::list<const Trk::PrepRawData*>& prds) const
{
    // create clusterRot for each PRD
    std::vector<const Muon::MuonClusterOnTrack*>* rots =
	new std::vector<const Muon::MuonClusterOnTrack*>;
    for (std::list<const Trk::PrepRawData*>::const_iterator  p = prds.begin();
	 p != prds.end();
	 ++p)
    {
	Identifier id = (**p).identify();
	const Trk::TrkDetElementBase* detectorElement	= (**p).detectorElement();
	const Amg::Vector3D globalPosition	= detectorElement->center(id);
	const Muon::MuonClusterOnTrack* cluster		=
	    m_clusterCreator->createRIO_OnTrack(**p,globalPosition); 
	rots->push_back(cluster);
    }

    return rots;
}

void
TriggerChamberClusterOnTrackCreator::makeClustersBySurface(
    std::list<int>&					limitingChannels,
    std::list<const Muon::MuonClusterOnTrack*>&		limitingRots,
    const std::list<const Trk::PrepRawData*>&		prds,
    const std::vector<const Muon::MuonClusterOnTrack*>& rots) const
{
    std::vector<const Trk::PrepRawData*> usedPrd;
    std::vector<const Muon::MuonClusterOnTrack*>::const_iterator r = rots.begin();
    for (std::list<const Trk::PrepRawData*>::const_iterator p = prds.begin();
	 p != prds.end();
	 ++p, ++r)
    {
	if (std::find(usedPrd.begin(),usedPrd.end(),*p) != usedPrd.end()) continue;
	usedPrd.push_back(*p);
	int channel	= 0;
	int gasGap	= 0;
	Identifier channelId = (**p).identify();
	if (m_isRpc)
	{
	    gasGap	= m_rpcIdHelper->gasGap(channelId);
	    channel	= m_rpcIdHelper->strip(channelId);
	}
	else
	{
	    gasGap	= m_tgcIdHelper->gasGap(channelId);
	    channel	= m_tgcIdHelper->channel(channelId);
	}
	int channelMax	= channel;
	int channelMin	= channel;
	const Muon::MuonClusterOnTrack* rotMax = *r;
	const Muon::MuonClusterOnTrack* rotMin = *r;
	
	std::list<const Trk::PrepRawData*>::const_iterator q = p;
	std::vector<const Muon::MuonClusterOnTrack*>::const_iterator s = r;
	for (++q, ++s;
	     q != prds.end();
	     ++q, ++s)
	{
	    channelId = (**q).identify();
	    if ((     m_isRpc && m_rpcIdHelper->gasGap(channelId)	!= gasGap)
		|| (! m_isRpc && m_tgcIdHelper->gasGap(channelId)	!= gasGap)) continue;
	    usedPrd.push_back(*q);
	    if (m_isRpc)
	    {
		channel	= m_rpcIdHelper->strip(channelId);
	    }
	    else
	    {
		channel	= m_tgcIdHelper->channel(channelId);
	    }
	    if (channel > channelMax)
	    {
		channelMax	= channel;
		rotMax		= *s;
	    }
	    if (channel < channelMin)
	    {
		channelMin	= channel;
		rotMin		= *s;
	    }
	    
	}
	limitingChannels.push_back(channelMin);
	limitingChannels.push_back(channelMax);
	limitingRots.push_back(rotMin);
	limitingRots.push_back(rotMax);
    }

    // debug
    if ( msgLvl(MSG::VERBOSE) )
    {
	std::list<int>::const_iterator l = limitingChannels.begin();
	std::list<int>::const_iterator m = limitingChannels.begin();
	int number	= 0;
	int size	= abs(*l - *(++m));
	for (std::vector<const Trk::PrepRawData*>::const_iterator q = usedPrd.begin();
	     q != usedPrd.end();
	     ++q, --size)
	{
	    Identifier channelId = (**q).identify();
	    if (m_isRpc)
	    {
		int stationIndex	= m_rpcIdHelper->stationName(channelId);
		ATH_MSG_VERBOSE(" rpc "  
		       << std::setiosflags(std::ios::fixed)
		       << " localPosition "
		       << std::setw(8) << std::setprecision(1) << (**q).localPosition()[Trk::locX]
		       << std::setw(8) << std::setprecision(1) << (**q).localPosition()[Trk::locY]
		       << "   doublet z/phi"
		       << std::setw(2) << m_rpcIdHelper->doubletZ(channelId)
		       << std::setw(2) << m_rpcIdHelper->doubletPhi(channelId)
		       << "   gasGap"  << std::setw(2) << m_rpcIdHelper->gasGap(channelId)
		       << "   strip"   << std::setw(3) << m_rpcIdHelper->strip(channelId)
		       << "   station " << m_rpcIdHelper->stationNameString(stationIndex)
		       << "  " << m_rpcIdHelper->show_to_string(channelId) );
	    }
	    else
	    {
		int stationIndex	= m_tgcIdHelper->stationName(channelId);
		ATH_MSG_VERBOSE(" tgc "
		       << std::setiosflags(std::ios::fixed)
		       << " localPosition "
		       << std::setw(8) << std::setprecision(1) << (**q).localPosition()[Trk::locX]
		       << std::setw(8) << std::setprecision(1) << (**q).localPosition()[Trk::locY]
		       << "   gasGap"  << std::setw(2) << m_tgcIdHelper->gasGap(channelId)
		       << "   channel" << std::setw(3) << m_tgcIdHelper->channel(channelId)
		       << "   station " << m_tgcIdHelper->stationNameString(stationIndex)
		       << "  " << m_tgcIdHelper->show_to_string(channelId) );
	    }
	    if (size == 0)
	    {
		ATH_MSG_VERBOSE(" cluster " << ++number
		       << "  between channels " << *l << " and " << *m );
		if (++m != limitingChannels.end())
		{
		    ++l;
		    size = 1 + abs(*(++l) - *(++m));
		}
	    }
	}
    }
}

void
TriggerChamberClusterOnTrackCreator::makeOverallParameters(
	Trk::LocalParameters*&					parameters,
	Amg::MatrixX*&					errorMatrix,
	Trk::Surface*&						surface,
	std::list<int>&						limitingChannels,
	std::list<const Muon::MuonClusterOnTrack*>&		limitingRots) const
{
    // surfaces, overall localPosition and error matrix
    //std::list<const Trk::Surface*>	surfaces;
    std::list<const Muon::MuonClusterOnTrack*>::const_iterator r = limitingRots.begin();
    Amg::Vector3D centre		= (**r).associatedSurface().center();
    Amg::MatrixX covariance	= (**r).localCovariance();
    parameters				= new Trk::LocalParameters((**r).localParameters());
    
    int pair = 1;
    for (++r;
	 r != limitingRots.end();
	 ++r, ++pair)
    {
	centre		+= (**r).associatedSurface().center();
	covariance	+= (**r).localCovariance();
	*parameters	+= (**r).localParameters();
    }
    double norm			= 1.;
    if(limitingRots.size()>0) norm /= static_cast<double>(limitingRots.size());
    std::list<int>::iterator l	= limitingChannels.begin();
    int firstChannel   		= *l;
    double width		= static_cast<double>(1 + abs(*(++l) - firstChannel));
    if (limitingRots.size() > 2)
    {
	int offset	= abs(*(++l) - firstChannel);
	if (! m_isRpc && offset < 2)
	{
	    width *= 0.5;
	}
	else
	{
	    width += static_cast<double>(offset);
	}
    }

    // get parameter means
    centre	*= norm;
    covariance	*= width*width*norm;
    *parameters	*= norm;

    // finally create the mean ErrorMatrix and the average Surface
    // note the cluster surfaces are assumed to have identical orientation and bounds
    errorMatrix			= new Amg::MatrixX(covariance);
    const Trk::Surface& surf	= (**limitingRots.begin()).associatedSurface();
    Amg::Transform3D rotation  = surf.transform();
    std::string shape		= "";

    const Trk::RectangleBounds* rectbds = dynamic_cast<const Trk::RectangleBounds*>(&surf.bounds());
    const Trk::TrapezoidBounds* trapbds = dynamic_cast<const Trk::TrapezoidBounds*>(&surf.bounds());
    const Trk::RotatedTrapezoidBounds* rottrapbds = dynamic_cast<const Trk::RotatedTrapezoidBounds*>(&surf.bounds());

    if (rectbds)
    {
       shape = " RPC rectangle ";
       surface = new Trk::PlaneSurface(
           new Amg::Transform3D(rotation),
           rectbds->clone());
    }
    else if (trapbds)
    {
       shape = " TGC trapezoid ";
       surface = new Trk::PlaneSurface(
           new Amg::Transform3D(rotation),
           trapbds->clone());
    }
    else if (rottrapbds)
    {
       shape = " TGC rotatedTrapezoid ";
       surface = new Trk::PlaneSurface(
           new Amg::Transform3D(rotation),
           rottrapbds->clone());
    }

    // debug
    if ( msgLvl(MSG::DEBUG) )
    {
	msg(MSG::DEBUG) << shape << "  width " << width
	       << "   localParameters " << (*parameters)[Trk::locX];
	if (covariance.cols() > 1) msg(MSG::DEBUG) << " " << (*parameters)[Trk::locY];
	msg(MSG::DEBUG) << "   covariance " << sqrt(covariance(Trk::locX,Trk::locX));
	if (covariance.cols() > 1) msg(MSG::DEBUG) << " " << sqrt(covariance(Trk::locY,Trk::locY));
	msg(MSG::DEBUG) << "   channel range (cluster) ";
	pair = 2;
	for (std::list<int>::iterator i = limitingChannels.begin();
	     i != limitingChannels.end();
	     ++i, ++pair)
	{
	    msg(MSG::DEBUG) << *i << " ";
	    if (pair%2) msg(MSG::DEBUG) << "(" << pair/2 << ")    ";
	}
	msg(MSG::DEBUG) << endmsg;
    }
}

}	// end of namespace
