/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Distinguish type/status of Track objects
 ----------------------------------------
 ***************************************************************************/

#ifndef IPATTRACK_HITSTATUS_H
# define IPATTRACK_HITSTATUS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <iostream>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>

enum  HitStatus
{
    // active hit classifications required for TrackFitter
    // passive material classifications required for TrackFinder
    pending		= 0,
    outside		= -1,
    pixel_cluster	= -20,
    broad_pixel		= -21,
    pixel_reject	= -22,
    strip_cluster	= -23,
    broad_strip		= -24,
    strip_reject	= -25,
    drift_time		= -30,
    drift_wire		= -31,
    drift_reject	= -32,
    scatterer		= -2,
    hole		= -3,
    rejected		= -4,
    inactive		= -5,
    inert		= -6,
    dead		= -7,
    boundary		= -8,
    calorimeter		= -9,
    brem		= -10,
    muon_parameters	= -11
};

//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>

std::ostream &operator<< (std::ostream &, HitStatus);

#endif	// IPATTRACK_HITSTATUS_H
