###############################################################
#
# TRT Digitization
#
#==============================================================

# File updated April 2007, by Esben Klinkby <klinkby@nbi.nbi>
from AthenaCommon.Resilience import protectedInclude
protectedInclude( "PartPropSvc/PartPropSvc.py" )
# Setup to use conditions database folder to steer digitization settings
from AthenaCommon.GlobalFlags import globalflags
if not ( globalflags.isOverlay() and globalflags.DataSource == 'data' ) :
    from IOVDbSvc.CondDB import conddb
    conddb.addFolder("TRT_OFL","/TRT/Cond/DigVers",className='AthenaAttributeList')
