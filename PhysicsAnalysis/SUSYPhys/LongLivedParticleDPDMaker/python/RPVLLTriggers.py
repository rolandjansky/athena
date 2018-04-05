#from TriggerMenu.api.TriggerAPI import TriggerAPI

# these are imported from TriggerAPI...
import sys, pickle, os.path
from TriggerMenu.api.TriggerInfo import TriggerInfo
#from PathResolver import PathResolver
from AthenaCommon.Logging import logging

from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType

# dummy general function
def getTriggerList():

    triggerList = []

    return triggerList


# dummy class
class RPVLLTriggers:

    # dummy test method
    def getTestTriggers(self):
        test_list = getTriggerList()
        return test_list
