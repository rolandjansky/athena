/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaConversionFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for photon conversion information.
 */


#include "egammaConversionFillerTool.h"
#include "xAODTracking/Vertex.h"
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <cmath>


using CLHEP::HepLorentzVector;


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
egammaConversionFillerTool::egammaConversionFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<xAOD::Photon> (type, name, parent)
{
  book().ignore();  // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode egammaConversionFillerTool::book()
{
  CHECK( addVariable ("hasconv",          m_hasconv) );
  CHECK( addVariable ("convvtxx",         m_convvtxx) );
  CHECK( addVariable ("convvtxy",         m_convvtxy) );
  CHECK( addVariable ("convvtxz",         m_convvtxz) );
  CHECK( addVariable ("Rconv",            m_Rconv) );
  CHECK( addVariable ("zconv",            m_zconv) );
  CHECK( addVariable ("convvtxchi2",      m_convvtxchi2) );
  CHECK( addVariable ("pt1conv",          m_pt1conv) );
  CHECK( addVariable ("convtrk1nBLHits",  m_convtrk1nBLHits) );
  CHECK( addVariable ("convtrk1nPixHits", m_convtrk1nPixHits) );
  CHECK( addVariable ("convtrk1nSCTHits", m_convtrk1nSCTHits) );
  CHECK( addVariable ("convtrk1nTRTHits", m_convtrk1nTRTHits) );
  CHECK( addVariable ("pt2conv",          m_pt2conv) );
  CHECK( addVariable ("convtrk2nBLHits",  m_convtrk2nBLHits) );
  CHECK( addVariable ("convtrk2nPixHits", m_convtrk2nPixHits) );
  CHECK( addVariable ("convtrk2nSCTHits", m_convtrk2nSCTHits) );
  CHECK( addVariable ("convtrk2nTRTHits", m_convtrk2nTRTHits) );
  CHECK( addVariable ("ptconv",           m_ptconv) );
  CHECK( addVariable ("pzconv",           m_pzconv) );

  return StatusCode::SUCCESS;
}


namespace {


/// Helper to get a TrackParticle from a vertex.
const xAOD::TrackParticle* gettp (const xAOD::Vertex* conv, unsigned int n)
{
  if (n >= conv->nTrackParticles()) return 0;
  return conv->trackParticle(n);
}


}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 *
 * FIXME: Split this up!
 */
StatusCode egammaConversionFillerTool::fill (const xAOD::Photon& p)
{
  const xAOD::Vertex* conv = p.vertex();
  if (conv) {
    *m_hasconv = true;

    *m_convvtxx = conv->x();
    *m_convvtxy = conv->y();
    *m_convvtxz = conv->z();
    *m_Rconv = static_cast<float> (hypot (conv->x(), conv->y()));
    *m_zconv = static_cast<float> (conv->z());
    *m_convvtxchi2 = static_cast<float> (conv->chiSquared());

    const xAOD::TrackParticle* tp1 = gettp (conv, 0);
    const xAOD::TrackParticle* tp2 = gettp (conv, 1);
    TLorentzVector psum;
    if (tp1) {
      psum += tp1->p4();
      *m_pt1conv = tp1->pt();
      tp1->summaryValue (*m_convtrk1nBLHits,  xAOD::numberOfInnermostPixelLayerHits);
      tp1->summaryValue (*m_convtrk1nPixHits, xAOD::numberOfPixelHits);
      tp1->summaryValue (*m_convtrk1nSCTHits, xAOD::numberOfSCTHits);
      tp1->summaryValue (*m_convtrk1nTRTHits, xAOD::numberOfTRTHits);
    }


    if (tp2) {
      psum += tp2->p4();
      *m_pt2conv = tp2->pt();
      tp2->summaryValue (*m_convtrk2nBLHits,  xAOD::numberOfInnermostPixelLayerHits);
      tp2->summaryValue (*m_convtrk2nPixHits, xAOD::numberOfPixelHits);
      tp2->summaryValue (*m_convtrk2nSCTHits, xAOD::numberOfSCTHits);
      tp2->summaryValue (*m_convtrk2nTRTHits, xAOD::numberOfTRTHits);
    }

    *m_ptconv = psum.Pt();
    *m_pzconv = psum.Pz();
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
