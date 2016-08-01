///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRecoTool.cxx 
// Implementation file for class METRecoTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METRecoTool.h"

// MET EDM
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETComponentMap.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

#include <iomanip>

namespace met {

  using xAOD::MissingET;
  using xAOD::MissingETContainer;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponentMap;
  using xAOD::MissingETAuxContainer;
  using xAOD::MissingETAuxComponentMap;
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
  METRecoTool::METRecoTool(const std::string& name) : 
    AsgTool(name),
    m_doMetSum(false),
    m_nevt(0)
  {
    declareProperty( "METBuilders",        m_metbuilders         );
    declareProperty( "METRefiners",        m_metrefiners         );
    declareProperty( "METContainer",       m_contname = "MET"    );
    declareProperty( "METComponentMap",    m_mapname  = "METMap" );
    declareProperty( "WarnIfDuplicate",    m_warnOfDupes = true  );
    declareProperty( "METFinalName",       m_metfinalname = ""   );
    declareProperty( "TimingDetail",       m_timedetail = 0      );
  }

  // Destructor
  ///////////////
  METRecoTool::~METRecoTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METRecoTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    if( m_contname.size()==0 ) {
      ATH_MSG_FATAL("Output MissingETContainer name must be provided.");
      return StatusCode::FAILURE;
    }

    if( m_mapname.size()==0 ) {
      ATH_MSG_FATAL("Output MissingETComponentMap name must be provided.");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO ("Reconstructing MET container: " << m_contname
		  << " with composition map: " << m_mapname     );

    // Do we need a flag to toggle the summation?
    // Or will we require the summation to be made 
    if( m_metfinalname.size()>0 ) {
      m_doMetSum = true;
      ATH_MSG_INFO ("Will produce final MET sum \"" << m_metfinalname << "\"");
    } else {
      ATH_MSG_INFO ("Will not sum MET in this container.");
    }

    // retrieve builders
    for(ToolHandleArray<IMETToolBase>::const_iterator iBuilder=m_metbuilders.begin();
	iBuilder != m_metbuilders.end(); ++iBuilder) {
      ToolHandle<IMETToolBase> tool = *iBuilder;
      if( tool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("Failed to retrieve tool: " << tool->name());
	return StatusCode::FAILURE;
      };
      ATH_MSG_INFO("Retrieved tool: " << tool->name() );
    }

    // retrieve refiners
    for(ToolHandleArray<IMETToolBase>::const_iterator iRefiner=m_metrefiners.begin();
	iRefiner != m_metrefiners.end(); ++iRefiner) {
      ToolHandle<IMETToolBase> tool = *iRefiner;
      if( tool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("Failed to retrieve tool: " << tool->name());
	return StatusCode::FAILURE;
      };
      ATH_MSG_INFO("Retrieved tool: " << tool->name() );
    }

    // generate clocks
    unsigned int ntool = m_metbuilders.size()+m_metrefiners.size();
    m_toolclocks.resize(ntool);
    for ( unsigned int itool=0; itool<ntool; ++itool ) {
      m_toolclocks[itool].Reset();
    }
    m_clock.Reset();

    return StatusCode::SUCCESS;
  }

  StatusCode METRecoTool::execute() const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    if( evtStore()->contains<MissingETContainer>(m_contname) ) {
      if(m_warnOfDupes)
	{ ATH_MSG_WARNING("MET container " << m_contname << " already in StoreGate"); }
      return StatusCode::SUCCESS;
    }

    if( evtStore()->contains<MissingETComponentMap>(m_mapname) ) {
      if(m_warnOfDupes)
	{ ATH_MSG_WARNING("MET map " << m_mapname << " already in StoreGate"); }
      return StatusCode::SUCCESS;
    }

    // Create a MissingETContainer with its aux store
    MissingETContainer* metCont = new MissingETContainer();
    if( evtStore()->record(metCont, m_contname).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETContainer: " << m_contname);
      return StatusCode::SUCCESS;
    }
    MissingETAuxContainer* metAuxCont = new MissingETAuxContainer();
    if( evtStore()->record(metAuxCont, m_contname+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxContainer: " << m_contname+"Aux.");
      return StatusCode::SUCCESS;
    }
    metCont->setStore(metAuxCont);

    // Create a MissingETComponentMap with its aux store
    MissingETComponentMap* metMap = new MissingETComponentMap();
    if( evtStore()->record(metMap, m_mapname).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETContainer: " << m_mapname);
      return StatusCode::SUCCESS;
    }
    MissingETAuxComponentMap* metAuxMap = new MissingETAuxComponentMap();
    if( evtStore()->record(metAuxMap, m_mapname+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxContainer: " << m_mapname+"Aux.");
      return StatusCode::SUCCESS;
    }
    metMap->setStore(metAuxMap);

    if( buildMET(metCont, metMap).isFailure() ) {
      ATH_MSG_WARNING("Failed in MissingET reconstruction");
      return StatusCode::SUCCESS;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METRecoTool::finalize()
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
      unsigned int ntool = m_metbuilders.size()+m_metrefiners.size();
      ATH_MSG_INFO("  CPU/wall time [ms] for " << ntool << " tools:");
      unsigned int itool=0;

      // time builders
      for(ToolHandleArray<IMETToolBase>::const_iterator iBuilder=m_metbuilders.begin();
	  iBuilder != m_metbuilders.end(); ++iBuilder) {
	ToolHandle<IMETToolBase> th = *iBuilder;
	double tctime = m_toolclocks[itool].CpuTime()/double(m_nevt)*1000;
	double twtime = m_toolclocks[itool].RealTime()/double(m_nevt)*1000;
	ATH_MSG_INFO("    " << setw(30) << th.typeAndName()
		     << fixed << setprecision(3) << setw(10) << tctime
		     << fixed << setprecision(3) << setw(10) << twtime);
	++itool;
      }

      // time refiners
      for(ToolHandleArray<IMETToolBase>::const_iterator iRefiner=m_metrefiners.begin();
	  iRefiner != m_metrefiners.end(); ++iRefiner) {
	ToolHandle<IMETToolBase> th = *iRefiner;
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

  StatusCode METRecoTool::buildMET(xAOD::MissingETContainer* metCont, xAOD::MissingETComponentMap* metMap) const
  {

    if ( m_timedetail > 0 ) m_clock.Start(false);

    MissingET* metFinal = 0;
    if( m_doMetSum ) {
      ATH_MSG_DEBUG("Building final MET sum: " << m_metfinalname);
      metFinal = new MissingET(0.,0.,0., m_metfinalname, MissingETBase::Source::total());
    }

    unsigned int itool=0;
    // Run the MET reconstruction tools in sequence
    for(ToolHandleArray<IMETToolBase>::const_iterator iBuilder=m_metbuilders.begin();
	iBuilder != m_metbuilders.end(); ++iBuilder) {
      ToolHandle<IMETToolBase> tool = *iBuilder;
      ATH_MSG_VERBOSE("Building new MET term with: " << tool->name() );
      MissingET* metTerm = new MissingET(0.,0.,0.);
      ATH_MSG_VERBOSE("Insert MET object into container");
      metCont->push_back(metTerm);
      ATH_MSG_VERBOSE("Insert MET object into ComponentMap");
      MissingETComposition::add(metMap,metTerm);
      ATH_MSG_VERBOSE("Execute tool");
      if ( m_timedetail > 1 ) m_toolclocks[itool].Start(false);
      if( tool->execute(metTerm, metMap).isFailure() ) {
	ATH_MSG_WARNING("Failed to execute tool: " << tool->name());
	if ( m_timedetail > 0 ) m_clock.Stop();
	if ( m_timedetail > 1 ) m_toolclocks[itool].Stop();
	// return StatusCode::SUCCESS;
      }
      if ( m_timedetail > 1 ) {
	m_toolclocks[itool].Stop();
	ATH_MSG_VERBOSE("  " << tool->name() << " CPU/wall time: " << m_clock.CpuTime()*1000
			<< "/" << m_clock.RealTime()*1000 << " ms");
      }

      ///////////////////////////////////////////////////
      // FIXME: Make a genuine decision about whether
      //        to include terms in the sum here.
      ///////////////////////////////////////////////////
      if( m_doMetSum && MissingETBase::Source::hasCategory(metTerm->source(),MissingETBase::Source::Refined) ) {
	ATH_MSG_DEBUG("Adding constructed term: " << metTerm->name() << " to sum" );
	(*metFinal) += (*metTerm);
      }
      ++itool;
    }

    // Run the MET reconstruction tools in sequence
    for(ToolHandleArray<IMETToolBase>::const_iterator iRefiner=m_metrefiners.begin();
	iRefiner != m_metrefiners.end(); ++iRefiner) {
      ToolHandle<IMETToolBase> tool = *iRefiner;
      ATH_MSG_VERBOSE("Refining MET with: " << tool->name() );
      MissingET* metTerm = new MissingET(0.,0.,0.);
      ATH_MSG_VERBOSE("Insert MET object into container");
      metCont->push_back(metTerm);
      ATH_MSG_VERBOSE("Insert MET object into ComponentMap");
      MissingETComposition::add(metMap,metTerm);
      ATH_MSG_VERBOSE("Execute tool");
      if ( m_timedetail > 1 ) m_toolclocks[itool].Start(false);
      if( tool->execute(metTerm, metMap).isFailure() ) {
	ATH_MSG_WARNING("Failed to execute tool: " << tool->name());
	// if ( m_timedetail > 0 ) m_clock.Stop();
	// if ( m_timedetail > 1 ) m_toolclocks[itool].Stop();
	// return StatusCode::SUCCESS;
      }
      if ( m_timedetail > 1 ) {
	m_toolclocks[itool].Stop();
	ATH_MSG_DEBUG("  " << tool->name() << " CPU/wall time: " << m_toolclocks[itool].CpuTime()*1000
		      << "/" << m_toolclocks[itool].RealTime()*1000 << " ms");
      }
      ++itool;
    }

    if( m_doMetSum ) {
      MissingETBase::Types::bitmask_t source = MissingETBase::Source::total();
      metFinal->setSource(source);
      metCont->push_back(metFinal);
    }

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
