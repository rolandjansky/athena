/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloJetElementFillerTool.cxx
*/

#include "L1CaloJetElementFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

  L1CaloJetElementFillerTool::L1CaloJetElementFillerTool(  const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent )
  : BlockFillerTool< LVL1::JetElement >( type, name, parent )
  {
    book().ignore(); // Avoid coverity warnings.
  }

  StatusCode L1CaloJetElementFillerTool::book(){

    CHECK( addVariable ("eta",m_eta) );
    CHECK( addVariable ("l1caloPhi",m_l1caloPhi) );
    CHECK( addVariable ("atlasPhi",m_atlasPhi) );
    CHECK( addVariable ("emEnergy",m_emEnergy) );
    CHECK( addVariable ("hadEnergy",m_hadEnergy) );
    CHECK( addVariable ("energy",m_energy) );
    CHECK( addVariable ("isSaturated",m_isSaturated) );
    CHECK( addVariable ("isEmSaturated",m_isEmSaturated) );
    CHECK( addVariable ("isHadSaturated",m_isHadSaturated) );
    CHECK( addVariable ("emError",m_emError) );
    CHECK( addVariable ("hadError",m_hadError) );
    CHECK( addVariable ("linkError",m_linkError) );
    CHECK( addVariable ("peak",m_peak) );
    CHECK( addVariable ("emEnergyVec",m_emEnergyVec) );
    CHECK( addVariable ("hadEnergyVec",m_hadEnergyVec) );
    CHECK( addVariable ("emErrorVec",m_emErrorVec) );
    CHECK( addVariable ("hadErrorVec",m_hadErrorVec) );
    CHECK( addVariable ("linkErrorVec",m_linkErrorVec) );

    return StatusCode::SUCCESS;
  }

  StatusCode L1CaloJetElementFillerTool::fill( const LVL1::JetElement& p ){

    *m_eta = p.eta();
    *m_l1caloPhi = p.phi();
    *m_atlasPhi = this->AtlasPhi( p.phi() );
    *m_emEnergy = p.emEnergy();
    *m_hadEnergy = p.hadEnergy();
    *m_energy = p.energy();
    *m_emError = p.emError();
    *m_hadError = p.hadError();
    *m_linkError = p.linkError();
    *m_peak = p.peak();
    *m_emEnergyVec = p.emEnergyVec();
    *m_hadEnergyVec = p.hadEnergyVec();
    *m_emErrorVec = p.emErrorVec();
    *m_hadErrorVec = p.hadErrorVec();
    *m_linkErrorVec = p.linkErrorVec();

    unsigned int sat(0),emsat(0),hadsat(0);
    if( p.isSaturated() == true ){sat = 1;}
    if( p.isEmSaturated() == true ){emsat = 1;}
    if( p.isHadSaturated() == true ){hadsat = 1;}

    *m_isSaturated = sat;
    *m_isEmSaturated = emsat;
    *m_isHadSaturated = hadsat;

    return StatusCode::SUCCESS;
  }

  float L1CaloJetElementFillerTool::AtlasPhi(const float& phi){
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
