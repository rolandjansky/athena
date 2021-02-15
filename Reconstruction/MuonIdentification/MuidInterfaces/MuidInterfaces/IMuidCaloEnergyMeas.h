/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuidCaloEnergyMeas
//  interface to access the energy deposited by a muon in the
//  calorimeters. The energy deposit is estimated according to
//  the active measurements in the traversed calorimeter cells.
//
///////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDCALOENERGYMEAS_H
#define MUIDINTERFACES_IMUIDCALOENERGYMEAS_H

#include "GaudiKernel/IAlgTool.h"

namespace Rec
{

/** Interface ID for IMuidCaloEnergyMeas*/  
static const InterfaceID IID_IMuidCaloEnergyMeas("IMuidCaloEnergyMeas", 1, 0);
  
/**@class IMuidCaloEnergyMeas

Base class for MuidCaloEnergyMeas AlgTool
     
     
@author Alan.Poppleton@cern.ch
*/

class CaloMeas;
    
class IMuidCaloEnergyMeas : virtual public IAlgTool
{
public:

    /**Virtual destructor*/
    virtual ~IMuidCaloEnergyMeas(){}
       
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_IMuidCaloEnergyMeas; }

    /**IMuidCaloEnergyMeas interface:
       to get the muon energy loss measurement from the calorimeter,
       knowing the track intersection at the em and had cals*/
    virtual CaloMeas*		energyMeasurement (double etaEM,
						   double phiEM,
						   double etaHad,
						   double phiHad) const = 0;
};

}	// end of namespace

#endif // MUIDINTERFACES_IMUIDCALOENERGYMEAS_H


