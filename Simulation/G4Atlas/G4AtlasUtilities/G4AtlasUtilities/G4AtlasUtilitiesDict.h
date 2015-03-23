/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4DetectorEnvelopes/ResizeableBoxEnvelope.h"
#include "G4DetectorEnvelopes/ResizeableTubsEnvelope.h"
#include "G4DetectorEnvelopes/GenericPCONEnvelope.h"
#include "G4DetectorEnvelopes/DefSvcEnvelope.h"
#include "G4DetectorEnvelopes/GenericPGONEnvelope.h"
#include "FadsKinematics/ManipulatorBase.h"
#include "FadsKinematics/VertexManipulator.h"
#include "FadsKinematics/ParticleManipulator.h"
#include "G4AtlasUtilities/EtaPhiFilters.h"
#include "G4AtlasUtilities/VertexPositioner.h"
#include "G4AtlasUtilities/VertexRangeChecker.h"
#include "G4AtlasUtilities/PrimaryEventRotations.h"
#include "G4AtlasUtilities/BeamEffectTransformation.h"
