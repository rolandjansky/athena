/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthMetaDataWriter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all truth particles/vertices which do not pass a user-defined cut

#include "DerivationFrameworkMCTruth/TruthMetaDataWriter.h"
#include "EventInfo/EventStreamInfo.h"
#include "xAODTruth/TruthMetaDataAuxContainer.h"
#include "xAODTruth/TruthMetaData.h"
#include <vector>
#include <string>
#include <numeric>

// Constructor
DerivationFramework::TruthMetaDataWriter::TruthMetaDataWriter(const std::string& t,
                                                                const std::string& n,
                                                                const IInterface* p) :

AthAlgTool(t,n,p),
m_metaStore( "MetaDataStore", n ),
inputMetaStore( "StoreGateSvc/InputMetaDataStore",n)
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty( "MetaObjectName", m_metaName = "TruthMetaData" );
    declareProperty( "MetaDataStore", m_metaStore );
    declareProperty( "WeightNamesMap", m_weightNamesMap );
}

// Destructor
DerivationFramework::TruthMetaDataWriter::~TruthMetaDataWriter() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthMetaDataWriter::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    
    CHECK( m_metaStore.retrieve() );
    // Create an empty truth meta data container:
    xAOD::TruthMetaDataAuxContainer* aux = new xAOD::TruthMetaDataAuxContainer();
    m_tmd = new xAOD::TruthMetaDataContainer();
    m_tmd->setStore( aux );
    
    CHECK( m_metaStore->record( aux, m_metaName + "Aux." ) );
    CHECK( m_metaStore->record( m_tmd, m_metaName ) );

    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TruthMetaDataWriter::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    return StatusCode::SUCCESS;
}

// Selection and collection creation
StatusCode DerivationFramework::TruthMetaDataWriter::addBranches() const
{
   
    

    //The mcChannelNumber is used as a unique identifier for which truth meta data belongs to
    const EventStreamInfo* esi = nullptr;
    CHECK( inputMetaStore->retrieve(esi));
    uint32_t mcChannelNumber = esi->getEventTypes().begin()->mc_channel_number();
    
    //Inserting in a (unordered_)set returns an <iterator, boolean> pair, where the boolean
    //is used to check if the key already exists (returns false in the case it exists)
    if( m_existingMetaDataChan.insert(mcChannelNumber).second ) {
        xAOD::TruthMetaData* md = new xAOD::TruthMetaData();
        m_tmd->push_back( md );
       
        std::vector<std::string> orderedWeightNameVec;
        orderedWeightNameVec.reserve( m_weightNamesMap.size() );
        for (auto& entry: m_weightNamesMap) {
            orderedWeightNameVec.push_back(entry.first);
        }
       
        //The map from the HepMC record pairs the weight names with a corresponding index,
        //it is not guaranteed that the indices are ascending when iterating over the map
        std::sort(orderedWeightNameVec.begin(), orderedWeightNameVec.end(),
                  [&](std::string i, std::string j){return m_weightNamesMap.at(i) < m_weightNamesMap.at(j);});
       
        md->setMcChannelNumber(mcChannelNumber);
        md->setWeightNames( std::move(orderedWeightNameVec) );
    } 
    
    return StatusCode::SUCCESS;
}
