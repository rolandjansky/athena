/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store MuonCalibRawTriggerHit classes           //
// Author: Giuseppe Salamanna                              //
// Date  : 18th february 2009                              //
//*********************************************************//

#ifndef MuonCalibRawTriggerHitCollection_h
#define MuonCalibRawTriggerHitCollection_h

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <vector>
#include <iostream>

namespace MuonCalib{

  class MuonCalibRawRpcTriggerHit ;

  /**
     @class MuonCalibRawTriggerHitCollection
  */
  
  class MuonCalibRawTriggerHitCollection{
  public:
    typedef std::vector<MuonCalibRawRpcTriggerHit*>      MuonCalibRawRpcTriggerHitVec   ; //!< typedef of a set of MuonCalibRawRpcTriggerHit s
    typedef MuonCalibRawRpcTriggerHitVec::iterator       MuonCalibRawRpcTriggerHitVecIt ; //!< typedef of an iterator of the set muonCalibRawRpcTriggerHit s
    typedef MuonCalibRawRpcTriggerHitVec::const_iterator MuonCalibRawRpcTriggerHitVecCit; //!< typedef of a const_iterator of the set muonCalibRawRpcTriggerHit s

    MuonCalibRawTriggerHitCollection(); //<! default constructor
    MuonCalibRawTriggerHitCollection( MuonCalibRawRpcTriggerHitVec rawRpcTriggerHitVec ) :  m_rawRpcTriggerHitVec(rawRpcTriggerHitVec){} ;//!< constructor initializing all raw hit collections
    ~MuonCalibRawTriggerHitCollection() ; //!< destructor
    MuonCalibRawTriggerHitCollection( const MuonCalibRawTriggerHitCollection& MuonCalibRawTriggerHitCollection ); //!< copyconstructor
      
    MuonCalibRawTriggerHitCollection& operator=( const MuonCalibRawTriggerHitCollection& rhs ); //!< assingment operator
      
    MuonCalibRawRpcTriggerHitVecIt  rawRpcTriggerHitCollectionBegin();       //!< retrieve iterator to the first MuonCalibRawRpcHit of the collection
    MuonCalibRawRpcTriggerHitVecIt  rawRpcTriggerHitCollectionEnd();         //!< retrieve iterator to the last MuonCalibRawRpcHit of the collection
    MuonCalibRawRpcTriggerHitVecCit rawRpcTriggerHitCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibRawRpcHit of the collection
    MuonCalibRawRpcTriggerHitVecCit rawRpcTriggerHitCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibRawRpcHit of the collection

    int numberOfMuonCalibRawRpcTriggerHits() const ;                 //!< retrieve the number of MuonCalibRawRpcHits in the collection
    void addMuonCalibRawTriggerHit( MuonCalibRawRpcTriggerHit* rawTriggerHit ) ;   //!< add a MuonCalibRawRpcHit to the collection
        
  private:
    MuonCalibRawRpcTriggerHitVec m_rawRpcTriggerHitVec;                     //!< set of MuonCalibRawRpcHit s
  };
  
  inline MuonCalibRawTriggerHitCollection::MuonCalibRawRpcTriggerHitVecIt  MuonCalibRawTriggerHitCollection::rawRpcTriggerHitCollectionBegin() {    return m_rawRpcTriggerHitVec.begin() ; }   
  inline MuonCalibRawTriggerHitCollection::MuonCalibRawRpcTriggerHitVecIt  MuonCalibRawTriggerHitCollection::rawRpcTriggerHitCollectionEnd() {    return m_rawRpcTriggerHitVec.end() ;  }  
  inline MuonCalibRawTriggerHitCollection::MuonCalibRawRpcTriggerHitVecCit MuonCalibRawTriggerHitCollection::rawRpcTriggerHitCollectionBegin() const {    return m_rawRpcTriggerHitVec.begin() ; }  
  inline MuonCalibRawTriggerHitCollection::MuonCalibRawRpcTriggerHitVecCit MuonCalibRawTriggerHitCollection::rawRpcTriggerHitCollectionEnd() const {    return m_rawRpcTriggerHitVec.end() ; }

  inline int MuonCalibRawTriggerHitCollection::numberOfMuonCalibRawRpcTriggerHits() const { return m_rawRpcTriggerHitVec.size() ; }

  inline void MuonCalibRawTriggerHitCollection::addMuonCalibRawTriggerHit( MuonCalibRawRpcTriggerHit* rawTriggerHit ) { 
    if(rawTriggerHit) {
      m_rawRpcTriggerHitVec.push_back(rawTriggerHit) ; 
    } else {
      MsgStream log(Athena::getMessageSvc(),"MuonCalibRawTriggerHitCollection");
      log<<MSG::WARNING<<"Refuse to put an empty MuonCalibRawRpcTriggerHit in MuonCalibRawTriggerHitCollection"<<endmsg;
    }
  }  
}

#endif
