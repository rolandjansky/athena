/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_DATATYPE_H
#define JIVEXML_DATATYPE_H

#include <string>
#include <sstream>
#include <map>
#include <vector>

namespace JiveXML{

  /**
   * Templated class to convert any object that is streamable
   * in a ostringstream in a string
   * @author Peter Sherwood
   * @author Sebastian Boeser
   */
  class DataType {
  
    private:
      std::string m_thing;
      
    public:

      /**
       * Templated constructor -- this is resolved at compile time
       * so the constructor must be implemented in the header.
       */
      template < class T > DataType(T t){ 
        /// Generate a stringstream object
        std::ostringstream ost;
        /// Generically stream the type object into it
        ost << t;
        /// Only retain the string object
        m_thing = ost.str() ;
        /// Convert some exception values
        if ((m_thing=="inf")||
            (m_thing=="nan")){
          m_thing = "0";
        }
      }

      /// Empty constructor
      DataType();

      /// Return the string
      std::string toString() const ;
  };
  
  /// Allow this to be streamed in an ostream
  std::ostream& operator<<( std::ostream& os, const DataType &dt );

  /// Defines a map with a key and a vector of DataType objects
  /// e.g. "CellID" and a vector of cell IDs
  typedef std::vector<DataType> DataVect ;
  typedef std::map<std::string, DataVect > DataMap ;

  /// Defines a tag as a pair of strings
  typedef std::pair<std::string,std::string> TagType;

} //namespace

#endif
