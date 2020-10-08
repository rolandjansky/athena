#include "TrackVertexAssociationTool/TrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/BaseTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/ElectronTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/MuonTrackVertexAssociationTool.h"
#include "TrackVertexAssociationTool/MVATrackVertexAssociationTool.h"

#include "../TrackVertexAssoTestAlg.h"

using namespace xAOD;

DECLARE_COMPONENT( CP::TrackVertexAssociationTool )
DECLARE_COMPONENT( CP::BaseTrackVertexAssociationTool )
DECLARE_COMPONENT( CP::ElectronTrackVertexAssociationTool )
DECLARE_COMPONENT( CP::MuonTrackVertexAssociationTool )
DECLARE_COMPONENT( CP::MVATrackVertexAssociationTool )
DECLARE_COMPONENT( TrackVertexAssoTestAlg )

