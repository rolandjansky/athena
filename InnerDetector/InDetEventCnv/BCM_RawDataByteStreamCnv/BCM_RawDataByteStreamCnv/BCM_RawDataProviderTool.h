/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCM_RAWDATABYTESTREAMCNV_BCM_RAWDATAPROVIDERTOOL_H
#define BCM_RAWDATABYTESTREAMCNV_BCM_RAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "ByteStreamData/RawEvent.h" 

#include <inttypes.h>
#include <atomic>

class BCM_RDO_Container;
class BCM_RodDecoder;

// the tool to decode a ROB fragment

class BCM_RawDataProviderTool : public AthAlgTool
{

 public:
   
  //! AthAlgTool InterfaceID
  static const InterfaceID& interfaceID();
  
  //! constructor
  BCM_RawDataProviderTool(const std::string& type, const std::string& name, const IInterface* parent);

  //! destructor 
  virtual ~BCM_RawDataProviderTool();

  //! initialize
  virtual StatusCode initialize() override;

  //! finalize
  virtual StatusCode finalize() override;
  
  //! this is the main decoding method
  StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, BCM_RDO_Container* rdoCont) const;

private:
  mutable std::atomic<int> m_DecodeErrCount;
  ToolHandle<BCM_RodDecoder>  m_decoder;
};

#endif
