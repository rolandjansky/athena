# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Logging import logging
import re 
logMuon = logging.getLogger("MuonAlign")


#
# Some helper functions
#
def hasBeenSet(prop):
    return id(prop.__class__.StoredValue) != id(prop.StoredValue) 

def setDefault(prop,value):
    global logMuon
    if value != prop.__class__.StoredValue:
        mess = "Changing default %s = %r" % ( prop.__name__, value )
        if hasBeenSet(prop):
            mess += " (current value: %r)" % prop.get_Value()
        logMuon.info(mess)
        prop.__class__.StoredValue = value

       
class MuonAlign(JobPropertyContainer):
    """Flags to steer the setup of the Muon ConvertersAlignment"""
    def setDefaults(self):
        # check on problematic global tags with no association of cabling map folders exist; in this case we use the old cabling  
        logMuon.debug("in setDefaults")
        problematic_tags = ['COMCOND-HLT[A-C]-00[01]-00', 'COMCOND-ES1C-00[01]-00', 'COMCOND-REPC-001', 'COMCOND-SIM-01-00']
        if any(re.match(tag,globalflags.ConditionsTag()) for tag in problematic_tags):
            # setting the values (not just the defaults)
            self.UseAlines = 'none'
            self.UseBlines = 'none'
            self.UseIlines = False
            self.UseAsBuilt = False
            logMuon.info("No good alignment constants associated to Conditions Tag %r: UseAlines=%r UseBlines=%r UseIlines=%r UseAsBuilt=%r",
                         globalflags.ConditionsTag(),self.UseAlines(), self.UseBlines(), self.UseIlines(), self.UseAsBuilt() )

        #
        # simulation, digitization and reconstruction 
        #
        # MDT
        elif globalflags.DataSource() == 'geant4': # MC reconstruction or digitization
            setDefault(self.UseAlines, 'none')
            setDefault(self.UseBlines, 'none')
            setDefault(self.UseIlines, False)
            setDefault(self.UseAsBuilt, False)
            logMuon.info("Running a reconstruction job on geant4 or a digitization job: UseAlines=%r UseBlines=%r UseIlines=%r UseAsBuilt=%r",
                         self.UseAlines(), self.UseBlines(), self.UseIlines(), self.UseAsBuilt())
        else:
            # here reconstruction or real data 
            setDefault(self.UseAlines, 'all')
            setDefault(self.UseBlines, 'all')
            setDefault(self.UseIlines, True)
            setDefault(self.UseAsBuilt, True)
            logMuon.info("Running a reconstruction job on data: UseAlines=%r UseBlines=%r UseIlines=%r UseAsBuilt=%r",
                         self.UseAlines(), self.UseBlines(), self.UseIlines(), self.UseAsBuilt())


jobproperties.add_Container(MuonAlign)
muonAlignFlags = jobproperties.MuonAlign

class UseAlines(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues=['none','all','barrel','endcaps']
    StoredValue='none' 

muonAlignFlags.add_JobProperty(UseAlines)

class UseBlines(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues=['none','all','barrel','endcaps']
    StoredValue='none' 

muonAlignFlags.add_JobProperty(UseBlines)

class UseIlines(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False 

muonAlignFlags.add_JobProperty(UseIlines)

class UseAsBuilt(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False 

muonAlignFlags.add_JobProperty(UseAsBuilt)

# at the end, set the defaults
muonAlignFlags.setDefaults()
