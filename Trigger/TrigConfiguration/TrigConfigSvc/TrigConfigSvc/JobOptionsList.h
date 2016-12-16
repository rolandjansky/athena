/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JobOptionsList.h: interface for the JobOptionsList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(JOBOPTIONSLIST_H_INCLUDED_)
#define JOBOPTIONSLIST_H_INCLUDED_

#include <string>
#include <vector>
//#include "GaudiKernel/Property.h"

class Property;

namespace TrigConf {

  /**@brief list of all properties of a single algorithm/component*/
  class JobOptionsList {
  public:

    /**@brief constructor with data
     *
     * @param myAlg name of the algorithm
     *
     * @param myOpt first property
     */
    JobOptionsList( const std::string& myAlg, const Property* const& myOpt );

    /**@brief destructor
     *
     * All properties are deleted
     */
    virtual ~JobOptionsList();

    /**@brief add property to the list
     *
     * @param myOpt reference to the Property pointer to be added
     *
     * JobOptionsList then assumes ownership of the pointer
     */
    virtual void addOption( const Property* const& myOpt );

    /**@brief accessor to the name of the algorithm
     * 
     * @returns name of algorithm
     */
    virtual const std::string& algorithmName() const;

    /**@brief accessor to the vector of properties
     * 
     * @returns pointer to the property vector
     */
    virtual const std::vector<const Property*>* properties() const;
    
  private:
    /**@brief default constructor (private)*/
    JobOptionsList() {}
    
    std::vector<const Property*> m_properties;    ///< vector of job properties
    std::string                  m_algorithmName; ///< name of the algorithm
  };

} // end of namespace TrigConf

#endif // !defined(JOBOPTIONSLIST_H_INCLUDED_)
