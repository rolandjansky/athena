/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelDBTools.h"
#include "LArBadChannelTool/LArBadChanBlobUtils.h"

#include "CoolKernel/types.h"
#include "CoolKernel/Record.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/HWIdentifier.h"
#include "LArRecConditions/LArBadChannel.h"
#include "LArBadChannelTool/LArBadChannelState.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h" 

#include <vector>
#include <typeinfo>

namespace LArBadChannelDBTools {

  using namespace LArBadChanBlobUtils;

  MsgStream getDefaultMsgStream()
  {
    MsgStream log(Athena::getMessageSvc());
    return log;
  }

  /// creates the AttributeListSpecification for the LArBadChannel DB
  coral::AttributeListSpecification* createCoolSpec()
  { 
    coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
    if (spec == 0) {
      return spec;
    }
    spec->extend("ChannelSize", "unsigned int");
    spec->extend("StatusWordSize", "unsigned int");
    spec->extend("Endianness", "unsigned int");
    spec->extend("Version", "unsigned int");
    spec->extend("Blob", "blob");

    return spec;
  }

  void fillControlInfo( AthenaAttributeList* attrList, unsigned int wordSize, unsigned int vers) 
  {
    (*attrList)["ChannelSize"].setValue( (unsigned int) sizeof(Channel));
    (*attrList)["StatusWordSize"].setValue( wordSize);
    (*attrList)["Endianness"].setValue( (unsigned int) machineEndianness());
    (*attrList)["Version"].setValue( vers);
  }


  AthenaAttributeList* createChanPayload( const BadChanVec& data, 
					  const coral::AttributeListSpecification& spec)
  {
    return createPayload( data, spec, badChanVersion());
  }

  AthenaAttributeList* createFebPayload( const BadFebVec& data)
  {
    coral::AttributeListSpecification* spec = createCoolSpec();
    return createPayload( data, *spec, badFebVersion());
  }

  AthenaAttributeList* createFebPayloadV1( const std::vector<HWIdentifier>& data)
  {
    coral::AttributeListSpecification* spec = createCoolSpec();

    AthenaAttributeList* attrList = new AthenaAttributeList(*spec);
    fillControlInfo(attrList, 0, 1);
    coral::Blob& blob=(*attrList)["Blob"].data<coral::Blob>();
    fillFebBlobV1( data, blob);
    return attrList;
  }

  CondAttrListCollection* createCoolCollection( const LArBadChannelState& bcState)
  {
    CondAttrListCollection* attrListColl = new CondAttrListCollection(true);

    coral::AttributeListSpecification* spec = createCoolSpec();

    for (int i=0; i<LArBadChannelState::NCoolChannels; i++) {
      LArBadChannelState::CoolChannelEnum cc = 
	static_cast<LArBadChannelState::CoolChannelEnum>(i);
      if (!bcState.coolChannel( cc).empty()) {
	AthenaAttributeList* ilist = 
	  createChanPayload( bcState.coolChannel( cc).stateVector(), *spec);
	CondAttrListCollection::ChanNum chanNum = i;
	attrListColl->add(chanNum, *ilist);
      }
    }

    if ( attrListColl->size() == 1) {
      // Protect against collections with a single element: currently Athena cannot 
      // read them back. We just add an empty EMBA or EMBC channel to the collection...
      LArBadChannelState::CoolChannelEnum cc = LArBadChannelState::EMBA;
      if (!bcState.coolChannel( cc).empty()) cc = LArBadChannelState::EMBC;
      AthenaAttributeList* ilist = 
	createChanPayload( bcState.coolChannel( cc).stateVector(), *spec);
      CondAttrListCollection::ChanNum chanNum = cc;
      attrListColl->add(chanNum, *ilist);
    }
    else if (attrListColl->size() == 0) {
      // Create empty collection with two COOL channels to maks sure it is stored 
      // as a collection
      for (int i=0; i<2; i++) {
	LArBadChannelState::CoolChannelEnum cc = 
	  static_cast<LArBadChannelState::CoolChannelEnum>(i);
	LArBadChannelState::CoolChannelData emptyChan(cc);
	AthenaAttributeList* ilist = createChanPayload( emptyChan.stateVector(), *spec);
	CondAttrListCollection::ChanNum chanNum = i;
	attrListColl->add(chanNum, *ilist);
      }
    }

    return attrListColl;
  }

  CondAttrListCollection* createEmptyCoolCollection()
  {
    CondAttrListCollection* attrListColl = new CondAttrListCollection(true);

    coral::AttributeListSpecification* spec = createCoolSpec();
    for (int i=0; i<2; i++) {
      LArBadChannelState::CoolChannelEnum cc = 
	static_cast<LArBadChannelState::CoolChannelEnum>(i);
      LArBadChannelState::CoolChannelData emptyChan(cc);
      AthenaAttributeList* ilist = createChanPayload( emptyChan.stateVector(), *spec);
      CondAttrListCollection::ChanNum chanNum = i;
      attrListColl->add(chanNum, *ilist);
    }
    return attrListColl;
  }

  std::vector<BadChanEntry>
  readBadChan( const coral::AttributeList& attrList, MsgStream& log) 
  {
    const coral::Blob& blob = attrList["Blob"].data<coral::Blob>();
    unsigned int chanSize = attrList["ChannelSize"].data<unsigned int>();
    unsigned int stateSize = attrList["StatusWordSize"].data<unsigned int>();
    unsigned int endian = attrList["Endianness"].data<unsigned int>();
    unsigned int version = attrList["Version"].data<unsigned int>();
    
    return 
      LArBadChanBlobUtils::decodeBlob<LArBadChannel>( &blob, chanSize, stateSize, endian,
						      version, log);
  }

  std::vector<BadFebEntry>
  readBadFeb( const coral::AttributeList& attrList, MsgStream& log) 
  {
    const coral::Blob& blob = attrList["Blob"].data<coral::Blob>();
    unsigned int chanSize = attrList["ChannelSize"].data<unsigned int>();
    unsigned int stateSize = attrList["StatusWordSize"].data<unsigned int>();
    unsigned int endian = attrList["Endianness"].data<unsigned int>();
    unsigned int version = attrList["Version"].data<unsigned int>();
    
    if (version == 1) {
      // Version 1 nas only HWIdentifier and no LArBadFeb, so we have to add a
      // LArBadFeb with status "deadAll" by hand
      std::vector<HWIdentifier> idvec = 
	LArBadChanBlobUtils::decodeFebBlobV1( &blob, chanSize, endian, version, log);
      std::vector<BadFebEntry> result;
      result.resize(idvec.size());
      LArBadFeb missingState;
      LArBadFebBitPacking febPacking;
      febPacking.setBit( LArBadFeb::deadAllBit, missingState);
      for (unsigned int i=0; i<idvec.size(); ++i) {
	result[i] = BadFebEntry( idvec[i], missingState);
      }
      return result;
    }
    else {
      return LArBadChanBlobUtils::decodeBlob<LArBadFeb>( &blob, chanSize, stateSize,
							 endian, version, log);
    }
  }

}
