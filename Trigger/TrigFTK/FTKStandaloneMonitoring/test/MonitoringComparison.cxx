#include <iostream>
#include <fstream>

#include <getopt.h>

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TH1F.h"
#include "TBenchmark.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TrigFTKSim/FTKTrackStream.h"
#include "FTKStandaloneMonitoring/FTKTrkAssoc.h"
#include "FTKStandaloneMonitoring/CompareFTKEvents.h"

using namespace std;
//using namespace FTKByteStreamDecoderEncoder;

#ifndef TRUE
  #define TRUE                  0x01
#endif

#ifndef FALSE
  #define FALSE                 0x00
#endif

#define BUFSIZE  (4*1024)

// Globals
int verbose = FALSE;
int readfile = FALSE;
char filename[200] = {0};
std::string inputfilename;
//="../../../compareInputs/OUT.BS_FTK.root";
uint32_t checkword=0xaa1234aa;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// read example Spy Buffer dump from file
/*streampos read_sb_file(vector<uint32_t>  &data, std::string filename,int ievent,streampos startbufpos)
{
  //std::cout.setf ( std::ios::hex, std::ios::basefield );  // set hex as the basefield
  //std::cout.setf ( std::ios::showbase );  
  data.clear();
  //int length = 0;
  std::string line;
  std::ifstream myfile(filename.c_str(),std::ios::in | std::ios::binary);
  uint32_t word;
  int iil=0;
  int nprocessedevents=0;
  streampos lastpos;
  if (verbose) cout<<"pointer position start"<<myfile.tellg()<<std::endl;
  if (ievent ==1){
      if (verbose) std::cout<<"going to buffer position "<<startbufpos<<std::endl;
      myfile.seekg(startbufpos);
  }
  while(myfile.is_open())
  {
     if(myfile.eof()){ printf("END\n"); break; }
     word=0;
     myfile.read ((char*)&word, sizeof(word));
     //std::cout<<word<<std::endl;
     //std::cout<<(int)iil<<" "<<std::hex<<word<<std::endl;
     //printf("%d\n",word);
     if (word==checkword) {
	nprocessedevents+=1;
        //std::cout<<"Found word number "<<nprocessedevents<<std::endl;
        if (verbose) cout<<"pointer position start event"<<myfile.tellg()<<std::endl;
     }
     if (nprocessedevents==ievent){ 
        data.push_back(word);
        iil++;
	lastpos=myfile.tellg();
     }
     else if (nprocessedevents>ievent) {
        if (verbose) std::cout<<"lpointer position end event "<<lastpos<<std::endl;
        //std::cout<<"Found word number "<<nprocessedevents<<" getting out of the loop after iteration "<<iil<<std::endl;     
        break;
     }
  }
  if (verbose) cout<<"pointer position before closing"<<myfile.tellg()<<std::endl;
  myfile.close();
  //length=iil;
  return lastpos;
}

int getNevents(std::string filename)
{
  //std::cout.setf ( std::ios::hex, std::ios::basefield );  // set hex as the basefield
  //std::cout.setf ( std::ios::showbase );  
  std::string line;
  std::ifstream myfile(filename.c_str(),std::ios::in | std::ios::binary);
  uint32_t word;
  int nevents=0;
  while(myfile.is_open())
  {
     if(myfile.eof()){ printf("END\n"); break; }
     word=0;
     myfile.read ((char*)&word, sizeof(word));
     if (word==checkword) {
        //cout<<"word"<<word;
	nevents+=1;
     }
  }
  return nevents;
}*/


/**************/
void usage(void)
/**************/
{
  std::cout << "Valid options are ..." << std::endl;
  std::cout << "-f x: Read BS_FTK data from file. The parameter is the path and the name of the file" << std::endl;
  std::cout << "-n x: Read NTUP_FTK data from file. The parameter is the path and the name of the file" << std::endl;
  std::cout << "-m x: maximum number of events" << std::endl;
  std::cout << "-v  : Verbose output                               -> Default: FALSE" << std::endl;
  std::cout << std::endl;
}



/*****************************/
int main(int argc, char **argv)
/*****************************/
{

  std::string inputfilenameBS= "/afs/cern.ch/user/g/giulini/workdir/public/FTKcomparison/compareInputs/OUT.BS_FTK.root";
  std::string inputfilenameNTUPFTK= "/afs/cern.ch/user/g/giulini/workdir/public/FTKcomparison/compareInputs/OUT.NTUP_FTK.root";
  //evtinfo TTree NTUP_FTK
  int c;
  int evtmax=0;
  static struct option long_options[] = {"DVS", no_argument, NULL, '1'}; 
  while ((c = getopt_long(argc, argv, "f:n:v:m:h", long_options, NULL)) != -1)
    switch (c) 
    {
    case 'h':
      std::cout << "Usage: " << argv[0] << " [options]: "<< std::endl;
      usage();
      exit(-1);
      break;

    case 'f':   
      readfile = TRUE;
      sscanf(optarg,"%s", filename);
      inputfilenameBS.clear();
      inputfilenameBS.append(filename);
      std::cout << "read BS_FTK data from file: " << std::endl<< inputfilenameBS<< std::endl << std::endl;
      break;

    case 'n':   
      readfile = TRUE;
      sscanf(optarg,"%s", filename);
      inputfilenameNTUPFTK.clear();
      inputfilenameNTUPFTK.append(filename);
      std::cout << "read NTUP_FTK data from file: "<< std::endl << inputfilenameNTUPFTK<< std::endl << std::endl;
      break;
      
    case 'm':   
      sscanf(optarg,"%d", &evtmax);
      std::cout << "maximum number of events: "<< evtmax << std::endl << std::endl;
      break;
      
    case 'v': verbose = TRUE;               break;

    default:
      std::cout << "Invalid option " << c << std::endl;
      std::cout << "Usage: " << argv[0] << " [options]: " << std::endl;
      usage();
      exit (-1);
    }
  ;
  
  CompareFTKEvents *comparison= new CompareFTKEvents(inputfilenameBS,inputfilenameNTUPFTK);
  comparison->PrintFiles();      
  if (verbose) comparison->SetVerbose();  
  if (evtmax!=0)    comparison->SetNEvents(evtmax);
  std::string str= "prova";
  std::vector<std::string> variable_list={"pt","eta","phi","d0","z0","chi2","ETA_PHI"};
  
  std::vector<std::string> histo_list;
  for (auto & istr: variable_list){
      std::stringstream ss;
      ss << "HWSW_" << istr;
      std::stringstream ss2;
      ss2 << "HWonly_" << istr;
      histo_list.push_back(ss.str());
      histo_list.push_back(ss2.str());
  }
  histo_list.push_back("m_res_pt");
  histo_list.push_back("m_res_d0");
  histo_list.push_back("nTrk_same_hw_sw");
  histo_list.push_back("nTrk_different_hw_sw");
  histo_list.push_back("nTrk_only_hw");
  histo_list.push_back("nTrk_only_sw");
  //histo_list.push_back("m_res_phi");
  //histo_list.push_back("m_res_eta");
  //histo_list.push_back("m_res_z0");
  //histo_list.push_back("m_res_chi2");
  comparison->SetHistos(histo_list);
  //comparison->SetEvtInfoString(str);
  comparison->Execute();      
  /*TFile *fntupftk = new TFile(inputfilenameNTUPFTK.c_str());
  TTree *tevtinfo = (TTree*)fntupftk->Get("evtinfo");
  int RN, EN, LB, BCID, EL1ID, l1TT;
  //vector<unsigned int>* l1TI; 
  tevtinfo->SetBranchAddress("RunNumber",&RN);  
  tevtinfo->SetBranchAddress("EventNumber",&EN);  
  tevtinfo->SetBranchAddress("LB",&LB);  
  tevtinfo->SetBranchAddress("BCID",&BCID);  
  tevtinfo->SetBranchAddress("ExtendedLevel1ID",&EL1ID);  
  tevtinfo->SetBranchAddress("Level1TriggerType",&l1TT); 
   
  //ftkdata TTree NTUP_FTK
  FTKTrackStream *ft=new FTKTrackStream();
  TFile fileIn(inputfilenameNTUPFTK.c_str());
  TTree *theTree=(TTree *)fileIn.Get("ftkdata");
  //theTree->Print();
  int nevtntup=theTree->GetEntries();
  auto branch  = theTree->GetBranch("FTKMergedTracksStream");
  branch->SetAddress(&ft);
  vector<uint32_t> data;
  int nevents=getNevents(inputfilenameBS);
  //DEBUG MODE
  //nevents=10;
  std::cout<<" nevents "<<nevents<<std::endl;   
  streampos bufpos=0; 
  uint32_t trackBlockOffsetStart=0;	  // Nr of words in ROB data block before track blocks start
  uint32_t trackBlockOffsetEnd=0;	  // Nr of words in ROB data block after track blocks end
  const uint32_t m_trackBlockSize = 22; // Size of track blocks, expected by decoder. Set according to FTK Data Format
  if (nevents!=nevtntup) {
      std::cout<<"different number of events: "<<nevents<<" "<<nevtntup<<std::endl;
      allmatched=FALSE;
  }
  for (int ievent=1;ievent<=std::min(nevents,nevtntup);ievent++){ //event loop to be processed 
      //--------------------------------------------------------//
      //reading one event from BSfile
      cout<<"Start reading BS file Event "<<ievent<<endl;
      bufpos = read_sb_file(data, inputfilenameBS, 1,bufpos);
      int lengthdata=data.size();
      uint32_t * data32=NULL;
      data32=new uint32_t[lengthdata]; 
      for(int ij=0; ij<lengthdata; ij++) {
          data32[ij]=data[ij];
      }
      IMessageSvc *msgSvc = 0;
      MsgStream msg(msgSvc, "testFTKTrkMatching");
      OFFLINE_FRAGMENTS_NAMESPACE::PointerType pdata=data32;
      //event info are given to FullEventFragment
      eformat::read::FullEventFragment fe(pdata);
      std::vector< eformat::read::ROBFragment > ROBFragments_v;
      if (verbose) cout<<"Run Number "<<fe.run_no()<<" Lvl1 "<<fe.lvl1_id()<<" BCID "<<fe.bc_id()<<std::endl;
      fe.robs( ROBFragments_v );
      FTK_RawTrackContainer* trkcontainer= new FTK_RawTrackContainer();
      std::vector<const FTK_RawTrack *> ftkBSref;
      int nrob=0;
      int nTrks=0;
      for(auto& ROBFragment : ROBFragments_v){
          eformat::helper::SourceIdentifier ROBSource_id = eformat::helper::SourceIdentifier( ROBFragment.source_id() );
          if(ROBSource_id.subdetector_id() != eformat::TDAQ_FTK ){
             continue;
          }
          const uint32_t *rod_data = ROBFragment.rod_data();
          uint32_t rod_ndata = ROBFragment.rod_ndata();
	  if( ( rod_ndata - trackBlockOffsetStart - trackBlockOffsetEnd ) % m_trackBlockSize != 0 ){
	     cout<<"Error: wrong size of rod"<<rod_ndata<<endl;
	     continue;
	  }
          nTrks = ( rod_ndata - trackBlockOffsetStart - trackBlockOffsetEnd ) / m_trackBlockSize;
          rod_data += trackBlockOffsetStart;            // moving data pointer to first track block
	  if (verbose){
	     cout<<"test of working rod l1 "<<ROBFragment.rod_lvl1_id()<<" ndata "<<rod_ndata<<endl;
	     cout<< "decode N tracks "<< decodeNumberOfTracks(rod_data)<< "Ntracks "<<nTrks<<endl;
	  }
	  // decoding the rod info into FTK track container
          StatusCode sc;
	  sc=decode(nTrks,rod_data, trkcontainer,msg);
	  if (trkcontainer->size()==0) {
              cout<<"no FTK tracks from BS file continue"<<endl;
	      continue;
	  }
	  if (verbose){
	     cout<<sc.isSuccess()<<endl;
             cout<< "collection of size " << trkcontainer->size() << endl;
             for ( unsigned int i = 0 ; i < nTrks; ++i ) {
		 cout<<"D0"<<trkcontainer->at(i)->getD0()<<endl;
	     }
	  }
          for ( unsigned int i = 0 ; i < nTrks; ++i ) {
	      ftkBSref.push_back(trkcontainer->at(i));
	  }
	  nrob+=1;	  
	  //delete [] rod_data;
      }
      if (nrob>1){cout<<"!!!!!!!!!!SOMETHING WRONG number of robs >1:"<<nrob<<endl;}
      cout<<"Event "<<ievent<<" N tracks "<<trkcontainer->size()<<endl;
      // end reading info from BS file
      //---------------------------------------------------------------------//
      // start reading info from NTUP_FTK file
      theTree->GetEvent(ievent-1); 
      FTK_RawTrackContainer* trkcontainerNTUP= new FTK_RawTrackContainer();
      std::vector<const FTK_RawTrack *> ftkNTUPtest;
      int NTracksNTUP;
      NTracksNTUP=ft->getNTracks();
      for(unsigned int it=0; it<NTracksNTUP;it++){
        FTKTrack* ftktrk=ft->getTrack(it);
        FTK_RawTrack* rawftktrk=new FTK_RawTrack();
	rawftktrk->setD0(ftktrk->getIP());
	rawftktrk->setZ0(ftktrk->getZ0());
	rawftktrk->setPhi(ftktrk->getPhi());
	rawftktrk->setCotTh(ftktrk->getCotTheta());
	//cout<<"Eta comp"<<TMath::ASinH(rawftktrk->getCotTh())<<" "<<ftktrk->getEta()<<endl;
	rawftktrk->setInvPt(2*ftktrk->getHalfInvPt());
	rawftktrk->setChi2(ftktrk->getChi2());
	rawftktrk->setBarcode(ftktrk->getBarcode());
	trkcontainerNTUP->push_back(rawftktrk);
        ftkNTUPtest.push_back(trkcontainerNTUP->at(it));
      }
      // end reading info from NTUP_FTK file
      //---------------------------------------------------------------------//
      // start comparing infos
      tevtinfo->GetEntry(ievent-1);
      if (NTracksNTUP!=nTrks){std::cout<<"different N tracks: "<<NTracksNTUP<<" "<<nTrks<<std::endl;allmatched=FALSE;}
      if (fe.run_no()!=(unsigned int)RN){std::cout<<"different RunNUmber: "<<fe.run_no()<<" "<<RN<<std::endl; allmatched=FALSE;}
      if (fe.bc_id()!=(unsigned int)BCID){std::cout<<"different BCID: "<<fe.bc_id()<<" "<<BCID<<std::endl;allmatched=FALSE;}
      // end comparing infos
      //---------------------------------------------------------------------//
      // start matching
      for ( unsigned int i = 0 ; i < std::min(nTrks,NTracksNTUP); ++i ) {
          if(trkcontainerNTUP->at(i)->getD0()!=0&&trkcontainerNTUP->at(i)->getZ0()!=0&&trkcontainerNTUP->at(i)->getInvPt()!=0&&
	     trkcontainerNTUP->at(i)->getPhi()!=0&&trkcontainerNTUP->at(i)->getCotTh()!=0&&trkcontainerNTUP->at(i)->getChi2()!=0
	     &&(
	      abs(trkcontainer->at(i)->getD0()/trkcontainerNTUP->at(i)->getD0()-1.)>0.001||
              abs(trkcontainer->at(i)->getZ0()/trkcontainerNTUP->at(i)->getZ0()-1.)>0.001||
              abs(trkcontainer->at(i)->getInvPt()/trkcontainerNTUP->at(i)->getInvPt()-1.)>0.001||
              abs(trkcontainer->at(i)->getPhi()/trkcontainerNTUP->at(i)->getPhi()-1.)>0.001||
              abs(trkcontainer->at(i)->getCotTh()/trkcontainerNTUP->at(i)->getCotTh()-1.)>0.001||
              abs(trkcontainer->at(i)->getChi2()/trkcontainerNTUP->at(i)->getChi2()-1.)>0.001)){
		  std::cout<<"Difference in comparing trk "<<i<<endl;
		  std::cout<<"\td0 : \t"<<trkcontainer->at(i)->getD0()<<"\t"<<trkcontainerNTUP->at(i)->getD0()<<endl;
		  std::cout<<"\tz0 : \t"<<trkcontainer->at(i)->getZ0()<<"\t"<<trkcontainerNTUP->at(i)->getZ0()<<endl;
		  std::cout<<"\tipt : \t"<<trkcontainer->at(i)->getInvPt()<<"\t"<<trkcontainerNTUP->at(i)->getInvPt()<<endl;
		  std::cout<<"\tPhi : \t"<<trkcontainer->at(i)->getPhi()<<"\t"<<trkcontainerNTUP->at(i)->getPhi()<<endl;
		  std::cout<<"\tCotTh : \t"<<trkcontainer->at(i)->getCotTh()<<"\t"<<trkcontainerNTUP->at(i)->getCotTh()<<endl;
		  std::cout<<"\tChi2 : \t"<<trkcontainer->at(i)->getChi2()<<"\t"<<trkcontainerNTUP->at(i)->getChi2()<<endl;
		  allmatched=FALSE;
              }
	  else {if (i%10==0){std::cout<<"Track Parameters btw BS_FTK and NTUP_FTK are the same: good!"<<std::endl;}}
      }
      double dmax=0.2;
      FTKTrkAssoc* m_associator= new FTKTrkAssoc("FTKTrkAssoc", dmax);
      m_associator->match( ftkBSref,ftkNTUPtest);
      const FTK_RawTrack* match_track = NULL;
      for(auto& ref_track : ftkBSref){
         match_track = m_associator->matched(ref_track);
      	 //std::cout<<"Phi matched "<<match_track->getPhi()<<" ref "<<ref_track->getPhi()<<endl;
	 if (ref_track->getPhi()!=0&&abs(match_track->getPhi()/ref_track->getPhi()-1.)>0.001){	    
      	    std::cout<<"Wrong matching: Phi matched "<<match_track->getPhi()<<" ref "<<ref_track->getPhi()<<endl;
	    allmatched=FALSE;
	 }
      }
      
  }// end event loop
  if (allmatched) cout<<"====>Finished successfully: the two files contain the same infos"<<endl;
  else cout<<"====>There was something different, check the printouts"<<endl;*/
  return 0;
}

