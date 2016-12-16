/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JobOptionsCatalogue.h,v 1.1 2007-07-29 13:25:43 stelzer Exp $
#ifndef GAUDISVC_JOBOPTIONSCATALOGUE_H
#define GAUDISVC_JOBOPTIONSCATALOGUE_H

#include <string>
#include <vector>

#include "GaudiKernel/StatusCode.h"
#include "TrigConfigSvc/JobOptionsList.h"

class MsgStream;

class Property;

namespace TrigConf {

  /** @brief Collection of JobOptionsList 's, one for each algorithm*/
  class JobOptionsCatalogue {
  public:

    /**@brief default constructor*/
    JobOptionsCatalogue() {}

    /**@brief destructor*/
    virtual ~JobOptionsCatalogue();

    /**@brief add property to algorithm
     *
     * @param[in] myAlg name of the algorithm
     *
     * @param[in] myOpt property to be added
     */
    virtual void addOption( const std::string& myAlg, const Property* const& myOpt );

    /**@brief retrieve all properties of an algorithm
     *
     * @param[in] myAlg name of the algorithm
     *
     * @param[out] myOpts reference to a property list that is filled
     *
     * @returns StatusCode about the success of the operation (success if it found the algorithm)
     */
    virtual StatusCode optionsOf( const std::string& myAlg,
                                  std::vector<const Property*>*& myOpts) const;

    /**@brief resolves all properties
     *
     * @returns StatusCode if no unresolved properties were found
     */
    virtual StatusCode resolveOptions(MsgStream& msgStream);

    /**@brief resolves all properties
     *
     * @returns StatusCode if no unresolved properties were found
     */
    virtual void printUnresolvedOptions(MsgStream& msgStream);

    /**@brief creates vector of all algorithm names
     *
     * @returns vector of all algorithm names 
     */
    virtual std::vector<std::string> getClients() const;

  private:
    typedef std::vector<JobOptionsList*>::iterator  algorithmsIterator;

    /**@brief Lookup of a property
     *
     * @param which algorithm-property name in the form 'algorithm.property'
     *
     * @returns Property or 0 if not found
     */
    const Property*    getOption(const std::string& which);

    /**@brief Sets the list of properties for a specified algorithm
     *
     * @param[in] myAlg name of the algorithm
     *
     * @param[out] myList reference to a JobOptionsList pointer that will be set if the algorithm name was found
     *
     * @returns StatusCode about the success of the operation (success if it found the algorithm)
     */
    StatusCode         findAlgorithm( const std::string& myAlg, JobOptionsList*& myList ) const;

    // data members
    std::vector<JobOptionsList*>  m_algorithmoptions; ///< vector of JobOptionList 's, one for each algorithm
  };

} // end of namespace TrigConf

#endif
