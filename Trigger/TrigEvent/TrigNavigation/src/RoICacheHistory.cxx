/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdexcept>
#include <string>

#include "TrigNavigation/RoICacheHistory.h"

//-----------------------------------------------------------------------------
HLT::RoICacheHistory& HLT::RoICacheHistory::instance() {
  static RoICacheHistory instance;
  return instance;
}

//-----------------------------------------------------------------------------
void HLT::RoICacheHistory::addAnswer(const TriggerElement* te, const TriggerElement::FeatureAccessHelper &f)
{
  if ( m_collect ) {
    if (  m_current.isValid() ) {
      m_current.addAnswer(te, f);
    } else {
      throw std::runtime_error("HLT caching problem: " + std::to_string(te->getId()) );
    }
  }
}

//-----------------------------------------------------------------------------
void HLT::RoICacheHistory::stopCurrentFeatureCall()
{
  if ( m_collect ) {
    if (  ! m_current.isValid() ) {     
      throw std::runtime_error("HLT caching problem: invalid current question" );
    } else {
      //      std::cout << "RoICacheHistory stopping feature call: " << m_current.isget() << std::endl;
      if ( !m_current.isget() ) {
	m_questions.push_back(m_current);
      } else {
	// traverse back the questions
	bool local = false;
	for ( unsigned i = 0, j = m_questions.size(); i < j ; i++ ) {
	  //	  std::cout << "RoICacheHistory traversing back local history isget: " <<m_questions[i].isget() << std::endl;
	  if ( !m_questions[i].isget()) {
	    //	    std::cout << "RoICacheHistory encountered algo dooing attach before get" << std::endl;
	    if ( m_current.getAnswers() == m_questions[i].getAnswers()){
	      local = true;
	      //	      std::cout << "RoICacheHistory algorithm asking for object which is creating itself, ignoring" << std::endl;
	      break;
	    }
	  }
	}
	if (!local)
	  m_questions.push_back(m_current);
      }
      m_current.reset();
    }
  } else {
    throw std::runtime_error("HLT caching problem: question not collected" );
  }
}

//-----------------------------------------------------------------------------
void HLT::RoICacheHistory::setCurrentFeatureCall(const FeatureCall &curr, NavigationCore *navig)
{  
  if(m_current.isValid()) {
    throw std::runtime_error("HLT caching problem: current question is valid" );
  } else {
    m_current = curr;
    m_navigation = navig;
  }
}

//-----------------------------------------------------------------------------
HLT::RoICacheHistory::FeatureCall::FeatureCall(const TriggerElement* work, const TriggerElement* te, CLID clid, const std::string& label, bool issingle, bool isget) 
  :m_init(te),
   m_work(work),
   m_clid(clid),
   m_label(label),
   m_issingle(issingle),
   m_isget(isget) {
}

//-----------------------------------------------------------------------------
HLT::RoICacheHistory::FeatureCall::FeatureCall() 
  :m_init(0),
   m_work(0),
   m_clid(0),
   m_issingle(false),
   m_isget(false) {
}

//-----------------------------------------------------------------------------
void HLT::RoICacheHistory::FeatureCall::addAnswer(const TriggerElement* /*te*/, const TriggerElement::FeatureAccessHelper&f)
{
  m_answers.push_back(f);
}

//-----------------------------------------------------------------------------
void HLT::RoICacheHistory::FeatureCall::reset() 
{
  // Complete reset
  m_init = 0;
  m_label.clear();
  m_answers.clear();
}

//-----------------------------------------------------------------------------
bool HLT::RoICacheHistory::FeatureCall::isValid() const {
  return m_init;
}

HLT::RoICacheHistory::RememberAttachFeature::RememberAttachFeature(const TriggerElement* te, CLID clid, const std::string& label,
								   NavigationCore* navig, TriggerElement::ObjectIndex& idx) {
  HLT::RoICacheHistory& roih = HLT::RoICacheHistory::instance();
  if(!roih.collectHistory()) return;
  //  std::cout << "RoICacheHistory recording also attach feature" << std::endl;
  roih.setCurrentFeatureCall(HLT::RoICacheHistory::FeatureCall(roih.getWorkTE(), te, clid, label, true, false), navig);
  roih.m_current.addAnswer(te, TriggerElement::FeatureAccessHelper(clid, idx));
  roih.stopCurrentFeatureCall();
}


//-----------------------------------------------------------------------------
HLT::RoICacheHistory::QuestionScope::QuestionScope(const TriggerElement* te,
						   CLID clid,
						   const std::string& label,
						   NavigationCore *navig, bool issingle)
  : m_start(te)
{
  HLT::RoICacheHistory& roih = HLT::RoICacheHistory::instance();
  
  // Are we event collecting history?
  if(!roih.collectHistory()) return;

  if(roih.getCurrentFeatureCall().isValid()) {
    return;
  } else { 
    // Create current question
    roih.setCurrentFeatureCall(HLT::RoICacheHistory::FeatureCall(roih.getWorkTE(), te, clid, label, issingle, true), navig);
  }
}

HLT::RoICacheHistory::QuestionScope::QuestionScope() 
  : m_start(0)
{}


//-----------------------------------------------------------------------------
HLT::RoICacheHistory::QuestionScope::~QuestionScope() noexcept(false)
{
  if ( !m_start ) 
    return;

  HLT::RoICacheHistory& roih = HLT::RoICacheHistory::instance();
  
  // Are we event collecting history?
  if(!roih.collectHistory()) return;

  // Alway see valid question
  if(!roih.getCurrentFeatureCall().isValid()) {
    throw std::runtime_error("HLT caching problem: logic error in history");
  }

  // Return out of scope for first call
  if(roih.getCurrentFeatureCall().getInitTE() == m_start) {
    roih.stopCurrentFeatureCall();
  }
}

