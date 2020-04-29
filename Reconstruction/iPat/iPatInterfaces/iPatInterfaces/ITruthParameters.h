/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITruthParameters.h, (c) ATLAS iPatRec
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_ITRUTHPARAMETERS_H
# define IPATINTERFACES_ITRUTHPARAMETERS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

/** Interface ID for ITruthParameters*/  
static const InterfaceID IID_ITruthParameters("ITruthParameters", 1, 0);
  
/**@class ITruthParameters

Abstract base class for TruthParameters AlgTool
      
@author Alan.Poppleton@cern.ch
*/

class PerigeeParameters;
class TrackParameters;
#include "AtlasHepMC/GenParticle_fwd.h"
class ITruthParameters: virtual public IAlgTool
{
public:
 
    /**Virtual destructor*/
    virtual ~ITruthParameters() {}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ITruthParameters; }
  
    /**ITruthParameters interface:
       obtain PerigeeParameters corresponding to a simulated particle from its barcode */
    virtual const PerigeeParameters*	perigeeParameters(int barcode,
							  const Amg::Vector3D& vertex) = 0;
     
    /**ITruthParameters interface:
       obtain PerigeeParameters corresponding to a simulated particle from a HepMC GenParticle */
    virtual const PerigeeParameters*	perigeeParameters(const HepMC::GenParticle&	particle,
							  const Amg::Vector3D&		vertex) = 0;
       
    /**ITruthParameters interface: 
       obtain TrackParameters corresponding to a simulated particle from its barcode */
    virtual const TrackParameters*	trackParameters(int barcode) = 0;
           
    /**ITruthParameters interface: 
       obtain TrackParameters corresponding to a simulated particle from a HepMC GenParticle */
    virtual const TrackParameters*	trackParameters(const HepMC::GenParticle& particle) = 0;
};

#endif // IPATINTERFACES_ITRUTHPARAMETERS_H

