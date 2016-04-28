/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBBEAMCHAMBERSCONT_H
#define LARTBBEAMCHAMBERSCONT_H

#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "LArTBEvent/LArTBBeamChambers.h"


/** @class LArTBBeamChambersCont LArTBBeamChambersCont.h 
    "LArTBEvent/LArTBBeamChambersCont.h"
    
    @author A. Abdesselam and P. Ghez
*/
class LArTBBeamChambersCont : public DataVector<LArTBBeamChambers> {
 public:
  /** Constructor */
  LArTBBeamChambersCont() : DataVector<LArTBBeamChambers>() { }
  
  virtual operator std::string();
  
  /** destructor  */
  virtual ~LArTBBeamChambersCont() { } 

} ;

CLASS_DEF(LArTBBeamChambersCont,2781,0)
#endif



