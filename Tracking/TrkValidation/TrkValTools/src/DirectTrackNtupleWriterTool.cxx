/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DirectTrackNtupleWriterTool.cxx
//   Source file for class DirectTrackNtupleWriterTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

// Gaudi/root
#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

// Trk
#include "DirectTrackNtupleWriterTool.h"
#include "TrkValInterfaces/ITrackValidationNtupleTool.h"
#include "TrkTrack/Track.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"

// constructor
Trk::DirectTrackNtupleWriterTool::DirectTrackNtupleWriterTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_ValidationNtupleTools(),
        m_extrapolatorHandle("Trk::Extrapolator/InDetExtrapolator"),
        m_nt(0),
        m_doTruth(true)
{
    declareInterface<IDirectTrackNtupleWriter>(this);
    // Declare tool handles
    declareProperty("ValidationNtupleTools",      m_ValidationNtupleTools, "set of tools to write the validation ntuple");
    declareProperty("ExtrapolatorTool",           m_extrapolatorHandle,                   "Extrapolator, eg for tracks without Pergiee parameters");
     
    // Declare the properties
    declareProperty("NtupleFileName",               m_ntupleFileName = "/NTUPLES/FILE1",    "Ntuple file handle");
    declareProperty("NtupleDirectoryName",          m_ntupleDirName = "FitterValidation",   "dircetory name for ntuple tree");
    declareProperty("NtupleTreeName",               m_ntupleTreeName = "Validation",        "Name of the ntuple tree");
    declareProperty("DoTruth",                      m_doTruth,                              "Write truth data?");
}

// destructor
Trk::DirectTrackNtupleWriterTool::~DirectTrackNtupleWriterTool() {}




// initialize
StatusCode Trk::DirectTrackNtupleWriterTool::initialize() {

  StatusCode sc(StatusCode::SUCCESS, true);
    
  // retrive pointer to THistSvc
  ITHistSvc *tHistSvc;
  sc =  service("THistSvc", tHistSvc);
  if (sc.isFailure()) {
      ATH_MSG_ERROR ("Unable to retrieve pointer to THistSvc");
      return sc;
  }
  // ---------------------------
  // create tree and register it to THistSvc
  m_nt = new TTree(TString(m_ntupleTreeName), "Track Validation");
  // NB: we must not delete the tree, this is done by THistSvc
  std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
  sc = tHistSvc->regTree(fullNtupleName, m_nt);
  if (sc.isFailure()) {
      ATH_MSG_ERROR ("Unable to register TTree : " << fullNtupleName);
      return sc;
  }
  
  // Get Validation ntuple Tools
  sc = m_ValidationNtupleTools.retrieve();
  if (sc.isFailure()) {
      ATH_MSG_FATAL ("Could not retrieve "<< m_ValidationNtupleTools <<" (to write validation ntuple) ");
      return sc;
  }

  // add the ntuple branches to our tree
  ToolHandleArray< Trk::ITrackValidationNtupleTool >::iterator itTools;
  itTools = m_ValidationNtupleTools.begin();
  for (  ; itTools != m_ValidationNtupleTools.end(); ++itTools ) {
     if (((*itTools)->addNtupleItems(m_nt)).isFailure()) {
       ATH_MSG_ERROR ("ValidationNtupleTool could not add its branches for tree " << fullNtupleName);
       return StatusCode::FAILURE;
     }
   }
   
   // FIXME retrieve extrapolator
   
   return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::DirectTrackNtupleWriterTool::finalize() {

  ATH_MSG_DEBUG ("start finalize() in " << name());
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// write track data to ntuple
//////////////////////////////////////
StatusCode Trk::DirectTrackNtupleWriterTool::writeTrackData (
     const Trk::Track& track,
     const int iterationIndex,
     const Trk::FitterStatusCode fitStatCode )  const {
    
  ATH_MSG_VERBOSE ("in writeTrackData(trk, indx)");

  ToolHandleArray< Trk::ITrackValidationNtupleTool >::const_iterator itTools;
  itTools = m_ValidationNtupleTools.begin();
  for (  ; itTools != m_ValidationNtupleTools.end(); ++itTools ) {
    if (((*itTools)->fillTrackData( track, iterationIndex, fitStatCode )).isFailure()) {
      ATH_MSG_ERROR ("Validation Ntuple Tool could not fill track data.");
      return StatusCode::FAILURE;
    }
  }

  //----------------------------------------------
  // write the ntuple record out (once per call)
  return writeRecord(m_nt);
}

/////////////////////////////////////
/// write track particle data to ntuple
//////////////////////////////////////
StatusCode Trk::DirectTrackNtupleWriterTool::writeTrackParticleData (
     const Trk::TrackParticleBase& particlebase )  const {
    
  ATH_MSG_VERBOSE ("in writeTrackParticleData()");

  ToolHandleArray< Trk::ITrackValidationNtupleTool >::const_iterator itTools;
  itTools = m_ValidationNtupleTools.begin();
  for (  ; itTools != m_ValidationNtupleTools.end(); ++itTools ) {
    if (((*itTools)->fillTrackParticleData( particlebase )).isFailure()) {
      ATH_MSG_ERROR ("Validation Ntuple Tool could not fill particlebase data.");
      return StatusCode::FAILURE;
    }
  }

  //----------------------------------------------
  // write the ntuple record out (once per call)
  return writeRecord(m_nt);
}

//////////////////////////////////////////////////////////
/// fill AND write ntuple data of a given proto-trajectory
//////////////////////////////////////////////////////////
StatusCode Trk::DirectTrackNtupleWriterTool::writeProtoTrajectoryData (
        const Trk::ProtoTrajectory& trajectory,
        const int iterationIndex,
        const Trk::Perigee* perigee,
        const Trk::FitterStatusCode fitStatCode ) const{
    if (!m_nt) {
        msg(MSG::ERROR) << "writeProtoTrajectoryData(protoTraj, indx) can only be used, if property BookNewNtuple is set to true"  << endreq;
        return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE ("in writeProtoTrajectoryData(protoTraj, indx)");
    
    ToolHandleArray< Trk::ITrackValidationNtupleTool >::const_iterator itTools;
    itTools = m_ValidationNtupleTools.begin();
    for (  ; itTools != m_ValidationNtupleTools.end(); ++itTools ) {
      if (((*itTools)->fillProtoTrajectoryData( trajectory, iterationIndex, perigee, fitStatCode )).isFailure()) {
        ATH_MSG_ERROR ("Validation Ntuple Tool could not fill track data.");
        return StatusCode::FAILURE;
      }
    }
    return writeRecord(m_nt);
}  

/////////////////////////////////////////
/// write the filled data into the ntuple
/////////////////////////////////////////
StatusCode Trk::DirectTrackNtupleWriterTool::writeRecord(TTree* tree) const {
    if (!tree) return StatusCode::FAILURE;
    tree->Fill();
    // reset variables for next call
    ToolHandleArray< Trk::ITrackValidationNtupleTool >::const_iterator itTools;
    itTools = m_ValidationNtupleTools.begin();
    for ( ; itTools != m_ValidationNtupleTools.end(); ++itTools ) (*itTools)->resetVariables();
    return StatusCode::SUCCESS;
}
