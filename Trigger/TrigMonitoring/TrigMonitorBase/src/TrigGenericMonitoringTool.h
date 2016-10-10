// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMONITORBASE_TRIGGENERICMONITORINGTOOL_H
#define TRIGMONITORBASE_TRIGGENERICMONITORINGTOOL_H

#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "GaudiKernel/ITHistSvc.h"
class TH1;
class TH2;
class TProfile;
class TProfile2D;


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

class TrigGenericMonitoringTool : public TrigMonitorToolBase {
public:
  
  TrigGenericMonitoringTool(const std::string & type, 
			    const std::string & name,
			    const IInterface* parent);
  virtual ~TrigGenericMonitoringTool();
  
  virtual StatusCode bookHists();

  virtual StatusCode fillHists();   //!< does histograms filling
  virtual StatusCode finalHists();  //!< dummy implementation
  
private:
  /**
   * @brief the internal class used to keep parsed Filler properties
   *
   */
  struct HistogramDef {
    std::vector<std::string> name;  //!< names of varaibles
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

  /**
   * @brief base class for fillers 
   */
  class HistogramFiller {
  public:
    HistogramFiller() {}
    virtual ~HistogramFiller() {}
    virtual unsigned fill() = 0;
  };
  
  /**
   * @brief filler for plain 1D hisograms 
   */
  class HistogramFiller1D : public HistogramFiller {
  public: 
    HistogramFiller1D(TH1* hist, IMonitoredAlgo::IGetter* var )
      : m_histogram(hist), m_variable(var) {}
    virtual unsigned fill();
  protected:
    TH1* m_histogram;
    IMonitoredAlgo::IGetter* m_variable;
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
  private:
    TProfile* m_histogram;
    IMonitoredAlgo::IGetter*  m_variable1;
    IMonitoredAlgo::IGetter*  m_variable2;
  };
  


  
  /**
   * @brief filler for plain 2D hisograms 
   */
  class HistogramFiller2D : public HistogramFiller {
  public:
    HistogramFiller2D(TH2* hist, IMonitoredAlgo::IGetter* var1, IMonitoredAlgo::IGetter* var2); 
    virtual unsigned fill();
  protected:
    TH2* m_histogram;
    IMonitoredAlgo::IGetter*  m_variable1;
    IMonitoredAlgo::IGetter*  m_variable2;
  };

  /**
   * @brief filler for profile 2D histogram
   */
  class HistogramFiller2DProfile : public HistogramFiller {
  public:
    HistogramFiller2DProfile(TProfile2D* hist, IMonitoredAlgo::IGetter* var1, IMonitoredAlgo::IGetter* var2, IMonitoredAlgo::IGetter* var3);
    virtual unsigned fill();
  private:
    TProfile2D* m_histogram;
    IMonitoredAlgo::IGetter* m_variable1;
    IMonitoredAlgo::IGetter* m_variable2;
    IMonitoredAlgo::IGetter* m_variable3;
  };  
  
  StatusCode createFiller(const HistogramDef& def); //!< creates filler and adds to the list of active fillers
  //  HistogramDef parseJobOptDefinition(const std::string& conf);
  //std::string generatePath(const std::string& userdef, const std::string& varname ) const;
  
  const IMonitoredAlgo* m_algo;   //!< ptr to the algorithm to which tool is attached (variables coming from)
  std::string m_parentName;       //!< name of parent algo (used in printouts to help debugging)
  ITHistSvc *m_rootHistSvc;   
  
  std::vector<HistogramFiller*> m_fillers;   //!< list of fillers

 
  const HistogramDef parseJobOptHistogram(const std::string& histDef); //!< utility method to parse JO
  std::vector<std::string> m_histograms;                               //!< property (list of histogram definitions)
  std::map<std::string, TrigMonGroup*> histogramCategory;              //!< predefined categories (drive booking paths)
  
  // utility functions
  
  void setOpts(TH1* histo, const std::string& opt);
  void setLabels(TH1* histo, const std::vector<std::string>& labels);
  template<class H> 
  TH1* create1D( TH1*& histo, ITrigLBNHist*& histoLBN,
		    const HistogramDef& def );
  template<class H> 
  TH1* createProfile( TProfile*& histo, ITrigLBNHist*& histoLBN,
		    const HistogramDef& def );
  template<class H> 
  TH1* create2D( TH2*& histo, ITrigLBNHist*& histoLBN,
		    const HistogramDef& def );
  template<class H>
  TH1* create2DProfile( TProfile2D*& histo, ITrigLBNHist*& histoLBN,
			const HistogramDef& def );
};

#endif
