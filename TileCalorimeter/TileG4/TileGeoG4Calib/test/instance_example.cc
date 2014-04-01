/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileGeoG4SD/TileGeoG4SDCalc.hh"
        .
        .
        .
    TileGeoG4SDCalc * m_calc = TileGeoG4SDCalc::instance();  //Sensitive Detector initialisation
        .
        .
        .
    TileMicroHit      micHit = m_calc->GetTileMicroHit(aStep);  //calculation of MicroHit with aStep
    Identifier        m_invalid_id;
    if ((micHit.pmt_up == m_invalid_id) && (micHit.pmt_down == m_invalid_id)) {  //MicroHit is not in scintillator
        if (m_debug) (*m_log) << MSG::DEBUG << "GetTileMicroHit: wrong pmtID_up,pmtID_down,edep_up,edep_down,scin_Time_up,scin_Time_down:\t"
                      << micHit.pmt_up <<"\t"<< micHit.pmt_down <<"\t"<< micHit.e_up <<"\t"<< micHit.e_down <<"\t"
                      << micHit.time_up <<"\t"<< micHit.time_down << endreq;
        return false;
    }

    (*m_log) << MSG::INFO << "GetTileMicroHit: pmtID_up,pmtID_down,edep_up,edep_down,scin_Time_up,scin_Time_down:\t"
             << micHit.pmt_up <<"\t"<< micHit.pmt_down <<"\t"<< micHit.e_up <<"\t"<< micHit.e_down <<"\t"
             << micHit.time_up <<"\t"<< micHit.time_down << endreq;
        .
        .
        .

