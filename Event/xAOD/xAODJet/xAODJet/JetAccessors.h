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
#include <type_traits>
#include "AthContainers/AuxElement.h"
#include "xAODBase/IParticle.h"
#include "xAODJet/JetTypes.h"

namespace xAOD {


  namespace JetAttributeAccessor {


    struct Named { 
      Named(const std::string & n) : m_name(n){}
      std::string name() const {return m_name;}
    protected:
      std::string m_name;
    };

    template<class TYPE>
    class AccessorWrapper : public Named {
    public:
      typedef typename SG::AuxElement::Accessor< TYPE > AccessorType;
      AccessorWrapper(const std::string & n) : Named(n), m_a(n) {}
      
      void setAttribute(SG::AuxElement& p, const TYPE& v) const {
        m_a(p) = v;
      }
      
      void getAttribute(const SG::AuxElement& p,  TYPE& v) const {
        v = m_a(p);
      }

      const TYPE & getAttribute(const SG::AuxElement& p) const {
        return m_a(p);
      }
      
      bool isAvailable(const SG::AuxElement& p) const { return m_a.isAvailable(p);}

      // forward calls to internal accessor. useful for performance critical code.
      const TYPE& operator()(const SG::AuxElement& p) const { return m_a(p);}
      TYPE& operator()(SG::AuxElement& p)  const { return m_a(p);}

    protected:
      AccessorType m_a;
    };

    /// Double accessor specialization : internally stored as float
    template<>
    class AccessorWrapper<double> : public Named {
    public:
      typedef SG::AuxElement::Accessor< float > AccessorType;
      AccessorWrapper(const std::string & n) : Named(n) , m_a(n) {}
      
      void setAttribute(SG::AuxElement& p, const double& v) const {
        m_a(p) = v;
      }
      
      void getAttribute(const SG::AuxElement& p,  double& v) const {
        v = m_a(p);
      } 

      // can't return a reference, since the internal is a float.
      double  getAttribute(const SG::AuxElement& p) const {
        return  m_a(p);
      }

      
      bool isAvailable(const SG::AuxElement& p) const { return m_a.isAvailable(p);}

    protected:
      AccessorType m_a;
    };


    ///  specialization : internally stored as float
    template<>
    class AccessorWrapper< std::vector<double> > : public Named{
    public:
      typedef SG::AuxElement::Accessor< std::vector<float>  > AccessorType;
      AccessorWrapper(const std::string & n) : Named(n), m_a(n) {}
      
      void setAttribute(SG::AuxElement& p, const std::vector<double>& v) const {
        m_a(p).assign( v.begin() , v.end() );
      }
      
      void getAttribute(const SG::AuxElement& p,  std::vector<double>& v) const {
        const std::vector<float> & vecF = m_a(p);
        v.assign( vecF.begin() , vecF.end() );
      }

      // can't return a reference, since the internal is a float.
      std::vector<double>  getAttribute(const SG::AuxElement& p) const {
        std::vector<double> v; getAttribute(p,v);
        return v;
      }

      
      bool isAvailable(const SG::AuxElement& p) const { return m_a.isAvailable(p);}

    protected:
      AccessorType m_a;
    };


    

    /////////////////////////////////////
    /// A base class holding accessors for 4 floats of a 4-vector
    class FourMomAccessor : public Named {
    public:
      FourMomAccessor(const std::string& name, const std::string& n0, const std::string& n1, 
                      const std::string& n2, const std::string& n3) :Named(name) , m_p0(n0), m_p1(n1), m_p2(n2), m_p3(n3) {}
      
      bool isAvailable(const SG::AuxElement& e) const {return m_p0.isAvailable(e);}
      
    protected:
      SG::AuxElement::Accessor< float > m_p0;
      SG::AuxElement::Accessor< float > m_p1;
      SG::AuxElement::Accessor< float > m_p2;
      SG::AuxElement::Accessor< float > m_p3;            
    };


    /////////////////////////////////////
    /// Specialiazed accessor for JetFourMom_t : internally stores the JetFourMom_t as 4 floats in the Aux Store
    template<>
    class AccessorWrapper<JetFourMom_t> : public FourMomAccessor {
    public:
      AccessorWrapper() :  FourMomAccessor("_unnamed_","pt", "eta","phi", "m") {}
      AccessorWrapper(const std::string &name) : FourMomAccessor(name, name+"_pt", name+"_eta",name+"_phi", name+"_m") {}
      
      const float & pt(const SG::AuxElement& p) const { return m_p0(p);}
      const float & eta(const SG::AuxElement& p) const { return m_p1(p);}
      const float & phi(const SG::AuxElement& p) const { return m_p2(p);}
      const float & m(const SG::AuxElement& p) const { return m_p3(p);}

      void setAttribute(SG::AuxElement& p, const JetFourMom_t& v) const {
        m_p0(p) = v.Pt();
        m_p1(p) = v.Eta();
        m_p2(p) = v.Phi();
        m_p3(p) = v.M();
      }
      
      void getAttribute(const SG::AuxElement& p,  JetFourMom_t& v) const {
        v.SetPt(  m_p0(p) );
        v.SetEta(  m_p1(p) );
        v.SetPhi(  m_p2(p) );
        v.SetM(  m_p3(p) );
      }           

      JetFourMom_t getAttribute(const SG::AuxElement &p) const {
        JetFourMom_t v; getAttribute(p,v);
        return v;
      }

      // other convenience function
      void setPtEtaPhiM(SG::AuxElement& p, float pt, float eta, float phi, float m ){
        m_p0(p) = pt ;
        m_p1(p) = eta ;
        m_p2(p) = phi ;
        m_p3(p) = m ;
      }


    };
    /////////////////////////////////////


    /////////////////////////////////////
    /// Specialiazed accessor for FourMom_t : internally stores the FourMom_t as 4 floats in the Aux Store    
    template<>
    class AccessorWrapper<IParticle::FourMom_t> : public FourMomAccessor {
    public:
      AccessorWrapper(const std::string &name) : FourMomAccessor(name, name+"_px", name+"_py",name+"_pz", name+"_e") {}
      void setAttribute(SG::AuxElement& p, const IParticle::FourMom_t& v) const {
        m_p0(p) = v.Px();
        m_p1(p) = v.Py();
        m_p2(p) = v.Pz();
        m_p3(p) = v.E();
      }
      
      void getAttribute(const SG::AuxElement& p,  IParticle::FourMom_t& v) const {
        v.SetPx( m_p0(p) );
        v.SetPy( m_p1(p) );
        v.SetPz( m_p2(p) );
        v.SetE( m_p3(p) );
      }           

      IParticle::FourMom_t getAttribute(const SG::AuxElement &p) const{
        IParticle::FourMom_t v; getAttribute(p,v);
        return v;
      }
      
    };
    /////////////////////////////////////



    /////////////////////////////////////    
    namespace {
      /// This anonymous namespace to define helper classes to deal with template code 

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
      typedef InternalTypes<TYPE, std::is_base_of<IParticle, TYPE>::value> InternalType;
      typedef typename InternalType::ContainerType ContainerType;
      typedef typename InternalType::LinkType LinkType;
      typedef typename InternalType::AccessorType AccessorType;

      ObjectAccessorWrapper(const std::string & n) : Named(n), m_a(n) {}

      void setAttribute(SG::AuxElement& p, const TYPE* o) const {
        LinkType &el = m_a(p);
        el.toIndexedElement( *( dynamic_cast< const ContainerType* >( o->container() ) ), o->index() );  
        el.toPersistent();
      }
      

      const TYPE * getAttribute(const SG::AuxElement& p) const {
        return InternalType::fromEL(  m_a(p) );
      }

      void getAttribute(const SG::AuxElement& p, const TYPE *& att) const {
        att= InternalType::fromEL(  m_a(p) );
      }
      
      bool isAvailable(const SG::AuxElement& p) const { return m_a.isAvailable(p);}
      
      // // forward calls to internal accessor. useful for performance critical code.
      const TYPE* operator()  (const SG::AuxElement& p)  const  { return getAttribute(p); }
      // TYPE& operator()(SG::AuxElement& p)  { LinkType &el=m_a(p); return *el;}

    protected:
      AccessorType m_a;      
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
      typedef InternalVectorTypes<TYPE, std::is_base_of<IParticle, TYPE>::value> InternalType;
      typedef typename InternalType::ContainerType ContainerType;
      typedef typename InternalType::LinkType LinkType;
      typedef typename InternalType::AccessorType AccessorType;

      ObjectAccessorWrapper(const std::string & n) : Named(n), m_a(n) {}

      
      void vector2vectorEL(const std::vector<const TYPE*> & vec, std::vector< LinkType > & elv) const {
        
        for(size_t i=0; i< vec.size() ; i++) { 
          LinkType el;
          el.toIndexedElement( *( dynamic_cast< const ContainerType* >( vec[i]->container() ) ), vec[i]->index() );      
          elv.push_back(el); 
          elv.back().toPersistent(); // The EL will not change anymore since it is purely internal. We can call toPersistent() to be sure it will be saved as expected.
        }
      }
      
      void setAttribute(SG::AuxElement& p, const std::vector<const TYPE*> &vec) const {
        std::vector<LinkType> &elv = m_a(p); elv.clear();elv.reserve(vec.size());
        this->vector2vectorEL(vec, elv);
      }
      
      void getAttribute(const SG::AuxElement& p,  std::vector<const TYPE*>& v) const {
        const std::vector<LinkType> &elv = m_a(p); 
        v.resize(elv.size());
        for(size_t i=0;i<elv.size(); i++) {v[i] = InternalType::fromEL(elv[i]) ; }
      }

      std::vector<const TYPE *> getAttribute(const SG::AuxElement& p) const {
        const std::vector<LinkType> &elv = m_a(p); 
        std::vector<const TYPE*>  ipvec(elv.size() );
        for(size_t i=0;i<elv.size(); i++) ipvec[i] = InternalType::fromEL(elv[i]) ;
        return ipvec;
      }
      
      bool isAvailable(const SG::AuxElement& p) const { return m_a.isAvailable(p);}
      
      // // forward calls to internal accessor. useful for performance critical code.
      // const TYPE* operator()(const AuxElement& p)  { const LinkType &el = m_a(p) ;return *el;}
      // TYPE& operator()(AuxElement& p)  { LinkType &el=m_a(p); return *el;}

    protected:
      AccessorType m_a;
    };




  }// namespace JetAttributeAccessor
}
#endif
