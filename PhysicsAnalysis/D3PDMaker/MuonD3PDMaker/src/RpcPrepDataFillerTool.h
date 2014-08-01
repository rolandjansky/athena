/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RpcPrepDataFillerTool.h 535262 2013-01-29 13:54:17Z salekd $
#ifndef D3PDMAKER_RPCPREPDATAFILLERTOOL_H
#define D3PDMAKER_RPCPREPDATAFILLERTOOL_H

#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include <string>

namespace D3PD {

/**
 * @brief Block filler tool for the RPC hits
 *
 * @author David Salek <David.Salek@cern.ch>
 *
 * $Revision: 535262 $
 * $Date: 2013-01-29 14:54:17 +0100 (Tue, 29 Jan 2013) $
 */
class RpcPrepDataFillerTool
  : public BlockFillerTool<Muon::RpcPrepData>
{
public:
  /// Regular Gaudi AlgTool constructor
  RpcPrepDataFillerTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  /// Function booking the ntuple variables
  virtual StatusCode book();

  /// Function filling the ntuple variables for a single object
  virtual StatusCode fill (const Muon::RpcPrepData& obj);


private:
 float* m_x;
 float* m_y;
 float* m_z;
 float* m_time;
 float* m_locX;
};

} // namespace D3PD

#endif
