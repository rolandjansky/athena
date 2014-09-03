
#jobOptions for Tile part
include( "TileConditions/TileConditions_jobOptions.py" )

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags

include("LArRecUtils/LArOFCTool_jobOptions.py")
include("LArRecUtils/LArAutoCorrTotalTool_jobOptions.py")
include("LArRecUtils/LArADC2MeVTool_jobOptions.py")

from CaloTools.CaloToolsConf import CaloNoiseTool
theCaloNoiseTool=CaloNoiseTool("calonoisetool")


# Properties
theCaloNoiseTool.WorkMode=1
theCaloNoiseTool.WithOF= True
theCaloNoiseTool.TileInfoName="TileInfo"

# Luminosity
from AthenaCommon.BeamFlags import jobproperties
        
if jobproperties.Beam.zeroLuminosity(): 
    theCaloNoiseTool.ReturnNoise="electronicNoise"
    theCaloNoiseTool.NMinBias=0
    ToolSvc.LArAutoCorrTotalTool.NMinBias=0
else:
    theCaloNoiseTool.ReturnNoise="totalNoise"
    theCaloNoiseTool.NMinBias=jobproperties.Beam.numberOfCollisions()
    ToolSvc.LArAutoCorrTotalTool.NMinBias=jobproperties.Beam.numberOfCollisions()

_deltaBunch=int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)
theCaloNoiseTool.deltaBunch = _deltaBunch
ToolSvc.LArAutoCorrTotalTool.deltaBunch = _deltaBunch
del _deltaBunch

 # Concerning CaloNoiseTool and luminosity:
 # It will take AutoCorr and OFC calculated at the initialization
 # according to the NMinBias set above (in LArAutoCorrTotalTool).
 # However if you want/need the expected noise at another luminosity
 # you can specify another value, then it will take AutoCorr and OFC
 # accordingly recomputed on-fly
 # (without affecting the cache of LArAutoCorrTotalTool and LArOFCTool,
 # so without affecting the reconstruction)
 # Same thing if you specify the luminosity in the calling functions of
 # CaloNoiseTool


# AutoCorrelation
#(if the matrix is not normalized (default cse for the moment), normalize it
# REMOVED
#if globalflags.DetGeo()=='ctbh8' and globalflags.DataSource()=='data':
#    theCaloNoiseTool.NormalizeAutoCorr=True
#    ToolSvc.LArOFCTool.NormalizeAutoCorr=True

# Set data type (needed to decide wether to retrieve LArNoise or LArPedestal for the noise)
if GlobalFlags.DataSource()=='data':
    theCaloNoiseTool.IsMC=False

# No symmetry for testbeam
if( GlobalFlags.DetGeo()=='ctbh8' or  GlobalFlags.DetGeo()=='ctbh6' ) : 
    theCaloNoiseTool.UseSymmetry=False
    ToolSvc.LArAutoCorrTotalTool.MCSym = False
    ToolSvc.LArADC2MeVTool.MCSym = False
    ToolSvc.LArOFCTool.MCSym = False
else:    
    theCaloNoiseTool.UseSymmetry= True
    ToolSvc.LArAutoCorrTotalTool.MCSym = True
    ToolSvc.LArADC2MeVTool.MCSym = True
    ToolSvc.LArOFCTool.MCSym = True


    
# Dump the database (ADC2MEV, OFC, AC, ...) used for each cell (long dump !)
#theCaloNoiseTool.DumpDatabaseHG=TRUE
#theCaloNoiseTool.DumpDatabaseMG=TRUE
#theCaloNoiseTool.DumpDatabaseLG=TRUE

ToolSvc+=theCaloNoiseTool
