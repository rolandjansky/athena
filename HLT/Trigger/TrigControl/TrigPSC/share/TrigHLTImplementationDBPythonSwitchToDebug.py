#
# Switch Output Level to DEBUG when using HLTImplementationDBPython in OKS
#
from AthenaCommon.Include import include
from AthenaCommon.Constants import *
from TrigPSC import PscConfig
from TrigServices.TriggerUnixStandardSetup import _Conf
_Conf.useOnlineTHistSvc=True
PscConfig.optmap['LOGLEVEL']='DEBUG'
include('TrigPSC/TrigPSCPythonDbSetup.py')
