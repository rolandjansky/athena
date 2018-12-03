/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ILArCalibCalculatorSvc.h
// Prepared in 2016 based on VCalibrationCalculator from Bill Seligman

#ifndef __ILARCALIBCALCULATORSVC_H__
#define __ILARCALIBCALCULATORSVC_H__

#include "GaudiKernel/IService.h"

#include "globals.hh"

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArG4EnumDefs.h"


// Forward declaractions:
class G4Step;
class StatusCode;

class ILArCalibCalculatorSvc: virtual public IService {
public:

  ILArCalibCalculatorSvc(){};

  DeclareInterfaceID(ILArCalibCalculatorSvc,1,0);

  virtual ~ILArCalibCalculatorSvc() {};


  virtual G4bool Process (const G4Step* step, LArG4Identifier & identifier,
                            std::vector<G4double> & energies,
                            const LArG4::eCalculatorProcessing process = LArG4::kEnergyAndID) const = 0;



};

#endif
