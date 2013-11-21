/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventShapeEvent/EventShapes.h"

std::string EventShapes::getName()
{
  std::string name="unknown";
  switch (m_type)
    {
    case CENTRAL_CUT:      name = "Central_Cut"; break;
    case THRUST:           name = "Thrust";      break;
    case THRUST_MINOR:     name = "ThrustMinor"; break;
    case FLIPVALUES:       name = "FlipValues";    break;
    case HEAVY_JETMASS:    name = "Heavy_JM";    break;
    case LIGHT_JETMASS:    name = "Light_JM";    break;
    case TOTAL_BROADENING: name = "B_T";         break;
    case WIDE_BROADENING:  name = "B_W";         break;
    case THRUST_PHI:       name = "phi_Thrust";  break;
    case THRUST_ETA:       name = "eta_Thrust";  break;
    case FORWARD_TERM:     name = "forward_term";break;
    case JET1_ET:          name = "Jet1_ET";     break;
    case JET2_ET:          name = "Jet2_ET";     break;
    case JET_ETA_CUT:      name = "Jet1_eta_cut";break;
    case FOXWOLFRAM:       name = "FoxWolfram";  break;
    case TRANSVERSESPHERICITY: name = "TransverseSphericity"; break;
    case CPARAMETER:       name = "CParameter";  break;
    case EEC_PHI:          name = "EECvsPhi";    break;
    case EEC_ETA:          name = "EECvsEta";    break;
    case EEC_RAD:          name = "EECvsRad";    break;
    case UNKNOWN:
    default:               name = "unknown";     break;
    }
  return name;
}
