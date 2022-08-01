/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   MuonLRTMergingAlg
//   author Sagar Addepalli: sagara17@SPAMNOT_CERN.CH
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Muon merger algorirthm is a wrapper algorithm around MuonMerger 
// to be used downstream for combining LRTmuons and standard muons 
///////////////////////////////////////////////////////////////////
#include "MuonAnalysisAlgorithms/MuonLRTMergingAlg.h"
#include <AsgTools/CurrentContext.h>
#include "xAODMuon/MuonAuxContainer.h"
#include <AsgTools/AsgToolConfig.h>

namespace CP{
    MuonLRTMergingAlg::MuonLRTMergingAlg( const std::string& name, ISvcLocator* svcLoc )
    : EL::AnaAlgorithm( name, svcLoc ){
        //nothing to do here
    }

    StatusCode MuonLRTMergingAlg::initialize() {

        // Greet the user:
        ATH_MSG_INFO( "Initialising" );

        /// initialize the handles
        ATH_CHECK( m_promptMuonLocation.initialize() );
        ATH_CHECK( m_lrtMuonLocation.initialize() );
        ATH_CHECK( m_outMuonLocation.initialize() );

        /// if the tool is not user-set, configure the automatic instance via our overlap flag
        if (m_overlapRemovalTool.empty()){
            asg::AsgToolConfig config("CP::MuonLRTOverlapRemovalTool/MuonLRTOverlapRemovalTool");
            ATH_CHECK(config.setProperty("overlapStrategy",m_ORstrategy.value()));
            ATH_CHECK(config.makePrivateTool(m_overlapRemovalTool));
        }

        // Retrieve the tools
        ATH_CHECK( m_overlapRemovalTool.retrieve() );
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    StatusCode MuonLRTMergingAlg::execute()  {

        const EventContext& ctx = Gaudi::Hive::currentContext();

        // retrieve the merged muon container        
        std::unique_ptr<xAOD::MuonContainer> outputCol;
        std::unique_ptr<xAOD::MuonAuxContainer> outputAuxCol;

        if(!m_createViewCollection) {
            outputCol = std::make_unique<xAOD::MuonContainer>();
            outputAuxCol = std::make_unique<xAOD::MuonAuxContainer>();
            outputCol->setStore(outputAuxCol.get());
        }
        else {
            outputCol = std::make_unique<xAOD::MuonContainer>(SG::VIEW_ELEMENTS);
        }

        /// retrieve muons from StoreGate
        SG::ReadHandle<xAOD::MuonContainer> promptCol (m_promptMuonLocation, ctx);
        SG::ReadHandle<xAOD::MuonContainer> lrtCol (m_lrtMuonLocation, ctx);
        if (!promptCol.isValid()) {
            ATH_MSG_FATAL("Unable to retrieve xAOD::MuonContainer, \"" << m_promptMuonLocation << "\", cannot run the LRT muon merger!");
            return StatusCode::FAILURE;
        }
        if (!lrtCol.isValid()) {
            ATH_MSG_FATAL("Unable to retrieve xAOD::MuonContainer, \"" << m_lrtMuonLocation << "\", cannot run the LRT muon merger!");
            return StatusCode::FAILURE;
        }
        outputCol->reserve(promptCol->size() + lrtCol->size());


        // Check and resolve overlaps 
        std::vector<bool> writePromptMuon;
        std::vector<bool> writeLRTMuon;
        m_overlapRemovalTool->checkOverlap(*promptCol, *lrtCol, writePromptMuon, writeLRTMuon);

        // Decorate the muons with their locations.
        static SG::AuxElement::Decorator<char> isLRT("isLRT"); //0 if prompt, 1 if LRT        
        for (const xAOD::Muon* mu : *promptCol) isLRT(*mu) = 0;
        for (const xAOD::Muon* mu : *lrtCol) isLRT(*mu) = 1;

        // and merge
        ATH_CHECK(mergeMuon(*promptCol, writePromptMuon, outputCol.get()));
        ATH_CHECK(mergeMuon(*lrtCol, writeLRTMuon, outputCol.get()));
        SG::WriteHandle<xAOD::MuonContainer> h_write(m_outMuonLocation, ctx);
        if (m_createViewCollection){
            ATH_CHECK(h_write.record(std::move(outputCol)));
        }
        else{  
            ATH_CHECK(h_write.record(std::move(outputCol), std::move(outputAuxCol))); 
        }

        ATH_MSG_DEBUG("Done initializing!");  
        return StatusCode::SUCCESS;

    }
  ///////////////////////////////////////////////////////////////////
  // Merge muon collections and remove duplicates
  ///////////////////////////////////////////////////////////////////

    StatusCode MuonLRTMergingAlg::mergeMuon(const xAOD::MuonContainer & muonCol,
                             const std::vector<bool> & writeMuon,
                                xAOD::MuonContainer* outputCol) const{
        // loop over muons, accept them and add them into association tool
        if(muonCol.empty()) {return StatusCode::SUCCESS;}
        static SG::AuxElement::Decorator<ElementLink<xAOD::MuonContainer>> originalMuonLink("originalMuonLink");
        for(const xAOD::Muon* muon : muonCol){
            // add muon into output
            if (writeMuon.at(muon->index())){
                xAOD::Muon* newMuon;
                if (!m_createViewCollection) {
                    newMuon = new xAOD::Muon(*muon);
                    ElementLink<xAOD::MuonContainer> myLink;
                    myLink.toIndexedElement(muonCol, muon->index());
                    originalMuonLink(*newMuon) = myLink;
                }
                else {
                    newMuon = const_cast<xAOD::Muon*>(muon);
                }
                outputCol->push_back(newMuon);
            }
        }
        return StatusCode::SUCCESS;
    }
}
