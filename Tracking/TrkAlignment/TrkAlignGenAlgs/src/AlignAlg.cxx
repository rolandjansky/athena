/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/TrackCollection.h"

#include "TrkAlignGenAlgs/AlignAlg.h"

#include "TrkAlignInterfaces/IGeometryManagerTool.h"
#include "TrkAlignInterfaces/IFillNtupleTool.h"
#include "TrkAlignInterfaces/ITrkAlignDBTool.h"
#include "TrkAlignInterfaces/IAlignTrackPreProcessor.h"
#include "TrkAlignInterfaces/ITrackCollectionProvider.h"

#include "TrkAlignEvent/AlignTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkAlignEvent/AlignVertex.h"

#include "TFile.h"

namespace Trk {

//___________________________________________________________________________
AlignAlg::AlignAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm (name, pSvcLocator)
  , m_trackCollectionProvider("Trk::TrackCollectionProvider",this)
  , m_alignTrackPreProcessor("Trk::AlignTrackPreProcessor",this)
  , m_alignTrackCreator("Trk::AlignTrackCreator",this)
  , m_alignTrackDresser("Trk::AlignTrackDresser",this)
  , m_alignTool("Trk::GlobalChi2AlignTool",this)
  , m_geometryManagerTool("Muon::MuonGeometryManagerTool") // use as public tool
  , m_trkAlignDBTool("Trk::TrkAlignDBTool", this)
  , m_fillNtupleTool("",this)
  , m_nDoF(-1)
  , m_ntuple(nullptr)
  , m_logStream(nullptr)
  , m_nevents(0)
  , m_ntracks(0)
  , m_ntracksSel(0)
  , m_ntracksProc(0)
  , m_ntracksDress(0)
  , m_ntracksAccum(0)
{

  // TrkGlobalChi2AlignTools
  declareProperty("AlignDBTool",            m_trkAlignDBTool,         
		  "tool for handling DB stuff ");

  declareProperty("TrackCollectionProvider", m_trackCollectionProvider,
		  "tool for getting track collection from StoreGate");

  declareProperty("AlignTrackPreProcessor", m_alignTrackPreProcessor, 
		  "tool for converting Trk::Track to AlignTrack after processing if necessary");

  declareProperty("AlignTrackCreator",      m_alignTrackCreator,      
		  "tool for creating AlignTSOSCollection to store on AlignTrack");

  declareProperty("AlignTrackDresser",      m_alignTrackDresser,      
		  "tool for dressing AlignTrack with residuals, derivatives, etc.");

  declareProperty("AlignTool",              m_alignTool,              
		  "alignment algorithm-specific tool");

  declareProperty("GeometryManagerTool",    m_geometryManagerTool,    
		  "tool for configuring geometry");

  declareProperty("FillNtupleTool",         m_fillNtupleTool,         
		  "tool for storing Trk::Track information into the ntuple");

  // processing options
  declareProperty("WriteNtuple",            m_writeNtuple  = true		);
  declareProperty("FileName",               m_filename     = "Align.root"	);
  declareProperty("FilePath",               m_filepath     = "./"		);

  declareProperty("AlignSolveLevel",        m_alignSolveLevel = 3		);

  declareProperty("SolveOnly",              m_solveOnly = false			);

  declareProperty("WriteLogFile",           m_writeLogfile = true		);
  declareProperty("LogFileName",            m_logfileName  = "alignlogfile.txt"	);

  declareProperty("AlignTracksName",        m_alignTracksName = "AlignTracks"	);    

}

//___________________________________________________________________________
AlignAlg::~AlignAlg() 
{

  if (m_ntuple && m_solveOnly) {  
    ATH_MSG_DEBUG("closing file");
    m_ntuple->Close(); 
    ATH_MSG_DEBUG("file closed");
  }
}

//___________________________________________________________________________
StatusCode AlignAlg::initialize() 
{
  ATH_MSG_INFO("AlignAlg::initialize()");

  // Get GeometryManagerTool
  if ( m_geometryManagerTool.retrieve().isFailure() ) {
    msg(MSG::FATAL)<<"Failed to retrieve tool " << m_geometryManagerTool
	   << endmsg;
    return StatusCode::FAILURE;
  }
  else
    ATH_MSG_INFO("Retrieved tool " << m_geometryManagerTool);

  // some tools are not needed if only solving is done
  if ( !m_solveOnly ) {

    // get TrackCollectionProvider
    if (m_trackCollectionProvider.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_trackCollectionProvider);
    else{
      msg(MSG::FATAL) << "Could not get " << m_trackCollectionProvider << endmsg; 
      return StatusCode::FAILURE;
    }

    // get AlignTrackPreProcessor
    if (m_alignTrackPreProcessor.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_alignTrackPreProcessor);
    else{
      msg(MSG::FATAL) << "Could not get " << m_alignTrackPreProcessor << endmsg; 
      return StatusCode::FAILURE;
    }

    // Get AlignTrackCreator tool
    if ( m_alignTrackCreator.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Failed to retrieve tool "<<m_alignTrackCreator<<endmsg;
      return StatusCode::FAILURE;
    } 
    else
      ATH_MSG_INFO("Retrieved tool "<<m_alignTrackCreator);

    // Get AlignTrackDresser tool
    if ( m_alignTrackDresser.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Failed to retrieve tool "<<m_alignTrackDresser<<endmsg;
      return StatusCode::FAILURE;
    } 
    else
      ATH_MSG_INFO("Retrieved tool "<<m_alignTrackDresser);

    // Get TrkAlign::FillTrack
    if (m_writeNtuple) {
      if ( m_fillNtupleTool.retrieve().isFailure() ) {
        msg(MSG::FATAL)<<"Failed to retrieve tool "<<m_fillNtupleTool<<endmsg;
        return StatusCode::FAILURE;
      }
      else
        ATH_MSG_INFO("Retrieved tool "<<m_fillNtupleTool);
    }
  }
  
  // Get AlignTool tool
  if ( m_alignTool.retrieve().isFailure() ) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<<m_alignTool<<endmsg;
    return StatusCode::FAILURE;
  } 
  else
    ATH_MSG_INFO("Retrieved tool "<<m_alignTool);

  // Get TrkAlignDataBaseTool  
  if ( m_trkAlignDBTool.retrieve().isFailure() ) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<<m_trkAlignDBTool<<endmsg;
    return StatusCode::FAILURE;
  } 
  else
    ATH_MSG_INFO("Retrieved tool "<<m_trkAlignDBTool);

  // Alignment Level
  // Number of layers in the superstructures
  if (m_alignSolveLevel>3 || m_alignSolveLevel<1) {
    msg(MSG::FATAL)<<"AlignSolveLevel could be 1, 2 or 3"<<endmsg;
    return StatusCode::FAILURE;
  }

  // open logfile
  if(m_writeLogfile) {
    std::ostream  * out   = &std::cout;
    std::ofstream * ofile = new std::ofstream(m_logfileName.c_str());
    if(!ofile->is_open()) {
      msg(MSG::ERROR)<<"Couldn't open logfile. Writing to standard output."<<endmsg;
      delete ofile;
    }
    else
      out = ofile;

    m_logStream = out;

    // set logStream in the tools
    m_geometryManagerTool->setLogStream(m_logStream);
    m_alignTool->setLogStream(m_logStream);
    m_trkAlignDBTool->setLogStream(m_logStream);
    if( !m_solveOnly ) {
      	 m_trackCollectionProvider->setLogStream(m_logStream);
      	 m_alignTrackPreProcessor->setLogStream(m_logStream);
     	 m_alignTrackCreator->setLogStream(m_logStream);
     	 m_alignTrackDresser->setLogStream(m_logStream);
      	 if (m_writeNtuple)
             m_fillNtupleTool->setLogStream(m_logStream);
    }
  }

  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode AlignAlg::start()
{
  ATH_MSG_DEBUG("AlignAlg::start()");

  if (m_writeNtuple) {
      m_ntuple = new TFile((m_filepath+m_filename).c_str(),"RECREATE");
      // set the ntuple points of the tools
      m_geometryManagerTool->setNtuple(m_ntuple);
      m_trackCollectionProvider->setNtuple(m_ntuple);
      m_alignTrackPreProcessor->setNtuple(m_ntuple);
      m_alignTool->setNtuple(m_ntuple);
      m_trkAlignDBTool->setNtuple(m_ntuple);
      m_fillNtupleTool->setNtuple(m_ntuple);
  }

  ATH_MSG_DEBUG("reading geometry");
  int NDoF = m_geometryManagerTool->ReadGeometry(m_alignSolveLevel);
  ATH_MSG_DEBUG("read geometry, "<<NDoF<<" alignment parameters");

  m_geometryManagerTool->PrintModPositions();

  // fix pathname (used to be m_pathbin+m+prefixName)
  // m_geometryManagerTool->StoreModPositions(pathname);

  StatusCode sc = m_alignTool->firstEventInitialize();

  if (sc != StatusCode::SUCCESS) {
    msg(MSG::FATAL)<<"problem with alignTool->firstEventInitialize()"<<endmsg;
    return StatusCode::FAILURE;
  }

  if(m_solveOnly) {
    ATH_MSG_DEBUG("getting data from files");
    m_alignTool->accumulateFromFiles();
  }

  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode AlignAlg::execute() 
{
  ATH_MSG_DEBUG("AlignAlg::execute()");

  // increment number of events
  m_nevents++;

  if(m_solveOnly) {    
    if (m_nevents<=1)
      return StatusCode::SUCCESS;    
    else { 
      // return FAILURE to make sure that the job ends after 1st event
      msg(MSG::INFO)<<"Only doing the solving. Aborting event processing after first event."<<endmsg;
      msg(MSG::INFO)<<"Following ERROR message is not a problem..."<<endmsg;
      return StatusCode::FAILURE;
    }
  }


  ///////////////////////////////////////////////////// 
  //  1.  apply event selection, select hits, refit tracks
  ///////////////////////////////////////////////////// 
  // m_trackSelector->cleanTracks();

  // loop over tracks
  TrackCollection * alignTracks;
  const TrackCollection* originalTracks;
  
  StatusCode sc = m_trackCollectionProvider->trackCollection(originalTracks);
  if (!originalTracks || sc.isFailure()) return sc;

  if(originalTracks->empty()) {
    ATH_MSG_DEBUG("found no tracks");
    return StatusCode::SUCCESS;
  }

  // increment number of processed tracks
  m_ntracks += originalTracks->size();
  ATH_MSG_DEBUG("nTracks: "<<m_ntracks);

  // process track collection and create collection of AlignTracks
  alignTracks = m_alignTrackPreProcessor->processTrackCollection(originalTracks);

  // method returns zero or empty collection
  if (!alignTracks || alignTracks->empty()) {
    ATH_MSG_DEBUG("processTrackCollection() returned empty Track collection.");
    return StatusCode::SUCCESS;
  }

  // increment number of selected tracks
  m_ntracksSel += alignTracks->size();

  // loop over AlignTracks and pass to AlignTrackCreator and Dresser for more processing
  int itrk = 0;
  for (Track* trk : *alignTracks) {                               //  first loop over AlignTracks

    ATH_MSG_DEBUG("1st loop: Processing track "<<itrk);

    // each track in the collection HAS to be an AlignTrack
    AlignTrack * alignTrack = dynamic_cast<AlignTrack*>(trk);
    if(!alignTrack) {
      msg(MSG::FATAL)<<"Track is not an AlignTrack. PreProcessor returns TrackCollection but"
                     <<"it has to be filled with AlignTracks. Otherwise the code doesn't work."<<endmsg;
      return StatusCode::FAILURE;
    }

    // process AlignTrack
    bool success = m_alignTrackCreator->processAlignTrack(alignTrack);
    if (!success) {
      ATH_MSG_DEBUG("processAlignTrack() returned FALSE, Skipping the track.");
      continue;
    }

    m_ntracksProc++;

    // dress AlignTrack with extra info
    success = m_alignTrackDresser->dressAlignTrack(alignTrack);
    ATH_MSG_DEBUG("back from dressing AlignTrack");
    
    if (!success) {
      ATH_MSG_DEBUG("dressAlignTrack() returned FALSE, Skipping the track.");
      continue;
    }
    
    m_ntracksDress++;

    // accumulate vertex fit information:
    m_alignTrackPreProcessor->accumulateVTX(alignTrack);   

    ATH_MSG_DEBUG("back from accumulateVTX");
    
  }                                                                   // end of the first loop over AlignTracks


  // solve existing vertices:
  m_alignTrackPreProcessor->solveVTX();   

                                    
  for (Track* trk : *alignTracks) {                               //  second loop over AlignTracks

    ATH_MSG_DEBUG("2nd loop: Processing tracks");

    // each track in the collection HAS to be an AlignTrack
    AlignTrack * alignTrack = dynamic_cast<AlignTrack*>(trk);
    if(!alignTrack) {
      msg(MSG::FATAL)<<"Track is not an AlignTrack. PreProcessor returns TrackCollection but"
                     <<"it has to be filled with AlignTracks. Otherwise the code doesn't work."<<endmsg;
      return StatusCode::FAILURE;
    }


    // check if the dressing was successful:
    Amg::SymMatrixX    * ptrWeights   = alignTrack->weightMatrix(); //Symmetric Matrix
    if ( !ptrWeights ) {
      ATH_MSG_DEBUG("dressAlignTrack failed, Skipping the track.");
      continue;
    }

    // check if the eventual VTX solving was successful:
    AlignVertex            * ptrVertex    = alignTrack->getVtx();
    if ( ptrVertex && (ptrVertex->type() < AlignVertex::Refitted) ) {
      ATH_MSG_DEBUG("Vertex fit failed for this track. Skipping the track.");
      continue;
    }


    // pass to AlignTool for processing of track information
    bool success = m_alignTool->accumulate(alignTrack);
    ATH_MSG_DEBUG("back from accumulate");
    
    if (!success) {
      ATH_MSG_DEBUG("accumulate() returned FALSE, Skipping the track.");
      continue;
    }
    
    ATH_MSG_DEBUG("calling fillntuple::dumptrack");

    m_ntracksAccum++;
    
    if (m_writeNtuple)
      m_fillNtupleTool->dumpTrack(itrk,alignTrack);
    ATH_MSG_DEBUG("done with track "<<itrk);

  }                                                                   // end of the second loop over AlignTracks




  // record the collection in the StoreGate
  // the ownership is transferred to StoreGate so we don't delete the collection here
  if(evtStore()->record(alignTracks,m_alignTracksName,false).isFailure()) {
    ATH_MSG_ERROR("Unable to record "<<m_alignTracksName<<" in the StoreGate");
    delete alignTracks;
    alignTracks = nullptr;
  }
  if (alignTracks) ATH_MSG_DEBUG("Recorded \'"<<m_alignTracksName<<"\' with size "<<alignTracks->size()<<" to StoreGate");

  ATH_MSG_DEBUG("returning SUCCESS");
  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode AlignAlg::stop()
{
  ATH_MSG_INFO("AlignAlg::stop()");

  if(m_logStream && !m_solveOnly) {
    *m_logStream<<"*************************************************************"<<std::endl;
    *m_logStream<<"******              Alignment Job summary              ******"<<std::endl;
    *m_logStream<<"*************************************************************"<<std::endl;
    *m_logStream<<"*"<<std::endl;
    *m_logStream<<"* number of processed events:     "<<m_nevents<<std::endl;
    *m_logStream<<"* number of processed tracks:     "<<m_ntracks<<std::endl;
    *m_logStream<<"* number of selected tracks:      "<<m_ntracksSel<<std::endl;
    *m_logStream<<"* number of tracks processed:     "<<m_ntracksProc<<std::endl;
    *m_logStream<<"* number of tracks dressed:       "<<m_ntracksDress<<std::endl;
    *m_logStream<<"* number of tracks accumulated:   "<<m_ntracksAccum<<std::endl;

    *m_logStream<<"*"<<std::endl;

     m_alignTrackPreProcessor->printSummary();
     m_trackCollectionProvider->printSummary();
  }

  // fill ntuple for all tools
  if (m_writeNtuple) {
    ATH_MSG_INFO("calling alignTool->fillNtuple()");
    if(m_alignTool->fillNtuple().isFailure()) {
      msg(MSG::ERROR)<<"problem with alignTool fillNtuple()!"<<endmsg;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("calling fillNtupleTool->fillNtuple()");
    if(m_fillNtupleTool->fillNtuple().isFailure()) {
      msg(MSG::ERROR)<<"problem with fillNtupleTool fillNtuple()!"<<endmsg;
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("calling alignTrackPreProcessor->fillNtuple()");
    if(m_alignTrackPreProcessor->fillNtuple().isFailure()) {
       msg(MSG::ERROR)<<"problem with alignTrackPreProcessor fillNtuple()!"<<endmsg;
       return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("calling trackCollectionProvider->fillNtuple()");
    if(m_trackCollectionProvider->fillNtuple().isFailure()) {
       msg(MSG::ERROR)<<"problem with trackCollectionProvider fillNtuple()!"<<endmsg;
       return StatusCode::FAILURE;
    }
  }
  
  if(!m_solveOnly && m_ntracksSel==0) {
    msg(MSG::WARNING)<<"No tracks passed track selection. No solving."<<endmsg;
    return StatusCode::SUCCESS;
  }

  ATH_MSG_INFO("calling trkAlignDBTool->preSolve()");
  if (m_trkAlignDBTool->preSolve().isFailure()) {
    msg(MSG::FATAL)<<"failure in ITrkAlignDBTool->preSolve()"<<endmsg;
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("calling alignTool->solve()");
  if (m_alignTool->solve().isFailure()) {
    msg(MSG::FATAL)<<"failure in IAlignTool->solve()"<<endmsg;
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("calling trkAlignDBTool->writeAlignPar()");
  m_trkAlignDBTool->writeAlignPar();

  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode AlignAlg::finalize() 
{
  ATH_MSG_INFO("AlignAlg::finalize()");

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
  
  if (m_writeNtuple) {
    ATH_MSG_INFO("closing "<<m_ntuple->GetName());
    m_ntuple->Close();
  }

  return StatusCode::SUCCESS;
}

  
}
