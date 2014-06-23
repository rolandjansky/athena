/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCellCollection_H
#define LArCellCollection_H

#include "Identifier/HWIdentifier.h"
#include "LArRecEvent/LArCell.h"
#include "DataModel/DataVector.h" 
#include "CLIDSvc/CLASS_DEF.h"

#include "Identifier/IdentifierHash.h" 
#include "LArRecEvent/LArFebEnergy.h" 

#include <map>
#include <vector>

/** 
    @class LArCellCollection
    @brief Container Class for LArCell in a ROB used by EF 

    It is meant to store LArCell in Store Gate

    @author Hong Ma
 */

class LArCellCollection : public DataVector<LArCell> {

 public:

  /** @brief required for a Digit Collection */
  typedef HWIdentifier  ID; 
  typedef unsigned int TT_ID; 
  typedef LArCell OBJECT ; 

  /** @brief construct a container for a given Front End Board */
  LArCellCollection ( const ID& id ) :
	DataVector<LArCell>(SG::VIEW_ELEMENTS), m_id(id) { };

  /** @brief destructor */
  virtual ~LArCellCollection() 
  { 
  };

  /** @brief return identity of the FEB corresponding to this container */
  ID  identify() const { return m_id;} 

  /** @brief set the TriggerTower */
  void setTT(const TT_ID& id, 
		    const_iterator b, const_iterator e); 

  /** @brief get the TriggerTower */
  void getTT(const TT_ID& id, const_iterator& b,
	       const_iterator& e) const ; 

  /** @brief set the feb energy */
  void addfebenergy(LArFebEnergy& larfebenergy){
            m_febE.push_back(larfebenergy);
  }

 private: 

  /** @brief internal map for TTs */ 
  ID  m_id;

  /** @brief iterator pair for begin and end */
  typedef std::pair<const_iterator,const_iterator> ITPAIR; 
  /** @brief ID to Iterator Map */
  typedef std::map<TT_ID,ITPAIR> ID_IT_MAP; 

  /** @brief vector of FEB energies */ 
  std::vector<LArFebEnergy> m_febE;

  /** @brief ID to Iterator Map */ 
  ID_IT_MAP  m_Map; 
};

CLASS_DEF(LArCellCollection,2724,0) 

#endif

