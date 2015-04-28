/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKTrack.h"

#include "TrigFTK_RawDataAlgs/FTK_RDO_CreatorAlgo.h"
#include "TrigFTKPool/FTKAthTrack.h"
#include "TrigFTKPool/FTKAthTrackContainer.h"

#include "GaudiKernel/MsgStream.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "PileUpTools/PileUpMergeSvc.h"

#include "TBranch.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"

#include <list>
#include <iostream>
#include <sstream>
#include <fstream>

#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

using namespace std;

/////////////////////////////////////////////////////////////////////////////
FTK_RDO_CreatorAlgo::FTK_RDO_CreatorAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_pmap_path(""), m_pmap(0x0),
  m_useStandalone(true),
  m_singleProces(false),
  m_doMerging(false),
  m_nregions(64), m_nsubregions(4),
  m_neventsUnmergedFiles(0),
  m_neventsMerged(0),
  m_outputTree(0),
  m_outputFile(0),
  m_merged_bank(0),
  m_HW_level(2),
  m_HW_ndiff(6),
  m_NCoords(0),
  m_HW_path(""),
  m_keep_rejected(0),
  m_SCTCoordOffset(0),
  m_ftktrack_tomerge_paths(),
  m_ftktrack_tomerge_firstregion(0),
  m_ftktrack_tomerge_firstsubreg(0),
  m_ftrtrack_tomerge_bfname("FTKTracksStream%u."),
  m_MergeRegion(-1),
  m_ftktrack_mergeoutput("ftkmerged.root"),
  m_ftktrack_mergeInputPath(""),
  m_ftktrack_mergeFileRoot(""),
  m_forceAllInput(true),
  m_doGrid(false),
  m_ftktrack_paths_merged(),
  m_mergedtracks_chain(0x0),
  m_mergedtracks_stream(0x0),
  m_mergedtracks_bname("FTKBankMerged"), /* Old standalone name is FTKBankMerged, prdosys FTKMergedTracksStream */
  m_force_merge(false),
  m_GenerateRDO(true),
  m_ftk_raw_trackcollection_Name("FTK_RDO_Tracks"),
  m_ftk_raw_trackcollection(0x0) // JWH Added to test FTK RDO
{
  declareProperty("useStandalone",m_useStandalone,"Use tracks produced from the standalone version");
  declareProperty("SingleProces",m_singleProces,"Assume in the same process with the TF");
  declareProperty("doMerging",m_doMerging,"Enable the merging");
  declareProperty("NBanks",m_nregions,"Number of regions");
  declareProperty("NSubRegions",m_nsubregions,"Number of sub-regions");
  declareProperty("mergedTrackPaths",m_ftktrack_paths_merged,
		  "Paths of the merged tracks");
  declareProperty("mergeTrackBName",m_mergedtracks_bname,"Branch name for the merged tracks");
  declareProperty("pmap_path",m_pmap_path,
		  "FTK layer configuration");
  declareProperty("force_merge",m_force_merge,"Force the merging disabling any check");
 
  declareProperty("HitWarrior",m_HW_level);
  declareProperty("HWNDiff",m_HW_ndiff);
  declareProperty("loadHWConf_path",m_HW_path); 
  declareProperty("KeepRejected",m_keep_rejected); 

  declareProperty("FTKToMergePaths",m_ftktrack_tomerge_paths);
  declareProperty("FirstRegion",m_ftktrack_tomerge_firstregion);
  declareProperty("FirstSubreg",m_ftktrack_tomerge_firstsubreg);
  declareProperty("FTKMergedOutput",m_ftktrack_mergeoutput);
  declareProperty("FTKUnmergedInputPath",m_ftktrack_mergeInputPath);
  declareProperty("FTKUnmergedFileRoot",m_ftktrack_mergeFileRoot);
  declareProperty("FTKForceAllInput",m_forceAllInput,"Require all inputFiles to be present");
  declareProperty("FTKDoGrid",m_doGrid,"Assume Naming used on grid");
  declareProperty("MergeRegion",m_MergeRegion,"If >=0 enables the merging for a single region, -1 means global merging");
  declareProperty("FTKUnmergedFormatName",m_ftrtrack_tomerge_bfname,"Format for the branch-name of the unmerged tracks");

  declareProperty("GenerateRDO",m_GenerateRDO);
}

FTK_RDO_CreatorAlgo::~FTK_RDO_CreatorAlgo()
{
  // destroy configuration
  if (m_mergedtracks_stream) delete m_mergedtracks_stream;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_CreatorAlgo::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTK_RDO_CreatorAlgo::initialize()" << endreq;


  log << MSG::VERBOSE << "useStandalone" << m_useStandalone << " - Use tracks produced from the standalone version" << endreq; 
  log << MSG::VERBOSE << "SingleProces: "<<m_singleProces<<" - Assume in the same process with the TF"<< endreq;
  log << MSG::VERBOSE << "doMerging: "<<m_doMerging<<" - Enable the merging"<< endreq;
  log << MSG::VERBOSE << "NBanks: "<<m_nregions<<" - Number of regions"<< endreq;
  log << MSG::VERBOSE << "NSubRegions: "<<m_nsubregions<<" - Number of sub-regions"<< endreq;
  log << MSG::VERBOSE << "mergedTrackPaths: "<<m_ftktrack_paths_merged<<
		  "Paths of the merged tracks: "<< endreq;
  log << MSG::VERBOSE << "mergeTrackBName: "<<m_mergedtracks_bname<<" - Branch name for the merged tracks"<< endreq;
  log << MSG::VERBOSE << "pmap_path: "<<m_pmap_path<< " - FTK layer configuration"<< endreq;
  log << MSG::VERBOSE << "force_merge: "<<m_force_merge<<" - Force the merging disabling any check"<< endreq;
 
  log << MSG::VERBOSE << "HitWarrior: "<<m_HW_level<< endreq;
  log << MSG::VERBOSE << "HWNDiff: "<<m_HW_ndiff<< endreq;
  log << MSG::VERBOSE << "loadHWConf_path: "<<m_HW_path<< endreq; 
  log << MSG::VERBOSE << "KeepRejected: "<<m_keep_rejected<< endreq; 

  log << MSG::VERBOSE << "FTKToMergePaths: "<<m_ftktrack_tomerge_paths<< endreq;
  log << MSG::VERBOSE << "FirstRegion"<<m_ftktrack_tomerge_firstregion<< endreq;
  log << MSG::VERBOSE << "FirstSubreg: "<<m_ftktrack_tomerge_firstsubreg<< endreq;
  log << MSG::VERBOSE << "FTKMergedOutput: "<<m_ftktrack_mergeoutput<< endreq;
  log << MSG::VERBOSE << "FTKUnmergedInputPath: "<<m_ftktrack_mergeInputPath<< endreq;
  log << MSG::VERBOSE << "FTKUnmergedFileRoot: "<<m_ftktrack_mergeFileRoot<< endreq;
  log << MSG::VERBOSE << "FTKForceAllInput: "<<m_forceAllInput<<" - Require all inputFiles to be present"<< endreq;
  log << MSG::VERBOSE << "FTKDoGrid: "<<m_doGrid<<" - Assume Naming used on grid"<< endreq;
  log << MSG::VERBOSE << "MergeRegion: "<<m_MergeRegion<<" - If >=0 enables the merging for a single region<< -1 means global merging"<< endreq;
  log << MSG::VERBOSE << "FTKUnmergedFormatName: "<<m_ftrtrack_tomerge_bfname<<" - Format for the branch-name of the unmerged tracks"<< endreq;

  log << MSG::VERBOSE << "GenerateRDO: "<<m_GenerateRDO<< endreq;

  
  /*TODO: this part is required only if the algo is reading from SG (in conversion mode)
   * or writing in the SG (not currently supported). This check can be relaxed
   */
  StatusCode scSG = service( "StoreGateSvc", m_StoreGate );
  if (scSG.isFailure()) {
    log << MSG::FATAL << "Unable to retrieve StoreGate service" << endreq;
    return scSG;
  }


  if (m_useStandalone && !m_doMerging) {
    // prepare the input from the FTK tracks, merged in an external simulation
    m_mergedtracks_chain = new TChain("ftkdata","Merged tracks chain");
    // add the file to the chain
    log << MSG::INFO << "Loading " << m_ftktrack_paths_merged.size() << " files with FTK merged tracks" << endreq;
    vector<string>::const_iterator imtp = m_ftktrack_paths_merged.begin();
    for (;imtp!=m_ftktrack_paths_merged.end();++imtp) {
      Int_t addres = m_mergedtracks_chain->Add((*imtp).c_str());
      log << MSG::DEBUG << "Added: " << *imtp << '[' << addres << ']' <<endreq;
    }
    m_mergedtracks_stream = new FTKTrackStream();
    TBranch *mergedtracks_branch;
    Int_t res = m_mergedtracks_chain->SetBranchAddress(m_mergedtracks_bname.c_str(),&m_mergedtracks_stream,&mergedtracks_branch);
    if (res<0) {
      log << MSG::FATAL << "Branch \"" << m_mergedtracks_bname << "\" with merged tracks not found" << endreq;
      return StatusCode::FAILURE;
    }


    // reset the counters
    m_mergedtracks_iev = 0;

    // cleanup the map between event number and vector of FTK tracks
    m_trackVectorMap.clear();

    Long64_t nEntries = m_mergedtracks_chain->GetEntries();

    int prevEventNumber(-1); // variable to check repetition of events, special warning condition
    bool hasRepeatedEvents(false); // flag to verify if the repetition of the event condition has been found

    for(Long64_t iEntry=0; iEntry<nEntries; iEntry++) {

      m_mergedtracks_chain->GetEntry(iEntry);
      int eventNumber = m_mergedtracks_stream->eventNumber();

      std::map<int, Long64_t >::iterator mapIt = m_trackVectorMap.find(eventNumber);

      if(mapIt==m_trackVectorMap.end()) {//new event
        if (!hasRepeatedEvents) {
          m_trackVectorMap.insert(std::pair<int, Long64_t >(eventNumber, iEntry ));
          prevEventNumber = eventNumber;
        }
        else {
          log << MSG::FATAL << "A duplicated event was found before the end of file, the error cannot be recoverd" << endreq;
          return StatusCode::FAILURE;
        }
      }
      else if (eventNumber==prevEventNumber) {
        log << MSG::WARNING << "Event " << eventNumber << " found in the previous event, failure condition masked if this happens at the end of the file" << endreq;
        hasRepeatedEvents = true;
      }
      else {
        // Duplicate events are a condition error at the moment, this
        // can be fixed using a runNumber and eventNumber paier as key
        log << MSG::FATAL << "Tracks for the current event (" << eventNumber << ") already exist. Duplication not allowed" << endreq;
        return StatusCode::FAILURE;
      }

    }

    log << MSG::DEBUG << "Tracks from " << m_trackVectorMap.size() << " events loaded"<<endreq;

  } // preparation for standalone track conversion block done
  else if (m_useStandalone && m_doMerging) {
    StatusCode sc = initStandaloneTracks();
    if (sc.isFailure()) {
      log << MSG::FATAL << "Unable initialize merged tracks" << endreq;
      return sc;
    }
  }
  else if (!m_singleProces) {
    /* merge the input from several POOL files, the PileUpMergeSvc is used to 
       make this task */
    if (!(service("PileUpMergeSvc", m_mergeSvc, true)).isSuccess() || 
	0 == m_mergeSvc) {
      log << MSG::ERROR << "Could not find PileUpMergeSvc" << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
    // The merger will assume the SG contains the results from NRegions and 1 sub-region
    if (m_nsubregions!=1) {
      log << MSG::ERROR << "The current code supports only 1 sub-region" << endreq;
      return StatusCode::FAILURE;
    }
  }

  // set the plane-map for the track merger
  if (m_pmap_path.empty()) {
    log << MSG::FATAL << "No FTK plane-map file is set" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endreq;
      return StatusCode::FAILURE;
    }

    // extract some general information from the PMAP file

    // get the total total dimension = total number of independent coordinates for a track
    m_NCoords = m_pmap->getTotalDim();

    // get the total number of pixel layers (assumed to be the first layers)
    for (int ip=0;ip!=m_pmap->getNPlanes();++ip) {
        if (!m_pmap->isPixel(ip)) {
            m_SCTCoordOffset = ip*2;
            break;
        }
    }
  }
  
  // Set Hit Warrior configuration when using mergeSGContent//
  if (m_doMerging){
    if (m_HW_path.empty()) {
      log << MSG::FATAL << "HWSDEV_FILE Nothing" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      m_HW_dev = new float[m_NCoords];
      for (int i=0; i!=m_NCoords ;++i) {
	m_HW_dev[i] = 0.f;
      }
      
      ifstream fin(m_HW_path.c_str());
      if (!fin) {   
	log << MSG::FATAL << "HWSDEV_FILE Nothing" << endreq;
	return StatusCode::FAILURE;
      }
      
      log << MSG::INFO << "Load Hit Warrior Configuration" << endreq;
      
      string line;
      // first line represents version, not used now
      getline(fin,line);
      // loop until an N in the first position is found,
      // this should change in the future, as in other map files
      while(getline(fin,line)) {
	if (line[0]=='N') break;
      }
      // start the real read of the file
      char flag;
      unsigned int nvals;
      int pos;
      float fval;
      fin >> flag >> nvals;
      for (unsigned int i=0;i<nvals;++i) {
	fin >> pos >> fval;
	m_HW_dev[pos] = fval;
      }
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_CreatorAlgo::execute() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "FTK_RDO_CreatorAlgo::execute() start" << endreq;

   if (m_useStandalone && !m_doMerging) {
     StatusCode sc = convertMergedTracks();
     if (sc.isFailure()) {
       log << MSG::FATAL << "Unable convert merged tracks" << endreq;
       return sc;
     }
   }
   else if (m_useStandalone && m_doMerging) {
     StatusCode sc = mergeStandaloneTracks();
     if (sc.isFailure()) {
       log << MSG::FATAL << "Unable convert merged tracks" << endreq;
       return sc;
     }
   }
   else if (m_singleProces && m_doMerging) {
     StatusCode sc = mergeSGContent();
     if (sc.isFailure()) {
       log << MSG::FATAL << "Unable merge the SG content" << endreq;
       return sc;
     }
   }
   else if (!m_useStandalone && m_doMerging) {
     StatusCode sc = mergePoolFiles();
     if (sc.isFailure()) {
       log << MSG::FATAL << "Unable merge the SG content from multiple POOL files" << endreq;
       return sc;
     }
   }
   else {
     log << MSG::ERROR << "Unmanaged merging/conversion combination useStandalone " << m_useStandalone 
	 << " singleProces " << m_singleProces << " doMerging " <<   m_doMerging << endreq;
     return StatusCode::FAILURE;
   }

   
   log << MSG::DEBUG << "FTK_RDO_CreatorAlgo::execute() end" << endreq;

   return StatusCode::SUCCESS;
}

//
//  Initialize inputput when merging standalone input
//   this mode reads unmerged tracks from a previous FTKTrackFitterAlgo 
//
StatusCode FTK_RDO_CreatorAlgo::initStandaloneTracks()
{
  MsgStream log(msgSvc(), name());

  //
  // reset the internal event counter
  //
  m_mergedtracks_iev = 0;
    
  // prepare the input from the FTK tracks, merged in an external simulation
  m_mergedtracks_tree = new TTree("ftkdata","Merged tracks chain");
  log << MSG::DEBUG << "Will Merge:  " << m_nregions << " region and "  << m_nsubregions << " subregions " <<endreq;
  log << MSG::DEBUG << "Starting with region " << m_ftktrack_tomerge_firstregion << " and subregion " << m_ftktrack_tomerge_firstsubreg << endreq;
	  

  //
  //  The output stream
  //
  m_mergedtracks_stream = new FTKTrackStream;
  m_mergedtracks_tree->Branch("FTKBankMerged",&m_mergedtracks_stream);

  //
  // allocate memory
  //
  m_ftktrack_tomerge_file   = new TFile**[m_nregions];
  m_ftktrack_tomerge_tree   = new TTree**[m_nregions];
  m_ftktrack_tomerge_branch = new TBranch**[m_nregions];
  m_ftktrack_tomerge_stream = new FTKTrackStream**[m_nregions]; 
  for (unsigned int ir=0;ir!=m_nregions;++ir) { // region loop
    m_ftktrack_tomerge_file[ir]   = new TFile*[m_nsubregions];
    m_ftktrack_tomerge_tree[ir]   = new TTree*[m_nsubregions];
    m_ftktrack_tomerge_branch[ir] = new TBranch*[m_nsubregions];
    m_ftktrack_tomerge_stream[ir] = new FTKTrackStream*[m_nsubregions];
  }

  //
  // Assume that the files are ordered, so all we need is:
  //   - the path
  //   - the first region
  //   - the number of regions
  //   - the number of subregions
  //
  if (!m_ftktrack_tomerge_paths.empty()) {
    // set the input files using a list of input files

    // initialize the merging structure   
    for (  unsigned int ireg=0; ireg!=m_nregions;   ++ireg) { // region loop
      for (unsigned int isub=0; isub!=m_nsubregions; ++isub) { // subregion loop

	//
	//  init memory
	//
	m_ftktrack_tomerge_file  [ireg][isub] = 0;
	m_ftktrack_tomerge_tree  [ireg][isub] = 0;
	m_ftktrack_tomerge_branch[ireg][isub] = 0;
	m_ftktrack_tomerge_stream[ireg][isub] = 0;
      } // end subreg loop
    } // end region loop
    // end merge structure initialization

    // calculate the offset of the file number
    const unsigned int ifile_offset = m_ftktrack_tomerge_firstregion*m_nsubregions+m_ftktrack_tomerge_firstsubreg;

    for (unsigned int ifile=0;ifile!=m_ftktrack_tomerge_paths.size();++ifile) { // file loop
      // get the current path
      const string &curfilepath = m_ftktrack_tomerge_paths[ifile];

      // identify the current region and sub-region ID, using the global file ID
      const unsigned int global_ifile = (ifile+ifile_offset);
      const unsigned int regionID(global_ifile/m_nsubregions);
      const unsigned int subregID(global_ifile%m_nsubregions);

      // associate the current file to the current (region,sub-region
      TFile *curfile  = TFile::Open(curfilepath.c_str());
      if (!curfile){
	log << MSG::FATAL << " Error opening the file: " << curfilepath << endreq;
	return StatusCode::FAILURE;
      }

      m_ftktrack_tomerge_file[regionID][subregID] = curfile;
      m_ftktrack_tomerge_tree[regionID][subregID] = (TTree*) curfile->Get("ftkdata");
      if (!m_ftktrack_tomerge_tree[regionID][subregID]) {
	log << MSG::FATAL << "TTree \"ftkdata\" not found in the file: " << curfilepath << endreq;
	return StatusCode::FAILURE;
      }

      m_ftktrack_tomerge_stream[regionID][subregID] = new FTKTrackStream();
      //m_ftktrack_tomerge_tree[regionID][subregID]->Print();
      Int_t res = m_ftktrack_tomerge_tree[regionID][subregID]->SetBranchAddress(Form(m_ftrtrack_tomerge_bfname.c_str(),regionID),&m_ftktrack_tomerge_stream[regionID][subregID],&m_ftktrack_tomerge_branch[regionID][subregID]);
      if (res<0) {
	// negative reeturn values are errors, no specific action taken, just exit
	log << MSG::FATAL << "Error connecting branch: " << Form(m_ftrtrack_tomerge_bfname.c_str(),regionID) << endreq;
	return StatusCode::FAILURE;
      }

      //
      // Set Number of events and the file name (from the first TTree)
      //
      if (m_neventsUnmergedFiles==0) {
	m_neventsUnmergedFiles = m_ftktrack_tomerge_tree[regionID][subregID]->GetEntries();
	log << MSG::DEBUG << "Setting nevents to " << m_neventsUnmergedFiles << " (with regionNum:" << regionID << " subregionNum: " << subregID << " ) " << endreq;
      }
      
    } // end file loop
  } // end merge using a list of input files
  else { 
    // Merge all the regions (correct for offset)
    //
    for (  unsigned int ireg=0; ireg!=m_nregions;   ++ireg) { // region loop

      //
      //  Get the region number including the offset
      //
      unsigned int regNum = ireg + m_ftktrack_tomerge_firstregion;

      // 
      // Merge all the subregions
      //
      for (unsigned int isub=0; isub!=m_nsubregions; ++isub) { // subregion loop

	//
	//  Get the subregion number including the offset
	//
	unsigned int subNum = isub + m_ftktrack_tomerge_firstsubreg;

	//
	//  init memory
	//
	m_ftktrack_tomerge_file  [ireg][isub] = 0;
	m_ftktrack_tomerge_tree  [ireg][isub] = 0;
	m_ftktrack_tomerge_branch[ireg][isub] = 0;
	m_ftktrack_tomerge_stream[ireg][isub] = 0;

	//
	// Path to files being merged
	//
	//m_ftktrack_mergeInputPath = "/afs/cern.ch/work/g/gvolpi/ftksimoutput20120918/";
	std::string thisPath = GetPathName(regNum,subNum);
      
	if(!m_doGrid){
	  //
	  //  Check that the intput file exists
	  //
	  ifstream testStream(thisPath.c_str());
	  if(!testStream.good()){
      
	    // 
	    // If not file doesnt exist try with only one digit
	    //
	    log << MSG::VERBOSE << " Could not read " << thisPath << endreq;
	    thisPath = m_ftktrack_mergeInputPath + "/"+m_ftktrack_mergeFileRoot + Form("_reg%d_sub%d.root",regNum,subNum);
	    log << MSG::VERBOSE << " Trying  " << thisPath << endreq;

	    testStream.close();
	    testStream.open(thisPath.c_str());
	
	    if(!testStream.good()){
	      log << MSG::VERBOSE << thisPath << " also failed" << endreq;
	      log << MSG::WARNING << " Could not read " << GetPathName(regNum,subNum)  
		  << " or " << thisPath << endreq;

	      if(m_forceAllInput){
		log << MSG::FATAL << " Missing inputFile, " << thisPath << " Failing... " << endreq;
		return StatusCode::FAILURE;
	      }else{
		log << MSG::WARNING << " Missing inputFile, " << thisPath << " Skipping... " << endreq;
		continue;
	      }
	    }else{
	      log << MSG::VERBOSE << " InputFile, " << thisPath << " Found " << endreq;
	    }
	  }
	  testStream.close();
	}

	//
	// this will become equal the filename of the first valid region
	//
	string merged_fname("merged_file.root");	

	if (thisPath.empty()) continue;
	log << MSG::INFO << "Opening: " << thisPath << endreq;
	
	//
	//  Get the TFile and the TTree
	//
	m_ftktrack_tomerge_file[ireg][isub] = TFile::Open(thisPath.c_str());
	m_ftktrack_tomerge_tree[ireg][isub] = (TTree*) m_ftktrack_tomerge_file[ireg][isub]->Get("ftkdata");
	if (!m_ftktrack_tomerge_tree[ireg][isub]) {
	  log << MSG::ERROR << "*** Error: TTree in file \"" << thisPath << "\" not found" << endreq;
	  return StatusCode::FAILURE;
	}
	
	//
	//  Create the Stream
	//
	m_ftktrack_tomerge_stream[ireg][isub] = new FTKTrackStream();
	
	// JOHNDA: Not sure which one we want here.
	//   assume output of track fitter for now
	bool input_tree_merged = false;
	if(input_tree_merged){ // if merging output of track_merger
	  m_ftktrack_tomerge_tree[ireg][isub]->Print();
	  m_ftktrack_tomerge_tree[ireg][isub]->SetBranchAddress("FTKBankMerged",&m_ftktrack_tomerge_stream[ireg][isub],&m_ftktrack_tomerge_branch[ireg][isub]);
	}else {                 
	  //
	  // Try to find branches named after teh region
	  //
	  if(m_ftktrack_tomerge_tree[ireg][isub]->FindBranch(Form("FTKTracksStream%d.",regNum))){
	    log << MSG::VERBOSE << "Setting branch with region number: " << regNum << " ireg: " << ireg << " isub: " << isub << endreq;
	    m_ftktrack_tomerge_tree[ireg][isub]->SetBranchAddress(Form("FTKTracksStream%d.",regNum),&m_ftktrack_tomerge_stream[ireg][isub],&m_ftktrack_tomerge_branch[ireg][isub]);

	    //
	    // If not, Try to find branches named after merging has been done
	    //

	  }else{
	    log << MSG::DEBUG << "Setting branch with name: FTKMergedTracksStream" << endreq;
	    m_ftktrack_tomerge_tree[ireg][isub]->SetBranchAddress("FTKMergedTracksStream",&m_ftktrack_tomerge_stream[ireg][isub],&m_ftktrack_tomerge_branch[ireg][isub]);
	  }
	}

	//
	// Set Number of events and the file name (from the first TTree)
	//
	if (m_neventsUnmergedFiles==0) {
	  m_neventsUnmergedFiles = m_ftktrack_tomerge_tree[ireg][isub]->GetEntries();
	  log << MSG::DEBUG << "Setting nevents to " << m_neventsUnmergedFiles << " (with regionNum:" << regNum << " subregionNum: " << subNum << " ) " << endreq;
	  merged_fname = gSystem->BaseName(thisPath.c_str());
	}

      }// subregions
    }// regions
  } // end by position merge

  //
  // Create the merged file
  //
  string ofname = Form("%s",m_ftktrack_mergeoutput.c_str());
  m_outputFile = TFile::Open(ofname.c_str(),"recreate");

  //
  // create the TTree and the branches
  //
  m_outputTree = new TTree("ftkdata","FTK Simulation Results (merged)");
  m_merged_bank = new FTKTrackStream;
  const int TREE_ROAD_BUFSIZE = 16000;
  if (m_MergeRegion==-1) { 
    // this represent a global merging
    m_outputTree->Branch("FTKMergedTracksStream",&m_merged_bank,TREE_ROAD_BUFSIZE);
  }
  else {
    // in this case only a partial merge, on a single region is assumed
    m_outputTree->Branch(Form("FTKMergedTracksStream%d",m_MergeRegion),&m_merged_bank,TREE_ROAD_BUFSIZE);
  }

  return StatusCode::SUCCESS;
}

//
//  The merging of teh standalone input
//
StatusCode FTK_RDO_CreatorAlgo::mergeStandaloneTracks()
{
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "Merging event " << m_mergedtracks_iev << endreq;
  
  //
  // Check if there are events left to merge
  //
  if(m_neventsMerged > m_neventsUnmergedFiles){
    log << MSG::INFO << "Merged all " << m_neventsUnmergedFiles << " events, nothing left to do." << endreq;
    return StatusCode::SUCCESS;    
  }

  // JWH Testing adding to storegate ///
  if (m_GenerateRDO) {
    m_ftk_raw_trackcollection = new FTK_RawTrackContainer;

    StatusCode scJ = m_StoreGate->record(m_ftk_raw_trackcollection, m_ftk_raw_trackcollection_Name);
    if (scJ.isFailure()) {
      log << MSG::FATAL << "Failure registering FTK_RawTrackContainer" << endreq;
      return StatusCode::FAILURE;
    } else{
      log << MSG::DEBUG << "Setting FTK_RawTrackContainer registered" << endreq;
    }
  }

  //
  //  Set event index
  //
  const unsigned int &iev = m_neventsMerged;

  log << MSG::VERBOSE << "Getting entries " << endreq;
  for (  unsigned int ireg=0; ireg!=m_nregions;   ++ireg) {
    for (unsigned int isub=0; isub!=m_nsubregions;  ++isub) {
      if(!m_ftktrack_tomerge_tree[ireg][isub]){
	log << MSG::VERBOSE << "Tree does not exist. ireg: " << ireg << " isub: " << isub << " iev: " << iev << endreq;
	continue;
      }
      log << MSG::VERBOSE << "ireg: " << ireg << " isub: " << isub << " iev: " << iev << endreq;
      log << MSG::VERBOSE << m_ftktrack_tomerge_branch[ireg][isub] << endreq;
      m_ftktrack_tomerge_branch[ireg][isub]->GetEntry(iev);
    }
  }
  log << MSG::VERBOSE << "Got entries " << endreq;
      
  // clear clones for this bank
  m_merged_bank->clear();
	
  // update roads[ib] summing, after the RW, the original roads
  merge_tracks(m_merged_bank, m_ftktrack_tomerge_stream);
      
  // fill the completed results
  if (m_GenerateRDO) {
      // copy the results in a RDO file
      for (int itrk=0;itrk!=m_merged_bank->getNTracks();++itrk) {
          const FTKTrack *cutrack = m_merged_bank->getTrack(itrk);
          m_ftk_raw_trackcollection->push_back(SimToRaw(*cutrack));
      }
  }
  else {
      m_outputTree->Fill();
  }

  //
  // Increment counters
  //  
  m_neventsMerged += 1;
  m_mergedtracks_iev+=1;

  return StatusCode::SUCCESS;
}

//
//  Cleaning up the standalone input
//
StatusCode FTK_RDO_CreatorAlgo::finalizeStandaloneTracks()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalizeStandaloneTracks() " << endreq;

  //
  // write, close, and destroy the output file
  //
  log << MSG::DEBUG << "writing output " << m_outputFile << " " << m_MergeRegion << " " << m_neventsUnmergedFiles << endreq;
  m_outputFile->Write();
  log << MSG::DEBUG << "closing file " << endreq;
  m_outputFile->Close();
  log << MSG::DEBUG << "deleting output " << endreq;
  delete m_outputFile;

  //
  // close input files
  //
  log << MSG::DEBUG << "Closing input " << endreq;
  set<TFile*> already_deleted;
  for( unsigned int ireg=0; ireg!=m_nregions; ++ireg ) { 
    for( unsigned int isub=0;  isub!=m_nsubregions;  ++isub ) { 

      if( !(m_ftktrack_tomerge_file[ireg][isub]) ) 
	continue; 
	
      if( already_deleted.find( m_ftktrack_tomerge_file[ireg][isub] ) != already_deleted.end() ) {
	m_ftktrack_tomerge_file[ireg][isub] = 0;
	continue; 
      }
	
      already_deleted.insert( m_ftktrack_tomerge_file[ireg][isub] );
      delete m_ftktrack_tomerge_file[ireg][isub];
      m_ftktrack_tomerge_file[ireg][isub] = 0;
    }
  }

  //
  // clean up other used memory;
  //
  log << MSG::DEBUG << "Clean up memory " << endreq;
  if( m_merged_bank ) {
    delete m_merged_bank; 
    m_merged_bank = 0; 
  }
     
  log << MSG::DEBUG << "Clean up track stream " << endreq;
  for (unsigned int ireg=0;ireg!=m_nregions;++ireg) { // region loop
    for (unsigned int isub=0;isub!=m_nsubregions;++isub) { // subregion loop
      if( m_ftktrack_tomerge_stream[ireg][isub] ) { 
	delete m_ftktrack_tomerge_stream[ireg][isub]; 
	m_ftktrack_tomerge_stream[ireg][isub] = 0; 
      }
    }
  }

  log << MSG::DEBUG << "Return success " << endreq;
  return StatusCode::SUCCESS;
}

StatusCode FTK_RDO_CreatorAlgo::mergeSGContent()
{
  MsgStream log(msgSvc(), name());

  // Get information on the events
  const EventInfo* eventInfo(0);
  if( m_StoreGate->retrieve(eventInfo).isFailure() ) {
    log << MSG::ERROR << "Could not retrieve event info" << endreq;
    return StatusCode::FAILURE;
  }
  const EventID* eventID( eventInfo->event_ID() );
  
  log << MSG::DEBUG
      << "entered execution for run " << eventID->run_number()
      << "   event " << eventID->event_number()
      << endreq;

  list<FTKAthTrack> SGMergedList; // list of merged tracks, std::list needed because HW may reject already included tracks

  for (unsigned int ibank=0;ibank!=m_nregions;++ibank) { // banks loop
    // compose the track container name
    ostringstream track_contname;
    track_contname << "FTKTracks" << ibank << ends;
    const char *Ccontname = track_contname.str().c_str();

    if (m_StoreGate->contains<FTKAthTrackContainer>(Ccontname)) {
      const DataHandle<FTKAthTrackContainer> ftktracks_cont;
      StatusCode sc = m_StoreGate->retrieve(ftktracks_cont,Ccontname);
      if (!sc.isSuccess()) {
	log << MSG::ERROR << "Could not get tracks in region " << ibank << endreq;
	return StatusCode::FAILURE;
      }

      log << MSG::DEBUG << "Number of tracks for region " << ibank << " : " << ftktracks_cont->size() << endreq;

      for (FTKAthTrackContainer::const_iterator itrack = ftktracks_cont->begin(); itrack != ftktracks_cont->end(); ++itrack) {

	/// Check you can actually make RDO tracks? ///
	
	/* TODO: before to insert this track in the SGMergedList the hit-warrior needs to be applied.
	   To do that the current track has to be compared with all the tracks in the current list,
	   decided if there is a match. In case of a match only 1 track is accepted: the new one, removing the old one, or the old one, avoiding to add the new one */

	// Hit Warrior //
	FTKAthTrack &track_cur = **itrack;
	// remains 0 if the track has to be added
	// -1 means is worse than an old track (duplicated)
	int accepted(0);
	
	list<FTKAthTrack>::iterator imtrk = SGMergedList.begin();
	// Disable hitwarrior according the flags
	if (m_HW_level==0 || track_cur.getHWRejected()/100!=0)
	  imtrk=SGMergedList.end();
	
	for (;imtrk!=SGMergedList.end();++imtrk) { // loop over tracks of this bank
	  // reference to an old track
	  FTKAthTrack &track_old = *imtrk;
	  if (track_old.getHWRejected()%10 !=0 || track_old.getHWRejected()/100!=0) 
	    // skip HW rejected tracks and tracks with bad chi2,
	    // tracks in removed roads are used, but if a match is found 
	    // will be always rejected or marked as removed, if compared
	    // to a combination in a good road
	    continue; 
	  
	  int HWres = track_cur.HWChoice(track_old,m_HW_dev,m_HW_ndiff,m_HW_level);
  	  
	  if (HWres==-1) {
	    accepted = -1;
	    // passing the end of the list means remove current
	    removeAthTrack(SGMergedList,SGMergedList.end(),track_cur,track_old,true);
	  }
	  else if (HWres==1) {
	    // return the new current position, it changes if the
	    // rejected track is removed from the list
	    imtrk = removeAthTrack(SGMergedList,imtrk,track_old,track_cur);
	  }
	} // end loop over tracks of this bank
	
	// create a copy of the track into the merged list
	if (accepted==0) {
	  SGMergedList.push_back(track_cur);
	}
	else if (m_keep_rejected>0) {
	  SGMergedList.push_back(track_cur);	
	}
	//SGMergedList.push_back(**itrack); //old
 
      }
    }
    else {
      log << MSG::DEBUG << "No tracks in region " << ibank  << endreq;
    }
  } // end banks loop


  m_ftk_raw_trackcollection = new FTK_RawTrackContainer;

  StatusCode scJ = m_StoreGate->record(m_ftk_raw_trackcollection, m_ftk_raw_trackcollection_Name);
  if (scJ.isFailure()) {
    log << MSG::FATAL << "Failure registering FTK_RawTrackContainer" << endreq;
    return StatusCode::FAILURE;
  } else{
    log << MSG::DEBUG << "Setting FTK_RawTrackContainer registered" << endreq;
  }
  
  FTKAthTrackContainer *SGFinalContainer = new FTKAthTrackContainer;

  list<FTKAthTrack>::iterator imtrack = SGMergedList.begin();
  for (;imtrack!=SGMergedList.end();imtrack = SGMergedList.erase(imtrack)) {
    FTKAthTrack &curtrk = (*imtrack);
    
    //JWH 
    FTK_RawTrack* jay_track = new FTK_RawTrack();
    jay_track->setD0(curtrk.getIP());
    jay_track->setPhi(curtrk.getPhi());
    jay_track->setZ0(curtrk.getZ0());
    m_ftk_raw_trackcollection->push_back(jay_track);

    // fill the final list of tracks in the custom format
    // TODO: add a flag to skip this step
    
    SGFinalContainer->push_back(new FTKAthTrack(*imtrack));
    


  }

  StatusCode sc = m_StoreGate->record(SGFinalContainer,"FTKTracksFinal");
  if (sc.isFailure()) {
    log << MSG::FATAL << "Failure registering final FTK collection" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Setting final FTK collection registered" << endreq;
  }

  


  return StatusCode::SUCCESS;


}


/** rejected, or mark as rejected, the track in the list
    against  the second track */
list<FTKAthTrack>::iterator FTK_RDO_CreatorAlgo::removeAthTrack(list<FTKAthTrack> &tracks_list, 
								list<FTKAthTrack>::iterator itrack,
								FTKAthTrack &rejtrk, 
								const FTKAthTrack &killer, 
								bool rejnew)
{
  
  if (!rejnew) { // is rejecting a track in the list
    if (!m_keep_rejected) { // remove from the list the rejected
      // save the position before this
      list<FTKAthTrack>::iterator before = itrack;
      // move the to the previous track
      --before;

      // remove the old track from the list
      tracks_list.erase(itrack);

      // assign the itrack to the previous position before, to not invalidate the loop
      itrack = before;
    }
    else if  (rejtrk.getHWRejected()%10==0) { // mark only if not yet marked
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());
      //nfits_rej += 1;  //This is not necessary in StoreGate 
    }

  }
  else {  // is rejecting the new track, not yet in the list
    if (rejtrk.getHWRejected()%10==0) { // mark only the first time
      // the new track is rejected
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());    

      //nfits_rej += 1; //This is not necessary in StoreGate 
    }
  }

  return itrack;
}



StatusCode FTK_RDO_CreatorAlgo::mergePoolFiles()
{
   MsgStream log(msgSvc(), name());

   StatusCode sc;

   // Get information on the events
   const EventInfo* eventInfo(0);
   if( m_StoreGate->retrieve(eventInfo).isFailure() ) {
     log << MSG::ERROR << "Could not retrieve event info" << endreq;
     return StatusCode::FAILURE;
   }
   const EventID* eventID( eventInfo->event_ID() );

   log << MSG::DEBUG
       << "entered execution for run " << eventID->run_number()
       << "   event " << eventID->event_number()
       << endreq;

  //Put all available ExampleClass with key "MyData" in alist
  typedef PileUpMergeSvc::TimedList<FTKAthTrackContainer>::type FTKAthTrackContainerList;
  FTKAthTrackContainerList unmerged_tracks;

  if (!(m_mergeSvc->retrieveSubEvtsData("FTKTracks0", unmerged_tracks).isSuccess())) {
    log << MSG::ERROR << "Could not fill FTKTracks0" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << unmerged_tracks.size() 
	<< " ExampleClassList with key " << "FTKTracks0"
	<< " found" << endreq;
  }


  return StatusCode::SUCCESS;
}


/** This method converts FTKTracks from a merged file in POOL
 * formats: TrigInDetTrack and TrackParticle.
 * The position of the FTKTrack in the input TChain is obtained
 * from a map. */
StatusCode FTK_RDO_CreatorAlgo::convertMergedTracks()
{
  MsgStream log(msgSvc(), name());

  // Get information on the events
  const EventInfo* eventInfo(0);
  if( m_StoreGate->retrieve(eventInfo).isFailure() ) {
    log << MSG::ERROR << "Could not retrieve event info" << endreq;
    return StatusCode::FAILURE;
  }
  const EventID* eventID( eventInfo->event_ID() );
  int eventNumber =  eventID->event_number();

  log << MSG::DEBUG
      << "entered execution for run " << eventID->run_number()
      << "   event " << eventNumber
      << endreq;
  
  // Extract the vector of tracks found by the FTK for the current event
  std::map<int, Long64_t >::iterator mapIt = m_trackVectorMap.find(eventNumber);
  if(mapIt == m_trackVectorMap.end()) {
    log << MSG::ERROR << "No FTK tracks for event " << eventNumber << " found, possible mismatch between RDO and FTK files"<<endreq;
    return StatusCode::FAILURE;
  }
  
  //Have Tracks
  const Long64_t &FTKEntry = (*mapIt).second;
  if (m_mergedtracks_chain->GetEntry(FTKEntry)==-1) {
   log << MSG::ERROR << "Error reading the FTK entry: " << FTKEntry <<endreq;
    return StatusCode::FAILURE;
  }
 
  int ntracks_merged = m_mergedtracks_stream->getNTracks();
  log << MSG::INFO << "Number of FTK tracks to merge in SG: " << ntracks_merged << endreq;
 


  if (m_GenerateRDO) {
    m_ftk_raw_trackcollection = new FTK_RawTrackContainer;
    
    StatusCode scJ = m_StoreGate->record(m_ftk_raw_trackcollection, m_ftk_raw_trackcollection_Name);
    if (scJ.isFailure()) {
      log << MSG::FATAL << "Failure registering FTK_RawTrackContainer" << endreq;
      return StatusCode::FAILURE;
    } else{
      log << MSG::DEBUG << "Setting FTK_RawTrackContainer registered" << endreq;
    }
  }

  
  //
  // Loop on merged tracks
  //
  for(int itrack=0;itrack!=ntracks_merged;++itrack) {
    
    // get the current track
    FTKTrack *curtrk = m_mergedtracks_stream->getTrack(itrack);
    
    // check RDO are only created from final tracks 
    if (curtrk->getHWRejected()) continue;
    
    m_ftk_raw_trackcollection->push_back(SimToRaw(*curtrk));

  } // end merged tracks loop

  // prepare to move to the next event
  m_mergedtracks_iev += 1;  
  
  return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_CreatorAlgo::finalize() {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize()" << endreq;

    //
    //  Clean up output and input merged files 
    //
    if (m_useStandalone && m_doMerging) {
      log << MSG::DEBUG << "finalize standalone tracks" << endreq;
      StatusCode sc = finalizeStandaloneTracks();
      if (sc.isFailure()) {
	log << MSG::FATAL << "Unable finalize merged tracks" << endreq;
	return sc;
      }
    }


    // cleanup the map containing the list of FTK tracks found in each event
    log << MSG::DEBUG << "Dump Track map" << endreq;
    m_trackVectorMap.clear();

    log << MSG::DEBUG << "end Finalize() " << endreq;
    return StatusCode::SUCCESS;
}





/** rejected, or mark as rejected, the track in the list
    against  the second track */
list<FTKTrack>::iterator FTK_RDO_CreatorAlgo::removeFTKTrack(list<FTKTrack> &tracks_list, list<FTKTrack>::iterator itrack,
				     FTKTrack &rejtrk, const FTKTrack &killer, bool rejnew)
{
  
  if (!rejnew) { // is rejecting a track in the list
    if (!m_keep_rejected) { // remove from the list the rejected
      // save the position before this
      list<FTKTrack>::iterator before = itrack;
      // move the to the previous track
      --before;

      // remove the old track from the list
      tracks_list.erase(itrack);

      // assign the itrack to the previous position before, to not invalidate the loop
      itrack = before;
    }
    else if  (rejtrk.getHWRejected()%10==0) { // mark only if not yet marked
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());
      m_nfits_rej += 1;
    }

  }
  else {  // is rejecting the new track, not yet in the list
    if (rejtrk.getHWRejected()%10==0) { // mark only the first time
      // the new track is rejected
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());    

      m_nfits_rej += 1;
    }
  }

  return itrack;
}


/** this function merges the tracks coming from the banks produced
    fitting the roads in a single banks, the output will have tracks
    from roads found in all the detectors*/
void FTK_RDO_CreatorAlgo::merge_tracks(FTKTrackStream* &merged_tracks, FTKTrackStream ***reg_tracks)
{
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "In merged_tracks " << endreq;

  list<FTKTrack> tracks_list;
  
  // reset rejected fits counter
  m_nfits_rej = 0;

  //
  // copy run and event number to the output, and check that the
  // banks are in sync.
  //
  unsigned long bank_run = 0ul;
  unsigned long bank_event = 0ul;
  for (unsigned int ireg=0; ireg!=m_nregions;++ireg) { // banks loop
    for ( unsigned int isub=0; isub!=m_nsubregions;++isub) { // subregions loop
      if(!reg_tracks[ireg][isub]) continue;
      unsigned long this_run = reg_tracks[ireg][isub]->runNumber();
      unsigned long this_event = reg_tracks[ireg][isub]->eventNumber();
      //if( this_run==0 && this_event==0 ) { continue; }
      bank_run = this_run;
      bank_event = this_event;
      break;
    }
  }
  merged_tracks->setRunNumber( bank_run );
  merged_tracks->setEventNumber( bank_event );
  //if( bank_run!=0 || bank_event!=0 ) {
  bool event_sync = true;
  for(unsigned int ireg=0; ireg!=m_nregions; ++ireg) {
    for (unsigned int isub=0; isub!=m_nsubregions;++isub) {
      if(!reg_tracks[ireg][isub]) continue;
      unsigned long this_run = reg_tracks[ireg][isub]->runNumber();
      unsigned long this_event = reg_tracks[ireg][isub]->eventNumber();
      if( this_run != bank_run || this_event != bank_event ) {
	event_sync = false;
	break;
      }
    }
  }
  if( !event_sync ) {
    std::cout << "BANKS ARE OUT OF SYNC!!!" << std::endl;
    std::cout << "Bank\t\tSubregion\t\tRun\t\tEvent!" << std::endl;
    for(unsigned int ireg=0; ireg!=m_nregions; ++ireg) {
      for(unsigned int isub=0; isub!=m_nsubregions; ++isub) {
	std::cout << ireg << "\t\t" << isub << "\t\t" << reg_tracks[ireg][isub]->runNumber() << "\t\t" << reg_tracks[ireg][isub]->eventNumber() << std::endl;
      }
    }
    return;
  }

  //
  //  The Merging 
  //
  for (unsigned int ireg=0;ireg!=m_nregions;++ireg) { // bank loop
    for (unsigned int isub=0; isub!=m_nsubregions;++isub) { // subregions loop
      if(!reg_tracks[ireg][isub]) continue;
      
      unsigned int ntracks = reg_tracks[ireg][isub]->getNTracks();
      
      merged_tracks->addNCombs(reg_tracks[ireg][isub]->getNCombs());
      merged_tracks->addNFits( reg_tracks[ireg][isub]->getNFits());
      merged_tracks->addNFitsMajority(reg_tracks[ireg][isub]->getNFitsMajority());
      merged_tracks->addNFitsRecovery(reg_tracks[ireg][isub]->getNFitsRecovery());
      merged_tracks->addNAddFitsRecovery(reg_tracks[ireg][isub]->getNAddFitsRecovery());
      merged_tracks->addNFitsBad(reg_tracks[ireg][isub]->getNFitsBad());
      merged_tracks->addNFitsHWRejected(reg_tracks[ireg][isub]->getNFitsHWRejected());
      merged_tracks->addNFitsBadMajority(reg_tracks[ireg][isub]->getNFitsBadMajority());
      merged_tracks->addNFitsHWRejectedMajority(reg_tracks[ireg][isub]->getNFitsHWRejectedMajority());


      unsigned int ntracksI = reg_tracks[ireg][isub]->getNTracksI();
      merged_tracks->addNCombsI(reg_tracks[ireg][isub]->getNCombsI());
      merged_tracks->addNFitsI(reg_tracks[ireg][isub]->getNFitsI());
      merged_tracks->addNFitsMajorityI(reg_tracks[ireg][isub]->getNFitsMajorityI());
      merged_tracks->addNFitsRecoveryI(reg_tracks[ireg][isub]->getNFitsRecoveryI());
      merged_tracks->addNAddFitsRecoveryI(reg_tracks[ireg][isub]->getNAddFitsRecoveryI());
      merged_tracks->addNFitsBadI(reg_tracks[ireg][isub]->getNFitsBadI());
      merged_tracks->addNFitsHWRejectedI(reg_tracks[ireg][isub]->getNFitsHWRejectedI());
      merged_tracks->addNFitsBadMajorityI(reg_tracks[ireg][isub]->getNFitsBadMajorityI());
      merged_tracks->addNFitsHWRejectedMajorityI(reg_tracks[ireg][isub]->getNFitsHWRejectedMajorityI());
      
      for (unsigned int itr=0;itr!=ntracks;++itr) { // track loop

	// get the track from the bank
	FTKTrack &newtrack = *(reg_tracks[ireg][isub]->getTrack(itr));

	//	FTK_RawTrack* test;
	//	std::cout << "JWH TEST TRACK BEFORE 2 " << std::endl;	
	//	std::cout << "Track Collection =  " << m_ftk_raw_trackcollection << std::endl;	
	//	m_ftk_raw_trackcollection->push_back(test);
	//	std::cout << "JWH TEST TRACK AFTER 2" << std::endl;

	// remains 0 if the track has to be added
	// -1 means is worse than an old track (duplicated)
	int accepted(0);
	
	list<FTKTrack>::iterator itrack = tracks_list.begin();
	// Disable hitwarrior according the flags
	if (m_HW_level==0 || newtrack.getHWRejected()/100!=0)
	  itrack=tracks_list.end();
	
	for (;itrack!=tracks_list.end();++itrack) { // loop over tracks of this bank
	  // reference to an old track
	  FTKTrack &track_old = *itrack;
	  if (track_old.getHWRejected()%10 !=0 || track_old.getHWRejected()/100!=0) 
	    // skip HW rejected tracks and tracks with bad chi2,
	    // tracks in removed roads are used, but if a match is found 
	    // will be always rejected or marked as removed, if compared
	    // to a combination in a good road
	    continue; 
	  
	  int HWres = newtrack.HWChoice(track_old,m_HW_dev,m_HW_ndiff,m_HW_level);
	  
	  if (HWres==-1) {
	    accepted = -1;
	    // passing the end of the list means remove current
	    removeFTKTrack(tracks_list,tracks_list.end(),newtrack,track_old,true);
	  }
	  else if (HWres==1) {
	    // return the new current position, it changes if the
	    // rejected track is removed from the list
	    itrack = removeFTKTrack(tracks_list,itrack,track_old,newtrack);
	  }
	  
	} // end loop over tracks of this bank

     
	// create a copy of the track into the merged list
	if (accepted==0) {
	  tracks_list.push_back(newtrack);
	}
	else if (m_keep_rejected>0) {
	  tracks_list.push_back(newtrack);	
	}
      } // end track loop



      for (unsigned int itrI=0;itrI!=ntracksI;++itrI) { // intermediate track loop
	// get the track from the bank
	FTKTrack &newtrack = *(reg_tracks[ireg][isub]->getTrackI(itrI));
	
	// the intermediate tracks are not filtered by HW
	merged_tracks->addTrackI(newtrack);
      } // end intermediate track loop

    } // end subregion loop
  } // end bank loop
  
  // synchronize the rejected counter
  merged_tracks->addNFitsHWRejected(m_nfits_rej);
  
  // push the tracks into the output stream
  list<FTKTrack>::iterator itrack = tracks_list.begin();
  for (;itrack!=tracks_list.end();itrack = tracks_list.erase(itrack)) { // loop over merged tracks
    merged_tracks->addTrack(*itrack);
  } // end loop over merged tracks
}


//
//  Format the input file names
//
std::string FTK_RDO_CreatorAlgo::GetPathName(unsigned int regNum, unsigned int subNum)
{
  if(m_doGrid){
    unsigned int fileIndex = m_nsubregions * regNum + subNum;
    std::string thisPath = m_ftktrack_mergeFileRoot + Form(".%06d.root.1",fileIndex);
    return thisPath;
  }

  std::string thisPath = m_ftktrack_mergeInputPath + "/"+ m_ftktrack_mergeFileRoot + Form("_reg%d_sub%02d.root",regNum,subNum);

  return thisPath;
}

FTK_RawTrack* FTK_RDO_CreatorAlgo::SimToRaw(const FTKTrack &track)
{
  // Words for Track Parameters and cluster objects//
  uint32_t word_1(0), 
    word_2(0), 
    word_3(0), 
    word_4(0), 
    word_5(0);
  /*  FTK_RawPixelCluster* pixel_cluster_1 = new FTK_RawPixelCluster(0);
  FTK_RawPixelCluster* pixel_cluster_2 = new FTK_RawPixelCluster(1);
  FTK_RawPixelCluster* pixel_cluster_3 = new FTK_RawPixelCluster(2);
  FTK_RawPixelCluster* pixel_cluster_4 = new FTK_RawPixelCluster(3);*/
  FTK_RawPixelCluster pixel_cluster_1(0), pixel_cluster_2(1), pixel_cluster_3(2), pixel_cluster_4(3);

  FTK_RawSCT_Cluster  sct_cluster_1(4), 
    sct_cluster_2(5), 
    sct_cluster_3(6), 
    sct_cluster_4(7), 
    sct_cluster_5(8), 
    sct_cluster_6(9), 
    sct_cluster_7(10), 
    sct_cluster_8(11);

  //  std::vector<FTK_RawPixelCluster*> pixel_clusters;
  std::vector<FTK_RawPixelCluster> pixel_clusters;
  std::vector<FTK_RawSCT_Cluster>  sct_clusters;
  bool p1(0), p2(0), p3(0), p4(0);
  bool f1(0), f2(0), f3(0), f4(0), f5(0), f6(0), f7(0), f8(0);
  for (int i = 0; i<track.getNPlanes(); ++i){
    const FTKHit& hit = track.getFTKHit(i);
    int type = hit.getDim();
    if ( type == 1){ // SCT
      if( i == 4 ){
	f1 = true;
	sct_cluster_1.setModuleID(hit.getIdentifierHash());
	sct_cluster_1.setHitCoord(hit.getCoord(0)); // loc pos
	sct_cluster_1.setHitWidth(hit.getNStrips()); // width
      } else if( i == 5 ){
	f2 = true;
	sct_cluster_2.setModuleID(hit.getIdentifierHash());
	sct_cluster_2.setHitCoord(hit.getCoord(0));
	sct_cluster_2.setHitWidth(hit.getNStrips());
      } else if( i == 6 ){
	f3 = true;
	sct_cluster_3.setModuleID(hit.getIdentifierHash());
	sct_cluster_3.setHitCoord(hit.getCoord(0));
	sct_cluster_3.setHitWidth(hit.getNStrips());
      } else if( i == 7 ){
	f4 = true;
	sct_cluster_4.setModuleID(hit.getIdentifierHash());
	sct_cluster_4.setHitCoord(hit.getCoord(0));
	sct_cluster_4.setHitWidth(hit.getNStrips());
      } else if( i == 8 ){
	f5 = true;
	sct_cluster_5.setModuleID(hit.getIdentifierHash());
	sct_cluster_5.setHitCoord(hit.getCoord(0));
	sct_cluster_5.setHitWidth(hit.getNStrips());
      } else if( i == 9 ){
	f6 = true;
	sct_cluster_6.setModuleID(hit.getIdentifierHash());
	sct_cluster_6.setHitCoord(hit.getCoord(0));
	sct_cluster_6.setHitWidth(hit.getNStrips());
      } else if( i == 10 ){
	f7 = true;
	sct_cluster_7.setModuleID(hit.getIdentifierHash());
	sct_cluster_7.setHitCoord(hit.getCoord(0));
	sct_cluster_7.setHitWidth(hit.getNStrips());
      } else if( i == 11 ){
	f8 = true;
	sct_cluster_8.setModuleID(hit.getIdentifierHash());
	sct_cluster_8.setHitCoord(hit.getCoord(0));
	sct_cluster_8.setHitWidth(hit.getNStrips());
      }
    } else if ( type == 2){ // PIXEL //
      if ( i == 0 ){
	p1 = true;
	pixel_cluster_1.setModuleID(hit.getIdentifierHash());
	pixel_cluster_1.setRowCoord(hit.getCoord(0));
	pixel_cluster_1.setRowWidth(hit.getPhiWidth());
	pixel_cluster_1.setColCoord(hit.getCoord(1)); 
	pixel_cluster_1.setColWidth(hit.getEtaWidth());
      } else if (i == 1){
	p2 = true;
	pixel_cluster_2.setModuleID(hit.getIdentifierHash());
	pixel_cluster_2.setRowCoord(hit.getCoord(0));
	pixel_cluster_2.setRowWidth(hit.getPhiWidth());
	pixel_cluster_2.setColCoord(hit.getCoord(1)); 
	pixel_cluster_2.setColWidth(hit.getEtaWidth());
      } else if (i == 2){
	p3 = true;
	pixel_cluster_3.setModuleID(hit.getIdentifierHash());
	pixel_cluster_3.setRowCoord(hit.getCoord(0));
	pixel_cluster_3.setRowWidth(hit.getPhiWidth());
	pixel_cluster_3.setColCoord(hit.getCoord(1)); 
	pixel_cluster_3.setColWidth(hit.getEtaWidth());
      } else if (i == 3){
	p4 = true;
	pixel_cluster_4.setModuleID(hit.getIdentifierHash());
	pixel_cluster_4.setRowCoord(hit.getCoord(0));
	pixel_cluster_4.setRowWidth(hit.getPhiWidth());
	pixel_cluster_4.setColCoord(hit.getCoord(1)); 
	pixel_cluster_4.setColWidth(hit.getEtaWidth());
      }
    }
  } // end of hit loop
  
  pixel_clusters.push_back(pixel_cluster_1);
  pixel_clusters.push_back(pixel_cluster_2);
  pixel_clusters.push_back(pixel_cluster_3);
  pixel_clusters.push_back(pixel_cluster_4);
  sct_clusters.push_back(sct_cluster_1);
  sct_clusters.push_back(sct_cluster_2);
  sct_clusters.push_back(sct_cluster_3);
  sct_clusters.push_back(sct_cluster_4);
  sct_clusters.push_back(sct_cluster_5);
  sct_clusters.push_back(sct_cluster_6);
  sct_clusters.push_back(sct_cluster_7);
  sct_clusters.push_back(sct_cluster_8);

  /// Road ID convert to 32 bit ///
  unsigned int road_id = (unsigned int)track.getRoadID();
  word_1 = road_id; 

  /// Pack D0 and Z0 ///
  uint32_t d0   = 0;
  uint32_t z0   = 0;
  uint32_t phi  = 0;
  uint32_t cot  = 0;
  uint32_t chi  = 0;
  uint32_t curv = 0;

  if(!(fabs(track.getIP())>32.767))
    d0   = NINT( 10e2*track.getIP() + 32767);
  if(!(fabs(track.getZ0())>327.67))
    z0   = NINT( 100*track.getZ0() + 32767);
  if(!(fabs(track.getPhi()>TMath::Pi())))
    phi  = NINT( 10000*track.getPhi() + 32767);
  if(!(fabs(track.getCotTheta())>6.5))
    cot  = NINT( 5000*track.getCotTheta() + 32767);
  if(!(fabs(track.getInvPt())>66))
    curv = NINT( 5*10e6*track.getInvPt() + 32767);

  d0 = d0 << 16;
  d0 = d0 | z0;
  word_2 = d0;

  cot = cot << 16;
  cot = cot | phi;
  word_3 = cot;

  chi = chi << 16;
  chi = chi | curv;
  word_4 = chi;

  FTK_RawTrack *raw_track = new FTK_RawTrack(word_1, word_2, word_3, word_4, word_5, pixel_clusters, sct_clusters);
//  FTK_RawTrack *raw_track = new FTK_RawTrack(word_1, word_2, word_3, word_4, word_5);
  raw_track->setZ0(track.getZ0());
  raw_track->setPhi(track.getPhi());
  raw_track->setCotTh(track.getCotTheta());
  raw_track->setCurv(track.getInvPt());
  raw_track->setBarcode(track.getBarcode());
  if (msgLvl(MSG::VERBOSE)) {
    std::cout << "FTK_RawTrack Parameter Summary: " << std::endl;
    std::cout << " - Road ID = " << raw_track->getRoadID() << " " << track.getRoadID() << std::endl;
    std::cout << " - Road ID = " << raw_track->getTH1() << " " << track.getRoadID() << std::endl;
    std::cout << " - d0      = " << raw_track->getD0()     << " " << track.getIP() << std::endl;
    std::cout << " - z0      = " << raw_track->getZ0()     << " " << track.getZ0() << std::endl;
    std::cout << " - phi     = " << raw_track->getPhi()    << " " << track.getPhi() << std::endl;
    std::cout << " - cot     = " << raw_track->getCotTh()  << " " << track.getCotTheta() << std::endl;
    std::cout << " - ipt     = " << raw_track->getCurv()   << " " << track.getInvPt() << std::endl;
    std::cout << " - barcode = " << raw_track->getBarcode()<< " " << track.getBarcode() << std::endl;
    for( unsigned int i = 0; i < raw_track->getPixelClusters().size(); ++i){
      std::cout << " PIXEL Cluster " << i << std::endl;
      std::cout << "       Layer      = " << raw_track->getPixelClusters()[i].getLayer() << std::endl;
      std::cout << "       Col Coord  = " << raw_track->getPixelClusters()[i].getColCoord() << std::endl;
      std::cout << "       Col Width  = " << raw_track->getPixelClusters()[i].getColWidth() << std::endl;
      std::cout << "       Row Coord  = " << raw_track->getPixelClusters()[i].getRowCoord() << std::endl;
      std::cout << "       Row Width  = " << raw_track->getPixelClusters()[i].getRowWidth() << std::endl;
      std::cout << "       Hash ID    = " << raw_track->getPixelClusters()[i].getModuleID() << std::endl;
      const FTKHit& hit = track.getFTKHit(i);
      //    if (!raw_track->getPixelClusters()[i].getLayer() ==-1){
      if ( (p1 && i == 0) ||
	   (p2 && i == 1) ||
	   (p3 && i == 2) ||
	   (p4 && i == 3)){
	std::cout << "       t Col Coord    = " << hit.getCoord(1)   << std::endl;
	std::cout << "       t Col Width    = " << hit.getEtaWidth() << std::endl;
	std::cout << "       t Row Coord    = " << hit.getCoord(0)   << std::endl;
	std::cout << "       t Row Width    = " << hit.getPhiWidth() << std::endl;
	std::cout << "       t Hash ID      = " << hit.getIdentifierHash() << std::endl;	
      }
    }
    for( unsigned int i = 0; i < raw_track->getSCTClusters().size(); ++i){
      std::cout << " SCT Cluster " << i << std::endl;
      std::cout << "       Layer        = " << raw_track->getSCTClusters()[i].getLayer() << std::endl;
      std::cout << "       Hit Coord    = " << raw_track->getSCTClusters()[i].getHitCoord() << std::endl;
      std::cout << "       Hit Width    = " << raw_track->getSCTClusters()[i].getHitWidth() << std::endl;
      std::cout << "       Module ID    = " << raw_track->getSCTClusters()[i].getModuleID() << std::endl;
      if ( (f1 && i == 0) ||
	   (f2 && i == 1) ||
	   (f3 && i == 2) ||
	   (f4 && i == 3) ||
	   (f5 && i == 4) ||
	   (f6 && i == 5) ||
	   (f7 && i == 6) ||
	   (f8 && i == 7)){//!raw_track->getSCTClusters()[i].getLayer() ==-1){
	const FTKHit& hit = track.getFTKHit(i+4);
	std::cout << "       a Coord     = " << hit.getCoord(0)  << std::endl;
	std::cout << "       a NStrips   = " << hit.getNStrips() << std::endl;
	std::cout << "       a Hash ID   = " << hit.getIdentifierHash() << std::endl;
      } 
    }
    std::cout << std::endl;
  }
  return raw_track;
}

void FTK_RDO_CreatorAlgo::printBits(unsigned int num, unsigned int length){
  for(unsigned int bit=0;bit<(sizeof(unsigned int) * 8) && bit < length; bit++){
    std::cout << int(num & 0x01);
    num = num >> 1;
  }
  std::cout << std::endl;
}
