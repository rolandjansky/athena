// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETANALYSISEDM_EventObject_H
#define JETANALYSISEDM_EventObject_H

#include "VarHandle.h"
#include "Utils.h"
#include "AttributeMap.h"
class TTree;

namespace JetAnalysisEDM {

  class EventObject {
  public:
    EventObject(const ::Long64_t& master, TTree * tree=0);
    virtual ~EventObject(){};

    virtual void readFrom(TTree* tree);

    // Some super usual numbers :
    /// Event number
    VarHandle< UInt_t > EventNumber;
    /// Run number
    VarHandle< UInt_t > RunNumber;
    /// Bunch crossing ID
    VarHandle< UInt_t > bcid;


    /// retrieve an attribute of type TT. 
    template<typename TT>
    typename tpl::const_if_ref_or_ptr<TT>::type get(const string_t & mName) const ;

    /// retrieve an attribute of type TT. 
    template<typename TT>
    typename tpl::const_if_ref_or_ptr<TT>::type get(const string_t & mName, bool &valid) const ;


    /// Some shortcuts 
    float getF(const string_t & mName);
    float getI(const string_t & mName);
    const std::vector<float>& getVecF(const string_t & mName);
    const std::vector<int>& getVecI(const string_t & mName);


    
    
    /// declare this variable to be saved in output tree
    template<typename T>
    void declareOutputVar(string_t varName) const {
      VarHandle<T>* v= m_varMap.addVar<T>( varName, false);
      v->setOutputVar(true);
      v->readFrom(m_varMap.inputTree());
    }

    ///declare the tree into which vars will be saved.
    void writeTo(TTree* tree);
    
    void readAllActive() const; 

    virtual const ::Long64_t * master() {return m_varMap.master();};
    virtual ::TTree*     inputTree() {return m_varMap.inputTree();}
  
    
  protected:
    
    mutable SimpleAttributeMap m_varMap;
    
  };


  
    /// retrieve an attribute of type TT. 
  template<typename TT>
  typename tpl::const_if_ref_or_ptr<TT>::type EventObject::get(const string_t & mName) const {    
    bool valid; 
    typename tpl::to_const_ref< TT >::type  r = get<TT>(mName, valid); 
    if (valid) return r;
    VarHandleLog::vlog.Error("EventObject::get"," can't retrieve %s. Will throw exception.",mName.c_str());    
    throw VarNotFound(string_t("Particle::get can't retrieve ")+mName);
    //abort();
  }    
  
  /// retrieve an attribute of type TT. 
  template<typename TT>
  typename tpl::const_if_ref_or_ptr<TT>::type EventObject::get(const string_t & mName, bool &valid) const {
    // make sure we use a non ref type.
    typedef typename tpl::remove_ref<TT>::type baseT; 
    // We create the var and associate it to the trree
    VarHandle<baseT> * v = m_varMap.addVar<baseT>(mName, false);
    if (!v->fromInput()) v->readFrom(m_varMap.inputTree() );
    typename tpl::to_const_ref< TT >::type  r = (*v)();
    valid = v->hasInputBranch();
    return r;
  }

}
#endif
