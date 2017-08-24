/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

#ifndef FEATURE_H
#define FEATURE_H

#include <iostream>
#include <cmath>
#include <string>
#include <bitset>
#include <vector>
#include <map>
#include <queue>

namespace Trig {

  struct TrigBtagEmulationJet {
    double pt;
    double eta;
    double phi;
    std::map< std::string, double > weights;
  };

  bool sortEmulationJet_byPt(Trig::TrigBtagEmulationJet&,Trig::TrigBtagEmulationJet&);

  // ***

  class feature {
  public:
    feature(int type = feature::UNKNOWN,std::string name = "NONE");
    feature(const feature&);
    ~feature();

    virtual bool isPassed() = 0;
    virtual bool evaluateJet(struct TrigBtagEmulationJet*) = 0;
    virtual void clear() = 0;
    virtual void Print() = 0;

  public:
    int _type;
    std::string _name;

    static const int UNKNOWN    = -1;
    static const int THRESHOLD  =  0;
    static const int SELECTION  =  1;
  };

  // *** B-TAG

  class feature_btag : public feature {
  public:
    feature_btag(std::string name = "NONE",float weight = -1000);
    feature_btag(const feature_btag&);
    ~feature_btag();

    virtual bool evaluateJet(struct TrigBtagEmulationJet*);
    virtual void Print();

    bool isPassed();
    void clear();

  public:
    float _weight;
  };

  // *** ANTI-B-TAG

  class feature_antibtag : public feature_btag {
  public:
    feature_antibtag(std::string name = "NONE",float weight = 1000);
    feature_antibtag(const feature_antibtag&);
    ~feature_antibtag();

    bool evaluateJet(struct TrigBtagEmulationJet*);
    void Print();
  };


  // *** HT
 
  class feature_ht : public feature {
  public:
    feature_ht(std::string triggerLevel = "L1",std::string name = "NONE", float ht = 0);
    feature_ht(const feature_ht&);
    ~feature_ht();

    void setCuts(float,float,float);
    bool satisfyCuts(struct TrigBtagEmulationJet*);

    bool isPassed();
    bool evaluateJet(struct TrigBtagEmulationJet*);
    virtual void clear();
    virtual void Print();

  protected:
    bool isPassed_L1();
    bool isPassed_HLT();

    virtual bool evaluateJet_L1(struct TrigBtagEmulationJet*);
    bool evaluateJet_HLT(struct TrigBtagEmulationJet*);

  public:
    std::string _trigLevel;
    float _min_ht;
    float _count_ht;

  protected:
    float _cut_pt;
    float _cut_min_eta;
    float _cut_max_eta;
    float _cut_btag;
    std::string _btagName;
  };

  // *** HT-TOP

  class feature_ht_top : public feature_ht {
  public:
    feature_ht_top(std::string triggerLevel = "L1",std::string name = "NONE", float ht = 0, unsigned int topEt = 0);
    feature_ht_top(const feature_ht_top&);
    ~feature_ht_top();
    
    void clear();
    void Print();

  protected:
    bool evaluateJet_L1(struct TrigBtagEmulationJet*);

  private:
    void calculateHT_L1();

  private:
    std::vector<struct TrigBtagEmulationJet> _piorityQueue;
    unsigned int _topEt;
  };

  // *** INVM
  class feature_invm : public feature {
  public:
    feature_invm(std::string triggerLevel = "L1",std::string name = "NONE", float min_invm = 0);
    feature_invm(const feature_invm&);
    ~feature_invm();

    bool isPassed();
    bool evaluateJet(struct TrigBtagEmulationJet*);
    void clear();
    virtual void Print();

    void setCuts(float,float,float);
    bool satisfyCuts(struct TrigBtagEmulationJet*);

  protected:
    void initLUTs();
    double cosh_LUT(double value) const;
    double cos_LUT(double value) const;

  protected:
    virtual bool evaluateJet_L1(struct TrigBtagEmulationJet*);  
    bool evaluateJet_HLT(struct TrigBtagEmulationJet*);  

  protected:
    double calculateINVM(struct TrigBtagEmulationJet const&,struct TrigBtagEmulationJet const&) const;

  protected:
    std::string _trigLevel;
    double _min_invm;
    double _count_invm;
    double _cut_pt;
    double _cut_min_eta;
    double _cut_max_eta;

  protected:
    std::vector<struct TrigBtagEmulationJet> _jetCollection_HLT;
    std::vector<struct TrigBtagEmulationJet> _priorityQueue_20;
    std::vector<struct TrigBtagEmulationJet> _priorityQueue_30;

  protected:
    std::map< int,double > _LUTcos;
    std::map< int,double > _LUTcosh;
  };

  // *** INVM CF
  class feature_invm_CF : public feature_invm {
  public:
    feature_invm_CF(std::string triggerLevel = "L1",std::string name = "NONE", float min_invm = 0);
    feature_invm_CF(const feature_invm_CF&);
    ~feature_invm_CF();

    void Print();

  private:
    bool evaluateJet_L1(struct TrigBtagEmulationJet*);    
  };


} //namespace

#endif
