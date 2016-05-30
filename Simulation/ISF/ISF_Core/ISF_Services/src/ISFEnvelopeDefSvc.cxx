/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFEnvelopeDefSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ISFEnvelopeDefSvc.h"

// STL
#include <limits>

/** Constructor */
ISF::ISFEnvelopeDefSvc::ISFEnvelopeDefSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name,svc),
  m_atlasEnvDefSvc("AtlasGeometry_EnvelopeDefSvc", name),
  m_rzBeamPipe(),
  m_rzInDet(),
  m_rposzBeamPipe(),
  m_rposzInDet()
{
  declareProperty("ATLASEnvelopeDefSvc"          , m_atlasEnvDefSvc      );
}


ISF::ISFEnvelopeDefSvc::~ISFEnvelopeDefSvc()
{
  // free memory
  // TODO :)
}


/** Athena AthService initialize hook */
StatusCode ISF::ISFEnvelopeDefSvc::initialize()
{
  ATH_MSG_INFO("Initializing ...");

  // retrieve ATLAS common envelope definition service
  if ( m_atlasEnvDefSvc.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve EnvelopeDefinition service ('" << m_atlasEnvDefSvc.typeAndName() << "'). Abort.");
    return StatusCode::FAILURE;
  }

  // change InnerDetector boundaries according to ISF conventions
  //   ie. remove the beampipe from inside the ID volume and move the
  //       ID volume all the way down to r==0.
  //
  m_rposzInDet = m_atlasEnvDefSvc->getRPositiveZBoundary( AtlasDetDescr::fAtlasID      );
  // will store the lowest radius of the ID volume
  double rMin    = std::numeric_limits<double>::max();
  double zAtRMin = 0.;
  {
    unsigned int    rMinPos = 0;

    // find the  smallest radius of the ID volume
    for ( unsigned int curPos=0; curPos<m_rposzInDet.size(); curPos++ ) {
      double curR = m_rposzInDet[curPos].first;
      double curZ = m_rposzInDet[curPos].second;

      if (curR < rMin) {
        rMin    = curR;
        zAtRMin = curZ;
        rMinPos = curPos;
      }
    }

    // set the lowest radius of the ID volume to ZERO
    ATH_MSG_DEBUG("Will shift ID boundary (r,|z|)=("<<rMin<<","<<zAtRMin<<") to (r,|z|)=("<<0.<<","<<zAtRMin<<")");
    m_rposzInDet[rMinPos].first = 0.;
  }

  // change BeamPipe/Forward boundaries according to ISF conventions
  //
  bool foundShiftPoint = false;
  m_rposzBeamPipe = m_atlasEnvDefSvc->getRPositiveZBoundary( AtlasDetDescr::fAtlasForward );
  {
    for ( unsigned int curPos=0; curPos<m_rposzBeamPipe.size(); curPos++ ) {
      double curR = m_rposzBeamPipe[curPos].first;
      double curZ = m_rposzBeamPipe[curPos].second;

      // if the current radius and z-coordinate are the same as the identified lowest ID radius
      // set this radius to ZERO too
      if ( (fabs(curR-rMin) < 1e-4) && (fabs(curZ-zAtRMin)<1e-4)  ) {
        ATH_MSG_VERBOSE("Found boundary point in BeamPipe volume. Shifting (r,|z|)=("<<curR<<","<<curZ<<") to (r,|z|)=("<<0.<<","<<curZ<<")");
        foundShiftPoint = true;
        m_rposzBeamPipe[curPos].first = 0.;
      }
    }
  }

  // if we have shifted an (r,z) point in the ID envelope, but not in the BeamPipe envelope
  //  -> something is wrong, better throw an error!
  if ( !foundShiftPoint) {
    ATH_MSG_ERROR("Could not find InDet envelope point (r,z)=("<<rMin<<","<<zAtRMin<<") in the BeamPipe/Forward envelope -> Unable to shift it down to r=0.");
    return StatusCode::FAILURE;
  }

  // mirror the RZPairs provided in m_rposz to describe all corner points
  // in (r,z) space for the BeamPipe/Forward and the InnerDetector envelopes
  mirrorRZ( m_rposzBeamPipe, m_rzBeamPipe );
  mirrorRZ( m_rposzInDet   , m_rzInDet    );

  // debugging output:
  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE( "Envelope: complete region=" << AtlasDetDescr::fAtlasForward);
    for ( unsigned int num = 0; num<m_rzBeamPipe.size(); num++) {
      ATH_MSG_VERBOSE("   pos=" << num << " r=" << m_rzBeamPipe[num].first << " z="<< m_rzBeamPipe[num].second);
    }
    ATH_MSG_VERBOSE( "Envelope: complete region=" << AtlasDetDescr::fAtlasID);
    for ( unsigned int num = 0; num<m_rzInDet.size(); num++) {
      ATH_MSG_VERBOSE("   pos=" << num << " r=" << m_rzInDet[num].first << " z="<< m_rzInDet[num].second);
    }
  }

  ATH_MSG_INFO("Initialize successful.");

  return StatusCode::SUCCESS;
}


/** Athena AthService finalize hook */
StatusCode ISF::ISFEnvelopeDefSvc::finalize()
{
  return StatusCode::SUCCESS;
}


/** return a vector of (r,z) pairs, defining the respective envelope */
const RZPairVector &ISF::ISFEnvelopeDefSvc::getRZBoundary( AtlasDetDescr::AtlasRegion region ) const {

  // treat Forward/BeamPipe and InnerDetector regions separately
  if      ( region == AtlasDetDescr::fAtlasForward ) return m_rzBeamPipe;
  else if ( region == AtlasDetDescr::fAtlasID )      return m_rzInDet;
  else                                               return m_atlasEnvDefSvc->getRZBoundary( region );

}


/** return a vector of (r,z) pairs, defining the envelope on the z>0 region */
const RZPairVector &ISF::ISFEnvelopeDefSvc::getRPositiveZBoundary( AtlasDetDescr::AtlasRegion region ) const {

  // treat Forward/BeamPipe and InnerDetector regions separately
  if      ( region == AtlasDetDescr::fAtlasForward ) return m_rposzBeamPipe;
  else if ( region == AtlasDetDescr::fAtlasID )      return m_rposzInDet;
  else                                               return m_atlasEnvDefSvc->getRPositiveZBoundary( region );
}


/** Query the interfaces. */
StatusCode ISF::ISFEnvelopeDefSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {

  if ( IID_IEnvelopeDefSvc == riid )
    *ppvInterface = (IEnvelopeDefSvc*)this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}
