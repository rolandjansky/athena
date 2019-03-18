/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_FEATURE_H
#define TRIGGER_FEATURE_H

#include "TrigBtagEmulationJet.h"

#include <iostream>
#include <cmath>
#include <string>
#include <bitset>
#include <vector>
#include <map>
#include <tuple>
#include <queue>

#include "AsgTools/MsgStream.h"
#include "AsgTools/MsgStreamMacros.h"

namespace Trig {

  class TriggerFeature {
  public:
    TriggerFeature(MsgStream&,int type = TriggerFeature::UNKNOWN,std::string name = "NONE");
    TriggerFeature(const TriggerFeature&);
    virtual ~TriggerFeature();

    const std::string& name() const;
    virtual bool isPassed() const = 0;
    virtual bool evaluateJet(const TrigBtagEmulationJet&) = 0;
    virtual void clear() = 0;
    virtual void print() const = 0;
    virtual void setCuts(float,float,float) {}

    virtual float getCut() const = 0;

    virtual std::unique_ptr< TriggerFeature > uniqueClone() const = 0;

  protected:
    MsgStream& msg() const;
    MsgStream& msg( const MSG::Level lvl ) const;
    bool msgLvl ( const MSG::Level lvl ) const;

  protected:
    enum JetElement {PT,ETA,PHI};

  public:
    int m_type;
    std::string m_name;

    static const int UNKNOWN    = -1;
    static const int THRESHOLD  =  0;
    static const int SELECTION  =  1;

  protected:
    MsgStream &m_msg;
  };

  // *** B-TAG

  class TriggerFeatureBtag : public TriggerFeature {
  public:
    TriggerFeatureBtag(MsgStream&,std::string name = "NONE",float weight = -1000);
    TriggerFeatureBtag(const TriggerFeatureBtag&);
    virtual ~TriggerFeatureBtag();

    virtual bool isPassed() const;
    virtual bool evaluateJet(const TrigBtagEmulationJet&);
    virtual void clear();
    virtual void print() const;

    virtual float getCut() const;

    virtual std::unique_ptr< TriggerFeature > uniqueClone() const;

  public:
    double m_weight;

  public:
    static constexpr double BCOMBLOOSE  = 0.25 ;
    static constexpr double BCOMBMEDIUM = 1.25 ;
    static constexpr double BCOMBTIGHT  = 2.65 ;
    static constexpr double BLOOSE      = 0.25 ;
    static constexpr double BMEDIUM     = 1.25 ;
    static constexpr double BTIGHT      = 2.65 ;

    static constexpr double MV2C2040    = 0.75 ;
    static constexpr double MV2C2050    = 0.50 ;
    static constexpr double MV2C2060    = -0.0224729 ;
    static constexpr double MV2C2070    = -0.509032  ;
    static constexpr double MV2C2077    = -0.764668  ;
    static constexpr double MV2C2085    = -0.938441  ;

    // mv2c10 cuts: https://indico.cern.ch/event/642743/contributions/2612294/attachments/1468496/2271441/BJetTrigTuning_TGM31May2017.pdf
    static constexpr double MV2C1040    = 0.978 ;
    static constexpr double MV2C1050    = 0.948 ;
    static constexpr double MV2C1060    = 0.846 ;
    static constexpr double MV2C1070    = 0.58  ;
    static constexpr double MV2C1077    = 0.162 ;
    static constexpr double MV2C1085    = -0.494;

    static constexpr double MV2C0040    = 0.973 ;
    static constexpr double MV2C0050    = 0.939 ;
    static constexpr double MV2C0060    = 0.835 ;
    static constexpr double MV2C0070    = 0.588 ;
    static constexpr double MV2C0077    = 0.192 ;
    static constexpr double MV2C0085    = -0.402;
  };

  // *** ANTI-B-TAG

  class TriggerFeatureAntiBtag : public TriggerFeatureBtag {
  public:
    TriggerFeatureAntiBtag(MsgStream&,std::string name = "NONE",float weight = 1000);
    TriggerFeatureAntiBtag(const TriggerFeatureAntiBtag&);
    virtual ~TriggerFeatureAntiBtag();

    virtual bool evaluateJet(const TrigBtagEmulationJet&);
    virtual void print() const;
  };


  // *** HT
 
  class TriggerFeatureHt : public TriggerFeature {
  public:
    TriggerFeatureHt(MsgStream&,std::string triggerLevel = "L1",std::string name = "NONE", float ht = 0);
    TriggerFeatureHt(const TriggerFeatureHt&);
    virtual ~TriggerFeatureHt();

    virtual bool isPassed() const;
    virtual bool evaluateJet(const TrigBtagEmulationJet&);
    virtual void clear();
    virtual void print() const;
    virtual void setCuts(float,float,float);
    virtual float getCut() const;

    bool satisfyCuts(const double,const double) const;

    virtual std::unique_ptr< TriggerFeature > uniqueClone() const;

  protected:
    bool isPassed_L1() const;
    bool isPassed_HLT() const;

    virtual bool evaluateJet_L1(const TrigBtagEmulationJet&);
    bool evaluateJet_HLT(const TrigBtagEmulationJet&);

  public:
    std::string m_trigLevel;
    float m_min_ht;
    float m_count_ht;

  protected:
    float m_cut_pt;
    float m_cut_min_eta;
    float m_cut_max_eta;
    float m_cut_btag;
    std::string m_btagName;
  };

  // *** HT-TOP

  class TriggerFeatureHtTop : public TriggerFeatureHt {
  public:
    TriggerFeatureHtTop(MsgStream&,std::string triggerLevel = "L1",std::string name = "NONE", float ht = 0, unsigned int topEt = 0);
    TriggerFeatureHtTop(const TriggerFeatureHtTop&);
    virtual ~TriggerFeatureHtTop();
    
    virtual void clear();
    virtual void print() const;

    virtual std::unique_ptr< TriggerFeature > uniqueClone() const;

  protected:
    virtual bool evaluateJet_L1(const TrigBtagEmulationJet&);

  private:
    void calculateHT_L1();

  private:
    std::vector< std::tuple<double,double,double> > m_piorityQueue;
    unsigned int m_topEt;
  };

  // *** INVM
  class TriggerFeatureInvm : public TriggerFeature {
  public:
    TriggerFeatureInvm(MsgStream&,std::string triggerLevel = "L1",std::string name = "NONE", float min_invm = 0,double minPtList = 0);
    TriggerFeatureInvm(const TriggerFeatureInvm&);
    virtual ~TriggerFeatureInvm();

    virtual bool isPassed() const;
    virtual bool evaluateJet(const TrigBtagEmulationJet&);
    virtual void clear();
    virtual void print() const;
    virtual void setCuts(float,float,float);
    virtual float getCut() const;

    bool satisfyCuts(const double,const double) const;

    virtual std::unique_ptr< TriggerFeature > uniqueClone() const;

  protected:
    void initLUTs();
    double cosh_LUT(double value) const;
    double cos_LUT(double value) const;

  protected:
    virtual bool evaluateJet_L1(const TrigBtagEmulationJet&);  
    bool evaluateJet_HLT(const TrigBtagEmulationJet&);  

  protected:
    double calculateINVM(const std::tuple<double,double,double>&,const std::tuple<double,double,double>&) const;

  protected:
    std::string m_trigLevel;
    double m_min_invm;
    double m_count_invm;
    double m_cut_pt;
    double m_cut_min_eta;
    double m_cut_max_eta;
    double m_minPtList;

  protected:
    std::vector< std::tuple<double,double,double> > m_jetCollection_HLT;
    std::vector< std::tuple<double,double,double> > m_priorityQueue_20;
    std::vector< std::tuple<double,double,double> > m_priorityQueue_30;

  protected:
    std::map< int,double > m_LUTcos;
    std::map< int,double > m_LUTcosh;
  };

  // *** INVM CF
  class TriggerFeatureInvmCF : public TriggerFeatureInvm {
  public:
    TriggerFeatureInvmCF(MsgStream&,std::string triggerLevel = "L1",std::string name = "NONE", float min_invm = 0);
    TriggerFeatureInvmCF(const TriggerFeatureInvmCF&);
    virtual ~TriggerFeatureInvmCF();

    virtual void print() const;

  protected:
    virtual bool evaluateJet_L1(const TrigBtagEmulationJet&);    
  };

  bool sortJetTupleByPt( const std::tuple<double,double,double>&,const std::tuple<double,double,double>& );

  // *** INVM NFF
  class TriggerFeatureInvmNFF : public TriggerFeatureInvm {
  public:
    TriggerFeatureInvmNFF(MsgStream&,std::string triggerLevel = "L1",std::string name = "NONE", float min_invm = 0);
    TriggerFeatureInvmNFF(const TriggerFeatureInvmNFF&);
    ~TriggerFeatureInvmNFF();

    virtual void print() const;

  protected:
    virtual bool evaluateJet_L1(const TrigBtagEmulationJet&);
  };
  
} //namespace

#endif
