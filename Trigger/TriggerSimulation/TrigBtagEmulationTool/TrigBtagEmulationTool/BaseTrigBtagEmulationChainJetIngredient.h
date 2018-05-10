/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BaseTrigBtagEmulationChainJetIngredient_H
#define BaseTrigBtagEmulationChainJetIngredient_H

#include "TrigBtagEmulationTool/TriggerFeature.h"

namespace Trig {

  // ==========================================================================
  // *** Virtual Base Class
  // ==========================================================================

  class BaseTrigBtagEmulationChainJetIngredient {
  public:
    BaseTrigBtagEmulationChainJetIngredient(MsgStream&,std::string triggerName = "",std::string jetCollection = "");
    BaseTrigBtagEmulationChainJetIngredient(const BaseTrigBtagEmulationChainJetIngredient&);
    virtual ~BaseTrigBtagEmulationChainJetIngredient();

    std::string getName() const;
    double getPtThreshold() const;
    double getMinEtaThreshold() const;
    double getMaxEtaThreshold() const;
    virtual int getMulteplicityThreshold() const;
    int getMulteplicity() const;

    virtual void initialize() = 0;

    bool evaluate();
    virtual void print() const = 0;
    virtual bool isPassed() const; 
    virtual bool hasFeature(const std::string&) const; 
    virtual void clear(); 

    virtual bool needsJet(const std::string&) const;

  protected:
    virtual bool evaluateJet(const TrigBtagEmulationJet&); 
    void addFeature(std::string,std::unique_ptr< TriggerFeature >&);

    virtual void setPT(const std::string&)   = 0;
    virtual void setEta(const std::string&)  = 0;
    virtual std::unique_ptr< TriggerFeature > setHT(const std::string&) = 0;
    virtual std::unique_ptr< TriggerFeature > setINVM(const std::string&) = 0;  

    template<typename T> void extract(const std::string&,const std::string&,T &valA);
    template<typename T,typename U> void extract(const std::string&,const std::string&,T &valA,U &valB);

    void printFeatures() const;

    MsgStream& msg() const;
    MsgStream& msg( const MSG::Level lvl ) const;

  protected:
    std::string m_triggerName;
    double m_min_pt;
    double m_min_eta;
    double m_max_eta;
    int m_min_mult;
    int m_count;

    std::map< std::string,std::unique_ptr< TriggerFeature > > m_type_THRESHOLD_features;
    std::map< std::string,std::unique_ptr< TriggerFeature > > m_type_SELECTION_features;

    std::string m_neededJetCollection;
    
    MsgStream &m_msg;

  public:
    static bool addJet(std::string,std::vector< std::unique_ptr< TrigBtagEmulationJet > >);
    static void clearJet();

  protected:
    static std::map< std::string,std::vector< std::unique_ptr< TrigBtagEmulationJet > > > m_jetCollection;

  };

  // ==========================================================================
  // *** L1
  // ==========================================================================

  class TrigBtagEmulationChainJetIngredient_L1 : public BaseTrigBtagEmulationChainJetIngredient {
  public:
    TrigBtagEmulationChainJetIngredient_L1(MsgStream&,std::string triggerName = "");
    TrigBtagEmulationChainJetIngredient_L1(const TrigBtagEmulationChainJetIngredient_L1&);
    virtual ~TrigBtagEmulationChainJetIngredient_L1();

    virtual void print() const;
    virtual void initialize();

  protected:
    virtual void setPT(const std::string&);
    virtual void setEta(const std::string&);
    virtual std::unique_ptr< TriggerFeature > setHT(const std::string&);
    virtual std::unique_ptr< TriggerFeature > setINVM(const std::string&);
  };  

  // ==========================================================================
  // *** L1 JJ
  // ==========================================================================

  class TrigBtagEmulationChainJetIngredient_L1_JJ : public TrigBtagEmulationChainJetIngredient_L1 {
  public:
    TrigBtagEmulationChainJetIngredient_L1_JJ(MsgStream&,std::string triggerName = "");
    TrigBtagEmulationChainJetIngredient_L1_JJ(const TrigBtagEmulationChainJetIngredient_L1_JJ&);
    virtual ~TrigBtagEmulationChainJetIngredient_L1_JJ();

    virtual void print() const;
  };

  // ==========================================================================
  // *** HLT
  // ==========================================================================

  class TrigBtagEmulationChainJetIngredient_HLT : public BaseTrigBtagEmulationChainJetIngredient {
  public:
    TrigBtagEmulationChainJetIngredient_HLT(MsgStream&,std::string triggerName = "");
    TrigBtagEmulationChainJetIngredient_HLT(const TrigBtagEmulationChainJetIngredient_HLT&);
    virtual ~TrigBtagEmulationChainJetIngredient_HLT();

    virtual void print() const;
    virtual void initialize();

    virtual bool overlaps(const TrigBtagEmulationChainJetIngredient_HLT&) const;
    virtual bool contains(const TrigBtagEmulationChainJetIngredient_HLT&) const;
    bool isContained(const TrigBtagEmulationChainJetIngredient_HLT&) const;
    virtual void addJetsMulteplicity(const TrigBtagEmulationChainJetIngredient_HLT&); 

  protected:
    virtual void setPT(const std::string&);
    virtual void setEta(const std::string&);
    virtual std::unique_ptr< TriggerFeature > setHT(const std::string&);
    virtual std::unique_ptr< TriggerFeature > setINVM(const std::string&);
    virtual void setBTAG(const std::string&);
  };

  // ==========================================================================
  // *** GSC
  // ==========================================================================

  class TrigBtagEmulationChainJetIngredient_GSC : public TrigBtagEmulationChainJetIngredient_HLT {
  public:
    TrigBtagEmulationChainJetIngredient_GSC(MsgStream&,std::string triggerName = "");
    TrigBtagEmulationChainJetIngredient_GSC(const TrigBtagEmulationChainJetIngredient_GSC&);
    virtual ~TrigBtagEmulationChainJetIngredient_GSC();

    virtual void print() const;
    virtual bool isPassed() const;
    virtual bool hasFeature(const std::string&) const;
    virtual void clear();
    virtual bool needsJet(const std::string&) const;
    virtual void initialize();
    virtual bool evaluateJet(const TrigBtagEmulationJet&);

    virtual void addJetsMulteplicity(const TrigBtagEmulationChainJetIngredient_HLT&);

    double getGscThreshold() const;
    virtual int getMulteplicityThreshold() const;
    int getMulteplicityGsc() const;

  protected:
    virtual void setPT(const std::string&);

  private:
    unsigned int matchingJets(const TrigBtagEmulationJet&);

  protected:
    double m_min_gsc;
    int m_min_mult_gsc;
    int m_count_gsc;

    std::string m_supportingJetCollection;
  };
  
}//namespace

#endif
