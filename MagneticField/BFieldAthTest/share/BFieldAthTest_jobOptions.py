#--------------------------------------------------------------
# BFieldAth
#--------------------------------------------------------------
include( "BFieldAth/BFieldAth_jobOptions.py" )

#-----------------------------------------------------------------
#                Steering Parameters for BFieldAthTest
#-----------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()
from BFieldAthTest.BFieldAthTestConf import MagFAthenaTest
topSequence += MagFAthenaTest( "MagFAthenaTestMaster" )

# --- GeoModel
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

theApp.EvtMax =10


#-----------------------------------------------------------------
theMagFAthenaTestMaster = topSequence.MagFAthenaTestMaster
theMagFAthenaTestMaster.MenbersName += [ "MagFTest01" ]
theMagFAthenaTestMaster.MenbersType += [ "MagFAthenaTest01" ]

from BFieldAthTest.BFieldAthTestConf import MagFAthenaTest01
theMagFAthenaTestMaster += MagFAthenaTest01("MagFTest01")

#-----------------------------------------------------------------
from BFieldAthTest.BFieldAthTestConf import MagFAthenaTest01
theMagFTest01 = MagFAthenaTest01("MagFTest01")
theMagFTest01.ca    =    9   ;
theMagFTest01.nx    =    2   ;
theMagFTest01.ny    =  150   ;
theMagFTest01.nz    =  230   ;
theMagFTest01.nr    = 1500   ;
theMagFTest01.nphi  =   23   ;
theMagFTest01.nstx  =    1   ;
theMagFTest01.nsty  =   10   ;
theMagFTest01.nstz  =   10   ;
theMagFTest01.nstr  =    0   ;
theMagFTest01.xmax  =    0   ;
theMagFTest01.xmin  =    0   ;
theMagFTest01.ymax  = 1500   ;
theMagFTest01.ymin  =    0   ;
theMagFTest01.zmax  =   10   ;
theMagFTest01.zmin  =   10   ;
theMagFTest01.rmax  = 1500   ;
theMagFTest01.rmin  =    0   ;
theMagFTest01.phimax=   23   ;
theMagFTest01.phimin=    0.5 ;
