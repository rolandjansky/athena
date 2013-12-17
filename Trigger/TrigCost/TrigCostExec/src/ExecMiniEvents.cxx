/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <string>
#include <utility>

// ROOT
#include "TTree.h"
#include "TDirectory.h"
// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/ExecData.h"
#include "TrigCostExec/ExecMiniEvents.h"

REGISTER_ANP_OBJECT(ExecClient,ExecMiniEvents)

using namespace std;
/*
#ifdef __MAKECINT__
#pragma link C++ class vector<pair<unsigned int,vector<pair<unsigned int ,unsigned int> > > >+;
#pragma link C++ class vector<pair<unsigned int,unsigned int > >+;
#endif
*/

//--------------------------------------------------------------------------------------      
Anp::ExecMiniEvents::ExecMiniEvents()
  :fDebug(true),
   fMiniEvt(0),
   fMiniTree(0)
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecMiniEvents::SetReg(const Registry &reg)
{
  //
  // Copy configuration
  //
  fReg = reg;
  
  //
  // Read configuration
  //
  reg.Get("Debug",           fDebug);
  reg.Get("Level",           fLevel);

  //
  // Creating the root file and ttree
  //

  fMiniEvt  = new TFile("MiniEvents.root","recreate");
  fMiniTree = new TTree("MiniEventsTree","Mini-Events with timing information of the ROS-requests");

  fMiniTree->Branch("EventStartMicroSec",&EventStartMicroSec,"EventStartMicroSec/i");
  fMiniTree->Branch("EventStartSec",&EventStartSec,"EventStartSec/i");
  fMiniTree->Branch("EventStopMicroSec",&TimeStampEventStop,"TimeStampEventStop/i");

  fMiniTree->Branch("retrieveStarts",&retrieveStarts);
  fMiniTree->Branch("processingStarts",&processingStarts);
  fMiniTree->Branch("nbROS",&nbROS);
  fMiniTree->Branch("listROS",&listROS);
  fMiniTree->Branch("nbROBperROS",&nbROBperROS);
  fMiniTree->Branch("listROB",&listROB);

  fAlg.clear();
  
}

//--------------------------------------------------------------------------------------      
Anp::ExecMiniEvents::~ExecMiniEvents()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecMiniEvents::SetDir(TDirectory *)
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecMiniEvents::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Save alg call in case the event times out
  //
  fAlg.push_back(alg);
}

//-------------------------------------------------------------------------------
void Anp::ExecMiniEvents::FinishEvent(const Exec::Event &event)
{
  //
  // Fill Algs and Event


//  fMiniTree->Fill();
//  fMiniTree->Write();



  Fill(event);
 // fMiniEvt->Close();

  //
  // Clear and reset for the next event
  //
  fAlg.clear();

}

//-------------------------------------------------------------------------------
const Anp::Exec::Count& Anp::ExecMiniEvents::GetCount() const
{
  return fCount;
}

//-------------------------------------------------------------------------------
void Anp::ExecMiniEvents::WriteExec(HtmlNode &, HtmlMenu &, const Exec::Summary &)
{
  //
  // Write and close the root file
  //    -- function is only called once at the end of all events
  //
  cout << " Fill tree: " << fMiniTree->Fill() << endl;
  fMiniEvt->cd();
  cout << " Write tree: " << fMiniTree->Write() << endl;

  fMiniEvt->Write();
  fMiniEvt->Close();

}

//-------------------------------------------------------------------------------
vector<unsigned int> Anp::ExecMiniEvents::FindRobs(uint32_t ROSId,const std::vector<Exec::ROBData> &rob_data)
{
  //unsigned int ROBid;
  vector<unsigned int> ROBids;
  for(vector<Exec::ROBData>::const_iterator robit = rob_data.begin(); robit != rob_data.end(); ++robit){
    if(ROSId==robit->ros_id){
      ROBids.push_back(robit->rob_data.getROBId());
    }
  }
  return ROBids;
}

//-------------------------------------------------------------------------------
void Anp::ExecMiniEvents::Fill(const Exec::Event &sevt)
{
    retrieveStarts.clear();
    processingStarts.clear();
    nbROS.clear();
    listROS.clear();
    nbROBperROS.clear();
    listROB.clear();
/*
    cout << " Event Sec: " << sevt.evt_data->getSec() << endl;
    cout << " Event NanoSec: " << sevt.evt_data->getNanoSec() << endl;

    cout << " Host Sec: " << sevt.host_sec << endl;
    cout << " Host Micro sec: " << sevt.host_usec << endl;

    cout << " Sec: " << sevt.sec << endl;
    cout << " nano sec: " << sevt.nsec << endl;

    cout << " bxid: " << TrigMonTimer(sevt.evt_data->getTimer()).getUSec() << endl;
    cout << " bxid: " << TrigMonTimer(sevt.evt_data->getTimer()).getSec() << endl;
*/

    int i,j,k;
    i = j = k = 0;
    vector<unsigned int> ROBids;     
    vector<TrigMonSeq> seqs = sevt.evt_data->getVec<TrigMonSeq>();

    for(vector<TrigMonSeq>::iterator seq = seqs.begin(); seq!=seqs.end();++seq){
	i++;
	const vector<TrigMonAlg> &algs = seq->getAlg();
        
 	for(vector<TrigMonAlg>::const_iterator alg = algs.begin(); alg!=algs.end();++alg){
		j++;
        	Exec::AlgCall call = GetAlgCall(*alg,*seq);
		if( j == 1 && i == 1){
			EventStartSec = call.alg_data.start().getSec();
			EventStartMicroSec = call.alg_data.start().getUSec();
		}
		TimeStampEventStop =  call.alg_data.stop().getUSec();

//		cout << "Alg Start: " << call.alg_data.start().getSec() << " " << call.alg_data.start().getUSec() << endl;
//		cout << "Alg Stop: " << call.alg_data.stop().getSec() << " " << call.alg_data.stop().getUSec() << endl;

	       	for(vector<Exec::ROBCall>::const_iterator rob = call.rob_data.begin(); rob != call.rob_data.end(); ++rob){
			k++;
/*
			cout << "Rob Call Start: " << TrigMonTimer(rob->rob_tbeg).getSec() << " " << TrigMonTimer(rob->rob_tbeg).getUSec() << endl;
			cout << "Rob Call End: " << TrigMonTimer(rob->rob_tend).getSec() << " " << TrigMonTimer(rob->rob_tend).getUSec() << endl;
*/
			retrieveStarts.push_back(TrigMonTimer(rob->rob_tbeg).getUSec());
			processingStarts.push_back(TrigMonTimer(rob->rob_tend).getUSec());
			nbROS.push_back(rob->ros_ids.size());
			for(set<uint32_t>::iterator rosit = rob->ros_ids.begin(); rosit != rob->ros_ids.end(); ++rosit){

				listROS.push_back(*rosit);
	    	        	ROBids = FindRobs(*rosit,rob->rob_data);
				nbROBperROS.push_back(ROBids.size());

				for(vector<unsigned>::iterator robit = ROBids.begin(); robit != ROBids.end(); ++robit){
					listROB.push_back(*robit);
				}

	        	}

        	}
			
    	}
    }
//	gDebug=2;
    fMiniEvt->cd();
    cout << " Fill tree: " << fMiniTree->Fill() << endl;
    cout << " tree " << fMiniTree->Write("",TObject::kOverwrite) << endl;
    retrieveStarts.clear();
    processingStarts.clear();
    nbROS.clear();
    listROS.clear();
    nbROBperROS.clear();
    listROB.clear();

}

//---------------------------------------------------------------------------------------
Anp::Exec::AlgCall Anp::ExecMiniEvents::GetAlgCall(const TrigMonAlg &alg,const TrigMonSeq &seq)
{
  if(fDebug)  cout << "ExecMiniEvents::GetAlgCall - starting GetAlgCall" << endl;

  Exec::AlgCall theCall;

  unsigned counter = seq.getChnCounter();
  unsigned index   = seq.getSeqIndex();

  for(vector<Exec::AlgCall>::const_iterator call = fAlg.begin();call!=fAlg.end();++call){
    if(counter == call->chn_conf->getCounter() && index == call->seq_conf->getIndex()){
      //
      // Check if alg time matches
      //
      bool position(alg.getPosition() == call->alg_data.getPosition());
      bool time(alg.start() == call->alg_data.start());
      bool roiId(alg.getRoiId() == call->alg_data.getRoiId());
      if(position && time && roiId) theCall = *call;
    }
  }

  return theCall;  
}

