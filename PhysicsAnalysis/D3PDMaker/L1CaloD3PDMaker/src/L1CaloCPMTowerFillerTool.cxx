/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloCPMTowerFillerTool.cxx
*/

#include "L1CaloCPMTowerFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {

  L1CaloCPMTowerFillerTool::L1CaloCPMTowerFillerTool(  const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent )
  : BlockFillerTool< LVL1::CPMTower >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloCPMTowerFillerTool::book(){

    CHECK( addVariable ("emEnergy",m_emEnergy) );
    CHECK( addVariable ("hadEnergy",m_hadEnergy) );
    CHECK( addVariable ("emEnergyVec",m_emEnergyVec) );
    CHECK( addVariable ("hadEnergyVec",m_hadEnergyVec) );
    CHECK( addVariable ("peak",m_peak) );
    CHECK( addVariable ("emError",m_emError) );
    CHECK( addVariable ("hadError",m_hadError) );
    CHECK( addVariable ("emErrorVec",m_emErrorVec) );
    CHECK( addVariable ("hadErrorVec",m_hadErrorVec) );      
    CHECK( addVariable ("eta",m_eta) );
    CHECK( addVariable ("l1caloPhi",m_l1caloPhi) );
    CHECK( addVariable ("atlasPhi",m_atlasPhi) );

    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloCPMTowerFillerTool::fill( const LVL1::CPMTower& p ){

    *m_emEnergy      = p.emEnergy();
    *m_hadEnergy     = p.hadEnergy();
    *m_emEnergyVec   = p.emEnergyVec();
    *m_hadEnergyVec  = p.hadEnergyVec();
    *m_peak          = p.peak();
    *m_emError       = p.emError();
    *m_hadError      = p.hadError();
    *m_emErrorVec    = p.emErrorVec();
    *m_hadErrorVec   = p.hadErrorVec();

    *m_eta           = p.eta();
    *m_l1caloPhi     = p.phi();
    *m_atlasPhi      = this->AtlasPhi( p.phi() );


    return StatusCode::SUCCESS;
  }

  float L1CaloCPMTowerFillerTool::AtlasPhi(const float& phi){
    float pi(3.14159265);
    if(phi >= 0.0 && phi < pi ){
      return phi;
    }
    if(phi >= pi){
      return phi - (2.0 * pi);
    }
    return phi;
  }     

}
