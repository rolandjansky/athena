/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASTSIMULATION_FASTCALOSIMTOOL_H
#define G4FASTSIMULATION_FASTCALOSIMTOOL_H

/* Fast simulation base include */
#include "G4AtlasTools/FastSimulationBase.h"
/* FastCaloSim parametrization service include */
#include "ISF_FastCaloSimInterfaces/IFastCaloSimParamSvc.h"
/* FastCaloSim calorimeter extrapolation include */
#include "ISF_FastCaloSimParametrization/IFastCaloSimCaloExtrapolation.h"
/* Random generator service include */
#include "AthenaKernel/IAthRNGSvc.h"

class G4VFastSimulationModel;

class FastCaloSimTool: public FastSimulationBase
{
 public:

  FastCaloSimTool(const std::string& type, const std::string& name, const IInterface *parent);   //!< Default constructor
  ~FastCaloSimTool() {}

  /** Begin of an athena event - do any thing that needs to be done at the beginning of each *athena* event. **/
  virtual StatusCode BeginOfAthenaEvent() override final;
  /** End of an athena event - do any thing that needs to be done at the end of each *athena* event. **/
  virtual StatusCode EndOfAthenaEvent() override final;

protected:
  /** Method to make the actual fast simulation model itself, which
   will be owned by the tool.  Must be implemented in all concrete
   base classes. */
  virtual G4VFastSimulationModel* makeFastSimModel() override final;  
 
 private:
  
  // FastCaloSim service 
  ServiceHandle<ISF::IFastCaloSimParamSvc> m_FastCaloSimSvc{this, "ISF_FastCaloSimV2ParamSvc", "ISF_FastCaloSimV2ParamSvc"};
  // FastCaloSim extrapolation tool
  PublicToolHandle<IFastCaloSimCaloExtrapolation> m_FastCaloSimCaloExtrapolation{this, "FastCaloSimCaloExtrapolation", "FastCaloSimCaloExtrapolation", ""};
  // Random generator service
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc{this, "RandomSvc", "AthRNGSvc", ""};
  // Random generator engine name
  Gaudi::Property<std::string> m_randomEngineName{this, "RandomStream", ""};
  // Name of associated CaloCellContainerSD
  Gaudi::Property<std::string> m_CaloCellContainerSDName{this, "CaloCellContainerSDName", "", "Name of the associated CaloCellContainerSD"};

};

#endif //G4FASTSIMULATION_FASTCALOSIMTOOL_H

