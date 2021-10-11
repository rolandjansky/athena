/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOL_H
#define MUONRDOTOPREPDATA_RPCRDOTOPREPDATATOOL_H

/// This code is used in both MT and single-thread                      
/// However the MT code uses the const cast in a single-thread          
/// mode by transfering the contents to a thread-safe container         
/// in order to prevent complex adjustments in the core decode functions
/// As such, it should not be flagged at this stage as unsafe           
#include "CxxUtils/checker_macros.h"

#include "RpcRdoToPrepDataToolCore.h"

namespace Muon {
/// This class is only used in a single-thread mode
class ATLAS_NOT_THREAD_SAFE RpcRdoToPrepDataTool
  : public extends<RpcRdoToPrepDataToolCore, IMuonRdoToPrepDataTool>
{
public:
  RpcRdoToPrepDataTool( const std::string&, const std::string&, const IInterface* );
  virtual ~RpcRdoToPrepDataTool()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect ) const override;
  virtual StatusCode decode( const std::vector<uint32_t>& robIds ) const override;

  virtual void printPrepData() const override;

protected:
  virtual StatusCode manageOutputContainers(bool& firstTimeInTheEvent) const;

private:
  mutable State m_state;
};

}

#endif // !ATHEXJOBOPTIONS_CONCRETETOOL_H
