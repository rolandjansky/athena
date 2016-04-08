###############################################################
# See InDetAlignExample/TestInDetAlignment_jobOptions.py
# for standalone example.
# This joboptions can be used as a job option fragment.
###############################################################

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


from InDetDetDescrExample.InDetDetDescrExampleConf import TestSiAlignment

if DetFlags.detdescr.pixel_on():
    job += TestSiAlignment("TestPixelAlignment")
    job.TestPixelAlignment.ManagerName="Pixel"
    job.TestPixelAlignment.LongPrintOut=False
    print job.TestPixelAlignment

if DetFlags.detdescr.SCT_on():
    job += TestSiAlignment("TestSCT_Alignment")
    job.TestSCT_Alignment.ManagerName="SCT"
    job.TestSCT_Alignment.LongPrintOut=False
    print job.TestSCT_Alignment
    
#--------------------------------------------------------------
#
# TestSiAlignment properties
#
# ManagerName:      Manager name.
# LongPrintOut:     If true prints out changes in the axes as well as center
# ErrorRotation:    Error used when checking if delta is identity (rotation part). Default is 1e-15
# ErrorTranslation: Error used when checking if delta is identity (translation part). Default is 1e-12
#
#--------------------------------------------------------------

