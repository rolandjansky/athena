#include "TrkAlignGenTools/MatrixTool.h"
#include "TrkAlignGenTools/IPCMatrixTool.h"
#include "TrkAlignGenTools/TrkAlignDBTool.h"
#include "TrkAlignGenTools/AnalyticalDerivCalcTool.h"
#include "TrkAlignGenTools/ShiftingDerivCalcTool.h"
#include "TrkAlignGenTools/AlignTrackPreProcessor.h"
#include "TrkAlignGenTools/TrackCollectionProvider.h"
#include "TrkAlignGenTools/MSConstraintTracksProvider.h"
#include "TrkAlignGenTools/AlignTrackCreator.h"
#include "TrkAlignGenTools/AlignTrackDresser.h"
#include "TrkAlignGenTools/AlignModuleTool.h"
#include "TrkAlignGenTools/AlignResidualCalculator.h"
#include "TrkAlignGenTools/BeamspotVertexPreProcessor.h"
#include "TrkAlignGenTools/ConstrainedTrackProvider.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MatrixTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, IPCMatrixTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrkAlignDBTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, AnalyticalDerivCalcTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, ShiftingDerivCalcTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, AlignTrackPreProcessor )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrackCollectionProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MSConstraintTracksProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, AlignTrackCreator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, AlignTrackDresser )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, AlignModuleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, AlignResidualCalculator )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, BeamspotVertexPreProcessor )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, ConstrainedTrackProvider )

