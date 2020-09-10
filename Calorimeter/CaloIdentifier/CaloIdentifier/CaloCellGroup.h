/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCELLGROUP_H
#define CALOCELLGROUP_H 
#include "GaudiKernel/MsgStream.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include <bitset>

class CaloCellGroup {

 public:
  CaloCellGroup();

  //Format: 
  //[EMEC,EMB/A/Layer/Region/Eta/Phi] value1, value2, ...
  bool setDefinition(const CaloCell_ID* caloCellId, const std::string& definition, MsgStream& logStr);
  bool inGroup(const Identifier id) const;
  const std::vector<float>& getValue() const {return m_values;}

  void printDef() const;


 private:
  void complain(MsgStream&,const std::string&) const;

  enum Fields {
    LAYER,
    REGION,
    ETA,
    PHI,
    NFIELDS
  };

  /*
  enum Calo {
    EMB=0,
    EMEC,
    HEC,
    FCAL
  };
  
  //Group defintion:
  std::bitset<4> m_subCalos;
  //std::bitset<CaloCell_ID:SUBCALO> m_subCalos;
  */


  bool m_pos,m_neg;
  bool m_emb,m_emecow,m_emeciw,m_hec,m_fcal;
  std::vector<std::pair<int,int> > m_fieldBoundaries;
  std::vector<float> m_values;

  const CaloCell_ID *m_caloCellId;
  
};

class CaloCellGroupList {
 public:
  CaloCellGroupList():m_initialized(false) {};

  //Convention: First element of vector has default values, following elements as defined above
  bool setDefinition(const CaloCell_ID* caloCellId, const std::vector<std::string>& definitions, MsgStream& logStr);

  void dump(const CaloCell_ID* caloCellId); // for debugging

   void printDef() const;
  
  const std::vector<float>& valuesForCell(const Identifier id);

  const std::vector<float>& getDefaults() const {return m_defaults; }

 private:
  std::vector<CaloCellGroup> m_groups;
  std::vector<float> m_defaults;
  bool m_initialized;
};


#endif
