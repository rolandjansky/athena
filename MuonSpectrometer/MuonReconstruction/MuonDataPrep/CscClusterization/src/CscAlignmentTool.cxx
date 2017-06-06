/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscAlignmentTool.cxx

#include "CscAlignmentTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/CscIdHelper.h"

using std::vector;
using std::string;

//**********************************************************************

CscAlignmentTool::
CscAlignmentTool(string type, string aname, const IInterface* parent)
  : AthAlgTool(type, aname, parent),
    m_pmuon_detmgr(0), m_phelper(0)
{
  declareInterface<ICscAlignmentTool>(this);
  declareProperty("useAlignment", m_useAlignment=false); // in strips
  declareProperty("etaposAlignConsts", m_etaposAlignconsts);
  declareProperty("phiposAlignConsts", m_phiposAlignconsts);

}

//**********************************************************************

CscAlignmentTool::~CscAlignmentTool() { }

//**********************************************************************

StatusCode CscAlignmentTool::initialize() {

  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_MSG_DEBUG ( "  useAlignment is " << m_useAlignment );
  ATH_MSG_DEBUG ( "     EtaPos AlignConsts size :" << m_etaposAlignconsts.size() );
  ATH_MSG_DEBUG ( "     PhiPos AlignConsts size :" << m_phiposAlignconsts.size() );

  // Retrieve the detector descriptor.
  if ( detStore()->retrieve(m_pmuon_detmgr).isFailure() ) {
    ATH_MSG_ERROR ( " Cannot retrieve MuonGeoModel " );
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG ( "Retrieved geometry." );
  m_phelper = m_pmuon_detmgr->cscIdHelper();


  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode CscAlignmentTool::finalize() {
  ATH_MSG_VERBOSE ( "Finalizing " << name() );
  return StatusCode::SUCCESS;
}

//**********************************************************************
double CscAlignmentTool::getAlignmentOffset(Identifier pstripId) const {

  if (! m_useAlignment)
    return 0.0;
  
  int zsec    = m_phelper->stationEta(pstripId);
  int station = m_phelper->stationName(pstripId) - 49;    // 1=CSS, 2=CSL
  int phisec  = m_phelper->stationPhi(pstripId);

  int sector  = zsec*(2*phisec - station + 1); // -16 ~ 16
  int wlay    = m_phelper->wireLayer(pstripId); // 1 ~ 4
  int measphi = m_phelper->measuresPhi(pstripId); // 0 for eta and 1 for phi

  const std::vector<double>* etaAlignTable = &m_etaposAlignconsts;
  const std::vector<double>* phiAlignTable = &m_phiposAlignconsts;
  
  int isec = (sector<0) ? sector+16 : sector+15; // convert -16 ~ 16 to 0 ~ 31...
  int lookupId = isec*4 + wlay-1;  // no info for 1st layer... so subtract 2....

  double offset = (measphi) ? (*phiAlignTable)[lookupId] : (*etaAlignTable)[lookupId];

  return offset;
}



