/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <assert.h>
#include "iPatTrack/TrackStatus.h"
 
std::ostream &operator<< (std::ostream &out, TrackStatus status)
{
    switch (status)
    {
    case undefined:
	return out << " undefined      ";
    case two_point:
	return out << " Two_point:     ";
    case three_point:
	return out << " Three_point:   ";
    case fast_segment:
	return out << " Fast_segment:  ";
    case segment:
	return out << " Segment:       ";
    case long_segment:
	return out << " Long_segment:  ";
    case secondary:
	return out << " Secondary:     ";
    case truncated:
	return out << " Truncated:     ";
    case pendingTRT:
	return out << " PendingTRT:    ";
    case primary:
	return out << " Primary:       ";	
    case muon:
	return out << " Muon:          ";
    case combined_muon:
	return out << " Combined_muon: ";
    case combined_ecal:
	return out << " Combined_ecal: ";
    default:
	std::cout << " exiting: unknown TrackStatus " << static_cast<int>(status) << std::endl;
	assert (false);
	return out << " fatal error    ";
    };
}
