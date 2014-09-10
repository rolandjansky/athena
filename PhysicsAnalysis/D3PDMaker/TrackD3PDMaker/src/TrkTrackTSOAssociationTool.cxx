/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackTSOAssociationTool.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "AthenaKernel/errorcheck.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"


namespace D3PD {

TrkTrackTSOAssociationTool::TrkTrackTSOAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      TSOSAssociationHelper()
{
//   declareProperty("InDetHoleSearchTool", m_idHoleSearch);
  for(std::map<std::string, bool>::iterator itr=m_getTypes.begin(); 
      itr!=m_getTypes.end(); itr++){
    declareProperty(itr->first, itr->second);
  }
}

StatusCode TrkTrackTSOAssociationTool::initialize(){

  CHECK( Base::initialize() );

  //   if ( !m_idHoleSearch.empty() ) {
  //     if (m_idHoleSearch.retrieve().isFailure() ) {
  //       REPORT_MESSAGE (MSG::ERROR) << "Failed to retrieve InDet hole search tool " << m_idHoleSearch;
  //       return StatusCode::FAILURE;
  //     } else {
  //       REPORT_MESSAGE (MSG::INFO) << "Retrieved tool " << m_idHoleSearch;
  //       m_doHolesInDet = true;
  //     }
  //   }

  // need Atlas id-helper to identify sub-detectors, take it from detStore
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Could not get AtlasID helper";
    return StatusCode::FAILURE;
  }   

  // need Pixel helper to identify b-layer TSOS, take it from detStore
  if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Could not get PixelID helper";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/**
 * @brief Start the iteration for a new association.
 * @param p The object from which to associate.
 */
StatusCode TrkTrackTSOAssociationTool::reset (const Trk::Track& p)
{

  return doReset(p);
}

/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Trk::TrackStateOnSurface* TrkTrackTSOAssociationTool::next()
{

  return getNext();
}

} // namespace D3PD
