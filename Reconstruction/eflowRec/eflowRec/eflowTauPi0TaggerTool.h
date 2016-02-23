/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTAUPI0TAGGERTOOL_H
#define EFLOWTAUPI0TAGGERTOOL_H

/********************************************************************

NAME:     eflowTauPi0TaggerTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  MAY 2013

********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "eflowRec/eflowTauBaseAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class eflowTauObject;

namespace TMVA{
  class Reader;
}
class StoreGateSvc;

//C++
#include <vector>
#include <string>

static const InterfaceID IID_eflowTauPi0TaggerTool("eflowTauPi0TaggerTool", 1, 0);

class eflowTauPi0TaggerTool : virtual public eflowTauBaseAlgTool, public AthAlgTool {

  public:
  
  eflowTauPi0TaggerTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowTauPi0TaggerTool();

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode execute(const eflowTauObject& eflowTauObject);
  StatusCode finalize();

 private:

  /** TMVA Tool to run Pi0 BDT */
  TMVA::Reader* m_Reader;
  /** Name of XML file to use in Pi0 BDT */
  std::string m_theXMLFile;

  /** The input variables for the Pi0 BDT */
  float m_emFracEnhanced;
  float m_eng_frac_core;
  float m_first_eng_dens;
  float m_centerLambda;
  float m_secondR;
  float m_deltaAlpha;
  float m_HotStripFrac;
  float m_ThreeCellStripFrac;
  float m_eta;

  /** Pointer to Storegate */
  StoreGateSvc* m_storeGate;

  static const double DETA_EM1;
  static const double DPHI_EM1;

};

inline const InterfaceID& eflowTauPi0TaggerTool::interfaceID()
{ 
  return IID_eflowTauPi0TaggerTool; 
}


#endif
