/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthMetaDataWriter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)

// Header for this class
#include "DerivationFrameworkMCTruth/TruthMetaDataWriter.h"

// EDM Objects that we need
#include "xAODTruth/TruthMetaData.h"
#include "xAODTruth/TruthMetaDataAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"

// For accessing the tagInfo
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// Service for the weights
#include "GenInterfaces/IHepMCWeightSvc.h"

// Constructor
DerivationFramework::TruthMetaDataWriter::TruthMetaDataWriter(const std::string& t,
                                                              const std::string& n,
                                                              const IInterface* p)
  : AthAlgTool(t,n,p)
  , m_metaStore( "MetaDataStore", n )
  , m_weightSvc( "HepMCWeightSvc/HepMCWeightSvc" , n )
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty( "MetaObjectName", m_metaName = "TruthMetaData" );
    declareProperty( "MetaDataStore", m_metaStore );
}

// Destructor
DerivationFramework::TruthMetaDataWriter::~TruthMetaDataWriter() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthMetaDataWriter::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    // Initialize the service handles
    CHECK( m_metaStore.retrieve() );
    CHECK( m_weightSvc.retrieve() );

    // Create an empty truth meta data container:
    xAOD::TruthMetaDataAuxContainer* aux = new xAOD::TruthMetaDataAuxContainer();
    m_tmd = new xAOD::TruthMetaDataContainer();
    m_tmd->setStore( aux );
    // Record it in the metadata store    
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
    uint32_t mcChannelNumber = 0;
    // If this fails, we are running on a datatype with no EventInfo.  Such data types should
    //  definitely not be mixing MC samples, so this should be safe (will fall back to 0 above)
    if (evtStore()->contains<xAOD::EventInfo>("McEventInfo")){
      const DataHandle<xAOD::EventInfo> eventInfo = nullptr;
      CHECK( evtStore()->retrieve(eventInfo) );
      mcChannelNumber = eventInfo->mcChannelNumber();
    }
    
    //Inserting in a (unordered_)set returns an <iterator, boolean> pair, where the boolean
    //is used to check if the key already exists (returns false in the case it exists)
    if( m_existingMetaDataChan.insert(mcChannelNumber).second ) {
        xAOD::TruthMetaData* md = new xAOD::TruthMetaData();
        m_tmd->push_back( md );

        // Get the list of weights from the metadata       
        std::map<std::string,std::size_t> weight_name_map = m_weightSvc->weightNames();

        std::vector<std::string> orderedWeightNameVec;
        orderedWeightNameVec.reserve( weight_name_map.size() );
        for (auto& entry: weight_name_map) {
            orderedWeightNameVec.push_back(entry.first);
        }
       
        //The map from the HepMC record pairs the weight names with a corresponding index,
        //it is not guaranteed that the indices are ascending when iterating over the map
        std::sort(orderedWeightNameVec.begin(), orderedWeightNameVec.end(),
                  [&](std::string i, std::string j){return weight_name_map.at(i) < weight_name_map.at(j);});
       
        md->setMcChannelNumber(mcChannelNumber);
        md->setWeightNames( std::move(orderedWeightNameVec) );

        // Shamelessly stolen from the file meta data tool
        const CondAttrListCollection* tagInfo(nullptr);
        ATH_CHECK( detStore()->retrieve( tagInfo, "/TagInfo" ) );

        // Access the first, and only channel of the object:
        const CondAttrListCollection::AttributeList& al = tagInfo->attributeList( 0 );

        if (al.exists("lhefGenerator")){
            md->setLhefGenerator( al["lhefGenerator"].data< std::string >() );
        }

        if (al.exists("generators")){
            md->setGenerators( al["generators"].data< std::string >() );
        }

        if (al.exists("evgenProcess")){
            md->setEvgenProcess( al["evgenProcess"].data< std::string >() );
        }

        if (al.exists("evgenTune")){
            md->setEvgenTune( al["evgenTune"].data< std::string >() );
        }

        if (al.exists("hardPDF")){
            md->setHardPDF( al["hardPDF"].data< std::string >() );
        }

        if (al.exists("softPDF")){
            md->setSoftPDF( al["softPDF"].data< std::string >() );
        }
        // Done getting things from the TagInfo

    } // Done making the new truth metadata object    
    return StatusCode::SUCCESS;
}
