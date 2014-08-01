/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TgcPrepDataFillerTool.h 542179 2013-03-24 05:55:13Z ssnyder $
#ifndef D3PDMAKER_TGCPREPDATAFILLERTOOL_H
#define D3PDMAKER_TGCPREPDATAFILLERTOOL_H

#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include <string>

namespace D3PD {

/**
 * @brief Block filler tool for the TGC hits
 *
 * @author David Salek <David.Salek@cern.ch>
 *
 * $Revision: 542179 $
 * $Date: 2013-03-24 06:55:13 +0100 (Sun, 24 Mar 2013) $
 */
class TgcPrepDataFillerTool
  : public BlockFillerTool<Muon::TgcPrepData>
{
public:
  /// Regular Gaudi AlgTool constructor
  TgcPrepDataFillerTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  /// Function booking the ntuple variables
  virtual StatusCode book();

  /// Function filling the ntuple variables for a single object
  virtual StatusCode fill (const Muon::TgcPrepData& obj);


private:
 float* m_x;
 float* m_y;
 float* m_z;
 float* m_locX;
 //int* m_bcBitMap;
};

} // namespace D3PD

#endif
