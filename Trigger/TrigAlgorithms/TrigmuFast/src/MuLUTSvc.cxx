/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "TrigmuFast/MuLUTSvc.h"
#include "PathResolver/PathResolver.h"

using namespace std;

MuLUTSvc::MuLUTSvc(const std::string& name,ISvcLocator* sl) :
    Service(name,sl) 
{
    declareProperty( "LUTfile", m_lut_filename="lut_rad.data" );
}

// queryInterface 
StatusCode MuLUTSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
    if(IID_IMuLUTSvc.versionMatch(riid) ) 
    { 
        *ppvIF = (MuLUTSvc*)this; 
    } else { 
        MsgStream log(messageService(), name());
        log << MSG::DEBUG << name() << " cannot found the interface!" <<endreq;
        return Service::queryInterface(riid, ppvIF); 
    }
    return StatusCode::SUCCESS;
} 

StatusCode MuLUTSvc::initialize()
{
    StatusCode sc;

    MsgStream log(messageService(), name());
    log << MSG::DEBUG << name() << ": Start of run initialisation" << endreq;

    sc = Service::initialize();
    if ( sc.isFailure() ) return sc;
    
    // implement the search of lut.data trought the pathresolver Tool.
    std::string lut_filename;
    lut_filename = PathResolver::find_file (m_lut_filename, "DATAPATH");
    log << MSG::INFO << lut_filename << endreq;

    if (lut_filename == "")
    {
       log << MSG::ERROR 
           << "Cannot locate " << m_lut_filename 
	   << " from ${DATAPATH}" << endreq;
       return StatusCode::FAILURE;
    }
    
    MuLUT::s_lut_file = lut_filename;

    // Instanciate the Muon Look Up Tables
    MuLUT::s_instance = new MuLUT;
    if(! MuLUT::s_instance )
    {
        log << MSG::ERROR <<   "Muon Roads are not loaded!"  << endl 
            << endreq;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode MuLUTSvc::finalize() 
{
    delete MuLUT::s_instance;
    return StatusCode::SUCCESS;
} 

// Implement the specifics ...

const MuLUT*
MuLUTSvc::muLUT() const
{
    return MuLUT::s_instance;
} 
