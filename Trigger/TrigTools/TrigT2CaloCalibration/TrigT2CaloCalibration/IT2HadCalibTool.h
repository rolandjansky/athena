/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     IT2HadCalibTool.h
// PACKAGE:  Trigger/TrigTools/TrigT2CaloCalibration
//
// AUTHORS:   C. Osuna <Carlos.Osuna@ifae.es>
//            P. Conde <Patricia,Conde.Muino@cern.ch>
//
//   Interface for T2Calibration tools.
//
// ********************************************************************

#ifndef TRIGT2CALOCALIBRATION_IT2HADCALIBTOOL_H
#define TRIGT2CALOCALIBRATION_IT2HADCALIBTOOL_H

#include "GaudiKernel/IAlgTool.h"

class IT2HadCalibTool : virtual public IAlgTool {
    public:
        /**interface ID*/
        static const InterfaceID& interfaceID();
        
        /**virtual method to be defined in the inherited class for*/
        virtual double c_energy(double EMenergy,double HADenergy,double eta) = 0;
        virtual double c_febenergy(double EMenergy,double HADenergy,double eta) = 0;
};

#endif
