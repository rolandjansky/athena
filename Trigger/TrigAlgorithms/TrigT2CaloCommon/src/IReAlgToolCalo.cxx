/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     IReAlgToolCalo.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
//
// AUTHOR:   Denis Oliveira Damazio
//
// REFERENCES: This will provide infrastructure for the Tools
//		to perform their algorithms
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArRecEvent/LArCell.h"

#include "CaloIdentifier/LArEM_ID.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCellContainer.h"

#include "TrigT2CaloCommon/IReAlgToolCalo.h"
#include "TrigInterfaces/FexAlgo.h"


// Initialize method for all tools
// Retrieval of all Tools to be used during run
StatusCode IReAlgToolCalo::initialize()
{
        ATH_MSG_DEBUG("in initialize() by IReAlgToolCalo");

        ATH_CHECK( detStore()->retrieve(m_larMgr) );
	if ( !m_dataSvc.empty() ) ATH_CHECK( m_dataSvc.retrieve() );
        ATH_CHECK( m_geometryTool.retrieve() );

        // Initialize timing service in order to perform some measures
        // of performance
        if( !m_timersvc.empty() && m_timersvc.retrieve().isFailure() ) {
          ATH_MSG_WARNING( name() <<
                           ": Unable to locate TrigTimer Service" );
	  // Does not need to fail the Algorithm if no timing service is found
        } // End of if timing service
        // Initialize four timers for RegionSelector, ByteStreamCnv,
        // Algorithm time, Saving EMShowerMinimal
        if (!m_timersvc.empty()) {
		if ( name().find("Fex",0) == std::string::npos) 
                  ATH_MSG_WARNING( " Name of Alg not found" );
		else {
                std::string basename(name().substr(6,1)+name().substr(name().find("Fex",0)-5,5));
                //basename+=(name().substr(6,1)+name().substr(name().find("Fex",0)-5,5));
                ATH_MSG_INFO( "BaseName is : " << basename );
		HLT::FexAlgo *p = dynamic_cast<HLT::FexAlgo*>(const_cast<IInterface*>(parent()));
		/*
                m_timer[0] = m_timersvc->addItem(basename+"Total");
                m_timer[1] = m_timersvc->addItem(basename+"RegSel");
                m_timer[2] = m_timersvc->addItem(basename+"BSCnv");
                m_timer[3] = m_timersvc->addItem(basename+"Algor");
                m_timer[4] = m_timersvc->addItem(basename+"SaveEM");
		*/
		std::string Total((basename+"Total"));
                m_timer[0] = p->addTimer(Total);
		std::string RegSel((basename+"RegSel"));
                m_timer[1] = p->addTimer(RegSel);
		std::string BSCnv((basename+"BSCnv"));
                m_timer[2] = p->addTimer(BSCnv);
		std::string Algor((basename+"Algor"));
                m_timer[3] = p->addTimer(Algor);
		std::string SaveEM((basename+"SaveEM"));
                m_timer[4] = p->addTimer(SaveEM);
		} // End of check for Fex
        } // End of if timing service

	// Calibration object initialization
	if ( m_limit.size() != 0 ){
		m_calib->initialize(m_limit,m_dimension,m_correction);
	}
        m_lardecoded=true;
        m_tiledecoded=true;

	return StatusCode::SUCCESS;
} // End of initialize

// Finalize method for all tools
// nothing realy important done here
StatusCode IReAlgToolCalo::finalize(){
  ATH_MSG_DEBUG( "in finalize() by IReAlgToolCalo" );
  return StatusCode::SUCCESS;
} // End of finalize

#ifndef NDEBUG
//#define EXTRADEBUG
// Method designed by Tomek Kott. This method 
// Loops over cells and try to find out about their energy
// Then it prints out a cluster view for debugging purposes.
#ifdef EXTRADEBUG
# define ARG(x) x
#else
# define ARG(x)
#endif

void IReAlgToolCalo::PrintCluster(const double ARG(energyFromAlg),
                                const int ARG(nCaloType),
                                const int ARG(nCaloSamp),
                                CaloSampling::CaloSample ARG(SAMP1), 
                                CaloSampling::CaloSample ARG(SAMP2) ) const
#undef ARG
{
#ifdef EXTRADEBUG
 // empty for now

#endif // End of if EXTRADEBUG
return;
} // End of PrintCluster

#endif

// very loose implementation. No worry about time performance
void IReAlgToolCalo::storeCells( void ) {

	// empty for now
	return;
}
