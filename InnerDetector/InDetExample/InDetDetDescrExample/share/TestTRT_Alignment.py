###############################################################
# See InDetAlignExample/TestInDetAlignment_jobOptions.py
# for standalone example.
# This joboptions can be used as a job option fragment.
###############################################################

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


from InDetDetDescrExample.InDetDetDescrExampleConf import TestTRT_Alignment

if DetFlags.detdescr.TRT_on():
    job += TestTRT_Alignment()
    print job.TestTRT_Alignment


# The following switches are available
#
# LongPrintOut:     If true prints out changes in the straw axes as well as center
# TestAllElements:  Printout for every element. If false only corner straws for each module will be printed.
#                   If true and TestAllStraws is false only first and last straw is printed.
# TestAllStraws:    Printout for every straw. Ignored if TestAllElements is false
# HardwiredShifts:  Use some hardcoded shifts rather than IOV.
# Precision:        Precision used to printout numbers.
