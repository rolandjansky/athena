/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_MDTCSM_H
#define MUONRDO_MDTCSM_H

#include "MuonRDO/MdtAmtHit.h"
#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include<stdint.h>

/** MDT RDOs : Chamber Service Module, container of AmtHits of a single Mdt chamber
  @author Stefano Rosati Feb 2003*/
class MdtCsm : public DataVector<MdtAmtHit>
{

 private:

  Identifier m_Id;
  IdentifierHash m_idHash; //!< Offline idenfifier hash corresponding to this collection.
  uint16_t m_SubDetId;   //!< Sub-detector identifier
  uint16_t m_MrodId;     //!< Mrod number
  uint16_t m_CsmId;      //!< Csm link number
  
 public:
  //typedef 
  typedef Identifier ID; 
  typedef MdtAmtHit DIGIT; 
  
  friend class MdtCsmCnv_p1;
  
  /** Default constructor*/
  MdtCsm();

  /** Constructor of an empty container, with an identifier & hash to identify 
  the collection*/
  MdtCsm(const Identifier  id, const IdentifierHash idHash);

  /** Full constructor*/
  MdtCsm(const Identifier  id, const IdentifierHash idHash, uint16_t SubDetId, uint16_t MrodId, uint16_t CsmId);

  virtual ~MdtCsm() { };

  void set_values(const Identifier id, const IdentifierHash idHash, uint16_t SubDetId, uint16_t MrodId, uint16_t CsmId); 

  /** Returns the CSM offline identifier (chamber offline id) */
  Identifier identify() const {return m_Id;}
  
  /** Returns the CSM offline identifier hash (chamber offline id) */
  IdentifierHash identifyHash() const {return m_idHash;}
  
  /** Returns the sub-detector Id*/
  uint16_t SubDetId() const {return m_SubDetId;}
  /** Returns the MROD id from the CSM header*/
  uint16_t MrodId() const {return m_MrodId;}
  /** Returns the CSM online id (online identifier inside a MROD)*/
  uint16_t CsmId() const {return m_CsmId;}
  
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const MdtCsm& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const MdtCsm& coll);


CLASS_DEF(MdtCsm, 4188, 1)

// Class needed only for persistency
typedef DataVector<MdtCsm> MDT_CSM_vector;
CLASS_DEF( MDT_CSM_vector, 4180, 1 )

#endif   // MUONRDO_MDTCSM_H

