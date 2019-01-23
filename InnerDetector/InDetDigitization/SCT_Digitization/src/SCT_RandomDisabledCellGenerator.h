/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_RandomDisabledCellGenerator.h
//   Header file for class SCT_RandomDisabledCellGenerator 
///////////////////////////////////////////////////////////////////
// Class to randomly disable cells for SCT
///////////////////////////////////////////////////////////////////
// Version 1.0 25/07/2007 Kondo Gnanvo
//   - Randomly select a cell and modify its flag by using the pointer to 
//     a SiHelper function.
//   - Inherits from ISiChargedDiodeProcessor, as SiPreDiodeProcessor
//     has been discontinued
// This can be used for disabling, disconnecting, and flagging bad_tot
///////////////////////////////////////////////////////////////////

#ifndef SCT_DIGITIZATION_SCT_RANDOMDISABLEDCELLGENERATOR_H
#define SCT_DIGITIZATION_SCT_RANDOMDISABLEDCELLGENERATOR_H

//Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Digitization/ISCT_RandomDisabledCellGenerator.h"

class SiChargedDiode;
class SiChargedDiodeCollection;

namespace CLHEP {
  class HepRandomEngine;
}

class SCT_RandomDisabledCellGenerator : public extends<AthAlgTool, ISCT_RandomDisabledCellGenerator> {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

 public:

  /**  constructor */
  SCT_RandomDisabledCellGenerator(const std::string& type, const std::string& name, const IInterface* parent) ;

  /** Destructor */
  virtual ~SCT_RandomDisabledCellGenerator() = default;

  /** AlgTool initialize */
  virtual StatusCode initialize();
  /** AlgTool finalize */
  virtual StatusCode finalize();

  virtual void process(SiChargedDiodeCollection& collection, CLHEP::HepRandomEngine * rndmEngine) const;

 private:

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:

  float m_disableProbability;   // probability that a cell is disabled

};

#endif //SCT_DIGITIZATION_SCT_RANDOMDISABLEDCELLGENERATOR_H
