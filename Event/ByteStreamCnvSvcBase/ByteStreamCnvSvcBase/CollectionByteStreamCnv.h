/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTRAMCNVSVC_COLLECTIONBYTESTREAMCNV_H
#define BYTESTRAMCNVSVC_COLLECTIONBYTESTREAMCNV_H

#include "GaudiKernel/Converter.h"
#include "ByteStreamData/ROBData.h" 

class IOpaqueAddress;
class DataObject;
class StatusCode;
class IAddressCreator;
class IROBDataProviderSvc; 

#include <string>

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/**
 * @class CollectionByteStreamCnv
 * @brief template class for BS converter for Collections

*/
  
template < class TOOL > 
class CollectionByteStreamCnv: public Converter {
  friend class CnvFactory<CollectionByteStreamCnv<TOOL> >;

 protected:
  CollectionByteStreamCnv(ISvcLocator* svcloc);

 public:

  //  typedef typename RDP::ROBData ROBData; 

  typedef typename TOOL::COLLECTION COLLECTION ; 

  /** @brief initialize the converter
   */
  virtual StatusCode initialize() override;

  /** @brief create data object from Address
   */
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
  
  /** @brief write data object to Bytestream
   */
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

  /** @brief  Storage type for BS 
   */
  virtual long repSvcType() const override { return i_repSvcType(); }

  /** @brief  Storage type for BS 
   */
  static long storageType();

  /** @brief  CLID for the data type 
   */
  static const CLID& classID();

private: 

  /** @brief pointer to the ROB data provider
   */
   IROBDataProviderSvc* m_robDataProvider; 

   /** @brief tool responsbile for converting the data.
    */
   TOOL*  m_tool; 

};

#include "ByteStreamCnvSvcBase/CollectionByteStreamCnv.icc"

#endif



