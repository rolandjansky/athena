// -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMONITORBASE_TRIGGENERICMONITORINGTOOL_H
#define TRIGMONITORBASE_TRIGGENERICMONITORINGTOOL_H

#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "GaudiKernel/EventContext.h"

#include <mutex>

class TH1;
class TH2;
class TProfile;
class TProfile2D;


/**
 * a BasicLockable noop for the non-MT use-case
 */
class NoMutex {   
public:
  void lock() {}
  void unlock() {}
};

/**
 * Context specific Getter
 *
 * Inspired by GaudiKernel/ContextSpecificPtr.h
 * Will return a copy of the default Getter (the one created during bookHist)
 * for each new Context
 */
template<class T>
class ContextGetter {
public:

  ContextGetter() = default;
  explicit ContextGetter(T* ptr) { set(ptr); }

  /// Return the pointer for the current context
  inline T* get() const {
    std::lock_guard<std::mutex> lock(m_ptrs_lock);
    
    auto itr = m_ptrs.find(Gaudi::Hive::currentContextId());
    if (itr!=m_ptrs.end()) 
      return itr->second;
   
    // If new context, return a clone of the object in the 
    // invalid slot (object before threading started)
    return (m_ptrs[Gaudi::Hive::currentContextId()] = 
            new T(*m_ptrs[(EventContext::ContextID_t)EventContext::INVALID_CONTEXT_ID]));
  }
  /// Set the pointer for the current context.
  inline T*& set(T* ptr) {
    std::lock_guard<std::mutex> lock(m_ptrs_lock);
    return m_ptrs[Gaudi::Hive::currentContextId()] = ptr;
  }
  
  /// Assignment operator (@see set).
  inline T*& operator= (T* ptr) { return set(ptr); }

  /// @{ Dereference operators.
  inline T& operator* () { return *get(); }
  inline const T& operator* () const { return *get(); }
  inline T* operator-> () { return get(); }
  inline const T* operator-> () const { return get(); }
  /// @}

  
private:
  mutable std::unordered_map<Gaudi::Hive::ContextIdType, T*> m_ptrs;  //!< ptr per context
  mutable std::mutex m_ptrs_lock;
};


/**
 * @brief Basic monitoring tool for the HLT trigger.
 *
 * That tool works as follows. It is supposed to be attached to HLT algorithms (put to the list of AthenaMonTools of the algorithm.) 
 * During initialization it reads JO and also the list of monitoring variables exported by the algorithm (via HLT::IMoniotredAlgo interface).
 * Then for each declared histogram simple helper classes are created which span hisogram defined and the variable to be monitorred. (they are called HistogramFiller1D, HistogramFiller2D, ...).
 * The varaibles which are not mentioned in the configuration, this for which histograms are not defined, are just silently ignored.
 * Other way around is not the case. If there is request to create some histogram for variable not beeing exported by the algorithms the WARNING message is printed.
 *
 * During execution (in fillHists() method) the loop over all histogram filles is executed and internally varaibles are picked up and used to fill histograms.
 *
 * The JO is an list of histogram definitions of the following form:
 * PATH, HTYPE, NAME, TITLE, BINNING ...., OPTIONS

 * Example configuration strings:
 * ["/SHIFT, TH1D, name, title, xbins, xmin, xmax, opt"]
 * ["/EXPERT, TH2D, "name1,name2", title, xbins, xmin, xmax, ybins, ymin, ymax, opt"]
 * In last case the decision to create and fill 2D histograms happens at configuration time.
 * Name specifier can get additional parameter which is an alias. This alias will be used for naming actual histogram.
 * ex. name;alias for TH1* , name1,name2;alias for TH2*
 *
 *
 *
 * Following hisogram types are supported: TH1(F|I|D), TH2(F|I|D). 
 * Following paths are supported: EXPERT, SHIFT, DEBUG, RUNSTAT
 * Following options are suppored: kCanRebin, kCummulative
 * -# kCanRebin enables ROOT (http://root.cern.ch) internal functionality of autobinning of the histogram.
 * -# kCumulative does fill of all bins left to the bin into which the value falls, sort of distribuant
 * -# kLBN does make the histogram LBN aware (means many histograms)
 * -# kVec adds the content of the monitored varaible to the bins as if monitored varaible would be another histogram
 * -# kVecUO adds the content of the monitored varaible to the bins as if monitored varaible would be another histogram treating oth and last elements as under/overflows
 *
 * @author Tomasz Bold <Tomasz.Bold@cern.ch>
 * @author Wang Meng
 */

template<class M, class P>     // Mutex and Proxy type
class TrigGenericMonitoringTool : public TrigMonitorToolBase {
public:
  
  TrigGenericMonitoringTool(const std::string & type, 
                            const std::string & name,
                            const IInterface* parent);
  virtual ~TrigGenericMonitoringTool();
  
  virtual StatusCode bookHists();
  virtual StatusCode fillHists();   //!< does histograms filling
  virtual StatusCode finalHists();  //!< dummy implementation
  virtual void setProxy(const std::string& name, IMonitoredAlgo::IGetter* g);
  
private:
  /**
   * @brief the internal class used to keep parsed Filler properties
   *
   */
  struct HistogramDef {
    std::vector<std::string> name;  //!< names of variables
    std::string alias;              //!< histogram name alias
    std::string type;               //!< histogram type
    std::string path;               //!< booking path
    std::string title;              //!< title of the histogram
    std::string opt;                //!< options
    
    int xbins{0};  //!< number of bins in X
    float xmin{0}; //!< left
    float xmax{0}; //!< right
    
    int ybins{0};  //!< number of bins in Y
    float ymin{0}; //!< bottom
    float ymax{0}; //!< top

    float zmin{0}; //!< in
    float zmax{0}; //!< out    

    bool  ok{false};	//!<  good declaration
    bool  ycut{false};	//!<  TProfile with cut on y
    bool  zcut{false};  //!<  TProfile2D with cut on z

    std::vector<std::string> labels; //!< bins labels
  };

  static void switchGetter(IMonitoredAlgo::IGetter* g,
                           IMonitoredAlgo::IGetter*& old) {
    if (old && g->name()==old->name()) {
      delete old;
      old = g;
    }
  }

  static void switchGetter(IMonitoredAlgo::IGetter* g,
                           ContextGetter<IMonitoredAlgo::IGetter>& old) {
    if (g->name()==old->name()) {
      delete old.get();
      old = g;
    }
  }
        

  /**
   * @brief base class for fillers 
   */
  class HistogramFiller {
  public:
    HistogramFiller() {}
    virtual ~HistogramFiller() {}
    virtual unsigned fill() = 0;
    virtual void updateGetter(IMonitoredAlgo::IGetter* g) = 0;
  protected:
    M m_mutex;
  };
  
  /**
   * @brief filler for plain 1D hisograms 
   */
  class HistogramFiller1D : public HistogramFiller {
  public: 
    HistogramFiller1D(TH1* hist, IMonitoredAlgo::IGetter* var )
      : m_histogram(hist), m_variable(var) {}
    virtual unsigned fill();
    virtual void updateGetter(IMonitoredAlgo::IGetter* g) {
      switchGetter(g, m_variable);
    }
  protected:
    TH1* m_histogram{0};
    P m_variable;
  };
  
  
  /**
   * @brief filler for 1D hisograms filled in cummulative mode
   */
  class CumulativeHistogramFiller1D : public HistogramFiller1D {
  public:
    CumulativeHistogramFiller1D(TH1* hist, IMonitoredAlgo::IGetter* var) 
      : HistogramFiller1D(hist, var) {}
    virtual unsigned fill();
  };
  

  class VecHistogramFiller1D : public HistogramFiller1D {
  public:
    VecHistogramFiller1D(TH1* hist, IMonitoredAlgo::IGetter* var) 
      : HistogramFiller1D(hist, var) {}
    virtual unsigned fill();
  };
  
  class VecHistogramFiller1DWithOverflows : public HistogramFiller1D {
  public:
    VecHistogramFiller1DWithOverflows(TH1* hist, IMonitoredAlgo::IGetter* var) 
      : HistogramFiller1D(hist, var) {}
    virtual unsigned fill();
  };

  

  /**
   * @brief filler for profile 1d  histogram 
   */
  class HistogramFillerProfile : public HistogramFiller {
  public:
    HistogramFillerProfile(TProfile* hist, IMonitoredAlgo::IGetter* var1, IMonitoredAlgo::IGetter* var2 );
    virtual unsigned fill();
    virtual void updateGetter(IMonitoredAlgo::IGetter* g) {
      switchGetter(g, m_variable1);
      switchGetter(g, m_variable2);
    }
      
  private:
    TProfile* m_histogram{0};
    P  m_variable1{0};
    P  m_variable2{0};
  };
  


  
  /**
   * @brief filler for plain 2D hisograms 
   */
  class HistogramFiller2D : public HistogramFiller {
  public:
    HistogramFiller2D(TH2* hist, IMonitoredAlgo::IGetter* var1, IMonitoredAlgo::IGetter* var2); 
    virtual unsigned fill();
    virtual void updateGetter(IMonitoredAlgo::IGetter* g) {
      switchGetter(g, m_variable1);
      switchGetter(g, m_variable2);
    }

  protected:
    TH2* m_histogram;
    P  m_variable1{0};
    P  m_variable2{0};
  };

  /**
   * @brief filler for profile 2D histogram
   */
  class HistogramFiller2DProfile : public HistogramFiller {
  public:
    HistogramFiller2DProfile(TProfile2D* hist, IMonitoredAlgo::IGetter* var1, IMonitoredAlgo::IGetter* var2, IMonitoredAlgo::IGetter* var3);
    virtual unsigned fill();
    virtual void updateGetter(IMonitoredAlgo::IGetter* g) {
      switchGetter(g, m_variable1);
      switchGetter(g, m_variable2);
      switchGetter(g, m_variable3);
    }

  private:
    TProfile2D* m_histogram{0};
    P m_variable1{0};
    P m_variable2{0};
    P m_variable3{0};
  };
  
  StatusCode createFiller(const HistogramDef& def); //!< creates filler and adds to the list of active fillers
  
  const IMonitoredAlgo* m_algo{0};   //!< ptr to the algorithm to which tool is attached (variables coming from)
  std::string m_parentName;          //!< name of parent algo (used in printouts to help debugging)
  ITHistSvc *m_rootHistSvc{0};   
  
  std::vector<HistogramFiller*> m_fillers;   //!< list of fillers

 
  const HistogramDef parseJobOptHistogram(const std::string& histDef); //!< utility method to parse JO
  std::vector<std::string> m_histograms;                               //!< property (list of histogram definitions)
  std::map<std::string, TrigMonGroup*> m_histogramCategory;            //!< predefined categories (drive booking paths)
  
  // utility functions
  
  void setOpts(TH1* histo, const std::string& opt);
  void setLabels(TH1* histo, const std::vector<std::string>& labels);

  template<class H, class HBASE, typename... Types> 
  HBASE* create( const HistogramDef& def, Types&&... hargs );
  template<class H> 
  TH1* create1D( TH1*& histo, const HistogramDef& def );
  template<class H> 
  TH1* createProfile( TProfile*& histo, const HistogramDef& def );
  template<class H>
  TH1* create2D( TH2*& histo, const HistogramDef& def );
  template<class H>
  TH1* create2DProfile( TProfile2D*& histo, const HistogramDef& def );
};

using TrigGenericMonitoringToolST = TrigGenericMonitoringTool<NoMutex, IMonitoredAlgo::IGetter*>;
using TrigGenericMonitoringToolMT = TrigGenericMonitoringTool<std::mutex, ContextGetter<IMonitoredAlgo::IGetter>>;

#endif
