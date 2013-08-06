/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/MdtCsm.h"
#include "GaudiKernel/MsgStream.h"

// Default constructor
MdtCsm::MdtCsm() :
  DataVector<MdtAmtHit>(), 
  m_Id(),
  m_idHash(),
  m_SubDetId(0),
  m_MrodId(0),
  m_CsmId(0)
{ }

// Partialconstructor
MdtCsm::MdtCsm(const Identifier id, const IdentifierHash idHash) :
  DataVector<MdtAmtHit>(), 
  m_Id(id),
  m_idHash(idHash),
  m_SubDetId(0),
  m_MrodId(0),
  m_CsmId(0)
{ }

// Full constructor
MdtCsm::MdtCsm(const Identifier id, const IdentifierHash idHash, uint16_t SubDetId, uint16_t MrodId, uint16_t CsmId) :
  DataVector<MdtAmtHit>(),
  m_Id(id),
  m_idHash(idHash),
  m_SubDetId(SubDetId),
  m_MrodId(MrodId),
  m_CsmId(CsmId)
{ }


void MdtCsm::set_values(const Identifier id, const IdentifierHash idHash, uint16_t SubDetId, uint16_t MrodId, uint16_t CsmId)
{
  m_Id = id;
  m_idHash = idHash;
  m_SubDetId = SubDetId;
  m_MrodId = MrodId;
  m_CsmId = CsmId;
}  


MsgStream& operator << ( MsgStream& sl, const MdtCsm& coll) 
{
    sl << "MdtCsm: "
     << "identify()="<< coll.identify().getString()
     << ", SubDetId()="<< coll.SubDetId()
     << ", MrodId()="<< coll.MrodId()
     << ", CsmId()="<< coll.CsmId()<<" MdtAmtHit=[";
    MdtCsm::const_iterator it = coll.begin();
    MdtCsm::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
}


std::ostream& operator << ( std::ostream& sl, const MdtCsm& coll) 
{
    sl << "MdtCsm: "
    << "identify()="<< coll.identify().getString()
    << ", SubDetId()="<< coll.SubDetId()
    << ", MrodId()="<< coll.MrodId()
    << ", CsmId()="<< coll.CsmId()<<" MdtAmtHit=[";
    MdtCsm::const_iterator it = coll.begin();
    MdtCsm::const_iterator itEnd = coll.end();
    for (;it!=itEnd;++it) sl<< (**it)<<", ";
    sl <<" ]"<<std::endl;
    return sl;
}
