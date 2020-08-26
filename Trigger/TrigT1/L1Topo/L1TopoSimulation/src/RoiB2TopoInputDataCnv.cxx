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

#include "Gaudi/Property.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"


// Constructor
LVL1::RoiB2TopoInputDataCnv::RoiB2TopoInputDataCnv( const std::string& name, 
                                                    ISvcLocator* pSvcLocator ) : 
   ::AthReentrantAlgorithm( name, pSvcLocator ),
   m_datamaker( new LVL1::L1TopoDataMaker() ),
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
   delete m_datamaker;
}

StatusCode
LVL1::RoiB2TopoInputDataCnv::initialize()
{
   ATH_CHECK(m_roibLocation.initialize());
   ATH_CHECK(m_emTauLocation.initialize());
   ATH_CHECK(m_jetLocation.initialize());
   ATH_CHECK(m_energyLocation.initialize());

   return StatusCode::SUCCESS;
}

StatusCode
LVL1::RoiB2TopoInputDataCnv::execute (const EventContext& ctx) const
{  
   ATH_MSG_DEBUG ("Executing " << name() << "...");
   
   SG::ReadHandle<ROIB::RoIBResult> roibResult(m_roibLocation, ctx);

   if( !roibResult.isValid() ) {
      ATH_MSG_WARNING("No RoIBResults with SG key '" << m_roibLocation.key() << "' found in the event. Can not create any information needed for L1Topo simulation.");
      return StatusCode::RECOVERABLE;
   }

   SG::WriteHandle<DataVector<CPCMXTopoData>> emtauTopoData(m_emTauLocation, ctx);
   // emtau
   if(emtauTopoData.isPresent()) {
      ATH_MSG_WARNING("DataVector<CPCMXTopoData> with SG key '" << m_emTauLocation.key() << "' already exists in SG, will not create a new one.");
   } else {
      ATH_MSG_DEBUG("Recording DataVector<CPCMXTopoData> with SG key '" << m_emTauLocation.key() << "'.");
      ATH_CHECK(emtauTopoData.record(std::make_unique< DataVector<CPCMXTopoData> >()));
      m_datamaker->makeCPCMXTopoData(roibResult.cptr(), emtauTopoData.ptr());
   }
   
   // jet
   SG::WriteHandle<DataVector<JetCMXTopoData>> jetTopoData(m_jetLocation, ctx);
   if(jetTopoData.isPresent()) {
      ATH_MSG_WARNING("DataVector<JetCMXTopoData> with SG key '" << m_jetLocation.key() << "' already exists in SG, will not create a new one.");
   } else {
      ATH_MSG_DEBUG("Recording DataVector<JetCMXTopoData> with SG key '" << m_jetLocation.key() << "'.");
      ATH_CHECK(jetTopoData.record(std::make_unique<DataVector<JetCMXTopoData>>()));
      m_datamaker->makeJetCMXTopoData(roibResult.cptr(), jetTopoData.ptr());
   }

   // energy
   SG::WriteHandle<EnergyTopoData> energyTopoData(m_energyLocation, ctx);
   if(energyTopoData.isPresent()) {
      ATH_MSG_WARNING("EnergyTopoData with SG key '" << m_energyLocation.key() << "' already exists in SG, will not create a new one.");
   } else {
      ATH_MSG_DEBUG("Recording EnergyTopoData with SG key '" << m_energyLocation.key() << "'.");
      ATH_CHECK(energyTopoData.record(std::make_unique<EnergyTopoData>()));
      m_datamaker->makeEnergyTopoData(roibResult.cptr(), energyTopoData.ptr());
   }

   return StatusCode::SUCCESS;
}

