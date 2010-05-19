/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BCM_RAWDATABYTESTREAMCNV_BCM_RAWDATAPROVIDERTOOL_H
#define BCM_RAWDATABYTESTREAMCNV_BCM_RAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "ByteStreamData/RawEvent.h" 

#include <inttypes.h>
#include <set>

class BCM_RDO_Container;
class BCM_RodDecoder;

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

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
  virtual StatusCode initialize();

  //! finalize
  virtual StatusCode finalize();
  
  //! this is the main decoding method
  StatusCode convert(std::vector<const ROBFragment*>& vecRobs, BCM_RDO_Container* rdoCont);

private: 
  
  ToolHandle<BCM_RodDecoder>  m_decoder;   
  
  // bookkeeping if we have decoded a ROB already
  std::set<uint32_t> m_robIdSet;
  unsigned int       m_lastLvl1ID;

};

#endif
