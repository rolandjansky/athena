/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOMODULETYPE_H
#define L1CALOMODULETYPE_H

#include <iostream>
#include <string>

class L1CaloModuleType final {
public:

  // Values: L1Calo trigger modules, common CERN modules and test modules.
  enum L1CaloModuleEnum {
    None =  0,
    Ppm = 1, Cpm=2,Jem=3,Cmm=4,Tcm=5,L1CaloRod=6,Cam=7,CtpEmulation,Dss,L1CaloBusy,L1CaloLtp,L1CaloLtpi,L1CaloRosInterface,Lrm,Lsm,Ttcvi
  };

  // Construction
  L1CaloModuleType(const L1CaloModuleEnum  mEnum);
  L1CaloModuleType(const std::string& mTypeName);
  L1CaloModuleType(const L1CaloModuleType& mType);
  L1CaloModuleType& operator=(const L1CaloModuleType& mType);
  ~L1CaloModuleType() = default;

  // Information
  const std::string toString() const;

  // Comparison
  bool operator==(const L1CaloModuleEnum  mEnum) const;
  bool operator==(const L1CaloModuleType& mType) const;
  bool operator!=(const L1CaloModuleEnum  mEnum) const { return !((*this) == mEnum); }
  bool operator!=(const L1CaloModuleType& mType) const { return !((*this) == mType); }

  operator unsigned int() {return (unsigned int) this->moduleEnum();};

private:
  // Access to data members
  L1CaloModuleEnum moduleEnum() const { return m_type; }

  // Data members
  L1CaloModuleEnum m_type;
};

// Global operator
//ostream& operator<< (ostream& s, const L1CaloModuleType& m);

#endif  // L1CALOMODULETYPE_H
