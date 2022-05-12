// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


#ifndef ITRIGHTTMAPPINGSVC_H
#define ITRIGHTTMAPPINGSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/IInterface.h"

// Forward declarations
class HTTRegionMap;
class HTTPlaneMap;
class HTTNNMap;


class ITrigHTTMappingSvc: virtual public IService
{
    public:
        DeclareInterfaceID(ITrigHTTMappingSvc, 1, 0);
  
        virtual const HTTPlaneMap* PlaneMap_1st() const = 0;
        virtual const HTTPlaneMap* PlaneMap_2nd() const = 0;
        virtual const HTTRegionMap* RegionMap_1st() const = 0;
        virtual const HTTRegionMap* RegionMap_2nd() const = 0;
        virtual const HTTRegionMap* SubRegionMap() const = 0;
        virtual const HTTNNMap* NNMap() const = 0;
};



#endif   // ITRIGHTTMAPPINGSVC_H
