/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6COLDTCMod0CalibCalculator.h"
#include "LArG4Code/ILArCalculatorSvc.h"
#include "G4Step.hh"

LArG4H6COLDTCMod0CalibCalculator::LArG4H6COLDTCMod0CalibCalculator(const std::string& name, ISvcLocator* pSvcLocator)
: LArCalibCalculatorSvcImp(name, pSvcLocator)
, m_Calculator("LArG4H6COLDTCMod0Calculator", name)
{
  declareProperty("Calculator", m_Calculator);
}

StatusCode LArG4H6COLDTCMod0CalibCalculator::initialize()
{
  // Initialize the geometry calculator.
  ATH_CHECK(m_Calculator.retrieve());
  return StatusCode::SUCCESS;
}

LArG4H6COLDTCMod0CalibCalculator::~LArG4H6COLDTCMod0CalibCalculator(){}


G4bool LArG4H6COLDTCMod0CalibCalculator::Process( const G4Step* a_step, LArG4Identifier & identifier,
                                                  std::vector<G4double> & energies, const LArG4::eCalculatorProcessing a_process ) const
{

  energies.clear();
  if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyEnergy )
    {
      m_energyCalculator.Energies( a_step, energies );
    }
  else
    for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );


  identifier = LArG4Identifier();
  if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyID )
    {
      // Calculate the identifier.
      std::vector<LArHitData> hdata;
      if(m_Calculator->Process(a_step, hdata))
        {
          if(hdata.size()>0) identifier = hdata[0].id;
        }
    }

  // Check for bad result.
  if ( identifier == LArG4Identifier() ) {
    std::cout<<"LArG4H6COLDTCMod0CalibCalculator::Process return false !!!"<<std::endl;
    return false;
  }

  return true;
}
