/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store MuonCalibRawHit classes in               //
// Author: Zdenko van Kesteren                             //
// Date  : 13th january 2006                               //
//*********************************************************//

#ifndef MuonCalibRawHitCollection_h
#define MuonCalibRawHitCollection_h

#include <vector>
#include <iostream>

namespace MuonCalib {

  class MuonCalibRawMdtHit;
  class MuonCalibRawRpcHit;
  class MuonCalibRawTgcHit;
  class MuonCalibRawTgcCoin;
  class MuonCalibRawCscHit;

  /**
     @class MuonCalibRawHitCollection
     A MuonCalibRawHitcollections is a container class within the
     Calibration EDM holding the MuonCalibXxxRawHits from the
     different technologies:
     - a set of MuonCalibRawMdtHit s
     - a set of MuonCalibRawRpcHit s
     - a set of MuonCalibRawTgcHit s
     - a set of MuonCalibRawCscHit s

     A MuonCalibXxxRawHit is constructed from PrepRawData hits,
     collected from stations which have a MuonCalibSegment. These raw
     hits are uncalibrated. In order to know whether the raw hit is NOT
     assigned to a MuonCalibSegment, check the occupancy variable. It
     yields the number of times that the raw hit is used on a
     MuonCalibSegment (one hit may be used multiple times on
     different segments). A raw hit not used on any segment yields an
     occupancy of -1.

  */
  
  class MuonCalibRawHitCollection{
    public:
    typedef std::vector<MuonCalibRawMdtHit*>      MuonCalibRawMdtHitVec;    //!< typedef of a set of MuonCalibRawMdtHit s
    typedef MuonCalibRawMdtHitVec::iterator       MuonCalibRawMdtHitVecIt;  //!< typedef of an iterator of the set muonCalibRawMdtHit s
    typedef MuonCalibRawMdtHitVec::const_iterator MuonCalibRawMdtHitVecCit; //!< typedef of a const_iterator of the set muonCalibRawMdtHit s

    typedef std::vector<MuonCalibRawRpcHit*>      MuonCalibRawRpcHitVec;    //!< typedef of a set of MuonCalibRawRpcHit s
    typedef MuonCalibRawRpcHitVec::iterator       MuonCalibRawRpcHitVecIt;  //!< typedef of an iterator of the set muonCalibRawRpcHit s
    typedef MuonCalibRawRpcHitVec::const_iterator MuonCalibRawRpcHitVecCit; //!< typedef of a const_iterator of the set muonCalibRawRpcHit s

    typedef std::vector<MuonCalibRawTgcHit*>      MuonCalibRawTgcHitVec;    //!< typedef of a set of MuonCalibRawTgcHit s
    typedef MuonCalibRawTgcHitVec::iterator       MuonCalibRawTgcHitVecIt;  //!< typedef of an iterator of the set muonCalibRawTgcHit s
    typedef MuonCalibRawTgcHitVec::const_iterator MuonCalibRawTgcHitVecCit; //!< typedef of a const_iterator of the set muonCalibRawTgcHit s

    typedef std::vector<MuonCalibRawTgcCoin*>      MuonCalibRawTgcCoinVec;   //!< typedef of a set of MuonCalibRawTgcHit s
    typedef MuonCalibRawTgcCoinVec::iterator       MuonCalibRawTgcCoinVecIt; //!< typedef of an iterator of the set muonCalibRawTgcHit s
    typedef MuonCalibRawTgcCoinVec::const_iterator MuonCalibRawTgcCoinVecCit;//!< typedef of a const_iterator of the set muonCalibRawTgcHit s

    typedef std::vector<MuonCalibRawCscHit*>      MuonCalibRawCscHitVec;     //!< typedef of a set of MuonCalibRawCscHit s
    typedef MuonCalibRawCscHitVec::iterator       MuonCalibRawCscHitVecIt;   //!< typedef of an iterator of the set muonCalibRawCscHit s
    typedef MuonCalibRawCscHitVec::const_iterator MuonCalibRawCscHitVecCit;  //!< typedef of a const_iterator of the set muonCalibRawCscHit s

    MuonCalibRawHitCollection(); //<! default constructor
    MuonCalibRawHitCollection( MuonCalibRawMdtHitVec rawMdtHitVec, 
			       MuonCalibRawRpcHitVec rawRpcHitVec, 
			       MuonCalibRawTgcHitVec rawTgcHitVec,
 			       MuonCalibRawTgcCoinVec rawTgcCoinVec, 
			       MuonCalibRawCscHitVec rawCscHitVec ) : m_rawMdtHitVec(rawMdtHitVec), m_rawRpcHitVec(rawRpcHitVec), 
      m_rawTgcHitVec(rawTgcHitVec), m_rawTgcCoinVec(rawTgcCoinVec), m_rawCscHitVec(rawCscHitVec) {}; //!< constructor initializing all raw hit collections
    ~MuonCalibRawHitCollection(); //!< destructor
    MuonCalibRawHitCollection( const MuonCalibRawHitCollection &MuonCalibRawHitCollection ); //!< copy constructor
      
    MuonCalibRawHitCollection& operator=( const MuonCalibRawHitCollection &rhs ); //!< assignment operator
      
    MuonCalibRawMdtHitVecIt rawMdtHitCollectionBegin();       //!< retrieve iterator to the first MuonCalibRawMdtHit of the collection
    MuonCalibRawMdtHitVecIt rawMdtHitCollectionEnd();         //!< retrieve iterator to the last MuonCalibRawMdtHit of the collection
    MuonCalibRawMdtHitVecCit rawMdtHitCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibRawMdtHit of the collection
    MuonCalibRawMdtHitVecCit rawMdtHitCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibRawMdtHit of the collection

    MuonCalibRawRpcHitVecIt rawRpcHitCollectionBegin();       //!< retrieve iterator to the first MuonCalibRawRpcHit of the collection
    MuonCalibRawRpcHitVecIt rawRpcHitCollectionEnd();         //!< retrieve iterator to the last MuonCalibRawRpcHit of the collection
    MuonCalibRawRpcHitVecCit rawRpcHitCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibRawRpcHit of the collection
    MuonCalibRawRpcHitVecCit rawRpcHitCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibRawRpcHit of the collection

    MuonCalibRawTgcHitVecIt rawTgcHitCollectionBegin();       //!< retrieve iterator to the first MuonCalibRawTgcHit of the collection
    MuonCalibRawTgcHitVecIt rawTgcHitCollectionEnd();         //!< retrieve iterator to the last MuonCalibRawTgcHit of the collection
    MuonCalibRawTgcHitVecCit rawTgcHitCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibRawTgcHit of the collection
    MuonCalibRawTgcHitVecCit rawTgcHitCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibRawTgcHit of the collection

    MuonCalibRawTgcCoinVecIt rawTgcCoinCollectionBegin();       //!< retrieve iterator to the first MuonCalibRawTgccoin of the collection
    MuonCalibRawTgcCoinVecIt rawTgcCoinCollectionEnd();         //!< retrieve iterator to the last MuonCalibRawTgccoin of the collection
    MuonCalibRawTgcCoinVecCit rawTgcCoinCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibRawTgccoin of the collection
    MuonCalibRawTgcCoinVecCit rawTgcCoinCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibRawTgccoin of the collection

    MuonCalibRawCscHitVecIt rawCscHitCollectionBegin();       //!< retrieve iterator to the first MuonCalibRawCscHit of the collection
    MuonCalibRawCscHitVecIt rawCscHitCollectionEnd();         //!< retrieve iterator to the last MuonCalibRawCscHit of the collection
    MuonCalibRawCscHitVecCit rawCscHitCollectionBegin() const;//!< retrieve const_iterator to the first MuonCalibRawCscHit of the collection
    MuonCalibRawCscHitVecCit rawCscHitCollectionEnd() const;  //!< retrieve const_iterator to the last MuonCalibRawCscHit of the collection
    
    int numberOfMuonCalibRawMdtHits() const;                 //!< retrieve the number of MuonCalibRawMdtHits in the collection
    int numberOfMuonCalibRawRpcHits() const;                 //!< retrieve the number of MuonCalibRawRpcHits in the collection
    int numberOfMuonCalibRawTgcHits() const;                 //!< retrieve the number of MuonCalibRawTgcHits in the collection
    int numberOfMuonCalibRawTgcCoins() const;                //!< retrieve the number of MuonCalibRawTgcHits in the collection
    int numberOfMuonCalibRawCscHits() const;                 //!< retrieve the number of MuonCalibRawCscHits in the collection
    void addMuonCalibRawHit( MuonCalibRawMdtHit *rawHit );   //!< add a MuonCalibRawMdtHit to the collection
    void addMuonCalibRawHit( MuonCalibRawRpcHit *rawHit );   //!< add a MuonCalibRawRpcHit to the collection
    void addMuonCalibRawHit( MuonCalibRawTgcHit *rawHit );   //!< add a MuonCalibRawTgcHit to the collection
    void addMuonCalibRawHit( MuonCalibRawTgcCoin *rawHit );  //!< add a MuonCalibRawTgcHit to the collection
    void addMuonCalibRawHit( MuonCalibRawCscHit *rawHit );   //!< add a MuonCalibRawCscHit to the collection

    private:
    MuonCalibRawMdtHitVec m_rawMdtHitVec;                     //!< set of MuonCalibRawMdtHit s
    MuonCalibRawRpcHitVec m_rawRpcHitVec;                     //!< set of MuonCalibRawRpcHit s
    MuonCalibRawTgcHitVec m_rawTgcHitVec;                     //!< set of MuonCalibRawTgcHit s
    MuonCalibRawTgcCoinVec m_rawTgcCoinVec;                   //!< set of MuonCalibRawTgcCoin s
    MuonCalibRawCscHitVec m_rawCscHitVec;                     //!< set of MuonCalibRawCscHit s
    
  };

  inline MuonCalibRawHitCollection::MuonCalibRawMdtHitVecIt MuonCalibRawHitCollection::rawMdtHitCollectionBegin() {
    return m_rawMdtHitVec.begin(); 
  }   
  inline MuonCalibRawHitCollection::MuonCalibRawMdtHitVecIt MuonCalibRawHitCollection::rawMdtHitCollectionEnd() {
    return m_rawMdtHitVec.end();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit MuonCalibRawHitCollection::rawMdtHitCollectionBegin() const {
    return m_rawMdtHitVec.begin();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit MuonCalibRawHitCollection::rawMdtHitCollectionEnd() const {
    return m_rawMdtHitVec.end();
  }
  inline MuonCalibRawHitCollection::MuonCalibRawRpcHitVecIt MuonCalibRawHitCollection::rawRpcHitCollectionBegin() {
    return m_rawRpcHitVec.begin();
  }   
  inline MuonCalibRawHitCollection::MuonCalibRawRpcHitVecIt MuonCalibRawHitCollection::rawRpcHitCollectionEnd() {
    return m_rawRpcHitVec.end();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawRpcHitVecCit MuonCalibRawHitCollection::rawRpcHitCollectionBegin() const {
    return m_rawRpcHitVec.begin();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawRpcHitVecCit MuonCalibRawHitCollection::rawRpcHitCollectionEnd() const {
    return m_rawRpcHitVec.end();
  }
  inline MuonCalibRawHitCollection::MuonCalibRawTgcHitVecIt MuonCalibRawHitCollection::rawTgcHitCollectionBegin() {
    return m_rawTgcHitVec.begin();
  }   
  inline MuonCalibRawHitCollection::MuonCalibRawTgcHitVecIt MuonCalibRawHitCollection::rawTgcHitCollectionEnd() {
    return m_rawTgcHitVec.end();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawTgcHitVecCit MuonCalibRawHitCollection::rawTgcHitCollectionBegin() const {
    return m_rawTgcHitVec.begin();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawTgcHitVecCit MuonCalibRawHitCollection::rawTgcHitCollectionEnd() const {
    return m_rawTgcHitVec.end();
  }
  inline MuonCalibRawHitCollection::MuonCalibRawTgcCoinVecIt MuonCalibRawHitCollection::rawTgcCoinCollectionBegin() {
    return m_rawTgcCoinVec.begin();
  }   
  inline MuonCalibRawHitCollection::MuonCalibRawTgcCoinVecIt MuonCalibRawHitCollection::rawTgcCoinCollectionEnd() {
    return m_rawTgcCoinVec.end();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawTgcCoinVecCit MuonCalibRawHitCollection::rawTgcCoinCollectionBegin() const {
    return m_rawTgcCoinVec.begin();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawTgcCoinVecCit MuonCalibRawHitCollection::rawTgcCoinCollectionEnd() const {
    return m_rawTgcCoinVec.end();
  }
  inline MuonCalibRawHitCollection::MuonCalibRawCscHitVecIt MuonCalibRawHitCollection::rawCscHitCollectionBegin() {
    return m_rawCscHitVec.begin();
  }   
  inline MuonCalibRawHitCollection::MuonCalibRawCscHitVecIt MuonCalibRawHitCollection::rawCscHitCollectionEnd() {
    return m_rawCscHitVec.end();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawCscHitVecCit MuonCalibRawHitCollection::rawCscHitCollectionBegin() const {
    return m_rawCscHitVec.begin();
  }  
  inline MuonCalibRawHitCollection::MuonCalibRawCscHitVecCit MuonCalibRawHitCollection::rawCscHitCollectionEnd() const {
    return m_rawCscHitVec.end();
  }
  inline int MuonCalibRawHitCollection::numberOfMuonCalibRawMdtHits() const {\
    return m_rawMdtHitVec.size();
  }
  inline int MuonCalibRawHitCollection::numberOfMuonCalibRawRpcHits() const {
    return m_rawRpcHitVec.size();
  }
  inline int MuonCalibRawHitCollection::numberOfMuonCalibRawTgcHits() const {
    return m_rawTgcHitVec.size();
  }
  inline int MuonCalibRawHitCollection::numberOfMuonCalibRawTgcCoins() const {
    return m_rawTgcCoinVec.size();
  }
  inline int MuonCalibRawHitCollection::numberOfMuonCalibRawCscHits() const {
    return m_rawCscHitVec.size();
  }
  
  inline void MuonCalibRawHitCollection::addMuonCalibRawHit( MuonCalibRawMdtHit *rawHit ) { 
    if(rawHit){
      m_rawMdtHitVec.push_back(rawHit); 
    }
  }  

  inline void MuonCalibRawHitCollection::addMuonCalibRawHit( MuonCalibRawRpcHit *rawHit ) { 
    if(rawHit){
      m_rawRpcHitVec.push_back(rawHit); 
    }
  }  

  inline void MuonCalibRawHitCollection::addMuonCalibRawHit( MuonCalibRawTgcHit *rawHit ) { 
    if(rawHit){
      m_rawTgcHitVec.push_back(rawHit); 
    }
  }  

  inline void MuonCalibRawHitCollection::addMuonCalibRawHit( MuonCalibRawTgcCoin *rawHit ) { 
    if(rawHit){
      m_rawTgcCoinVec.push_back(rawHit); 
    }
  }  

  inline void MuonCalibRawHitCollection::addMuonCalibRawHit( MuonCalibRawCscHit *rawHit ) { 
    if(rawHit){
      m_rawCscHitVec.push_back(rawHit); 
    }
  }

}  //namespace MuonCalib

#endif
