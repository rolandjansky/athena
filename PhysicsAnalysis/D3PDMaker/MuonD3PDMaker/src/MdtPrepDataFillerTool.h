/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MdtPrepDataFillerTool.h 535262 2013-01-29 13:54:17Z salekd $
#ifndef D3PDMAKER_MDTPREPDATAFILLERTOOL_H
#define D3PDMAKER_MDTPREPDATAFILLERTOOL_H

#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include <string>

namespace Muon { class MdtPrepData; }

namespace D3PD {

/**
 * @brief Block filler tool for the MDT hits
 *
 * @author David Salek <David.Salek@cern.ch>
 *
 * $Revision: 535262 $
 * $Date: 2013-01-29 14:54:17 +0100 (Tue, 29 Jan 2013) $
 */
class MdtPrepDataFillerTool
  : public BlockFillerTool<Muon::MdtPrepData>
{
public:
  /// Regular Gaudi AlgTool constructor
  MdtPrepDataFillerTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  /// Function booking the ntuple variables
  virtual StatusCode book();

  /// Function filling the ntuple variables for a single object
  virtual StatusCode fill (const Muon::MdtPrepData& obj);


private:
 float* m_x;
 float* m_y;
 float* m_z;
 int* m_tdc;
 int* m_adc;
 float* m_locX;
 int* m_status;
};

} // namespace D3PD

#endif
