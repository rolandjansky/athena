/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CscAlignmentTool.cxx

#include "CscAlignmentTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/CscIdHelper.h"

//**********************************************************************

CscAlignmentTool::CscAlignmentTool(const std::string& type, const std::string& aname, const IInterface* parent) :
  AthAlgTool(type, aname, parent),
  m_phelper(nullptr)
{
  declareInterface<ICscAlignmentTool>(this);
  declareProperty("useAlignment", m_useAlignment=false); // in strips
  declareProperty("etaposAlignConsts", m_etaposAlignconsts);
  declareProperty("phiposAlignConsts", m_phiposAlignconsts);

}

//**********************************************************************

StatusCode CscAlignmentTool::initialize() {

  ATH_MSG_DEBUG ( "Initializing " << name() );
  ATH_MSG_DEBUG ( "  useAlignment is " << m_useAlignment );
  ATH_MSG_DEBUG ( "     EtaPos AlignConsts size :" << m_etaposAlignconsts.size() );
  ATH_MSG_DEBUG ( "     PhiPos AlignConsts size :" << m_phiposAlignconsts.size() );

  const MuonGM::MuonDetectorManager* muDetMgr=nullptr;
  ATH_CHECK_RECOVERABLE(detStore()->retrieve(muDetMgr));
  ATH_MSG_DEBUG ( "Retrieved geometry." );
  m_phelper = muDetMgr->cscIdHelper();

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



