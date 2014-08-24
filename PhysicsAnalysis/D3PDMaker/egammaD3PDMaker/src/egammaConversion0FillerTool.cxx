/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaConversion0FillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Block filler tool for photon conversion information, for LOD 0.
 */


#include "egammaConversion0FillerTool.h"
#include "xAODTracking/Vertex.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
egammaConversion0FillerTool::egammaConversion0FillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<xAOD::Photon> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Book variables for this block.
 */
StatusCode egammaConversion0FillerTool::book()
{
  CHECK( addVariable ("convFlag",       m_convFlag,
                      "Number of tracks for the first conversion candidate; "
                      "0 if no conversion or if there are more than "
                      "two tracks.") );
  CHECK( addVariable ("isConv",         m_isConv,
                      "True if the first conversion candidate has "
                      "1 or 2 tracks.") );
  CHECK( addVariable ("nConv",          m_nConv,
                      "Number of conversion vertices.") );
  CHECK( addVariable ("nSingleTrackConv",    m_nSingleTrackConv,
                   "Number of conversion vertices with exactly one track.") );
  CHECK( addVariable ("nDoubleTrackConv",    m_nDoubleTrackConv,
                   "Number of conversion vertices with exactly two tracks.") );

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
StatusCode egammaConversion0FillerTool::fill (const xAOD::Photon& p)
{
  const xAOD::Vertex* conv = p.vertex();

  if (conv && conv->nTrackParticles()<=2)
    *m_convFlag = conv->nTrackParticles();
  else
    *m_convFlag = 0;

  *m_isConv = (*m_convFlag > 0);

  *m_nConv = p.nVertices();
  for (int i = 0; i < *m_nConv; i++) {
    const xAOD::Vertex* conv = p.vertex(i);
    if (conv) {
      int ntr = conv->nTrackParticles();
      if (ntr == 1)
        ++*m_nSingleTrackConv;
      else if (ntr == 2)
        ++*m_nDoubleTrackConv;
    }
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
