/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HTTTrackPars.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date March 27th, 2020
 * @brief Structs that store the 5 track parameters
 */

#include "TrigHTTObjects/HTTTrackPars.h"

#include <stdexcept>

const double & HTTTrackPars::operator[](unsigned i) const
{
    switch (i)
    {
        case 0: return phi;
        case 1: return qOverPt;
        case 2: return d0;
        case 3: return z0;
        case 4: return eta;
        default: throw std::out_of_range("HTTTrackPars[] out of range");
    }
}


double & HTTTrackPars::operator[](unsigned i)
{
    switch (i)
    {
        case 0: return phi;
        case 1: return qOverPt;
        case 2: return d0;
        case 3: return z0;
        case 4: return eta;
        default: throw std::out_of_range("HTTTrackPars[] out of range");
    }
}


const int & HTTTrackParsI::operator[](unsigned i) const
{
    switch (i)
    {
        case 0: return phi;
        case 1: return qOverPt;
        case 2: return d0;
        case 3: return z0;
        case 4: return eta;
        default: throw std::out_of_range("HTTTrackPars[] out of range");
    }
}


int & HTTTrackParsI::operator[](unsigned i)
{
    switch (i)
    {
        case 0: return phi;
        case 1: return qOverPt;
        case 2: return d0;
        case 3: return z0;
        case 4: return eta;
        default: throw std::out_of_range("HTTTrackPars[] out of range");
    }
}


std::string HTTTrackPars::parName(unsigned i)
{
    switch (i)
    {
        case 0: return "phi";
        case 1: return "qOverPt";
        case 2: return "d0";
        case 3: return "z0";
        case 4: return "eta";
        default: throw std::out_of_range("HTTTrackPars::parName out of range");
    }
}

std::ostream & operator<<(std::ostream & os, const HTTTrackPars & pars)
{
    os << "phi:" << pars.phi
       << " q/pt:" << pars.qOverPt
       << " d0:" << pars.d0
       << " z0:" << pars.z0
       << " eta:" << pars.eta;
    return os;
}

std::ostream & operator<<(std::ostream & os, const HTTTrackParsI & pars)
{
    os << "phi:" << pars.phi
       << " q/pt:" << pars.qOverPt
       << " d0:" << pars.d0
       << " z0:" << pars.z0
       << " eta:" << pars.eta;
    return os;
}
