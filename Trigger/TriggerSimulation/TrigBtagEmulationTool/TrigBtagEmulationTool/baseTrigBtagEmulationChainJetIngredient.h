/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef baseTrigBtagEmulationChainJetIngredient_H
#define baseTrigBtagEmulationChainJetIngredient_H

#include "TrigBtagEmulationTool/feature.h"

namespace Trig {

  // ==========================================================================
  // *** Virtual Base Class
  // ==========================================================================

  class baseTrigBtagEmulationChainJetIngredient {
  public:
    baseTrigBtagEmulationChainJetIngredient(std::string triggerName = "");
    baseTrigBtagEmulationChainJetIngredient(const baseTrigBtagEmulationChainJetIngredient&);
    virtual ~baseTrigBtagEmulationChainJetIngredient();

    std::string getName() const;

    bool evaluate();
    virtual void Print() = 0;
    virtual bool isPassed(); 
    virtual bool hasFeature(std::string) const; 
    virtual void clear(); 

    virtual bool needsJet(std::string);
    bool addJet(std::string,std::vector< struct TrigBtagEmulationJet >&);

    virtual void initialize() = 0;

  protected:
    virtual bool evaluateJet(struct TrigBtagEmulationJet&); 
    void addFeature(std::string,feature*); 

    virtual void setPT(std::string&)   = 0;
    virtual void setEta(std::string&)  = 0;
    virtual feature* setHT(std::string&)   = 0;
    virtual feature* setINVM(std::string&) = 0;  

    template<typename T> void extract(std::string,std::string,T &valA);
    template<typename T,typename U> void extract(std::string,std::string,T &valA,U &valB);
       
  public:
    std::string m_triggerName;
    double m_min_pt;
    double m_min_eta;
    double m_max_eta;
    int m_min_mult;
    int m_count;

    std::vector< std::pair<std::string,feature*> > m_type_THRESHOLD_features;
    std::vector< std::pair<std::string,feature*> > m_type_SELECTION_features;

  public:
    std::map< std::string,std::string > m_neededJetCollection;
    std::map< std::string,std::vector< struct TrigBtagEmulationJet >* > m_jetCollection;
  };

  // ==========================================================================
  // *** L1
  // ==========================================================================

  class TrigBtagEmulationChainJetIngredient_L1 : public baseTrigBtagEmulationChainJetIngredient {
  public:
    TrigBtagEmulationChainJetIngredient_L1(std::string triggerName = "");
    TrigBtagEmulationChainJetIngredient_L1(const TrigBtagEmulationChainJetIngredient_L1&);
    virtual ~TrigBtagEmulationChainJetIngredient_L1();

    virtual void Print();
    virtual void initialize();

  protected:
    virtual void setPT(std::string&);
    virtual void setEta(std::string&);
    virtual feature* setHT(std::string&);
    virtual feature* setINVM(std::string&);
  };  

  // ==========================================================================
  // *** L1 JJ
  // ==========================================================================

  class TrigBtagEmulationChainJetIngredient_L1_JJ : public TrigBtagEmulationChainJetIngredient_L1 {
  public:
    TrigBtagEmulationChainJetIngredient_L1_JJ(std::string triggerName = "");
    TrigBtagEmulationChainJetIngredient_L1_JJ(const TrigBtagEmulationChainJetIngredient_L1_JJ&);
    virtual ~TrigBtagEmulationChainJetIngredient_L1_JJ();

    virtual void Print();
  };

  // ==========================================================================
  // *** HLT
  // ==========================================================================

  class TrigBtagEmulationChainJetIngredient_HLT : public baseTrigBtagEmulationChainJetIngredient {
  public:
    TrigBtagEmulationChainJetIngredient_HLT(std::string triggerName = "");
    TrigBtagEmulationChainJetIngredient_HLT(const TrigBtagEmulationChainJetIngredient_HLT&);
    virtual ~TrigBtagEmulationChainJetIngredient_HLT();

    virtual void Print();
    virtual void initialize();

    virtual bool overlaps(const TrigBtagEmulationChainJetIngredient_HLT*) const;
    virtual bool contains(const TrigBtagEmulationChainJetIngredient_HLT*) const;
    bool isContained(const TrigBtagEmulationChainJetIngredient_HLT*) const;
    virtual void addJetsMulteplicity(const TrigBtagEmulationChainJetIngredient_HLT*); 

  protected:
    virtual void setPT(std::string&);
    virtual void setEta(std::string&);
    virtual feature* setHT(std::string&);
    virtual feature* setINVM(std::string&);
    virtual void setBTAG(std::string&);
  };

  // ==========================================================================
  // *** GSC
  // ==========================================================================

  class TrigBtagEmulationChainJetIngredient_GSC : public TrigBtagEmulationChainJetIngredient_HLT {
  public:
    TrigBtagEmulationChainJetIngredient_GSC(std::string triggerName = "");
    TrigBtagEmulationChainJetIngredient_GSC(const TrigBtagEmulationChainJetIngredient_GSC&);
    virtual ~TrigBtagEmulationChainJetIngredient_GSC();

    virtual void Print();
    virtual bool isPassed();
    virtual bool hasFeature(std::string) const;
    virtual void clear();
    virtual bool needsJet(std::string);
    virtual void initialize();
    virtual bool evaluateJet(struct TrigBtagEmulationJet&);

    virtual void addJetsMulteplicity(const TrigBtagEmulationChainJetIngredient_HLT*);

  protected:
    virtual void setPT(std::string&);

  private:
    unsigned int matchingJets(struct TrigBtagEmulationJet&);

  public:
    double m_min_gsc;
    int m_min_mult_gsc;
    int m_count_gsc;
  };
  
}//namespace

#endif
