# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


"""
#
#the configuration of tools shared between L2 and EF
#

"""
___author___ = "Jiri Masik"
___version___ = "$Id: $"


from AthenaCommon.Logging import logging
log = logging.getLogger('InDetTrigCommonTools')


from AthenaCommon.GlobalFlags import globalflags


# Straw status DB Tool
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
InDetTrigTRTStrawStatusSummaryTool = TRT_StrawStatusSummaryTool(name = "InDetTrigTRTStrawStatusSummaryTool",
                                                                isGEANT4 = (globalflags.DataSource == 'geant4'))



