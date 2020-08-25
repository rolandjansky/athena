///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    AsgTool(name),
    m_metassociators(this)
  {
    declareProperty( "METAssociators", m_metassociators              );
    declareProperty( "METSuffix",      m_metsuffix = "AntiKt4LCTopo" );
    declareProperty( "AllowOverwrite", m_overwrite = false           );
    declareProperty( "TimingDetail",   m_timedetail = 0      );

    //These properties will be overwritten in intialize
    declareProperty( "CoreOutputKey", m_corenameKey );
    declareProperty( "AssociationOutputKey", m_mapnameKey );
    

  }

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METAssociationTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    if( m_metsuffix.size()==0 ) {
      ATH_MSG_ERROR("Output suffix for MET names must be provided.");
      return StatusCode::FAILURE;
    } else {
      m_corename = "MET_Core_"+m_metsuffix;
      m_mapname = "METAssoc_"+m_metsuffix;
      ATH_MSG_INFO("Tool configured to build MET with names:");
      ATH_MSG_INFO("   Core container  ==> " << m_corename);
      ATH_MSG_INFO("   Association map ==> " << m_mapname);
      ATH_CHECK( m_corenameKey.assign(m_corename) );
      ATH_CHECK( m_corenameKey.initialize() );
      ATH_CHECK( m_mapnameKey.assign(m_mapname) );
      ATH_CHECK( m_mapnameKey.initialize() );


    }

    // retrieve associators and generate clocks
    unsigned int ntool = m_metassociators.size();
    m_toolclocks.resize(ntool);

    ATH_CHECK(m_metassociators.retrieve());    
    for (auto & clock : m_toolclocks) {
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

    auto metHandle= SG::makeHandle (m_corenameKey);
    ATH_CHECK( metHandle.record (std::make_unique<xAOD::MissingETContainer>(),                      std::make_unique<xAOD::MissingETAuxContainer>()) );
    xAOD::MissingETContainer* metCont=metHandle.ptr();

    auto metMapHandle= SG::makeHandle (m_mapnameKey);
    ATH_CHECK( metMapHandle.record (std::make_unique<xAOD::MissingETAssociationMap>(),                      std::make_unique<xAOD::MissingETAuxAssociationMap>()) );
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

    if ( m_timedetail > 0 ) {
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

    if ( m_timedetail > 1 && m_nevt > 0 ) {
      unsigned int ntool = m_metassociators.size();
      ATH_MSG_INFO("  CPU/wall time [ms] for " << ntool << " tools:");
      unsigned int itool=0;

      // time associators
      for(ToolHandleArray<IMETAssocToolBase>::const_iterator iAssociator=m_metassociators.begin();
	  iAssociator != m_metassociators.end(); ++iAssociator) {
	ToolHandle<IMETAssocToolBase> th = *iAssociator;
	double tctime = m_toolclocks[itool].CpuTime()/double(m_nevt)*1000;
	double twtime = m_toolclocks[itool].RealTime()/double(m_nevt)*1000;
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

    if ( m_timedetail > 0 ) m_clock.Start(false);

    unsigned int itool=0;
    // Run the MET reconstruction tools in sequence
    for(ToolHandleArray<IMETAssocToolBase>::const_iterator iAssociator=m_metassociators.begin();
	iAssociator != m_metassociators.end(); ++iAssociator) {
      ToolHandle<IMETAssocToolBase> tool = *iAssociator;
      if ( m_timedetail > 1 ) m_toolclocks[itool].Start(false);
      if (tool->execute(metCont,metMap).isFailure()){
        ATH_MSG_WARNING("Failed to execute tool: " << tool->name());
	if ( m_timedetail > 0 ) m_clock.Stop();
	if ( m_timedetail > 1 ) m_toolclocks[itool].Stop();
        return StatusCode::FAILURE;
      }
      if ( m_timedetail > 1 ) {
	m_toolclocks[itool].Stop();
	ATH_MSG_VERBOSE("  " << tool->name() << " CPU/wall time: " << m_clock.CpuTime()*1000
			<< "/" << m_clock.RealTime()*1000 << " ms");
      }
      ++itool;
    }
    bool foundOverlaps = metMap->identifyOverlaps();
    ATH_MSG_DEBUG( (foundOverlaps ? "Overlaps" : "No overlaps") << " identified!");
    if ( m_timedetail > 0 ) m_clock.Stop();
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
