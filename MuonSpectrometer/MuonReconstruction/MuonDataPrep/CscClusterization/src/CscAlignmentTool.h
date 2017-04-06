/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscAlignmentTool.h

#ifndef CscAlignmentTool_H
#define CscAlignmentTool_H

// Woochun Park
// April 2009
//
// Strip fitter using the parabolic fit fron the CSC calibration tool.

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CscClusterization/ICscAlignmentTool.h"
class Identifier;
class CscIdHelper;
namespace MuonGM 
{
    class MuonDetectorManager;
}

class CscAlignmentTool : virtual public ICscAlignmentTool, public AthAlgTool {

public:  // Ctors and dtor.

  // Constructor.
  CscAlignmentTool(std::string, std::string, const IInterface*);

  // Destructor.
  ~CscAlignmentTool();

public:  // AlgTool methods

  // Initialization.
  StatusCode initialize();

  // Finalization.
  StatusCode finalize();

public:
  double getAlignmentOffset(Identifier pstripId) const;

  

private:  // data
  // Pointer to muon geometry manager.
  const MuonGM::MuonDetectorManager* m_pmuon_detmgr;
  const CscIdHelper* m_phelper;

  // Strip fitter.
  bool m_useAlignment;
  std::vector<double> m_etaposAlignconsts;
  std::vector<double> m_phiposAlignconsts;
};

#endif
