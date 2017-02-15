## @file   OfflineTHistSvc.py
## @brief  Configure the offline THistSvc
## @author Frank Winklmeier <fwinkl@cern>
##
## This fragment will be included by athenaHLT if we are
## running withtout the OH monitoring. Otherwise, the default
## is to use the online histogramming service (TrigMonTHistSvc)
###################################################################

from TrigServices.TriggerUnixStandardSetup import _Conf
_Conf.useOnlineTHistSvc = False
del _Conf

