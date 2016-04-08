# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from RecExConfig.RecFlags import rec as recFlags
import re 
logMuon = logging.getLogger(__name__)

from MuonCnvUtils import setJobPropertyDefault

def setDefault(prop,value):
    return setJobPropertyDefault(prop,value,logMuon)


class MuonCnv(JobPropertyContainer):
    """Flags to steer the setup of the Muon Converters, including the Cabling"""
    def setDefaults(self):
        # check on problematic global tags with no association of cabling map folders exist; in this case we use the old cabling  
        problematic_tags_mdtrpc = ['COMCOND-00[0-6]','COMCOND-HLT[A-C]-00[0-1]','COMCOND-MONC-00[0-1]', 'COMCOND-ES1C-00[0-1]', 'COMCOND-REPC-00[0-4]', 'OFLCOND-CSC','OFLCOND-FDR']
        problematic_tags_tgc = ['COMCOND-00[0-5]','COMCOND-006-00','CMCCOND-CSC','OFLCOND-FDR-02-0[4-6]','OFLCOND-FDR-02-10']

        # MDT
#        if globalflags.InputFormat.is_bytestream() and globalflags.DataSource() == 'geant4':
#            setDefault(self.MdtCablingMode, 'old')
#        else:
#            if any(re.match(tag,globalflags.ConditionsTag()) for tag in problematic_tags_mdtrpc):
#                setDefault(self.MdtCablingMode, 'old')
#            else:
#                if globalflags.DataSource() == 'data' or DetFlags.digitize.MDT_on():
#                    setDefault(self.MdtCablingMode, 'new')
#                else:
#                    setDefault(self.MdtCablingMode, 'auto')


        # RPC
        if globalflags.InputFormat.is_bytestream() and globalflags.DataSource() == 'geant4':
         setDefault(self.RpcCablingMode, 'sim')
        else:
            if any(re.match(tag,globalflags.ConditionsTag()) for tag in problematic_tags_mdtrpc):
                setDefault(self.RpcCablingMode, 'old')
            else:
                if globalflags.DataSource() == 'data' or DetFlags.digitize.RPC_on():
                    setDefault(self.RpcCablingMode, 'new')
                else:
                    setDefault(self.RpcCablingMode, 'auto')
         
#        if globalflags.DataSource() == 'data':
#            # 2010Jan7th - move to new cabling also when the trigger (i.e. LVL1 emul. or HLT algorithms) is on
#            #if doTrigger==True  or any(re.match(tag,globalflags.ConditionsTag()) for tag in problematic_tags_mdtrpc):
#            if any(re.match(tag,globalflags.ConditionsTag()) for tag in problematic_tags_mdtrpc):
#                setDefault(self.RpcCablingMode, 'old')  # default is old when using trigger
##                if doTrigger:
##                    logMuon.info("RPC cabling: Setting to OLD because of trigger settings")
##                else:
#                logMuon.info("RPC cabling: Setting to OLD because of condition data tags not including RPC cabling maps")
#            else:
#                setDefault(self.RpcCablingMode, 'new')
#                logMuon.info("RPC cabling: Setting to NEW")
#        else:                         
#            setDefault(self.RpcCablingMode, 'sim') 
#            logMuon.info("RPC cabling: Setting to SIM")
      
        #TGC 
        setDefault(self.TgcCablingMode, 'auto')

        # do not use the call-back for digitization job
        #use the call-back in the overlay job that has no bytestream data as input
        # and avoid access to Db for problematic tags
        
        if DetFlags.digitize.TGC_on() and not globalflags.isOverlay():
            if any(re.match(tag,globalflags.ConditionsTag()) for tag in problematic_tags_tgc):
                setDefault(self.TgcCablingMode, 'old 12-fold')
            else:
                setDefault(self.TgcCablingMode, '12-fold')


        # avoid to use call-back if input is bytestream or data and avoid access to Db for problematic tags
        if globalflags.DataSource.is_data() or globalflags.InputFormat.is_bytestream():
            if any(re.match(tag,globalflags.ConditionsTag()) for tag in problematic_tags_tgc):
                setDefault(self.TgcCablingMode, 'old 12-fold')
            else:
                setDefault(self.TgcCablingMode, '12-fold')


        # call-back works for RDO files only and avoid access to Db for problematic tags
        if self.TgcCablingMode()=='auto' and not DetFlags.haveRDO.TGC_on():
            if any(re.match(tag,globalflags.ConditionsTag()) for tag in problematic_tags_tgc):
                setDefault(self.TgcCablingMode, 'old 12-fold')
            else:
                setDefault(self.TgcCablingMode, '12-fold')


jobproperties.add_Container(MuonCnv)
muonCnvFlags = jobproperties.MuonCnv

class MdtCablingMode(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues=['new']
    StoredValue='new' 

muonCnvFlags.add_JobProperty(MdtCablingMode)


class RpcCablingMode(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues=['sim','old','new', 'auto']
    StoredValue='new'

muonCnvFlags.add_JobProperty(RpcCablingMode)


class TgcCablingMode(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues=['auto','8-fold','12-fold', 'old 12-fold']
    StoredValue='auto'

muonCnvFlags.add_JobProperty(TgcCablingMode)




# at the end, set the defaults
muonCnvFlags.setDefaults()
