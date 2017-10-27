#include "TileConditions/TileInfoLoader.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileConditions/TileBadChanTool.h"
#include "TileConditions/TileCondToolOfc.h"
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileConditions/TileCellNoiseTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileExpertToolEmscale.h"
#include "TileConditions/TileCondToolIntegrator.h"
#include "TileConditions/TileCondToolMuID.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolNoiseRawChn.h"
#include "TileConditions/TileCondProxyWrapper.h"
#include "TileConditions/TileDCSSvc.h"
#include "TileConditions/TileCondDCS_Data.h"
#include "TileConditions/TileCondToolAutoCr.h"
#include "TileConditions/TileCondToolTMDB.h"
#include "TileConditions/TileCondToolDspThreshold.h"


DECLARE_COMPONENT( TileInfoLoader )
DECLARE_COMPONENT( TileCablingSvc )
DECLARE_COMPONENT( TileDCSSvc )
DECLARE_COMPONENT( TileCondIdTransforms )
DECLARE_COMPONENT( TileBadChanTool )
DECLARE_COMPONENT( TileCondToolOfc )
DECLARE_COMPONENT( TileCondToolOfcCool )
DECLARE_COMPONENT( TileCellNoiseTool )
DECLARE_COMPONENT( TileCondToolEmscale )
DECLARE_COMPONENT( TileExpertToolEmscale )
DECLARE_COMPONENT( TileCondToolIntegrator )
DECLARE_COMPONENT( TileCondToolMuID )
DECLARE_COMPONENT( TileCondToolTiming )
DECLARE_COMPONENT( TileCondToolPulseShape )
DECLARE_COMPONENT( TileCondToolNoiseSample )
DECLARE_COMPONENT( TileCondToolAutoCr )
DECLARE_COMPONENT( TileCondToolNoiseRawChn )
DECLARE_COMPONENT( TileCondToolTMDB )
DECLARE_COMPONENT( TileCondToolDspThreshold )
DECLARE_COMPONENT( TileCondProxyCoolFlt )
DECLARE_COMPONENT( TileCondProxyCoolBch )
DECLARE_COMPONENT( TileCondProxyCoolOfc )
DECLARE_COMPONENT( TileCondProxyFileFlt )
DECLARE_COMPONENT( TileCondProxyFileBch )

