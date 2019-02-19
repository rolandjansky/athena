/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelDBTools_H
#define LArBadChannelDBTools_H

#include "CoolKernel/types.h"
#include "CoolKernel/Record.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/HWIdentifier.h"
#include "LArBadChannelTool/HWIdentifier32.h"
#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/LArBadFeb.h"
#include "LArRecConditions/LArBadFebBitPacking.h"
#include "LArBadChannelTool/LArBadChannelState.h"
#include "LArBadChannelTool/LArBadChanBlobUtils.h"

#include "GaudiKernel/MsgStream.h"

#include <vector>

namespace LArBadChannelDBTools {

  typedef std::pair<HWIdentifier, LArBadChannel>                BadChanEntry;
  typedef std::pair<HWIdentifier, LArBadFeb>                    BadFebEntry;
  typedef std::vector<BadChanEntry>                             BadChanVec;
  typedef std::vector<BadFebEntry>                              BadFebVec;
  typedef HWIdentifier32::value_type                              Channel;

  /// used to get default msg stream 
  MsgStream getDefaultMsgStream();

  /// creates the AttributeListSpecification for the LArBadChannel DB
  coral::AttributeListSpecification* createCoolSpec();

  template <class Entry>
  AthenaAttributeList* createPayload( const std::vector<Entry>& data, 
				      const coral::AttributeListSpecification& spec );

  CondAttrListCollection* createCoolCollection( const LArBadChannelState& bcState);

  AthenaAttributeList* createChanPayload( const BadChanVec& data, 
					  const coral::AttributeListSpecification& spec);

  AthenaAttributeList* createFebPayload( const BadFebVec& data);

  AthenaAttributeList* createFebPayloadV1( const std::vector<HWIdentifier32>& data);

  /// Create an empty collection with no bad channels, e.g. for MC
  CondAttrListCollection* createEmptyCoolCollection();

  /// Version of the BLOB encoding, to be incremented each time the BLOB format changes
  inline unsigned int badChanVersion() {return 1;}
  inline unsigned int badFebVersion() {return 2;}

  void fillControlInfo( AthenaAttributeList* attrList, unsigned int wordSize, unsigned int vers); 

  template <class T>
  AthenaAttributeList* createPayload( const std::vector< std::pair<HWIdentifier,T> >& data, 
				      const coral::AttributeListSpecification& spec,
				      unsigned int vers)
  {
    AthenaAttributeList* attrList = new AthenaAttributeList(spec);
    fillControlInfo(attrList, sizeof( typename T::BitWord), vers);
    coral::Blob& blob=(*attrList)["Blob"].data<coral::Blob>();
    LArBadChanBlobUtils::fillBlob( data, blob);
    return attrList;
  }

  std::vector<BadChanEntry>
  readBadChan( const coral::AttributeList& attrList, MsgStream& log);

  std::vector<BadFebEntry>
  readBadFeb( const coral::AttributeList& attrList, MsgStream& log);
}

#endif
