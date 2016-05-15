/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// AthMemoryAuditor.cxx 
// Author: Rolf Seuster

#include "GaudiKernel/AudFactory.h"
#include "GaudiKernel/INamedInterface.h"

#include "AthMemoryAuditor.h"

#include <boost/lexical_cast.hpp>

#include <cstdint>
#include <string.h>
#include <utility> // for std::pair
#include <stdlib.h> // for getenv

// needed for placement new
#include <new> 

#include <cstddef>
#include <typeinfo>
#include <dlfcn.h>
#include <sstream>

#include "memory_hooks-common.h"
#include "memory_hooks-stdcmalloc.h"
#include "memory_hooks-tcmalloc.h"

bool AthMemoryAuditor::m_usetcmalloc=true;

AthMemoryAuditor::AthMemoryAuditor( const std::string& name, 
				    ISvcLocator* pSvcLocator ) : 
  Auditor     ( name, pSvcLocator  ),
  AthMessaging(msgSvc(), name),
  m_reported(true),
  m_stmax(10),
  m_defaultStacktraceDepth(0)
{
  declareProperty("DefaultStacktraceDepth", m_defaultStacktraceDepth);
  declareProperty("StacktraceDepthPerAlg", m_depthPerAlg);
  declareProperty("MaxStacktracesPerAlg", m_stmax);
  current_stage=1;
}

AthMemoryAuditor::~AthMemoryAuditor()
{
  ATH_MSG_DEBUG("In Destructor");
}

StatusCode
AthMemoryAuditor::finalize()
{
  // silence when called outside of athena
  if ( current_stage > 1 )
    {
      if(m_usetcmalloc)
	uninstall();
      ATH_MSG_INFO("In finalize");
      if(!m_reported)
	{
	  report();
	  m_reported=true;
	}
      
      ATH_MSG_INFO("");
      ATH_MSG_INFO("Malloc Statistics:");
      if(m_usetcmalloc)
	{
	  ATH_MSG_INFO("number of mallocs      : " << counter_tc_m);
	  ATH_MSG_INFO("number of frees        : " << counter_tc_f);
	  ATH_MSG_INFO(" allocated then freed  : " << counter_tc_mtf);
	  ATH_MSG_INFO("mmaped                 : " << counter_tc_mm);
	  ATH_MSG_INFO("munmaped               : " << counter_tc_mum);
	}
      else
	{
	  ATH_MSG_INFO("number of mallocs      : " << counter_m);
	  ATH_MSG_INFO("number of frees        : " << counter_f);
	  ATH_MSG_INFO(" no associated malloc  : " << counter_fna);
	  ATH_MSG_INFO("number of reallocs     : " << counter_r);
	  ATH_MSG_INFO(" to shrink             : " << counter_rs);
	  ATH_MSG_INFO(" to grow               : " << counter_rg);
	  ATH_MSG_INFO(" unknown               : " << counter_rnr);
	  ATH_MSG_INFO(" used as malloc        : " << counter_rm);
	  ATH_MSG_INFO(" used as free          : " << counter_rf);
	  ATH_MSG_INFO("number of memalign     : " << counter_ma);
	}
      // some memory statistics for debugging
      if (this->msgLvl(MSG::DEBUG))
	{
	  std::ifstream infile("/proc/self/status");
	  std::string line;
	  while (std::getline(infile, line))
	    {
	      ATH_MSG_DEBUG(line);
	    }
	}
      // we are done. When cleaning up itself, crash might occure, so let's not free
      // anything anymore...
      finished=true;
    }
  return StatusCode::SUCCESS;
}

StatusCode AthMemoryAuditor::initialize()
{
  current_stage=2;
  
  m_reported=false;
  
  int l(0);
  algorithms[l]="N/A";
  arrayAlgIndex["N/A"]=aiStruct(l,m_defaultStacktraceDepth); ++l;
  algorithms[l]="'framework'";
  arrayAlgIndex["'framework'"]=aiStruct(l,m_defaultStacktraceDepth); ++l;
  
  stacktraceDepth=m_defaultStacktraceDepth;
  collectStacktraces=bool(m_defaultStacktraceDepth);
  
  ATH_MSG_INFO("==> initialize");
  
  for(auto it=m_depthPerAlg.begin(); it!=m_depthPerAlg.end(); ++it )
    {
      ATH_MSG_INFO("INIT " << *it);
      std::istringstream buffer(*it);
      std::string s;
      int r=0;
      buffer >> s >> r;
      ATH_MSG_INFO("INIT leaks in algorithm " << s << " will have stacktrace depth of " << r );
      arrayAlgIndex[s]=aiStruct(l,r);
      algorithms[l]=s;
      ++l;
    }
  curMaxIndex=l-1;
  
  // replace test with coe from here
  // perftest/Control/AthenaAuditors/src/TCMallocAuditor.cxx:  MallocExtension::instance()->GetNumericProperty("generic.current_allocated_bytes", &m_value);
  // TCTEST/Control/AthenaAuditors/src/FPEAuditor.cxx:bool MallocExtension::GetNumericProperty(const char* /* property */, size_t* /* value*/ ) { return true; }
  // TCTEST/Control/AthenaAuditors/src/FPEAuditor.cxx:      MallocExtension::instance()->GetNumericProperty("generic.current_allocated_bytes", &value);
  
  if ( ! initialized )
    {
      // If tcmalloc is not preloaded this call goes to our implementation which will set m_usetcmalloc to false
      MallocExtension::instance();
      
      if(m_usetcmalloc)
	{
	  ATH_MSG_INFO("Using hooks for tcmalloc");
	  my_init_tcmalloc();
	}
      else
	{
	  ATH_MSG_INFO("Using hooks for stdcmalloc");
	  ATH_MSG_WARNING("stdcmalloc is currently not well supported, consider using the much faster tcmalloc implementation");
	  my_init_hook();
	}
    }
  
  return StatusCode::SUCCESS;
}
 
void AthMemoryAuditor::report()
{
  if ( current_stage > 1 )
    {
      current_stage = 0;
      
      std::cerr << "AthMemoryAuditor : Creating report\n";
      std::cerr << "==================================\n";
      
      std::cerr << "-------------------------------------------------------------------------------------\n";
      if(getenv("AtlasBaseDir"))
	std::cerr << "| AtlasBaseDir : " << std::setw(66) << getenv("AtlasBaseDir")  << " |\n";
      if(getenv("AtlasVersion"))
	std::cerr << "| AtlasVersion : " << std::setw(66) << getenv("AtlasVersion")  << " |\n";
      if(getenv("CMTCONFIG"))
	std::cerr << "| CMTCONFIG    : " << std::setw(66) << getenv("CMTCONFIG")  << " |\n";
      std::cerr << "-------------------------------------------------------------------------------------\n";
      std::cerr << " Note: to see line numbers in below stacktrace you might consider running following :\n";
      std::cerr << "  atlasAddress2Line --file <logfile> [--alg <algorithm>]\n";
      std::cerr << "-------------------------------------------------------------------------------------\n";
      
      std::cerr << "Report of summary of leaks\n";
      std::cerr << "--------------------------\n";
      
      std::string str;
      std::string str0("None");
      
      std::vector<unsigned long> leaksInStage(10,0);
      std::vector<unsigned long> leakedMemInStage(10,0);
      
      myBlocks_tc *b=new myBlocks_tc();
      myBlocks_tc *b2=new myBlocks_tc();
      
      struct info
      {
	uint32_t total_size;
	std::vector<allocSet_tc::iterator> l;
	std::vector<uintptr_t> hash;
      };
      
      std::map<uint32_t,std::map<uint32_t,info> > accum;
      
      // first the code for tcmalloc
      if(m_usetcmalloc)
	{
	  for( allocSet_tc::iterator it=allocset_tc.begin(); it!=allocset_tc.end(); ++it )
	    {
	      unsigned int reportStage(0);
	      uint32_t stage   = it->curstage;
	      size_t   size    = it->allocsize;
	      uint32_t context = it->algindex;
	      reportStage=stage;
	      if( stage >= 1E6 && stage < 9E7 )
		reportStage=0;
	      if(stage == 1E6-10 )
		reportStage=5;
	      if(stage == 9E7 )
		reportStage=6;
	      if(stage == 1E8-2 )
		reportStage=7;
	      leaksInStage[reportStage]++;
	      leakedMemInStage[reportStage]+=size;
	      
	      uintptr_t h(0);
	      if (it!=allocset_tc.end())
		{
		  if(it->allocatedFrom.size()>=2)
		    h=(uintptr_t)it->allocatedFrom[1];
		  for ( unsigned int j = 2; j < it->allocatedFrom.size(); j++)
		    if(it->allocatedFrom[j])
		      h^=(uintptr_t)it->allocatedFrom[j];
		}
	      
	      accum[stage][context].total_size+=size;
	      accum[stage][context].l.push_back(it);
	      accum[stage][context].hash.push_back(h);
	    }
	}
      else
	{
	  for ( unsigned int reportStage(0); reportStage<8; ++reportStage )
	    {
	      for ( std::map<uint32_t,info>::const_iterator it=accum[reportStage].begin(); it!=accum[reportStage].end(); ++it )
		std::cerr << " NOW " << it->first << " / " << it->second.total_size << "\n";
	      
	      // accum[stage][context].total_size+=size;
	    }
	  for ( unsigned int reportStage(0); reportStage<8; ++reportStage )
	    {
	      /* std::cerr << "INIT : s_first / s_last " << &s_first << " / " << &s_last << "\n";
		 std::cerr << "INIT : s_first / s_last "
		 << s_first.getNext() << " / " << s_last.getPrev() << "\n"; */
	      
	      node* nf(s_first.getNext());
	      node* np(s_first.getNext());
	      node* nn(s_first.getNext());
	      while ( nf != &s_last )
		{
		  // std::cerr << "INIT : nf " << nf << "\n";
		  nn=nf->getNext();
		  size_t   size    = nf->getSize();
		  redzone *r = new ((void*)((uintptr_t)(nf)+deltaLow+size)) redzone( false );
		  uint32_t stage   = r->getStage();
		  uint32_t context = r->getAlgIndex();
		  
		  np=nf;
		  nf=nn;
		  
		  if( reportStage==0 )
		    if( stage < 1E6 || stage >= 9E7 )
		      continue;
		  if(reportStage==1)
		    if(stage != 1 )
		      continue;
		  if(reportStage==2)
		    if(stage != 2 )
		      continue;
		  if(reportStage==3)
		    if(stage != 3 )
		      continue;
		  if(reportStage==4)
		    if(stage != 4 )
		      continue;
		  if(reportStage==5)
		    if(stage != 1E6-10 )
		      continue;
		  if(reportStage==6)
		    if(stage != 9E7 )
		      continue;
		  if(reportStage==7)
		    if(stage != 1E8-2 )
		      continue;
		  
		  leaksInStage[reportStage]++;
		  leakedMemInStage[reportStage]+=size;
		  
		  uintptr_t h(0);
		  b->allocated=(uintptr_t)(np+np->getDeltaPayload());
		  allocSet_tc::iterator it = allocset_tc.find(*b);
		  if (it!=allocset_tc.end())
		    {
		      if(it->allocatedFrom.size()>=2)
			h=(uintptr_t)it->allocatedFrom[1];
		      for ( unsigned int j = 2; j < it->allocatedFrom.size(); j++)
			if(it->allocatedFrom[j])
			  h^=(uintptr_t)it->allocatedFrom[j];
		      
		      /* std::cerr << "RS find : "
			 << size << " " << it->allocsize << " | "
			 << stage << " " << it->curstage << "\n"; */
		      
		      it->allocsize = size;
		      it->curstage  = stage;
		      // it->context   = algindex;
		    }
		  
		  accum[stage][context].total_size+=size;
		  accum[stage][context].l.push_back(it);
		  accum[stage][context].hash.push_back(h);
		}
	    }
	}
      
      std::cerr << "Summary\n" ;
      std::cerr << "  " << leaksInStage[0] << " Reported leak(s) allocated in event loop, leaking " << leakedMemInStage[0] << " bytes\n";
      std::cerr << "  " << leaksInStage[1] << " Reported leak(s) allocated in constructors, leaking " << leakedMemInStage[1] << " bytes\n";
      std::cerr << "  " << leaksInStage[2] << " Reported leak(s) allocated before initialize, leaking " << leakedMemInStage[2] << " bytes\n";
      std::cerr << "  " << leaksInStage[3] << " Reported leak(s) allocated in initialize, leaking " << leakedMemInStage[3] << " bytes\n";
      std::cerr << "  " << leaksInStage[4] << " Reported leak(s) allocated in re-initialize, leaking " << leakedMemInStage[4] << " bytes\n";
      std::cerr << "  " << leaksInStage[5] << " Reported leak(s) allocated in begin-run, leaking " << leakedMemInStage[5] << " bytes\n";
      std::cerr << "  " << leaksInStage[6] << " Reported leak(s) allocated in end-run, leaking " << leakedMemInStage[6] << " bytes\n";
      std::cerr << "  " << leaksInStage[7] << " Reported leak(s) allocated in finalize, leaking " << leakedMemInStage[7] << " bytes\n";
      
      std::cerr << std::dec << "\n";
      
      for ( unsigned int reportStage(0); reportStage<8; ++reportStage )
	{
	  switch(reportStage)
	    {
	    case 0: std::cerr << "Reporting leaks allocated in event loop\n"; break;
	    case 1: std::cerr << "Reporting leaks allocated in constructors\n"; break;
	    case 2: std::cerr << "Reporting leaks allocated before initialize\n"; break;
	    case 3: std::cerr << "Reporting leaks allocated in initialize\n"; break;
	    case 4: std::cerr << "Reporting leaks allocated in re-initialize\n"; break;
	    case 5: std::cerr << "Reporting leaks allocated in begin-run\n"; break;
	    case 6: std::cerr << "Reporting leaks allocated in end-run\n"; break;
	    case 7: std::cerr << "Reporting leaks allocated in finalize\n"; break;
	    }
	  // remember leaks
	  std::map<uint32_t,info> leaks;
	  
	  for(std::map<uint32_t,std::map<uint32_t,info> >::const_iterator outer_iter=accum.begin(); outer_iter!=accum.end(); ++outer_iter)
	    {
	      uint32_t stage = outer_iter->first;
	      
	      if( reportStage==0 )
		if( stage < 1E6 || stage >= 9E7 )
		  continue;
	      if(reportStage==1)
		if(stage != 1 )
		  continue;
	      if(reportStage==2)
		if(stage != 2 )
		  continue;
	      if(reportStage==3)
		if(stage != 3 )
		  continue;
	      if(reportStage==4)
		if(stage != 4 )
		  continue;
	      if(reportStage==5)
		if(stage != 1E6-10 )
		  continue;
	      if(reportStage==6)
		if(stage != 9E7 )
		  continue;
	      if(reportStage==7)
		if(stage != 1E8-2 )
		  continue;
	      
	      // add all leaks from this algorithm in this stage (event loop has several) and sort by size
	      for(std::map<uint32_t,info>::const_iterator inner_iter = outer_iter->second.begin(); inner_iter!=outer_iter->second.end(); ++inner_iter)
		{
		  leaks[inner_iter->first].total_size+=inner_iter->second.total_size;
		  leaks[inner_iter->first].l.insert(leaks[inner_iter->first].l.end(), inner_iter->second.l.begin(), inner_iter->second.l.end());
		  leaks[inner_iter->first].hash.insert(leaks[inner_iter->first].hash.end(), inner_iter->second.hash.begin(), inner_iter->second.hash.end());
		}
	    }
	  
	  // printout, sorted by size of leaks per algorithms in this step, largest first
	  while (! leaks.empty())
	    {
      	      auto imax=leaks.begin();
	      for(auto iter=leaks.begin(); iter!=leaks.end(); ++iter)	  
		if(imax->second.total_size < iter->second.total_size)
		  imax=iter;
	      uint32_t ai=imax->first;
	      std::map<uint32_t, std::string>::const_iterator ait=algorithms.find(ai);
	      if ( ait != algorithms.end() )
		str=ait->second;
	      else
		str=str0;
	      std::cerr << " Accumulated Leak(s) in algorithm " << str << " of total size " << imax->second.total_size << " bytes\n";
	      
	      auto fit=arrayAlgIndex.find(str);
	      if ( fit == arrayAlgIndex.end() )
		{
		  std::cerr << "something is wrong \n";
		  abort();
		}
	      else
		{
		  stacktraceDepth=fit->second.stacktrace;
		  // std::cerr << " ROLF : stacktracesize " << stacktraceDepth << "\n";
		  if(stacktraceDepth > 0 )
		    {
		      std::vector<info> thisAlg;
		      
		      // print at most m_stmax stack traces per algorithm
		      int stmax=m_stmax;
		      for(unsigned int i_tr = 0; i_tr < imax->second.l.size(); ++i_tr )
			{
			  info leakSum;
			  
			  size_t   size    = imax->second.l[i_tr]->allocsize;
			  // redzone *r = new ((void*)((uintptr_t)imax->second.l[i_tr]+deltaLow+size)) redzone( false );
			  uint32_t stage   = imax->second.l[i_tr]->curstage;
			  
			  leakSum.total_size=size;
			  leakSum.l.push_back(imax->second.l[i_tr]);
			  leakSum.hash.push_back(stage);
			  
			  // std::cerr << " ROLF : stage " << stage << " " << size << "\n";
			  
			  // stage == 0 means, this leak was reported earlier with another leak with same stacktrace
			  if(stage>0)
			    {
			      b->allocated=(uintptr_t)imax->second.l[i_tr]->allocated;
			      allocSet_tc::iterator it = allocset_tc.find(*b);
			      if (it!=allocset_tc.end())
				{
				  bool same=true;
				  unsigned int i_tr2 = i_tr+1;
				  while ( i_tr2 < imax->second.l.size() )
				    {
				      if ( imax->second.hash[i_tr] == imax->second.hash[i_tr2] )
					{
					  size    = imax->second.l[i_tr2]->allocsize;
					  // redzone *r2 = new ((void*)((uintptr_t)imax->second.l[i_tr2]+deltaLow+size)) redzone( false );
					  stage   = imax->second.l[i_tr2]->curstage;
					  
					  if(stage>0) 
					    {
					      b2->allocated=(uintptr_t)imax->second.l[i_tr2]->allocated;
					      allocSet_tc::iterator it2 = allocset_tc.find(*b2);
					      if (it2!=allocset_tc.end())
						{
						  same=true;
						  for ( unsigned int j = 0; j < std::min(it->allocatedFrom.size(),it2->allocatedFrom.size()); j++)
						    if( same && it->allocatedFrom[j] && it2->allocatedFrom[j] )
						      if( it->allocatedFrom[j] != it2->allocatedFrom[j] )
							same=false;
						  if(same)
						    {
						      // redzone *r2 = new ((void*)((uintptr_t)imax->second.l[i_tr2]+deltaLow+size)) redzone( false );
						      stage   = imax->second.l[i_tr2]->curstage;
						      
						      leakSum.total_size+=size;
						      // how many leaks with same stacktrace ?
						      leakSum.l.push_back(imax->second.l[i_tr2]);
						      leakSum.hash.push_back(stage);
						      
						      // set stage to zero to suppress further printout
						      imax->second.l[i_tr2]->curstage=0;
						    }
						}
					    }
					}
				      ++i_tr2;
				    }
				  
				}
			      thisAlg.push_back(leakSum);
			    }
			}
		      
		      // now printout
		      int nleaks(0);
		      int mleaks(0);
		      uint32_t stage(0);
		      
		      while(thisAlg.size())
			{
			  auto itmax = thisAlg.end();
			  itmax--;
			  // if(thisAlg.size()%100000==0)
			  // std::cerr << " RS " << thisAlg.size() << "\n";

			  if(stmax>=0)
			    {
			      itmax = thisAlg.begin();
			      for(auto iter=thisAlg.begin(); iter!=thisAlg.end(); ++iter)	  
				if(itmax->total_size < iter->total_size)
				  itmax=iter;
			      
			      stage   = itmax->hash[0];
			      std::multimap<int,int> here;
			      
			      std::cerr << "   " << itmax->l.size() << " leak(s) in algorithm " << str << " allocated in "
					<< stageToString(stage) << " with total size of " << itmax->total_size << " bytes";
			      if(itmax->l.size()>1)
				{
				  for(auto i = itmax->l.begin(); i != itmax->l.end(); ++i )
				    here.insert(std::pair<int, int>((*i)->allocsize, 1));
				  
				  std::string s("");
				  auto it = here.end(); --it;
				  std::cerr << " (";
				  while( ! here.empty() )
				    {
				      std::cerr << s;
				      if( here.count(it->first) > 1 )
					std::cerr << here.count(it->first) << "x";
				      std::cerr << it->first;
				      s=",";
				      here.erase(here.lower_bound(it->first),here.end());
				      it = here.end(); 
				      if ( ! here.empty() ) --it;
				    }
				  std::cerr << ")";
				}
			      std::cerr << "\n";
			      
			      // print stacktrace
			      // b->allocated=(uintptr_t)itmax->l[0]->allocated;
			      // allocSet::iterator it = allocset.find(*b);
			      allocSet_tc::iterator it = itmax->l[0];
			      
			      // if (it!=allocset.end())
			      for ( unsigned int j = 1; j < it->allocatedFrom.size() ; j++)
				{
				  if(it->allocatedFrom[j])
				    {
				      fprintf(stderr,"%7u ",j);
				      Dl_info       info;
				      ptrdiff_t relative_address(0);
				      const char *libname = "N/A";
				      if (dladdr (it->allocatedFrom[j], &info) && info.dli_fname && info.dli_fname[0])
					{
					  libname = info.dli_fname;
					  // difference of two pointers
					  uintptr_t p1=(uintptr_t)it->allocatedFrom[j];
					  uintptr_t p2=(uintptr_t)info.dli_fbase;
					  relative_address = (p1 > p2) ? p1 - p2 : p2 - p1;
					  if (strstr (info.dli_fname, ".so") == 0)
					    relative_address = p1;
					}
				      fprintf(stderr," %s D[%p]\n",libname,(void*)relative_address);
				    }
				}
			    }
			  else
			    {
			      nleaks++;
			      mleaks+=itmax->total_size;
			    }
			  
			  --stmax;
			  
			  thisAlg.erase(itmax);
			}
		      if(nleaks>1)
			std::cerr << "   further " << nleaks << " leak(s) in algorithm " << str << " allocated in "
				  << stageToString(stage) << " with total size of " << mleaks << " bytes\n";
		      
		    }
		}
	      
	      leaks.erase(imax);
	    }
	}
      // delete b;
      // delete b2;
      
    }
}

std::string AthMemoryAuditor::stageToString(long s)
{
  std::string str("N/A");
  
  if( s >= 1E6 && s < 9E7 )
    {
      str="Event "; str+=boost::lexical_cast<std::string>(s-1E6);
    }
  if( s == 1 )
    str="Constructorx";
  if( s == 2 )
    str="before initialize";
  if( s == 3 )
    str="initialize";
  if( s == 4 )
    str="re-initialize";
  if( s == 1E6-10 )
    str="begin-run";
  if( s == 9E7 )
    str="end-run";
  if( s == 1E8-2 )
    str="finalize";
  return str;  
}

void AthMemoryAuditor::beforeInitialize(INamedInterface* comp)
{
  // ATH_MSG_INFO("==> beforeInitialize " << comp->name() << " . " << initialized );
  context=(uintptr_t)(&(comp->name()));
  
  stacktraceDepth=m_defaultStacktraceDepth;
  auto fit=arrayAlgIndex.find(comp->name());
  if ( fit == arrayAlgIndex.end() )
    {
      // this allocation would show up as memory leak - suppress later printout by setting stage to zero
      current_stage=0;
      collectStacktraces=false;
      arrayAlgIndex[comp->name()]=aiStruct(curIndex= ++curMaxIndex,m_defaultStacktraceDepth);
    }
  else
    {
      curIndex=fit->second.index;
      stacktraceDepth=fit->second.stacktrace;
    }
  algorithms[curIndex]=comp->name();
  current_stage=3;
}

void AthMemoryAuditor::beforeReinitialize(INamedInterface* comp)
{
  context=(uintptr_t)(&(comp->name()));
  
  stacktraceDepth=m_defaultStacktraceDepth;
  auto fit=arrayAlgIndex.find(comp->name());
  if ( fit == arrayAlgIndex.end() )
    {
      // this allocation would show up as memory leak - suppress later printout by setting stage to zero
      current_stage=0;
      collectStacktraces=false;
      arrayAlgIndex[comp->name()]=aiStruct(curIndex= ++curMaxIndex,m_defaultStacktraceDepth);
    }
  else
    {
      curIndex=fit->second.index;
      stacktraceDepth=fit->second.stacktrace;
    }
  algorithms[curIndex]=comp->name();
  current_stage=4;
  collectStacktraces=bool(stacktraceDepth);
}

void AthMemoryAuditor::beforeExecute(INamedInterface* comp)
{
  stacktraceDepth=m_defaultStacktraceDepth;
  auto fit=arrayAlgIndex.find(comp->name());
  if ( fit == arrayAlgIndex.end() )
    {
      // this allocation would show up as memory leak - suppress later printout by setting stage to zero
      current_stage=0;
      collectStacktraces=false;
      arrayAlgIndex[comp->name()]=aiStruct(curIndex= ++curMaxIndex,m_defaultStacktraceDepth);
    }
  else
    {
      curIndex=fit->second.index;
      stacktraceDepth=fit->second.stacktrace;
    }
  algorithms[curIndex]=comp->name();
  
  if(current_stage<1E6)
    current_stage=1E6;
  
  context=(uintptr_t)(&(comp->name()));
  
  if (context && ( contextFirst == context ) )
    ++current_stage;
  
  if (context && ( contextFirst == 0 ) )
    contextFirst = context;
  collectStacktraces=bool(stacktraceDepth);
}

void AthMemoryAuditor::beforeBeginRun(INamedInterface* comp)
{
  context=(uintptr_t)(&(comp->name()));
  
  stacktraceDepth=m_defaultStacktraceDepth;
  auto fit=arrayAlgIndex.find(comp->name());
  if ( fit == arrayAlgIndex.end() )
    {
      // this allocation would show up as memory leak - suppress later printout by setting stage to zero
      current_stage=0;
      collectStacktraces=false;
      arrayAlgIndex[comp->name()]=aiStruct(curIndex= ++curMaxIndex,m_defaultStacktraceDepth);
    }
  else
    {
      curIndex=fit->second.index;
      stacktraceDepth=fit->second.stacktrace;
    }
  algorithms[curIndex]=comp->name();
  current_stage=1E6-10;
  collectStacktraces=bool(stacktraceDepth);
}

void AthMemoryAuditor::beforeEndRun(INamedInterface* comp)
{
  context=(uintptr_t)(&(comp->name()));
  
  stacktraceDepth=m_defaultStacktraceDepth;
  auto fit=arrayAlgIndex.find(comp->name());
  if ( fit == arrayAlgIndex.end() )
    {
      // this allocation would show up as memory leak - suppress later printout by setting stage to zero
      current_stage=0;
      collectStacktraces=false;
      arrayAlgIndex[comp->name()]=aiStruct(curIndex= ++curMaxIndex,m_defaultStacktraceDepth);
    }
  else
    {
      curIndex=fit->second.index;
      stacktraceDepth=fit->second.stacktrace;
    }
  algorithms[curIndex]=comp->name();
  current_stage=9E7;
  collectStacktraces=bool(stacktraceDepth);
}

void AthMemoryAuditor::beforeFinalize(INamedInterface* comp)
{
  context=(uintptr_t)(&(comp->name()));
  
  stacktraceDepth=m_defaultStacktraceDepth;
  auto fit=arrayAlgIndex.find(comp->name());
  if ( fit == arrayAlgIndex.end() )
    {
      // this allocation would show up as memory leak - suppress later printout by setting stage to zero
      current_stage=0;
      collectStacktraces=false;
      arrayAlgIndex[comp->name()]=aiStruct(curIndex= ++curMaxIndex,m_defaultStacktraceDepth);
    }
  else
    {
      curIndex=fit->second.index;
      stacktraceDepth=fit->second.stacktrace;
    }
  algorithms[curIndex]=comp->name();
  current_stage=1E8-2;
  collectStacktraces=bool(stacktraceDepth);
}

void AthMemoryAuditor::afterInitialize(INamedInterface* /* comp */)
{
  curIndex=1;
  stacktraceDepth=m_defaultStacktraceDepth;
  collectStacktraces=bool(m_defaultStacktraceDepth);
}

void AthMemoryAuditor::afterReinitialize(INamedInterface* /* comp */)
{
  curIndex=1;
  stacktraceDepth=m_defaultStacktraceDepth;
  collectStacktraces=bool(m_defaultStacktraceDepth);
}

void AthMemoryAuditor::afterExecute(INamedInterface* /* comp */, const StatusCode& /* sc */)
{
  curIndex=1;
  stacktraceDepth=m_defaultStacktraceDepth;
  collectStacktraces=bool(m_defaultStacktraceDepth);
}

void AthMemoryAuditor::afterBeginRun(INamedInterface* /* comp */)
{
  curIndex=1;
  stacktraceDepth=m_defaultStacktraceDepth;
  collectStacktraces=bool(m_defaultStacktraceDepth);
}

void AthMemoryAuditor::afterEndRun(INamedInterface* /* comp */)
{
  curIndex=1;
  stacktraceDepth=m_defaultStacktraceDepth;
  collectStacktraces=bool(m_defaultStacktraceDepth);
}

void AthMemoryAuditor::afterFinalize(INamedInterface* /* comp */)
{
  curIndex=1;
  stacktraceDepth=m_defaultStacktraceDepth;
  collectStacktraces=bool(m_defaultStacktraceDepth);
}
