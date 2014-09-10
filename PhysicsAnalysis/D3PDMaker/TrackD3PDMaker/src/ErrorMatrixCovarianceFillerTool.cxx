/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/ErrorMatrixCovarianceFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2012
 * @brief Fill covariance elements from ErrorMatrix.
 */


#include "ErrorMatrixCovarianceFillerTool.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
ErrorMatrixCovarianceFillerTool::ErrorMatrixCovarianceFillerTool (const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent)
  : D3PD::BlockFillerTool<AmgSymMatrix(5)> (type, name, parent)
{
  declareProperty ("LocalType", m_localType = "CYLINDRICAL",
                   "Type of local variables: "
                   "CYLINDRICAL -- d0/z0; CARTESIAN -- locX/loxY; "
                   "NONE -- not written.");
  declareProperty ("FillThetaAndQoverP", m_fillThetaAndQoverP = true,
                   "Should theta and qoverp be filled?");
  declareProperty ("FillPhi", m_fillPhi=true,
                   "Should the phi variable be filled?");
  declareProperty ("DefaultValue", m_defaultValue = 0,
                   "Default value to which to set unfilled variables.");
}


/**
 * @brief Book variables for this block.
 */
StatusCode ErrorMatrixCovarianceFillerTool::book()
{
  std::vector<Trk::ParamDefs> varcodes;
  std::vector<std::string> varnames;

  if (m_localType == "CYLINDRICAL") {
    varcodes.push_back (Trk::loc1);    varnames.push_back ("d0");
    varcodes.push_back (Trk::loc2);    varnames.push_back ("z0");
  }
  else if (m_localType == "CARTESIAN") {
    varcodes.push_back (Trk::loc1);    varnames.push_back ("locX");
    varcodes.push_back (Trk::loc2);    varnames.push_back ("locY");
  }

  if (m_fillPhi) {
    varcodes.push_back (Trk::phi0);    varnames.push_back ("phi");
  }

  if (m_fillThetaAndQoverP){
    varcodes.push_back (Trk::theta);   varnames.push_back ("theta");
    varcodes.push_back (Trk::qOverP);  varnames.push_back ("qoverp");
  }

  size_t n = varcodes.size();
  m_vars.clear();
  m_vars.resize (n * (n-1) / 2);

  size_t ipos = 0;
  for (size_t i = 0; i < n; i++) {
    for (size_t j = i+1; j < n; j++) {
      assert (ipos < m_vars.size());
      std::string name = varnames[i] + "_" + varnames[j] + "_err";
      CHECK( addVariable (name, m_vars[ipos].var,
                          varnames[i] + " / " + varnames[j] + " covariance",
                          m_defaultValue) );
      m_vars[ipos].v1 = varcodes[i];
      m_vars[ipos].v2 = varcodes[j];
      ++ipos;
    }
  }
  assert (ipos == m_vars.size());

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
  StatusCode ErrorMatrixCovarianceFillerTool::fill (const AmgSymMatrix(5) &p)
{
  for (size_t ipos = 0; ipos < m_vars.size(); ipos++) {
    Var& v = m_vars[ipos];
    *v.var = p(v.v1, v.v2);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD
