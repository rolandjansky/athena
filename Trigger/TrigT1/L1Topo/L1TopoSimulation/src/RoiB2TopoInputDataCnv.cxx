/*//////////////////////// -*- C++ -*- ///////////////////////////// */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 RoiB2TopoInputDataCnv.cxx 
 Implementation file for class RoiB2TopoInputDataCnv
*/

#include "RoiB2TopoInputDataCnv.h"
#include "TrigT1CaloUtils/L1TopoDataMaker.h"

#include "GaudiKernel/Property.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Result/RoIBResult.h"

#include "TrigT1CaloEvent/EnergyTopoData.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"
#include "TrigT1CaloEvent/JetCMXTopoDataCollection.h"

// Constructor
LVL1::RoiB2TopoInputDataCnv::RoiB2TopoInputDataCnv( const std::string& name, 
                                                    ISvcLocator* pSvcLocator ) : 
   ::AthAlgorithm( name, pSvcLocator ),
   datamaker( new LVL1::L1TopoDataMaker() ),
   m_roibLocation( "RoIBResult" ),
   m_emTauLocation( TrigT1CaloDefs::EmTauTopoTobLocation ),
   m_jetLocation(TrigT1CaloDefs::JetTopoTobLocation),
   m_energyLocation(TrigT1CaloDefs::EnergyTopoDataLocation)
{
   declareProperty( "ROIBResultLocation", m_roibLocation, "Storegate key for the reading the ROIBResult" );
   declareProperty( "EmTauROILocation", m_emTauLocation, "Storegate key for the writing EMTAU info" );
   declareProperty( "JetROILocation", m_jetLocation, "Storegate key for writing the Jet ROIs" );
   declareProperty( "EnergyROILocation", m_energyLocation, "Storegate key for writing the Topo Energy ROI" );
}


// Destructor
LVL1::RoiB2TopoInputDataCnv::~RoiB2TopoInputDataCnv()
{
   delete datamaker;
}


// Athena Algorithm's Hooks
StatusCode
LVL1::RoiB2TopoInputDataCnv::initialize()
{
   ATH_MSG_INFO ("Initializing " << name() << "...");
   
   return StatusCode::SUCCESS;
}

StatusCode
LVL1::RoiB2TopoInputDataCnv::finalize()
{
   ATH_MSG_INFO ("Finalizing " << name() << "...");
   
   return StatusCode::SUCCESS;
}

StatusCode
LVL1::RoiB2TopoInputDataCnv::execute()
{  
   ATH_MSG_DEBUG ("Executing " << name() << "...");
   
   const ROIB::RoIBResult* roibResult{nullptr};

   if( evtStore()->contains<ROIB::RoIBResult>(m_roibLocation) ) {
      CHECK( evtStore()->retrieve(roibResult, m_roibLocation) );
   } else {
      ATH_MSG_WARNING("No RoIBResults with SG key '" << m_roibLocation.toString() << "' found in the event. Can not create any information needed for L1Topo simulation.");
      return StatusCode::RECOVERABLE;
   }


   // emtau
   if(evtStore()->contains<DataVector<CPCMXTopoData>>(m_emTauLocation)) {
      ATH_MSG_WARNING("DataVector<CPCMXTopoData> with SG key '" << m_emTauLocation.toString() << "' already exists in SG, will not create a new one.");
   } else {
      DataVector<CPCMXTopoData>* emtauTopoData(new DataVector<CPCMXTopoData>());
      datamaker->makeCPCMXTopoData(roibResult, emtauTopoData);
      ATH_MSG_DEBUG("Recording DataVector<CPCMXTopoData> with SG key '" << m_emTauLocation.toString() << "'.");
      CHECK(evtStore()->record( emtauTopoData, m_emTauLocation ));
   }
   
   // jet
   if(evtStore()->contains<DataVector<JetCMXTopoData>>(m_jetLocation)) {
      ATH_MSG_WARNING("DataVector<JetCMXTopoData> with SG key '" << m_jetLocation.toString() << "' already exists in SG, will not create a new one.");
   } else {
      DataVector<JetCMXTopoData>* jetTopoData(new DataVector<JetCMXTopoData>());
      datamaker->makeJetCMXTopoData(roibResult, jetTopoData);
      ATH_MSG_DEBUG("Recording DataVector<JetCMXTopoData> with SG key '" << m_jetLocation.toString() << "'.");
      CHECK(evtStore()->record( jetTopoData, m_jetLocation ));
   }

   // energy
   if(evtStore()->contains<EnergyTopoData>(m_energyLocation)) {
      ATH_MSG_WARNING("EnergyTopoData with SG key '" << m_energyLocation.toString() << "' already exists in SG, will not create a new one.");
   } else {
      EnergyTopoData* energyTopoData(new EnergyTopoData());
      datamaker->makeEnergyTopoData(roibResult, energyTopoData);
      ATH_MSG_DEBUG("Recording EnergyTopoData with SG key '" << m_energyLocation.toString() << "'.");
      CHECK(evtStore()->record( energyTopoData, m_energyLocation ));
   }

   return StatusCode::SUCCESS;
}

