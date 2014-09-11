///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Py::DataStore.h 
// Header file for class Py::DataStore
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONEVENT_PY_DATASTORE_H 
#define PERFMONEVENT_PY_DATASTORE_H 

// PyCXX includes
#include "CXX/Extensions.hxx"

// STL includes
#include <string>
#include <list>
#include <vector>

// Forward declaration

namespace Py {

  /// helper method to convert a type to a Py::Object
  template<typename CxxType>
  struct cnv {
    // may work...
    static Py::Object to( CxxType t ) { return Py::Object(t); }
  };

  // partial specializations
  template<typename T>
  struct cnv< std::vector<T> >
  {
    static Py::Object to( const std::vector<T>& t ) 
    { 
      Py::Tuple o( t.size() );
      for ( std::size_t i = 0, iMax = t.size(); i != iMax; ++i ) {
	o[i] = cnv<T>::to(t[i]); 
      }
      return o;
    }
  };

  template<typename T, typename U>
  struct cnv< std::pair<T,U> >
  {
    static Py::Object to( const std::pair<T,U>& t ) 
    { 
      Py::Tuple o(2); 
      o[0] = cnv<T>::to(t.first);
      o[1] = cnv<U>::to(t.second);
      return o;
    }
  };

  // specializations
  template<>
  struct cnv<int>
  {
    static Py::Object to( int t )
    { return Py::Int(t); }
  };

  template<>
  struct cnv<unsigned int>
  {
    static Py::Object to( unsigned int t )
    { return Py::Int(static_cast<long int>(t)); }
  };

  template<>
  struct cnv<long>
  {
    static Py::Object to( long t )
    { return Py::Int(static_cast<long int>(t)); }
  };

  template<>
  struct cnv<long unsigned int>
  {
    static Py::Object to( long unsigned int t )
    { return Py::Int(static_cast<long int>(t)); }
  };

  template<>
  struct cnv<bool>
  {
    static Py::Object to( bool t )
    { return Py::Object( t ? Py_True : Py_False ); }
  };

  template<>
  struct cnv<float>
  {
    static Py::Object to( float t )
    { return Py::Float(t); }
  };

  template<>
  struct cnv<double>
  {
    static Py::Object to( double t ) 
    { return Py::Float(t); }
  };

  template<>
  struct cnv<std::string>
  {
    static Py::Object to( const std::string& t ) 
    { return Py::String(t); }
  };

}

namespace Py {

class DataStore : public Py::PythonExtension<Py::DataStore>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  DataStore( const Py::String& name = "", bool owned = false );

  /// Destructor: 
  virtual ~DataStore(); 

  /// python registration of this type
  static void init_type(void);

  // override functions from PythonExtension
  virtual Py::Object repr();
  virtual Py::Object getattr( const char *name );

  // define python methods of this object
  Py::Object value( const Py::Tuple& args );
  Py::Object assign( const Py::Tuple& args ); 
  Py::Object reference_count( const Py::Tuple& );

  Py::Object c_value(const Py::Tuple&) const
  {
    Py::List result;
    result.append( m_name );
    result.append( m_store );
    return result;
  }

  void c_assign(const Py::Tuple&, const Py::Object& rhs);

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  ///@{ python-API
  /// register a node in the store and some path to some optional data
  Py::Object py_book( const Py::Tuple& args, const Py::Dict& kws );

  /// fill some already registered node with data
  Py::Object py_fill( const Py::Tuple& args, const Py::Dict& kws );

  /// retrieve a data-node from a registered node/folder 
  Py::Object py_get( const Py::Tuple& args, const Py::Dict& kws );

  ///@}

  ///@{ C++ API
  /// register a node with the store
  void book( const std::string& nodeName,
	     const std::string& path );
  /// register a node with the store
  void book( const std::string& nodeName,
	     const std::list<std::string>& path );
  
  /// fill some already registered node with data
  template< typename Data >
  void fill( const std::string& name,
	     const std::list<std::string>& dataPath,
	     const Data& data )
  {
    Py::Tuple path( dataPath.size() );
    std::size_t idx = 0;
    for ( std::list<std::string>::const_iterator 
	    itr = dataPath.begin(),
	    iEnd= dataPath.end();
	  itr != iEnd;
	  ++itr, ++idx ) {
      path[idx] = Py::String(*itr);
    }
//     std::cout << "--fill( name=[" << name << "], "
// 	      << "dataPath=[" << path << "], "
// 	      << "data )..."
// 	      << std::endl;
    Py::Dict  store;
    if ( !m_store.hasKey(name) ) {
      throw Py::KeyError( "no such key["
			  +name
			  +"] registered in Py::DataStore !!" );
    }
    store = m_store[name];
    this->bucket( store, path ).append( Py::cnv<Data>::to(data) );
//     std::cout << "--fill( name=[" << name << "], "
// 	      << "dataPath=[" << path << "], "
// 	      << "data )..."
// 	      << " [OK]"
// 	      << std::endl;
  }

  /// fill some already registered node with data
  template< typename Data >
  void fill( const std::string& name,
	     const std::string& dataPath,
	     const Data& data )
  {
    std::list<std::string> path;
    this->tokenize( dataPath, path, "/" );
    this->fill( name, path, data );
  }
  ///@}

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// acces a data bucket
  Py::List bucket( const Py::Dict& store, const Py::Tuple& path );

  /// helper method to tokenize stuff
  template< typename T >
  void tokenize( const std::string& in,
		 std::list<T>& tokens,
		 const std::string& delimiters = "/" ) const
  {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = in.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = in.find_first_of(delimiters, lastPos);

    while ( std::string::npos != pos || std::string::npos != lastPos ) {
      // Found a token, add it to the vector.
      tokens.push_back( T( in.substr(lastPos, pos - lastPos) ) );
      // Skip delimiters.  Note the "not_of"
      lastPos = in.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = in.find_first_of(delimiters, lastPos);
    }
  
    return;
  }

  /// helper method to tokenize a Py::Object
  Py::Tuple tokenize( const Py::Object& obj, 
		      const std::string& delimiter = "/" ) const;
  
  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Name of that store
  Py::String m_name;

  /// data store
  Py::Dict m_store;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} // end namespace Py

#endif //> PERFMONEVENT_PY_DATASTORE_H
