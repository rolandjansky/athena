/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileGeoG4SD
// Sensitive detector for TileCal G4 simulations with TileGeoModel
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
//
// Major updates: July, 2013 (Sergey)
//
//************************************************************

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "TileGeoG4SD/TileGeoG4SD.hh"
#include "TileGeoG4SD/TileGeoG4SDCalc.hh"
#include "TileGeoG4SD/TileGeoG4LookupBuilder.hh"
#include "TileGeoG4SD/TileGeoG4Lookup.hh"
#include "TileSimEvent/TileHitVector.h"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#include "G4HCofThisEvent.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4Trd.hh"
#include "G4UImanager.hh"
#include "G4Geantino.hh"

#include "G4SubtractionSolid.hh"
#include "G4BooleanSolid.hh"

#include "TileSimUtils/TileG4SimOptions.h"
#include "PathResolver/PathResolver.h"

static SensitiveDetectorEntryT<TileGeoG4SD> tileGeoG4SD("TileGeoG4SD");
static SensitiveDetectorEntryT<TileGeoG4SD> tileCTBGeoG4SD("TileCTBGeoG4SD");



TileGeoG4SD::TileGeoG4SD(G4String name)
    : FadsSensitiveDetector(name)
{
    // Get MessageSvc pointers:
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
    StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
    m_log = new MsgStream (m_msgSvc, "TileGeoG4SD");
    m_debug = (m_log->level()<=MSG::DEBUG);
    m_verbose = (m_log->level()<=MSG::VERBOSE);
    if (status.isFailure()) 
        (*m_log) << MSG::FATAL << "MessageSvc not found!" << endreq;
    else if (m_debug)
        (*m_log) << MSG::DEBUG << "MessageSvc initialized." << endreq;

    //Sensitive Detector initialisation for TileCal G4 simulations
    bool is_ctb = (name.find("CTB") != G4String::npos);
    m_calc = TileGeoG4SDCalc::instance(is_ctb);

    //build tilecal ordinary look-up table
    m_lookup = m_calc->m_lookup;
    if (m_debug) (*m_log) << MSG::DEBUG << "Lookup built for Tile" << endreq;

}



TileGeoG4SD::~TileGeoG4SD()
{
    delete m_calc;
    delete m_lookup;
    delete m_log;
}



void TileGeoG4SD::Initialize(G4HCofThisEvent* /*HCE*/)
{
    if (m_debug) (*m_log) << MSG::DEBUG << "Initializing SD" << endreq;
}



G4bool TileGeoG4SD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
    G4bool stat = m_calc->FindTileScinSection(aStep);  //Search for the tilecal sub-section, its module and some identifiers
    if ( !stat ) {
        if (m_debug) (*m_log) << MSG::DEBUG <<"ProcessHits: FindTileScinSection(aStep) is false!" << endreq;
        return false;
    }

    stat = m_calc->MakePmtEdepTime(aStep);  //calculation of pmtID, edep and scin_Time with aStep (Sergey)
    if ( !stat ) {
        if (m_verbose) (*m_log) << MSG::VERBOSE <<"ProcessHits: wrong pmtID_up,pmtID_down,edep_up,edep_down,"
            <<"scin_Time_up,scin_Time_down:\t"<<m_calc->pmtID_up<<"\t"<<m_calc->pmtID_down<<"\t"<<m_calc->edep_up
            <<"\t"<<m_calc->edep_down<<"\t"<< m_calc->scin_Time_up<<"\t"<<m_calc->scin_Time_down << endreq;
        return false;
    }

    stat = m_calc->ManageScintHit();  //create or update hit object in the collection
    if ( !stat ) {
        (*m_log) << MSG::ERROR <<"ProcessHits: TileHit can not be produced; pmtID_up,pmtID_down,edep_up,edep_down,"
            <<"scin_Time_up,scin_Time_down:\t"<<m_calc->pmtID_up<<"\t"<<m_calc->pmtID_down<<"\t"<<m_calc->edep_up
            <<"\t"<<m_calc->edep_down<<"\t"<< m_calc->scin_Time_up<<"\t"<<m_calc->scin_Time_down << endreq;
        return false;
    }

    return true;
}


void TileGeoG4SD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
    //TileHitVector* tileHitsCollection = new TileHitVector("TileHitVec"); 
    TileHitVector* tileHitsCollection = m_hitCollHelp.RetrieveNonconstCollection<TileHitVector>("TileHitVec");

    m_lookup->ResetCells(tileHitsCollection);
    if (m_debug) (*m_log) << MSG::DEBUG <<"EndOfEvent: exporting TileHitVec" << endreq;
    if (!m_allowMods) m_hitCollHelp.SetConstCollection(tileHitsCollection);

    if (m_debug) (*m_log) << MSG::DEBUG << "--> End of event" << endreq;
}

