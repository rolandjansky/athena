/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool selecting RDO's in vicinity of a Track prolongation into the TRT.
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetReadoutGeometry/TRT_BarrelElement.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TRT_Rec/TRT_DataSelector.h"
#include "TRT_Rec/TRT_Prediction.h"
#include "TRT_Rec/TRT_RoadData.h"

using InDetDD::TRT_BarrelElement;
using InDetDD::TRT_DetectorManager;
using InDetDD::TRT_EndcapElement;

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TRT_DataSelector::TRT_DataSelector (const std::string&	type,
				    const std::string&	name, 
				    const IInterface*	parent)
    :	AthAlgTool		(type, name, parent),
	m_debug			(false),
	m_driftCircleContainer	(0),
	m_log			(0),
	m_verbose		(false)
{
    declareInterface<ITRT_DataSelector>(this);
}

TRT_DataSelector::~TRT_DataSelector(void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TRT_DataSelector::initialize()
{
    // print where you are
    m_log = new MsgStream(msgSvc(), name());
    *m_log << MSG::INFO << "TRT_DataSelector::initialize()" << endmsg;
    if (m_log->level() < 3)
    {
	if (m_log->level() < 2) m_verbose = true;
	m_debug = true;
    }

    // check the DetectorStore service is available
    if (StatusCode::SUCCESS != detStore()->retrieve(m_trtHelper,"TRT_ID"))
    {
        ATH_MSG_FATAL( "Could not retrieve TRT_ID: switch off TRT " );
	m_trtHelper = 0;
    }

    delete m_log;
    m_log = 0;
    return StatusCode::SUCCESS;
}

StatusCode
TRT_DataSelector::finalize()
{
    delete m_log;
    m_log = new MsgStream(msgSvc(), name());
    *m_log << MSG::INFO << "TRT_DataSelector::finalize()" << endmsg;
    
    delete m_log;
    for (std::vector<TRT_RoadData*>::reverse_iterator rd = m_roadDataList.rbegin();
	 rd !=  m_roadDataList.rend();
	 ++rd)
    {
        delete *rd;
	m_roadDataList.pop_back();
    }
    return StatusCode::SUCCESS;
}

const std::vector<TRT_RoadData*>&
TRT_DataSelector::roadData(const std::vector<TRT_Prediction*>&	predictions,
			   double				rRoadVertex)
{
    // clear roadData and counters
    m_selectedLayers	= 0;
    m_selectedStraws	= 0;
    m_selectedTrLayers	= 0;
    
    for (std::vector<TRT_RoadData*>::reverse_iterator rd = m_roadDataList.rbegin();
	 rd !=  m_roadDataList.rend();
	 ++rd)
    {
        delete *rd;
	m_roadDataList.pop_back();
    }

    // quit if no data (missing TRT_DriftCircle container)
    if (! m_driftCircleContainer) return m_roadDataList;

    // collect data in narrow road - but stop if too large a gap is encountered
    unsigned maxGap	= 16;
    unsigned gap	= 0;
    unsigned size	= 0;
    std::vector<TRT_Prediction*>::const_iterator pLast = predictions.end();
    
    for (std::vector<TRT_Prediction*>::const_iterator p = predictions.begin();
	 p != predictions.end();
	 ++p)
    {
	if ((**p).isDiscard()) continue;
	if ((**p).nearBoundary())
	{
	    // no gap penalty when close to boundary
	}
	else if (m_roadDataList.size() == size)
	{
	    if (++gap > maxGap) break;
	}
	else
	{
	    gap		= 0;
	    pLast	= p;
	    size	= m_roadDataList.size();
	}
	
	// print out of selected predictions
// 	(**p).print();

	// get DriftCircle collection
	InDet::TRT_DriftCircleContainer::const_iterator driftCircleCollection =
	    m_driftCircleContainer->indexFind((**p).hashId());
	if (driftCircleCollection == m_driftCircleContainer->end()) continue;

// 	// Now, look at all straw data, and for each, create the correct
// 	// road test.
// 	// Then store it ready for the pattern recognition (histogram method).
	Amg::Vector3D prediction	= (**p).position();
	bool newLayer			= true;
	bool newTrLayer			= true;
	// bool foundDrift			= false;
	
	//*** iterate over TRT_DriftCircles.
	for (InDet::TRT_DriftCircleCollection::const_iterator driftCircle =
		 (**driftCircleCollection).begin();
	     driftCircle != (**driftCircleCollection).end();
	     ++driftCircle)
	{
	    Identifier	strawId	= (**driftCircle).identify();
	    int		straw	= m_trtHelper->straw(strawId);
// 	    std::cout << " found straw# " << straw
// 		      << " size,gap " << size << " " << gap << std::endl;
	    
            // TODO::
	    //     cut on predicted straw range

	    // get closest point of approach of straw to prediction
	    Amg::Vector3D	 strawAxis;
	    Amg::Vector3D	 strawPos;
	    bool		 inBarrel = true;
	    if ((**p).barrelElement())
	    {
		const TRT_BarrelElement& element= *(**p).barrelElement();
		strawAxis			=  element.strawAxis(straw);
		strawPos       			=  element.strawCenter(straw);
	    }
	    else
	    {
		inBarrel = false;
		const TRT_EndcapElement& element= *(**p).endcapElement();
		strawAxis			=  element.strawAxis(straw);
		strawPos       			=  element.strawCenter(straw);
	    }
	    double distance		=  (prediction - strawPos).dot(strawAxis);
	    strawPos			+= distance*strawAxis;
	    Amg::Vector3D offset	=  strawPos - prediction;
	    double r			=  strawPos.perp();
	    double rPhiRoad		=  (offset.y()*strawPos.x() - offset.x()*strawPos.y())/r;

// 	    std::cout << std::setiosflags(std::ios::fixed)
// 		      << " straw #"
// 		      << std::setw(6) << straw
// 		      << "  straw: r,phi,z "
// 		      << std::setw(9) << std::setprecision(3) << strawPos.perp()
// 		      << std::setw(9) << std::setprecision(4) << strawPos.phi()
// 		      << std::setw(9) << std::setprecision(1) << strawPos.z()
// 		      << std::endl;
				   
// 	    std::cout << " phiRoad,deltaPhi " << std::setiosflags(std::ios::fixed)
// 		      << std::setw(12) << std::setprecision(4)
// 		      << strawPos.phi() - (**p).phi()
// 		      << std::setw(12) << std::setprecision(4)
// 		      << rPhiRoad/r
// 		      << std::setw(12) << std::setprecision(4)
// 		      << (**p).phi()
// 		      << std::endl;
	    
// 	    if ((**p).expectedStraws() - m_selectedLayers > 6) std::cout << " missing layers " << std::endl;
	    
	    if (!(**p).inPhiRange(rPhiRoad/r))
	    {
// 		std::cout << "   outside phi range " << std::endl;
		if (rPhiRoad > 0.) break;
		continue;
	    }

	    ++m_selectedStraws;
	    if (newLayer)
	    {
		++m_selectedLayers;
		newLayer = false;
	    }
	    if (newTrLayer && (**driftCircle).highLevel())
	    {
		++m_selectedTrLayers;
                newTrLayer = false;
	    }

	    // put short drift distances onto wire
	    double     			driftDistance	= 0.;
	    TRT_RoadData::StrawStatus	strawStatus	= TRT_RoadData::pendingWire;
	    if ((**driftCircle).localPosition()[Trk::locX] > 0.20*Gaudi::Units::mm)
	    {
		driftDistance 	= (**driftCircle).localPosition()[Trk::locX];
		strawStatus    	= TRT_RoadData::pendingDrift;
	    }
	    
// 	    std::cout << " drift distance " 
// 		      << static_cast<int>(1000.*driftDistance) << std::endl;
	    
	    double rhoRoad	= r - rRoadVertex;
            //      std::cout << "strawPhi driftDistance r phiP phiM = " << std::setprecision(6)
            //                << strawPhi << " "  << driftDistance << " " << r
            //                << " "  << phiP << " " << phiM << std::endl;

	    // save straw hit followed by wire or left/right drift hits (as appropriate)
	    m_roadDataList.push_back(new TRT_RoadData(inBarrel,
						      strawId,
						      (**p).projectionFactor(),
						      rPhiRoad,
						      rhoRoad,
						      *driftCircle,
						      strawPos,
						      (**driftCircle).highLevel()));

	    // positive drift sign (or on wire) - if in narrow road
	    if ((**p).inDriftRoad(rPhiRoad + driftDistance))
	    {
		//              std::cout << "PhiPlus OK" << std::endl;
		// foundDrift = true;
		m_roadDataList.push_back(new TRT_RoadData(inBarrel,
							  driftDistance,
							  +1,
							  strawId,
							  (**p).projectionFactor(),
							  rPhiRoad,
							  rhoRoad,
							  *driftCircle,
							  strawPos,
							  strawStatus,
							  (**driftCircle).highLevel()));
            }
	    
            // negative drift sign - if in narrow road
	    if (strawStatus == TRT_RoadData::pendingDrift
		&& (**p).inDriftRoad(rPhiRoad - driftDistance))
	    {
	        //              std::cout << "PhiMinus OK" << std::endl;
	        // foundDrift = true;
		m_roadDataList.push_back(new TRT_RoadData(inBarrel,
							  driftDistance,
							  -1,
							  strawId,
							  (**p).projectionFactor(),
							  rPhiRoad,
							  rhoRoad,
							  *driftCircle,
							  strawPos,
							  strawStatus,
							  (**driftCircle).highLevel()));
            }
	}
// 	if (!newLayer && !foundDrift) std::cout << " layer without drift hit " << std::endl;
    }

    // in the case when a gap was encountered,
    // resume data selection without drift info from the gap onwards,
    // using an electron road for brem recovery
    if (gap > maxGap && pLast != predictions.end())
    {
// 	std::cout << " resume after gap ";
// 	(**pLast).print();
	
	for (std::vector<TRT_Prediction*>::const_iterator p = ++pLast;
	     p != predictions.end();
	     ++p)
	{
	    if ((**p).isDiscard()) continue;
	    bool newLayer			= true;
	    bool newTrLayer			= true;
	    Amg::Vector3D prediction		= (**p).position();

	    // get DriftCircle collection
	    InDet::TRT_DriftCircleContainer::const_iterator driftCircleCollection =
		m_driftCircleContainer->indexFind((**p).hashId());
	    if (driftCircleCollection == m_driftCircleContainer->end()) continue;
	    //*** iterate over TRT_DriftCircles.
	    for (InDet::TRT_DriftCircleCollection::const_iterator driftCircle =
		     (**driftCircleCollection).begin();
		 driftCircle != (**driftCircleCollection).end();
		 ++driftCircle)
	    {
		Identifier		strawId	= (**driftCircle).identify();
		int			straw	= m_trtHelper->straw(strawId);
		Amg::Vector3D		strawAxis;
		Amg::Vector3D		strawPos;
		bool			inBarrel = true;
		if ((**p).barrelElement())
		{
		    const TRT_BarrelElement& element	= *(**p).barrelElement();
		    strawAxis				= element.strawAxis(straw);
		    strawPos				= element.strawCenter(straw);
		}
		else
		{
		    inBarrel				= false;
		    const TRT_EndcapElement& element	= *(**p).endcapElement();
		    strawAxis				= element.strawAxis(straw);
		    strawPos				= element.strawCenter(straw);
		}
		double distance			=  (prediction - strawPos).dot(strawAxis);
		strawPos			+= distance*strawAxis;
		Amg::Vector3D offset		=  strawPos - prediction;
		double r			=  strawPos.perp();
		double rPhiRoad			=  (offset.y()*strawPos.x() - offset.x()*strawPos.y())/r;

		if (! (**p).inElectronPhiRange(rPhiRoad/r)) continue;

		++m_selectedStraws;
		if (newLayer)
		{
		    ++m_selectedLayers;
		    newLayer = false;
		}
		if (newTrLayer && (**driftCircle).highLevel())
		{
		    ++m_selectedTrLayers;
		    newTrLayer = false;
		}

		m_roadDataList.push_back(new TRT_RoadData(inBarrel,
							  strawId,
							  (**p).projectionFactor(),
							  rPhiRoad,
							  r - rRoadVertex,
							  *driftCircle,
							  strawPos,
							  (**driftCircle).highLevel()));
// 		std::cout << std::setiosflags(std::ios::fixed)
// 			  << " straw #"
// 			  << std::setw(6) << straw
// 			  << " r,z"
// 			  << std::setw(9) << std::setprecision(3) << strawPos.perp()
// 			  << std::setw(9) << std::setprecision(1) << strawPos.z()
// 			  << "  rPhiRoad" << std::setw(9) << std::setprecision(1) << rPhiRoad
// 			  << "  tr " << (**driftCircle).highLevel()
// 			  << std::endl;
	    }
	}
    }
	
    return m_roadDataList;
}

void
TRT_DataSelector::setDataContainer (const InDet::TRT_DriftCircleContainer* container)
{
    if (m_trtHelper) m_driftCircleContainer = container;
    if (m_verbose)
    {
	delete m_log;
	m_log = new MsgStream(msgSvc(), name());
	*m_log << MSG::VERBOSE << "setDataContainer: setting TRT_DriftCircleContainer" << endmsg;
    }
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>






