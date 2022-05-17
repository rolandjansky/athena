///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// METAssociationTool.cxx 
// Implementation file for class METAssociationTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METAssociationTool.h"

// MET EDM
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

#include <iomanip>

namespace met {

  using namespace xAOD;
  //
  using std::string;
  using std::setw;
  using std::setprecision;
  using std::fixed;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METAssociationTool::METAssociationTool(const std::string& name) : 
    AsgTool(name)
  {
  }

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METAssociationTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    if( m_metSuffix.size()==0 ) {
      ATH_MSG_ERROR("Output suffix for MET names must be provided.");
      return StatusCode::FAILURE;
    } else {
      std::string coreName = "MET_Core_" + m_metSuffix;
      std::string mapName = "METAssoc_" + m_metSuffix;
      ATH_MSG_INFO("Tool configured to build MET with names:");
      ATH_MSG_INFO("   Core container  ==> " << coreName);
      ATH_MSG_INFO("   Association map ==> " << mapName);
      ATH_CHECK( m_coreKey.assign(coreName) );
      ATH_CHECK( m_coreKey.initialize() );
      ATH_CHECK( m_mapKey.assign(mapName) );
      ATH_CHECK( m_mapKey.initialize() );
    }

    // retrieve associators and generate clocks
    unsigned int ntool = m_metAssociators.size();
    m_toolClocks.resize(ntool);

    ATH_CHECK(m_metAssociators.retrieve());    
    for (auto & clock : m_toolClocks) {
      clock.Reset();
    }
    m_clock.Reset();
    return StatusCode::SUCCESS;
  }

  StatusCode METAssociationTool::execute() const
  {

    //this section has had a very big re-write, after discussions with TJK...
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    //Create map and core containers

    auto metHandle = SG::makeHandle (m_coreKey);
    ATH_CHECK( metHandle.record (std::make_unique<xAOD::MissingETContainer>(), std::make_unique<xAOD::MissingETAuxContainer>()) );
    xAOD::MissingETContainer* metCont=metHandle.ptr();

    auto metMapHandle = SG::makeHandle (m_mapKey);
    ATH_CHECK( metMapHandle.record (std::make_unique<xAOD::MissingETAssociationMap>(), std::make_unique<xAOD::MissingETAuxAssociationMap>()) );
    xAOD::MissingETAssociationMap* metMap=metMapHandle.ptr();


    if( buildMET(metCont, metMap).isFailure() ) {
      ATH_MSG_DEBUG("Failed in MissingET reconstruction");
      return StatusCode::SUCCESS;
    }

    // Lock the containers in SG
    //ATH_CHECK( evtStore()->setConst(metMap) );
    //ATH_CHECK( evtStore()->setConst(metCont) );

    return StatusCode::SUCCESS;
  }

  StatusCode METAssociationTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    if ( m_timeDetail > 0 ) {
      double ctime = m_clock.CpuTime()*1000;
      double wtime = m_clock.RealTime()*1000;
      double actime = 0.0;
      double awtime = 0.0;
      if ( m_nevt > 0 ) {
	actime = ctime/double(m_nevt);
	awtime = wtime/double(m_nevt);
      }
      ATH_MSG_INFO("  Total CPU/wall time: " << ctime << "/" << wtime << " ms");
      ATH_MSG_INFO("   Avg. CPU/wall time: " << actime << "/" << awtime << " ms");
    }

    if ( m_timeDetail > 1 && m_nevt > 0 ) {
      unsigned int ntool = m_metAssociators.size();
      ATH_MSG_INFO("  CPU/wall time [ms] for " << ntool << " tools:");
      unsigned int itool=0;

      // time associators
      for(ToolHandleArray<IMETAssocToolBase>::const_iterator iAssociator=m_metAssociators.begin();
	  iAssociator != m_metAssociators.end(); ++iAssociator) {
	ToolHandle<IMETAssocToolBase> th = *iAssociator;
	double tctime = m_toolClocks[itool].CpuTime()/double(m_nevt)*1000;
	double twtime = m_toolClocks[itool].RealTime()/double(m_nevt)*1000;
	ATH_MSG_INFO("    " << setw(30) << th.typeAndName()
		     << fixed << setprecision(3) << setw(10) << tctime
		     << fixed << setprecision(3) << setw(10) << twtime);
	++itool;
      }
    }

    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  StatusCode METAssociationTool::buildMET(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const
  {

    if ( m_timeDetail > 0 ) m_clock.Start(false);

    unsigned int itool=0;
    // Run the MET reconstruction tools in sequence
    for(ToolHandleArray<IMETAssocToolBase>::const_iterator iAssociator=m_metAssociators.begin();
	iAssociator != m_metAssociators.end(); ++iAssociator) {
      ToolHandle<IMETAssocToolBase> tool = *iAssociator;
      if ( m_timeDetail > 1 ) m_toolClocks[itool].Start(false);
      if (tool->execute(metCont,metMap).isFailure()){
        ATH_MSG_WARNING("Failed to execute tool: " << tool->name());
	if ( m_timeDetail > 0 ) m_clock.Stop();
	if ( m_timeDetail > 1 ) m_toolClocks[itool].Stop();
        return StatusCode::FAILURE;
      }
      if ( m_timeDetail > 1 ) {
	m_toolClocks[itool].Stop();
	ATH_MSG_VERBOSE("  " << tool->name() << " CPU/wall time: " << m_clock.CpuTime()*1000
			<< "/" << m_clock.RealTime()*1000 << " ms");
      }
      ++itool;
    }
    bool foundOverlaps = metMap->identifyOverlaps();
    ATH_MSG_DEBUG( (foundOverlaps ? "Overlaps" : "No overlaps") << " identified!");
    if ( m_timeDetail > 0 ) m_clock.Stop();
    ATH_MSG_DEBUG("  " << this->name() << " total CPU/wall time: " << m_clock.CpuTime()*1000
		  << "/" << m_clock.RealTime()*1000 << " ms");

    ++m_nevt;
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}
