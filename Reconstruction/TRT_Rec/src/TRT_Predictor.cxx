/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool creating TRT_Predictions (along a Track prolongation into the TRT)
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/TRT_BarrelElement.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "TRT_Rec/TRT_Prediction.h"
#include "TRT_Rec/TRT_Predictor.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "iPatTrackParameters/PerigeeParameters.h"

#include "RoiDescriptor/RoiDescriptor.h"

using InDetDD::TRT_BarrelElement;
using InDetDD::TRT_BaseElement;
using InDetDD::TRT_DetectorManager;
using InDetDD::TRT_EndcapElement;

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TRT_Predictor::TRT_Predictor (const std::string&	type,
			      const std::string&	name, 
			      const IInterface*      	parent)
    :	AthAlgTool			(type, name, parent),
	m_regionSelector		("RegSelSvc", name),
	m_rungeKuttaIntersector		("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
	m_manager			(nullptr),
	m_barrelAcceptance		(0.55),
	m_cotThetaCorner		(2708./968.),
	m_electronRoadFactor		(3.0),
	m_etaRoadSafetyFactor		(0.03),
	m_extrapolationTolerance	(20.*Gaudi::Units::mm),
	m_layerTolerance		(2.*Gaudi::Units::mm),
	m_midBarrelSurface		(nullptr),
	m_outerBackwardsSurface		(nullptr),
	m_outerForwardsSurface		(nullptr),
	m_phiSectors			(32),
    	m_rMeanBarrel			(816.*Gaudi::Units::mm),
	m_strawWidth			(4.0*Gaudi::Units::mm),
    	m_zEndcapMax			(2708.*Gaudi::Units::mm),
	m_cosPhiStart			(0.),
	m_deltaCurv			(0.),
	m_deltaPhi			(0.),
	m_expectedStraws		(0.),
	m_hashIds			(new std::vector<IdentifierHash>),
	m_intersectedLayers		(0),
	m_intersection			(0),
	m_phiHalfWidth			(0.),
	m_qOverP			(0.),
	m_rWidth			(0.),
	m_signDeltaPhi			(0.),
	m_sinPhiStart			(0.),
	m_strawHalfWidth		(0.),
	m_zWidth			(0.)
{
    declareInterface<ITRT_Predictor>(this);
    declareProperty("Intersector",		m_rungeKuttaIntersector);
    declareProperty("barrelAcceptance",		m_barrelAcceptance);
    declareProperty("electronRoadFactor",	m_electronRoadFactor);
    declareProperty("etaRoadSafetyFactor",	m_etaRoadSafetyFactor);
    declareProperty("extrapolationTolerance",	m_extrapolationTolerance);
    declareProperty("layerTolerance",		m_layerTolerance);
    declareProperty("phiSectors",		m_phiSectors);
    declareProperty("rMeanBarrel",		m_rMeanBarrel);
    declareProperty("strawWidth",		m_strawWidth);
    declareProperty("zEndcapMax",		m_zEndcapMax);
    declareProperty("cotThetaCorner",		m_cotThetaCorner);
}

TRT_Predictor::~TRT_Predictor(void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TRT_Predictor::initialize()
{
    // print where you are
    MsgStream log(msgSvc(), name());
    ATH_MSG_INFO( "TRT_Predictor::initialize()" );

    // Retrieve GeoModel Detector Elements and set some parameters
    if (StatusCode::SUCCESS != detStore()->retrieve(m_manager,"TRT"))
    {
	ATH_MSG_WARNING( "Could not find the TRT Manager: "
			 << m_managerName << ". The TRT is switched off !" );
    }

    // get the RegionSelector service
    if (m_regionSelector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve service " << m_regionSelector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved service " << m_regionSelector );
    }

    // retrieve the Runge-Kutta Intersector
    if (m_rungeKuttaIntersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_rungeKuttaIntersector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_rungeKuttaIntersector );
    }

    // create surfaces describing TRT midpoint
    Amg::Transform3D* transform1	= new Amg::Transform3D;
    transform1->setIdentity();
    (*transform1)			= Amg::Translation3D(0.,0.,0.);
    double halfZLength			= m_cotThetaCorner*m_rMeanBarrel;
    m_midBarrelSurface			= new Trk::CylinderSurface(transform1,m_rMeanBarrel,halfZLength);
    Amg::Transform3D* transform2	= new Amg::Transform3D;
    transform2->setIdentity();
    (*transform2)			= Amg::Translation3D(0.,0.,-m_zEndcapMax);
    m_outerBackwardsSurface		= new Trk::DiscSurface(transform2, 0., m_rMeanBarrel);
     Amg::Transform3D* transform3	= new Amg::Transform3D;
    transform3->setIdentity();
    (*transform3)			= Amg::Translation3D(0.,0.,m_zEndcapMax);
    m_outerForwardsSurface		= new Trk::DiscSurface(transform3, 0., m_rMeanBarrel);
    
    // initialise some useful constants
    m_phiHalfWidth			= M_PI/static_cast<double>(m_phiSectors);
    m_strawHalfWidth			= 0.5*m_strawWidth;

    return StatusCode::SUCCESS;
}

StatusCode
TRT_Predictor::finalize(void)
{
    ATH_MSG_INFO( "TRT_Predictor::finalize()" );
    delete m_midBarrelSurface;
    delete m_outerBackwardsSurface;
    delete m_outerForwardsSurface;
    delete m_hashIds;
    for (std::vector<TRT_Prediction*>::reverse_iterator p = m_predictions.rbegin();
	 p !=  m_predictions.rend();
	 ++p)
    {
	delete *p;
	m_predictions.pop_back();
    }
    return StatusCode::SUCCESS;
}

const std::vector<TRT_Prediction*>&
TRT_Predictor::makePredictions(const PerigeeParameters&	perigee,
			       const TrackParameters&	parameters)
{
    // erase old predictions
    m_expectedStraws	= 0.;
    m_intersectedLayers	= 0;
    for (std::vector<TRT_Prediction*>::reverse_iterator p = m_predictions.rbegin();
	 p !=  m_predictions.rend();
	 ++p)
    {
	delete *p;
	m_predictions.pop_back();
    }

    // initial parameters
    m_cosPhiStart	= parameters.direction().x();
    m_sinPhiStart	= parameters.direction().y();
    m_deltaPhi		= 5.0*std::sqrt(perigee.covariance()(2,2));
    m_deltaCurv		= 3.0*std::sqrt(perigee.covariance()(4,4))*perigee.abs_pt();
    m_intersection	= new Trk::TrackSurfaceIntersection(parameters.position(),
							    parameters.direction(),
							    0.);
    m_qOverP		= parameters.qOverP();
    m_zWidth		= 5.0*std::sqrt(perigee.covariance()(1,1));
        
    // create vector<HashIdentifiers> according to the predicted (eta, phi) at the TRT
    setRegion(parameters);
    if (! m_hashIds->size())
    {
	delete m_intersection;
	return m_predictions;
    }
    
    // make a TRT_Prediction for each element in region.
    // Subject to crude filtering using line parameters in rz projection
    crudePredictions();

    // std::cout << "  have crudePredictions " << std::endl;
    
    // sort predictions in outwards order:
    std::sort (m_predictions.begin(), m_predictions.end(), TRT_Prediction::OutwardsOrder());

    // now make fine check to discard any predictions incompatible in phi.
    // Otherwise compute and store electron and narrow road widths and
    // the number of intersected layers with the number of straws to be expected
    finePredictions();
    delete m_intersection;

    // std::cout << "  have finePredictions " << std::endl;
    // std::cout << "Identifiers found by Region Selector: size = "
    // 	      << m_hashIds->size()
    // 	      << "   predictions: size = "
    // 	      << m_predictions.size() << std::endl;
    
    
    return m_predictions;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
TRT_Predictor::crudePredictions(void)
{
    // make a TRT_Prediction for each element in region.
    // Subject to crude filtering using line parameters in rz projection:
    double cotTheta     =  m_intersection->direction().z()/m_intersection->direction().perp();
    double rMean        =  m_intersection->position().perp();
    double zMean        =  m_intersection->position().z();
    
    for (std::vector<IdentifierHash>::const_iterator id = m_hashIds->begin();
	 id != m_hashIds->end();
	 ++id)
    {
        const TRT_BaseElement* baseElement	= m_manager->getElement(*id);
	const TRT_BarrelElement* barrelElem	= 0;
	const TRT_EndcapElement* endcapElem	= 0;
	if (baseElement->type() == TRT_BaseElement::BARREL)
	{
	    barrelElem	= dynamic_cast<const TRT_BarrelElement*>(baseElement);
	}
	else
	{
	    endcapElem	= dynamic_cast<const TRT_EndcapElement*>(baseElement);
	}
	if (barrelElem)
	{
	    int    straw	= barrelElem->nStraws()/2;
	    double x		= barrelElem->strawXPos(straw);
	    double y		= barrelElem->strawYPos(straw);
	    double r		= sqrt(x*x + y*y);
	    double zPred	= zMean + cotTheta*(r - rMean);
	    double zPos		= barrelElem->strawZPos(straw);	// this is not the active centre - bug?
	    // double zDif = std::abs(zPred) - barrelElem->getDescriptor()->strawZDead();	// dz from start of active 
	    double length = barrelElem->strawLength() / 2.;

	    // if (zDif < -m_zWidth || zDif > barrelElem->strawLength()+m_zWidth) continue;
	    // if (zPred*zPos < 0.) continue;

	    if ((zPos - length + m_zWidth) >= zPred || zPred >= (zPos + length - m_zWidth)) continue;

	    TRT_Prediction* pred = new TRT_Prediction(barrelElem,
						      0,
						      *id,
						      r,
						      zPred);
	    // if (zDif < m_zWidth || zDif > barrelElem->strawLength()-m_zWidth) pred->boundary();

	     if ((zPos + length >= zPred && zPred >= zPos + length - m_zWidth) ||
	      (zPos - length <= zPred && zPred <= zPos - length + m_zWidth))   
	       pred->boundary();

	    m_predictions.push_back(pred);
	    
	    // std::cout << " radius " << r
	    // 	      << "   zpred " << zPred
	    // 	      << "   elem.strawZPos(centre) " << barrelElem->getDescriptor()->strawZPos()
	    // 	      << "   " << barrelElem->strawZPos(straw)
	    // 	      << "   elem.strawLength() " << barrelElem->getDescriptor()->strawLength()
	    // 	      << "   " << barrelElem->strawLength()
	    // 	      << "  dead " <<  barrelElem->getDescriptor()->strawZDead()
	    // 	      << std::endl;
	}
	else if (endcapElem)
	{
	    //	    HepGeom::Transform3D t = endcapElem->getAbsoluteTransform(1);
	    
	    //	    std::cout << " transform " << std::endl;
	    //	    std::cout << " transform " << t.getTranslation() << std::endl;
	    //	    std::cout << " transform " << t.getRotation() << std::endl;
	    
	    //	    Amg::Vector3D centre = endcapElem->getAbsoluteTransform(1)*Amg::Vector3D(0.,0.,0.);
	    
	    double z		= endcapElem->transform().translation().z();
	    double rPred	= rMean + (z - zMean)/cotTheta;
	    m_rWidth		= m_zWidth/std::abs(cotTheta);
	    double deltaR	= rPred - endcapElem->getDescriptor()->innerRadius();
	    if (deltaR < -m_rWidth
		|| deltaR > endcapElem->getDescriptor()->strawLength() + m_rWidth) continue;
	    TRT_Prediction* pred = new TRT_Prediction(0,
						      endcapElem,
						      *id,
						      rPred,
						      z);
	    if (deltaR < +m_rWidth
		|| deltaR > endcapElem->getDescriptor()->strawLength() - m_rWidth) pred->boundary();
	    m_predictions.push_back(pred);
	    
	    // std::cout << " radius " << rPred
	    // 	      << "   zpred " << z
	    // 	      << "   centre " << centre
	    // 	      << std::endl;
	}
    }
}

void
TRT_Predictor::finePredictions(void)
{
    // now discard any predictions incompatible in phi
    // otherwise compute and store electron and narrow road widths and
    // the number of intersected layers with the number of straws to be expected
    
    // extrapolation starts from last silicon measurement (== roadVertex)
    double rRoadVertex	= m_intersection->position().perp();
    double phiPrev	= m_intersection->position().phi();
    double rPrev	= rRoadVertex;
    double zPrev	= m_intersection->position().z();
    double phiSlope	= 0.;
    double rSlope	= 0.;
    double zSlope	= 0.;
    int step = 0;
    
    for (std::vector<TRT_Prediction*>::const_iterator p = m_predictions.begin();
         p != m_predictions.end();
         ++p)
    {
	bool extrapolationPerformed = false;
	if ((**p).barrelElement())
	{
	    double r		= (**p).r();
	    double deltaR	= r - rPrev;
	    if (deltaR > m_extrapolationTolerance)
	    {
		// extrapolate to plane
		if (m_intersection)
		{
		    extrapolationPerformed = true;
		    const Trk::TrackSurfaceIntersection* nextIntersection =
			m_rungeKuttaIntersector->intersectSurface((**p).surface(),
								  m_intersection,
								  m_qOverP);
		    // finish if trapped
		    if (! nextIntersection)
		    {
			(**p).discard();
			while (++p != m_predictions.end()) (**p).discard();
			break;
		    }
		    
		    delete m_intersection;
		    m_intersection	= nextIntersection;
		    r			= m_intersection->position().perp();
		    deltaR		= r - rPrev;
		}
	
		double phi	= m_intersection->position().phi();
		phiSlope	= (phi - phiPrev)/deltaR;
		zSlope		= (m_intersection->position().z() - zPrev)/deltaR;
		phiPrev		= phi;
		rPrev		= r;
		zPrev		= m_intersection->position().z();
		deltaR		= 0.;
		++step;
	    }
	    double phi  =  phiPrev + phiSlope*deltaR;
	    double z	=  zPrev + zSlope*deltaR;

	    // find closest approach of first/last straws to prediction
	    Amg::Vector3D prediction(r*cos(phi), r*sin(phi), z);
	    const TRT_BarrelElement& element	=  *(**p).barrelElement();
	    int	firstStraw			=  0;
	    Amg::Vector3D	firstAxis	=  element.strawAxis(firstStraw);
	    Amg::Vector3D	firstPos       	=  element.strawCenter(firstStraw);
	    double distance			=  firstAxis.dot(prediction - firstPos);
	    firstPos				+= distance*firstAxis;
	    int lastStraw			=  element.nStraws() - 1;
	    Amg::Vector3D	lastAxis	=  element.strawAxis(lastStraw);
	    Amg::Vector3D	lastPos       	=  element.strawCenter(lastStraw);
	    distance				=  lastAxis.dot(prediction - lastPos);
	    lastPos				+= distance*lastAxis;
	    // std::cout << std::setiosflags(std::ios::fixed)
 	    // 	      << std::setw(9) << std::setprecision(3)
	    // 	      << " prediction r,phi,z " << (**p).r() << " " << phi << " " << z
	    // 	      << "   strawPos " << firstPos
	    // 	      << "   centre " << element.strawCenter(firstStraw)
	    // 	      << std::endl;

	    double firstPhi	= firstPos.phi();
	    double phiRange	= lastPos.phi() - firstPhi;
	    if (phiRange < 0.) phiRange += 2.*M_PI;
	    double pitch	= phiRange / static_cast<double>(lastStraw);
	    double deltaPhi	=  phiPrev + phiSlope*deltaR - firstPhi - 0.5*phiRange;
	    if (deltaPhi < -M_PI) deltaPhi += 2.*M_PI;
	    
	    // correct predicted r as the straws lie in a plane tangent to the element phi centre
	    if (! extrapolationPerformed && std::abs(deltaPhi) < m_phiHalfWidth)
	    {
		double dR	=  0.5*r*deltaPhi*deltaPhi;
		deltaR		+= dR;
		deltaPhi	+= phiSlope*dR;
		r		=  rPrev + deltaR;
	    }

	    double nStraws		=  element.nStraws();
	    double straw		=  deltaPhi/pitch + 0.5*nStraws;
	    double predWidth		=  m_deltaPhi*r +
					   m_deltaCurv*(r - rRoadVertex) *
					   std::abs(m_intersection->direction().y()*m_cosPhiStart -
						    m_intersection->direction().x()*m_sinPhiStart);
	    double widthStrawUnits	=  (predWidth + m_strawHalfWidth) / (r*pitch);

	    // std::cout << std::setiosflags(std::ios::fixed)
 	    // 	      << std::setw(9) << std::setprecision(3)
	    // 	      << "  straw " << straw << std::endl;
	    // std::cout << std::setiosflags(std::ios::fixed)
	    // 	      << std::setw(9) << std::setprecision(1)
 	    // 	      << "  r " << r
  	    // 	      << std::setw(9) << std::setprecision(3)
	    // 	      << "  angle term " << m_deltaPhi*(r - rRoadVertex)
	    // 	      << "  curv term " << m_deltaCurv*(r - rRoadVertex)*
	    // 	std::abs(m_intersection->direction().y()*m_cosPhiStart -
	    // 		 m_intersection->direction().x()*m_sinPhiStart)
	    // 	      << "  width " << widthStrawUnits * r*pitch << std::endl;
	    
	    if (straw > 0. && straw < nStraws)
	    {
		++m_intersectedLayers;
		if (! (**p).nearBoundary()) m_expectedStraws += m_strawWidth/(r*pitch);
	    }
	    else if (straw > nStraws + widthStrawUnits || straw < -widthStrawUnits)
	    {
		(**p).discard();
		continue;
	    }
	    else
	    {
		(**p).boundary();
	    }

	    phi	=  phiPrev + phiSlope*deltaR;
	    z	=  zPrev + zSlope*deltaR;
	    (**p).setExpectedStraws(m_expectedStraws);
	    (**p).setPosition(phi,
			      r,
			      z,
			      Amg::Vector3D(m_intersection->direction()));	//// NOT
	    double deltaPhiNarrow = pitch*widthStrawUnits;
	    (**p).setPhiRange(deltaPhiNarrow*m_signDeltaPhi*m_electronRoadFactor,
			      deltaPhiNarrow,
			      predWidth,
			      rRoadVertex);
	}
	else	// endcap
	{
	    double phi;
	    double r;
	    double deltaZ	= (**p).z() - zPrev;
	    if (std::abs(deltaZ) > m_extrapolationTolerance)
	    {
		// extrapolate to plane
		if (m_intersection)
		{
		    const Trk::TrackSurfaceIntersection* nextIntersection =
			m_rungeKuttaIntersector->intersectSurface((**p).surface(),
								  m_intersection,
								  m_qOverP);
		    // finish if trapped
		    if (! nextIntersection)
		    {
			(**p).discard();
			while (++p != m_predictions.end()) (**p).discard();
			break;
		    }
		    delete m_intersection;
		    m_intersection	= nextIntersection;
		}
		phi		= m_intersection->position().phi();
		r		= m_intersection->position().perp();
		phiSlope	= (phi - phiPrev)/deltaZ;
		rSlope		= (r - rPrev)/deltaZ;
		phiPrev		= phi;
		rPrev		= r;
		zPrev		= (**p).z();
		++step;
	    }
	    else
	    {
	        // set r,phi by linear extrapolation
	        phi     	= phiPrev + phiSlope*deltaZ;
		r       	= rPrev + rSlope*deltaZ;
	    }
	    
	    // find closest approach of first/last straws to prediction
	    Amg::Vector3D prediction((**p).r()*cos(phi), (**p).r()*sin(phi), (**p).z());
	    const TRT_EndcapElement& element	=  *(**p).endcapElement();
	    int	firstStraw			=  0;
	    Amg::Vector3D	firstAxis	=  element.strawAxis(firstStraw);
	    Amg::Vector3D	firstPos       	=  element.strawCenter(firstStraw);
	    double distance			=  firstAxis.dot(prediction - firstPos);
	    firstPos				+= distance*firstAxis;
	    int lastStraw			=  element.nStraws() - 1;
	    Amg::Vector3D	lastAxis	=  element.strawAxis(lastStraw);
	    Amg::Vector3D	lastPos       	=  element.strawCenter(lastStraw);
	    distance				=  lastAxis.dot(prediction - lastPos);
	    lastPos				+= distance*lastAxis;
	    double firstPhi			=  firstPos.phi();
	    double phiRange			=  lastPos.phi() - firstPhi;
	    if (phiRange < 0.) phiRange 	+= 2.*M_PI;
	    double pitch			=  phiRange / static_cast<double>(lastStraw);
	    double deltaPhi     		=  phi - firstPhi;
            if (deltaPhi < -M_PI) deltaPhi 	+= 2.*M_PI;
            double straw			=  deltaPhi/pitch + 0.5;
	    double predWidth			=  m_deltaPhi*r +
						   m_deltaCurv*(r - rRoadVertex) *
						   std::abs(m_intersection->direction().y()*m_cosPhiStart -
							    m_intersection->direction().x()*m_sinPhiStart);
	    double widthStrawUnits		=  (predWidth + m_strawHalfWidth) / (r*pitch);
	    // double predWidth			=  100.*m_intersection->pathlength()*m_deltaPhi;
	    // double width			=  (predWidth + m_strawHalfWidth)/(r*pitch);
	    // double width      =  (m_deltaPhi*r +
	    // 			  m_deltaCurv*(r - rRoadVertex) *
	    // 			  std::abs(m_intersection->direction().y()*m_cosPhiStart -
	    // 				   m_intersection->direction().x()*m_sinPhiStart) +
	    // 			  m_strawHalfWidth) / (r*pitch);
	    // double width      =  ((m_deltaPhi + m_deltaCurv*
	    // 			   std::abs(m_intersection->direction().y()*m_cosPhiStart -
	    // 				    m_intersection->direction().x()*m_sinPhiStart)) *
	    // 			  (r - rRoadVertex)  + m_strawHalfWidth) / (r*pitch);
	    int strawMin			=  static_cast<int>(straw - widthStrawUnits + 0.4);
	    int strawMax			=  static_cast<int>(straw + widthStrawUnits - 0.5);
// 	    std::cout << std::setiosflags(std::ios::fixed)
// 		      << std::setw(9) << std::setprecision(1)
// 		      << " z " << (**p).z()
//  		      << std::setw(9) << std::setprecision(3)
// 		      << "  straw " << straw
// 		      << std::setw(6) << strawMin << std::setw(4) << strawMax<< std::endl;
	    
	    //            if (straw > 0. && straw < nStraws)
	    if (strawMin > lastStraw || strawMax < 0 || strawMax < strawMin)
	    {
                (**p).discard();
                continue;
	    }
	    else if (straw > 0. && straw < static_cast<double>(lastStraw+1))
	    {
		++m_intersectedLayers;
                if (!(**p).nearBoundary()) m_expectedStraws += m_strawWidth/(r*pitch);
	    }
	    else
	    {
		(**p).boundary();
	    }

	    (**p).setExpectedStraws(m_expectedStraws);
	    (**p).setPosition(phi,
			      r,
			      (**p).z(),
			      Amg::Vector3D(m_intersection->direction()));
	    double deltaPhiNarrow = pitch*widthStrawUnits;
	    (**p).setPhiRange(deltaPhiNarrow*m_signDeltaPhi*m_electronRoadFactor,
			      deltaPhiNarrow,
			      predWidth,
			      rRoadVertex);
	}
	(**p).layer(m_intersectedLayers);
    }
    //    std::cout << "   steps " << step << std::endl;

    // pop_back trailing discards
    while (m_predictions.begin() != m_predictions.end()
	   && (**m_predictions.rbegin()).isDiscard())
    {
	delete *m_predictions.rbegin();
	m_predictions.pop_back();
    }
}

void
TRT_Predictor::setRegion(const TrackParameters&	parameters)
{
    // check track penetrates some distance into TRT
    m_hashIds->clear();
    const Trk::TrackSurfaceIntersection* intersection = 0;

    // // find (eta, phi) at the TRT from the SurfaceIntersector
    if (parameters.cotTheta() < -m_cotThetaCorner)
    {
	intersection = m_rungeKuttaIntersector->intersectSurface(*m_outerBackwardsSurface,
								 m_intersection,
								 m_qOverP);
	if (! intersection)
	{
	    intersection = m_rungeKuttaIntersector->intersectSurface(*m_midBarrelSurface,
								     m_intersection,
								     m_qOverP);
	}
	if (! intersection)		return;
    }
    else if (parameters.cotTheta() < m_cotThetaCorner)
    {
	intersection = m_rungeKuttaIntersector->intersectSurface(*m_midBarrelSurface,
								 m_intersection,
								 m_qOverP);
	if (! intersection)		return;
    }
    else
    {
	intersection = m_rungeKuttaIntersector->intersectSurface(*m_outerForwardsSurface,
								 m_intersection,
								 m_qOverP);
	if (! intersection)
	{
	    intersection = m_rungeKuttaIntersector->intersectSurface(*m_midBarrelSurface,
								     m_intersection,
								     m_qOverP);
	}
	if (! intersection)		return;
    }

    double eta		=  intersection->position().eta();
    double deltaEta	=  m_etaRoadSafetyFactor + std::abs(parameters.position().eta() - eta);
    double etaMin	=  eta - deltaEta;
    double etaMax	=  eta + deltaEta;;
    double phi		=  intersection->position().phi();
    double deltaPhi	=  std::abs(parameters.position().phi() - phi);

    /// What is going on here? If you have a very wide RoI, it is made to be the 
    /// smaller compliment? This makes no sense. And you need to check that you still 
    /// have sensible phi limits !!!!
    /// This will end in disaster, what if the eventual phiHalfWidth > M_PI? Then 
    /// the starting and ending phi limits will be swapped round, and you will not 
    /// get the RoI you wanted. And if the half width == M_PI, then you will end up 
    /// with a 0 width RoI for all phi != 0; Need to check that the half width 
    /// is less than M_PI !!

    m_signDeltaPhi	=  -(parameters.position().phi() - phi)/deltaPhi;
    if (deltaPhi > M_PI)
    {
	deltaPhi	=  2.0*M_PI - deltaPhi;
	m_signDeltaPhi	*= -1.0;
    }
    deltaPhi		+= m_phiHalfWidth + 3.*m_deltaPhi;  // somewhat arbitrary but sufficient
 
    if ( !(deltaPhi<M_PI) ) deltaPhi = M_PI-1e-5;
    
    double phiMin	=  phi - deltaPhi;
    double phiMax	=  phi + deltaPhi;
    delete intersection;

    /// Check the phi limits !!!! 
    while ( phiMin<-M_PI ) phiMin += 2*M_PI;
    while ( phiMin> M_PI ) phiMin -= 2*M_PI;

    while ( phiMax<-M_PI ) phiMax += 2*M_PI;
    while ( phiMax> M_PI ) phiMax -= 2*M_PI;

    //    std::cout << "Road limits for this track" << std::endl;
    //    std::cout << "phiMin, phiMax = " 
    //    	      << phiMin << ", " << phiMax << std::endl;
    //    std::cout << "etaMin, etaMax = " 
    //    	      << etaMin << ", " << etaMax << std::endl;


    
    // get the detector element hashIDs from the RegionSelector.
    RoiDescriptor _roi( etaMin, etaMax, phiMin, phiMax );

    m_regionSelector->DetHashIDList(TRT, _roi, *m_hashIds);
}



