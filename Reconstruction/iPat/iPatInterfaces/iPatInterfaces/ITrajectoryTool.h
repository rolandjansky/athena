/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrajectoryTool.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRAJECTORYTOOL_H
# define IPATINTERFACES_ITRAJECTORYTOOL_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaKernel/IOVSvcDefs.h" 
#include "GaudiKernel/IAlgTool.h"

/** Interface ID for ITrajectoryTool*/  
static const InterfaceID IID_ITrajectoryTool("ITrajectoryTool", 1, 0);
  
/**@class ITrajectoryTool

Abstract base class for TrajectoryTool AlgTool
      
@author Alan.Poppleton@cern.ch
*/

class FastField;
class PerigeeParameters;
class TrackParameters;
#include "AtlasHepMC/GenParticle_fwd.h"
namespace iPat
{
    
class ITrajectoryTool: virtual public IAlgTool
{
public:
 
    /**Virtual destructor*/
    virtual ~ITrajectoryTool() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITrajectoryTool; }

    /**ITrajectoryTool interface:
       central field value in MagFieldAthenaSvc units (kGauss) */
    virtual double		centralField (void) const = 0;
     
    /**ITrajectoryTool interface:
       access the field parametrization */
    virtual FastField*		fastField (void) const = 0;
   
    /**ITrajectoryTool interface:
       query validity of parametrization within extrapolation range */
    virtual bool		isValid (Amg::Vector3D startPosition,
					 Amg::Vector3D endPosition) const = 0;
    
    /**ITrajectoryTool interface: 
       query field on */
    virtual double		lightSpeed (void) const = 0;
              
    /**ITrajectoryTool interface: 
       query field on */
    virtual bool		parametrizedSolenoid (void) const = 0;
           
    /**ITrajectoryTool interface: 
       query field on */
    virtual bool		solenoidOn (void) const = 0;
           
};

} // end of namespace


#endif // IPATINTERFACES_ITRAJECTORYTOOL_H

