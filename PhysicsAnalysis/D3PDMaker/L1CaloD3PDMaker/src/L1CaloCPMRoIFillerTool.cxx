/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1CaloCPMRoIFillerTool.cxx
*/

#include "L1CaloCPMRoIFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"

namespace D3PD {

  L1CaloCPMRoIFillerTool::L1CaloCPMRoIFillerTool(  const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent )
  : BlockFillerTool< LVL1::CPMRoI >( type, name, parent ),
  m_emTauTool("LVL1::L1EmTauTools/L1EmTauTools")
  {
    declareProperty("L1EmTauTool",m_emTauTool);
    declareProperty( "CPMTowersLocation", m_CPMTowersLocation = "CPMTowers" );
    
    book().ignore(); // Avoid coverity warnings.
  }


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode L1CaloCPMRoIFillerTool::initialize()
  {
    CHECK( m_emTauTool.retrieve() );
    return StatusCode::SUCCESS;
  }


  StatusCode L1CaloCPMRoIFillerTool::book()
  {
    CHECK( addVariable ("crate",m_crate) );
    CHECK( addVariable ("cpm",m_cpm) );
    CHECK( addVariable ("chip",m_chip) );
    CHECK( addVariable ("location",m_location) );
    CHECK( addVariable ("hits",m_hits) );
    CHECK( addVariable ("error",m_error) );
    CHECK( addVariable ("roiWord",m_roiWord) );
    CHECK( addVariable ("eta",m_eta) );
    CHECK( addVariable ("l1caloPhi",m_l1caloPhi) );
    CHECK( addVariable ("atlasPhi",m_atlasPhi) );

    CHECK( addVariable ("core",m_core) );
    CHECK( addVariable ("emClus",m_emClus) );
    CHECK( addVariable ("tauClus",m_tauClus) );
    CHECK( addVariable ("emIsol",m_emIsol) );
    CHECK( addVariable ("hadIsol",m_hadIsol) );
    CHECK( addVariable ("hadVeto",m_hadVeto) );
    CHECK( addVariable ("isEtMax",m_isEtMax) );

    return StatusCode::SUCCESS;
  }

 StatusCode L1CaloCPMRoIFillerTool::fill( const LVL1::CPMRoI& p ){

    *m_crate     = p.crate();
    *m_cpm       = p.cpm();
    *m_chip      = p.chip();
    *m_location  = p.location();
    *m_hits      = p.hits();
    *m_error     = p.error();
    *m_roiWord   = p.roiWord();

    LVL1::CPRoIDecoder decoder;
    const LVL1::CoordinateRange coord( decoder.coordinate( p.roiWord() ) );
    
    *m_eta       = coord.eta();
    *m_l1caloPhi = coord.phi();
    *m_atlasPhi  = this->AtlasPhi( coord.phi() );

    // If there are no CPMTowers, set values to zero and exit gracefully
    if( ! evtStore()->contains< CPMTowerCollection >( m_CPMTowersLocation ) ){
      *m_core = 0;
      *m_emClus = 0;
      *m_tauClus = 0;
      *m_emIsol = 0;
      *m_hadIsol = 0;
      *m_hadVeto = 0;
      *m_isEtMax = 0;

      return 0;
    }
    
    const DataVector<LVL1::CPMTower>* storedCPMTs = 0;
    std::map<int, LVL1::CPMTower*> cpmtowers;
    if( evtStore()->retrieve( storedCPMTs, m_CPMTowersLocation ).isFailure() ){
      REPORT_ERROR( StatusCode::FAILURE )
      << "CPM Towers could not be found with key: "
      << m_CPMTowersLocation;
      return 0;
    }

    m_emTauTool->mapTowers(storedCPMTs, &cpmtowers);
    LVL1::CPAlgorithm simRoI = m_emTauTool->findRoI(coord.eta(), coord.phi(), &cpmtowers);
    
    *m_core = simRoI.Core();
    *m_emClus = simRoI.EMClus();
    *m_tauClus = simRoI.TauClus();
    *m_emIsol = simRoI.EMIsol();
    *m_hadIsol = simRoI.HadIsol();
    *m_hadVeto = simRoI.HadVeto();

    if( simRoI.isEtMax() ){
      *m_isEtMax = 1;
    }
    else{
      *m_isEtMax = 0;
    }
    return StatusCode::SUCCESS;
  }

  float L1CaloCPMRoIFillerTool::AtlasPhi(const float& phi){
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
