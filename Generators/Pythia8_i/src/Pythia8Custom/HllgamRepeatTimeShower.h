/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IHLLGAMREPEATTIMESHOWER_H
#define IHLLGAMREPEATTIMESHOWER_H

#include "Pythia8_i/IPythia8Custom.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "Pythia8/Pythia.h"


/** Tool that will repeatedly perform  time showering on the intial photons
 * from a Higgs decay until at least one is offshell */
class HllgamRepeatTimeShower: public AthAlgTool, virtual public IPythia8Custom {
  
  public:
  
  /** AlgTool style constructor */
  HllgamRepeatTimeShower(const std::string&,const std::string&,const IInterface*);
  
  /** Destructor */
  virtual ~HllgamRepeatTimeShower(){};
  
  /** AlgTool initialize method */
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();
  
  /** Update the pythia event*/
  StatusCode ModifyPythiaEvent(Pythia8::Pythia& pythia) const;

  /** Return how much the cross section is modified.
   *  Should only be called once all events have been processed */
  virtual double CrossSectionScaleFactor() const;
  
  private:
  
  mutable unsigned long m_nPass;
  mutable unsigned long m_nVetos;
  
};

#endif
