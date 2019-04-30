/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __SUSYCROSSSECTION__
#define __SUSYCROSSSECTION__

// Member variables
#include <cstdlib>
#include <string>
#include <map>

// For PMG centralized tool's wrapper tool handles 
#include "AsgTools/AnaToolHandle.h"

// Lots of function inlining! 
#include "PMGAnalysisInterfaces/IPMGCrossSectionTool.h"

namespace PMGTools {
  class IPMGCrossSectionTool;
}

namespace SUSY
{

unsigned int finalState(const int SUSY_Spart1_pdgId, const int SUSY_Spart2_pdgId);

class CrossSectionDB
{
public:
  CrossSectionDB(const std::string& txtfilename = "dev/PMGTools/PMGxsecDB_mc16.txt", bool usePathResolver = true, bool isExtended = false, bool usePMGTool = true);

  // Load all the information from a file
  void loadFile(const std::string&);
  // Extend the record based on information from a second file
  void extend(const std::string&);

  // class to defined a MC process with id (or run #), name and cross-section
  class Process
  {
  public:
    Process() :
      m_id(-1), m_name(""), m_cross_section(-1.f), m_kfactor(-1.f), m_efficiency(-1.f), m_relunc(-1.f),  m_sumweight(-1.f), m_stat(-1.f) {}

    Process(int id, const std::string& name, float cross_section, float kfactor, float efficiency, float relunc, float sumweight, float stat):
      m_id(id), m_name(name), m_cross_section(cross_section), m_kfactor(kfactor), m_efficiency(efficiency), m_relunc(relunc), m_sumweight(sumweight), m_stat(stat) {}

    int ID() const { return m_id;}
    const std::string& name() const { return m_name;}
    float xsect() const { return m_cross_section;}
    float kfactor() const { return m_kfactor;}
    float efficiency() const { return m_efficiency;}
    float relunc() const { return m_relunc;}
    float sumweight() const { return m_sumweight;}
    float stat() const { return m_stat;}
    void  sumweight(float s) { m_sumweight=s; }
    void  stat(float s) { m_stat=s; }
  private:
    int m_id;
    std::string m_name;
    float m_cross_section;
    float m_kfactor;
    float m_efficiency;
    float m_relunc;
    float m_sumweight;
    float m_stat;
  };

  class Key {
    int sample_id;
    int proc_id;
  public:
    Key(): sample_id(0), proc_id(0) {}
    Key(int _sample_id, int _proc_id): sample_id(_sample_id), proc_id(_proc_id) {}
    Key(int _sample_id, std::string name): sample_id(_sample_id) {
      proc_id = atoi(name.c_str());
    }
    bool operator<(const Key & k) const {
      return this->sample_id < k.sample_id || (this->sample_id == k.sample_id && this->proc_id < k.proc_id);
    }
  };


  // set extended mode
  void setExtended(bool isExtended=true){ m_extended = isExtended; };
  void setUsePMGTool(bool usePMGTool=true){ m_usePMGTool = usePMGTool; };


  // invalid ID returns a process with ID < 0
  Process process(int id, int proc = 0) const;
  Process process(int id, int pdgId1, int pdgId2) const { return process(id, finalState(pdgId1, pdgId2)); }

  // invalid process give < 0 result
  float xsectTimesEff(int id, int proc = 0) const {
    Process p = process(id, proc);
    return p.xsect() * p.kfactor() * p.efficiency();
  }
  float xsectTimesEff(int id, int pdgId1, int pdgId2) const { return xsectTimesEff(id, finalState(pdgId1, pdgId2)); }
  float rawxsect(int id, int proc = 0) const { return process(id, proc).xsect(); }
  float rawxsect(int id, int pdgId1, int pdgId2) const { return rawxsect(id, finalState(pdgId1, pdgId2)); }
  float kfactor(int id, int proc = 0) const { return process(id, proc).kfactor(); }
  float kfactor(int id, int pdgId1, int pdgId2) const { return kfactor(id, finalState(pdgId1, pdgId2)); }
  float efficiency(int id, int proc = 0) const { return process(id, proc).efficiency(); }
  float efficiency(int id, int pdgId1, int pdgId2) const { return efficiency(id, finalState(pdgId1, pdgId2)); }
  float rel_uncertainty(int id, int proc = 0) const { return process(id, proc).relunc(); }
  float rel_uncertainty(int id, int pdgId1, int pdgId2) const { return rel_uncertainty(id, finalState(pdgId1, pdgId2)); }
  float sumweight(int id, int proc = 0) const { return process(id, proc).sumweight(); }
  float sumweight(int id, int pdgId1, int pdgId2) const { return sumweight(id, finalState(pdgId1, pdgId2)); }

  // invalid ID return "" string
  std::string name(int id) const { return process(id, 0).name(); }

  // Allow iteration over all samples, denying modification of private data
private:
  typedef std::map<Key, Process> xsDB_t; // Internal data format
  bool m_extended;
  bool m_usePMGTool;
public:
  typedef xsDB_t::const_iterator iterator; // External iterator format
  iterator begin() const { return m_xsectDB.begin(); }
  iterator end() const {return m_xsectDB.end(); }

private:
  //PMG tool
  asg::AnaToolHandle<PMGTools::IPMGCrossSectionTool> m_pmgxs;

  xsDB_t::iterator my_find( const int proc );
  xsDB_t m_xsectDB;
  xsDB_t m_cache;
};

}

#endif
