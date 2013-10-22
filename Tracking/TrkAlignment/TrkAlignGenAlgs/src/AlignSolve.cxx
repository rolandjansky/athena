/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "TrkTrack/TrackCollection.h"

#include "TrkAlignGenAlgs/AlignSolve.h"

#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "TrkAlignInterfaces/ITrkAlignDBTool.h"

#include "TrkAlignEvent/AlignTrack.h"

// standard libraries
#include "TFile.h"

namespace Trk {

  //___________________________________________________________________________
  AlignSolve::AlignSolve(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm (name, pSvcLocator)
    , m_alignTool("Trk::GlobalChi2AlignTool",this)
    , m_geometryManagerTool("Muon::MuonGeometryManagerTool") // use as public tool
    , m_trkAlignDBTool("Trk::TrkAlignDBTool", this) 
    , m_file(0)
    , m_logStream(0)
  {
    declareProperty("AlignDBTool", m_trkAlignDBTool,
		    "tool for handling DB stuff ");
    declareProperty("AlignTool", m_alignTool,
		    "alignment algorithm-specific tool");

    declareProperty("GeometryManagerTool", m_geometryManagerTool,
		    "tool for configuring geometry");
  
    // processing operations
    declareProperty("AlignSolveLevel",   m_alignSolveLevel = 3);
    declareProperty("WriteNtuple",       m_writeNtuple       = true);

    declareProperty("WriteLogFile",     m_writeLogfile = true);
    declareProperty("LogFileName",      m_logfileName  = "alignlogfile.txt");
  }

  //___________________________________________________________________________
  AlignSolve::~AlignSolve() 
  {

  }

  //___________________________________________________________________________
  StatusCode AlignSolve::initialize() 
  {
    ATH_MSG_INFO("AlignSolve::initialize()"); 
  
    // Get GeometryManagerTool
    if ( m_geometryManagerTool.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Failed to retrieve tool " << m_geometryManagerTool << endreq;
      return StatusCode::FAILURE;
    } 
    else ATH_MSG_DEBUG("Retrieved tool " << m_geometryManagerTool); 
    
    // Get AlignTool tool
    if ( m_alignTool.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Failed to retrieve tool " << m_alignTool << endreq;
      return StatusCode::FAILURE;
    } 
    else ATH_MSG_DEBUG("Retrieved tool " << m_alignTool); 
  
    // Get TrkAlignDataBaseTool  
    if ( m_trkAlignDBTool.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Failed to retrieve tool " << m_trkAlignDBTool<<endreq;
      return StatusCode::FAILURE;
    } 
    else ATH_MSG_INFO("Retrieved tool " << m_trkAlignDBTool);

  
    // Alignment Level
    // Number of layers in the superstructures
    if (m_alignSolveLevel>3 || m_alignSolveLevel<1) {
      msg(MSG::FATAL)<<"AlignSolveLevel could be 1, 2 or 3"<<endreq;
      return StatusCode::FAILURE;
    }

    if (m_writeNtuple) {
      m_file = new TFile("AlignSolve.root","RECREATE");
      m_trkAlignDBTool->setNtuple(m_file);
    }

    // open logfile
    if(m_writeLogfile) {
      std::ostream  * out   = &std::cout;
      std::ofstream * ofile = new std::ofstream(m_logfileName.c_str());
      if(!ofile->is_open()) {
        msg(MSG::ERROR)<<"Couldn't open logfile. Writing to standard output."<<endreq;
        delete ofile;
      }
      else
        out = ofile;

      m_logStream = out;

      // set logStream in the tools
      m_geometryManagerTool->setLogStream(m_logStream);
      m_alignTool          ->setLogStream(m_logStream);
      m_trkAlignDBTool     ->setLogStream(m_logStream);
    }
  
    return StatusCode::SUCCESS;
  }
  
  //___________________________________________________________________________
  StatusCode AlignSolve::beginRun()
  {
    ATH_MSG_DEBUG("AlignSolve::beginRun()");

    ATH_MSG_DEBUG("reading geometry");
    int NDoF = m_geometryManagerTool->ReadGeometry(m_alignSolveLevel);
    ATH_MSG_DEBUG("built geometry with "<<NDoF<<" DoF");

    ATH_MSG_DEBUG("initializing AlignTool and TrkAlignDBTool");
    StatusCode sc = m_alignTool->firstEventInitialize();
    if (sc != StatusCode::SUCCESS) {
      msg(MSG::FATAL)<<"problem with alignTool->firstEventInitialize()"<<endreq;
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("getting data from files");
    m_alignTool->accumulateFromFiles();

    return StatusCode::SUCCESS;
  }

  //___________________________________________________________________________
  StatusCode AlignSolve::execute()
  {
    ATH_MSG_DEBUG("AlignSolve::execute()");
    // return FAILURE to make sure that the job ends after 1st event
    return StatusCode::FAILURE;
  }

  //___________________________________________________________________________
  StatusCode AlignSolve::endRun()
  {
    ATH_MSG_INFO("In AlignSolve::endRun()");

    StatusCode sc = m_trkAlignDBTool->preSolve();
    if (sc.isFailure()) {
      msg(MSG::FATAL)<<"failure in preSolve in trkAlignDBTool"<<endreq;
      return StatusCode::FAILURE;
    }

    sc = m_alignTool->solve();
    if (sc.isFailure()) {
      msg(MSG::FATAL)<<"failure in solve in alignTool"<<endreq;
      return StatusCode::FAILURE;
    }

    m_trkAlignDBTool->writeAlignPar();

    return sc;
  }

  //___________________________________________________________________________
  StatusCode AlignSolve::finalize()
  {
    ATH_MSG_INFO("In AlignSolve::finalize()");

    if (m_file)
      m_file->Close();

    // close the logfile
    if(m_writeLogfile) {
      if(std::ofstream * ofs = dynamic_cast<std::ofstream *>(m_logStream)) {
        if(ofs->is_open()) {
          *ofs<<std::flush;
          ofs->close();
        }
        delete m_logStream;
      }
    }

    return StatusCode::SUCCESS;
  }

}
