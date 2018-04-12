/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_variablerpseudojetcaltool_header
#define jetsubstructuremomenttools_variablerpseudojetcaltool_header

#include "AsgTools/AsgTool.h"

#include "JetSubStructureMomentTools/IVariableRPseudoJetCalTool.h"

#include "JetInterface/IJetFromPseudojet.h"


#include <string>
#include <map>

class JetContainer;

/**
 * @brief Tool to calibrate fastjet::Pseudojets using ATLAS xAOD::Jet calibration tools
 *
 * This tool can calibrate fastjet::Pseudojets by turning them into ATLAS xAOD::Jets
 * and then obtain calibration factors for those using the default ATLAS JetCalibrationTool.
 *
 * As calibration factors are jet-radius dependent, the tool allows to have various 
 * JetCalibrationTools for calibrating different jet radii.
 *
 * For exclusively reclustered fastjet::Pseudojets, the tool estimates a jet-radius
 * and uses the appropriate JetCalibrationTool to calibrate the jet.
 *
 */
class VariableRPseudoJetCalTool: public asg::AsgTool, public virtual IVariableRPseudoJetCalTool { 
 public: 
   ASG_TOOL_CLASS( VariableRPseudoJetCalTool , IVariableRPseudoJetCalTool )

   VariableRPseudoJetCalTool( const std::string& name );

   virtual StatusCode  initialize() override;

   /** 
    * @brief Calibrate a fastjet::Pseudojet using the ATLAS xAOD::Jet JetCalibrationTool
    *
    * @param jet input jet to be calibrated
    * @param R jet clustering radius, if R<=0, estimate jet Radius from the constituent cluster separation
    * 
    */
   virtual void calibratePseudoJet( 
       fastjet::PseudoJet &jet, double R=0 ) const override;

   /** 
    * @brief Calibrate vector of fastjet::Pseudojets using the ATLAS xAOD::Jet JetCalibrationTool
    *
    * @param jetVec input jet vector of jets to be calibrated
    * @param R jet clustering radius, if R<=0, estimate jet Radius from the constituent cluster separation
    * 
    */
   virtual void calibratePseudoJets( 
       std::vector<fastjet::PseudoJet> &jetVec, double R=0 ) const override;

 private: 
   bool                                 m_isData;
   std::string                          m_jetAlgorithm;
   std::string                          m_jetInputs;
   std::vector< int >                   m_jetRadiiVec;
   std::string                          m_configFile;
   std::string                          m_calibSequence;
   std::string                          m_calToolsNamePrefix;
   /// Property: exclusive subjets R-parameter mode
   /// if true:  Rmax mode: max. distance of constituent from jet axis
   /// if false: Rmerge:    (smallest) R that would yield the same subjet in inclusive clustering
   bool                                 m_exclusiveSubjetsRparam; 
   
   /// Property: JetCalibrationTool selection mode
   /// if 0 -> smalles radius calTool with radius larger than the jet radius R
   /// if 1 -> calTool with radius closest to jet radius R
   int                                  m_calToolSelectionMode;  

  
   std::map<int, ToolHandle<IJetCalibrationTool> > m_varRJetCalibToolsMap;
   ToolHandleArray<IJetCalibrationTool>            m_customVarRJetCalibTools;   

   ToolHandle<IJetFromPseudojet>        m_jetFromPJTool;

   // helper functions
   void fillJetContainer( 
       const std::vector<fastjet::PseudoJet>& in_pjVec, 
       xAOD::JetContainer& out_jetCont) const;
   
   const IJetCalibrationTool* getCalibrator(
       double R, 
       int mode=0) const;
   
   /**
    * @brief Try to estimate jet radius for exclusively clustered jet
    *
    * @param pj input jet
    *
    * The tool tries to estimate a jet radius from the associated clusters
    * Two modes exist that can be set using the m_exclusiveSubjetsRparam variable.
    * Either, Rmax mode to choose max. distance of constituent from jet axis, or
    * Rmerge to choose smallest R that would yield the same subjet in inclusive clustering
    *
    */
   double findJetRadiusParameter( 
       const fastjet::PseudoJet& pj ) const;

}; 

#endif 
