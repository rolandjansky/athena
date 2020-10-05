/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store MuonCalibTruth classes in                 //
// Author: Zdenko van Kesteren                             //
// Date  : 2 December 2005                                 //
//*********************************************************//

#ifndef MuonCalibTruthCollection_h
#define MuonCalibTruthCollection_h

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <vector>
#include <iostream>

namespace MuonCalib{

  class MuonCalibTruth ;
  class MuonCalibMdtTruthHit ;
  class MuonCalibRpcTruthHit ;
  class MuonCalibTgcTruthHit ;
  class MuonCalibCscTruthHit ;

  /**
     @class MuonCalibTruthCollection

     MuonCalibTruthCollection is a Calibration EDM container class containing the following truth classes:
     - a set of MuonCalibTruth s (C-EDM version of Truth Track)
     - a set of MuonCalibMdtTruthHit s (C-EDM version of MdtSDO)
     - a set of MuonCalibRpcTruthHit s (C-EDM version of RpcSDO)
     - a set of MuonCalibTgcTruthHit s (C-EDM version of TgcSDO)
     - a set of MuonCalibCscTruthHit s (C-EDM version of CscSDO)

   */
  
  class MuonCalibTruthCollection{
    public:
    typedef std::vector<MuonCalibTruth*>  TruthVec   ;          //!< typedef of a set of MuonCalibTruth* s
    typedef TruthVec::iterator            TruthVecIt ;          //!< typedef of the iterator to the set of MuonCalibTruth s
    typedef TruthVec::const_iterator      TruthVecCit;          //!< typedef of the const_iterator to the set of MuonCalibTruth s

    typedef std::vector<MuonCalibMdtTruthHit*> MdtTruthVec   ;  //!< typedef of set of MuonCalibMdtTruthHit* s
    typedef MdtTruthVec::iterator              MdtTruthVecIt ;  //!< typedef of the iterator to the set of MuonCalibMdtTruthHit* s
    typedef MdtTruthVec::const_iterator        MdtTruthVecCit;  //!< typedef of the const_iterator to the set of MuonCalibMdtTruthHit* s

    typedef std::vector<MuonCalibRpcTruthHit*> RpcTruthVec   ;  //!< typedef of set of MuonCalibRpcTruthHit* s
    typedef RpcTruthVec::iterator              RpcTruthVecIt ;  //!< typedef of the iterator to the set of MuonCalibRpcTruthHit* s
    typedef RpcTruthVec::const_iterator        RpcTruthVecCit;  //!< typedef of the const_iterator to the set of MuonCalibRpcTruthHit* s

    typedef std::vector<MuonCalibTgcTruthHit*> TgcTruthVec   ;  //!< typedef of set of MuonCalibTgcTruthHit* s
    typedef TgcTruthVec::iterator              TgcTruthVecIt ;  //!< typedef of the iterator to the set of MuonCalibTgcTruthHit* s
    typedef TgcTruthVec::const_iterator        TgcTruthVecCit;  //!< typedef of the const_iterator to the set of MuonCalibTgcTruthHit* s

    typedef std::vector<MuonCalibCscTruthHit*> CscTruthVec   ;  //!< typedef of set of MuonCalibCscTruthHit* s
    typedef CscTruthVec::iterator              CscTruthVecIt ;  //!< typedef of the iterator to the set of MuonCalibCscTruthHit* s
    typedef CscTruthVec::const_iterator        CscTruthVecCit;  //!< typedef of the const_iterator to the set of MuonCalibCscTruthHit* s

    MuonCalibTruthCollection();  //!< Default constructor
    MuonCalibTruthCollection( TruthVec truthVec ) : m_truthVec(truthVec){} ;  //!< constructor initializing the set of MuonCalibTruth s
      ~MuonCalibTruthCollection() ; //!< destructor
      MuonCalibTruthCollection( const MuonCalibTruthCollection& truthCollection );//!< copyconstructor
      
      MuonCalibTruthCollection& operator=( const MuonCalibTruthCollection& rhs ); //!< assignment operator

    TruthVecIt truthCollectionBegin();             //!< retrieve iterator to the first MuonCalibTruth* of this truth collection.
    TruthVecIt truthCollectionEnd();               //!< retrieve iterator to the last MduonCalibTruth* of this truth collection.
    TruthVecCit truthCollectionBegin() const;      //!< retrieve const_iterator to the first MuonCalibTruth* of this truth collection.
    TruthVecCit truthCollectionEnd() const;        //!< retrieve const_iterator to the last MuonCalibTruth* of this truth collection.
    
    MdtTruthVecIt  mdtTruthCollectionBegin();      //!< retrieve iterator to the first MuonCalibMdtTruthHit* of this truth collection.
    MdtTruthVecIt  mdtTruthCollectionEnd();        //!< retrieve iterator to the last MuonCalibMdtTruthHit* of this truth collection.
    MdtTruthVecCit mdtTruthCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibMdtTruthHit* of this truth collection.
    MdtTruthVecCit mdtTruthCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibMdtTruthHit* of this truth collection.

    RpcTruthVecIt  rpcTruthCollectionBegin();      //!< retrieve iterator to the first MuonCalibRpcTruthHit* of this truth collection.
    RpcTruthVecIt  rpcTruthCollectionEnd();        //!< retrieve iterator to the last MuonCalibRpcTruthHit* of this truth collection.
    RpcTruthVecCit rpcTruthCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibRpcTruthHit* of this truth collection.
    RpcTruthVecCit rpcTruthCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibRpcTruthHit* of this truth collection.

    TgcTruthVecIt  tgcTruthCollectionBegin();      //!< retrieve iterator to the first MuonCalibTgcTruthHit* of this truth collection.
    TgcTruthVecIt  tgcTruthCollectionEnd();        //!< retrieve iterator to the last MuonCalibTgcTruthHit* of this truth collection.
    TgcTruthVecCit tgcTruthCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibTgcTruthHit* of this truth collection.
    TgcTruthVecCit tgcTruthCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibTgcTruthHit* of this truth collection.

    CscTruthVecIt  cscTruthCollectionBegin();      //!< retrieve iterator to the first MuonCalibCscTruthHit* of this truth collection.
    CscTruthVecIt  cscTruthCollectionEnd();        //!< retrieve iterator to the last MuonCalibCscTruthHit* of this truth collection.
    CscTruthVecCit cscTruthCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibCscTruthHit* of this truth collection.
    CscTruthVecCit cscTruthCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibCscTruthHit* of this truth collection.

    int numberOfTruth() const ;                      //!< retrieve the number of MuonCalibTruth s in the collection
    void addTruth( MuonCalibTruth* truth ) ;         //!< add a MuonCalibTruth to the collection
    void addTruth( MuonCalibMdtTruthHit* mdtTruth) ; //!< add a MuonCalibMdtTruthHit to the collection 
    void addTruth( MuonCalibRpcTruthHit* rpcTruth) ; //!< add a MuonCalibRpcTruthHit to the collection 
    void addTruth( MuonCalibTgcTruthHit* tgcTruth) ; //!< add a MuonCalibTgcTruthHit to the collection 
    void addTruth( MuonCalibCscTruthHit* cscTruth) ; //!< add a MuonCalibCscTruthHit to the collection 

    private:
    TruthVec    m_truthVec;     //!< set of MuonCalibTruth s
    MdtTruthVec m_mdtTruthVec;  //!< set of MuonCalibMdtTruthHit s
    RpcTruthVec m_rpcTruthVec;  //!< set of MuonCalibRpcTruthHit s
    TgcTruthVec m_tgcTruthVec;  //!< set of MuonCalibTgcTruthHit s
    CscTruthVec m_cscTruthVec;  //!< set of MuonCalibCscTruthHit s
 
  };

  inline MuonCalibTruthCollection::TruthVecIt MuonCalibTruthCollection::truthCollectionBegin() { return m_truthVec.begin() ; } 
  inline MuonCalibTruthCollection::TruthVecIt MuonCalibTruthCollection::truthCollectionEnd(  ) { return m_truthVec.end() ; }
  inline MuonCalibTruthCollection::TruthVecCit MuonCalibTruthCollection::truthCollectionBegin() const { return m_truthVec.begin() ; }
  inline MuonCalibTruthCollection::TruthVecCit MuonCalibTruthCollection::truthCollectionEnd() const   { return m_truthVec.end() ; }

  inline MuonCalibTruthCollection::MdtTruthVecIt MuonCalibTruthCollection::mdtTruthCollectionBegin() { return m_mdtTruthVec.begin() ; } 
  inline MuonCalibTruthCollection::MdtTruthVecIt MuonCalibTruthCollection::mdtTruthCollectionEnd()   { return m_mdtTruthVec.end() ; }
  inline MuonCalibTruthCollection::MdtTruthVecCit MuonCalibTruthCollection::mdtTruthCollectionBegin() const { return m_mdtTruthVec.begin() ; }
  inline MuonCalibTruthCollection::MdtTruthVecCit MuonCalibTruthCollection::mdtTruthCollectionEnd() const   { return m_mdtTruthVec.end() ; }

  inline MuonCalibTruthCollection::RpcTruthVecIt MuonCalibTruthCollection::rpcTruthCollectionBegin() { return m_rpcTruthVec.begin() ; } 
  inline MuonCalibTruthCollection::RpcTruthVecIt MuonCalibTruthCollection::rpcTruthCollectionEnd()   { return m_rpcTruthVec.end() ; }
  inline MuonCalibTruthCollection::RpcTruthVecCit MuonCalibTruthCollection::rpcTruthCollectionBegin() const { return m_rpcTruthVec.begin() ; }
  inline MuonCalibTruthCollection::RpcTruthVecCit MuonCalibTruthCollection::rpcTruthCollectionEnd() const   { return m_rpcTruthVec.end() ; }

  inline MuonCalibTruthCollection::TgcTruthVecIt MuonCalibTruthCollection::tgcTruthCollectionBegin() { return m_tgcTruthVec.begin() ; } 
  inline MuonCalibTruthCollection::TgcTruthVecIt MuonCalibTruthCollection::tgcTruthCollectionEnd()   { return m_tgcTruthVec.end() ; }
  inline MuonCalibTruthCollection::TgcTruthVecCit MuonCalibTruthCollection::tgcTruthCollectionBegin() const { return m_tgcTruthVec.begin() ; }
  inline MuonCalibTruthCollection::TgcTruthVecCit MuonCalibTruthCollection::tgcTruthCollectionEnd() const   { return m_tgcTruthVec.end() ; }

  inline MuonCalibTruthCollection::CscTruthVecIt MuonCalibTruthCollection::cscTruthCollectionBegin() { return m_cscTruthVec.begin() ; } 
  inline MuonCalibTruthCollection::CscTruthVecIt MuonCalibTruthCollection::cscTruthCollectionEnd()   { return m_cscTruthVec.end() ; }
  inline MuonCalibTruthCollection::CscTruthVecCit MuonCalibTruthCollection::cscTruthCollectionBegin() const { return m_cscTruthVec.begin() ; }
  inline MuonCalibTruthCollection::CscTruthVecCit MuonCalibTruthCollection::cscTruthCollectionEnd() const   { return m_cscTruthVec.end() ; }

  inline int MuonCalibTruthCollection::numberOfTruth() const { 
    return m_truthVec.size() ; 
  }
  
  inline void MuonCalibTruthCollection::addTruth( MuonCalibTruth* truth ) { 
    if (truth) {
      m_truthVec.push_back(truth) ; 
    } else {
      MsgStream log(Athena::getMessageSvc(),"MuonCalibTruthCollection");
      log<<MSG::WARNING<<"MuonCalibTruth not passed to MuonCalibTruthCollection"<<endmsg;
    }
  }
  

  inline void MuonCalibTruthCollection::addTruth( MuonCalibMdtTruthHit* truth ) { 
    if (truth) {
      m_mdtTruthVec.push_back(truth) ;
    } else {
      MsgStream log(Athena::getMessageSvc(),"MuonCalibTruthCollection");
      log<<MSG::WARNING<<"MuonCalibMdtTruthHit not passed to MuonCalibTruthCollection"<<endmsg;
    }
  }

  inline void MuonCalibTruthCollection::addTruth( MuonCalibRpcTruthHit* truth ) { 
    if (truth) {
      m_rpcTruthVec.push_back(truth) ;
    } else {
      MsgStream log(Athena::getMessageSvc(),"MuonCalibTruthCollection");
      log<<MSG::WARNING<<"MuonCalibRpcTruthHit not passed to MuonCalibTruthCollection"<<endmsg;
    }
  }

  inline void MuonCalibTruthCollection::addTruth( MuonCalibTgcTruthHit* truth ) { 
    if (truth) {
      m_tgcTruthVec.push_back(truth) ;
    } else {
      MsgStream log(Athena::getMessageSvc(),"MuonCalibTruthCollection");
      log<<MSG::WARNING<<"MuonCalibTgcTruthHit not passed to MuonCalibTruthCollection"<<endmsg;
    }
  }

  inline void MuonCalibTruthCollection::addTruth( MuonCalibCscTruthHit* truth ) { 
    if (truth) {
      m_cscTruthVec.push_back(truth) ;
    } else {
      MsgStream log(Athena::getMessageSvc(),"MuonCalibTruthCollection");
      log<<MSG::WARNING<<"MuonCalibCscTruthHit not passed to MuonCalibTruthCollection"<<endmsg;
    }
  }

}//end namespace MuonCalib

#endif
