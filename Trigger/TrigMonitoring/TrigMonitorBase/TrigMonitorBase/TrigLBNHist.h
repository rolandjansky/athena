/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigMonitorBase_TrigLBNHist_h
#define TrigMonitorBase_TrigLBNHist_h

#include <map>
#include <vector>
#include <list>
#include <set>
#include <queue>

#include "TH1.h"

class ITHistSvc;

/**
 * The TrigLBNHist is derived from ROOT histogram.
 * The added fnctionality is that this histograms can keep track 
 * of lumi block number changes. 
 * 
 * The idea of subclassing ROOT histograms with the use of templates
 * is by Mike Meddinins who did it for steering monitoring.
 * It was then taken to the base package for trigger wide profit.
 *
 * When this histogram class is used in fact number of histograms end up in the
 * THistSvc:
 *
 *    -# The run summary histogram is created each time. It contains the whole statistics. 
 *     It has postfix _runsummary and title with appended "Run Summary"
 *    -# One histogram per lumi block. (or group of lumi blocks)
 *    -# If history depth is insufficient i.e. some events get stuck in the system and arrive late
 * the extra histogram for them with postfix _lateevents will be created and registered.
 * 
 *
 * @author  Tomasz Bold <Tomasz.Bold@cern.ch>
 **/

class ITrigLBNHist {
public:
  ITrigLBNHist();
  virtual ~ITrigLBNHist();
  void setDepth( unsigned int depth );      //!< sets the history depth
  void setGroup( unsigned int grp );        //!< sets the history grouping
  void setITHistSvc( ITHistSvc* );          //!< sets the histogramming service pointer, it is needed because every new LBN histogram must be somewhere registered  
  void setPath( const std::string& path );  //!< sets the booking path
  virtual void reg() = 0;                   //!< forces initial registration 
  
  /**
   * global LBN changeover
   **/
  static void set_lbn(unsigned int lbn);

  /**
   * global LBN force changeover 
   **/
  static void force_lbn(unsigned int lbn);


protected:
  virtual bool lbn( unsigned int lbn ) = 0;     //!< inform histogram about new LBN
  virtual bool force( unsigned int lbn ) = 0;     //!< inform histogram that it has book new histogram no matter what (i.e. there is change in configuration)

  unsigned int m_depth;
  unsigned int m_group;
  std::string m_path;

  //  unsigned int m_last_forced_lbn;
  unsigned int m_last_lbn;
  unsigned int m_last_force;

  ITHistSvc* m_histSvc;
  //  static std::set<ITrigLBNHist*> s_lbnHistograms; //!< list of histograms to be notified about LBN update
};



/**
 * @brief classes implementing LBN histograms for specific ROOT histogram types
 **/

template<class HTYPE>
class TrigLBNHist : public HTYPE, public ITrigLBNHist {
 public:
  /**
   * @brief the constructor which is cloning plain ROOT histograms
   * The best way to use it is as follows: 
   * new TrigLBNHist<TH1F>(TH1F("name", "title", 10, 0, 10))
   */
  TrigLBNHist(const HTYPE& stencil); 
  virtual ~TrigLBNHist();
  virtual Int_t Fill(Double_t x);
  virtual Int_t Fill(const char* name, Double_t x);
  virtual Int_t Fill(Double_t x, Double_t y);  

  // this is for 2D
  virtual Int_t Fill(Double_t x, Double_t y, Double_t w);
  virtual Int_t Fill(Double_t x, const char* namey, Double_t w);
  virtual Int_t Fill(const char* namex, Double_t y, Double_t w);
  virtual Int_t Fill(const char* namex, const char* namey, Double_t w);
  virtual Int_t Fill(Double_t x, Double_t y, Double_t z, Double_t w);

  void print();

  virtual void reg(); //!< needs to be called (done in TrigMonGroup to have summary histogram registred)
  virtual bool lbn( unsigned int lbn );
  virtual bool force( unsigned int lbn );

private:
  HTYPE *fetchHist();

  bool fetchLateEventsHist();
  bool addNewHist(unsigned lbn, bool force=false);
  void releaseOldHist(unsigned lbn);
  std::string m_name;   
  std::string m_title;



  // internal structures to keep associations between histograms and LBNs
  // this is tag (one tag is one or few LBNs)
  
  struct lbn_tag {
    lbn_tag(unsigned int b, unsigned int e) : begin_lbn(b), end_lbn(e) {}
    bool operator == (unsigned int lbn ) const {
	if ( begin_lbn <= lbn && lbn < end_lbn )
	  return true;
      return false;
      //std::cout  << "comparying lbn: " << lbn << " to the tag: " << begin_lbn << " " << end_lbn << std::endl;
    }

    //    bool operator == (const lbn_tag& rhs) const {
    //      return begin_lbn == rhs.begin_lbn && end_lbn == rhs.end_lbn;
    //    }
    void terminate(unsigned new_end) { end_lbn = new_end; }
    unsigned int begin_lbn;
    unsigned int end_lbn;
  };


  // histogram decorated by the LBN tag, list is sortable by the tags precedence
  struct tagged_histogram {
    tagged_histogram (const lbn_tag& t, HTYPE* h): tag(t), histogram(h) {}
    lbn_tag tag;
    HTYPE* histogram;
    bool operator< ( const tagged_histogram& t ) const {return tag.begin_lbn < t.tag.begin_lbn; }
  };

  struct matching_lbn_tag{
    matching_lbn_tag(unsigned l) : m_lbn(l) {}    
    bool operator()(const lbn_tag& tag) const { return tag == m_lbn; }

    bool operator()(const tagged_histogram& hist) const { return hist.tag == m_lbn; }
    unsigned m_lbn; 
  };



  void append(std::list<tagged_histogram>& hists, const tagged_histogram& h);
  std::list<tagged_histogram> m_theHists;

  tagged_histogram m_lateEvents; //!< histogram for late events
  std::list<lbn_tag> m_deregistered; //!< list of tags which were already dropped (if entry found here then lateEvetns shoudl be filled)

  
  tagged_histogram m_current;    //!< histogram for currently processed LBN
};


#endif
