/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _VrtSecInclusive_NtupleVars_H
#define _VrtSecInclusive_NtupleVars_H


// Normal STL and physical vectors

#include "VrtSecInclusive/IntersectionPos.h"


#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cassert>
#include <cxxabi.h>

#include <TTree.h>

namespace VKalVrtAthena {
  //____________________________________________________________________________________________________
  // This base holder allows for NtupleVars to hold any type of variable.
  class BaseHolder {
  public:
    BaseHolder() {}
    virtual ~BaseHolder() throw() {}

    // This get_typename() function gives back the type of the variable
    // stored in the derived class varHolder<T>.
    // This ensures to check the type of a given variable name.
    virtual std::string& get_typename() = 0;
    virtual void* get_var() = 0;
    virtual void clear() = 0;
    virtual void branch_tree( const std::string&, TTree* ) = 0;
  };


  //____________________________________________________________________________________________________
  // This is a class template which holds a variable m_var.
  // This class is a BaseHolder, then can be stored in
  // std::map<string, BaseHoder*> variable list

  namespace varHolder_detail {
    template<typename T> void clear( T& var ) { var = 0; }
    template<typename T> void clear( std::vector<T>& var ) { var.clear(); }
    template<typename T> void clear( std::vector< std::vector<T> >& var ) {
      for( auto & v : var ) v.clear();
      var.clear();
    }
    
    static std::map<const std::string, const std::string> create_tree_str() {
      std::map<const std::string, const std::string> m;
      int stat(0);
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( char )                   .name(), 0, 0, &stat)), std::string("/B") ) );
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( unsigned char )          .name(), 0, 0, &stat)), std::string("/b") ) );
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( short )                  .name(), 0, 0, &stat)), std::string("/S") ) );
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( unsigned short )         .name(), 0, 0, &stat)), std::string("/s") ) );
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( int )                    .name(), 0, 0, &stat)), std::string("/I") ) );
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( unsigned int )           .name(), 0, 0, &stat)), std::string("/i") ) );
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( long long int )          .name(), 0, 0, &stat)), std::string("/L") ) );
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( unsigned long long int ) .name(), 0, 0, &stat)), std::string("/l") ) );
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( float )                  .name(), 0, 0, &stat)), std::string("/F") ) );
      m.insert( std::make_pair( std::string(abi::__cxa_demangle(typeid( double )                 .name(), 0, 0, &stat)), std::string("/D") ) );
      
      return m;
    }
    
    static const std::map<const std::string, const std::string> tree_str_map = create_tree_str();
    
    template<typename T>
    void branch_tree( const std::string& name, const std::string& type_name, TTree *tree, T* var )
    { tree->Branch( name.c_str(), var, ( name+tree_str_map.at(type_name) ).c_str() ); }
    
    template<typename T>
    void branch_tree( const std::string& name, const std::string& type_name, TTree *tree, std::vector<T>* var )
    { tree->Branch( name.c_str(), type_name.c_str(), var );                        }
  }
  
  
  //____________________________________________________________________________________________________
  template<class T>
  class varHolder : public BaseHolder {
  private:
    std::string m_typename;
  public:
    std::shared_ptr<T> m_var;
    
  varHolder() : m_var( new T ){
      int stat(0);
      char* name = abi::__cxa_demangle(typeid( T ).name(), 0, 0, &stat);
      m_typename = name;
      delete name;
    }
    virtual ~varHolder() {}
    T& get() { return (*m_var); }
    void* get_var() { return (void*)( m_var.get() ); }
    std::string& get_typename() { return m_typename; }
    
    // Partial template specialization for member functions are not allowed in C++.
    // --> defined a set of detailed functions, and call it.
    void clear() { varHolder_detail::clear( this->get() ); }
    
    void branch_tree( const std::string& name, TTree *tree ) {
      varHolder_detail::branch_tree( name, m_typename, tree, m_var.get() );
      //tree->Branch(name.c_str(), m_typename.c_str(), m_var.get() );
    }

    
  };
  
  
  //____________________________________________________________________________________________________
  class NtupleVars {
  private:
    std::map<std::string, BaseHolder*> m_varHolder;
    
  public:
    
    std::vector<IntersectionPos*> m_RecoTrk_Int;
    std::vector<IntersectionPos*> m_SVTrk_Int;
    
    NtupleVars();
    ~NtupleVars();
    
    void setupNtupleVariables();
    void branchNtuple( TTree *tree );
    void deleteNtupleVariables();
    
    
    template<class T> void addNewVar(const std::string varname) {
      m_varHolder.insert( make_pair(std::string(varname), new varHolder<T>) );
    }
  
    template<class T> bool isAvailable(const std::string varname) {
      if( m_varHolder.find(varname) == m_varHolder.end() ) {
        std::cerr << "Warning<TImprovedTree::isAvailable>: "
          << "couldn't find the variable name " << varname
                  << std::endl;
        return false;
      } else {
        BaseHolder* var = m_varHolder.at( varname );
        int stat;
        char* name = abi::__cxa_demangle(typeid( T ).name(), 0, 0, &stat);
      
        if( var->get_typename() == name ) {
          delete name;
          return true;
        } else {
          delete name;
          std::cerr << "Info<TImprovedTree::isAvailable>: " << "typeid didn't match." << std::endl;
          std::cerr << "Possible candidates are..." << std::endl;
          std::cerr << "========" << std::endl;
          std::cerr << varname << " (" << var->get_typename() << ")" << std::endl;
          std::cerr << "========" << std::endl;
          return false;
        }
        
      }
    }
  
    template<class T> T& get(const std::string varname) {
      try{
        if( !isAvailable<T>(varname) ) {
          throw( varname );
        }

        varHolder<T>* holder = static_cast< varHolder<T>* >( m_varHolder.at( varname ) );
        return holder->get();
      
      } catch( std::string& varname ) {
        std::cerr << "Variable " << varname << " is not found in the list!" << std::endl;
        std::cerr << "Candidates are..." << std::endl;
        int stat(0);
        for( auto pair :  m_varHolder ) {
          char* name = abi::__cxa_demangle(typeid( T ).name(), 0, 0, &stat);
          if( pair.second->get_typename() == name ) {
            std::cerr << "  " << pair.second->get_typename() << " : " << pair.first << std::endl;
          }
          delete name;
        }
        
      }

      exit(1);
    }

    void clear();

    
  };
  
  
}  // end of namespace bracket


#endif /* _VrtSecInclusive_NtupleVars_H */
