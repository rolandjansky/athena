// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_GenericMonitoringTool_h
#define AthenaMonitoring_GenericMonitoringTool_h

#include <functional>
#include <vector>
#include <mutex>
#include <memory>

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "GaudiKernel/ITHistSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaMonitoring/IMonitoredVariable.h"

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

class GenericMonitoringTool : public AthAlgTool {
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

    bool  ok{false};    //!<  good declaration
    bool  ycut{false};  //!<  TProfile with cut on y
    bool  zcut{false};  //!<  TProfile2D with cut on z

    std::vector<std::string> labels; //!< bins labels
  };
public:
  /**
   * @brief base class for fillers 
   */
  class HistogramFiller {
  public:
    friend GenericMonitoringTool;
    HistogramFiller(const HistogramFiller& hf) 
      : m_hist(hf.m_hist), m_mutex(hf.m_mutex), m_histDef(hf.m_histDef) {}
    HistogramFiller(HistogramFiller&&) = default;

    virtual ~HistogramFiller() {}
    virtual unsigned fill() = 0;

    void setMonitoredVariables(std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> monitoredVariables) {
      m_monVariables = monitoredVariables;
    }

    std::vector<std::string> histogramVariablesNames() {
      return m_histDef->name;
    }
  protected:
    HistogramFiller(TH1* hist, HistogramDef histDef) 
      : m_hist(hist), m_mutex(std::make_shared<std::mutex>()), m_histDef(new HistogramDef(histDef)) {}

    virtual TH1* histogram() = 0;

    TH1* m_hist;
    std::shared_ptr<std::mutex> m_mutex;
    std::shared_ptr<HistogramDef> m_histDef;
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> m_monVariables;
  private:
    HistogramFiller& operator=(HistogramFiller const&) = delete;
  };

	static const InterfaceID& interfaceID();

  GenericMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent);
  virtual ~GenericMonitoringTool();
  
  virtual StatusCode initialize();
  virtual std::vector<HistogramFiller*> getHistogramsFillers(std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> monitoredVariables);
  
  enum Level { debug, expert, shift, express, runsum, runstat = runsum };
  std::string level2string( Level l );
  class MonGroup {
  public:
    MonGroup(GenericMonitoringTool* tool, const std::string& algo, Level l );
    StatusCode regHist(TH1* h);
    template <class T>
    T* getHist(const std::string& hname) const {
      T* h(0);
      const std::string name = m_tool->level2string(m_level)+m_algo+"/"+hname;
      if (m_tool->m_histSvc->exists(name)) {
        m_tool->m_histSvc->getHist(name, h).ignore();
      }
      return h;
    }
    StatusCode deregHist(TH1* h);
  private:
    GenericMonitoringTool* m_tool;
    std::string m_algo;
    Level m_level;
  };

private: 
  /**
   * @brief filler for plain 1D hisograms 
   */
  class HistogramFiller1D : public HistogramFiller {
  public: 
    HistogramFiller1D(TH1* hist, HistogramDef histDef)
      : HistogramFiller(hist, histDef) {}
    virtual unsigned fill();
  protected:
    virtual TH1* histogram() { return m_hist; }
  };
  
  /**
   * @brief filler for 1D hisograms filled in cummulative mode
   */
  class CumulativeHistogramFiller1D : public HistogramFiller1D {
  public:
    CumulativeHistogramFiller1D(TH1* hist, HistogramDef histDef) 
      : HistogramFiller1D(hist, histDef) {}
    virtual unsigned fill();
  };

  class VecHistogramFiller1D : public HistogramFiller1D {
  public:
    VecHistogramFiller1D(TH1* hist, HistogramDef histDef) 
      : HistogramFiller1D(hist, histDef) {}
    virtual unsigned fill();
  };
  
  class VecHistogramFiller1DWithOverflows : public HistogramFiller1D {
  public:
    VecHistogramFiller1DWithOverflows(TH1* hist, HistogramDef histDef) 
      : HistogramFiller1D(hist, histDef) {}
    virtual unsigned fill();
  };

  /**
   * @brief filler for profile 1d histogram 
   */
  class HistogramFillerProfile : public HistogramFiller {
  public:
    HistogramFillerProfile(TProfile* hist, HistogramDef histDef)
      : HistogramFiller(hist, histDef) {};
    virtual unsigned fill();
  protected:
    virtual TProfile* histogram() { return static_cast<TProfile*>(m_hist); }
  };
  
  /**
   * @brief filler for plain 2D hisograms 
   */
  class HistogramFiller2D : public HistogramFiller {
  public:
    HistogramFiller2D(TH2* hist, HistogramDef histDef)
      : HistogramFiller(hist, histDef) {};
    virtual unsigned fill();
  protected:
    virtual TH2* histogram() { return static_cast<TH2*>(m_hist); }
  };

  /**
   * @brief filler for profile 2D histogram
   */
  class HistogramFiller2DProfile : public HistogramFiller {
  public:
    HistogramFiller2DProfile(TProfile2D* hist, HistogramDef histDef)
      : HistogramFiller(hist, histDef) {};
    virtual unsigned fill();
  protected:
    virtual TProfile2D* histogram() { return static_cast<TProfile2D*>(m_hist); }
  };  
  
  StatusCode createFiller(const HistogramDef& def); //!< creates filler and adds to the list of active fillers
  const HistogramDef parseJobOptHistogram(const std::string& histDef); //!< utility method to parse JO  
  // utility functions
  
  void setOpts(TH1* hist, const std::string& opt);
  void setLabels(TH1* hist, const std::vector<std::string>& labels);

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
  
  ServiceHandle<ITHistSvc> m_histSvc;  
  std::vector<HistogramFiller*> m_fillers;                         //!< list of fillers
  std::vector<std::string> m_histograms;                           //!< property (list of histogram definitions)
  std::map<std::string, MonGroup*> m_histogramCategory;            //!< predefined categories (drive booking paths)
};

#endif /* AthenaMonitoring_GenericMonitoringTool_h */