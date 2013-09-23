/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
// AlgTool used for trigger chamber MuonClusterOnTrack object creation
//
// (c) ATLAS muon software
///////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "TriggerChamberClusterOnTrackCreator.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "StoreGate/StoreGateSvc.h"
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
  :	AlgTool				(type, name, parent),
	m_clusterCreator		("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
	m_chooseBroadestCluster		(true),
	m_debug				(false),
	m_log				(0),
	m_verbose			(false)	
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
    MsgStream log(msgSvc(),name());
    if (log.level() < 3) m_debug	= true;
    if (log.level() < 2) m_verbose	= true;
    
    // get the Helpers
    StoreGateSvc* detStore = 0;
    if (StatusCode::SUCCESS != service("DetectorStore", detStore))
    {
	log << MSG::ERROR << " Can't get DetectorStore " << endreq;
	return StatusCode::FAILURE;
    }

    const MuonGM::MuonDetectorManager*  muonManager;
    if (StatusCode::SUCCESS != detStore->retrieve(muonManager))
    {
	log << MSG::ERROR << " Cannot retrieve MuonDetectorManager " << endreq;
	return StatusCode::FAILURE;
    }
    m_rpcIdHelper = muonManager->rpcIdHelper();
    m_tgcIdHelper = muonManager->tgcIdHelper();

    // get the Tools
    if (m_clusterCreator.retrieve().isFailure())
    {
	log << MSG::FATAL << "Failed to retrieve tool " << m_clusterCreator << endreq;
	return StatusCode::FAILURE;
    }
    else
    {
	log << MSG::INFO << "Retrieved tool " << m_clusterCreator << endreq;
    }

    return StatusCode::SUCCESS;
}

StatusCode
TriggerChamberClusterOnTrackCreator::finalize()
{
    delete m_log;
    return StatusCode::SUCCESS;
}
  
const CompetingMuonClustersOnTrack*
TriggerChamberClusterOnTrackCreator::createBroadCluster(const std::list<const Trk::PrepRawData*>& prds,
							const double) const
{
    if (m_debug)
    {
	delete m_log;
	m_log = new MsgStream(msgSvc(), name());
	*m_log << MSG::VERBOSE << "enter createBroadCluster: number of prds " << prds.size() << endreq;
    }

    // make some PRD consistency checks
    if (! prds.size())
    {
	MsgStream log(msgSvc(), name());
	log << MSG::WARNING << "fails: empty PRD list " << endreq;
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
	    MsgStream log(msgSvc(), name());
	    log << MSG::WARNING << "fails: PRD must be from rpc or tgc " << endreq;
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
	    MsgStream log(msgSvc(), name());
	    log << MSG::WARNING << "fails: PRDs must measure same coordinate " << endreq;
	    return 0;
	}
	if ((**p).detectorElement() != detectorElement)
	{
	    	MsgStream log(msgSvc(), name());
		log << MSG::WARNING << "fails: PRDs must be from same detectorElement " << endreq;
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
	if (m_verbose) *m_log << MSG::VERBOSE << " discard cluster #" << 1 << endreq;
	limitingRots.pop_front();
	limitingRots.pop_front();
	limitingChannels.pop_front();
	limitingChannels.pop_front();
    }
    else if (discard != limitingChannels.end())
    {
	if (m_verbose) *m_log << MSG::VERBOSE << " discard cluster #" << numClusters << endreq;
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
    if (m_verbose)
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
		*m_log << MSG::VERBOSE << " rpc "  
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
		       << "  " << m_rpcIdHelper->show_to_string(channelId)
		       << endreq;
	    }
	    else
	    {
		int stationIndex	= m_tgcIdHelper->stationName(channelId);
		*m_log << MSG::VERBOSE << " tgc "
		       << std::setiosflags(std::ios::fixed)
		       << " localPosition "
		       << std::setw(8) << std::setprecision(1) << (**q).localPosition()[Trk::locX]
		       << std::setw(8) << std::setprecision(1) << (**q).localPosition()[Trk::locY]
		       << "   gasGap"  << std::setw(2) << m_tgcIdHelper->gasGap(channelId)
		       << "   channel" << std::setw(3) << m_tgcIdHelper->channel(channelId)
		       << "   station " << m_tgcIdHelper->stationNameString(stationIndex)
		       << "  " << m_tgcIdHelper->show_to_string(channelId)
		       << endreq;
	    }
	    if (size == 0)
	    {
		*m_log << MSG::VERBOSE << " cluster " << ++number
		       << "  between channels " << *l << " and " << *m << endreq;
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
    double norm			= 1./static_cast<double>(limitingRots.size());
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
    Amg::Transform3D rotation	= surf.transform();
    std::string shape		= "";
    if (dynamic_cast<const Trk::RectangleBounds*>(&surf.bounds()))
    {
	shape = " RPC rectangle ";
	surface = new Trk::PlaneSurface(
	    new Amg::Transform3D(rotation),
	    dynamic_cast<const Trk::RectangleBounds*>(&surf.bounds())->clone());
    }
    else if (dynamic_cast<const Trk::TrapezoidBounds*>(&surf.bounds()))
    {
	shape = " TGC trapezoid ";
	surface = new Trk::PlaneSurface(
	    new Amg::Transform3D(rotation),
	    dynamic_cast<const Trk::TrapezoidBounds*>(&surf.bounds())->clone());
    }
    else if (dynamic_cast<const Trk::RotatedTrapezoidBounds*>(&surf.bounds()))
    {
	shape = " TGC rotatedTrapezoid ";
	surface = new Trk::PlaneSurface(
	    new Amg::Transform3D(rotation),
	    dynamic_cast<const Trk::RotatedTrapezoidBounds*>(&surf.bounds())->clone());
    }

    // debug
    if (m_debug)
    {
	*m_log << MSG::DEBUG << shape << "  width " << width
	       << "   localParameters " << (*parameters)[Trk::locX];
	if (covariance.cols() > 1) *m_log << " " << (*parameters)[Trk::locY];
	*m_log << "   covariance " << sqrt(covariance(Trk::locX,Trk::locX));
	if (covariance.cols() > 1) *m_log << " " << sqrt(covariance(Trk::locY,Trk::locY));
	*m_log << "   channel range (cluster) ";
	pair = 2;
	for (std::list<int>::iterator i = limitingChannels.begin();
	     i != limitingChannels.end();
	     ++i, ++pair)
	{
	    *m_log << *i << " ";
	    if (pair%2) *m_log << "(" << pair/2 << ")    ";
	}
	*m_log << endreq;
    }
}

}	// end of namespace
