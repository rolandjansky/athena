/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATAPROVIDERTOOL_CHARGE_H
#define ALFA_RAWDATAPROVIDERTOOL_CHARGE_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "ByteStreamData/RawEvent.h" 

#include "DataModel/DataVector.h"

#include "ALFA_RawEv/ALFA_RawDataContainer_charge.h"
#include "ALFA_RawEv/ALFA_RawDataCollection_charge.h"
#include "ALFA_RawEv/ALFA_RawData_charge.h"

#include "ALFA_Decoder_charge.h"

#include <stdint.h>

#include <vector>
#include <set>
#include <string>


class ALFA_RawData_charge;
class ALFA_RawDataCollection_charge;
class ALFA_RawDataContainer_charge;


// the tool to decode a ROB fragment

class ALFA_RawDataProviderTool_charge : public AthAlgTool
{

 public:
   
  //! AthAlgTool InterfaceID
  static const InterfaceID& interfaceID();
 
  //! constructor
  ALFA_RawDataProviderTool_charge(const std::string& type, const std::string& name, const IInterface* parent);

  //! destructor
  virtual ~ALFA_RawDataProviderTool_charge();

  //! initialize
  virtual StatusCode initialize();

//! finalize
  virtual StatusCode finalize();
  

   //! this is the main decoding method
  StatusCode convert_charge(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,ALFA_RawDataContainer_charge* rdoCont);
  
private:
 
 
  ToolHandle<ALFA_Decoder_charge>  m_decoder_charge;  
  // bookkeeping if we have decoded a ROB already
  std::set<uint32_t> m_robIdSet;
  unsigned int                      m_lastLvl1ID;
 

};

#endif

