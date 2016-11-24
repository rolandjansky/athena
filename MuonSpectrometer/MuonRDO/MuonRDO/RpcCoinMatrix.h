/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RpcCoincMatrix.h

#ifndef RpcCoinMatrixH
#define RpcCoinMatrixH


// M.Grothe, 28/11/02
// K. Assamagan 06/12/02

#include "DataModel/DataVector.h"
#include "MuonRDO/RpcFiredChannel.h"
#include "Identifier/Identifier.h"

typedef unsigned short int ubit16;

class RpcCoinMatrix : public DataVector<RpcFiredChannel> {

 private:

  Identifier m_id;
  ubit16 m_onlineId;
  ubit16 m_crc;
  ubit16 m_fel1Id;
  ubit16 m_febcId;

 public:
	 friend class RpcCoinMatrixCnv_p1;
  // default constructor
  RpcCoinMatrix() 
    : DataVector<RpcFiredChannel>(), m_id(), m_onlineId(0), m_crc(0),
    m_fel1Id(0), m_febcId(0) { 
		//std::cout<<"RpcCoinMatrix ctor 0 ["<<this<<"]"<<std::endl; 
	};

  // Partial constructor.
  RpcCoinMatrix(const Identifier& id, ubit16 onlineId, 
                 ubit16 fel1Id, ubit16 febcId)
    : DataVector<RpcFiredChannel>(), m_id(id), m_onlineId(onlineId), m_crc(0),
    m_fel1Id(fel1Id), m_febcId(febcId) { 
		// std::cout<<"RpcCoinMatrix ctor 1 ["<<this<<"]"<<std::endl;
	};

  // Full constructor.
  RpcCoinMatrix(const Identifier& id, ubit16 onlineId, 
                 ubit16 crc, ubit16 fel1Id, 
                 ubit16 febcId)
    : DataVector<RpcFiredChannel>(), m_id(id), m_onlineId(onlineId),
    m_crc(crc), m_fel1Id(fel1Id), m_febcId(febcId) { 
		// std::cout<<"RpcCoinMatrix ctor 2 ["<<this<<"]"<<std::endl;
	};

  // virtual destructor for persistency
  virtual ~RpcCoinMatrix() { 
	  // std::cout<<"RpcCoinMatrix dtor ["<<this<<"]"<<std::endl;
  }
  
  // return the pad Identifier of this CM
  Identifier identify() const { return m_id;}

  // return the online identifier
  ubit16 onlineId() const {return m_onlineId; }

  ubit16 crc() const {return m_crc; }

  ubit16 fel1Id() const {return m_fel1Id; }

  ubit16 febcId() const {return m_febcId; }

  void setCRC(ubit16 crc) { m_crc = crc;}
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const RpcCoinMatrix& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const RpcCoinMatrix& coll);

#endif






