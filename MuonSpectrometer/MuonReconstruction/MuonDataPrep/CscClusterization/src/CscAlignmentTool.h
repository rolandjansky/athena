/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscAlignmentTool_H
#define CscAlignmentTool_H

// Woochun Park
// April 2009
//
// Strip fitter using the parabolic fit fron the CSC calibration tool.

#include "AthenaBaseComps/AthAlgTool.h"
#include "CscClusterization/ICscAlignmentTool.h"

#include <string>
#include <vector>

class Identifier;
class CscIdHelper;

class CscAlignmentTool : virtual public ICscAlignmentTool, public AthAlgTool {

public:  // Ctors and dtor.

  // Constructor.
  CscAlignmentTool(const std::string&, const std::string&, const IInterface*);

  // Destructor.
  ~CscAlignmentTool()=default;

  // Initialization.
  StatusCode initialize();

public:
  double getAlignmentOffset(Identifier pstripId) const;

private:  // data
  const CscIdHelper* m_phelper;

  // Strip fitter.
  bool m_useAlignment;
  std::vector<double> m_etaposAlignconsts;
  std::vector<double> m_phiposAlignconsts;
};

#endif
