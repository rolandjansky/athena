///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// T_TilePoolContainerCnv.h 
// Base class for Tile converters
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_T_TILEPOOLCONTAINERCNV_H
#define TILETPCNV_T_TILEPOOLCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "EventContainers/SelectAllObject.h"
#include "TileEvent/TileRawDataContainer.h"
#include "TileEvent/TileMutableDataContainer.h"

#include <vector>
#include <inttypes.h>

template<class TRANS, class PERS, class CONV>
class T_TilePoolContainerCnv : public T_AthenaPoolTPCnvBase<TRANS, PERS> {
public:
  
  typedef typename PERS::ElemVector pers_ElemVector;
  typedef typename PERS::const_iterator pers_const_iterator;
  typedef typename SelectAllObject<TRANS>::const_iterator trans_const_iterator;
  using Collection = typename TRANS::IDENTIFIABLE;

  T_TilePoolContainerCnv() : m_elementCnv()  {}

  /** Converts vector of PERS::value_type objects to vector of TRANS::value_type objects,
      using converter CONV
      @param persVect [IN] vector of persistent objects
      @param transVect [IN] vector of transient object
      @param log [IN] output message stream
  */
  virtual void persToTrans(const PERS* pers, TRANS* trans, MsgStream &log) {

    const std::vector<unsigned int>& param = pers->getParam();
    const pers_ElemVector& vec = pers->getVector();

    unsigned int pers_type = (param.size()>0) ? param[0] : 0;
    int hashType = pers_type    & 0xF;
    int type = (pers_type >> 4) & 0xF;
    int unit = (pers_type >> 8) & 0xF;
    uint32_t bsflags = pers_type & 0xFFFFF000;
    if (hashType == 0xF) hashType = TileFragHash::Beam;
    if (type == 0xF) type = TileFragHash::Beam;

    log << MSG::DEBUG << MSG::hex << "pers_type= 0x" << pers_type 
        << " - " <<  bsflags << " " << unit << " " << type << " " << hashType
        << MSG::dec << " Nelements= " << vec.size() << endmsg;

    trans->cleanup(); // remove all collections

    if ( abs(trans->get_hashType()-hashType) > 0xF) {
      log << MSG::DEBUG << "Pers hash type " << hashType
          << " does not match Trans hash type " << trans->get_hashType()
          << " ==> reinitializing hash " << endmsg;

      trans->initialize(false,(TileFragHash::TYPE)hashType); 
    }

    trans->set_unit((TileRawChannelUnit::UNIT)unit);
    trans->set_type((TileFragHash::TYPE)type);
    trans->set_bsflags(bsflags);


    auto mutableContainer = std::make_unique<TileMutableDataContainer<TRANS>>();
    if (mutableContainer->status().isFailure()) {
      throw std::runtime_error("Failed to initialize Tile mutable Container");
    }

    for(const auto& element : vec) {
      if (mutableContainer->push_back(m_elementCnv.createTransient(&element, log)).isFailure()) {
        throw std::runtime_error("Failed to add Tile element to Collection");
      }
    }

    auto hashes = mutableContainer->GetAllCurrentHashes();
    for (auto hash : hashes) {
      Collection* coll = mutableContainer->indexFindPtr(hash);
      auto newColl = std::make_unique<Collection>(std::move(*coll));
      if (trans->addOrDelete(std::move(newColl), hash).isFailure()) {
        throw std::runtime_error("Failed to add Tile collection to Identifiable Container");
      }
    }

  }
  
  /** Converts vector of TRANS::value_type objects to vector of PERS::value_type objects,
      using converter CONV
      @param transVect [IN] vector of transient object
      @param persVect [IN] vector of persistent objects
      @param log [IN] output message stream
  */
  virtual void transToPers(const TRANS* trans, PERS* pers, MsgStream &log) {

    pers->clear();
    pers->reserve(1, 12288);

    unsigned int pers_type = ((trans->get_hashType() & 0xF) |
                             ((trans->get_type() & 0xF)<<4) |
                             ((trans->get_unit() & 0xF)<<8) |
                             (trans->get_bsflags() & 0xFFFFF000) ) ; 
    pers->push_back_param(pers_type);

    SelectAllObject<TRANS> selAll(trans); 
    for(trans_const_iterator it = selAll.begin(),
                           iEnd = selAll.end();
        it != iEnd; ++it) {
      m_elementCnv.transToPers((*it), pers->newElem(), log);
    }

    log << MSG::DEBUG << MSG::hex << "pers_type= 0x" << pers_type
        << " - " << (trans->get_bsflags()>>12)
        << " " << trans->get_unit() 
        << " " << trans->get_type() 
        << " " << trans->get_hashType()
        << MSG::dec << "  Nelements= " << pers->getVector().size() << endmsg;
  }

private:
  /// TP converter used for vector elements
  CONV 	m_elementCnv;	
};

#endif
