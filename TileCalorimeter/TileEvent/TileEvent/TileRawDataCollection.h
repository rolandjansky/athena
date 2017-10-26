/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************
// Filename : TileRawDataCollection.h
// Author   : A.Solodkov
// Created  : Oct, 2002
//
// DESCRIPTION:
//   TileCal drawer output object Collection   
//   It contains the ROD output objects from one super-drawer
//   i.e. up to 48 TileRawData objects 
//   It is meant to store TileRawData in Store Gate
//    
//
// HISTORY:
//    
// BUGS:
//
// *******************************************************************

#ifndef TILEEVENT_TILERAWDATACOLLECTION_H
#define TILEEVENT_TILERAWDATACOLLECTION_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/Identifier.h"
// #include "TileEvent/TileRoI_ID.h"

#include <string>
#include <map>
#include <inttypes.h>
                                        
template <typename _TElement> class TileRawDataCollection
  : public DataVector<_TElement>
{
 public:                        
                                
    typedef typename TileRawDataCollection<_TElement>::const_iterator TCollection_const_iterator ; 

    // required for a Digit Collection
    typedef int ID;
    typedef Identifier TT_ID;
    typedef _TElement DIGIT;
    typedef _TElement OBJECT;

    // construct a container for a given super-drawer (fragment)
    TileRawDataCollection<_TElement> ( ID id, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS )
      : DataVector <_TElement> (ownPolicy), 
        m_id(id), m_lvl1Id(0), m_lvl1Type(0), m_detEvType(0), m_rodBCID(0) { this->reserve(48); }
    TileRawDataCollection<_TElement> ( SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS )
      : DataVector < _TElement > (ownPolicy),
        m_id(0), m_lvl1Id(0), m_lvl1Type(0), m_detEvType(0), m_rodBCID(0) { this->reserve(48); }

    // destructor               
    virtual ~TileRawDataCollection<_TElement> () { }

    // clear everything for next event
    virtual void clear ();

    // return fragment ID corresponding to this container 
    ID  identify() const { return m_id; }
                              
//    void setTT(const TT_ID& id, 
//                     TCollection_const_iterator b, TCollection_const_iterator e); 
//
//    void getTT(const TT_ID& id,
//                TCollection_const_iterator& b, TCollection_const_iterator& e) const ; 

    // print all the Channels
    virtual void print() const;
    virtual operator std::string () const;
    virtual std::string whoami (void) const { _TElement elem;
                 return "TileRawDataCollection of "+elem.whoami(); }
    
    /** Setter for level1 id from ROD header */
    void setLvl1Id(uint32_t lvl1Id) { m_lvl1Id = lvl1Id; }
  
    /** Getter for level1 id */
    inline uint32_t getLvl1Id() const { return m_lvl1Id; }
    
    /** Setter for level1 type from ROD header */
    void setLvl1Type(uint32_t lvl1Type) { m_lvl1Type = lvl1Type; }
    
    /** Getter for level1 type */
    inline uint32_t getLvl1Type() const { return m_lvl1Type; }
    
    /** Setter for detector event type from ROD header */
    void setDetEvType(uint32_t detEvType) { m_detEvType = detEvType; }
    
    /** Getter for detector event type */
    inline uint32_t getDetEvType() const { return m_detEvType; }
    
    /** Setter for BCID from ROD header */
    void setRODBCID(uint32_t rodBCID) { m_rodBCID = rodBCID; }
    
    /** Getter for BCID from */
    inline uint32_t getRODBCID() const { return m_rodBCID; }
 
protected:
    void setOwnership(SG::OwnershipPolicy ownPolicy) { this->m_ownPolicy = ownPolicy; }
    SG::OwnershipPolicy getOwnership() const { return this->m_ownPolicy; }

private:      
    ID  m_id;

    /** Level1 id from ROD header */
    uint32_t m_lvl1Id;
    /** Level1 Type from ROD header */
    uint32_t m_lvl1Type;
    /** Detector event type from ROD header */
    uint32_t m_detEvType;
    /** BCID from ROD header */
    uint32_t m_rodBCID;

    /** internal map for TTs - commented

    // iterator pair for begin and end 
    typedef std::pair<TCollection_const_iterator,TCollection_const_iterator> ITPAIR; 
    // ID to Iterator Map 
    typedef std::map<TT_ID,ITPAIR> ID_IT_MAP;

    typedef typename ID_IT_MAP::const_iterator  ID_IT_MAP_ITER;

    ID_IT_MAP  m_Map; 
    */ 
};

#include "TileEvent/TileRawDataCollection.icc"

#endif

