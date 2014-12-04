/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Muon_ROIFillerTool.cxx 620244 2014-10-06 19:02:48Z ssnyder $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "Muon_ROIFillerTool.h"


namespace D3PD {


Muon_ROIFillerTool::Muon_ROIFillerTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : BlockFillerTool< xAOD::MuonRoI >( type, name, parent )
{
  
  book().ignore(); // Avoid coverity warnings
}


StatusCode Muon_ROIFillerTool::book()
{
  CHECK( addVariable( "thrNumber", m_thrNumber,
                      "Number of the highest threshold this RoI passed" ) );
  CHECK( addVariable( "RoINumber", m_roi,
                      "\"RoI\" (pad) number of the muon candidate" ) );
  CHECK( addVariable( "sectorAddress", m_sectorAddress,
                      "The encoded \"full\" sector address" ) );
  CHECK( addVariable( "firstCandidate", m_firstCandidate,
                      "True if the candidate had the highest pT in its sector" ) );
  CHECK( addVariable( "moreCandInRoI", m_moreCandInRoI,
                      "True if there were other muons detected in the same pad" ) );
  CHECK( addVariable( "moreCandInSector", m_moreCandInSector,
                      "True if there were >2 muons detected in the same sector" ) );
  CHECK( addVariable( "source", m_source,
                      "Muon candidate system ID (Barrel=0, Endcap=1, Forward=2)" ) );
  CHECK( addVariable( "hemisphere", m_hemisphere,
                      "Hemisphere of the muon candidate (Negative=0, Positive=1)" ) );
  CHECK( addVariable( "charge", m_charge,
                      "Charge of the muon candidate (Negative=0, Positive=1,"
                      " Undefined=100)" ) );
  CHECK( addVariable( "vetoed", m_vetoed,
                      "True if the candidate was vetoed in the multiplicity sum" ) );

  return StatusCode::SUCCESS;
}


StatusCode Muon_ROIFillerTool::fill( const xAOD::MuonRoI& roi )
{
  *m_thrNumber = roi.getThrNumber();
  *m_roi = roi.getRoI();
  *m_sectorAddress = roi.getSectorAddress();
  *m_firstCandidate = roi.isFirstCandidate();
  *m_moreCandInRoI = roi.isMoreCandInRoI();
  *m_moreCandInSector = roi.isMoreCandInSector();
  switch( roi.getSource() ) {
  case xAOD::MuonRoI::Barrel:
    *m_source = 0;
    break;
  case xAOD::MuonRoI::Endcap:
    *m_source = 1;
    break;
  case xAOD::MuonRoI::Forward:
    *m_source = 2;
    break;
  default:
    *m_source = -1;
    break;
  }
  switch( roi.getHemisphere() ) {
  case xAOD::MuonRoI::Negative:
    *m_hemisphere = 0;
    break;
  case xAOD::MuonRoI::Positive:
    *m_hemisphere = 1;
    break;
  default:
    *m_hemisphere = -1;
    break;
  }
  switch( roi.getCharge() ) {
  case xAOD::MuonRoI::Neg:
    *m_charge = 0;
    break;
  case xAOD::MuonRoI::Pos:
    *m_charge = 1;
    break;
  case xAOD::MuonRoI::Undef:
    *m_charge = 100;
    break;
  default:
    *m_charge = -1;
    break;
  }
  *m_vetoed = roi.isVetoed();
  return StatusCode::SUCCESS;
}


} // namespace D3PD
