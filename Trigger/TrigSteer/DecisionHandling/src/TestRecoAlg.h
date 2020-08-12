/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTRECOALG_H
#define TRIGUPGRADETEST_TESTRECOALG_H 1


#include <string>
#include <utility>
#include "xAODTrigger/TrigCompositeContainer.h"

#include "AthenaBaseComps/AthAlgorithm.h"



namespace HLTTest {

  /**
   * @class TestRecoAlg 
   * @brief Pseudo reco alg, reads input file formatted as follows:
   * prop1:value, prop2:value; prop1:value, prop2:value; ... 
   * and created TrigComposites with prop1 & prop2 details set to corresponding values.
   * one line is one event
   **/

  class TestRecoAlg
    : public ::AthAlgorithm
  { 
  public: 
    TestRecoAlg( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TestRecoAlg(); 

    StatusCode  initialize() override;
    StatusCode  execute() override;
    StatusCode  finalize() override;

  private: 
    TestRecoAlg();

    typedef std::pair< std::string, float > Prop_t;
    typedef std::vector< Prop_t > Obj_t;
    typedef std::vector< Obj_t > Event_t;
    std::vector< Event_t > m_data;
    StringProperty m_fileName { this, "FileName", "Input file with fake objects"};
    
    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_output { this, "Output",   "Output collection name" };
    SG::ReadHandleKey<xAOD::TrigCompositeContainer>  m_input  { this, "Input",    "Input collection name" };

    
  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTRECOALG_H
