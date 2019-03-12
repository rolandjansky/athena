/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_DATASTRUCTURE
#define TRIGCONFDATA_DATASTRUCTURE

/**
 * @file TrigConfData/DataStructure.h
 * @author J. Stelzer
 * @date Feb 2019
 * @brief Base class for Trigger configuration data and wrapper around underlying representation
 */

#include <iostream>
#include <vector>
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

      /** Constructor initialized with configuration data 
       * @param data Reference to the data container 
       */
      DataStructure(const ptree &);

      /** Destructor */
      virtual ~DataStructure();

      /** @brief Setting the configuration data */
      void setData(const ptree & data);

      /** Clearing the configuration data
       * 
       * leads to an uninitialized object
       */
      void clear();

      /** Access to the underlying data, if needed */ 
      const ptree & data() const { return m_data; }

      /** Access to simple attribute */ 
      std::string operator[](const std::string & key) const;

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

      /** Access to initialized state */
      explicit operator bool() const { return m_initialized; }

      /** Check if children exist */
      bool empty() const { return m_data.empty(); }

      /* Print this object including children
       * @param os The output stream
       */
      void print(std::ostream & os = std::cout) const;

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

   protected:

      bool m_initialized { false }; //!< if initialized, the underlying ptree is valid (can be empty)

      ptree m_data; //!< object holding the configuration data
   
   };

}


#endif
