/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <assert.h>
#include "iPatTrack/HitStatus.h"

std::ostream &operator<< (std::ostream &out, HitStatus status)
{
    switch (status)
    {
    case pending:
	return out << " pending         ";
    case outside:
	return out << " outside         ";
    case pixel_cluster:
	return out << " pixel_cluster   ";
    case broad_pixel:
	return out << " broad_pixel     ";
    case pixel_reject:
	return out << " pixel_reject    ";
    case strip_cluster:
	return out << " strip_cluster   ";
    case broad_strip:
	return out << " broad_strip     ";
    case strip_reject:
	return out << " strip_reject    ";	
    case drift_time:
	return out << " drift_time      ";
    case drift_wire:
	return out << " drift_wire      ";
    case drift_reject:
	return out << " drift_reject    ";
    case scatterer:
	return out << " scatterer       ";
    case hole:
	return out << " hole            ";	
    case rejected:
	return out << " rejected        ";
    case inactive:
	return out << " inactive        ";
    case dead:
	return out << " dead            ";
    case boundary:
	return out << " boundary        ";
    case calorimeter:
	return out << " calorimeter     ";	
    case brem:
        return out << " brem            ";
    case muon_parameters:
	return out << " muon_parameters ";
    default:
	std::cout << " exiting: unknown HitStatus " << static_cast<int>(status) << std::endl;
	assert (false);
	return out << " fatal error    ";
    }    
}
