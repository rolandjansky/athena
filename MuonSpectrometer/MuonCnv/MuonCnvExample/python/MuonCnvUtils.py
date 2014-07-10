# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

### Simple function holding connecting names to the different calibration window options
# The window values themselves are defined in C++ in MdtCalibSvc/MdtCalibrationSvcSettings.h
# Author: Mike Flowerdew <michael.flowerdew@cern.ch>

__mdtCalibWindows = ['User',
                     'Default', ## 1000,2000
                     'Collision_G4', ## 20,30
                     'Collision_data', ## 10,30
                     'Collision_t0fit', ## 50,100
                     ]

def mdtCalibWindowNumber(name):
    """Returns index number corresponding to the calibration window name.
    This will throw a ValueError if name is not in the list.
    """
    return __mdtCalibWindows.index(name)

def mdtCalibWindowName(number):
    """Returns calibration name corresponding to the index number.
    This will throw an IndexError if number is out of range.
    """
    return __mdtCalibWindows[number]




def specialAddFolderSplitOnline(ident,folder1,folder2,force=False,forceMC=False,forceData=False):
    "Add access to given folder, using folder1 online, folder2 offline but do not append _OFL to the ident"
    from IOVDbSvc.CondDB import conddb
    if conddb.isOnline and not conddb.isMC:
        conddb.addFolder(ident,folder1,force=force,forceMC=forceMC,forceData=forceData)
    else:
        conddb.addFolder(ident,folder2,force=force,forceMC=forceMC,forceData=forceData)


## Set the default value of JobProperty @c prop to @c value. Should really go to AthenaCommon
def setJobPropertyDefault(prop,value,log=None):
    global logMuon
    if value != prop.__class__.StoredValue:
        mess = "Changing default %s = %r" % ( prop.__name__, value )
        if prop.__class__.StoredValue is not prop.StoredValue:
            mess += " (current value: %r)" % prop.get_Value()
        if log is not None: log.info(mess)
        prop.__class__.StoredValue = value

