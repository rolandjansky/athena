/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoWAuditor.h"
#include "GaudiKernel/AudFactory.h"
#include "GaudiKernel/INamedInterface.h"
#include <algorithm>
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include <chrono>

CoWAuditor::CoWAuditor( const std::string& name, 
			ISvcLocator* pSvcLocator ) : 
  Auditor     ( name, pSvcLocator  ),
  AthMessaging(msgSvc(), name),
  m_dumpFinalize(true),m_dumpInfo(true),m_detailed(false),m_streamName("CoWMoO.root"),m_hashTree(0),m_snapshotTree(0),
  m_algId(0),m_vmem(0),m_rss(0),m_pss(0),m_shared(0),m_private(0),m_anon(0),m_swap(0),m_currTime(0)
{
  //
  // Property declaration
  // 
  declareProperty( "DumpAtFinalize", m_dumpFinalize, "Dump statistics at finalize (true)" );
  declareProperty( "DetailedDump", m_detailed, "Dump per library details(false)" );
  declareProperty( "DumpAll", m_dumpInfo, "Dump delta for every call (true)" );
  declareProperty( "MonFile", m_streamName, "output file name (CoWMoO.root)" );
}

CoWAuditor::~CoWAuditor()
{ 
  //m_msg << MSG::DEBUG << "Calling destructor" << endreq;
  delete m_hashTree;
  delete m_snapshotTree;
}

StatusCode CoWAuditor::initialize()
{
  // if(m_detailed){
  //   m_detailedStack.reserve(200);//200 deep stack
  // }else{
  m_summaryStack.reserve(200);//200 deep stack
  // }
  IIncidentSvc* incsvc;
  
  if (StatusCode::SUCCESS!=service("IncidentSvc",incsvc)) {
    ATH_MSG_FATAL( "Incident service not found");
    return StatusCode::FAILURE;
  }
  incsvc->addListener(this,"PreFork", -500);
  incsvc->addListener(this,"PostFork",10000);
  m_snapshotTree=new TTree("Snapshots","Smaps aggregated snapshots for given algorithm");
  m_snapshotTree->Branch("algID",&m_algId,"algID/l");  
  m_snapshotTree->Branch("vmem",&m_vmem,"vmem/L");
  m_snapshotTree->Branch("rss",&m_rss,"rss/L");
  m_snapshotTree->Branch("pss",&m_pss,"pss/L");
  m_snapshotTree->Branch("shared",&m_shared,"shared/L");
  m_snapshotTree->Branch("private",&m_private,"private/L");
  m_snapshotTree->Branch("anon",&m_anon,"anon/L");
  m_snapshotTree->Branch("swap",&m_swap,"swap/L");
  m_snapshotTree->Branch("time",&m_currTime,"time/l");
  //THistSvc does not work for root histograms need another solution
  //Disabling registration for the time being
  // ServiceHandle<ITHistSvc> thistSvc("THistSvc",name());
  // thistSvc->regTree(m_streamName+"/Snapshots",m_snapshotTree);
  return StatusCode::SUCCESS;

}

StatusCode CoWAuditor::finalize()
{
  if(m_dumpFinalize){
    ATH_MSG_INFO("CoW stats Summary");
    char buff [1000];
    std::stringstream oss;
    snprintf(buff,1000,"%28s\t|   %8s    |  %8s    |  %8s    |   %8s    |    %8s    |   %8s    |   %8s    |",
	     "Name","VMem","RSS","PSS","Shared","Private","Swap","Anon");
    ATH_MSG_INFO(buff);
    typedef std::pair<std::string,CoWTools::CoWRecordStats> pair_t;
    std::vector< pair_t > sortedStack;
    sortedStack.reserve(m_runTotals.size());
    for(auto it=m_runTotals.begin();it!=m_runTotals.end();++it){
      sortedStack.push_back(std::make_pair(it->first,it->second));
    }
    std::sort(sortedStack.begin(),sortedStack.end(),[](const pair_t &a,const pair_t &b){return (a.second)<(b.second);});
    for(auto it=sortedStack.begin();it!=sortedStack.end();++it){
      oss.str("");
      oss<<it->second;
      snprintf(buff,1000,"%-28s\t: %s",it->first.c_str(),oss.str().c_str());
      ATH_MSG_INFO(buff);
    }
  }
  std::string algName;
  std::size_t algHash;
  m_hashTree=new TTree("HashTable","Lookup table from hash to alg name");
  m_hashTree->Branch("algHash",&algHash,"algHash/l");  
  m_hashTree->Branch("algName",&algName);
  // ServiceHandle<ITHistSvc> thistSvc("THistSvc",name());
  // thistSvc->regTree(m_streamName+"/hashes",m_hashTree);

  //register tree!
  for(auto& h:m_algHashes){
    algName=h.first;
    algHash=h.second;
    m_hashTree->Fill();
  }
  m_hashTree->Write();
  m_snapshotTree->Write();
  return StatusCode::SUCCESS;
}

void CoWAuditor::beforeInitialize(INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  pushStats(comp->name());
}
 
void CoWAuditor::afterInitialize(INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  popStats(comp->name());
}
 
void CoWAuditor::beforeReinitialize(INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  pushStats(comp->name());
}
 
void CoWAuditor::afterReinitialize( INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  popStats(comp->name());
}
 
void CoWAuditor::beforeExecute(INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  pushStats(comp->name());
}
 
void CoWAuditor::afterExecute( INamedInterface* comp, 
			       const StatusCode& ) 
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  popStats(comp->name());
}
 
void CoWAuditor::beforeBeginRun(INamedInterface* comp){
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  pushStats(comp->name());
}
 
void CoWAuditor::afterBeginRun(INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  popStats(comp->name());
}
 
void CoWAuditor::beforeEndRun(INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  pushStats(comp->name());
}
 
void CoWAuditor::afterEndRun(INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  popStats(comp->name());
}
 
void CoWAuditor::beforeFinalize(INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  pushStats(comp->name());
}
 
void CoWAuditor::afterFinalize(INamedInterface* comp)
{
  ATH_MSG_VERBOSE("Running for "<<comp->name());
  popStats(comp->name());
}
 
void CoWAuditor::before(CustomEventTypeRef evt, 
			const std::string& caller)
{
  ATH_MSG_VERBOSE("Running before for "<<caller<<" with event "<<evt);
  pushStats(caller);
}
 
void CoWAuditor::after(CustomEventTypeRef evt, 
		       const std::string& caller,
		       const StatusCode&)
{
  ATH_MSG_VERBOSE("Running after for "<<caller<<" with event "<<evt);
  popStats(caller);
}

bool CoWAuditor::pushStats(const std::string& caller){
  if(m_detailed){
    auto first=parseDetailedSmaps();
    if(first){
      m_detailedStack.push_back(first);
    }else{
      ATH_MSG_ERROR("Parsing smaps failed while pushing for "<<caller);
      return false;
    }
  }else{
    auto first=parseSmaps();
    if(first){
      first->m_libName=caller;
      m_summaryStack.push_back(first);
    }else{
      ATH_MSG_ERROR("Parsing smaps failed while pushing for "<<caller);
      return false;
    }
  }
  return true;
}

bool CoWAuditor::popStats(const std::string& caller){
  if(m_detailed){
    auto last=parseDetailedSmaps();
    if(m_detailedStack.empty()){
      ATH_MSG_ERROR("Popping detail stack failed for caller="<<caller);
      return false;
    }
    auto first=m_detailedStack.back();
    m_detailedStack.pop_back();
    if(last){
      diffDetailedMaps(last,first);
      if(checkChange(last)){
	if(m_dumpInfo){
	  std::stringstream oss;
	  int count=0;
	  for(auto l:*last){
	    if(*(l.second)){
	      count++;
	      oss<<*(l.second)<<std::endl;
	    }
	  }
	  msg(MSG::INFO)<<caller<<" : Total libs="<<count<<" stack depth="<<m_detailedStack.size()<<std::endl
			<<oss.str()<<endreq;
	}
      }
    }else{
      ATH_MSG_ERROR("Parsing smaps failed for caller="<<caller);
      return false;
    }
  }else{//do summary
    auto last=parseSmaps();
    if(m_summaryStack.empty()){
      ATH_MSG_ERROR("Popping summary stack failed while popping for "<<caller);
      return false;
    }
    auto first=m_summaryStack.back();
    m_summaryStack.pop_back();
    if(last){
      auto delta=(last->m_ms-first->m_ms);
      if(delta){
	for(auto& it:m_summaryStack){
	  it->m_ms+=delta;
	}
	auto it=m_algHashes.insert(std::make_pair(caller,m_hasher(caller)));
	if(it.second){
	  auto hit= m_existingHashes.insert(it.first->second);
	  if(!hit.second){
	    m_existingHashes.insert((it.first->second<<1));
	  }
	  it.first->second=((it.first->second)<<1);
	}	
	auto vals=delta.getValueArray();
	m_vmem=vals[0];
	m_rss=vals[1];
	m_pss=vals[2];
	m_shared=vals[3]+vals[4];
	m_private=vals[5]+vals[6];
	m_anon=vals[8];
	m_swap=vals[10];
	m_algId=it.first->second;
	m_currTime=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	m_snapshotTree->Fill();
	if(m_dumpInfo){
	  msg(MSG::INFO)<<caller<<" : "<<delta<<" oldName="<<first->m_libName<<" stack depth="<<m_summaryStack.size()<<endreq;
	}
	
	auto prev=m_runTotals.find(caller);
	if(prev!=m_runTotals.end()){
	  prev->second += delta;
	}else{
	  m_runTotals.insert(std::make_pair(caller,delta));
	}
      }
    }else{
      ATH_MSG_ERROR("Parsing smaps failed!");
      return false;
    }
  }
  return true;
}

std::shared_ptr<CoWAuditor::LibMap_t> CoWAuditor::parseDetailedSmaps(){
  auto lmap=std::make_shared<CoWAuditor::LibMap_t>();
  std::ifstream pfile("/proc/self/smaps");
  if(pfile.is_open()){
    std::string line;
    std::stringstream ss;  
    while(!std::getline(pfile,line).eof()){
      if(line.empty())continue;
      if(line.size()>30 && line.at(0)!='V'&& ss.str().size()!=0){
	ss.seekg(0);
	//std::cout<<"Ss "<<ss.str()<<" size="<<ss.str().size()<<std::endl;

	auto lib=CoWTools::CoWLibrary::fromRecord(ss,true);
	auto l=lmap->find(lib->m_libName);
	if(l==lmap->end()){
	  lmap->insert(std::make_pair(lib->m_libName,lib));
	}else{
	  *(l->second)+=*lib;
	}
	ss.str("");
	ss.clear();
	// continue;
      }
      
      ss<<line<<std::endl;
      ss.clear();
    }
    if(ss.str().size()>200){
      auto lib=CoWTools::CoWLibrary::fromRecord(ss,true);
      auto l=lmap->find(lib->m_libName);
      if(l==lmap->end()){
	lmap->insert(std::make_pair(lib->m_libName,lib));
      }else{
	*(l->second)+=*lib;
      }
    }
    pfile.close();
  }
  return lmap;

}

std::shared_ptr<CoWTools::CoWLibrary> CoWAuditor::parseSmaps(){
  std::ifstream pfile("/proc/self/smaps");
  std::shared_ptr<CoWTools::CoWLibrary> lr(0);
  if(pfile.is_open()){
    std::string line;
    std::stringstream ss;
    std::string libName="Anonymous";
    lr=std::make_shared<CoWTools::CoWLibrary>(true);
    while(!std::getline(pfile,line).eof()){
      if(line.empty())continue;
      if(line.size()>30 && line.at(0)!='V' &&  ss.str().size()!=0){
	ss.seekg(0);
	m_ms.parseRecord(ss);
	//std::cout<<"MS ="<<ms<<std::endl;
	lr->m_ms+=m_ms;
	ss.str("");
	ss.clear();
	continue;
      }
      ss<<line<<std::endl;
      ss.clear();
    }
    //if there is any records left (should never be possible)
    if(ss.str().size()>200){
      m_ms.parseRecord(ss);
      lr->m_ms+=m_ms;
    }
  }
  pfile.close();
  return lr;
}

bool CoWAuditor::checkChange(std::shared_ptr<LibMap_t> &capture){
  for( auto l: *capture){
    if(*(l.second))return true;
  }
  return false;
}

void CoWAuditor::diffDetailedMaps(std::shared_ptr<LibMap_t> &dst,std::shared_ptr<LibMap_t> &src){
  for( auto l: *src){
    auto old=dst->find(l.first);
    if(old!=dst->end()){
      *(old->second)-=*(l.second);
    }else{
      auto x=std::make_shared<CoWTools::CoWLibrary>();
      *x=-*(l.second);
      dst->insert(std::make_pair(l.first,x));
    }
  } 
}

void CoWAuditor::handle(const Incident &inc){
  ATH_MSG_INFO("Got incident "<<inc.type()<<" from "<<inc.source());
  if(!m_detailed){
    for( auto& i: m_summaryStack){
      auto v=i->m_ms.getValueArray();
      v[3]+=v[5];
      v[5]=0;
      v[4]+=v[6];
      v[6]=0;
    }
    for(auto& i:m_runTotals){
      auto v=i.second.getValueArray();
      v[3]+=v[5];
      v[5]=0;
      v[4]+=v[6];
      v[6]=0;
    }
  }
}
