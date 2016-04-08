/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * TrackIsolationDecorAlg.cxx
 *
 *  Created on: Sep 18, 2014
 *      Author: goblirsc
 */


#include "TrackIsolationDecorAlg.h"
#include "GaudiKernel/Property.h"
#include "xAODMuon/MuonContainer.h"
#include "GaudiKernel/ITHistSvc.h"

using std::string;

//**********************************************************************

TrackIsolationDecorAlg::TrackIsolationDecorAlg(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name, pSvcLocator ),
  m_histSvc("THistSvc", name)
# if !defined(XAOD_ANALYSIS)
  ,m_track_iso_tool(),
  m_calo_iso_tool()
# endif
{
# if !defined(XAOD_ANALYSIS)
  declareProperty("TrackIsolationTool",        m_track_iso_tool);
  declareProperty("CaloIsolationTool",        m_calo_iso_tool);
# endif
}

//**********************************************************************

TrackIsolationDecorAlg::~TrackIsolationDecorAlg() { }

//**********************************************************************

StatusCode TrackIsolationDecorAlg::initialize() {

  ATH_CHECK(m_histSvc.retrieve());
# if !defined(XAOD_ANALYSIS)
  ATH_CHECK(m_track_iso_tool.retrieve());
  ATH_CHECK(m_calo_iso_tool.retrieve());
# endif

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrackIsolationDecorAlg::finalize() {

	return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrackIsolationDecorAlg::execute() {

  return StatusCode::SUCCESS;
}

//**********************************************************************
