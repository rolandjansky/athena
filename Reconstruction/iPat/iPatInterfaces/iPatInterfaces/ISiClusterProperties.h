/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISiClusterProperties.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ISICLUSTERPROPERTIES_H
# define IPATINTERFACES_ISICLUSTERPROPERTIES_H

#include <utility>
#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "iPatTrack/HitStatus.h"

class HitOnTrack;
class SiliconDetector;
namespace InDet
{
    class SiCluster;
}

/** Interface ID for ISiClusterProperties*/  
static const InterfaceID IID_ISiClusterProperties("ISiClusterProperties", 1, 0);
  
/**@class ISiClusterProperties

Abstract base class for SiClusterProperties AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class ISiClusterProperties: virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~ISiClusterProperties() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ISiClusterProperties; }

    /**ISiClusterProperties interface:
       conservative cluster errors for use during pattern recognition  */
    virtual std::pair<double,double>	broadErrors (const InDet::SiCluster*	cluster,
						     const SiliconDetector*	detector,
						     const Amg::Vector3D&	trackIntersect) = 0;

    /**ISiClusterProperties interface:
       precise cluster position and errors including track-dependent corrections  */
    virtual HitOnTrack*			hitOnTrack (const InDet::SiCluster*	cluster,
						    const SiliconDetector*	detector,
						    const Trk::TrackParameters*	parameters) = 0;
};


#endif // IPATINTERFACES_ISICLUSTERPROPERTIES_H

