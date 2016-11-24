/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcPad.h

#ifndef RpcPadH
#define RpcPadH

// M.Grothe, 28/11/02
// K. Assamagan 07/12/02

#include "MuonRDO/RpcCoinMatrix.h"
#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

typedef unsigned short int ubit16;

class RpcPad : public DataVector<RpcCoinMatrix> {

public:  // functions

  //typedef 
  typedef Identifier ID; 
  typedef RpcCoinMatrix DIGIT; 

  // Constructors.

  /** Default constructor */
  RpcPad ()
    : DataVector<RpcCoinMatrix>(), m_id(0), m_idHash(0), 
    m_onlineId(0), m_lvl1Id(0), m_bcId(0), m_status(0), m_errorCode(0), m_sector(0)
  {
  // std::cout<<"RpcPad default ctor ["<<this<<"]"<<std::endl;

}

  /** Creates an empty container ready for writing.
   * constructor to be used to identified the collection (pad)
   * to be filled in bytestream*/

  RpcPad(const Identifier& id, const IdentifierHash& idHash) 
    : DataVector<RpcCoinMatrix>(), m_id(id), m_idHash(idHash), 
    m_onlineId(0), m_lvl1Id(0), m_bcId(0), m_status(0), m_errorCode(0), m_sector(0)
        {
  // std::cout<<"RpcPad ctor 2 ["<<this<<"]"<<std::endl;

 };

  /** full constructor*/
  RpcPad(const Identifier& id, const IdentifierHash& idHash, ubit16 onlineId, ubit16 status,
         ubit16 errorCode, int sector) 
    : DataVector<RpcCoinMatrix>(), m_id(id), m_idHash(idHash),m_onlineId(onlineId),
    m_lvl1Id(0), m_bcId(0),
    m_status(status), m_errorCode(errorCode), m_sector(sector)
    {
  // std::cout<<"RpcPadContainer ctor 3 ["<<this<<"]"<<std::endl;

 };
  
  /** full constructor with Lvl1 and Bcid*/
  RpcPad(const Identifier& id, const IdentifierHash& idHash, ubit16 onlineId, 
  	 ubit16 lvl1Id, ubit16 bcId, ubit16 status,
	 ubit16 errorCode, int sector) 
    : DataVector<RpcCoinMatrix>(), m_id(id), m_idHash(idHash),m_onlineId(onlineId),
    m_lvl1Id(lvl1Id), m_bcId(bcId),
    m_status(status), m_errorCode(errorCode), m_sector(sector)
    {
  // std::cout<<"RpcPadContainer ctor 4 ["<<this<<"]"<<std::endl;

 };
  
  /** partial contructor: needed when filling pad from bytestream
    * errorCode not know until one is reading the pad footer!*/
  RpcPad(const Identifier& id, const IdentifierHash& idHash, ubit16 onlineId, 
         int sector, ubit16 status) 
    : DataVector<RpcCoinMatrix>(), m_id(id), m_idHash(idHash),m_onlineId(onlineId),
    m_lvl1Id(0), m_bcId(0),
    m_status(status), m_errorCode(0), m_sector(sector)
    { 
  // std::cout<<"RpcPadContainer ctor 5 ["<<this<<"]"<<std::endl;

};
  
  // virtual destructor for persistency
  virtual ~RpcPad() {
  // std::cout<<"RpcPadContainer dtor ["<<this<<"]"<<std::endl;
  // for( auto it : this->stdcont() ) {
  //   for (auto it2 : it->stdcont() ) {
  //     delete it2;
  //   }
  //   delete it;
  // }
}
  
  Identifier identify() const { return m_id; }
  
  /** Returns the OFFLINE identifier hash for this collection*/
  IdentifierHash identifyHash() const {return m_idHash;}
  
  ubit16 status() const { return m_status; }
  ubit16 errorCode() const { return m_errorCode; }
  ubit16 onlineId() const {return m_onlineId; }

  ubit16 lvl1Id() const {return m_lvl1Id; }
  ubit16 bcId() const {return m_bcId; }

  int sector() const {return m_sector; }

  // This method to be used with the second constructor above
  void setErrorCode (ubit16 errorCode) { m_errorCode = errorCode;}
  void setOnlineId  (ubit16 onlineId)  { m_onlineId = onlineId;}
  void setBcId      (ubit16 bcId)      { m_bcId = bcId;}
  void setLvl1Id    (ubit16 lvl1Id)    { m_lvl1Id = lvl1Id;}

  void setStatus    (ubit16 status)    { m_status    = status;}
  void setSector    (int sector)       { m_sector    = sector;}

private:
  friend class RpcPadContainerCnv_p1;
  friend class RpcPadCnv_p1;

  Identifier m_id;   
  IdentifierHash m_idHash; 
  ubit16 m_onlineId;

  ubit16 m_lvl1Id;
  ubit16 m_bcId;

  ubit16 m_status;
  ubit16 m_errorCode;
  int m_sector;

};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const RpcPad& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const RpcPad& coll);


CLASS_DEF(RpcPad, 4189, 1)

// Class needed only for persistency
typedef DataVector<RpcPad> RPC_PAD_vector;
CLASS_DEF( RPC_PAD_vector, 4181, 1 )

#endif




