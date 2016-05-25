/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUBUILDERTOOL_H
#define TAUREC_TAUBUILDERTOOL_H

#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/ITauToolExecBase.h"
#include "AsgTools/ToolHandleArray.h"

/**
 * @brief Implementation of tauRec/TauBuilder as a tool.
 * 
 * @author Justin Griffiths
 * @authors (tauRec/TauBuilder.h)    Srini Rajagopalan, Michael Heldmann, Lukasz Janyst, Anna Kaczmarska, Felix Friedrich
 *                                                                              
 */

class TauBuilderTool : public asg::AsgTool, virtual public ITauToolExecBase {
 public:

  ASG_TOOL_CLASS1( TauBuilderTool, ITauToolExecBase )
    
    TauBuilderTool(const std::string& type);
  ~TauBuilderTool();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:
  std :: string                 m_tauContainerName;             //!< tau output container
  std :: string                 m_tauAuxContainerName;             //!< tau output aux store container
  std::string m_tauTrackContainerName;
  std::string m_tauTrackAuxContainerName;
  std::string m_seedContainerName;            //!< seed input container
  double m_maxEta; //!< only build taus with eta_seed < m_maxeta
  double m_minPt;  //!< only build taus with pt_seed > m_minpt

  /** switch to create tau containers, 
   * if false tau containers will be updated only 
   */ 
  bool m_doCreateTauContainers;               
  ToolHandleArray<ITauToolBase>  m_tools;
  TauEventData m_data;

 public:

};

#endif //TAUREC_TAUBUILDERTOOL_H
