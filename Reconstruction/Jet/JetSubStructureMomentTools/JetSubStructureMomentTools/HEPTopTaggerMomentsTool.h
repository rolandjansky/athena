/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_heptoptaggermomentstool_header
#define jetsubstructuremomenttools_heptoptaggermomentstool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

#include "JetSubStructureMomentTools/IVariableRPseudoJetCalTool.h"


#include "JetInterface/IJetFromPseudojet.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include <string>

class HEPTopTaggerImpl;

class HEPTopTaggerMomentsTool :
  public JetSubStructureMomentToolsBase
{
  ASG_TOOL_CLASS(HEPTopTaggerMomentsTool, IJetModifier)

  public:
    // Constructor and destructor
    HEPTopTaggerMomentsTool(std::string name);
    ~HEPTopTaggerMomentsTool();
 
    // set one of the predefined HTT configurations
    StatusCode setPredefConfiguration( std::string predefConfName="def" );

    StatusCode initialize() override;

    // modify single jet
    int modifyJet(xAOD::Jet &jet) const override;

    // modify whole jet container (call modifyJet on each jet)
    int modify(xAOD::JetContainer& jets) const override;

    void print() const override;

    HEPTopTaggerImpl* getHEPTopTaggerImpl( );

  private:
    // tool configurable attributes
    ToolHandle<IVariableRPseudoJetCalTool>  m_variableRPseudoJetCalTool;
    ToolHandle<IJetFromPseudojet>           m_jetFromPseudojetTool;
    
    // misc
    std::string                             m_HTTAttrPrefix;
    bool                                    m_storeFullHTTResult;
    bool                                    m_storeUntaggedHTTCandidates;
    std::string                             m_predefHTTConf;
    bool                                    m_calibrateHTTSubjets;

    std::string                             m_runOnLinkedJetName;

    // HTT implementation
    HEPTopTaggerImpl*                       m_HEPTopTaggerImpl;

    // helper functions
    std::string getHTTAttributeName( const char* variableName ) const;
};

#endif
