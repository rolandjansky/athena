/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/TrackIsolationFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2010
 * @brief Fill track isolation variables.
 */


#include "TrackIsolationFillerTool.h"
#include "xAODBase/IParticle.h"
#include "IsolationTool/ITrackIsolationTool.h"
#include "IsolationTool/ICaloIsolationTool.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/StrFormat.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TrackIsolationFillerTool::TrackIsolationFillerTool (const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent)
  : BlockFillerTool<xAOD::IParticle> (type, name, parent),
    m_trackIsoTool ("TrackIsolationTool"),
    m_caloIsoTool ("CaloIsolationTool")
{
  declareProperty ("TrackIsoVar", m_trackIsoVar = "trackIso",
                   "Name to use for track isolation tuple variable. "
                   "If blank, the variable will not be filled. "
                   "If it contains a `%%', this will be replaced by the "
                   "cone size.  Otherwise, if more than one cone size is "
                   "requested, the the cone size will be appended to the "
                   "variable name.");
  declareProperty ("CaloIsoVar", m_caloIsoVar = "caloIso",
                   "Name to use for calorimeter isolation tuple variable. "
                   "If blank, the variable will not be filled. "
                   "If it contains a `%%', this will be replaced by the "
                   "cone size.  Otherwise, if more than one cone size is "
                   "requested, the the cone size will be appended to the "
                   "variable name.");
  declareProperty ("NTrackIsoVar", m_nTrackIsoVar = "nTrackIso",
                   "Name to use for isolation track count tuple variable. "
                   "If blank, the variable will not be filled. "
                   "If it contains a `%%', this will be replaced by the "
                   "cone size.  Otherwise, if more than one cone size is "
                   "requested, the the cone size will be appended to the "
                   "variable name.");

  declareProperty ("ConeSizes", m_coneSizes,
                   "List of isolation cone sizes.");
  declareProperty ("ConeSize", m_coneSize = 0.3,
                   "Isolation cone size.  Only used if ConeSizes is not set.");
  declareProperty ("TrackIsolationTool", m_trackIsoTool,
                   "Track isolation tool instance.");
  declareProperty ("CaloIsolationTool", m_caloIsoTool,
                   "Calorimeter isolation tool instance.");

  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TrackIsolationFillerTool::initialize()
{
  if (m_coneSizes.empty())
    m_coneSizes.push_back (m_coneSize);

  m_coneSizeTypes.reserve (m_coneSizes.size());
  for (float conesz : m_coneSizes) {
    m_coneSizeTypes.push_back ( (conesz-0.1) / 0.05 + 0.5 );
  }

  CHECK( m_trackIsoTool.retrieve() );
  CHECK( m_caloIsoTool.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Helper to book isolation variables.
 */
template <class T>
StatusCode TrackIsolationFillerTool::bookIsoVar (std::string varName,
                                                 std::vector<T>& var,
                                                 const std::string& docString)
{
  if (varName.find ("%%") == std::string::npos && m_coneSizes.size() > 1)
    varName = varName + "%%";
  std::string::size_type pos = varName.find ("%%");

  var.resize (m_coneSizes.size());

  for (size_t i = 0; i < m_coneSizes.size(); i++) {
    double coneSize = m_coneSizes[i];

    std::string thisName = varName;
    if (pos != std::string::npos) {
      int conetag = int(coneSize*100 + 0.5);
      thisName.replace (pos, 2, CxxUtils::strformat("%2d", conetag));
    }
    std::string doc = docString + " for cone size " +
      CxxUtils::strformat("%.1f", coneSize);
    CHECK( addVariable (thisName, var[i], doc) );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode TrackIsolationFillerTool::book()
{
  if (!m_trackIsoVar.empty())
    CHECK( bookIsoVar (m_trackIsoVar, m_trackIso,
                       "Tracking isolation parameter") );

  if (!m_caloIsoVar.empty())
    CHECK( bookIsoVar (m_caloIsoVar,   m_caloIso,
                       "Calorimeter isolation parameter") );

  if (!m_nTrackIsoVar.empty())
    CHECK( bookIsoVar (m_nTrackIsoVar, m_nTrackIso,
                       "Number of tracks in isolation cone") );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TrackIsolationFillerTool::fill (const xAOD::IParticle& p)
{
  size_t ncones = m_coneSizes.size();
  std::vector<double> energies (ncones);

  xAOD::TrackIsolation trk_res;
  if ((!m_trackIso.empty() || !m_nTrackIso.empty()) &&
      m_trackIsoTool->trackIsolation (trk_res, p, cones(xAOD::Iso::ptcone)))
  {
    assert (trk_res.ptcones.size() == ncones);
    assert (trk_res.nucones.size() == ncones);
    for (size_t i = 0; i < ncones; i++) {
      if (!m_trackIso.empty())
        *m_trackIso[i] = trk_res.ptcones[i];
      if (!m_nTrackIso.empty())
      *m_nTrackIso[i] = trk_res.nucones[i];
    }
  }

  xAOD::CaloIsolation calo_res;
  if (!m_caloIsoVar.empty() &&
      m_caloIsoTool->caloIsolation (calo_res, p, cones(xAOD::Iso::etcone)))
  {
    assert (calo_res.etcones.size() == ncones);
    for (size_t i = 0; i < ncones; i++)
      *m_caloIso[i] = calo_res.etcones[i];
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a vector of isolation type codes for the requested
 *        cone sizes and a specified flavor.
 * @param f The isolation flavor.
 */
std::vector<xAOD::Iso::IsolationType>
TrackIsolationFillerTool::cones (xAOD::Iso::IsolationFlavour f) const
{
  size_t ncones = m_coneSizeTypes.size();
  std::vector<xAOD::Iso::IsolationType> out (ncones);
  for (size_t i = 0; i < ncones; i++)
    out[i] = static_cast<xAOD::Iso::IsolationType> (m_coneSizeTypes[i] + f*10);
  return out;
}


} // namespace D3PD
