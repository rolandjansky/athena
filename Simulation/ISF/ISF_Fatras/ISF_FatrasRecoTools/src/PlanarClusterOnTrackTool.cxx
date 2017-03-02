/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class PlanarClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for PlanarClusterOnTrack object production
///////////////////////////////////////////////////////////////////

#include "ISF_FatrasRecoTools/PlanarClusterOnTrackTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"
#include <sstream>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

namespace iFatras {
  PlanarClusterOnTrackTool::PlanarClusterOnTrackTool
  (const std::string& t,const std::string& n,const IInterface* p) :
    ::AthAlgTool(t,n,p),
    m_useDefinedParameters(false)
  {
    declareInterface<IRIO_OnTrackCreator>(this);
    declareProperty("UseDefinedParameters",     m_useDefinedParameters);
  }
  

  ///////////////////////////////////////////////////////////////////
  // Destructor  
  ///////////////////////////////////////////////////////////////////
  
  PlanarClusterOnTrackTool::~PlanarClusterOnTrackTool(){}

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  
  StatusCode PlanarClusterOnTrackTool::initialize()
  {
    StatusCode sc = AthAlgTool::initialize(); 
    
    msg(MSG::INFO)  << name() << " initialize()" << endmsg;   
    
    return sc;
  }
  
  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////
  
  StatusCode PlanarClusterOnTrackTool::finalize()
  {
    StatusCode sc = AlgTool::finalize(); 
    return sc;
  }
  
  ///////////////////////////////////////////////////////////////////
  // iFatras::PlanarClusterOnTrack  production
  ///////////////////////////////////////////////////////////////////
  
  const PlanarClusterOnTrack* PlanarClusterOnTrackTool::correct
  (const Trk::PrepRawData& rio,const Trk::TrackParameters&) const
  {

    ATH_MSG_VERBOSE ("PlanarClusterOnTrackTool:: correct()");
  
    const PlanarCluster* plc=dynamic_cast<const PlanarCluster*>(&rio);
  
    if(plc==0) {
      ATH_MSG_WARNING("This is not a planar cluster, return 0.");
      return 0;
    }
    
    // Get pointer to detector element
    //
    const iFatras::PlanarDetElement* element = plc->detectorElement(); 
    if(!element) {
      ATH_MSG_WARNING ("Cannot access detector element. Aborting cluster correction...");
      return 0;
    }

    bool isPixel = element->isPixel();
    bool isDisc = (element->surface().type() == Trk::Surface::Disc);

    IdentifierHash iH = element->identifyHash();
  
    // PlanarCluster global and local position
    //
    Amg::Vector3D   glob(plc->globalPosition());
    Amg::Vector2D locpos = plc->localPosition();    
    Trk::LocalParameters locpar;

    // Covariance matrix
    //
    Amg::MatrixX oldLocalCov = plc->localCovariance();
    Amg::MatrixX* loce = 0;

    if(!isPixel) { // SCT case
      // TODO! At the time being the KalmanFitter doesn't understand yet the DefinedParameters
      // The code is already here when this will be the case
      if ( m_useDefinedParameters ) {
	if (isDisc) { // Disc surface with trapezoidal shape
	  Trk::DefinedParameter par(locpos[Trk::locPhi], Trk::locPhi);
	  locpar = Trk::LocalParameters(par);
	  Amg::MatrixX mat(1,1);
	  mat(0,0) = oldLocalCov(Trk::locPhi, Trk::locPhi);
	  loce = new Amg::MatrixX(mat);
	} else { // Planar surface with trapezoidal shape
	  Trk::DefinedParameter par(locpos[Trk::locX], Trk::locX);
	  locpar = Trk::LocalParameters(par);
	  Amg::MatrixX mat(1,1);
	  mat(0,0) = oldLocalCov(Trk::locX, Trk::locX);
	  loce = new Amg::MatrixX(mat);
	}
      } else {
	locpar = Trk::LocalParameters(locpos);
	loce = new Amg::MatrixX(oldLocalCov);
      }
    } else { // Pixel case
      locpar = Trk::LocalParameters(locpos);
      loce = new Amg::MatrixX(oldLocalCov);
    }
      
    ATH_MSG_DEBUG ("PlanarCluster: Global Position --> "  << glob);  
    ATH_MSG_DEBUG ("PlanarCluster: Local Position --> "   << locpos);  
    ATH_MSG_DEBUG ("PlanarCluster: Local Parameters --> " << locpar);  
    ATH_MSG_DEBUG ("PlanarCluster: Error Matrix --> "     << *loce);  

    iFatras::PlanarClusterOnTrack* pcot = new iFatras::PlanarClusterOnTrack(plc,locpar,*loce,iH,glob);

    std::ostringstream ss;
    pcot->dump(ss);
    ATH_MSG_DEBUG ("Planar: Planar Cluster On Track "     << ss.str());
    
    return pcot;
  }

void PlanarClusterOnTrackTool::handle(const Incident& inc) 
{
  if ( inc.type() == "EndEvent" ){
    ATH_MSG_VERBOSE("'EndEvent' incident caught.");
  }
}

}
