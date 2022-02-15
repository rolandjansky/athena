/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class SemiDetHelper                     //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2011                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "SemiDetHelper.h"
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <libgen.h>//needed on mac for basename
#include <algorithm>
#include <vector>
#include <set>
#include "boost/io/ios_state.hpp"

PMonSD::SemiDetHelper::SemiDetHelper(const std::string& jobStartJiffies)
  : m_cfg_walltime(0),
    m_meas_pre_ini(true),
    m_meas_post_ini(true),
    m_meas_post_1stevt(true),
    m_meas_post_lastevt(true),
    m_meas_post_fin(true),
    m_last_stepidx(INT_MAX),
    m_last_nevts(UINT_MAX),
    m_nevts(0),
    m_need_finalise(true),
    m_vmpeak(-1),
    m_nevts_sum(0),
    m_vmem_sum_evtloop(0),
    m_rss_sum_evtloop(0),
    m_prefix("PMonSD "),
    m_stepprefix("step "),
    m_specialstep("---"),
    m_malloclibname(basename((char*)symb2lib("malloc"))),//const cast to char* is for compilation on mac.
    m_malloc_status("ok"),
    m_malloc_might_get_slow(false),
    m_overhead_cpu_tmp(0),
    m_overhead_cpu_total(0),
    m_overhead_cpu_n(0),
    m_overhead_cpu_snapshot_post_1stevt(-1),
    m_overhead_cpu_snapshot_post_lastevt(-1)
{
  //Magic sequence of malloc calls to tell any spylib (if present,
  //rather harmless otherwise) that now would be a good time to start
  //monitoring:
  {delete[] new char[11];delete[] new char[117];delete[] new char[17];}

  m_tmpstr.reserve(127);
  if (!jobStartJiffies.empty()) {
    //How much time elapsed until now:
    m_cfg_walltime=get_absolute_wall_ms()-jobstart_jiffy2unix_ms(jobStartJiffies);
  }
  m_meas_pre_ini.capture();
  setUTCTimeString(m_starttime,-m_cfg_walltime);
  m_data_std_lastlookup=m_data_std.end();
  m_data_other_lastlookup=m_data_other.end();
  //Small test that our malloc collection actually works on this platform and with this allocator:
  // cppcheck-suppress uninitdata
  double m0=get_malloc_kb(); char *volatile  c = new char[1024]; double m1=get_malloc_kb(); delete[] c;
  if (m0==m1) {
    m_malloc_status="noinfo";
    m_meas.disableMalloc();
    std::cout<<m_prefix<<"WARNING Can't retrieve Malloc information in this setup.";
    if (m_malloclibname!=basename((char*)symb2lib("mallinfo")))
      std::cout<<" This is likely because mallinfo is in a different lib than malloc.";
    std::cout<<std::endl;
  }
#if __GLIBC__<=2 && __GLIBC_MINOR__<=5
  if (!m_meas.mallocDisabled()&&m_malloclibname=="libc.so.6"&&!envvar_is_set("PMONSD_DONTDISABLESLOWMALLOC")) {
    //Get ready to disable calls to mallinfo since they might get really slow.
    //Bug: http://sourceware.org/bugzilla/show_bug.cgi?id=4349
    m_malloc_might_get_slow=true;
  }
#endif

#if DEBUG_SemiDetHelper
  m_debug_nfinds=0;
  m_debug_ninserts=0;
#endif

}

void PMonSD::SemiDetHelper::startAud(const std::string& stepName,const std::string& compName, unsigned val_nevts)
{
#if DEBUG_SemiDetHelper
  m_debug_meas.capture(false/*cpufirst*/);
  m_debug_data.addPointStart(m_debug_meas);
#endif

  //We delay the m_measurement.capture() call as long as possible to minimise inclusion of own overhead

  m_overhead_cpu_tmp = get_cpu_ms();

  int stepidx = CompDataStdSteps::index(stepName);
  if (val_nevts==0&&stepidx==CompDataStdSteps::index_evt)
    stepidx = CompDataStdSteps::index_1st;

  if (stepidx==CompDataStdSteps::index_other) {
    startAudOther(compName,stepName);
  } else {
    m_nevts=std::max(m_nevts,val_nevts);
    if (m_last_stepidx!=stepidx||m_last_nevts!=val_nevts)
      specialAud(stepidx,val_nevts);
    startAudStd(compName,stepidx);
  }

#if DEBUG_SemiDetHelper
  m_debug_meas.capture(true/*cpufirst*/);
  m_debug_data.addPointStop(m_debug_meas);
#endif
}

void PMonSD::SemiDetHelper::stopAud(const std::string& stepName,const std::string& compName, unsigned val_nevts)
{
#if DEBUG_SemiDetHelper
  m_debug_meas.capture(false/*cpufirst*/);
  m_debug_data.addPointStart(m_debug_meas);
#endif

  m_meas.capture(true/*cpufirst*/);//Do this ASAP to minimise inclusion of own overhead
  m_overhead_cpu_tmp=m_meas.last_cpu_raw_ms();

  int stepidx = CompDataStdSteps::index(stepName);
  if (val_nevts==0&&stepidx==CompDataStdSteps::index_evt)
    stepidx = CompDataStdSteps::index_1st;

  if (stepidx==CompDataStdSteps::index_other)
    stopAudOther(compName,stepName);
  else
    stopAudStd(compName,stepidx,val_nevts);

  double dcpu=get_cpu_ms()-m_overhead_cpu_tmp;
  m_meas.incrementOffsets(0,0,dcpu);
  m_overhead_cpu_total+=dcpu; ++m_overhead_cpu_n;

#if DEBUG_SemiDetHelper
  m_debug_meas.capture(true/*cpufirst*/);
  m_debug_data.addPointStop(m_debug_meas);
#endif
}

PMonSD::CompDataStdSteps * PMonSD::SemiDetHelper::findStdData(const std::string& compName, int /*stepidx*/)
{
  //Fixme: for now we don't use the code below. Need to figure out the
  //optimal way to get rid of addresses from call-backs:
  return &(mapLookup(compName,m_data_std,m_data_std_lastlookup));

  //tmp//size_t pos;
  //tmp//if (stepidx!=CompDataStdSteps::index_cbk||(pos=compName.find('['))==0||pos>=compName.size()) 
  //tmp//  return &(mapLookup(compName,m_data_std,m_data_std_lastlookup));
  //tmp////compNames like "TrigConf::DSConfigSvc[0x1e3de4f0]+e7797940" in
  //tmp////callbacks are annoying and irreproducible. Remove the addresses:
  //tmp//m_tmpstr.assign(compName.c_str(),pos);
  //tmp//return &(mapLookup(m_tmpstr,m_data_std,m_data_std_lastlookup));
}

void PMonSD::SemiDetHelper::startAudStd(const std::string& compName, int stepidx)
{
  CompDataStdSteps * d = findStdData(compName,stepidx);
  CompDataBasic * data;
  if (stepidx==CompDataStdSteps::index_evt) {
    m_meas_post_1stevt.captureIfUnused();
    if (m_overhead_cpu_snapshot_post_1stevt<0)
      m_overhead_cpu_snapshot_post_1stevt=m_overhead_cpu_total;
    m_meas.capture(false/*cpufirst*/);//as late as possible to minimise inclusion of own overhead
    data=&(d->data_evt);
  } else {
    if (stepidx==CompDataStdSteps::index_fin) {
      static bool first=true;
      if (first) {
	first=false;
	//Magic sequence of malloc calls+specific string to tell any
	//spylib (if present, rather harmless otherwise) that now
	//would be a good time to produce a report from collected data:
	{ std::string dummy("libstringspy_trigger_report"); }
	{ delete[] new char[13];delete[] new char[117];delete[] new char[17]; }
      }
      m_meas_post_lastevt.captureIfUnused();
      if (m_overhead_cpu_snapshot_post_lastevt<0)
	m_overhead_cpu_snapshot_post_lastevt=m_overhead_cpu_total;
    } else if (stepidx==CompDataStdSteps::index_1st) {
      m_meas_post_ini.captureIfUnused();
    }
    data=&(d->data[stepidx]);
  }
  startAudFinishUp(data);
}
void PMonSD::SemiDetHelper::startAudFinishUp(CompDataBasic*data)
{
  m_meas.capture(false/*cpufirst*/);//as late as possible to minimise inclusion of own overhead
  double dcpu=m_meas.last_cpu_raw_ms()-m_overhead_cpu_tmp;
  m_meas.cpu-=dcpu;
  m_meas.incrementOffsets(0,0,dcpu);//dcpu will be subtracted on next "capture"
  m_overhead_cpu_total+=dcpu;
  ++m_overhead_cpu_n;
  data->addPointStart(m_meas);
}

void PMonSD::SemiDetHelper::stopAudStd(const std::string& compName, int stepidx,unsigned val_nevts)
{
  CompDataStdSteps * d = findStdData(compName,stepidx);
  if (stepidx==CompDataStdSteps::index_evt) {
    d->data_evt.addPointStop(m_meas,val_nevts);
  } else {
    d->data[stepidx].addPointStop(m_meas);
    //Check repeatedly during finalisation for the issue appearing
    //(due to heavy fragmentation occuring):
    if (m_malloc_might_get_slow&&stepidx==CompDataStdSteps::index_fin)
      checkForMallocSlowness(CompDataStdSteps::index_fin);
  }
}

void PMonSD::SemiDetHelper::checkForMallocSlowness(int stepidx)
{
  if (m_overhead_cpu_n>1000&&m_overhead_cpu_total>0.2*m_overhead_cpu_n) {
    //Spending more than 0.2ms/snapshot is unacceptable and is
    //almost certainly due to the mallinfo performance bug in glibc2.5.
    m_meas.disableMalloc();
    if (stepidx==CompDataStdSteps::index_evt) m_malloc_status="autooff_evt";
    else if (stepidx==CompDataStdSteps::index_fin) m_malloc_status="autooff_fin";
    else m_malloc_status="autooff";
    m_malloc_might_get_slow=false;
    std::cout<<m_prefix<<"WARNING Malloc information gathering becoming slow. Disabling."<<std::endl;
  }
}

void PMonSD::SemiDetHelper::specialAud(int stepidx,unsigned val_nevts)
{
  if (m_last_stepidx!=stepidx) {
    //step changed:
    switch (stepidx) {
    case CompDataStdSteps::index_1st:
      m_meas_post_ini.captureIfUnused();
      break;
    case CompDataStdSteps::index_evt:
      m_meas_post_1stevt.captureIfUnused();
      break;
    case CompDataStdSteps::index_fin:
      m_meas_post_1stevt.captureIfUnused();
      m_meas_post_lastevt.captureIfUnused();
      if (m_malloc_might_get_slow)
	checkForMallocSlowness(CompDataStdSteps::index_fin);
      break;
    default: break;
    };
  }
 
  if (stepidx==CompDataStdSteps::index_evt&&m_last_nevts!=val_nevts) {
    double vmem,rss;
    get_vmem_rss_kb(vmem,rss);
    ++m_nevts_sum;//should be m_nevts-1 in normal jobs, but better be safe.
    m_vmem_sum_evtloop+=vmem;
    m_rss_sum_evtloop+=rss;
    if (val_nevts>=10) {
      double malloc(get_malloc_kb());
      if (val_nevts<=100) {
	m_fit_11to100_vmem.addPoint(val_nevts,vmem);
	m_fit_11to100_malloc.addPoint(val_nevts,malloc);
      } else {
	m_fit_101plus_vmem.addPoint(val_nevts,vmem);
	m_fit_101plus_malloc.addPoint(val_nevts,malloc);
      }
    }
    m_last_nevts=val_nevts;//important to only increment in evt steps (since interspersed with cbk steps)
    if (m_malloc_might_get_slow)
      checkForMallocSlowness(CompDataStdSteps::index_evt);
  }
  m_last_stepidx=stepidx;
}

void PMonSD::SemiDetHelper::startAudOther(const std::string& compName, const std::string& stepName)
{
  m_tmp_otherkey.first=compName;
  m_tmp_otherkey.second=stepName;
  CompDataBasic& data=mapLookup(m_tmp_otherkey,m_data_other,m_data_other_lastlookup);
  startAudFinishUp(&data);
}

void PMonSD::SemiDetHelper::stopAudOther(const std::string& compName, const std::string& stepName)
{
  m_tmp_otherkey.first=compName;
  m_tmp_otherkey.second=stepName;
  CompDataBasic& data=mapLookup(m_tmp_otherkey,m_data_other,m_data_other_lastlookup);

  data.addPointStop(m_meas);
  //if (data.nEntries()==2&&data.sortValCPU()<200&&data.sortValMemory()<200&&stepName=="dso") {
  //  //Special: For some reason some dso's are loaded twice (but taking
  //  //essentially zero resources). This is rather confusing to most
  //  //users, so we exclude additional entries count for a given dso *IF* it
  //  //covers innocent resource movements.
  //  data.setEntries(data.nEntries()-1);
  //  static unsigned hackcount=0;
  //  if (++hackcount==50)
  //    std::cout<<m_prefix<<"WARNING: Internal dso reporting"
  //	       <<" hack has gotten out of hand (current dso "<<compName<<")"<<std::endl;
  //}
}

void PMonSD::SemiDetHelper::finalise()
{
  if (!m_need_finalise)
    return;
  m_need_finalise=false;
  m_meas_post_fin.captureIfUnused();
  m_vmpeak=vmpeak();
}

bool PMonSD::SemiDetHelper::reportToFile(const std::string& file,
					 const std::string& info_full_output_inside)
{
  finalise();
  bool ok=true;
  std::ofstream f;
  f.open(file.c_str());
  if (!f.fail()&&f.is_open()) {
    actualReport(f,true,info_full_output_inside);
  } else {
    ok=false;
  }
  f.close();
  if (!ok)
    std::cout<<m_prefix<<"WARNING Could not write output to file: "<<file<<std::endl;
  return ok;
}

void PMonSD::SemiDetHelper::reportToStdout(const std::string& info_full_output_inside,bool showall)
{
  finalise();
  actualReport(std::cout,showall,info_full_output_inside);
}

bool PMonSD::SemiDetHelper::report(const std::string& file,
				   const std::string& info_full_output_inside,
				   bool stdout_uncollapsed)
{
  finalise();
  std::string actualfile=file;
  bool fileok=reportToFile(file,info_full_output_inside);
  if (!fileok)
    actualfile+=" (failed)";
  reportToStdout(info_full_output_inside,stdout_uncollapsed);
  return fileok;
}

void PMonSD::SemiDetHelper::format(std::ostream&os,
				   const std::string& stepName,
				   const std::string& infoName,
				   const Meas&m) const
{
  double vmem(0),malloc(0),cpu(0),wall(0);
  unsigned n(0);
  if (!m.unused()) {
    n=1;
    vmem=m.vmem;
    malloc=m.malloc;
    cpu=m.cpu;
    wall=m.wall;
  }
  bool special=m.hasWallTime();
  int w(special?8:6);
  os<<m_prefix<<"["<<stepName<<"]";
  os<<" ";field_i(os,4,n);
  os<<" ";field_f(os,w,cpu,false);
  if (special) {
    os<<" ";
    field_f(os,w,wall,false);
  }
  os<<" ";field_f(os,w,vmem,false);
  os<<" ";field_f(os,w,malloc,false);
  os<<" "<<infoName;
  os<<"\n";
}

void PMonSD::SemiDetHelper::format(std::ostream&os,
				   const std::string& stepName,
				   const std::string& compName,
				   const CompDataBasic* data,
				   bool normal_as_extended) const
{
  const CompDataExtended* data_e = dynamic_cast<const CompDataExtended*>(data);
  os<<m_prefix<<"["<<stepName<<"]";
  if (data_e) {
    os<<" ";field_i(os,4,data_e->nEntries());
    os<<" ";field_f(os,6,data_e->meanDeltaCPU(),false);
    os<<" ";field_f(os,6,data_e->maxDeltaCPU(),false);
    os<<"@";field_i(os,4,data_e->maxDeltaCPU_iEntry(),true);
    os<<" ";field_f(os,6,data_e->meanDeltaVMEM(),false);
    os<<" ";field_f(os,6,data_e->maxDeltaVMEM(),false);
    os<<"@";field_i(os,4,data_e->maxDeltaVMEM_iEntry(),true);
    os<<" ";field_f(os,6,data_e->meanDeltaMalloc(),false);
    os<<" ";field_f(os,6,data_e->maxDeltaMalloc(),false);
    os<<"@";field_i(os,4,data_e->maxDeltaMalloc_iEntry(),true);
  } else {
    os<<" ";field_i(os,4,data->nEntries());
    os<<" ";field_f(os,6,data->meanDeltaCPU(),false);
    if (normal_as_extended) { os <<"           "; }
    os<<" ";field_f(os,7,data->meanDeltaVMEM(),false);
    if (normal_as_extended) { os <<"           "; }
    os<<" ";field_f(os,7,data->meanDeltaMalloc(),false);
    if (normal_as_extended) { os <<"            "; }
  }
  os <<" "<<compName<<"\n";
}

void PMonSD::SemiDetHelper::format(std::ostream&os,const std::string& stepName, const std::string& infoName,
				   const PerfMon::LinFitSglPass& fit_vmem, const PerfMon::LinFitSglPass& fit_malloc) const
{
  os<<m_prefix<<"["<<stepName<<"]";
  os<<" ";field_i(os,4,fit_vmem.nPoints());
  os<<"        - ";
  os<<"       - ";
  field_f(os,8,fit_vmem.slope(),false);
  os<<" ";field_f(os,8,fit_malloc.slope(),false);
  os<<" "<<infoName;
  os<<"\n";
}

void PMonSD::SemiDetHelper::format(std::ostream&os,const std::string& str, bool center) const
{
  const unsigned width(80);
  unsigned nleft=0;
  if (str.size()<width)
    nleft=(width-str.size())/2;
  unsigned nright=nleft;
  if (!center) {
    nleft=0;
    nright*=2;
  }
  if ((nleft+nright+str.size())<width)
    nright+=1;

  os<<m_prefix<<"===";
  for (unsigned i=0;i<nleft;++i) os<<"=";
  os<<" "<<str<<" ";
  for (unsigned i=0;i<nright;++i) os<<"=";
  os<<"===\n";
}
void PMonSD::SemiDetHelper::colheader(std::ostream&os,StepWrapper*sw) const
{
  //null sw means special section
  if (sw&&dynamic_cast<const CompDataExtended*>(sw->getExampleData())) {
    //extended...
    os<<m_prefix;spaces(os,sw->stepName().size()+2);
    os<<"    n    cpu    max@evt    vmem    max@evt  malloc    max@evt  component\n";
  } else {
    os<<m_prefix;spaces(os,(sw?sw->stepName():m_specialstep).size()+2);
    os<<"    n";
    if (!sw)
      os<<"      cpu     wall     vmem   malloc component";//bigger numbers, bigger spacing
    else
      os<<"    cpu    vmem  malloc component";
    os<<"\n";
  }
}

namespace PMonSD {
  struct Entry {
    static const int show_group_undef = -2;
    static const int show_group_displayed = -1;
    int m_show_group;
  public:
    const std::string* compName;
    const CompDataBasic* data;

    Entry(const std::string*c,const CompDataBasic*d)
      : m_show_group(show_group_undef),compName(c), data(d) {}

    void setCollapsed(unsigned nentries) { m_show_group=int(nentries); }
    void setDisplayed() { m_show_group=show_group_displayed; }
    bool displayed() const { return m_show_group==show_group_displayed; }
    int collapse_nentries() const { assert(m_show_group>=0); return m_show_group; }
  };

  class CmpEntries {
  public:
    static const unsigned specialsort=UINT_MAX;
    CmpEntries(unsigned ival) : m_ival(ival) {}
    bool operator()(const Entry&a,const Entry&b) {
      double va,vb;
      if (m_ival==specialsort) {
	//sort by vmem+malloc followed cpu:
	va=a.data->sortValMemory();
	vb=b.data->sortValMemory();
	if (va!=vb)
	  return va>vb;
	va=a.data->sortValCPU();vb=b.data->sortValCPU();
      } else {
	va=a.data->sortVal(m_ival);vb=b.data->sortVal(m_ival);
      }
      if (va==vb) {
	//if cpu, try to sort by vmem+malloc as well (for the final evt-report):
	if (m_ival==CompDataBasic::ival_cpu) {
	  va=a.data->sortValMemory();
	  vb=b.data->sortValMemory();
	  if (va!=vb)
	    return va>vb;
	}
	//Finally sort by names, just to have a well-defined order.
	return *(a.compName)<*(b.compName);
      }
      return va>vb;
    }
  private:
    unsigned m_ival;
  };

  struct Count { Count(): val(0) {} unsigned val; };//an integer which inits to 0.

}

void PMonSD::SemiDetHelper::processStep(std::ostream&os,StepWrapper*sw,bool showall) const {

  ////////////////////////////////////////
  //Settings for collapsing insignificant entries:

  unsigned collapse_limit=6;//fewer than this won't be collapsed
  double showfraction=1.0;
  if (!showall) {
    if (sw->stepName()=="evt") showfraction=0.99;
    else if (sw->stepName()=="dso") showfraction=0.25;
    else showfraction=0.8;
  }

  const std::string ignore_name("PerfMonSlice");

  ////////////////////////////////////////
  //1) Collect info in more handy structures:
  static const unsigned nvals=CompDataBasic::nvals;
  float totals[nvals];
  float totals_sortvals[nvals];//sum of absolute values
  std::vector<Entry> entries;
  //  => Init:
  entries.reserve(sw->nComps());
  for(unsigned ival=0; ival<nvals; ++ival)
    totals[ival]=totals_sortvals[ival]=0.0;
  //  => Collect
  const std::string* compName;
  const CompDataBasic *data(0);
  sw->iterateReset();
  bool hasentry(false);
  while(sw->getNext(compName,data)) {
    if (*compName==ignore_name) continue;
    if (data->nEntries()==0) continue;//always ignore data with no entries
    hasentry=true;
    entries.push_back(Entry(compName,data));
    for(unsigned ival=0; ival<nvals; ++ival) {
      totals[ival] += data->getVal(ival);//nb: this is not the value mean, but the sum (which is what we want)
      totals_sortvals[ival] += data->sortVal(ival);
    }
  }

  ////////////////////////////////////////
  //2) Find out which entries to show and which to collapse

  if (showall) {
    //Don't collapse any components...
    std::vector<Entry>::iterator it(entries.begin()),itE(entries.end());
    for (;it!=itE;++it)
      it->setDisplayed();
  } else {
    //  => For each type of value, take the first components until
    //  limit is reached and collapse the rest:
    for(unsigned ival=0; ival<nvals; ++ival) {
      double sum(0.0), limit(showfraction*totals_sortvals[ival]);
      std::sort(entries.begin(),entries.end(),CmpEntries(ival));
      std::vector<Entry>::iterator it(entries.begin()),itE(entries.end());
      for (;it!=itE;++it) {
	double v=it->data->sortVal(ival);
	if (sum<limit&&(showall||v>0.5)) {
	  sum+=v;
	  it->setDisplayed();
	} else {
	  //Collapse unless already displayed due to other value:
	  if (!it->displayed())
	    it->setCollapsed(it->data->nEntries());
	}
      }
    }
    {
      //To avoid stupidities like collapsing 1 component, go back and decollapse some:
      // -> count:
      std::map<unsigned,Count> collapse_count;
      std::map<unsigned,Count>::iterator collapse_count_cache(collapse_count.end());
      std::vector<Entry>::iterator it(entries.begin()),itE(entries.end());
      for (it=entries.begin();it!=itE;++it)
	if (!it->displayed())
	  mapLookup(it->data->nEntries(),collapse_count,collapse_count_cache).val += 1;
      // -> decollapse:
      for (it=entries.begin();it!=itE;++it)
	if (!it->displayed()&&collapse_count[it->data->nEntries()].val<collapse_limit)
	  it->setDisplayed();
    }

  }

  ////////////////////////////////////////
  //3) Print
  format(os,m_stepprefix+sw->stepName());
  if (hasentry)
    colheader(os,sw);
  //  -> print displayed entries and collect collapsed/total statistics
  std::sort(entries.begin(),entries.end(),CmpEntries(sw->stepName()=="evt"?0:CmpEntries::specialsort));
  std::vector<Entry>::iterator it(entries.begin()),itE(entries.end());
  std::map<unsigned,CompDataBasic> collapse_map,totals_map;
  std::map<unsigned,CompDataBasic>::iterator it_collapse_cache(collapse_map.end());
  std::map<unsigned,CompDataBasic>::iterator it_totals_cache(totals_map.end());
  for (;it!=itE;++it) {
    unsigned n(it->data->nEntries());
    CompDataBasic& data_total = mapLookup(n,totals_map,it_totals_cache);
    data_total.add(*(it->data));
    if (it->displayed()) {
      format(os,sw->stepName(),*(it->compName),it->data);
    } else {
      //Add to collapse
      CompDataBasic& data_collapse = mapLookup(n,collapse_map,it_collapse_cache);
      data_collapse.add(*(it->data));
    }
  }
  //  -> print collapsed and total statistics:
  std::map<unsigned,CompDataBasic>::iterator it_summary, it_summaryE;
  it_summary=collapse_map.begin();it_summaryE=collapse_map.end();
  for (;it_summary!=it_summaryE;++it_summary) {
    std::stringstream s;
    s<<"[collapsed_"<<it_summary->second.nEntries()<<"_comps]";
    it_summary->second.setEntries(it_summary->first);
    format(os,sw->stepName(),s.str(),&it_summary->second,sw->extended());
  }
  it_summary=totals_map.begin();it_summaryE=totals_map.end();
  for (;it_summary!=it_summaryE;++it_summary) {
    std::stringstream s;
    s<<"[total_for_"<<it_summary->second.nEntries()<<"_comps]";
    it_summary->second.setEntries(it_summary->first);
    format(os,sw->stepName(),s.str(),&it_summary->second,sw->extended());
  }
}

void PMonSD::SemiDetHelper::actualReport(std::ostream&os,bool showall,const std::string& info_full_output_inside)
{
  boost::io::ios_base_all_saver ssave (os);
  finalise();
#if DEBUG_SemiDetHelper
//   m_debug_meas.capture(false/*cpufirst*/);
//   m_debug_data.addPointStart(m_debug_meas);
#endif

  os << std::fixed;//because we don't care about differences <1ms or <1kb.

  format(os,showall?"semi-detailed perfmon info v1.0f / start"  //Remember to update version number if changing!
                   :"semi-detailed perfmon info v1.0c / start");//=> And must ALWAYS be the same as in PMonSD.py
  format(os,"Documentation: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PerfMonSD",false/*center*/);
  format(os,"Note that documentation includes recipe for easy parsing from python.  ",false/*center*/);
  format(os,"Units are milliseconds (cpu/wall) and kilobytes (vmem/malloc).         ",false/*center*/);
  if (!info_full_output_inside.empty()) {
    const unsigned cw(71);
    std::string s;s.reserve(cw);
    s.clear();s+="Full output inside: ";s+=info_full_output_inside;
    while (s.size()<cw) s+=" ";
    format(os,s,false/*center*/);
  }
  //Standard steps - output:
  { StepWrapperStd step_ini("ini",&m_data_std);processStep(os,&step_ini,showall); }
  { StepWrapperStd step_1st("1st",&m_data_std);processStep(os,&step_1st,showall); }
  { StepWrapperStd step_cbk("cbk",&m_data_std);processStep(os,&step_cbk,showall); }
  { StepWrapperStd step_evt("evt",&m_data_std);processStep(os,&step_evt,showall); }
  { StepWrapperStd step_fin("fin",&m_data_std);processStep(os,&step_fin,showall); }

  //Other steps - which do we have?:
  std::set<std::string> othersteps;
  MapOtherSteps::iterator it_other(m_data_other.begin()),it_other_E(m_data_other.end());
  for (;it_other!=it_other_E;++it_other)
    othersteps.insert((*it_other).first.second);

  //Other steps - output:
  std::set<std::string>::const_iterator it_other_stepname(othersteps.begin()),it_other_stepnameE(othersteps.end());
  for (;it_other_stepname!=it_other_stepnameE;++it_other_stepname) {
    StepWrapperOther step(*it_other_stepname,&m_data_other);
    processStep(os,&step,showall);
  }

  //Special steps
  format(os,"special info");
  colheader(os,0);
  //since we don't know what the first value of wall/cpu actually means, subtract it:
  m_meas_post_ini.wall-=m_meas_pre_ini.wall;
  m_meas_post_1stevt.wall-=m_meas_pre_ini.wall;
  m_meas_post_lastevt.wall-=m_meas_pre_ini.wall;
  m_meas_post_fin.wall-=m_meas_pre_ini.wall;
  m_meas_pre_ini.wall=0;
  m_meas_post_ini.cpu-=m_meas_pre_ini.cpu;
  m_meas_post_1stevt.cpu-=m_meas_pre_ini.cpu;
  m_meas_post_lastevt.cpu-=m_meas_pre_ini.cpu;
  m_meas_post_fin.cpu-=m_meas_pre_ini.cpu;
  m_meas_pre_ini.cpu=0;
  //Figure out evtloop and overhead contributions:
  unsigned evtloop_n(0), overhead_n(0);
  double evtloop_cpu(0.),evtloop_wall(0.), overhead_cpu(0.), overhead_wall(0.);
  double internalcpu_evtloop(0),internalcpu_onceperjob(0);
  //NB: if first event is puny and the following are huge, we could in
  //principle end up with negative overhead numbers.
  if (m_nevts>1 && !m_meas_post_lastevt.unused() && !m_meas_post_1stevt.unused()) {
    evtloop_cpu  = (m_meas_post_lastevt.cpu -m_meas_post_1stevt.cpu )/(m_nevts-1);
    evtloop_wall = (m_meas_post_lastevt.wall-m_meas_post_1stevt.wall)/(m_nevts-1);
    evtloop_n = m_nevts;
    internalcpu_evtloop=(m_overhead_cpu_snapshot_post_lastevt-m_overhead_cpu_snapshot_post_1stevt)/(m_nevts-1);
    if (!m_meas_pre_ini.unused()&&!m_meas_post_fin.unused()) {
      overhead_cpu  = m_meas_post_fin.cpu -m_meas_pre_ini.cpu-evtloop_n*evtloop_cpu;
      overhead_wall = m_meas_post_fin.wall-m_meas_pre_ini.wall-evtloop_n*evtloop_wall;
      overhead_n = 1;
      internalcpu_onceperjob=m_overhead_cpu_total-evtloop_n*internalcpu_evtloop;
    }
  }
  //output
  os<<m_prefix<<"[---] ";field_i(os,4,evtloop_n);os<<" ";field_f(os,8,evtloop_cpu);
  os<<" ";field_f(os,8,evtloop_wall);os<<"        -        - evtloop_time\n";
  os<<m_prefix<<"[---] ";field_i(os,4,overhead_n);os<<" ";field_f(os,8,overhead_cpu);
  os<<" ";field_f(os,8,overhead_wall);os<<"        -        - overhead_time\n";
  const std::string s=m_specialstep;
  format(os,s,"snapshot_pre_ini",m_meas_pre_ini);
  format(os,s,"snapshot_post_ini",m_meas_post_ini);
  format(os,s,"snapshot_post_1stevt",m_meas_post_1stevt);
  format(os,s,"snapshot_post_lastevt",m_meas_post_lastevt);
  format(os,s,"snapshot_post_fin",m_meas_post_fin);
  format(os,s,"leakperevt_evt11to100",m_fit_11to100_vmem,m_fit_11to100_malloc);
  format(os,s,"leakperevt_evt101plus",m_fit_101plus_vmem,m_fit_101plus_malloc);
  os<<m_prefix<<"["<<s<<"] vmem_peak="<<int(m_vmpeak+0.5)
    <<" vmem_mean="<<(m_nevts_sum?int(m_vmem_sum_evtloop/m_nevts_sum+.5):0)
    <<" rss_mean="<< (m_nevts_sum?int( m_rss_sum_evtloop/m_nevts_sum+.5):0)<<"\n";
  os<<m_prefix<<"["<<s<<"] jobcfg_walltime="<<int(m_cfg_walltime+0.5)<<" jobstart="<<m_starttime<<"\n";
  os<<m_prefix<<"["<<s<<"] cpu_bmips="<<bogomips()
    <<" cpu_res="<<int(0.5+1000.0*secs_per_jiffy())
    <<" release="<<envvar("AtlasVersion","unknown_release")<<"/"<<envvar("CMTCONFIG","unknown_cmtconfig")<<"\n";
  os<<m_prefix<<"["<<s<<"] cpu_model="<<cpu_model()<<"\n";
  os<<m_prefix<<"["<<s<<"] malloc="<<m_malloclibname<<"/"<<m_malloc_status
    <<" pycintex_vmemfix="<<envvar("ATHENA_PYCINTEX_MINVMEM","0")<<"\n";
  os<<m_prefix<<"["<<s<<"] pmonsd_cost_onceperjob="<<int(0.5+internalcpu_onceperjob)
    <<" pmonsd_cost_perevt="<<int(0.5+internalcpu_evtloop)<<"\n";

#if DEBUG_SemiDetHelper
//   m_debug_meas.capture(true/*cpufirst*/);
//   m_debug_data.addPointStop(m_debug_meas);
  os<<m_prefix<<"Own resource consumption ["<<m_debug_data.nEntries()
    <<" calls]: <cpu> [ms]="<<m_debug_data.meanDeltaCPU()
    <<", total memory usage[kb]~="<<m_debug_data.meanDeltaMalloc()*m_debug_data.nEntries()
    <<", total memory usage vmem [kb]~="<<m_debug_data.meanDeltaVMEM()*m_debug_data.nEntries()
    <<", #map lookups="<<m_debug_nfinds
    <<", #map inserts="<<m_debug_ninserts
    <<"\n";
#endif
  format(os,"semi-detailed perfmon info / end");
  os<<std::flush;
}
