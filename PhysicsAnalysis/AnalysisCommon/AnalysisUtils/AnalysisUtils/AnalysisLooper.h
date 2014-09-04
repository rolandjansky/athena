/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISUTILS_ANALYSISLOOPER_H
#define ANALYSISUTILS_ANALYSISLOOPER_H

/**
   generalized looper

   @author Tadashi Maeno
*/

#include <vector>

namespace AnalysisUtils {
  namespace Private {
    
    // Null type for list terminator
    class NullType {};

    // Bool to Type mapper
    template <bool v> struct Bool2Type
    {
      enum {value = v};
    };

    // forward declaration
    template <class PAR1, class PAR2> struct ParamChain;

    // get length of the parameter chain
    template <class CHAIN> struct Length;

    template <class H, class T> struct Length< ParamChain<H,T> >
    {
      enum {value = 1+Length<T>::value};
    };
    
    template <> struct Length<NullType>
    {
      enum {value = 0};
    };
    
    // get type at index in the parameter chain
    template <class CHAIN, unsigned int i> struct TypeAt;

    template <class H, class T, unsigned int i> struct TypeAt< ParamChain<H,T>, i >
    {
      typedef typename TypeAt<T,i-1>::type type;
    };

    template <class H, class T> struct TypeAt< ParamChain<H,T>, 0 >
    {
      typedef H type;
    };

    // parameter chain
    template <class PAR1, class PAR2> struct ParamChain
    {
      typedef PAR1 Head;
      typedef PAR2 Tail;
      typedef ParamChain<PAR1,PAR2> MyType;
      
      // constructor
      ParamChain(PAR2 *p2) : m_p1(0), m_p2(p2) {}
      
      // assign
      void assign(PAR1 p1) {m_p1 = p1;}
      
      // get parameter stored at the index in the chain
      template <unsigned int i> typename TypeAt<MyType,Length<MyType>::value-1-i>::type par()
      {
	return parHelper<i>(Bool2Type<i==Length<MyType>::value-1>());
      }
      
    private:
      PAR1 m_p1;
      PAR2 *m_p2;

      // helper to get parameter stored at the index in the chain
      template <unsigned int i> typename TypeAt<MyType,Length<MyType>::value-1-i>::type parHelper(Bool2Type<true>)
      {
	typedef typename TypeAt<MyType,Length<MyType>::value-1-i>::type return_t;
	if (i==Length<MyType>::value-1) return reinterpret_cast<return_t>(m_p1);
      }

      template <unsigned int i> typename TypeAt<MyType,Length<MyType>::value-1-i>::type parHelper(Bool2Type<false>)
      {
	typedef typename TypeAt<MyType,Length<MyType>::value-1-i>::type return_t;
	return reinterpret_cast<return_t>(m_p2->par<i>());
      }
    };
    
    // specialized for the chain end
    template <class PAR1> struct ParamChain<PAR1, NullType>
    {
      typedef PAR1     Head;
      typedef NullType Tail;
      typedef ParamChain<PAR1,NullType> MyType;
      
      ParamChain() : m_p1(0) {}
      // assign
      void assign(PAR1 p1) {m_p1 = p1;}
      
      template <unsigned int i> PAR1 par() {return m_p1;}
      
    private:
      PAR1 m_p1;
    };
    
    // base class to run all Looper at the same time
    class LooperBase 
    {
    public:
      LooperBase () {}
      virtual ~LooperBase() {}
      
      // this method must be implemented in derived classes
      virtual void execute() = 0;
      
      // this method may be called from Daughter Looper
      void addDaughter (LooperBase *d) {m_daughter.push_back(d);}
      
      // execute all daughters
      void executeDaughter ()
      {
	std::vector <LooperBase *>::iterator it  = m_daughter.begin();
	std::vector <LooperBase *>::iterator itE = m_daughter.end();
	for (; it!=itE; ++it)
	  (*it)->execute();
      }
      
    private:
      std::vector <LooperBase *> m_daughter;
    };
    
  }; // end of Private
  
  
  /** Analysis Looper class
      @param COLL    : collection
      @param ACTION  : action policy must have a method "void action(...)"
      @param PLOOPER : parent looper
  */
  template <class COLL, class ACTION, class PLOOPER>
  struct Looper : public ACTION, public Private::LooperBase
  {
    typedef Private::ParamChain<typename COLL::value_type, typename PLOOPER::chain_type >
    chain_type;
    
    // parameter chain
    chain_type *m_chain;
    
    // collection
    COLL *m_coll;
    
    // constructor
    Looper (COLL *coll, PLOOPER *parent) : m_coll(coll) {
      parent->addDaughter(this);
      m_chain = new chain_type (parent->m_chain);
    }
    
    // destructor
    ~Looper() {delete m_chain;}
    
    void execute ()
    {
      typename COLL::const_iterator it  = m_coll->begin();
      typename COLL::const_iterator itE = m_coll->end();
      for (; it!=itE; ++it)
	{
	  m_chain->assign(*it);
	  ACTION::action(m_chain);
	  executeDaughter ();
	}
    }
  };
  
  // specialized for Top Looper with Action
  template <class COLL, class ACTION>
  struct Looper<COLL, ACTION, Private::NullType>
    : public ACTION, public Private::LooperBase
  {
    typedef Private::ParamChain<typename COLL::value_type, Private::NullType> chain_type;
    
    chain_type *m_chain;
    COLL *m_coll;
    
    Looper (COLL *coll) : m_coll(coll) {
      m_chain = new chain_type;
    }
    ~Looper() {delete m_chain;}
    
    void execute ()
    {
      typename COLL::const_iterator it  = m_coll->begin();
      typename COLL::const_iterator itE = m_coll->end();
      for (; it!=itE; ++it)
	{
	  m_chain->assign(*it);
	  ACTION::action(m_chain);
	  executeDaughter ();
	}
    }
  };
  
  // specialized for Top Looper without Action
  template <class COLL>
  struct Looper<COLL, Private::NullType, Private::NullType>
    : public Private::NullType, public Private::LooperBase
  {
    typedef Private::ParamChain<typename COLL::value_type, Private::NullType> chain_type;
    
    chain_type *m_chain;
    COLL *m_coll;
    
    Looper (COLL *coll) : m_coll(coll) {
      m_chain = new chain_type;
    }
    ~Looper() {delete m_chain;}
    
    // execute only daughters
    void execute ()
    {
      typename COLL::const_iterator it  = m_coll->begin();
      typename COLL::const_iterator itE = m_coll->end();
      for (; it!=itE; ++it)
	{
	  m_chain->assign(*it);
	  executeDaughter ();
	}
    }
  };
} // end of AnalysisUtils namespace


#define PARLIST_1(P1)          AnalysisUtils::Private::ParamChain<P1,AnalysisUtils::Private::NullType>
#define PARLIST_2(P1,P2)       AnalysisUtils::Private::ParamChain<P2, PARLIST_1(P1) >
#define PARLIST_3(P1,P2,P3)    AnalysisUtils::Private::ParamChain<P3, PARLIST_2(P1,P2) >
#define PARLIST_4(P1,P2,P3,P4) AnalysisUtils::Private::ParamChain<P4, PARLIST_3(P1,P2,P3) >

#define DEF_TopLooper(COLL,ACT)   typedef AnalysisUtils::Looper<COLL,ACT,AnalysisUtils::Private::NullType>
#define DEF_TopLooperWoAct(COLL)  typedef AnalysisUtils::Looper<COLL,AnalysisUtils::Private::NullType,AnalysisUtils::Private::NullType>
#define DEF_Looper(COLL,ACT,PAR)  typedef AnalysisUtils::Looper<COLL,ACT,PAR>
#define DEF_LooperWoAct(COLL,PAR) typedef AnalysisUtils::Looper<COLL,AnalysisUtils::Private::NullType,PAR>

#endif // end of ANALYSISTOOLS_ANALYSISLOOPER_H
