/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamFractionContainer.h
 * header file for SCT byte stream fractions object
 * @author Susumu Oda - 2018-03-06
 **/

#ifndef SCT_ByteStreamFractionContainer_h
#define SCT_ByteStreamFractionContainer_h

#include "InDetByteStreamErrors/SCT_ByteStreamFraction.h"

// Include Athena stuff
#include "AthenaKernel/CLASS_DEF.h"

class SCT_ByteStreamFractionContainer {
public:

  // Constructor
  SCT_ByteStreamFractionContainer();

  // Destructor
  virtual ~SCT_ByteStreamFractionContainer() = default;

  enum Type {FirstIndex=0, HVOn=0, SimulatedData, CondensedMode, SuperCondensedMode, NumberOfTypes};

  // Insert a new rodId with a bool value
  void insert(const Type type, const uint32_t rodId, const bool value);

  // Clear m_trueSets and m_falseSets
  void clear();

  // Get majority
  bool majority(const Type type) const;

  // Set true set
  void setTrueSet(const Type type, const std::set<uint32_t> trueSet);

  // Set false set
  void setFalseSet(const Type type, const std::set<uint32_t> falseSet);

  // Get fraction
  SCT_ByteStreamFraction getFraction(const Type type) const;

private:
  // Store RODIDs with true and false separately
  SCT_ByteStreamFraction m_fraction[NumberOfTypes];

};

CLASS_DEF( SCT_ByteStreamFractionContainer , 1200846985 , 1 )

#endif // SCT_ByteStreamFractionContainer_h
