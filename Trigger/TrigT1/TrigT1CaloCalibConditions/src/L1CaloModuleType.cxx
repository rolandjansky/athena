/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloModuleType.h"

/*!
 *  \class L1CaloModuleType
 *  Infrastructure class encapsulating our known module types.
 *
 *  This class is a very close adaptation of the online infraL1Calo/L1CaloModuleType class
 */

//==============================================================================
// constructor
//==============================================================================
/*!
 *  Construct a L1CaloModuleType given its type value.
 */
L1CaloModuleType::L1CaloModuleType(const L1CaloModuleEnum mEnum)
{
  m_type = mEnum;
}

//==============================================================================
// constructor
//==============================================================================
/*!
 *  Construct a L1CaloModuleType given its type name.
 */
L1CaloModuleType::L1CaloModuleType(const std::string& mTypeName)
{
  if      (mTypeName == "Cam")   { m_type = Cam; }
  else if (mTypeName == "Cmm") { m_type = Cmm; }
  else if (mTypeName == "Cpm")   { m_type = Cpm; }
  else if (mTypeName == "CtpEmulation")   { m_type = CtpEmulation; }
  else if (mTypeName == "Jem")   { m_type = Jem; }
  else if (mTypeName == "L1CaloBusy") { m_type = L1CaloBusy; }
  else if (mTypeName == "L1CaloLtp")   { m_type = L1CaloLtp; }
  else if (mTypeName == "L1CaloLtpi") { m_type = L1CaloLtpi; }
  else if (mTypeName == "L1CaloRod")  { m_type = L1CaloRod; }
  else if (mTypeName == "L1CaloRosInterface") { m_type = L1CaloRosInterface; }
  else if (mTypeName == "Lrm")   { m_type = Lrm; }
  else if (mTypeName == "Lsm")   { m_type = Lsm; }
  else if (mTypeName == "Ppm")   { m_type = Ppm; }
  else if (mTypeName == "Tcm")   { m_type = Tcm; }
  else if (mTypeName == "Ttcvi")   { m_type = Ttcvi; }
  else                           { m_type = None; }
}

//==============================================================================
// copy constructor
//==============================================================================
/*!
 *  Copy a L1CaloModuleType.
 */
L1CaloModuleType::L1CaloModuleType(const L1CaloModuleType& mType)
{
  m_type = mType.moduleEnum();
}

//==============================================================================
// assignment operator
//==============================================================================
/*!
 *  Assign a L1CaloModuleType.
 */
L1CaloModuleType&
L1CaloModuleType::operator=(const L1CaloModuleType& mType)
{
  if (this != &mType) m_type = mType.moduleEnum();
  return *this;
}

//==============================================================================
// toString
//==============================================================================
/*!
 *  Compare with type value.
 */
const std::string
L1CaloModuleType::toString() const
{
  std::string s;
  switch (this->moduleEnum()) {
    case Cam:   s = "Cam"; break;
    case Cmm: s = "Cmm"; break;
    case Cpm:   s = "Cpm"; break;
    case CtpEmulation:   s = "CtpEmulation"; break;
    case Jem:   s = "Jem"; break;
    case L1CaloBusy: s = "L1CaloBusy"; break;
    case L1CaloLtp:   s = "L1CaloLtp"; break;
    case L1CaloLtpi: s = "L1CaloLtpi"; break;
    case L1CaloRod:  s = "L1CaloRod"; break;
    case L1CaloRosInterface: s = "L1CaloRosInterface"; break;
    case Lrm:   s = "Lrm"; break;
    case Lsm:   s = "Lsm"; break;
    case Ppm:   s = "Ppm"; break;
    case Tcm:   s = "Tcm"; break;
    case Ttcvi:   s = "Ttcvi"; break;
    default:    s = "Unknown"; break;
  }
  return s;
}

//==============================================================================
// operator==
//==============================================================================
/*!
 *  Compare with type value.
 */
bool
L1CaloModuleType::operator==(const L1CaloModuleEnum mEnum) const
{
  return (this->moduleEnum() == mEnum);
}

//==============================================================================
// operator==
//==============================================================================
/*!
 *  Compare with another L1CaloModuleType.
 */
bool
L1CaloModuleType::operator==(const L1CaloModuleType& mType) const
{
  return (this->moduleEnum() == mType.moduleEnum());
}

//==============================================================================
// Global operator<<
//==============================================================================
/*!
 *  Write module type to the output stream as a string.
 */
//ostream&
//operator<< (ostream& s, const L1CaloModuleType& mType)
//{
//  s << mType.toString();
//  return s;
//}
