/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TriggerEDMSerialiserTool_H
#define TRIGOUTPUTHANDLING_TriggerEDMSerialiserTool_H


#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODCore/AuxSelection.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaSerializeSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "TrigOutputHandling/HLTResultMTMakerTool.h"
#include "TrigSerializeTP/TrigSerTPTool.h"

/**
 * @class TriggerEDMSerialiserTool is tool responsible for creation of HLT Result filled with streamed EDM collections
 **/

class DataObject;

class TriggerEDMSerialiserTool: public extends<AthAlgTool, HLTResultMTMakerTool>
{

 public:

  TriggerEDMSerialiserTool( const std::string& type,
	     const std::string& name,
	     const IInterface* parent );

  virtual ~TriggerEDMSerialiserTool();
  virtual StatusCode fill( HLT::HLTResultMT& resultToFill ) const override;

  virtual StatusCode  initialize() override;

 private:
  Gaudi::Property<std::vector<std::string>> m_collectionsToSerialize { this, "CollectionsToSerialize", {}, "TYPE#SG.aux1.aux2..etc key of collections to be streamed (like in StreamAOD), the type has to be an exact type i.e. with _vN not the alias type" };

  Gaudi::Property<int> m_moduleID { this, "ModuleID", 0, "The HLT result fragment to which the output should be added"};

  // internal structure to keep configuration organised conveniently
  struct Address {
    enum Catrgory { xAODInterface, xAODAux, OldTP, xAODDecoration, None };
    Address( const std::string& transType_,
	     const std::string& persType_,
	     const CLID clid_,
	     const std::string& key_,
	     const Catrgory category_ = None,
	     const xAOD::AuxSelection& sel_ = {} )
    : transType(transType_),
      persType(persType_),
      clid(clid_),
      key(key_),
      category(category_),
      sel(sel_){}

    std::string transType;
    std::string persType; // actuall versioned type
    CLID clid;
    std::string key;
    Catrgory category;
    xAOD::AuxSelection sel = {}; // xAOD dynamic varaibles selection, relevant only for xAODAux category

  };

  std::vector< Address > m_toSerialise; // postprocessed configuration info

  ServiceHandle<IClassIDSvc> m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc", "Service to translate class name to CLID" };
  ServiceHandle<IAthenaSerializeSvc> m_serializerSvc{ this, "Serializer", "AthenaRootSerializeSvc", "Service that translates transient to persistent respresenation" };

  ToolHandle<TrigSerTPTool> m_tpTool{ this, "TPTool", "TrigSerTPTool/TrigSerTPTool", "Tool to do Transient/Persistent conversion (Old EDM)"};


  /**
   * Given the ID if the collection (in address arg) insert basic streaming info into the buffer.
   */
  StatusCode makeHeader( const TriggerEDMSerialiserTool::Address& address, std::vector<uint32_t>& buffer  ) const;

  /**
   * For copy bytest from the memory into the buffer converting from char[] to uint32_t[]
   * This function is candidate to be made global function at some point
   * and we will need also readPayload function
   */
  StatusCode fillPayload( const void* data, size_t sz, std::vector<uint32_t>& buffer ) const;

  /**
   * Place inside the buffer the serialised container (can be either TP, xAOD)
   * invloves simple involcation of serialiser
   */
  StatusCode serialiseContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const;
  /**
   * Place inside the buffer serialised the xOAD Aux container
   * invloves selection and recording of dynamic variables
   */
  StatusCode serialisexAODAuxContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const;

  /**
   * Place inside the buffer the serialised old type of container
   * invloves T/P conversion
   */
  StatusCode serialiseTPContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const;



  /**
   * Adds dynamic variables to the payload
   */
  StatusCode serialiseDynAux( DataObject* dObject, const Address& address, std::vector<uint32_t>& buffer ) const;
};


#endif //> !TRIGOUTPUTHANDLING_TriggerEDMSerialiserTool_H
