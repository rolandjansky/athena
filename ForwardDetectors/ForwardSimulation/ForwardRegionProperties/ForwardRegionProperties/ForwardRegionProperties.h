/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForwardRegionProperties_H
#define ForwardRegionProperties_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ForwardRegionGeoModel/IForwardRegionProperties.h"
#include "CLHEP/Geometry/Point3D.h"

class ForwardRegionProperties : public extends<AthAlgTool, IForwardRegionProperties>
{
public:
    ForwardRegionProperties( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~ForwardRegionProperties();

    virtual PFWDMg_CONFIGURATION getConf() override final;

    void getMagTransforms(std::string, int, HepGeom::Point3D<double>&, HepGeom::Point3D<double>&, double&, bool twiss = false) override final;
    void getMagTransforms(std::string, int, HepGeom::Point3D<double>&, HepGeom::Point3D<double>&, bool twiss = false) override final;

private:
    // property configuration
    FWDMg_CONFIGURATION m_Config;
};

#endif // ForwardRegionProperties_H
