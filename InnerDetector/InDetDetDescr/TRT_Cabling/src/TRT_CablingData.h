/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// TRT_CablingData class - class for TRT Mapping storing
//    A.Zalite
//
#ifndef TRT_CABLINGDATA_H
#define TRT_CABLINGDATA_H

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include <map>
#include <vector>
#include <stdint.h>

class TRT_CablingData{

public:
    //Constructor
  TRT_CablingData();

    //Destructor
  virtual ~TRT_CablingData();
  
    // Initialize m_identifierForAllStraws with zeros (TB case)
  void zero_identifierForAllStraws(int rod, const std::vector<Identifier>& tempbuff);  
  
    // Initialize m_identifierHashForAllStraws with zeros (TB case)
  void zero_identifierHashForAllStraws(int rod, const std::vector<IdentifierHash>& tempbuff);
  
    // Set value of Identifier for each straw (TB case)
  void set_identifierForAllStraws(int rod, int bufferPosition, 
    Identifier strawID);

    // Set value of Identifier for each straw (DC1, DC2)
  void set_identifierForAllStraws(Identifier strawID);

    // Set value of IdentifierHash for each straw (TB case)
  void set_identifierHashForAllStraws(int rod, int bufferPosition, 
    IdentifierHash hashId);

    // Set value of IdentifierHash for each straw (DC1, DC2)
  void set_identifierHashForAllStraws(IdentifierHash hashId);

    // Get value of Identifier for each straw (TB case)
  Identifier get_identifierForAllStraws(int rod, int bufferPosition) const;
  
    // Get value of IdentifierHash for each straw (TB case)
  IdentifierHash get_identifierHashForAllStraws(int rod, int bufferPosition) const;
    
    // Get value of Identifier for each straw (DC1, DC2)
  Identifier get_identifierForAllStraws(int shift) const;

    // Get value of IdentifierHash for each straw (DC1, DC2)
  IdentifierHash get_identifierHashForAllStraws(int shift) const;

  // Get buffer offset from Identifier
  uint32_t get_BufferOffset( Identifier StrawId );


    // Add collection IDs for ROB
  void add_collID(uint32_t rob_id, std::vector<IdentifierHash> * vectID);
     
    // Get Identifiers of Collections for the given ROBID
  const std::vector<IdentifierHash>& get_collID(int shift);

    // Add SourceID 
  void add_allRobs(uint32_t sidROB);

    // Get all RODIDs for TRT
  const std::vector<uint32_t>& get_allRods() const;

private:
    // Source ID for all robs
  std::vector<uint32_t> m_allRobs;

    // Identifiers for all straws
  std::vector<Identifier> m_identifierForAllStraws;

    // Hashed Identifiers for all straws;
  std::vector<IdentifierHash> m_identifierHashForAllStraws;

  // Buffer Offsets for all Identifiers;
  struct idpair
  {
    idpair (const Identifier& a, uint32_t b)
      : first (a), second (b) {}
    bool operator< (const idpair& other) const { return first < other.first; }
    bool operator< (const Identifier& other) const { return first < other; }
    Identifier first;
    uint32_t second;
  };
  std::vector<idpair> m_BufferOffsetForAllIdentifiers;
  bool m_bofai_sorted;


    // Identifiers for all Collections for RODs
  std::map< uint32_t, std::vector<IdentifierHash> *> m_collID;

  typedef std::pair<Identifier, IdentifierHash> idandhash_t;
  typedef std::vector<idandhash_t> offsetvec_t;
  typedef std::vector<offsetvec_t> rodvec_t;
  typedef std::map<unsigned int, rodvec_t> rodmap_t;
  rodmap_t m_rodoffset_to_id;

  idandhash_t& find_idandhash (int rod, int bufferPosition);
  const idandhash_t& find_idandhash (int rod, int bufferPosition) const;
};

#endif // TRT_CABLINGDATA_H
