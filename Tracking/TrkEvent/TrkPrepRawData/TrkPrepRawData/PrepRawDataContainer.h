/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawDataContainer.h
//   Header file for class PrepRawDataContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Container for RIO  collections for InnerDetector and Muons
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_PREPRAWDATACONTAINER_H
#define TRKPREPRAWDATA_PREPRAWDATACONTAINER_H

// Base classes
#include "EventContainers/IdentifiableContainer.h"

namespace Trk{

template<class CollectionT>
class PrepRawDataContainer 
    : public IdentifiableContainer<CollectionT>{
    
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    public:
    
    /** Constructor with parameters:*/
    PrepRawDataContainer(unsigned int max);
    
    /** Constructor with External Cache*/
    PrepRawDataContainer(EventContainers::IdentifiableCache<CollectionT>*);

    /**default ctor - for POOL only*/
    PrepRawDataContainer();
    
    /** Destructor:*/
    virtual ~PrepRawDataContainer();
    
    /** return class ID */
    static const CLID& classID();
    
    /** return class ID */
    virtual const CLID& clID() const;
    
    private:
    
    PrepRawDataContainer(const PrepRawDataContainer&);
    PrepRawDataContainer &operator=(const PrepRawDataContainer&);

};
// member functions that use Collection T
#include"TrkPrepRawData/PrepRawDataContainer.icc"

}

#endif // TRKPREPRAWDATA_PREPRAWDATACONTAINER_H

