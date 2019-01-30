/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          ModuleEnergy.h  -  description
                             -------------------
    begin                : Tues Sep 4 2007
    email                : Alan.Watson@cern.ch
 ***************************************************************************/


#ifndef MODULEENERGY_H
#define MODULEENERGY_H

#include <map>
#include <vector>
//#include "GaudiKernel/ServiceHandle.h"
#include "xAODTrigL1Calo/JetElement.h"
#include "TrigT1CaloUtils/JetElementKey.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h"
#include "TrigT1Interfaces/Coordinate.h"

/**LVL1 namespace.
This belongs to the TrigT1Calo  simulation.
*/
namespace LVL1 {

/**
This is an internal class, used in the Energy trigger. <P>
The ModuleEnergy:<br>
  - identifies JetElements belonging to this particular module and uses them
    to form ET sums<br>
  - applies thresholds to JetElements on input to ETmiss and ETsum chains <br>
  - converts JetElements to Ex, Ey using coefficients from TrigConfigSvc <br>
  - Ex, Ey, Et all stored with values we would obtain after quad-linear
    encoding<br>

  */

class ModuleEnergy {
public: 
  
  ModuleEnergy(const xAOD::JetElementMap_t* JEContainer,  unsigned int crate, unsigned int module,
               int JEThresholdEtSum, int JEThresholdEtMiss, const std::map<int, int>* TEMasks = 0, int slice = -1);

  ModuleEnergy(unsigned int crate, unsigned int module, unsigned int et,
               unsigned int ex, unsigned int ey);

  ~ModuleEnergy();
  
  /** which module is this? */
  unsigned int crate() const;
  unsigned int module() const;
  /** return the scalar & vector sums of all JE ETs (i.e sums the energies of up to 32 contained JEs) */
  /** module sums are unsigned. As a convenience, add methods to return signs for this module */
  unsigned int et() const;
  unsigned int ex() const;
  unsigned int ey() const;
  int signX() const;
  int signY() const;
  
private: //atribs
  int m_jetElementThresholdEtSum;
  int m_jetElementThresholdEtMiss;
  unsigned int m_Et;
  unsigned int m_Ex;
  unsigned int m_Ey;
  int m_signX;
  int m_signY;
  unsigned int m_crate;
  unsigned int m_module;
  bool m_debug;
  static const unsigned int m_EtBits = 14;

  void getSinCos(double eta, double phi, int& cosPhi, int& sinPhi);

};

}
#endif

