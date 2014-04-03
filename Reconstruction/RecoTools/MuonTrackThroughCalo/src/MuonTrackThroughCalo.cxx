/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTrackThroughCalo.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuonTrackThroughCalo.h"

#include "GeoPrimitives/GeoPrimitives.h"

// Trk include
#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include <math.h>

// using namespace Calo;
using namespace Rec;
using namespace Trk;

// constructor
MuonTrackThroughCalo::MuonTrackThroughCalo(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_staticExtrapolator("Trk::Extrapolator/StaticExtrapolator"),
  m_dynamicParamExtrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_dynamicCombiExtrapolator("Trk::Extrapolator/MuonCaloDynamicCombiExtrapolator")

{
   declareInterface<IMuonTrackThroughCalo>(this);

   declareProperty("StaticExtrapolator", m_staticExtrapolator);
   declareProperty("ParamExtrapolator", m_dynamicParamExtrapolator);
   declareProperty("CombiExtrapolator", m_dynamicCombiExtrapolator);

}

// destructor
MuonTrackThroughCalo::~MuonTrackThroughCalo()
{

}

// Athena standard methods
// initialize
StatusCode MuonTrackThroughCalo::initialize()
{
  StatusCode sc = AthAlgTool::initialize();

  if(m_staticExtrapolator.retrieve().isFailure()){
    ATH_MSG_ERROR ("Failed to retrieve tool " << m_staticExtrapolator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ("Retrieved tool " << m_staticExtrapolator);
  }

  if (m_dynamicParamExtrapolator.retrieve().isFailure()){
    ATH_MSG_ERROR ("Failed to retrieve tool " << m_dynamicParamExtrapolator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ("Retrieved tool " << m_dynamicParamExtrapolator);
  }

  if (m_dynamicCombiExtrapolator.retrieve().isFailure()){
    ATH_MSG_ERROR ("Failed to retrieve tool " << m_dynamicCombiExtrapolator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ("Retrieved tool " << m_dynamicCombiExtrapolator);
  }

  ATH_MSG_DEBUG ("Initialize() successful in "<<name());
  return sc;
}

// finalize
StatusCode MuonTrackThroughCalo::finalize()
{
  ATH_MSG_DEBUG ("finalize() successful in " << name());
  return StatusCode::SUCCESS;
}


const Trk::TrackParameters* MuonTrackThroughCalo::extrapolateToPerigee (const Trk::TrackParameters& parms,
									bool useMeasurement,
									Trk::PropDirection propagationDirection)
{

  //create appropriate surface (hardcoded surfaces for now)
  Trk::PerigeeSurface* sf = new Trk::PerigeeSurface(Amg::Vector3D(0,0,0));
  std::vector<Trk::Surface*> surfaces;
  surfaces.push_back(sf);
  const Trk::TrackParameters* result = 0;
  result = extrapolateToSurfaces (parms,
				  useMeasurement,
				  surfaces,
				  propagationDirection); 

  surfaces.clear();
  delete sf;
  return result;
}

const Trk::TrackParameters* MuonTrackThroughCalo::extrapolateToMS (const Trk::TrackParameters& parms,
								   bool useMeasurement)
{

  //create appropriate concentric surfaces (hardcoded surfaces for now)
  Trk::CylinderSurface* cyl = new Trk::CylinderSurface(4250,6500);

  Trk::DiscSurface* dsk = 0;
  
  if(parms.momentum().z() > 0)
    dsk    = new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,6500))), 0, 4250);
  else dsk = new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-6500))), 0, 4250);

  //get a first guess of where it is going
  bool hittingCyl = (42.5/65 > fabs(parms.momentum().perp()/parms.momentum().z())) ? false : true;

  std::vector<Trk::Surface*> surfaces;
  if(hittingCyl){

    surfaces.push_back(cyl);
    surfaces.push_back(dsk);

  }else{

    surfaces.push_back(dsk);
    surfaces.push_back(cyl);

  }

  const Trk::TrackParameters* result = 0;
  result = extrapolateToSurfaces (parms,
				  useMeasurement,
				  surfaces,
				  Trk::alongMomentum); //could be a problem for cosmics

  surfaces.clear();
  delete cyl;
  delete dsk;
  return result;
}

const Trk::TrackParameters* MuonTrackThroughCalo::forwardExtrapolateToID (const Trk::TrackParameters& parms)
{

  return extrapolateToID(parms, Trk::alongMomentum);

}

const Trk::TrackParameters* MuonTrackThroughCalo::backwardExtrapolateToID (const Trk::TrackParameters& parms,
									   bool useMeasurement)
{

  return extrapolateToID(parms, Trk::oppositeMomentum, useMeasurement);

}

const Trk::TrackParameters* MuonTrackThroughCalo::extrapolateToID (const Trk::TrackParameters& parms,
								   Trk::PropDirection prop,
								   bool useMeasurement)
{

  //create appropriate concentric surface (hardcoded surfaces for now)
  Trk::CylinderSurface* cyl = new Trk::CylinderSurface(1050,3200);

  Trk::DiscSurface* dsk = 0;
  
  if(parms.momentum().z() > 0) 
    dsk    = new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,3200))), 0, 1050);
  else dsk = new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-3200))), 0, 1050);

  //get a first guess of where it is going
  bool hittingCyl = (10.5/32. > fabs(parms.momentum().perp()/parms.momentum().z())) ? false : true;

  std::vector<Trk::Surface*> surfaces;
  if(hittingCyl){

    surfaces.push_back(cyl);
    surfaces.push_back(dsk);

  }else{

    surfaces.push_back(dsk);
    surfaces.push_back(cyl);

  }

  const Trk::TrackParameters* result = 0;
  result = extrapolateToSurfaces (parms,
				  useMeasurement,
				  surfaces,
				  prop, 
				  (prop == Trk::alongMomentum),
				  (prop == Trk::oppositeMomentum));
  surfaces.clear();
  delete cyl;
  delete dsk;
  return result;

}


const Trk::TrackParameters* MuonTrackThroughCalo::extrapolateToSurfaces (const Trk::TrackParameters& parms,
									 bool useMeasurement,
									 std::vector<Trk::Surface*> surfaces,
									 Trk::PropDirection prop,
									 bool /*id*/,
									 bool checkHemisphereChange)
{

  const Trk::TrackParameters* result = 0;

  std::vector<Trk::Surface*>::iterator sf, sfEnd = surfaces.end();

  if(useMeasurement){
    for(sf = surfaces.begin(); sf != sfEnd; sf++){
      //try with all provided surfaces until we get a good extrapolation
      result = m_dynamicCombiExtrapolator->extrapolate(parms,
						       **sf,
						       prop, 
						       true,
						       Trk::muon);
      if(checkHemisphereChange && result != 0){
	double dotInit = parms.momentum().x()*parms.position().x()+parms.momentum().y()*parms.position().y();
	double dotFin = result->momentum().x()*result->position().x()+result->momentum().y()*result->position().y();
	if(dotInit*dotFin < 0){
	  delete result;
	  result = 0;
	}
      }
      if(result != 0) break;
    }
  }
  
  if(result == 0){
      //Try with parameterized
    for(sf = surfaces.begin(); sf != sfEnd; sf++){
      //std::cout << "calling extrapolate" << std::endl;
      result = m_dynamicParamExtrapolator->extrapolate(parms,
						       **sf,
						       prop, 
						       true,
						       Trk::muon);
      if(checkHemisphereChange && result != 0){
	double dotInit = parms.momentum().x()*parms.position().x()+parms.momentum().y()*parms.position().y();
	double dotFin = result->momentum().x()*result->position().x()+result->momentum().y()*result->position().y();
	if(dotInit*dotFin < 0){
	  delete result;
	  result = 0;
	}
      }
      if(result != 0) break;
    }
  }
  if(result == 0){
    //failed extrapolation. Try with static
    for(sf = surfaces.begin(); sf != sfEnd; sf++){
      result = m_staticExtrapolator->extrapolate(parms,
						 **sf,
						 prop,
						 true,
						 Trk::muon);
      if(checkHemisphereChange && result != 0){
	double dotInit = parms.momentum().x()*parms.position().x()+parms.momentum().y()*parms.position().y();
	double dotFin = result->momentum().x()*result->position().x()+result->momentum().y()*result->position().y();
	if(dotInit*dotFin < 0){
	  delete result;
	  result = 0;
	}
	//does not necessarily work for cosmics
      }
      if(result != 0) break;
    }    
  }
  return result;
}
