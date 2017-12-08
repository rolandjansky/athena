/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthJiveXML/TruthMuonTrackRetriever.h"

//#include "GaudiKernel/Bootstrap.h"
//#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "HepMC/GenParticle.h"
#include "TrackRecord/TrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"


namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
   TruthMuonTrackRetriever::TruthMuonTrackRetriever(const std::string& type ,const std::string& name,const IInterface* parent):
     AthAlgTool(type, name, parent),
     typeName("SMTr") {
     
     //Declare the interface
     declareInterface<IDataRetriever>(this);

     //Fill the list with the names of track record collections to try
     TrackRecCollNames.push_back("MuonEntryLayer");
     TrackRecCollNames.push_back("MuonEntryRecord");

     //Declare the properties
     declareProperty("TrackRecCollNames",TrackRecCollNames,"List of track records collections to try in this order - only one is retrieved");

   }


  /**
   * Initialize before event loop
   */
  StatusCode TruthMuonTrackRetriever::initialize(){
    
    //Nothing to be done here
    return StatusCode::SUCCESS;
  }
  
  /**
   * Loop over all true particles, find the muons and get their basic parameters
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode TruthMuonTrackRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    ATH_MSG_DEBUG( "Retrieving " << dataTypeName() ); 

    //Try to retrieve the track record collection
    const TrackRecordCollection* TrackRecordColl = NULL ;
    //Loop over all the collections and try a retrieve (more efficenct than
    //contain-retrieve combination)
    std::vector<std::string>::const_iterator CollNameItr = TrackRecCollNames.begin();
    for (auto CollNameItr : TrackRecCollNames ) {
      //be verbose
      ATH_MSG_DEBUG( "Trying to retrieve " << CollNameItr );
      //try to retrive
      if ( !evtStore()->contains<TrackRecordCollection>( CollNameItr )){ continue; } // skip if not in SG
      if (evtStore()->retrieve(TrackRecordColl, CollNameItr).isSuccess()) break ;
    }

    //If we didnt' get any, return
    if (TrackRecordColl == NULL ) {
      ATH_MSG_WARNING( "Unable to retrieve any track collection from " << TrackRecCollNames );
      return StatusCode::RECOVERABLE;
    }

    //Reserve space for the output
    DataVect pt; pt.reserve(TrackRecordColl->size());
    DataVect phi; phi.reserve(TrackRecordColl->size());
    DataVect eta; eta.reserve(TrackRecordColl->size());
    DataVect rhoVertex; rhoVertex.reserve(TrackRecordColl->size());
    DataVect phiVertex; phiVertex.reserve(TrackRecordColl->size());
    DataVect zVertex; zVertex.reserve(TrackRecordColl->size());
    DataVect code; code.reserve(TrackRecordColl->size());
    DataVect id; id.reserve(TrackRecordColl->size()); 

    //Now loop over the collection and retrieve data
    TrackRecordCollection::const_iterator record;
    for (auto record : *TrackRecordColl ) {

      //Get the pdg code
      int pdgCode = record.GetPDGCode();

      //Only accept muons
      if (abs(pdgCode) != 13) {
        ATH_MSG_DEBUG( "Reject non-muon track with PDG ID " << pdgCode );
        continue;
      }

      //Get vertex and momentum
      HepGeom::Point3D<double> vertex = record.GetPosition();
      HepGeom::Vector3D<double> momentum = record.GetMomentum();

      //And store output
      pt.push_back(DataType( momentum.perp()/CLHEP::GeV ));
      phi.push_back(DataType( momentum.phi() < 0 ? momentum.phi() + 2*M_PI : momentum.phi() ));
      eta.push_back(DataType( momentum.pseudoRapidity() ));
      rhoVertex.push_back(DataType( vertex.perp()*CLHEP::mm/CLHEP::cm ));
      phiVertex.push_back(DataType( vertex.phi() < 0 ? vertex.phi() + 2*M_PI : vertex.phi() ));
      zVertex.push_back(DataType( vertex.z()*CLHEP::mm/CLHEP::cm ));
      code.push_back(DataType( pdgCode ));
      id.push_back(DataType( record.GetBarCode() ));
    }
    
    //Finall add everything to the datamap
    DataMap dataMap;
    dataMap["pt"] = pt;
    dataMap["phi"] = phi;
    dataMap["eta"] = eta;
    dataMap["rhoVertex"] = rhoVertex;
    dataMap["phiVertex"] = phiVertex;
    dataMap["zVertex"] = zVertex;
    dataMap["code"] = code;
    dataMap["id"] = id;
    
    //some summary
    ATH_MSG_DEBUG( dataTypeName() << ": "<< pt.size() );

    //forward data to formating tool
    //return FormatTool->AddToEvent(dataTypeName(), (*CollNameItr), &dataMap);
    //// Atlantis can't deal with sgkey in xml output in SMTr (freezes)
    //// So no sgkey printed for now. jpt 20Aug09
    std::string emptyStr="";
    return FormatTool->AddToEvent(dataTypeName(), emptyStr, &dataMap);
  }
}
