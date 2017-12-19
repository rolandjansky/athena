#include "TrackVertexAssociationTool/LooseTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/TightTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/BaseTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/ElectronTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/MuonTrackVertexAssociationTool.h"

#include "../TrackVertexAssoTestAlg.h"

using namespace xAOD;

DECLARE_COMPONENT( CP::LooseTrackVertexAssociationTool )
DECLARE_COMPONENT( CP::TightTrackVertexAssociationTool )
DECLARE_COMPONENT( CP::BaseTrackVertexAssociationTool )
DECLARE_COMPONENT( CP::ElectronTrackVertexAssociationTool )
DECLARE_COMPONENT( CP::MuonTrackVertexAssociationTool )
DECLARE_COMPONENT( TrackVertexAssoTestAlg )

