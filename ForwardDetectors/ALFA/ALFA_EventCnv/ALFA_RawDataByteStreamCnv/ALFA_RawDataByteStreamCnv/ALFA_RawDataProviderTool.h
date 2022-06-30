/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATAPROVIDERTOOL_H
#define ALFA_RAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_RawEv/ALFA_RawDataCollection.h"
#include "ALFA_RawEv/ALFA_RawData.h"

#include "ALFA_Decoder.h"

#include <stdint.h>

#include <vector>
#include <set>
#include <string>


class ALFA_RawData;
class ALFA_RawDataCollection;
class ALFA_RawDataContainer;


// the tool to decode a ROB fragment

class ALFA_RawDataProviderTool : public AthAlgTool
{

 public:
   
  //! constructor
  ALFA_RawDataProviderTool(const std::string& type, const std::string& name, const IInterface* parent);

  //! destructor
  virtual ~ALFA_RawDataProviderTool();

  //! initialize
  virtual StatusCode initialize() override;

   //! this is the main decoding method
  StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,ALFA_RawDataContainer* rdoCont);
  
private:

  ToolHandle<ALFA_Decoder>  m_decoder{this, "Decoder", "ALFA_Decoder"};

};

#endif

