/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuidCaloEnergyParam
//  interface to access the energy deposited by a muon in the
//  calorimeters. The energy is parametrized according to
//  trackParameters given at the calo mid-surface.
//
///////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDCALOENERGYPARAM_H
#define MUIDINTERFACES_IMUIDCALOENERGYPARAM_H

#include "GaudiKernel/IAlgTool.h"

class CaloEnergy;
namespace Rec
{

/** Interface ID for IMuidCaloEnergyParam*/  
static const InterfaceID IID_IMuidCaloEnergyParam("IMuidCaloEnergyParam", 1, 0);
  
/**@class IMuidCaloEnergyParam

Base class for MuidCaloEnergyParam AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/
class IMuidCaloEnergyParam : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuidCaloEnergyParam(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IMuidCaloEnergyParam; }

    /**IMuidCaloEnergyParam interface:
       to get the mean muon energy loss in the calorimeter
       (with asymmetric errors describing the whole distribution) */
    virtual CaloEnergy*		meanParametrizedEnergy(double trackMomentum,
						       double eta,
						       double phi) const = 0;

    /**IMuidCaloEnergyParam interface:
       to get the most probable (peak value) energy loss in the calorimeter
       (with asymmetric errors describing the whole distribution)  */
    virtual CaloEnergy*		mopParametrizedEnergy(double trackMomentum,
						      double eta,
						      double phi) const = 0;
    
    /**IMuidCaloEnergyParam interface:
       to get the most probable (peak value) energy loss in the calorimeter
       (with asymmetric errors describing 2 sigma around the peak region)  */
    virtual CaloEnergy*		mopPeakEnergy(double trackMomentum,
					      double eta,
					      double phi) const = 0;
    
    /**IMuidCaloEnergyParam interface:
       to get the most probable (peak value) energy loss and error in the calorimeter */
    virtual CaloEnergy*		mopDepositedEnergy(double trackMomentum,
						   double eta,
						   double phi) const = 0;
    
    /**IMuidCaloEnergyParam interface:
       returns the fraction of inert material between InDet and MS */
    virtual double		x0mapInertMaterial(double eta) const = 0;
    
    /**IMuidCaloEnergyParam interface:
       returns the fraction of em calorimeter inert material between InDet and MS */
    virtual double		x0mapEmMaterial(double eta) const = 0;
    
    /**IMuidCaloEnergyParam interface:
       returns the fraction of hec calorimeter inert material between InDet and MS */
    virtual double		x0mapHecMaterial(double eta) const = 0;
  
    /**IMuidCaloEnergyParam interface: depth of calo compartment */
    virtual double		caloCompartmentDepth(int,int) const = 0;
      
    /**IMuidCaloEnergyParam interface:
       fraction of the full mop deposition corresponding to the em */
    virtual double		emMopFraction(double eta) const = 0;
         
    /**IMuidCaloEnergyParam interface:
       fraction of the full mop deposition corresponding to the em presampler + compartment #1 */
    virtual double		em1MopFraction(double eta) const = 0;
};

}	// end of namespace

#endif // MUIDINTERFACES_IMUIDCALOENERGYPARAM_H


