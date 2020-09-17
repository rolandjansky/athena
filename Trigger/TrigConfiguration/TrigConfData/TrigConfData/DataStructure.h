/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_DATASTRUCTURE_H
#define TRIGCONFDATA_DATASTRUCTURE_H

/**
 * @brief Base class for Trigger configuration data and wrapper around underlying representation
 */

#include <iostream>
#include <vector>
#include <memory>
#include <type_traits>
#include "boost/property_tree/ptree.hpp"

namespace TrigConf {

   /**
    * @brief Base class for Trigger configuration data and wrapper around underlying representation
    *
    * Triggger configuration description data uses json format which can be represented 
    * as boost::ptree (or other formats if ATLAS decides to support those)
    * 
    * The class acts as layer to shield the clients of the trigger configuration from the concrete
    * representation of the transient configuration data
    * 
    * Configuration data is typically recursive, meaning that sub-configurations can also be expressed
    * as DataStructure objects. Therefore this class also is base class to all other classes describing
    * parts of the trigger configuration in this package
    *
    * Detailed information can be found under
    *     <https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerConfigurationAccess>
    */
   class DataStructure {
   public:

      using ptree = boost::property_tree::ptree;

      /** Default constructor, leading to an uninitialized configuration object */
      DataStructure();

      DataStructure(const DataStructure&) = default;
      DataStructure(DataStructure&&) = default;

      DataStructure& operator=(const DataStructure&) = default;
      DataStructure& operator=(DataStructure&&) = default;

      /** Constructor initialized with configuration data 
       * @param data Reference to the data container 
       */
      DataStructure(const ptree & data);
      DataStructure(const std::string & name, const ptree & data);
      DataStructure(ptree && data);
      DataStructure(const std::string & name, ptree && data);

      /** Destructor */
      virtual ~DataStructure();

      /** @brief Setting the configuration data */
      void setData(const ptree & data);
      void setData(ptree && data);

      /** @brief Setting the configuration element name */
      void setName(const std::string& n);

      /** A string that is the name of the class */
      virtual std::string className() const;

      virtual const std::string & name() const final;

      /** Clearing the configuration data
       * 
       * leads to an uninitialized object
       */
      void clear();

      /** Access to the underlying data, if needed */
      const ptree & data() const {
         if( ! isInitialized() ) {
            throw std::runtime_error("Trying to access data of uninitialized object of type " + className());
         }
         return ownsData() ? *m_dataSPtr.get() : *m_dataPtr;
      }

      /** Check for attribute
       * @return true if the structure is just a value
       */
      bool isValue() const;

      /** Access to simple content
       * @return string that is the content of the structure
       *
       * For instance when the json structure contains an array of values (ptree only works with strings) which
       * one retrieved via @c getList, then the values in the vector<@c DataStructure> can be accessed using getValue
       */
      std::string getValue() const;  // this will be removed in the next MR (TrigSignatureMoniMT still depends on it)

      template<class T>
      T getValue() const {
         return data().get_value<T>();
      }

      /** Check for attribute
       * @param key The path to the attribute name, relative to the current one in form "path.to.child"
       * @return true if path @c key exists and is an attribute
       */
      bool hasAttribute(const std::string & key) const;

      /** Check if child exists
       * @param path The path to the child, relative to the current one in form "path.to.child"
       * @return true if path exists
       */
      bool hasChild(const std::string & path) const;

      /** Access to simple attribute
       * @param key The path to the attribute name, relative to the current one in form "path.to.child"
       */
      std::string operator[](const std::string & key) const;

      /** Access to simple attribute
       * @param key The path to the attribute name, relative to the current one in form "path.to.child"
       * @param ignoreIfMissing Controls the behavior in case of missing configuration child
       */
      template<class T>
      T getAttribute(const std::string & key, bool ignoreIfMissing = false, const T & def = T()) const {
         const auto & obj = data().get_child_optional(key);
         if( !obj ) {
            if( ignoreIfMissing ) {
               return def;
            } else {
               throw std::runtime_error(className() + "#" + name() + ": structure '" + key + "' does not exist" );
            }
         }
         return obj.get().get_value<T>();
      }

      const std::string & getAttribute(const std::string & key, bool ignoreIfMissing = false, const std::string & def = "") const;

      /** Access to array structure
       * @param pathToChild The path to the configuration child, relative to the current one in form "path.to.child"
       * @param ignoreIfMissing Controls the behavior in case of missing configuration child
       *
       * In case the child is missing and @c ignoreIfMissing is set to @c true, and empty vector 
       * will be returned. Otherwise a runtime exception will be thrown. 
       */
      std::vector<DataStructure> getList(const std::string & pathToChild, bool ignoreIfMissing = false) const;

      /** Access to configuration object 
       * @param pathToChild The path to the configuration child, relative to the current one
       * @param ignoreIfMissing Controls the behavior in case of missing configuration child
       *
       * In case the child is missing and @c ignoreIfMissing is set to @c true, an uninitialized 
       * @c DataStructure will be returned. Otherwise a runtime exception will be thrown. 
       *@code
       *bool ignoreIfMissing = true;
       *if( const DataStructure & child = ds.getObject("path.to.child", ignoreIfMissing) ) {
       *   cout << child["key"] << endl;
       *}
       *@endcode
       **/
      DataStructure getObject(const std::string & pathToChild, bool ignoreIfMissing = false) const;


      /** Access to the keys of an DataStructure which presents a dictionary 
       *
       * In case the DataStructure is a list or a simple attribute, an empty vector is returned
       **/
      std::vector<std::string> getKeys() const;


      /** Access to initialized state */
      explicit operator bool() const { return m_initialized; }
      bool isValid() const { return m_initialized; }
      bool isInitialized() const { return m_initialized; }

      /** Check if children exist */
      bool empty() const { return data().empty(); }

      /* Print this object including children
       * @param os The output stream
       */
      void printRaw(std::ostream & os = std::cout) const;

      /* Print this object including children
       * @param os The output stream
       */
      virtual void print(std::ostream & os = std::cout) const;

      /** Static function to print a @c ptree object
       * @param key The key of this data as found in the parent structure
       * @param data The ptree to print
       * @param level The substruture level used to indent the output
       * @param os The output stream
       */
      static void printElement(const std::string& key, 
                               const ptree & data,
                               uint level = 0,
                               std::ostream & os = std::cout);

      bool ownsData() const {
         return (bool)m_dataSPtr;
      }

   protected:

      /** Update the internal data after modification of the data object
       * 
       * to be implemented by the derived class
       */
      virtual void update() {};

      bool m_initialized { false }; //!< if initialized, the underlying ptree is has been assigned to (can be empty)

      std::shared_ptr<ptree> m_dataSPtr { nullptr }; // used when owning the tree
      const ptree * m_dataPtr { nullptr }; // used when not owning the tree

      std::string m_name{""}; // most objects are named
   
   };

}

#ifndef TRIGCONF_STANDALONE

#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF( TrigConf::DataStructure , 98904516 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrigConf::DataStructure , 265887802 );

#endif


#endif
