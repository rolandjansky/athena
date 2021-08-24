/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// /***************************************************************************
//                           CPCMX.cxx  -  description
//                              -------------------
//     begin                : Mon Jul 28 2014
//     email                : Alan.Watson@CERN.CH
//  ***************************************************************************/
//
//
//================================================
// CPCMX class Implementation
// ================================================
//
//
//

// Utilities
//#include <memory>

// This algorithm includes
#include "CPCMX.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigConfData/L1Threshold.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/CMXCPHits_ClassDEF.h"
#include "TrigT1CaloEvent/CMXCPTob_ClassDEF.h"
#include "TrigT1CaloEvent/CPTopoTOB.h"
#include "TrigT1CaloEvent/CPCMXTopoData_ClassDEF.h"
#include "TrigT1CaloEvent/CPMCMXData.h"

#include "TrigConfL1Data/CTPConfig.h"



namespace LVL1{

using namespace TrigConf;

const int CPCMX::s_SourceLocal;
const int CPCMX::s_SourceTotal;


//--------------------------------
// Constructors and destructors
//--------------------------------

CPCMX::CPCMX
  ( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ) {}


//---------------------------------
// initialise()
//---------------------------------

StatusCode CPCMX::initialize()
{
  ATH_CHECK(m_CMXCPHitsLocation.initialize());
  ATH_CHECK(m_CMXCPTobLocation.initialize());
  ATH_CHECK(m_TopoOutputLocation.initialize());
  ATH_CHECK(m_CTPOutputKey.initialize());
  ATH_CHECK(m_CPMCMXDataLocation.initialize());
  ATH_CHECK( m_L1MenuKey.initialize() );

  return StatusCode::SUCCESS;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode CPCMX::execute( )
{
    
  /*
    The idea is that this thing gets the inputs, formats them as required, then
    uses a port of the online code. Then formats outputs and sends them to the
    required destinations.
    
    If that doesn't work, we'll put the processing in here as well.
  */

  //make a message logging stream

  ATH_MSG_DEBUG ( "starting CPCMX" );
  
  /** Create containers for BS simulation */
  DataVector<CMXCPTob>*  CMXTobs = new DataVector<CMXCPTob>;
  ATH_CHECK(SG::makeHandle(m_CMXCPTobLocation).record( std::unique_ptr<DataVector<CMXCPTob>>(CMXTobs ) ));
  DataVector<CMXCPHits>* CMXHits = new DataVector<CMXCPHits>;
  ATH_CHECK(SG::makeHandle(m_CMXCPHitsLocation).record( std::unique_ptr<DataVector<CMXCPHits>>(CMXHits ) ));


  /** Create and initialise arrays for storing hit results */
  std::vector< std::string > triggerTypes;
  triggerTypes.push_back(L1DataDef::emType());
  triggerTypes.push_back(L1DataDef::tauType());
  
  std::vector< std::vector< std::vector<int> > > crateHits;
  std::vector< std::vector<int> > Hits;
  
  Hits.resize(2);
  Hits[0].resize(16);
  Hits[1].resize(16);
  
  crateHits.resize(4);
  for (int crate = 0; crate < 4; ++crate) {
    crateHits[crate].resize(2);
    for (int cmx = 0; cmx < 2; ++cmx) {
      crateHits[crate][cmx].resize(16);
    }
  }
  
  
  // Create objects to store TOBs for L1Topo
  SG::WriteHandle<DataVector<CPCMXTopoData>> topoData = SG::makeHandle(m_TopoOutputLocation);
  ATH_CHECK(topoData.record(std::make_unique<DataVector<CPCMXTopoData>>()));
  for (int crate = 0; crate < 4; ++crate) {
    for (int cmx = 0; cmx < 2; ++cmx) {
       CPCMXTopoData* link = new CPCMXTopoData(crate, cmx);
       topoData->push_back(link);
    }
  }

  // Overflow flags for EM and TAU RoIs (i.e. the 2 CMXes)
  bool cpmOverflow[2] = {false, false};

  /** Get EM and Tau Trigger Thresholds */
  auto l1Menu = SG::makeHandle( m_L1MenuKey );


  float cpScale = l1Menu->thrExtraInfo().EM().emScale();
  std::vector<std::shared_ptr<TrigConf::L1Threshold>> allThresholds = l1Menu->thresholds();
  std::vector<std::shared_ptr<TrigConf::L1Threshold>> thresholds;
  for ( const auto& thresh : allThresholds  ) {
    if ( thresh->type() == L1DataDef::emType() || thresh->type() == L1DataDef::tauType() ) 
      thresholds.push_back( thresh );
  }

  /** Retrieve the CPCMXData (backplane data packages) */
  SG::ReadHandle<t_cpmDataContainer> bpData = SG::makeHandle(m_CPMCMXDataLocation);
  // Analyse module results
  t_cpmDataContainer::const_iterator it = bpData->begin();
  for ( ; it != bpData->end(); ++it) {
    int crate = (*it)->crate();
    int cmx   = (*it)->type();
    std::vector<unsigned int> tobWords = (*it)->TopoTOBs();

    // Store data for L1Topo
    int index = 2*crate + cmx;	
    bool overflow = (*it)->overflow();
    if (overflow) {
      (*topoData)[index]->setOverflow(true);
      cpmOverflow[cmx] = true;
    }

    for (std::vector<unsigned int>::const_iterator word = tobWords.begin();
        word != tobWords.end(); ++word) {

      // Push back to Topo link
      (*topoData)[index]->addTOB( (*word) );

      // Decode TOB word 
      CPTopoTOB tob( crate, cmx, (*word) );
      int ieta = tob.ieta() - 1;
      int iphi = tob.iphi();
      if (iphi < 0) iphi += 64;
      int et = tob.et();
      unsigned int isol = tob.isolation();

      // Now check against trigger thresholds
      for ( const auto& thresh : thresholds  ) {
        // Right type?
        if ( thresh->type() != triggerTypes[cmx] ) continue;
        // Does TOB satisfy this threshold?
          std::optional<uint16_t> isolMask;
          if (thresh->className() == "L1Threshold_EM") {
            std::shared_ptr<TrigConf::L1Threshold_EM> thresh_EM = std::static_pointer_cast<TrigConf::L1Threshold_EM>(thresh);
            isolMask = thresh_EM->isolationMask(ieta);
          }
          else if (thresh->className() == "L1Threshold_TAU") {
            std::shared_ptr<TrigConf::L1Threshold_TAU> thresh_TAU = std::static_pointer_cast<TrigConf::L1Threshold_TAU>(thresh);
            isolMask = thresh_TAU->isolationMask();
          }

          
          bool isolationPassed = true;
          if (isolMask) {
          for (unsigned int bit = 0; bit < TrigT1CaloDefs::numOfIsolationBits; ++bit) 
            if ( (*isolMask & (1<<bit)) && !(isol & (1<<bit)) ) isolationPassed = false;
          }

          int etCut             = thresh->thrValue(ieta)*cpScale;
          if ( et > etCut && isolationPassed ) {		
            int num = thresh->mapping();
            if (num < 16) {
              if (crateHits[crate][cmx][num] < 7) crateHits[crate][cmx][num]++;
              if (Hits[cmx][num] < 7)             Hits[cmx][num]++;
            }
           else ATH_MSG_WARNING("Invalid threshold number " << num );
          } // passes cuts

      } // Loop over thresholds

    } // Loop over TOBs

  } // Loop over module results

  // Overflow sets all trigger bits
  for (int cmx = 0; cmx < 2; ++cmx) {
    if (cpmOverflow[cmx]) {
      for (int i = 0; i < 16; ++i) Hits[cmx][i] = 7;
    }
  }

  // Form CTP data objects
  unsigned int cableWord0 = 0;
  unsigned int cableWord1 = 0;
  unsigned int cableWord2 = 0;
  unsigned int cableWord3 = 0;
  for (int i = 0; i < 8; ++i) {
    cableWord0 |= ( Hits[0][i]<<(3*i) );
    cableWord1 |= ( Hits[0][i+8]<<(3*i) );
    cableWord2 |= ( Hits[1][i]<<(3*i) );
    cableWord3 |= ( Hits[1][i+8]<<(3*i) );
  }

  SG::WriteHandle<EmTauCTP> emTauCTP = SG::makeHandle(m_CTPOutputKey);
  ATH_CHECK(emTauCTP.record(std::make_unique<EmTauCTP>(cableWord0, cableWord1, cableWord2, cableWord3)));

  // Form and store CPCMXHits
  std::vector<int> error0;  // Dummies - there will be no actual errors simulated
  std::vector<int> error1;

  // Now form hits words from module results and insert into objects
  std::vector<unsigned int> cratehits0;
  std::vector<unsigned int> cratehits1;
  const int peak = 0;
  const int system_crate = 3;

  // Crate sums (local and remote)  
  for (int crate = 0; crate < 4; ++crate) {
    for (int cmx = 0; cmx < 2; ++cmx) {
      cratehits0.assign(1,0);
      cratehits1.assign(1,0);
      for (int i = 0; i < 8; ++i) {
        cratehits0[0] |= ( crateHits[crate][cmx][i]<<(3*i) );
        cratehits1[0] |= ( crateHits[crate][cmx][i+8]<<(3*i) );
      }
      CMXCPHits* crateCMXHits = new CMXCPHits(crate, cmx, LVL1::CMXCPHits::LOCAL,
          cratehits0, cratehits1, error0, error1, peak);
      CMXHits->push_back(crateCMXHits);
      if (crate != system_crate) {
        CMXCPHits* remoteCMXHits = new CMXCPHits(system_crate, cmx, crate,
            cratehits0, cratehits1, error0, error1, peak);
        CMXHits->push_back(remoteCMXHits);
      }
    } // loop over CMXes
  } // loop over crates

  // global sums
  cratehits0.assign(1,cableWord0);
  cratehits1.assign(1,cableWord1);
  CMXCPHits* totalCMXHits0 = new CMXCPHits(system_crate, 0, LVL1::CMXCPHits::TOTAL,
      cratehits0, cratehits1, error0, error1, peak);
  cratehits0.assign(1,cableWord2);
  cratehits1.assign(1,cableWord3);
  CMXCPHits* totalCMXHits1 = new CMXCPHits(system_crate, 1, LVL1::CMXCPHits::TOTAL,
      cratehits0, cratehits1, error0, error1, peak);
  CMXHits->push_back(totalCMXHits0);
  CMXHits->push_back(totalCMXHits1);
      

  return StatusCode::SUCCESS ;
}

} // end of namespace bracket

