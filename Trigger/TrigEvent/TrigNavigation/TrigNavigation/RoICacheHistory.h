// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_ROICACHEHISTORY_H
#define TRIGNAVIGATION_ROICACHEHISTORY_H

#include <vector>
#include <string>

#include "CLIDSvc/CLASS_DEF.h"
#include "TrigNavigation/TriggerElement.h"

namespace HLT {

  class NavigationCore;

  class RoICacheHistory {
  public:
    class FeatureCall {
    public:
      FeatureCall(const TriggerElement* work,
		  const TriggerElement* te, CLID clid, const std::string& label, 
		  bool issingle, bool isget);
      FeatureCall();
      
      void addAnswer( const TriggerElement* te, const TriggerElement::FeatureAccessHelper&);

      const TriggerElement::FeatureVec& getAnswers() const { return m_answers; }


      const TriggerElement* getInitTE() const { return m_init; }
      const TriggerElement* getWorkTE() const { return m_work; }

      bool isValid() const;
      void reset();
      const std::string& label() const { return m_label; }
      CLID clid() const {return m_clid; }
      bool issingle() const {return m_issingle; }
      bool isget() const {return m_isget; }

      
    private:

      const TriggerElement* m_init; // place where the first query starts from
      const TriggerElement* m_work; // current TE on which algorithm is working
      CLID m_clid;
      std::string m_label;
      TriggerElement::FeatureVec m_answers;
      bool m_issingle;
      bool m_isget;
    };
    

    class QuestionScope {
    public:
      QuestionScope(const TriggerElement *te, CLID clid, const std::string& label, NavigationCore *navig, bool issingle=false);  
      QuestionScope();
      ~QuestionScope() noexcept (false);
    private:      
      const TriggerElement *m_start;
    };

    class RememberAttachFeature {
    public:
      RememberAttachFeature(const TriggerElement* te, CLID clid, const std::string& label,
			    NavigationCore* navig, TriggerElement::ObjectIndex& idx);
    };



    static RoICacheHistory& instance();


    void startHistory(const TriggerElement* te) { m_collect = true; m_questions.clear(); m_work = te; }
    void stopHistory() { m_collect = false; m_questions.clear(); }

    bool collectHistory() const { return m_collect; }

    void setCurrentFeatureCall(const FeatureCall &curr, NavigationCore *navig);
    const FeatureCall& getCurrentFeatureCall() const { return m_current; }

    void addAnswer(const TriggerElement* te, const TriggerElement::FeatureAccessHelper&);


    void stopCurrentFeatureCall();

    NavigationCore* getNavigation() const { return m_navigation; }
    const TriggerElement* getWorkTE() const { return m_work; }
    const std::vector<FeatureCall>& getFeatureCalls() const { return m_questions; }       

  private:
    
    RoICacheHistory()
    : m_collect(false), m_navigation(0), m_work(0) {}
    ~RoICacheHistory(){}

    RoICacheHistory(const RoICacheHistory&);
    RoICacheHistory& operator=(const RoICacheHistory&);

  private:

    FeatureCall                m_current;
    std::vector<FeatureCall>   m_questions;
    bool                    m_collect;

    NavigationCore         *m_navigation;
    const TriggerElement   *m_work;
  };
}

#endif
