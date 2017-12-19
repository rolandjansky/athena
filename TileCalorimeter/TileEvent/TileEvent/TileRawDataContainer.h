/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************
// Filename : TileRawDataContainer.h
// Author   : A.Solodkov
// Created  : Oct, 2002
//
// DESCRIPTION:
//   Base Raw Event Container class for Tile calorimeter
//   The collections inside container are sorted by frag_id
//    
//
// HISTORY:
//    
// BUGS:
//
// *******************************************************************

#ifndef TILEEVENT_TILERAWDATACONTAINER_H
#define TILEEVENT_TILERAWDATACONTAINER_H

#include "EventContainers/IdentifiableContainer.h" 
#include "AthenaKernel/CLASS_DEF.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileIdentifier/TileFragHash.h"
#include "TileEvent/TileRawDataCollection.h"
#include <string>
#include <inttypes.h>

template <typename _TCollection> class TileRawDataContainer
  : public IdentifiableContainer<_TCollection>
{
public:

    typedef typename _TCollection::_TElement _TElement;
    typedef typename TileRawDataContainer<_TCollection>::const_iterator TContainer_const_iterator ; 
    typedef typename _TCollection::TCollection_const_iterator TCollection_const_iterator ;

    typedef IdentifiableContainer<_TCollection> MyBase;
    typedef TileFragHash::TYPE TYPE;
    typedef TileRawChannelUnit::UNIT UNIT;

    // constructor
    TileRawDataContainer(bool createColl=false, 
                         TYPE type=TileFragHash::Digitizer,
                         UNIT unit=TileRawChannelUnit::ADCcounts,
                         SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS);
    TileRawDataContainer(bool createColl, SG::OwnershipPolicy ownPolicy);

    // desctructor
    virtual ~TileRawDataContainer() { }

    // initialize all collections
    void initialize(bool createColl, TYPE type,
                    SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS);

    // clear all collections
    void clear();
  
    // insert a TileRawData element into a collection.
    // this is only to be used by Algorithm-builder
    void add (_TElement* rc, bool createColl=false, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS);
    inline void push_back(_TElement* rc) { add(rc,true); }
    inline TYPE get_hashType() const { return this->m_hashFunc.type(); }
    inline UNIT get_unit() const { return this->m_unit; }
    inline void set_unit(UNIT unit) { m_unit=unit; }
    inline TYPE get_type() const { return this->m_type; }
    inline void set_type(TYPE type) { m_type=type; }
    inline uint32_t get_bsflags() const { return this->m_bsflags; }
    inline void set_bsflags(uint32_t bsflags) { m_bsflags=bsflags; }
    inline const TileFragHash & hashFunc() const { return this->m_hashFunc; }

    // print all the Collections
    virtual void print() const;
    virtual operator std::string () const;
    virtual std::string whoami (void) const { _TCollection  coll;
                 return "TileRawDataContainer of "+coll.whoami(); }

private:

  UNIT m_unit; // we want to know if it's ADC counts or pCb or ...
  TYPE m_type; // type of algorithm used to produce RawChannels
  uint32_t m_bsflags; // some status bits from bytestream

  TileFragHash m_hashFunc;
};


// Persistent representation of TileRawDataContainer for POOL

template <typename _TCollection> class TileRawDataCollectionVec
  : public DataVector<_TCollection>
{

public:

  TileRawDataCollectionVec()
    : DataVector<_TCollection>(SG::VIEW_ELEMENTS)
    , m_type(TileFragHash::Default)
    //, m_unit(TileRawChannelUnit::ADCcounts)
    { }

  virtual ~TileRawDataCollectionVec() { }

  TileFragHash::TYPE m_type; // flag saved for initializing TileRawDataContainer.

};


#include "TileEvent/TileRawDataContainer.icc"

#endif

