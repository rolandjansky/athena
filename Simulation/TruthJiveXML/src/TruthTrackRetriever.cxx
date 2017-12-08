/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthJiveXML/TruthTrackRetriever.h"

#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"
#include "GaudiKernel/SystemOfUnits.h" 
#include "EventPrimitives/EventPrimitives.h"

#include "GeneratorObjects/McEventCollection.h"
#include "TrackRecord/TrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"

namespace JiveXML {


  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  TruthTrackRetriever::TruthTrackRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    typeName("STr")
  {
    
    declareInterface<IDataRetriever>(this);
    
    declareProperty("StoreGateKey", m_McEvtCollName = "TruthEvent", "Name of the McEventCollection");
    declareProperty("UnstableMinPtCut",     m_MinPtCut = 100*Gaudi::Units::MeV, "Minimum pT for an unstable particle to get accepted");
    declareProperty("UnstableMinRhoCut",    m_MinRhoCut = 40*Gaudi::Units::mm, "Minium radius of the end-vertex for unstable particle to get accepted");

  }
  
  /**
   * Initialize before event loop
   */
  StatusCode TruthTrackRetriever::initialize(){
    //Nothing to do here
    return StatusCode::SUCCESS;
  }

  /**
   * Loop over all true particles and get their basic parameters
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode TruthTrackRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() << endmsg; 

    //Retrieve the collection
    const McEventCollection* McEvtColl = NULL;
    if ( !evtStore()->contains<McEventCollection>( m_McEvtCollName )){ 
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not find McEventCollection " << m_McEvtCollName << endmsg;
      return StatusCode::SUCCESS;
    }
    if( evtStore()->retrieve(McEvtColl, m_McEvtCollName).isFailure() ){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not retrieve McEventCollection " << m_McEvtCollName << endmsg;
      return StatusCode::SUCCESS;
    }
    
    // Calculate the size
    long NParticles=0;
    McEventCollection::const_iterator McEvtCollItr = McEvtColl->begin(); 
    for ( ; McEvtCollItr != McEvtColl->end(); ++McEvtCollItr)
      NParticles +=  (*McEvtCollItr)->particles_size();

    //Show in verbose mode
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Total number of particles in McEventCollection \""
                                            << m_McEvtCollName << "\" is " << NParticles << endmsg;

    //Reserve space for the output data
    DataVect pt; pt.reserve(NParticles);
    DataVect phi; phi.reserve(NParticles);
    DataVect eta; eta.reserve(NParticles);
    DataVect rhoVertex; rhoVertex.reserve(NParticles);
    DataVect phiVertex; phiVertex.reserve(NParticles);
    DataVect zVertex; zVertex.reserve(NParticles);
    DataVect code; code.reserve(NParticles);
    DataVect id; id.reserve(NParticles);
    DataVect rhoEndVertex; rhoEndVertex.reserve(NParticles);
    DataVect phiEndVertex; phiEndVertex.reserve(NParticles);
    DataVect zEndVertex; zEndVertex.reserve(NParticles);

      
    //Now loop events and retrieve
    for ( McEvtCollItr = McEvtColl->begin(); McEvtCollItr != McEvtColl->end(); ++McEvtCollItr){

      //Loop over particles in the event
      HepMC::GenEvent::particle_const_iterator ParticleItr = (*McEvtCollItr)->particles_begin();
      for ( ; ParticleItr!=(*McEvtCollItr)->particles_end(); ++ParticleItr ) {

        //Get the particle
        HepMC::GenParticle* particle = (*ParticleItr);
        
        //Additional cuts for decaying particles
        if ( particle->end_vertex() ) {
          //Reject particles that fail the pt cut
          if ( particle->momentum().perp() < m_MinPtCut) continue ; 
          //Reject particles that fail the minimum end-vertex cut
          if (particle->end_vertex()->point3d().perp() < m_MinRhoCut ) continue ;
        }

        //Get basic parameters (eta, phi, pt, ...)
        pt.push_back(DataType(particle->momentum().perp()/Gaudi::Units::GeV));
        float thePhi = particle->momentum().phi();
        phi.push_back(DataType( (thePhi<0) ? thePhi+=2*M_PI : thePhi ));
        eta.push_back(DataType( particle->momentum().pseudoRapidity() ));
        code.push_back(DataType( particle->pdg_id() ));
        id.push_back(DataType( particle->barcode() ));

        // Get the vertex information
        HepMC::GenVertex* vertex =  particle->production_vertex();
        if (vertex) {
          rhoVertex.push_back(DataType( vertex->point3d().r()*Gaudi::Units::mm/Gaudi::Units::cm ));
          float vtxPhi = vertex->point3d().phi();
          phiVertex.push_back(DataType( (vtxPhi<0)? vtxPhi+=2*M_PI : vtxPhi ));
          zVertex.push_back(DataType( vertex->point3d().z()*Gaudi::Units::mm/Gaudi::Units::cm )); 
        } else {
          rhoVertex.push_back(DataType( 0. ));
          phiVertex.push_back(DataType( 0. ));
          zVertex.push_back(DataType( 0. )); 
        }
        //Do the same for the end vertex
        vertex =  particle->end_vertex();
        if ( vertex ) {
         rhoEndVertex.push_back(DataType(vertex->point3d().r()*Gaudi::Units::mm/Gaudi::Units::cm));
         float vtxPhi = vertex->point3d().phi();
         phiEndVertex.push_back(DataType( (vtxPhi<0)? vtxPhi+=2*M_PI : vtxPhi ));
         zEndVertex.push_back(DataType(vertex->point3d().z()*Gaudi::Units::mm/Gaudi::Units::cm)); 
        } else {
         rhoEndVertex.push_back(DataType( 0. ));
         phiEndVertex.push_back(DataType( 0. ));
         zEndVertex.push_back(DataType( 0. )); 
        }
      }
    }

    DataMap myDataMap;
    myDataMap["pt"] = pt;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["code"] = code;
    myDataMap["id"] = id;
    myDataMap["rhoVertex"] = rhoVertex;
    myDataMap["phiVertex"] = phiVertex;
    myDataMap["zVertex"] = zVertex;
    myDataMap["rhoEndVertex"] = rhoEndVertex;
    myDataMap["phiEndVertex"] = phiEndVertex;
    myDataMap["zEndVertex"] = zEndVertex;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< pt.size() << endmsg;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_McEvtCollName, &myDataMap);
  }
}
