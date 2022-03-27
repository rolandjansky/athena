/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_HTTTRACKPARS_H
#define TRIGHTTOBJECTS_HTTTRACKPARS_H


/**
 * @file HTTTrackPars.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date March 27th, 2020
 * @brief Structs that store the 5 track parameters
 */


#include <string>
#include <iostream>
#include <cmath>

struct HTTTrackPars
{
    double phi;
    double qOverPt;
    double d0;
    double z0;
    double eta;

    ////////////////////////////////////////////////////////////////
    // Methods

    HTTTrackPars(double init = std::nan("")) :
        phi(init),
        qOverPt(init),
        d0(init),
        z0(init),
        eta(init)
    {
    }
    const double& operator[](unsigned i) const;
    double& operator[](unsigned i);

    ////////////////////////////////////////////////////////////////
    // Statics

    friend std::ostream& operator<<(std::ostream& os, const HTTTrackPars& pars);

    enum pars_index { IPHI, IHIP, ID0, IZ0, IETA, NPARS };
    static std::string parName(unsigned i);
};


// Useful to store bin numbers, i.e.
struct HTTTrackParsI
{
    int phi;
    int qOverPt;
    int d0;
    int z0;
    int eta;

    const int& operator[](unsigned i) const;
    int& operator[](unsigned i);

    friend std::ostream& operator<<(std::ostream& os, const HTTTrackParsI& pars);

    HTTTrackParsI(int init = 0) :
        phi(init),
        qOverPt(init),
        d0(init),
        z0(init),
        eta(init)
    {
    }
};



#endif
