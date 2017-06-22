/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "FTKStandaloneMonitoring/CompareFTKEvents.h"
#include <iostream>
#include <string>
CompareFTKEvents::CompareFTKEvents()
{
    std::cout<<"Empty constructor"<<std::endl;
}

CompareFTKEvents::CompareFTKEvents(const std::string &BSfile, const std::string &NTUP_FTK_file):
   m_BSfile(BSfile),
   m_NTUP_FTK_file(NTUP_FTK_file)
{
    std::cout<<"Input files constructor"<<std::endl;
    std::cout<<m_BSfile<<std::endl;
    std::cout<<m_NTUP_FTK_file<<std::endl;
    m_myBSfile.open(m_BSfile.c_str(),std::ios::in | std::ios::binary);
    if (m_myBSfile.fail()){
        std::cout << "ERROR! cannot open file "<<m_BSfile.c_str();
	m_myBSfile.clear( );
    }
}

void CompareFTKEvents::SetupPartition(const std::string &partition_name)
{   
    m_partition_name=partition_name;
    m_setup_partition=true;
    // initialization of IPCParition
    try {
       m_ipcpartition      = daq::rc::OnlineServices::instance().getIPCPartition();
       std::cout<<"==> Getting partition from Online Service Instance: you are running in a partition"<<std::endl;
    }
    catch ( daq::rc::Exception & ex ){	    
       ers::warning(ex);  //or throw e; 
       if (m_partition_name!="") std::cout<< "Partition set to the input variable: "<<m_partition_name<<std::endl;
       else if (std::getenv("TDAQ_PARTITION")!=nullptr) {
           m_partition_name = std::getenv("TDAQ_PARTITION");
	   std::cout<< "Partition set to the environment variable TDAQ_PARTITION: "<<m_partition_name<<std::endl;
       }	   
       //set to some default, e.g. ATLAS and print a warning, this should not happen
       else {
           m_partition_name="ATLAS";
	   std::cout<< "Partition set to ATLAS, this should not happen!!! "<<std::endl;
       }   
       m_ipcpartition = IPCPartition( m_partition_name );
       std::cout<<"==> Setting up partition from the name you provided in the command line: "<<m_partition_name<<std::endl;
       if (m_partition_name.empty()) std::cout<<"!!!! Empty partition name => you should provide it with \"-p [partition_name]\" as argument while executing"<<std::endl;
    }
    // Creating the Histogramming provider
    std::string OHServer("Histogramming"); 
    std::string OHName("FTK_" + m_name);   
    std::cout<<"partition initialized"<<std::endl;
    //initialization of the OH provider
    try {
      m_ohProvider = new OHRootProvider ( m_ipcpartition , OHServer, OHName );
      ERS_LOG("OH: publishing in " << OHServer << "." << m_name );
    }
    catch ( daq::oh::Exception & ex)
    { // Raise a warning or throw an exception.
      std::cout<<"OH error exception"<<std::endl;
      ers::warning(ex);  
    }  
}

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
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta p_{T} (test-ref); p_{T}^{test} - p_{T}^{ref} [MeV];FTK Tracks",1000,-5000,5000)));
            else if (istr.find("HWvsSWdiff_eta")!=std::string::npos)
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta #eta (test-ref); #eta^{test} - #eta^{ref} ;FTK Tracks",2000,-0.1,0.1)));
            else if (istr.find("HWvsSWdiff_phi")!=std::string::npos)
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta #phi (test-ref); #phi^{test} - #phi^{ref} ;FTK Tracks",2000,-0.1,0.1)));
            else if (istr.find("HWvsSWdiff_d0")!=std::string::npos) 
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta d_{0} (test-ref); d_{0}^{test} - d_{0}^{ref} [mm];FTK Tracks",4000,-5.,5.)));
            else if (istr.find("HWvsSWdiff_z0")!=std::string::npos) 
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta z_{0} (test-ref); z_{0}^{test} - z_{0}^{ref} [mm];FTK Tracks",4000,-2.,2.)));
            else if (istr.find("HWvsSWdiff_chi2")!=std::string::npos) 
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta chi2 (test-ref); chi2^{test} - chi2^{ref} ;FTK Tracks",4000,-5.,5.)));
            else if (istr.find("res_pt")!=std::string::npos)
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta p_{T} (test-ref)/ p_{T}^{ref};( p_{T}^{test} - p_{T}^{ref} )/ p_{T}^{ref};FTK Tracks",2000,-1.,1.)));
            else if (istr.find("res_eta")!=std::string::npos)
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta #eta (test-ref)/ #eta^{ref};( #eta^{test} - #eta^{ref} )/ #eta^{ref};FTK Tracks",2000,-1.,1.)));
            else if (istr.find("res_phi")!=std::string::npos)
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta #phi (test-ref)/ #phi^{ref};( #phi^{test} - #phi^{ref} )/ #phi^{ref};FTK Tracks",2000,-1.,1.)));
            else if (istr.find("res_d0")!=std::string::npos) 
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta d_{0} (test-ref)/ d_{0}^{ref};( d_{0}^{test} - d_{0}^{ref} )/ d_{0}^{ref};FTK Tracks",4000,-2.,2.)));
            else if (istr.find("res_z0")!=std::string::npos) 
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta z_{0} (test-ref)/ z_{0}^{ref};( z_{0}^{test} - z_{0}^{ref} )/ z_{0}^{ref};FTK Tracks",4000,-2.,2.)));
            else if (istr.find("res_chi2")!=std::string::npos) 
		m_map_histo.insert(std::map<std::string, TH1D *>::value_type(istr, new TH1D(istr.c_str(),"#Delta chi2 (test-ref)/ chi2^{ref};( chi2^{test} - chi2^{ref} )/ chi2^{ref};FTK Tracks",4000,-2.,2.)));
	}
	
	// 2D histograms containing ETA_PHI in the histo name are not published on OH
	else {
 	    m_map_histo_2D.insert(std::map<std::string, TH2D *>::value_type(istr, new TH2D(istr.c_str(),title.c_str(),100,-2.5,2.5,100,-3.2,3.2)));	
	}
    }	
}

void CompareFTKEvents::Execute()
{
    //Getting BS file event number
    m_Nevents_BS=GetNEventsBS();
    std::cout<<"Input BS file has N events: "<<m_Nevents_BS<<std::endl;
    // reading NTUP_FTK file
    readNTUP_FTKfile();
    if (m_Nevents_BS!=m_Nevents_NTUPFTK) {
	std::cout<<"different number of events: "<<m_Nevents_BS<<" "<<m_Nevents_NTUPFTK<<std::endl;
	m_allmatched=false;
    }
    //looping over events
    EventLoop();
    // writing the histograms
    WriteHistos();
}

void CompareFTKEvents::readNTUP_FTKfile()
{
    std::cout<<"Reading NTUP_FTK file "<<m_NTUP_FTK_file<<std::endl;
    m_fntupftk = new TFile(m_NTUP_FTK_file.c_str());
    if (!m_fntupftk) std::cout<<"ERROR!! NTUP_FTK file could not be opened: "<<m_NTUP_FTK_file.c_str()<<std::endl;
    if (!m_fntupftk->GetListOfKeys()->Contains(m_str_tree_evtinfo.c_str())) std::cout<<"ERROR!! NTUP_FTK file does not contain tree: "<<m_str_tree_evtinfo.c_str()<<std::endl;
    m_tevtinfo = (TTree*)m_fntupftk->Get(m_str_tree_evtinfo.c_str());
    m_tevtinfo->SetBranchAddress("RunNumber",&m_RN);  
    m_tevtinfo->SetBranchAddress("EventNumber",&m_EN);  
    m_tevtinfo->SetBranchAddress("LB",&m_LB);  
    m_tevtinfo->SetBranchAddress("BCID",&m_BCID);  
    m_tevtinfo->SetBranchAddress("ExtendedLevel1ID",&m_EL1ID);  
    m_tevtinfo->SetBranchAddress("Level1TriggerType",&m_l1TT); 
    if (!m_fntupftk->GetListOfKeys()->Contains(m_str_tree_ftkdata.c_str())) std::cout<<"ERROR!! NTUP_FTK file does not contain tree: "<<m_str_tree_ftkdata.c_str()<<std::endl;
    m_theTree=(TTree *)m_fntupftk->Get(m_str_tree_ftkdata.c_str());
    m_ft=new FTKTrackStream();
    if (!m_theTree->GetListOfBranches()->Contains(m_str_tree_ftkstream.c_str()))std::cout<<"ERROR!! NTUP_FTK file does not contain branch: "<<m_str_tree_ftkstream.c_str()<<std::endl;
    auto branch  = m_theTree->GetBranch(m_str_tree_ftkstream.c_str());
    branch->SetAddress(&m_ft);
    m_Nevents_NTUPFTK=m_theTree->GetEntries();
}

StatusCode CompareFTKEvents::decode(uint32_t nTracks, OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData, FTK_RawTrackContainer* result) {

  result->reserve(result->size() + nTracks);
  for ( size_t i = 0; i < nTracks; ++i ) {
    FTK_RawTrack* track = unpackFTTrack( rodData );
    rodData += m_TrackBlobSize;
    result->push_back(track);
  }
  return StatusCode::SUCCESS;
}

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

void CompareFTKEvents::unpackPixCluster(OFFLINE_FRAGMENTS_NAMESPACE::PointerType data, FTK_RawPixelCluster& cluster) {
  cluster.setWordA(*data);
  cluster.setWordB(*(data+1));
}

void CompareFTKEvents::EventLoop()
{
    std::cout<<"Starting Loop"<<std::endl;
    m_tmpbufpos=0;
    if (m_nevtmax==0)m_nevtmax=std::min(m_Nevents_BS,m_Nevents_NTUPFTK);
    // looping over the requested number of events 
    for (int ievent=1;ievent<=m_nevtmax;ievent++){
       m_tmpbufpos =readBSevent( 1,m_tmpbufpos);  
       int lengthdata=m_data.size();
       // some transformation into a uint32_t vector and then into a PointerType
       uint32_t * data32=NULL;
       data32=new uint32_t[lengthdata]; 
       for(int ij=0; ij<lengthdata; ij++) {
           data32[ij]=m_data[ij];
       }
       OFFLINE_FRAGMENTS_NAMESPACE::PointerType pdata=data32;
       //event info are given to FullEventFragment
       eformat::read::FullEventFragment fe(pdata);
       std::vector< eformat::read::ROBFragment > ROBFragments_v;
       if (m_verbose)std::cout<<"Run Number "<<fe.run_no()<<" Lvl1 "<<fe.lvl1_id()<<" BCID "<<fe.bc_id()<<std::endl;
       fe.robs( ROBFragments_v );
       FTK_RawTrackContainer* trkcontainerBS= new FTK_RawTrackContainer();
       std::vector<const FTK_RawTrack *> ftkBSref;
       int nrob=0;
       int nTrks=0;
       for(auto& ROBFragment : ROBFragments_v){
           unsigned int niTrks=0;
           eformat::helper::SourceIdentifier ROBSource_id = eformat::helper::SourceIdentifier( ROBFragment.source_id() );
           if(ROBSource_id.subdetector_id() != eformat::TDAQ_FTK ){
              continue;
           }
           const uint32_t *rod_data = ROBFragment.rod_data();
           uint32_t rod_ndata = ROBFragment.rod_ndata();
	   if( ( rod_ndata - m_trackBlockOffsetStart - m_trackBlockOffsetEnd ) % m_TrackBlobSize != 0 ){
	      std::cout<<"Error: wrong size of rod"<<rod_ndata<<std::endl;
	      continue;
	   }
           niTrks = ( rod_ndata - m_trackBlockOffsetStart - m_trackBlockOffsetEnd ) / m_TrackBlobSize;
	   nTrks+=niTrks;
           rod_data += m_trackBlockOffsetStart;            // moving data pointer to first track block
	   if (m_verbose){
	      std::cout<<"test of working rod l1 "<<ROBFragment.rod_lvl1_id()<<" ndata "<<rod_ndata<<std::endl;
	   }
	   // decoding the rod info into FTK track container
           StatusCode sc;
	   sc=decode(niTrks,rod_data, trkcontainerBS);
	   if (!sc.isSuccess()) std::cout<<"ERROR!! decode function did not return success"<<std::endl;
	   if (trkcontainerBS->size()==0) {
               std::cout<<"no FTK tracks from BS file continue"<<std::endl;
	       continue;
	   }
	   if (m_verbose){
	      std::cout<<sc.isSuccess()<<std::endl;
              std::cout<< "collection of size " << trkcontainerBS->size() << std::endl;
	   }
           for ( unsigned int i = 0 ; i < niTrks; ++i ) {
	       ftkBSref.push_back(trkcontainerBS->at(i));
	   }
	   nrob+=1;	  
       }
       // filling histograms with number of HW tracks per event
       auto search = m_map_histo.find("nTrk_HW");
       if(search != m_map_histo.end()) search->second->Fill(nTrks);
       else  std::cout << "Not found "<<search->first<<std::endl;
       if (nrob>1){std::cout<<"!!!!!!!!!!SOMETHING WRONG number of robs >1:"<<nrob<<std::endl;}
       std::cout<<"Event "<<ievent<<" N tracks BS file "<<trkcontainerBS->size()<<std::endl;
       // end reading info from BS file
       //---------------------------------------------------------------------//
       // start reading info from NTUP_FTK file
       m_theTree->GetEvent(ievent-1); 
       FTK_RawTrackContainer* trkcontainerNTUP= new FTK_RawTrackContainer();
       std::vector<const FTK_RawTrack *> ftkNTUPtest;
       int NTracksNTUP;
       NTracksNTUP=m_ft->getNTracks();
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
	 trkcontainerNTUP->push_back(rawftktrk);
         ftkNTUPtest.push_back(trkcontainerNTUP->at(it));
       }
       std::cout<<"Event "<<ievent<<" N tracks NTUP_FTK "<<trkcontainerNTUP->size()<<std::endl;
       // end reading info from NTUP_FTK file
       //---------------------------------------------------------------------//
       // start comparing infos
       m_tevtinfo->GetEntry(ievent-1);
       if (NTracksNTUP!=nTrks){std::cout<<"different N tracks: "<<NTracksNTUP<<" "<<nTrks<<std::endl;m_allmatched=false;}
       if (fe.run_no()!=(unsigned int)m_RN){std::cout<<"different RunNUmber: "<<fe.run_no()<<" "<<m_RN<<std::endl; m_allmatched=false;}
       if (fe.bc_id()!=(unsigned int)m_BCID){std::cout<<"different BCID: "<<fe.bc_id()<<" "<<m_BCID<<std::endl;m_allmatched=false;}
       // end comparing infos
       //---------------------------------------------------------------------//
       // quick manual check of the parameters of SW and HW tracks before doing the real matching
       // if the two files BS anf NTUP_FTK contained exactly the same information, 
       // there should be no difference in the track parameters
       for ( int i = 0 ; i < std::min((int) nTrks,NTracksNTUP); ++i ) {
           if(trkcontainerNTUP->at(i)->getD0()!=0&&trkcontainerNTUP->at(i)->getZ0()!=0&&trkcontainerNTUP->at(i)->getInvPt()!=0&&
	      trkcontainerNTUP->at(i)->getPhi()!=0&&trkcontainerNTUP->at(i)->getCotTh()!=0&&trkcontainerNTUP->at(i)->getChi2()!=0
	      &&(
	       fabs(trkcontainerBS->at(i)->getD0()/trkcontainerNTUP->at(i)->getD0()-1.)>0.001||
               fabs(trkcontainerBS->at(i)->getZ0()/trkcontainerNTUP->at(i)->getZ0()-1.)>0.001||
               fabs(trkcontainerBS->at(i)->getInvPt()/trkcontainerNTUP->at(i)->getInvPt()-1.)>0.001||
               fabs(trkcontainerBS->at(i)->getPhi()/trkcontainerNTUP->at(i)->getPhi()-1.)>0.001||
               fabs(trkcontainerBS->at(i)->getCotTh()/trkcontainerNTUP->at(i)->getCotTh()-1.)>0.001||
               fabs(trkcontainerBS->at(i)->getChi2()/trkcontainerNTUP->at(i)->getChi2()-1.)>0.001)){
		   if (m_verbose){
		       std::cout<<"Difference in comparing trk "<<i<<std::endl;
		       std::cout<<"\td0 : \t"<<trkcontainerBS->at(i)->getD0()<<"\t"<<trkcontainerNTUP->at(i)->getD0()<<std::endl;
		       std::cout<<"\tz0 : \t"<<trkcontainerBS->at(i)->getZ0()<<"\t"<<trkcontainerNTUP->at(i)->getZ0()<<std::endl;
		       std::cout<<"\tipt : \t"<<trkcontainerBS->at(i)->getInvPt()<<"\t"<<trkcontainerNTUP->at(i)->getInvPt()<<std::endl;
		       std::cout<<"\tPhi : \t"<<trkcontainerBS->at(i)->getPhi()<<"\t"<<trkcontainerNTUP->at(i)->getPhi()<<std::endl;
		       std::cout<<"\tCotTh : \t"<<trkcontainerBS->at(i)->getCotTh()<<"\t"<<trkcontainerNTUP->at(i)->getCotTh()<<std::endl;
		       std::cout<<"\tChi2 : \t"<<trkcontainerBS->at(i)->getChi2()<<"\t"<<trkcontainerNTUP->at(i)->getChi2()<<std::endl;
		   }    
		   m_allmatched=false;
               }
	   else {if (i%std::min(nTrks,NTracksNTUP)==0 && m_allmatched){std::cout<<"Track Parameters btw BS_FTK and NTUP_FTK are the same: good!"<<std::endl;}}
       }
       //---------------------------------------------------------------------//
       // start matching
       m_compTrk= new CompareFTKTracks(ftkBSref,ftkNTUPtest, m_map_histo, m_map_histo_2D);
       m_compTrk->AssociateTracks();
       m_compTrk->FillHistos();
       //---------------------------------------------------------------------//
       delete data32;
     }
}

void CompareFTKEvents::WriteHistos(){
    m_fout= new TFile("./out.histo.root","RECREATE");
    m_fout->cd();
    for(auto & imap : m_map_histo){
        imap.second->Write();
	if (m_setup_partition){
	   try { m_ohProvider->publish( imap.second, imap.second->GetName(), true ); }
	   catch ( daq::oh::Exception & ex)
	   { // Raise a warning or throw an exception.
	     std::cout<<"ERROR: OH provider error"<<std::endl;
	     ers::warning(ex);  //or throw e; 
	   }
	}
    }
    for(auto & imap : m_map_histo_2D){
        imap.second->Write();
    }
    m_fout->Write();
    m_fout->Close();
    std::cout<<"Histo written into file "<<m_fout->GetName()<<std::endl;
}

std::streampos CompareFTKEvents::readBSevent(int ievent,std::streampos startbufpos)
{
    if (m_verbose) std::cout<<"Reading BS event"<<std::endl;
    m_data.clear();
    uint32_t word;
    int iil=0;
    int nprocessedevents=0;
    std::streampos lastpos;
    if (m_verbose) std::cout<<"pointer position start"<<m_myBSfile.tellg()<<std::endl;
    if (ievent ==1){
       if (m_verbose) std::cout<<"going to buffer position "<<startbufpos<<std::endl;
       m_myBSfile.seekg(startbufpos);
    }
    if (m_verbose) std::cout<<"starting reading words "<<startbufpos<<std::endl;
    while(m_myBSfile.is_open())
    {
       if(m_myBSfile.eof()){  break; }
       word=0;
       m_myBSfile.read ((char*)&word, sizeof(word));
       if (word==m_checkword) {
	  nprocessedevents+=1;
          if (m_verbose) std::cout<<"pointer position start event"<<m_myBSfile.tellg()<<std::endl;
       }
       if (nprocessedevents==ievent){ 
          m_data.push_back(word);
          iil++;
	  lastpos=m_myBSfile.tellg();
       }
       else if (nprocessedevents>ievent) {
          if (m_verbose) std::cout<<"pointer position end event "<<lastpos<<std::endl;
          break;
       }
    }
    if (m_verbose) std::cout<<"pointer position before closing"<<m_myBSfile.tellg()<<std::endl;
    return lastpos;    
}

int CompareFTKEvents::GetNEventsBS()
{
    std::cout<<"Getting N events BS file"<<std::endl;
    uint32_t word;
    int nevents=0;
    while(m_myBSfile.is_open())
    {
       if(m_myBSfile.eof()){ break; }
       word=0;
       m_myBSfile.read ((char*)&word, sizeof(word));
       if (word==m_checkword) {
	  nevents+=1;
       }
    }
    m_myBSfile.clear();
    m_myBSfile.seekg(0,std::ios::beg);
    return nevents;    
}


void CompareFTKEvents::PrintFiles()
{
    std::cout<<"Print Input files"<<std::endl;
    std::cout<<m_BSfile<<std::endl;
    std::cout<<m_NTUP_FTK_file<<std::endl;    
}

CompareFTKEvents::~CompareFTKEvents()
{
    m_myBSfile.close();
    if (m_allmatched) std::cout<<"====>Finished successfully: the two files contain the same infos"<<std::endl;
    else std::cout<<"====>There was something different, check the printouts"<<std::endl;
    
}
