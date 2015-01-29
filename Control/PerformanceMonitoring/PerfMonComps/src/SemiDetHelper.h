/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SemiDetHelper                       //
//                                                            //
//  Description: Main helper class for inclusion of           //
//               "semi-detailed" monitoring in PerfMon.       //
//               "Semi" because it unlike detailed mode of    //
//               PerfMon, doesn't store per-component and     //
//               per-evt info, but only per-component.        //
//               It also monitors fewer variables than        //
//               PerfMon: cpu+vmem+malloc + a few pieces of   //
//               specialised information for convenience.     //
//                                                            //
//               It doesn't know anything about the framework //
//               it sits in, but requires PerfMon to supply   //
//               calls to startAud/stopAud as appropriate.    //
//                                                            //
//               In the end, data is printed to stdout and to //
//               text-files. A python module, pmonsd.py, is   //
//               provided for easy parsing into a python      //
//               dictionary. It is the intention to never     //
//               break the ability to parse older versions    //
//               of the output with this module.              //
//                                                            //
//  Full documentation:                                       //
//  https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PerfMonSD  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2011                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SEMIDETHELPER_H
#define SEMIDETHELPER_H

#include "SemiDetMisc.h"
#include <string>
#include <iomanip>
#include <ostream>
#include "LinFitSglPass.h"
#include "boost/io/ios_state.hpp"

namespace PMonSD {
  class SemiDetHelper {
  public:

    //Only constructor/destructor + methods needed in PerfMon:
    SemiDetHelper(const std::string& jobStartJiffies="");
    //jobStartJiffies: supply the starttime since boot in jiffies
    //(from /proc/PID/stat) if you want to know how much time elapsed
    //before this point (cfg_time)

    ~SemiDetHelper(){}

    //Call from perfmons start/stop auditor hooks (val_nevts is solely to distinguish 1st event from other events)
    void startAud(const std::string& stepName,const std::string& compName,
		  unsigned val_nevts);//should be last in perfmons startaud method
    void stopAud(const std::string& stepName,const std::string& compName,
		 unsigned val_nevts);//should be first in perfmons stopaud method
    bool report(const std::string& file,
		const std::string& info_full_output_inside,
		bool stdout_uncollapsed);


  private:

    SemiDetHelper( const SemiDetHelper & );
    SemiDetHelper & operator= ( const SemiDetHelper & );

    void reportToStdout(const std::string& info_full_output_inside,bool showall);
    bool reportToFile(const std::string& file,
		      const std::string& info_full_output_inside);

    //Actual start/stop aud methods:
    void startAudStd(const std::string& compName, int stepidx);
    void stopAudStd(const std::string& compName, int stepidx,unsigned val_nevts);
    void startAudOther(const std::string& compName, const std::string& stepName);
    void stopAudOther(const std::string& compName, const std::string& stepName);
    void startAudFinishUp(CompDataBasic*);
    CompDataStdSteps * findStdData(const std::string& compName, int stepidx);
    //For capturing cpu/mem measurements:
    Meas m_meas;//per-component monitoring (with dbl-counting and timing overhead exclusion)
    double m_cfg_walltime;
    Meas m_meas_pre_ini;
    Meas m_meas_post_ini;
    Meas m_meas_post_1stevt;
    Meas m_meas_post_lastevt;
    Meas m_meas_post_fin;
    std::string m_starttime;
    PerfMon::LinFitSglPass m_fit_11to100_vmem;
    PerfMon::LinFitSglPass m_fit_11to100_malloc;
    PerfMon::LinFitSglPass m_fit_101plus_vmem;
    PerfMon::LinFitSglPass m_fit_101plus_malloc;
    int m_last_stepidx;
    unsigned m_last_nevts;
    unsigned m_nevts;
    bool m_need_finalise;
    double m_vmpeak;
    unsigned m_nevts_sum;//should be m_nevts-1 in normal jobs, but better be safe.
    double m_vmem_sum_evtloop;
    double m_rss_sum_evtloop;

    //Data structures for standard steps (evt,1st,ini,fin,cbk)
    MapStdSteps m_data_std;
    MapStdSteps::iterator m_data_std_lastlookup;
    //Data structures for other steps (dso, ...)
    OtherKey m_tmp_otherkey;
    MapOtherSteps m_data_other;
    MapOtherSteps::iterator m_data_other_lastlookup;

//Set to 1 to benchmark ourselves:
#define DEBUG_SemiDetHelper 0
#if DEBUG_SemiDetHelper
    Meas m_debug_meas;
    CompDataBasic m_debug_data;
    mutable unsigned m_debug_nfinds;
    mutable unsigned m_debug_ninserts;
#endif

    void specialAud(int stepidx,unsigned val_nevts);
    void finalise();

    //Helper method for map lookups with automatic growth in case of nonexisting key
    template<class Tmap>
    typename Tmap::mapped_type& mapLookup(const typename Tmap::key_type& key,Tmap&m, typename Tmap::iterator& cached_iterator) const
    {
      //lookup key, and grow automatically if needed.
      if (cached_iterator==m.end()||(*cached_iterator).first!=key) {
	cached_iterator=m.find(key);
#if DEBUG_SemiDetHelper
	++m_debug_nfinds;
#endif
	if (cached_iterator==m.end()) {
	  std::pair<typename Tmap::iterator,bool> r
	    =m.insert(std::pair<typename Tmap::key_type,typename Tmap::mapped_type>(key,typename Tmap::mapped_type()));
	  assert(r.second);
	  cached_iterator=r.first;
#if DEBUG_SemiDetHelper
	  ++m_debug_ninserts;
#endif
	}
      }
      return (*cached_iterator).second;
    }

    //For the report:
    const std::string m_prefix;
    const std::string m_stepprefix;
    const std::string m_specialstep;
    const std::string m_malloclibname;
    std::string m_malloc_status;

    void actualReport(std::ostream&,bool showall,const std::string& info_full_output_inside);
    void format(std::ostream&os,const std::string& stepName,const std::string& compName,
		const CompDataBasic*,bool normal_as_extended=false) const;
    void format(std::ostream&,const std::string& str, bool center=true) const;
    void format(std::ostream&,const std::string& stepName, const std::string& infoName, const Meas&) const;
    void format(std::ostream&,const std::string& stepName, const std::string& infoName,
		const PerfMon::LinFitSglPass& fit_vmem, const PerfMon::LinFitSglPass& fit_malloc) const;
    void processStep(std::ostream&os,StepWrapper*,bool showall) const;
    void field_i(std::ostream&os,unsigned width,int64_t val,bool leftjustify=false) const
    {
      //NB: Coverity complains about us modifying the left/right state
      //of "os". But in ::actualReport we are actually restoring the
      //state, so it is a false positive.
      //Just save the state anyway.
      boost::io::ios_base_all_saver ssave (os);
      os << (leftjustify?std::left:std::right) << std::setw(width) << val;
    }
    void field_f(std::ostream&os,unsigned width,double val,bool leftjustify=false) const
    {
      //NB: Coverity complains about us modifying the left/right state
      //of "os". But in ::actualReport we are actually restoring the
      //state, so it is a false positive.
      //always rounded to integers (milliseconds and kilobytes are good enough precision)
      field_i(os,width,int64_t(val+0.5),leftjustify);
    }
    void spaces(std::ostream&os,unsigned n) const { os << std::setw(n) << ""; }
    void colheader(std::ostream&os,StepWrapper*sw) const;

    void checkForMallocSlowness(int stepidx);
    bool m_malloc_might_get_slow;

    double m_overhead_cpu_tmp;
    double m_overhead_cpu_total;
    unsigned m_overhead_cpu_n;
    double m_overhead_cpu_snapshot_post_1stevt;
    double m_overhead_cpu_snapshot_post_lastevt;
    std::string m_tmpstr;//One temporary string to reuse
  };

}
#endif
