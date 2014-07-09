from PyKernel import PyKernel
from LArRawConditions.LArConditionsContainer import *

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
#theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")(OutputLevel = INFO)

def pyroot_typedef_bug_workaround():
  # delete iterator types to prevent spurious typedeffing
  import libPyROOT

  try:
    del libPyROOT.const_iterator
  except AttributeError:
    pass

  try:
    del libPyROOT.iterator
  except AttributeError:
    pass


#pylcgdict.loadDictionary("LArToolsDict")
#pylcgdict.loadDictionary("LArRawConditionsDict")

try:
  container1 = LArConditionsContainer('LArOnlCalibDataContainer')
  container2 = LArConditionsContainer('LArOnlEnergyDataContainer')
except:
  print "Can't get LArOnlCalibDataContainer"

from StoreGateBindings.Bindings import StoreGate
detStore = StoreGate.pointer("DetectorStore")

onlContainer = detStore.retrieve("LArOnlCalibDataContainer","LArOnlCalibData")
thrContainer = detStore.retrieve("LArOnlEnergyDataContainer","LArOnlEnergyData")

dir(onlContainer)
dir(thrContainer)

import ROOT

febList_int = getattr(ROOT,"std::vector<unsigned int>")
febList = febList_int()
##febList.push_back(949518336)
##febList.push_back(949551104)
##febList.push_back(949583872)
##febList.push_back(949616640)
##febList.push_back(949649408)
##febList.push_back(949682176)
##febList.push_back(949714944)
##febList.push_back(949747712)
febList.push_back(974782464) ## HEC
febList.push_back(974651392) ## EMEC IW
febList.push_back(991264768) ## EMEC OW
febList.push_back(993001472) ## FCal
febList.push_back(958562304) ## EMBA
print febList

import time

startTime = time.time()

xml_str = getattr(ROOT,"std::string")
xmlstr = xml_str()

prep.getXML(febList,xmlstr)

print xmlstr

elapsedTime = (time.time() - startTime)

#print "TIME ELAPSED: ", elapsedTime
