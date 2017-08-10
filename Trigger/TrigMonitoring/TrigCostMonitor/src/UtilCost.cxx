/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <sstream>

// Framework
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigCostMonitor/UtilCost.h"

using namespace std;

//-----------------------------------------------------------------------------
void Trig::Print(const TrigMonEvent &event,
		 const TrigMonConfig &config,
		 MsgStream &log, MSG::Level level, int verb)
{
  //
  // Print event
  //
  if(log.level() > level) return;

  const std::vector<TrigMonL1Item> lv1 = event.getL1Items();
  const std::vector<TrigMonChain>  hlt = event.getChains();

  if (verb == 0) { // Ultra compact
    float costEvent = 0;
    event.getVar(kIsCostEvent, costEvent); // 47 is magic number for this data. TODO. Put all magic numbers in an enum
    log << level 
        << "TrigMonEvent R:" << event.getRun() 
        << " L:" << event.getLumi()
        << " E:" << event.getEvent()
        << " [#ROB=" << event.size<TrigMonROB>() 
        << "|#ROI=" << event.size<TrigMonRoi>() 
        << "|#SEQ=" << event.size<TrigMonSeq>() 
        << "|#TE=" << event.size<TrigMonTE> ()
        << "|#L1=" << event.getL1Item().size()
        << "|#HLT=" << event.getChain().size() 
        << "|#WORD=" << event.getWord().size() 
        << "|CE?:" << (costEvent < 0.5 ? "N" : "Y")  << "]" << endmsg;
    return;
  }
 
  log << level 
      << "TrigMonEvent run #" << event.getRun() 
      << " lumi #" << event.getLumi()
      << " event #" << event.getEvent() << endmsg;

  if (verb == 1) return;

  log << level
      << "   # of rob  = " << event.size<TrigMonROB>() << endmsg
      << "   # of roi  = " << event.size<TrigMonRoi>() << endmsg
      << "   # of seq  = " << event.size<TrigMonSeq>() << endmsg
      << "   # of te   = " << event.size<TrigMonTE> () << endmsg
      << "   # of l1   = " << event.getL1Item().size() << endmsg
      << "   # of hlt  = " << event.getChain().size()  << endmsg
      << "   # of word = " << event.getWord().size()   << endmsg;

  if (verb == 2) return;
  
  log << level << "   LV1: ";
  for(unsigned int i = 0; i < lv1.size(); ++i) {
    const TrigMonL1Item &val = lv1[i];
    std::vector<TrigConfChain>::const_iterator it = config.findChain(val);
    if(it != config.end<TrigConfChain>()) {
      log << level << it->getName() << " ";
    }
    else {
      log << level << val.getCounter() << " ";
    }
  }
  log << level << endmsg;

  log << level << "   HLT: ";
  for(unsigned int i = 0; i < hlt.size(); ++i) {
    const TrigMonChain &val = hlt[i];
    std::vector<TrigConfChain>::const_iterator it = config.findChain(val);
    if(it != config.end<TrigConfChain>()) {
      log << level << it->getName() << " ";
    }
    else {
      log << level << val.getCounter() << " ";
    }
  }
  log << level << endmsg;

  log << level << "   WORD: ";
  for(unsigned int i = 0; i < event.getWord().size(); ++i) {
    log << level << event.getWord()[i] << " ";
  }
  log << level << endmsg;

  if (verb == 3) return;

  for(unsigned int i = 0; i < event.size<TrigMonRoi>(); ++i) {
    const TrigMonRoi &roi = event.at<TrigMonRoi>(i);
    float eta = -1999.0, phi = -1999.0;
    if(roi.getVarKey().size() > 1 && roi.getVarVal().size() > 1) {
      if(roi.getVarKey()[0] == kRoIEta) eta = roi.getVarVal()[0];
      if(roi.getVarKey()[1] == kRoIPhi) phi = roi.getVarVal()[1];
    }
    
    unsigned rword = 0;
    if(!roi.getWord().empty()) rword = roi.getWord().front();

    log << level << "RoI: id, eta, phi, word: " 
	    << int(roi.getRoiId()) << ", " 
	    << eta << ", " 
	    << phi << ", " 
	    << rword
	    << endmsg;
  }

  if (verb == 4) return;

  for(unsigned int i = 0; i < event.size<TrigMonROB>(); ++i) {
    const TrigMonROB &rob = event.at<TrigMonROB>(i);
    std::string rname;
    
    for(unsigned int s = 0; s < config.size<TrigConfSeq>(); ++s) {
      const TrigConfSeq &seq = config.at<TrigConfSeq>(s);    
      
      for(unsigned int j = 0; j < seq.getNAlg(); ++j) {
	      const TrigConfAlg &alg = seq.getAlg(j);
	      if(rname.empty() && alg.getNameId() == rob.getRequestorId()) {
	        rname = alg.getName();
	        break;
	      }
      }
    }
    
    unsigned int ndata = 0;
    unsigned int rsize = 0;
    
    const std::vector<TrigMonROBData> &data_vec = rob.getData();
    const std::vector<TrigMonROBSum>   data_sum = rob.getSum();
    
    for(unsigned int d = 0; d < data_vec.size(); ++d) {
      const TrigMonROBData &data = data_vec.at(d);
      rsize += data.getROBSize();
      ndata++;
    }
    
    for(unsigned int d = 0; d < data_sum.size(); ++d) {
      const TrigMonROBSum &dsum = data_sum[d];
      rsize += dsum.getSize();
      ndata += dsum.getNROB();
    }  
    
    log << level << "ROB " 
	      << rname << "/" << rob.getRequestorId() << ": "
	      << "start: " << rob.start().getSec() + 1.e-6*rob.start().getUSec()
	      << " stop: " << rob.stop().getSec() + 1.e-6*rob.stop().getUSec()
	      << " nrob="  << rob.getData().size()
	      << " #sum="  << rob.getSum().size()
	      << " #data=" << ndata << ", "
	      << " ROB size=" << rsize 
	      << endmsg;
  }

  if (verb == 5) return;

  const std::vector<uint32_t> &key_vec = event.getVarKey();
  const std::vector<float>    &var_vec = event.getVarVal();
  
  if(var_vec.size() == key_vec.size()) {
    for(unsigned i = 0; i < key_vec.size(); ++i) {
      log << " key=" << key_vec.at(i) << " var=" << var_vec.at(i) << endmsg;
    }
  }
  else {
    log << "Size mismatch for var and key vectors" << endmsg;
  }
}

//-----------------------------------------------------------------------------
void Trig::Print(const TrigMonConfig &config,
		 std::ostream &os)
{
  //
  // Print configuration
  //
  os << "TrigMonConfig - print full configuration" << endl
     << "  run   " << config.getRun()   << endl
     << "  lumi  " << config.getLumi()  << endl
     << "  event " << config.getEvent() << endl
     << "  sec   " << config.getSec()   << endl
     << "  nsec  " << config.getNanoSec()   << endl
     << "  SMK   " << config.getMasterKey() << endl
     << "  LV1PS " << config.getLV1PrescaleKey() << endl
     << "  HLTPS " << config.getHLTPrescaleKey() << endl
     << "  #chains:    " << config.size<TrigConfChain>() << endl
     << "  #sequences: " << config.size<TrigConfSeq>()   << endl
     << "  #variables: " << config.getVarName().size()   << endl    
     << "  #pairs:     " << config.getPairKey().size()   << endl;
  
  for(unsigned l = 0; l < config.size<TrigConfChain>(); ++l) {
    const TrigConfChain &c = config.at<TrigConfChain>(l);

    os << "chain: " << c.getLevel() << " counter=" << setw(4) << c.getCounter() 
       << " id=" << setw(10) << c.getId() << " " << c.getName() << endl
       << "  lower chain: " << setw(4) << c.getLowerCounter() 
       << " id=" << setw(10) << c.getLowerId() << " "<< c.getLowerName() << endl
       << "  PS=" << c.getPrescale() << " PT=" << c.getPassThrough() << endl;

    os << "  " << c.getSignature().size() << " signature(s)" << endl;
    for(unsigned i = 0; i < c.getSignature().size(); ++i) {
      const TrigConfSig &s = c.getSignature().at(i);
      os << "  logic=" << s.getLogic() << " counter=" << s.getCounter()
	 << " label: "<< s.getLabel() << endl
	 << "    " << s.getOutputTEs().size() << " output TE(s)" << endl;
      
      for(unsigned t = 0; t < s.getOutputTEs().size(); ++t) {
	const unsigned tid = s.getOutputTEs().at(t);
	std::vector<TrigConfSeq>::const_iterator it = config.findSeq<uint32_t>(tid);
	if(it == config.end<TrigConfSeq>()) {
	  os << "    TE: " << setw(10) << tid << " - missing TE configuration" << endl;
	}
	else {
	  os << "    TE: " << setw(10) << tid << " - " << it->getName() << endl;
	}
      }
    }
    
    os << "  " << c.getStream().size() << " stream(s)" << endl;
    for(unsigned i = 0; i < c.getStream().size(); ++i) {
      os << "    " << c.getStream().at(i) 
	 << " PS=" << c.getSignaturePrescale(c.getStream().at(i)) << endl;
    }

    os << "  " << c.getGroup().size() << " group(s)" << endl;
    for(unsigned i = 0; i < c.getGroup().size(); ++i) {
      os << "    " << c.getGroup().at(i) << endl;
    }

    os << "  " << c.getLowerIds().size() << " lower chain id(s)" << endl;
    for(unsigned i = 0; i < c.getLowerIds().size(); ++i) {
      os << "    " << c.getLowerIds().at(i) << endl;
    }
  }

  for(unsigned l = 0; l < config.size<TrigConfSeq>(); ++l) {
    const TrigConfSeq &s = config.at<TrigConfSeq>(l);
    os << "Sequence: index=" << setw(6) << s.getIndex() 
       << " id=" << setw(10) << s.getId() << " topoTE=" << s.getTopoTE()
       << " name=" << s.getName() << endl;
    
    for(unsigned i = 0; i < s.getInputTEs().size(); ++i) {
      const unsigned tid = s.getInputTEs().at(i);
      std::vector<TrigConfSeq>::const_iterator it = config.findSeq<uint32_t>(tid);
      if(it == config.end<TrigConfSeq>()) {
	os << "    input TE " << i << ": " << setw(10) << tid << " - missing TE configuration" << endl;
      }
      else {
	os << "    input TE " << i << ": " << setw(10) << tid << " - " << it->getName() << endl;
      }
    }

    for(unsigned i = 0; i < s.getAlg().size(); ++i) {
      const TrigConfAlg &a = s.getAlg().at(i);
      os << "    algorithm " << setw(2) << i << ": index=" << setw(5) << a.getIndex() 
	 << " position=" << setw(2) << int(a.getPosition())
	 << " name/type: " << setw(10) << a.getNameId() << "/" << setw(10) << a.getTypeId() << " "
	 << a.getName() << "/" << a.getType() << endl;
    }
  }
}

//---------------------------------------------------------------------------------------
std::string Trig::getUniqueString()
{
  //
  // Make unique string using: run.date.time.hostname.pid
  // 
  
  // Get time and pid
  const time_t my_time = time(0);
  const pid_t  my_pid  = getpid();
  const struct tm *my_tm = localtime(&my_time);

  std::stringstream result;
  
  if(my_tm) {
    result << my_tm->tm_year+1900 << "-" 
	   << std::setw(2) << std::setfill('0') << my_tm->tm_mon+1 << "-" 
	   << std::setw(2) << std::setfill('0') << my_tm->tm_mday
	   << "."
	   << std::setw(2) << std::setfill('0') << my_tm->tm_hour << "-" 
	   << std::setw(2) << std::setfill('0') << my_tm->tm_min << "-" 
	   << std::setw(2) << std::setfill('0') << my_tm->tm_sec;
  }
  else {
    result << "anytime";
  }

  // Determine host name
  char my_host[256];
  std::string host_str;
  if(gethostname(my_host, 256) == 0) {
    host_str = my_host;    
  }
  else {
    host_str = "unknown";
  }

  result << "."
	 << my_host
	 << "-" << std::setw(5) << std::setfill('0') << my_pid;
  
  return result.str();
}

//---------------------------------------------------------------------------------------
std::string Trig::MergeEvent(TrigMonEvent &event, 
			     const TrigMonEvent &other, 
			     int level_other,
			     const std::string &option)
{
  //
  // Add items from "other" to "event"
  //
  std::stringstream log;
  
  //
  // Check header information: L2 and EF event numbers are different!
  //
  if(event.getRun() != other.getRun()) {
    log << "MergeEvent - run mismatch:" << event.getRun() << "!=" << other.getRun() << std::endl;
  }
  if(event.getLumi() != other.getLumi()) {
    log << "MergeEvent - lb mismatch:" << event.getLumi() << "!=" << other.getLumi() << std::endl;
  }
  if(event.getSec() != other.getSec()) {
    log << "MergeEvent - sec mismatch:" << event.getSec() << "!=" << other.getSec() << std::endl;
  }
  if(event.getNanoSec() != other.getNanoSec()) {
    log << "MergeEvent - nsec mismatch:" << event.getNanoSec() << "!=" << other.getNanoSec() << std::endl;
  }
  if(event.getBunchId() != other.getBunchId()) {
    log << "MergeEvent - bcid mismatch:" << event.getBunchId() << "!=" << other.getBunchId() << std::endl;
  }

  const std::vector<TrigMonL1Item> event_lv1 = event.getL1Items();
  const std::vector<TrigMonChain>  event_hlt = event.getChains();
  const std::vector<TrigMonVar>    event_var = event.getVar();

  const std::vector<TrigMonL1Item> other_lv1 = other.getL1Items();
  const std::vector<TrigMonChain>  other_hlt = other.getChains();
  const std::vector<TrigMonVar>    other_var = other.getVar();
  
  //
  // Copy TrigMonL1Item: special case
  //
  for(unsigned int i = 0; i < other_lv1.size(); ++i) {
    const TrigMonL1Item &val = other_lv1.at(i);

    if(std::find(event_lv1.begin(), event_lv1.end(), val) == event_lv1.end()) {
      event.add<TrigMonL1Item>(val);
    }
    else {
      log << "Duplicate TrigMonL1Item: counter=" << val.getCounter() << std::endl;
    }
  }

  //
  // Copy TrigMonChain: special case
  //
  for(unsigned int i = 0; i < other_hlt.size(); ++i) {
    const TrigMonChain &val = other_hlt.at(i);
    const std::vector<TrigMonChain>::const_iterator fit = std::find(event_hlt.begin(), event_hlt.end(), val);

    if(fit == event_hlt.end()) {
      event.add<TrigMonChain>(val);
    }
    else if(val.getLevel() == 2 && option.find("IgnoreL2Duplicate") != std::string::npos) {
      if(val.getEncoded() != fit->getEncoded()) {
	log << "Duplicate TrigMonChain: level, counter = " 
	    << val.getLevel() << ", " << val.getCounter() 
	    << " - should not be ignored because of mismatch in payload: "
	    << val.getEncoded() << "!=" << fit->getEncoded() << std::endl;
      }
      else {
	log << "Duplicate TrigMonChain: level, counter = " 
	    << val.getLevel() << ", " << val.getCounter() << std::endl;
      }
    }
  }

  //
  // Copy TrigMonVar: special case
  //
  for(unsigned int i = 0; i < other_var.size(); ++i) {
    const TrigMonVar &val = other_var[i];

    std::vector<TrigMonVar>::const_iterator vit = std::find(event_var.begin(), event_var.end(), val);
    if(vit == event_var.end()) {
      event.addVar(val.key(), val.data());
    }
  }

  //
  // Copy TrigMonSeq wihout checks
  //
  for(unsigned int i = 0; i < other.size<TrigMonSeq>(); ++i) {
    event.add<TrigMonSeq>(other.at<TrigMonSeq>(i));
  }

  if(level_other == 2) {
    //
    // Copy TrigMonRoi
    //
    for(unsigned int i = 0; i < other.size<TrigMonRoi>(); ++i) {
      event.add<TrigMonRoi>(other.at<TrigMonRoi>(i));
    }
    //
    // Copy TrigMonROB
    //
    for(unsigned int i = 0; i < other.size<TrigMonROB>(); ++i) {
      event.add<TrigMonROB>(other.at<TrigMonROB>(i));
    }
  }
  else if(level_other == 3) {
    //
    // Copy TrigMonTE
    //
    for(unsigned int i = 0; i < other.size<TrigMonTE>(); ++i) {
      event.add<TrigMonTE>(other.at<TrigMonTE>(i));
    }
  }
  else if(level_other == 4) {
    //
    // Copy TrigMonRoi
    //
    for(unsigned int i = 0; i < other.size<TrigMonRoi>(); ++i) {
      event.add<TrigMonRoi>(other.at<TrigMonRoi>(i));
    }
    //
    // Copy TrigMonROB
    //
    for(unsigned int i = 0; i < other.size<TrigMonROB>(); ++i) {
      event.add<TrigMonROB>(other.at<TrigMonROB>(i));
    }
    //
    // Copy TrigMonTE
    //
    for(unsigned int i = 0; i < other.size<TrigMonTE>(); ++i) {
      event.add<TrigMonTE>(other.at<TrigMonTE>(i));
    }
  }


  return log.str();
}
