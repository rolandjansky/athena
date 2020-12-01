/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4FASTSIMULATION_LARFASTSHOWERTOOL_H
#define LARG4FASTSIMULATION_LARFASTSHOWERTOOL_H

// Base class header
#include "G4AtlasTools/FastSimulationBase.h"

// Member variable headers
#include "GaudiKernel/ServiceHandle.h"
#include "LArG4ShowerLibSvc/ILArG4ShowerLibSvc.h"
#include "FastShowerConfigStruct.h"

// STL headers
#include <string>

/// NEEDS DOCUMENTATION
class LArFastShowerTool: public FastSimulationBase
{
public:

  LArFastShowerTool(const std::string& type, const std::string& name, const IInterface *parent);  //!< Default constructor

  virtual ~LArFastShowerTool() {}                 //!< Default destructor

  StatusCode initialize() override final;

  StatusCode EndOfAthenaEvent() override final;

protected:
  /** Method to make the actual fast simulation model itself, which
   will be owned by the tool.  Must be implemented in all concrete
   base classes. */
  virtual G4VFastSimulationModel* makeFastSimModel() override final;

private:
  std::string                       m_fastSimDedicatedSD; //!< Shower library sensitive detector for this shower
  ServiceHandle<ILArG4ShowerLibSvc> m_showerLibSvc;       //!< Pointer to the shower library service
  FastShowerConfigStruct            m_configuration;
};

#endif //LARG4FASTSIMULATION_LARFASTSHOWERTOOL_H
