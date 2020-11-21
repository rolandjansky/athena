/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOL_H
#define MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOL_H
   
#include "CxxUtils/checker_macros.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MDT_RawDataProviderToolCore.h"
#include <vector>
#include <string>

#include "MuonRDO/MdtCsm_Cache.h"

namespace Muon 
{

/** @class MDT_RawDataProviderTool       
    @author  Edward Moyse <Edward.Moyse@cern.ch>
    @author  Mark Owen <markowen@cern.ch>
*/  
/// This class is only used in a single-thread mode
class ATLAS_NOT_THREAD_SAFE MDT_RawDataProviderTool : virtual public IMuonRawDataProviderTool, public MDT_RawDataProviderToolCore
{
  public:    
  MDT_RawDataProviderTool(const std::string&,const std::string&,const IInterface*);

  /** default destructor */
  virtual ~MDT_RawDataProviderTool ();
    
  /** standard Athena-Algorithm method */
  virtual StatusCode initialize() override;
    
  /** standard Athena-Algorithm method */
  virtual StatusCode finalize  () override;
  
  /** Convert method - declared in Muon::IMuonRdoToPrepDataTool*/
  virtual StatusCode convert ATLAS_NOT_THREAD_SAFE ( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs) override;
  virtual StatusCode convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, const std::vector<IdentifierHash>&) override;

  /** the new ones */
  virtual StatusCode convert() override; //!< for the entire event 
  virtual StatusCode convert(const std::vector<IdentifierHash>& HashVec) override;
  virtual StatusCode convert(const std::vector<uint32_t>& robIds) override;//!< for a particular vector of ROBId's
  /** EventContext ones **/
  virtual StatusCode convert(const EventContext&) const override; //!< for the entire event 
  virtual StatusCode convert(const std::vector<IdentifierHash>& HashVec, const EventContext&) const override;
  virtual StatusCode convert(const std::vector<uint32_t>& robIds, const EventContext&) const override;//!< for a particular vector of ROBId's
  virtual StatusCode convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, const EventContext&) const override;
  virtual StatusCode convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, const std::vector<IdentifierHash>&, const EventContext&) const override;


};
}

#endif
