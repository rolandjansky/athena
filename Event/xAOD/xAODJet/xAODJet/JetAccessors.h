// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_JETACCESSORS_H
#define XAODJET_JETACCESSORS_H
/////////////////////////////////////////////
/// \file JetAccessors.h
/// \brief This header defines wrapper classes around SG::AuxElement::Accessor used internally in the Jet EDM.
/// 
/// By default the wrappers add no functionality to this class. 
/// Their intent is to act as converter between types exposed in the Jet interface and the 
/// types stored in the Aux. Store.
/// For example :
///   - double         -> float
///   - vector<double> -> vector<float>
///   - JetFourMom_t   -> 4 separated floats
///   - IParticle and any class inheriting it -> ElementLink<IParticleContainer>
///   - Object inheriting AuxElement          -> ElementLink< DataVector<Object> >
///
/// These wrappers can then be used directly in the implementation of Jet::get/setAttribute 
/// methods which are thus much simpler.
/// 
/////////////////////////////////////////////

#include <string>
#include "AthContainers/AuxElement.h"
#include "xAODBase/IParticle.h"
#include "xAODJet/JetTypes.h"

using SG::AuxElement;
namespace xAOD {


  namespace JetAttributeAccessor {


    struct Named { 
      Named(const std::string & n) : m_name(n){}
      std::string name(){return m_name;}
    protected:
      std::string m_name;
    };

    template<class TYPE>
    class AccessorWrapper : public Named {
    public:
      typedef typename SG::AuxElement::Accessor< TYPE > AccessorType;
      AccessorWrapper(const std::string & n) : Named(n), a(n) {}
      
      void setAttribute(AuxElement& p, const TYPE& v){
        a(p) = v;
      }
      
      void getAttribute(const AuxElement& p,  TYPE& v){
        v = a(p);
      }

      const TYPE & getAttribute(const AuxElement& p){
        return a(p);
      }
      
      bool isAvailable(const AuxElement& p){ return a.isAvailable(p);}

      // forward calls to internal accessor. useful for performance critical code.
      const TYPE& operator()(const AuxElement& p)  { return a(p);}
      TYPE& operator()(AuxElement& p)  { return a(p);}

    protected:
      AccessorType a;
    };

    /// Double accessor specialization : internally stored as float
    template<>
    class AccessorWrapper<double> : public Named {
    public:
      typedef SG::AuxElement::Accessor< float > AccessorType;
      AccessorWrapper(const std::string & n) : Named(n) , a(n) {}
      
      void setAttribute(AuxElement& p, const double& v){
        a(p) = v;
      }
      
      void getAttribute(const AuxElement& p,  double& v){
        v = a(p);
      }

      // can't return a reference, since the internal is a float.
      double  getAttribute(const AuxElement& p){
        return  a(p);
      }

      
      bool isAvailable(const AuxElement& p){ return a.isAvailable(p);}

    protected:
      AccessorType a;
    };


    ///  specialization : internally stored as float
    template<>
    class AccessorWrapper< std::vector<double> > : public Named{
    public:
      typedef SG::AuxElement::Accessor< std::vector<float>  > AccessorType;
      AccessorWrapper(const std::string & n) : Named(n), a(n) {}
      
      void setAttribute(AuxElement& p, const std::vector<double>& v){
        a(p).assign( v.begin() , v.end() );
      }
      
      void getAttribute(const AuxElement& p,  std::vector<double>& v){
        const std::vector<float> & vecF = a(p);
        v.assign( vecF.begin() , vecF.end() );
      }

      // can't return a reference, since the internal is a float.
      std::vector<double>  getAttribute(const AuxElement& p){
        std::vector<double> v; getAttribute(p,v);
        return v;
      }

      
      bool isAvailable(const AuxElement& p){ return a.isAvailable(p);}

    protected:
      AccessorType a;
    };


    

    /////////////////////////////////////
    /// A base class holding accessors for 4 floats of a 4-vector
    class FourMomAccessor : public Named {
    public:
      FourMomAccessor(const std::string& name, const std::string& n0, const std::string& n1, const std::string& n2, const std::string& n3) :Named(name) , p0(n0), p1(n1), p2(n2), p3(n3) {}
      
      bool isAvailable(const AuxElement& e) const {return p0.isAvailable(e);}
      
    protected:
      SG::AuxElement::Accessor< float > p0;
      SG::AuxElement::Accessor< float > p1;
      SG::AuxElement::Accessor< float > p2;
      SG::AuxElement::Accessor< float > p3;            
    };


    /////////////////////////////////////
    /// Specialiazed accessor for JetFourMom_t : internally stores the JetFourMom_t as 4 floats in the Aux Store
    template<>
    class AccessorWrapper<JetFourMom_t> : public FourMomAccessor {
    public:
      AccessorWrapper() :  FourMomAccessor("_unnamed_","pt", "eta","phi", "m") {}
      AccessorWrapper(const std::string &name) : FourMomAccessor(name, name+"_pt", name+"_eta",name+"_phi", name+"_m") {}
      
      const float & pt(const AuxElement& p){ return p0(p);}
      const float & eta(const AuxElement& p){ return p1(p);}
      const float & phi(const AuxElement& p){ return p2(p);}
      const float & m(const AuxElement& p){ return p3(p);}

      void setAttribute(AuxElement& p, const JetFourMom_t& v){
        p0(p) = v.Pt();
        p1(p) = v.Eta();
        p2(p) = v.Phi();
        p3(p) = v.M();
      }
      
      void getAttribute(const AuxElement& p,  JetFourMom_t& v){
        v.SetPt(  p0(p) );
        v.SetEta(  p1(p) );
        v.SetPhi(  p2(p) );
        v.SetM(  p3(p) );
      }           

      JetFourMom_t getAttribute(const AuxElement &p){
        JetFourMom_t v; getAttribute(p,v);
        return v;
      }

      // other convenience function
      void setPtEtaPhiM(AuxElement& p, float pt, float eta, float phi, float m ){
        p0(p) = pt ;
        p1(p) = eta ;
        p2(p) = phi ;
        p3(p) = m ;
      }


    };
    /////////////////////////////////////


    /////////////////////////////////////
    /// Specialiazed accessor for FourMom_t : internally stores the FourMom_t as 4 floats in the Aux Store    
    template<>
    class AccessorWrapper<IParticle::FourMom_t> : public FourMomAccessor {
    public:
      AccessorWrapper(const std::string &name) : FourMomAccessor(name, name+"_px", name+"_py",name+"_pz", name+"_e") {}
      void setAttribute(AuxElement& p, const IParticle::FourMom_t& v){
        p0(p) = v.Px();
        p1(p) = v.Py();
        p2(p) = v.Pz();
        p3(p) = v.E();
      }
      
      void getAttribute(const AuxElement& p,  IParticle::FourMom_t& v){
        v.SetPx( p0(p) );
        v.SetPy( p1(p) );
        v.SetPz( p2(p) );
        v.SetE( p3(p) );
      }           

      IParticle::FourMom_t getAttribute(const AuxElement &p){
        IParticle::FourMom_t v; getAttribute(p,v);
        return v;
      }
      
    };
    /////////////////////////////////////



    /////////////////////////////////////    
    namespace {
      /// This anonymous namespace to define helper classes to deal with template code 
      /// 

      /// \class InheritsIParticle 
      /// Only one purpose : define a compile time constant :
      ///  InheritsIParticle<Obj>::Test is true if Obj inherits IParticle
      template<typename Obj>
      class InheritsIParticle
      {
       
        class No { };
        class Yes { No no[3]; };
        
        static Yes testFunc( IParticle* ); // declared, but not defined
        static No testFunc( ... ); // declared, but not defined
        
      public:
        enum { Test = sizeof(testFunc(static_cast<Obj*>(0))) == sizeof(Yes) };
      }; 
      

      /// Define typedef to the types used in Auxiliary Store for the template parameter Obj
      template<typename Obj, bool IsIP> 
      struct InternalTypes {
        typedef DataVector<Obj> ContainerType;
        typedef ElementLink< ContainerType > LinkType;
        typedef SG::AuxElement::Accessor< LinkType > AccessorType;        
        static const Obj* fromEL(const LinkType&el){if(el.isValid())return *el; return NULL;}
      };

      template<typename Obj> 
      struct InternalTypes<Obj,true> {
        typedef IParticleContainer ContainerType;
        typedef ElementLink< ContainerType >  LinkType;
        typedef SG::AuxElement::Accessor< LinkType > AccessorType;        
        static const Obj* fromEL(const LinkType&el){if(el.isValid()) return dynamic_cast<const Obj*>(*el); return NULL;}
      };

      template<typename Obj, bool IsIP> 
      struct InternalVectorTypes : public InternalTypes<Obj,IsIP> {
        typedef typename InternalTypes<Obj,IsIP>::LinkType LinkType;
        typedef SG::AuxElement::Accessor< std::vector<LinkType> > AccessorType;
      };
      


    }


    template<class TYPE>
    class ObjectAccessorWrapper: public Named {
    public:
      typedef InternalTypes<TYPE, InheritsIParticle<TYPE>::Test> InternalType;
      typedef typename InternalType::ContainerType ContainerType;
      typedef typename InternalType::LinkType LinkType;
      typedef typename InternalType::AccessorType AccessorType;

      ObjectAccessorWrapper(const std::string & n) : Named(n), a(n) {}

      void setAttribute(AuxElement& p, const TYPE* o){
        LinkType &el = a(p);
        el.toIndexedElement( *( dynamic_cast< const ContainerType* >( o->container() ) ), o->index() );  
        el.toPersistent();
      }
      

      const TYPE * getAttribute(const AuxElement& p){
        return InternalType::fromEL(  a(p) );
      }

      void getAttribute(const AuxElement& p, const TYPE *& att){
        att= InternalType::fromEL(  a(p) );
      }
      
      bool isAvailable(const AuxElement& p){ return a.isAvailable(p);}
      
      // // forward calls to internal accessor. useful for performance critical code.
      const TYPE* operator()(const AuxElement& p)  { return getAttribute(p); }
      // TYPE& operator()(AuxElement& p)  { LinkType &el=a(p); return *el;}

    protected:
      AccessorType a;      
    };

    ///////////////////////////////////////
    /// Accessor wrapper for vector< AuxElement> objects.
    ///
    /// The templated parameter is expected to inherit AuxElement.
    /// This wrapper convertes vector<const TYPE*> to vector<ElementLink<ContainerType>>
    /// where ContainerType is :
    ///   - IParticleContainer if TYPE also inherits IParticle
    ///   - DataVector<TYPE> else/
    //////////////////////////////////////////
    template<class TYPE>
    class ObjectAccessorWrapper<std::vector<const TYPE*> > : public Named {
    public:
      typedef InternalVectorTypes<TYPE, InheritsIParticle<TYPE>::Test> InternalType;
      typedef typename InternalType::ContainerType ContainerType;
      typedef typename InternalType::LinkType LinkType;
      typedef typename InternalType::AccessorType AccessorType;

      ObjectAccessorWrapper(const std::string & n) : Named(n), a(n) {}

      
      void vector2vectorEL(const std::vector<const TYPE*> & vec, std::vector< LinkType > & elv) {
        
        for(size_t i=0; i< vec.size() ; i++) { 
          LinkType el;
          el.toIndexedElement( *( dynamic_cast< const ContainerType* >( vec[i]->container() ) ), vec[i]->index() );      
          elv.push_back(el); 
          elv.back().toPersistent(); // The EL will not change anymore since it is purely internal. We can call toPersistent() to be sure it will be saved as expected.
        }
      }
      
      void setAttribute(AuxElement& p, const std::vector<const TYPE*> &vec){
        std::vector<LinkType> &elv = a(p); elv.clear();elv.reserve(vec.size());
        this->vector2vectorEL(vec, elv);
      }
      
      void getAttribute(const AuxElement& p,  std::vector<const TYPE*>& v){
        const std::vector<LinkType> &elv = a(p); 
        v.resize(elv.size());
        for(size_t i=0;i<elv.size(); i++) {v[i] = InternalType::fromEL(elv[i]) ; }
      }

      std::vector<const TYPE *> getAttribute(const AuxElement& p){
        const std::vector<LinkType> &elv = a(p); 
        std::vector<const TYPE*>  ipvec(elv.size() );
        for(size_t i=0;i<elv.size(); i++) ipvec[i] = InternalType::fromEL(elv[i]) ;
        return ipvec;
      }
      
      bool isAvailable(const AuxElement& p){ return a.isAvailable(p);}
      
      // // forward calls to internal accessor. useful for performance critical code.
      // const TYPE* operator()(const AuxElement& p)  { const LinkType &el = a(p) ;return *el;}
      // TYPE& operator()(AuxElement& p)  { LinkType &el=a(p); return *el;}

    protected:
      AccessorType a;
    };




  }// namespace JetAttributeAccessor
}
#endif
