//  -*- c++ -*- 

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETVARIABLE_H
#define JETMONITORING_JETVARIABLE_H
#include <vector>


#include "xAODJet/Jet.h"

///////////////////////////////////////////////////////////
/// \class JetVar::Variable
///
/// The goal of this class is to define a common way to access any "jet variable".
/// Here "jet variable" is any quantity calculable from a single jet.
/// Often this will be a jet attribute stored in the xAOD::Jet class, but it can also
/// be kinematics (ex: rapidity()) or an other quantity.
/// JetVar::Variable allows access to the variables independently of their type or implementation details. It can then be used in generic tools to fill histograms (JetHistoAttributeFiller) or to select Jets (JetHistoSelectSort).
///
/// JetVar::Variable is the base class providing the acces methods. Concrete classes derive from it, in
///  particular VariableAtt is the generic (templated) class to access atributes stored in the xAOD::Jet object.
/// The function JetVar::Variable::create() allows to dynamicaly instantiate a variable from a name and a type passed as strings.
///



namespace JetVar {
  template<typename T>
  using Accessor = SG::AuxElement::Accessor< T >;


  /// VectorValue is a helper class to access any jet variable of type vector<X>
  /// It is implemented this way, so it is cheap to copy and can be returned from
  /// a const method. (there's possibly a simpler implementation...)
  struct VectorValue {
    struct VectorWrapper {
      virtual ~VectorWrapper(){}
      virtual float at(int) const =0;
      virtual size_t size() const =0;
    };
    VectorValue(VectorWrapper * v=nullptr , float s=1.) : m_v(v), m_scale(s){}
    float operator[](int i) const {return m_v->at(i)*m_scale;}
    size_t size()const {return m_v->size();}
    std::unique_ptr<VectorWrapper> m_v;
    float m_scale=1;
  };

  
  class Variable {
  public:
    
    Variable(const std::string & name) : m_name(name) {}
    virtual ~Variable() = default;
    virtual float value(const xAOD::Jet &) const = 0;

    /// return true if the underlying variable is of type vector<X>
    virtual bool isVector() const {return false;}

    /// return a helper object allowing to iterate over the underlying vector. !!! use only if isVector()==true !!!
    virtual VectorValue vector(const xAOD::Jet &) const {return VectorValue();}
    
    
    virtual std::string name() const {return m_name;}

    float scale() const {return m_scale;}
    void setScale(float s)  { m_scale=s;}



    /// create and return a new Variable of a given name & type. If type is a vector an index 
    /// can be specified : the variable will behave as a non-vector Variable corresponding to the value at the given index
    static Variable* create(const std::string & name, const std::string &type="float", int index=-1);

    std::string m_name;
    float m_scale = 1;
    int m_index = -1;
  };




  // ********************************************************
  // Concrete implementations of Variable


  /// \class VariableAtt the variable is an attribute in the EDM
  template<typename T>
  struct VariableAtt : public Variable {
    VariableAtt(const std::string & name) : Variable(name), m_acc(name) {}
    virtual float value(const xAOD::Jet & j) const { return m_acc(j)*m_scale;}
    Accessor<T> m_acc;    
  };

  
  
  /// Specialization of VariableAtt for the vector<> type
  template<typename T>
  struct VariableAtt<std::vector<T> > : public Variable {
    typedef typename std::vector<T> vect_t;
    struct VectorWrapperT : public VectorValue::VectorWrapper {
      VectorWrapperT(const vect_t *v): m_v(v){};
      float at(int i) const {return (*m_v)[i];}
      size_t size() const {return m_v->size();}
      const vect_t * m_v;
    };
    
    VariableAtt(const std::string & name, int index) : Variable(name), m_acc(name) {
      m_index=index;
    }

    virtual std::string name() const {
      if(isVector() )return m_name;
      return m_name+std::to_string(m_index);
    }

    // returns false if the index is valid : in this case it is a simple variable
    virtual bool isVector() const {return m_index==-1;}

    // use only if the index is valid
    virtual float value(const xAOD::Jet & j) const { return m_acc(j)[m_index]*m_scale;}

    virtual VectorValue vector(const xAOD::Jet &j) const {
      VectorValue v( new VectorWrapperT(&m_acc(j)) , m_scale ) ;
      return v;
    }


    Accessor<vect_t> m_acc;
  };

  
  

  // *******************************************
  // The classes below represent variables not stored as attribute in the Jet EDM
  // 
  
  struct EVar : public Variable {
    using Variable::Variable;
    virtual float value(const xAOD::Jet & j) const { return j.e()*m_scale;}    
  };

  struct PzVar : public Variable {
    using Variable::Variable;
    virtual float value(const xAOD::Jet & j) const { return j.pz()*m_scale;}    
  };

  struct NconstitVar : public Variable {
    using Variable::Variable;
    virtual float value(const xAOD::Jet & j) const { return j.numConstituents();}    
  };

  struct Rapidity : public Variable {
    using Variable::Variable;
    virtual float value(const xAOD::Jet & j) const { return j.rapidity();}    
  };
  
  
  struct AbsEtaVar : public Variable {
    using Variable::Variable;
    virtual float value(const xAOD::Jet & j) const { return fabs(j.eta());}    
  };

  
}

#endif
