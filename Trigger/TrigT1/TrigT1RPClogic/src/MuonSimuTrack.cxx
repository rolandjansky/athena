/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <math.h>
#include <iostream>
#include <iomanip>

#include "TrigT1RPClogic/MuonSimuTrack.h"

MuonSimuTrack::MuonSimuTrack(int number,const SimuTrackData& data) :
RPCtrigDataObject(number,"SimulatedMuonTrack")
{
    this->m_event_number  = data.event_number;
    this->m_vertex_number = data.vertex_number;
    this->m_track_number  = data.track_number;

    this->m_Px = data.Px;
    this->m_Py = data.Py;
    this->m_Pz = data.Pz;
    this->m_Energy = data.Energy;
}

MuonSimuTrack::MuonSimuTrack(const MuonSimuTrack& muon) :
RPCtrigDataObject(muon.number(),muon.name())
{
    this->m_event_number  = muon.event_number();
    this->m_vertex_number = muon.vertex_number();
    this->m_track_number  = muon.track_number();

    this->m_Px = muon.Px();
    this->m_Py = muon.Py();
    this->m_Pz = muon.Pz();
    this->m_Energy = muon.Energy();
}

MuonSimuTrack MuonSimuTrack::operator = (const MuonSimuTrack& muon)
{
    static_cast<RPCtrigDataObject&>(*this) =
                                   static_cast<const RPCtrigDataObject&>(muon);
    this->m_event_number  = muon.event_number();
    this->m_vertex_number = muon.vertex_number();
    this->m_track_number  = muon.track_number();

    this->m_Px = muon.Px();
    this->m_Py = muon.Py();
    this->m_Pz = muon.Pz();
    this->m_Energy = muon.Energy();

    return *this;
}

float MuonSimuTrack::Pt(void) const
{
    return sqrt(m_Px * m_Px + m_Py * m_Py);
}

float MuonSimuTrack::generation_theta(void) const
{
    return atan(this->Pt()/fabsf(this->m_Pz));
}

float MuonSimuTrack::generation_eta(void) const
{
    float eta = -log(tan(this->generation_theta()/2.));
    return (this->m_Pz > 0.)? eta : -eta;
}

float MuonSimuTrack::generation_phi(void) const
{
    return atan2(this->m_Py,this->m_Px);
}

void MuonSimuTrack::Print(std::ostream& stream,bool detail) const
{
    unsigned int upper = 17;
    unsigned int lower = 19;

    for(int i=0;i<61;++i) stream << (char) upper;
    stream << std::endl;

    stream << " Muon track number "   << track_number()  << 
              " from vertex n. "      << vertex_number() << 
              ", belonging to event " << event_number()  << 
              ":" << std::endl;
    stream << "  generation eta = " << std::setw(6) << generation_eta() << 
              " generation phi = "  << std::setw(6) << generation_phi() << 
              std::endl;
    
    if (detail)
    {
        stream << "  Px = " << std::setw(7) << Px() <<
	          ", Py = " << std::setw(7) << Py() <<
	          ", Pz = " << std::setw(7) << Pz() <<
	          ", Energy = " << std::setw(7) << Energy() << std::endl;
    }

    for(int i=0;i<61;++i) stream << (char) lower;
    stream << std::endl;
}
