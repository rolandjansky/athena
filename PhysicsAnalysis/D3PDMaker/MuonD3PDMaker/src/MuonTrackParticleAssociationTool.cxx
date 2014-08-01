/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonD3PDMaker/src/MuonTrackParticleAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from an Muon to its primary TrackParticle.
 */


#include "MuonTrackParticleAssociationTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MuonTrackParticleAssociationTool::MuonTrackParticleAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_type (PRIMARY)
{
  declareProperty ("Type", m_typeprop = "Primary",
                   "Which TrackParticle to return.  One of "
                   "Primary, Combined, "
                   "InDet, InDetOrPrimary, MuonSpectrometer");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode MuonTrackParticleAssociationTool::initialize()
{
  CHECK( Base::initialize() );

  if (m_typeprop == "Primary")
    m_type = PRIMARY;
  else if (m_typeprop == "Combined")
    m_type = COMBINED;
  else if (m_typeprop == "InDet")
    m_type = INDET;
  else if (m_typeprop == "InDetOrPrimary")
    m_type = INDET_OR_PRIMARY;
  else if (m_typeprop == "MuonSpectrometer") {
    m_type = MUONSPECTROMETER;
    REPORT_MESSAGE(MSG::INFO) << "called with type : MuonSpectrometer" << m_typeprop;
  }
  else {
    REPORT_MESSAGE(MSG::ERROR) << "Bad type property " << m_typeprop;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const xAOD::TrackParticle*
MuonTrackParticleAssociationTool::get (const xAOD::Muon& p)
{
  switch (m_type) {
  case PRIMARY:
    return p.primaryTrackParticle();
  case COMBINED:
    return p.trackParticle (xAOD::Muon::CombinedTrackParticle);
  case INDET:
    return p.trackParticle (xAOD::Muon::InnerDetectorTrackParticle);
  case MUONSPECTROMETER:
    return p.trackParticle (xAOD::Muon::MuonSpectrometerTrackParticle);
  case INDET_OR_PRIMARY:
    {
      const xAOD::TrackParticle* t = p.trackParticle (xAOD::Muon::InnerDetectorTrackParticle);
      if (!t)
        t = p.primaryTrackParticle();
      return t;
    }
  }
  std::abort();
}


} // namespace D3PD
