/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Local
#include "TrigCostBase/Registry.h"

// Local
#include "TrigCostExec/TrigTimeAlg.h"
#include "TrigCostExec/CostService.h"
#include "TrigCostExec/TestInterface.h"

using namespace std;

//-----------------------------------------------------------------------------------------      
void Anp::TestInterface::RunTest(const string &config_path, int nevent)
{
  //
  // Run test of cost interface 
  //
  CostService::Instance().Config(config_path);

  for(int i = 0; i < nevent; ++i) {
    CostService::Instance().LoadNextEvent();
    
    cout << "-----------------------------------------------------------------" << endl
	 << "Event number: " << i << endl;
    
    //
    // Get event information
    //
    const Anp::Exec::Event event = CostService::Instance().GetEvent("execL2");
    
    //
    // Print brief event summary
    //
    cout << "run=" << event.run << " lb=" << event.lb << endl
	 << "L1 event time - sec:      " << event.sec           << endl
	 << "L1 event time - sec%3600: " << event.sec%3600      << endl
	 << "L1 event time - nsec:     " << event.nsec          << endl
	 << "HLT application name:     " << event.host          << endl
	 << "HLT host time - sec:      " << event.host_sec      << endl
	 << "HLT host time - sec%3600: " << event.host_sec%3600 << endl
	 << "HLT host time - microsec: " << event.host_usec     << endl
	 << "total event time (ms):    " << event.timer_steer   << endl;

    TrigMonTimer talg0, talg1;

    //
    // Get all executed HLT algorithms
    //
    const std::vector<Anp::Ptr<Anp::Exec::AlgCall> > calls = CostService::Instance().GetAlgCalls("execL2");

    //
    // Iterate over executed algorithms
    //
    for(unsigned i = 0; i < calls.size(); ++i) {
      const Exec::AlgCall &alg = calls.at(i).ref();

      //
      // Print brief summary of algorithm execution
      //
      const TrigMonTimer t0 = alg.alg_data.start();
      const TrigMonTimer t1 = alg.alg_data.stop();

      cout << "Algorithm name: " << alg.alg_conf->getName() << endl
	   << "  execution time (ms): " << alg.alg_data.getTimer() << endl
	   << "  execution start:     " << t0.getSec() << " sec, " << t0.getUSec() << " microsec" << endl
	   << "  execution stop:      " << t1.getSec() << " sec, " << t1.getUSec() << " microsec" << endl
	   << "  # of data requests:  " << alg.rob_data.size()     << endl;
      
      if(talg0.getEncoded() != 0) {
	if(t0    < talg0) talg0 = t0;
	if(talg1 < t1)    talg1 = t1;
      }
      else {
	 talg0 = t0;
	 talg1 = t1;
      }

      for(unsigned j = 0; j < alg.rob_data.size(); ++j) {
	const Exec::ROBCall &data_req = alg.rob_data.at(j);
	
	//
	// Print brief summary for single data request
	//
	const TrigMonTimer rt0(data_req.rob_tbeg);
	const TrigMonTimer rt1(data_req.rob_tend);

	cout << "    data request number: " << j << endl
	     << "    request time (ms):   " << data_req.rob_timer       << endl
	     << "    request start:       " << rt0.getSec() << "sec, " << rt0.getUSec() << " microsec" << endl
	     << "    request stop:        " << rt1.getSec() << "sec, " << rt1.getUSec() << " microsec" << endl
	     << "    number of ROBs:      " << data_req.rob_data.size() << endl;
	
	//
	// Print ROB ids
	//	
	std::stringstream rob_str;

	for(unsigned r = 0; r < data_req.rob_data.size(); ++r) {
	  const Exec::ROBData &rob = data_req.rob_data.at(r);
	  const Exec::ROBConf &cob = rob.rob_conf.ref();
	  
	  rob_str << "      "
		  << "ROB id=0x" << std::hex << rob.rob_data.getROBId()
		  << " retrieved=" << rob.rob_data.isRetrieved() 
		  << " cached="    << rob.rob_data.isCached() 
		  << " subdet="    << alg.GetDetName(cob.det_id) 
		  << " ROS="       << alg.GetROSName(cob.ros_id) 
		  << endl;
	}
	
	cout << rob_str.str();
      }
    }

    if(talg0.getEncoded() != 0) {
      const TrigMonTimer tevent(event.host_sec % 3600, event.host_usec);
      
      cout << "Event timer (ms):               " << event.timer_steer     << endl
	   << "Delta t: t_event_end - t_alg_0: " << tevent.elapsed(talg0) << endl
	   << "Delta t: t_event_end - t_alg_1: " << tevent.elapsed(talg1) << endl;
    }
  }
}
