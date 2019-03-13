/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISiPropertiesTool.h
 * @author Susumu.Oda@cern.ch
**/
#ifndef SiPropertiesTool_h
#define SiPropertiesTool_h

#include "SiPropertiesTool/ISiPropertiesTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "SiPropertiesTool/SiliconPropertiesVector.h"
#include "StoreGate/ReadCondHandleKey.h"

/**
 * @class SiPropertiesTool
 * Concrete class for service providing silicon properties (mobility, etc).
 * for each detector element. These depend on conditions such as temperature.
**/
class SiPropertiesTool: public extends<AthAlgTool, ISiPropertiesTool>
{
public:
  SiPropertiesTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SiPropertiesTool() = default;
 
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /// Get properties for the detector element.
  virtual const InDet::SiliconProperties& getSiProperties(const IdentifierHash& elementHash) const override;

private:
  // Properties
  std::string m_detectorName;
  SG::ReadCondHandleKey<InDet::SiliconPropertiesVector> m_propertiesVector;

  static const InDet::SiliconProperties s_defaultProperties;
};

#endif // SiPropertiesTool_h
