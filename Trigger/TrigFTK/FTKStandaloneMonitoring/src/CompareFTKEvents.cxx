/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "FTKStandaloneMonitoring/CompareFTKEvents.h"
#include <iostream>
#include <string>
#include "oh/OHRootProvider.h"
#include "oh/OHRootReceiver.h"
#include "oh/OHIterator.h"
#include "oh/OHProviderIterator.h"
#include "oh/OHServerIterator.h"
#include "ipc/partition.h"
#include "ipc/core.h"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

// ________________________________________________________________ //
const std::string CompareFTKEvents::m_name = "HLmon_HWSWcomp";


// ________________________________________________________________ //
CompareFTKEvents::CompareFTKEvents()
{
    std::cout<<"Empty constructor. Please call: CompareFTKEvents(BSfile, NTUP_FTK_file)."<<std::endl;
}


// ________________________________________________________________ //
CompareFTKEvents::CompareFTKEvents(const std::string& BSfile, const std::string& NTUP_FTK_file)
{
    std::cout<<"CompareFTKEvents::CompareFTKEvents(). Reading files."<<std::endl;
    // --- reading files and getting event numbers
    read_BSfile(BSfile);
    // reading NTUP_FTK file
    readNTUP_FTKfile(NTUP_FTK_file);
}


// ________________________________________________________________ //
void CompareFTKEvents::SetHistos(std::vector<std::string> histo_list){
    m_histo_list=histo_list;
    for (auto & istr : m_histo_list){
	std::size_t pos = istr.find("_");      
        std::string str3 = istr.substr (pos+1);
        std::stringstream ss;
        
	// definition of the titles and axis labels of 1D histograms published on OH
	if (istr.find("HWSW")!=std::string::npos)        ss<<"HW=SIM;"<<str3<<m_param_units[str3]<<";FTK HW=SIM Tracks";
        else if (istr.find("HWonly")!=std::string::npos) ss<<"HW only;"<<str3<<m_param_units[str3]<<";FTK HW Tracks w/o SW match";
        else if (istr.find("SWonly")!=std::string::npos) ss<<"SW only;"<<str3<<m_param_units[str3]<<";FTK SW Tracks w/o HW match";
        else if (istr.find("HWvsSWhw")!=std::string::npos) ss<<"Matched Tracks but HW!=SW; HW "<<str3<<m_param_units[str3]<<";FTK HW Tracks w/ SW match "<<str3;
        else if (istr.find("HWvsSWsw")!=std::string::npos) ss<<"Matched Tracks but HW!=SW; SW "<<str3<<m_param_units[str3]<<";FTK SW Tracks w/ HW match "<<str3;
	//else if (istr.find("res")!=std::string::npos)  ss<<"Matched Tracks but HW!=SW, #Delta  (SW-HW)/ HW;"<<str3<<";FTK HW Tracks w/ SW match";
	else if (istr.find("nTrk")!=std::string::npos) ss<<istr<<";N tracks;Events";
	std::string title = ss.str();
	
	//1D histograms
	//if histo is NOT eta vs phi 2D histo these histos are published on OH
        if (istr.find("ETA_PHI")==std::string::npos){
            if (istr.find("nTrk")!=std::string::npos) m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),title.c_str(),100,0,100)));
 	    else if (istr.find("HWSW")!=std::string::npos|| istr.find("HWonly")!=std::string::npos||istr.find("SWonly")!=std::string::npos||(istr.find("HWvsSW")!=std::string::npos&&istr.find("diff")==std::string::npos))
	       m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),title.c_str(),m_histo_param[str3].at(0),m_histo_param[str3].at(1),m_histo_param[str3].at(2))));
            else if (istr.find("HWvsSWdiff_pt")!=std::string::npos)
	       m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta p_{T} (test-ref); p_{T}^{test} - p_{T}^{ref} [MeV];FTK Tracks",1000,-5000,5000);
            else if (istr.find("HWvsSWdiff_eta")!=std::string::npos)
		m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta #eta (test-ref); #eta^{test} - #eta^{ref} ;FTK Tracks",2000,-0.1,0.1);
            else if (istr.find("HWvsSWdiff_phi")!=std::string::npos)
		m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta #phi (test-ref); #phi^{test} - #phi^{ref} ;FTK Tracks",2000,-0.1,0.1);
            else if (istr.find("HWvsSWdiff_d0")!=std::string::npos) 
	       m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta d_{0} (test-ref); d_{0}^{test} - d_{0}^{ref} [mm];FTK Tracks",4000,-5.,5.);
            else if (istr.find("HWvsSWdiff_z0")!=std::string::npos) 
		m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta z_{0} (test-ref); z_{0}^{test} - z_{0}^{ref} [mm];FTK Tracks",4000,-2.,2.);
            else if (istr.find("HWvsSWdiff_chi2")!=std::string::npos) 
		m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta chi2 (test-ref); chi2^{test} - chi2^{ref} ;FTK Tracks",4000,-5.,5.);
            else if (istr.find("res_pt")!=std::string::npos)
		m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta p_{T} (test-ref)/ p_{T}^{ref};( p_{T}^{test} - p_{T}^{ref} )/ p_{T}^{ref};FTK Tracks",2000,-1.,1.);
            else if (istr.find("res_eta")!=std::string::npos)
		m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta #eta (test-ref)/ #eta^{ref};( #eta^{test} - #eta^{ref} )/ #eta^{ref};FTK Tracks",2000,-1.,1.);
            else if (istr.find("res_phi")!=std::string::npos)
		m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta #phi (test-ref)/ #phi^{ref};( #phi^{test} - #phi^{ref} )/ #phi^{ref};FTK Tracks",2000,-1.,1.);
            else if (istr.find("res_d0")!=std::string::npos) 
		m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta d_{0} (test-ref)/ d_{0}^{ref};( d_{0}^{test} - d_{0}^{ref} )/ d_{0}^{ref};FTK Tracks",4000,-2.,2.);
            else if (istr.find("res_z0")!=std::string::npos) 
		m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta z_{0} (test-ref)/ z_{0}^{ref};( z_{0}^{test} - z_{0}^{ref} )/ z_{0}^{ref};FTK Tracks",4000,-2.,2.);
            else if (istr.find("res_chi2")!=std::string::npos) 
	       m_map_histo[istr] = new TH1D(istr.c_str(),"#Delta chi2 (test-ref)/ chi2^{ref};( chi2^{test} - chi2^{ref} )/ chi2^{ref};FTK Tracks",4000,-2.,2.);
	}
	
	// 2D histograms containing ETA_PHI in the histo name are not published on OH
	else {
 	    m_map_histo_2D.insert(std::map<std::string, TH2D *>::value_type(istr, new TH2D(istr.c_str(),title.c_str(),100,-2.5,2.5,100,-3.2,3.2)));	
	}
    }	
}


// ________________________________________________________________ //
bool CompareFTKEvents::readNTUP_FTKfile(const std::string& NTUP_FTK_file)
{
   //! Reading FTK file
   //! Reading number of events, setting branches, ttrees for further reading
   //! returning true when successful, otherwise false

    // --- open file
   std::cout<<"\nCompareFTKEvents::readNTUP_FTKfile()"<<std::endl; 
   std::cout<<"Reading NTUP_FTK file: "<<NTUP_FTK_file<<std::endl;
    m_fntupftk = TFile::Open(NTUP_FTK_file.c_str(),"READ");
    if (!m_fntupftk) {
       std::cout<<"ERROR!! NTUP_FTK file could not be opened: "<<NTUP_FTK_file<<std::endl;
       exit(3);
       return false;
    }

    // --- reading tree evtinfo
    static const TString str_tree_evtinfo = "evtinfo";
    m_tevtinfo = (TTree*)m_fntupftk->Get(str_tree_evtinfo);
    if ( !m_tevtinfo ) {
       std::cout<<"ERROR!! NTUP_FTK file does not contain tree: "<<str_tree_evtinfo<<std::endl;
       return false;
    }
    m_tevtinfo->SetBranchAddress("RunNumber",&m_RN);  
    m_tevtinfo->SetBranchAddress("EventNumber",&m_EN);  
    m_tevtinfo->SetBranchAddress("LB",&m_LB);  
    m_tevtinfo->SetBranchAddress("BCID",&m_BCID);  
    m_tevtinfo->SetBranchAddress("ExtendedLevel1ID",&m_EL1ID);  
    m_tevtinfo->SetBranchAddress("Level1TriggerType",&m_l1TT); 

    // --- reading tree ftkdata
    static const TString str_tree_ftkdata = "ftkdata";
    m_theTree=(TTree*)m_fntupftk->Get(str_tree_ftkdata);
    if ( !m_theTree ) {
       std::cout<<"ERROR!! NTUP_FTK file does not contain tree: "<<str_tree_ftkdata<<std::endl;
       return false;
    }
    static const TString str_tree_ftkstream = "FTKMergedTracksStream";
    TBranch*  branch  = m_theTree->GetBranch(str_tree_ftkstream);
    if ( !branch ) {
       std::cout<<"ERROR!! NTUP_FTK file does not contain branch: "<<str_tree_ftkstream<<std::endl;                                                        return false;
    }
    m_ft = new FTKTrackStream();
    branch->SetAddress(&m_ft);
    
    // --- setting number of events
    //m_Nevents_NTUPFTK = m_theTree->GetEntries();
    std::cout<<"Info. Input FTK file has "<<m_theTree->GetEntries()<<" events."<<std::endl;
    return true;
}


// ________________________________________________________________ //
void CompareFTKEvents::decode(uint32_t nTracks, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result) {
  
  result->reserve(result->size() + nTracks);
  for ( size_t i = 0; i < nTracks; ++i ) {
    FTK_RawTrack* track = unpackFTTrack( rodData );
    rodData += m_TrackBlobSize;
    result->push_back(track);
  }
}


// ________________________________________________________________ //
FTK_RawTrack* CompareFTKEvents::unpackFTTrack( OFFLINE_FRAGMENTS_NAMESPACE::PointerType data) {  
  FTK_RawTrack* track = new FTK_RawTrack(data[0], data[1], data[2], data[3], data[4], data[5]); // first six words are track params
  data += m_TrackParamsBlobSize;

  // get pixel hits  
  track->getPixelClusters().resize(m_NPixLayers);
  for ( size_t i = 0; i < size_t(m_NPixLayers); ++i) {
    size_t offset = m_PixHitParamsBlobSize*i;
    unpackPixCluster(data+offset, track->getPixelCluster(i) );    
  }
  data += m_PixHitParamsBlobSize*m_NPixLayers;

  // gets SCT hits
  track->getSCTClusters().resize(m_NSCTLayers);
  for ( size_t i = 0; i < size_t(m_NSCTLayers); ++i) {
    size_t offset = m_SCTHitParamsBlobSize*i;
    unpackSCTCluster(data+offset, track->getSCTCluster(i) );    
  }
  // no more shifts needed
  return track;
}  


// ________________________________________________________________ //
void CompareFTKEvents::unpackPixCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawPixelCluster& cluster) {
  cluster.setWordA(*data);
  cluster.setWordB(*(data+1));
}


// ________________________________________________________________ //
void CompareFTKEvents::ExecuteEventLoop(int nevtmax)
{
   //! Run over all events of BS and FTKSim file
   //! nevtmax > 0: limit number of events
   
   std::cout<<"Running event loop"<<std::endl;
   //! looping over events
   int Nevents_NTUPFTK = m_theTree->GetEntries();
   if (m_Nevents_BS!=Nevents_NTUPFTK) {
      std::cout<<"Error! Different number of events: "<<m_Nevents_BS<<" (BS) and "<<Nevents_NTUPFTK<<" (FTKSim)"<<std::endl;
      m_allmatched=false;
      return;
    }
    nevtmax = ( nevtmax>0 ) ? std::min(nevtmax,Nevents_NTUPFTK) : Nevents_NTUPFTK;

    // looping over the requested number of events 
    std::streampos tmpbufpos=0;
    for (int ievent=1 ; ievent<=nevtmax ; ievent++ ){
       // --- BS file
       std::vector<uint32_t> data;
       tmpbufpos = readBSevent( 1,tmpbufpos, data);  
       //event info are given to FullEventFragment
       eformat::read::FullEventFragment fe(&data[0]);// OFFLINE_FRAGMENTS_NAMESPACE::PointerType p=&m_data[0];
       if (m_verbose) std::cout<<"Run Number "<<fe.run_no()<<" Lvl1 "<<fe.lvl1_id()<<" BCID "<<fe.bc_id()<<std::endl;
       std::vector< eformat::read::ROBFragment > ROBFragments_v;
       fe.robs( ROBFragments_v );

       FTK_RawTrackContainer trkcontainerBS;
       std::vector<const FTK_RawTrack *> ftkBSref;
       int nrob=0;
       int nTrks=0;
       // --- BS, run over all ROBFragments
       for(auto& ROBFragment : ROBFragments_v){
           eformat::helper::SourceIdentifier ROBSource_id = eformat::helper::SourceIdentifier( ROBFragment.source_id() );
           if(ROBSource_id.subdetector_id() != eformat::TDAQ_FTK )  continue;
           const uint32_t* rod_data = ROBFragment.rod_data();
           uint32_t rod_ndata = ROBFragment.rod_ndata();
	   if( ( rod_ndata - m_trackBlockOffsetStart - m_trackBlockOffsetEnd ) % m_TrackBlobSize != 0 ){
	      std::cout<<"Error: wrong size of rod"<<rod_ndata<<std::endl;
	      continue;
	   }
           unsigned int niTrks = ( rod_ndata - m_trackBlockOffsetStart - m_trackBlockOffsetEnd ) / m_TrackBlobSize;
	   nTrks+=niTrks;
           rod_data += m_trackBlockOffsetStart;            // moving data pointer to first track block
	   if (m_verbose) std::cout<<"test of working rod l1 "<<ROBFragment.rod_lvl1_id()<<" ndata "<<rod_ndata<<std::endl;

	   // decoding the rod info into FTK track container
           decode(niTrks,rod_data, &trkcontainerBS);
	   if ( trkcontainerBS.size() == 0 ) {
               std::cout<<"no FTK tracks from BS file continue"<<std::endl;
	       continue;
	   }
	   if (m_verbose)  std::cout<< "collection of size " << trkcontainerBS.size() << std::endl;
           for ( unsigned int i = 0 ; i < niTrks; ++i ) ftkBSref.push_back(trkcontainerBS.at(i));
	   nrob+=1;	  
       }
       // filling histograms with number of HW tracks per event
       auto search = m_map_histo.find("nTrk_HW");
       if(search != m_map_histo.end()) search->second->Fill(nTrks);
       else  std::cout << "Not found "<<search->first<<std::endl;
       if (nrob>1){std::cout<<"!!!!!!!!!!SOMETHING WRONG number of robs >1:"<<nrob<<std::endl;}
       std::cout<<"Event "<<ievent<<" N tracks BS file "<<trkcontainerBS.size()<<std::endl;
       // end reading info from BS file
       //---------------------------------------------------------------------//


       //---------------------------------------------------------------------//
       // --- start reading info from NTUP_FTK file
       m_theTree->GetEvent(ievent-1); 
       FTK_RawTrackContainer trkcontainerNTUP;
       std::vector<const FTK_RawTrack *> ftkNTUPtest;
       int NTracksNTUP = m_ft->getNTracks();
       // filling histograms with number of SW tracks per event
       auto search_SW = m_map_histo.find("nTrk_SW");
       if(search_SW != m_map_histo.end()) search_SW->second->Fill(NTracksNTUP);
       else  std::cout << "Not found "<<search_SW->first<<std::endl;
       // to be able to match SW and HW tracks the same object class needs to be used:
       // HW tracks are FTK_RawTrack; SW tracks are FTKTrack.
       // needed a by-hand transformation of a FTKTrack object into a FTK_RawTrack object 
       for(int it=0; it<NTracksNTUP;it++){
         FTKTrack* ftktrk=m_ft->getTrack(it);
         FTK_RawTrack* rawftktrk=new FTK_RawTrack();
	 rawftktrk->setD0(ftktrk->getIP());
	 rawftktrk->setZ0(ftktrk->getZ0());
	 rawftktrk->setPhi(ftktrk->getPhi());
	 rawftktrk->setCotTh(ftktrk->getCotTheta());
	 rawftktrk->setInvPt(2*ftktrk->getHalfInvPt());
	 rawftktrk->setChi2(ftktrk->getChi2());
	 rawftktrk->setBarcode(ftktrk->getBarcode());
	 trkcontainerNTUP.push_back(rawftktrk);
         ftkNTUPtest.push_back(trkcontainerNTUP.at(it));
       }
       std::cout<<"Event "<<ievent<<" N tracks NTUP_FTK "<<trkcontainerNTUP.size()<<std::endl;
       // end reading info from NTUP_FTK file
       //---------------------------------------------------------------------//
       // start comparing infos
       m_tevtinfo->GetEntry(ievent-1);
       if ( NTracksNTUP != nTrks ){
	  std::cout<<"Error! different N tracks. FTKSim:"<<NTracksNTUP<<"\t BS: "<<nTrks<<std::endl;
	  m_allmatched=false;
       }
       if ( fe.run_no()!=(unsigned int)m_RN ) {
	  std::cout<<"Error! different RunNumber: "<<fe.run_no()<<" "<<m_RN<<std::endl; 
	  m_allmatched=false;
       }
       if ( fe.bc_id()!=(unsigned int)m_BCID ) {
	  std::cout<<"Error! different BCID: "<<fe.bc_id()<<" "<<m_BCID<<std::endl;
	  m_allmatched=false;
       }
       // end comparing infos
       //---------------------------------------------------------------------//
       // quick manual check of the parameters of SW and HW tracks before doing the real matching
       // if the two files BS anf NTUP_FTK contained exactly the same information, 
       // there should be no difference in the track parameters
       for ( int i = 0 ; i < std::min((int) nTrks,NTracksNTUP); ++i ) {
           if(trkcontainerNTUP.at(i)->getD0()!=0&&trkcontainerNTUP.at(i)->getZ0()!=0&&trkcontainerNTUP.at(i)->getInvPt()!=0&&
	      trkcontainerNTUP.at(i)->getPhi()!=0&&trkcontainerNTUP.at(i)->getCotTh()!=0&&trkcontainerNTUP.at(i)->getChi2()!=0 && 
	      ( fabs(trkcontainerBS.at(i)->getD0()/trkcontainerNTUP.at(i)->getD0()-1.)>0.001||
		fabs(trkcontainerBS.at(i)->getZ0()/trkcontainerNTUP.at(i)->getZ0()-1.)>0.001||
		fabs(trkcontainerBS.at(i)->getInvPt()/trkcontainerNTUP.at(i)->getInvPt()-1.)>0.001||
		fabs(trkcontainerBS.at(i)->getPhi()/trkcontainerNTUP.at(i)->getPhi()-1.)>0.001||
		fabs(trkcontainerBS.at(i)->getCotTh()/trkcontainerNTUP.at(i)->getCotTh()-1.)>0.001||
		fabs(trkcontainerBS.at(i)->getChi2()/trkcontainerNTUP.at(i)->getChi2()-1.)>0.001) ) {
	      m_allmatched=false;
	      if (m_verbose) {
		       std::cout<<"Difference in comparing trk "<<i<<std::endl;
		       std::cout<<"\td0 : \t"<<trkcontainerBS.at(i)->getD0()<<"\t"<<trkcontainerNTUP.at(i)->getD0()<<std::endl;
		       std::cout<<"\tz0 : \t"<<trkcontainerBS.at(i)->getZ0()<<"\t"<<trkcontainerNTUP.at(i)->getZ0()<<std::endl;
		       std::cout<<"\tipt : \t"<<trkcontainerBS.at(i)->getInvPt()<<"\t"<<trkcontainerNTUP.at(i)->getInvPt()<<std::endl;
		       std::cout<<"\tPhi : \t"<<trkcontainerBS.at(i)->getPhi()<<"\t"<<trkcontainerNTUP.at(i)->getPhi()<<std::endl;
		       std::cout<<"\tCotTh : \t"<<trkcontainerBS.at(i)->getCotTh()<<"\t"<<trkcontainerNTUP.at(i)->getCotTh()<<std::endl;
		       std::cout<<"\tChi2 : \t"<<trkcontainerBS.at(i)->getChi2()<<"\t"<<trkcontainerNTUP.at(i)->getChi2()<<std::endl;
	      }    
	   }
	   else {
	      if ( i%std::min(nTrks,NTracksNTUP)==0 && m_allmatched ) {
		 std::cout<<"Track Parameters btw BS_FTK and NTUP_FTK are the same: good!"<<std::endl;
	      }
	   }
       }
       //---------------------------------------------------------------------//
       // start matching
       m_compTrk= new CompareFTKTracks(ftkBSref,ftkNTUPtest, m_map_histo, m_map_histo_2D);
       m_compTrk->AssociateTracks();
       m_compTrk->FillHistos();
       //---------------------------------------------------------------------//
     }
}


// ________________________________________________________________ //
std::unique_ptr<const IPCPartition*> CompareFTKEvents::GetIPCPartition(const std::string& partition_name) 
{   
   //!  partition_name must be: 'online', "", or <partition_name>
   //! Returns always a valid IPCPartition ptr.
   
   const IPCPartition* ipcpartition = nullptr; // return value

   // initialization of IPCParition
   if ( partition_name=="" || partition_name=="online" ) {
      try {
	 ipcpartition      = &daq::rc::OnlineServices::instance().getIPCPartition();
	 std::cout<<"==> Using partition from Online Service Instance with name "<<ipcpartition->name()
		  <<". You are running in a partition"<<std::endl;
      }
      catch ( daq::rc::Exception & ex ){	    
	 ers::warning(ex);  //or throw e; 
	 std::cout<<"CompareFTKEvents. Unable to retrieve IPCPartition from daq::rc::OnlineServiced."<<std::endl; //...
      }
      //if ( partition_name == "online" || ipcpartition )  return ipcpartition;
      if ( ipcpartition ) {
	 std::unique_ptr<const IPCPartition*> ret = std::make_unique<const IPCPartition*>(ipcpartition);
	 ret.release();
	 return ret;
      }
   }

   if ( !ipcpartition ) {
      if ( partition_name != "" ) {
	 std::cout<<"Instantiatinng new IPCPartition with name specified through input argument: "<<partition_name<<std::endl;
	 ipcpartition = new IPCPartition( partition_name );
      }
      else if ( std::getenv("TDAQ_PARTITION") && std::string(std::getenv("TDAQ_PARTITION"))!="" ) {
	 ipcpartition = new IPCPartition( std::getenv("TDAQ_PARTITION") );
	 std::cout<<"Using partition name set by environment variable TDAQ_PARTITION: "<<std::getenv("TDAQ_PARTITION")<<std::endl;
      }
      else {
	 ipcpartition = new IPCPartition( "ATLAS" );
	 std::cout<< "Using default partition name: "<<ipcpartition->name()<<std::endl;
	 std::cout<< "Please provide it with \"-p [partition_name]\" or with env variable TDAQ_PARTITION"<<std::endl;
      }
   }
   //   return std::unique_ptr<const IPCPartition*>(ipcpartition);
   return std::make_unique<const IPCPartition*>(ipcpartition);
}


// ________________________________________________________________ //
void CompareFTKEvents::PublishHistosToPartition(const std::string& partition_name) {
   //! Publish histograms to (online) partition [OH].
   //! Default, when no string is provided:
   //!     Try to access 'online' partition. Otherwise use env variable 
   //!     TDAQ_PARTITION, or use default partition-name "ATLAS".
   //! 
   
   
   // --- Publish histograms to OH
   auto ipcpartition = GetIPCPartition(partition_name);
   if ( !ipcpartition.get() ) {
      std::cout<<"PublishHistosToPartition. No partition initiated. Cannot publish to online service."<<std::endl;
      return;
   }

   //initialization of the OH provider
   const std::string OHServer("Histogramming"); 
   const std::string OHName("FTK_" + m_name);   
   const std::string OHCumulName("FTK_" + m_name + "_Cumulative");   

   try {
      OHRootProvider ohProvider ( **ipcpartition , OHServer, OHName ); // try...catch..
      OHRootProvider ohProvCumu ( **ipcpartition , OHServer, OHCumulName ); // try...catch..
      std::cout<<"OK! Publishing to OH. Server: "<<OHServer<<"\t providers: "<<OHName<<", "<<OHCumulName<<std::endl;
      ERS_LOG("OH: publishing in " << OHServer << "." << m_name );
       
      for(auto & imap : m_map_histo)  {
	 TH1D* histo = imap.second;
	 TH1D sumhist(*histo);

	 try {
	    OHRootHistogram ohh = OHRootReceiver::getRootHistogram( **ipcpartition , OHServer, OHCumulName, histo->GetName());
	    TH1D* oh1 = (TH1D*)ohh.getObject().get();
	    std::cout<<"Histogram found with "<<oh1->GetEntries()<<" entries."<<std::endl;
	    sumhist.Add(oh1);
	 }
	 catch ( daq::oh::Exception& ex) { 
	    std::cout<<"No initial cumulative histogram found for histo: "<<histo->GetName()<<std::endl;
	    //ers::warning(ex);
	 }
      
	 // --- list valid histograms
	 // try {
	 // 	 std::cout<<" --- test : "<<std::endl;
	 // 	 OHHistogramIterator hit2( *ipcpartition, OHServer, OHCumulName, ".*" );//OHIterator
	 // 	 std::cout << " contains " << hit2.entries() << " " << "H" << "(s) *ipcpartition (4)." << std::endl;
	 // 	 OHHistogramIterator hit3( *ipcpartition, OHServer, OHCumulName, histo->GetName()  );//OHIterator
	 // 	 std::cout << " contains " << hit3.entries() << " " << "H" << "(s) *ipcpartition (4)." << std::endl;
	 // }
	 // catch( ers::Issue & ex ) {
	 // 	 std::cout<<"test 'list valid histograms' failed"<<std::endl;
	 // 	 ers::error( ex );
	 // }

	 // publish histograms ...
	 try {
	    ohProvCumu.OHRootProvider::publish( sumhist, sumhist.GetName(), -1 ); 
	    ohProvider.OHRootProvider::publish( *((TH1D*)histo),  histo->GetName(), -1 );
	    std::cout<<"Histograms published successfully ("<<histo->GetName()<<")."<<std::endl;
	 }
	 catch( ers::Issue & ex ) {
	    std::cout<<"Something went wrong while publishing histograms."<<std::endl;
	    ers::error( ex );
	 }

      }
   }
   catch ( daq::oh::Exception & ex) { // OHRootProvider constructor
      std::cout<<"Warning. OH error exception. Cannot communicate with OH when using partition "<< (*ipcpartition)->name()<<std::endl;
      std::cout<<"Printing warning message:"<<std::endl;
      ers::warning(ex);  
      std::cout<<"PublishHistosToPartition. Cannot publish to online service."<<std::endl;
      return;
   }  
}


// ________________________________________________________________ //
void CompareFTKEvents::WriteHistosToFile(const std::string& filename){

    // --- write histograms to file.
    std::cout<<"CompareFTKEvents. Writing histos to file:     "<<filename<<std::endl;
    TFile* fout= TFile::Open(filename.c_str(),"RECREATE");
    fout->cd();
    for(auto & imap : m_map_histo)    imap.second->Write();
    for(auto & imap : m_map_histo_2D) imap.second->Write();
    std::cout<<"CompareFTKEvents. Histograms written to file: "<<fout->GetName()<<std::endl;
    fout->Close();
    delete fout;
}


// ________________________________________________________________ //
std::streampos CompareFTKEvents::readBSevent(int ievent,std::streampos startbufpos, std::vector<uint32_t>& data)
{
   
    if (m_verbose) std::cout<<"Reading BS event"<<std::endl;
    if (m_verbose) std::cout<<"pointer position start"<<m_myBSfile.tellg()<<std::endl;

    data.clear();
    int iil=0;
    int nprocessedevents=0;
    std::streampos lastpos;
    if (ievent == 1 ){
       if (m_verbose) std::cout<<"going to buffer position "<<startbufpos<<std::endl;
       m_myBSfile.seekg(startbufpos);
    }
    if (m_verbose) std::cout<<"starting reading words "<<startbufpos<<std::endl;
    while(m_myBSfile.is_open())
    {
       if(m_myBSfile.eof()){  break; }
       uint32_t word=0;
       m_myBSfile.read ((char*)&word, sizeof(word));
       if (word==m_checkword) {
	  nprocessedevents+=1;
          if (m_verbose) std::cout<<"pointer position start event"<<m_myBSfile.tellg()<<std::endl;
       }
       if (nprocessedevents==ievent){ 
          data.push_back(word);
          iil++;
	  lastpos = m_myBSfile.tellg();
       }
       else if ( nprocessedevents > ievent ) {
          if (m_verbose) std::cout<<"pointer position end event "<<lastpos<<std::endl;
          break;
       }
    }
    if (m_verbose) std::cout<<"pointer position before closing"<<m_myBSfile.tellg()<<std::endl;
    return lastpos;    
}


// ________________________________________________________________ //
bool CompareFTKEvents::read_BSfile(const std::string& BSfile)
{
   //! Opening BSfile for later reading.
   //! Counting number of events (m_Nevents_BS) and checking consistency of all lines
   //! return true when successful. Otherwise false.
   
   std::cout<<"\nCompareFTKEvents::read_BSfile()"<<std::endl;
   std::cout<<"Getting number of events from BS file: "<<BSfile<<std::endl;
    m_myBSfile.open(BSfile.c_str(),std::ios::in | std::ios::binary);
    if (m_myBSfile.fail()){
       std::cout << "ERROR! cannot open BS file: "<<BSfile.c_str()<<std::endl;
       exit(3);
       return false;
    }

    // --- count events
    m_Nevents_BS = 0;
    while ( m_myBSfile.is_open() ) {
       if ( m_myBSfile.eof() ) break; 
       uint32_t word=0;
       m_myBSfile.read ((char*)&word, sizeof(word));
       if ( word == m_checkword )  m_Nevents_BS++;
    }
    m_myBSfile.clear();
    m_myBSfile.seekg(0,std::ios::beg);

    std::cout<<"Info. Input BS file has "<<m_Nevents_BS<<" events."<<std::endl;
    return true;    
}


// ________________________________________________________________ //
CompareFTKEvents::~CompareFTKEvents()
{
    m_myBSfile.close();
    if (m_allmatched) 
       std::cout<<"====> Finished successfully: the two files contain the same infos."<<std::endl;
    else 
       std::cout<<"====> Something went wrong. Please check the printouts."<<std::endl;
    
}
