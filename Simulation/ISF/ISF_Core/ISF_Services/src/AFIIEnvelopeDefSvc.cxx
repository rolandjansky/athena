/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AFIIEnvelopeDefSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "AFIIEnvelopeDefSvc.h"

// AtlasDetDescr
#include "AtlasDetDescr/AtlasRegionHelper.h"

// STL
#include <limits>

/** Constructor */
ISF::AFIIEnvelopeDefSvc::AFIIEnvelopeDefSvc(const std::string& name, ISvcLocator* svc) :
  base_class(name,svc),
  m_isfEnvDefSvc("ISF_ISFEnvelopeDefSvc", name),
  m_tolerance(1e-4),
  m_idMaxExtentZ(3550.),
  m_rzBeamPipe(),
  m_rzInDet(),
  m_rposzBeamPipe(),
  m_rposzInDet()
{
  declareProperty( "ISFEnvelopeDefSvc",
                   m_isfEnvDefSvc,
                   "The ISF EnvelopeDefinition Service");

  declareProperty( "Tolerance",
                   m_tolerance,
                   "Tolerance on coordinates.");

  declareProperty( "InDetMaxExtentZ",
                   m_idMaxExtentZ,
                   "The desired InnerDetector maximum extension in |z|");
}


ISF::AFIIEnvelopeDefSvc::~AFIIEnvelopeDefSvc()
{
  // free memory
  // TODO :)
}


/** Athena AthService initialize hook */
StatusCode ISF::AFIIEnvelopeDefSvc::initialize()
{
  ATH_MSG_INFO("Initializing ...");

  // retrieve ATLAS common envelope definition service
  if ( m_isfEnvDefSvc.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve EnvelopeDefinition service ('" << m_isfEnvDefSvc.typeAndName() << "'). Abort.");
    return StatusCode::FAILURE;
  }

  //
  // identify highest z extent in the ID volume
  //
  m_rposzInDet = m_isfEnvDefSvc->getRPositiveZBoundary( AtlasDetDescr::fAtlasID      );
  double zMax    = std::numeric_limits<double>::min();
  {
    // find the greatest z value of the ID volume
    for ( unsigned int curPos=0; curPos<m_rposzInDet.size(); curPos++ ) {
      double curZ = m_rposzInDet[curPos].second;

      if (curZ > zMax) { 
        zMax    = curZ;
      }
    }
  }

  //
  // shift ID/BeamPipe/Calo boundaries according to the desired new ID extent
  //
  m_rposzInDet    = getShiftedBoundary( AtlasDetDescr::fAtlasID     , zMax, m_idMaxExtentZ);
  m_rposzBeamPipe = getShiftedBoundary( AtlasDetDescr::fAtlasForward, zMax, m_idMaxExtentZ);
  m_rposzCalo     = getShiftedBoundary( AtlasDetDescr::fAtlasCalo   , zMax, m_idMaxExtentZ);

  // mirror the RZPairs provided in m_rposz to describe all corner points
  // in (r,z) space for the BeamPipe/Forward and the InnerDetector envelopes
  mirrorRZ( m_rposzBeamPipe, m_rzBeamPipe );
  mirrorRZ( m_rposzInDet   , m_rzInDet    );
  mirrorRZ( m_rposzCalo    , m_rzCalo     );


  // debugging output: print volume definitions
  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE( "Envelope: complete region=" << AtlasDetDescr::fAtlasForward);
    for ( unsigned int num = 0; num<m_rzBeamPipe.size(); num++) {
      ATH_MSG_VERBOSE("   pos=" << num << " r=" << m_rzBeamPipe[num].first << " z="<< m_rzBeamPipe[num].second);
    }
    ATH_MSG_VERBOSE( "Envelope: complete region=" << AtlasDetDescr::fAtlasID);
    for ( unsigned int num = 0; num<m_rzInDet.size(); num++) {
      ATH_MSG_VERBOSE("   pos=" << num << " r=" << m_rzInDet[num].first << " z="<< m_rzInDet[num].second);
    }
    ATH_MSG_VERBOSE( "Envelope: complete region=" << AtlasDetDescr::fAtlasCalo);
    for ( unsigned int num = 0; num<m_rzCalo.size(); num++) {
      ATH_MSG_VERBOSE("   pos=" << num << " r=" << m_rzCalo[num].first << " z="<< m_rzCalo[num].second);
    }
  }

  ATH_MSG_INFO("Initialize successful.");

  return StatusCode::SUCCESS;
}


/** Athena AthService finalize hook */
StatusCode ISF::AFIIEnvelopeDefSvc::finalize()
{
  return StatusCode::SUCCESS;
}


/** return a vector of (r,z) pairs, defining the respective envelope */
const RZPairVector &ISF::AFIIEnvelopeDefSvc::getRZBoundary( AtlasDetDescr::AtlasRegion region ) const {

  // treat Forward/BeamPipe and InnerDetector regions separately
  if      ( region == AtlasDetDescr::fAtlasForward ) return m_rzBeamPipe;
  else if ( region == AtlasDetDescr::fAtlasID      ) return m_rzInDet;
  else if ( region == AtlasDetDescr::fAtlasCalo    ) return m_rzCalo;
  else                                               return m_isfEnvDefSvc->getRZBoundary( region );

}


/** return a vector of (r,z) pairs, defining the envelope on the z>0 region */
const RZPairVector &ISF::AFIIEnvelopeDefSvc::getRPositiveZBoundary( AtlasDetDescr::AtlasRegion region ) const {

  // treat Forward/BeamPipe and InnerDetector regions separately
  if      ( region == AtlasDetDescr::fAtlasForward ) return m_rposzBeamPipe;
  else if ( region == AtlasDetDescr::fAtlasID      ) return m_rposzInDet;
  else if ( region == AtlasDetDescr::fAtlasCalo    ) return m_rposzCalo;
  else                                               return m_isfEnvDefSvc->getRPositiveZBoundary( region );
}


/** return boundary with shifted z values */
RZPairVector ISF::AFIIEnvelopeDefSvc::getShiftedBoundary( AtlasDetDescr::AtlasRegion region,
                                                           double shiftFromZ,
                                                           double shiftToZ ) const {

  const char *volName = AtlasDetDescr::AtlasRegionHelper::getName(region);
  ATH_MSG_DEBUG("Will shift '" << volName << "' boundary at |z|="<<shiftFromZ<<" to |z|="<<shiftToZ<<"");

  RZPairVector rposzPairs = m_isfEnvDefSvc->getRPositiveZBoundary(region);

  for ( unsigned int curPos=0; curPos<rposzPairs.size(); curPos++ ) {
    double curR = rposzPairs[curPos].first;
    double curZ = rposzPairs[curPos].second;

    // is current boundary point to be shifted?
    if ( fabs(curZ-shiftFromZ) < m_tolerance ) {

      ATH_MSG_VERBOSE("Found boundary point in '" << volName <<"' volume." <<
                      " Shifting (r,|z|)=("<<curR<<","<<curZ<<") to (r,|z|)=("<<curR<<","<<shiftToZ<<")");

      rposzPairs[curPos].second = shiftToZ;
    }
  }

  return rposzPairs;
}
