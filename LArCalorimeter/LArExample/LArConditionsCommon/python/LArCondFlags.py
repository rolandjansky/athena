# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#======================================================================
# File:   LArConditionsCommon/python/LArConditionsFlags.py
#=======================================================================
""" Flags LAr Conditions Data 
"""
#
#
__author__  = 'H. Ma'
__version__="$Revision: 1.14 $"
__doc__="LAr Conditions flags . "

"""

usage:

from LArConditionsCommon.LArCondFlags import larCondFlags 
# set flags 
larCondFlags.XXX=YYY

# Valid flags are: 

name                             type
--------------------------------------------
LArfSamplG4Phys                   str
LArDBConnection                   str
LArCoolChannelSelection           str
LArForceIOVRunNumber              str
OFCShapeFolder                    str
LArCondFolderTags,                dict
hasMphys,                         bool 
hasHVCorr,                        bool
SingleVersion,                    bool
SingleVersionKey,                 str
useShape                          bool
DDVtoElecCalibMCTag               dict
DDVtoOnOffIdMCTag                 dict
DDVtoFebRodIdMCTag                dict
useMCShape                        bool
useOFCOnlyCoolChannelSelection    bool
ua2MeVFolder                      str
MphysOverMcalFolder               str
fSamplTag                         str #Only used in overlay
methods
-------------------------------------------
config_ElecCalibMC(): sets flags for MC calibration constants 
config_idmap_MC(): sets flags for MC id map
config_idmap_comm(): sets flags for Commissioning idmap
addTag(folder,conddb):  add tag if any for this folder

This flag object is used by the following jobOptions to set the IOV DB Folders. 
LArConditionsCommon_MC_jobOptions.py
LArConditionsCommon_comm_jobOptions.py
LArIdMap_comm_jobOptions.py
LArIdMap_MC_jobOptions.py


example:
-- specify explicitly an ElecCalibMC tag

larCondFlags.LArCondFolderTags()['/LAR/ElecCalibMC']='LARElecCalibMC-CSC02-K-QGSP_BERT'


"""


#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class LArfSamplG4Phys(JobProperty):
    ### MC tag suffix for G4 Physics (EMV or BERT)
    
    statusOn=True
    allowedTypes=['str']
    StoredValue="QGSP_BERT"

class LArDBConnection(JobProperty):
    ### explicit db server 
    ###
    statusOn=False
    allowedTypes=['str']
    StoredValue=""

class LArElecCalibSqlite(JobProperty):
    ### sqlite file containing electronic calibration values
    ###
    statusOn=False
    allowedTypes=['str']
    StoredValue=""

    
class LoadElecCalib(JobProperty):
    ### allow to switch off the loading of electronic calibration constants
    ###
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class LArCoolChannelSelection(JobProperty):
    ### Select COOL channels to be loaded
    ###
    statusOn=False
    allowedTypes=['str']
    StoredValue=""

class LArForceIOVRunNumber(JobProperty):
    ### Load conditions with this run-number
    ###
    statusOn=False
    allowedTypes=['str']
    StoredValue=""

class OFCShapeFolder(JobProperty):
    ### Folder name for Optimal Filtering coefficients
    ###
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

class hasMphys(JobProperty):
    ### flag for including Mphys folder or not
    ###   some old DB tag does not contain this folder
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class hasHVCorr(JobProperty):
    ### flag to include HVCorr folder in MC or not
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class useShape(JobProperty):
    ### flag for including Shape folder or not for Commissioning
    ###  
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True

class useMCShape(JobProperty):
    ### Flag for including MC Shape folder or not
    ###
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True   

class SingleVersion(JobProperty):
    ### add SingleVersion folders to IOVDbSvc (acutally not use any more)
    ###  
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class LArCondFolderTags(JobProperty):
    ### explicit tag for some folders 
    ###
    statusOn=True
    allowedTypes=['dict']
    StoredValue={}

class LArfSamplTag(JobProperty):
    ## Used only in overlay! 
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

class DDVtoElecCalibMCTag(JobProperty):
    ### explicit tag for some folders 
    ###
    statusOn=True
    allowedTypes=['dict']
    StoredValue= {'Rome':'InitLayout-A',
                  'DC2':'DC2-D',
                  'ATLAS-DC3-00':'DC3-A',
                  'ATLAS-DC3-01':'DC3-A',
                  'ATLAS-DC3-02':'DC3-A',
                  'ATLAS-DC3-04':'DC3-B',
                  'ATLAS-DC3-05':'DC3-B-IdFix1',
                  'CTB':'DC3-B-IdFix1',
                  'ATLAS-CSC-01':'DC3-B-IdFix1',
                  'ATLAS-CSC-02-QGSP_BERT':'CSC02-G-QGSP_BERT',
                  'ATLAS-CSC-02-QGSP_EMV':'CSC02-E-QGSP_EMV',
                  'ATLAS-CSC-03-QGSP_BERT':'CSC02-J-QGSP_BERT',
                  'ATLAS-CSC-04-QGSP_BERT':'CSC02-J-QGSP_BERT',
                  'ATLAS-CSC-05-QGSP_BERT':'CSC02-J-QGSP_BERT',
                  'ATLAS-CSC-03-QGSP_EMV':'CSC02-J-QGSP_EMV',
                  'ATLAS-CSC-04-QGSP_EMV':'CSC02-J-QGSP_EMV',
                  'ATLAS-CSC-05-QGSP_EMV':'CSC02-J-QGSP_EMV',
                  'ATLAS-GEO-01-QGSP_EMV':'CSC02-J-QGSP_EMV',
                  'ATLAS-GEO-01-QGSP_BERT':'CSC02-J-QGSP_BERT',
                  'ATLAS-GEO-02-QGSP_EMV':'CSC02-J-QGSP_EMV',
                  'ATLAS-GEO-02-QGSP_BERT':'CSC02-J-QGSP_BERT',
                  'ATLAS-GEO-03-QGSP_EMV':'CSC02-J-QGSP_EMV',
                  'ATLAS-GEO-03-QGSP_BERT':'CSC02-J-QGSP_BERT',
                  'ATLAS-GEO-04-QGSP_EMV':'CSC02-J-QGSP_EMV',
                  'ATLAS-GEO-04-QGSP_BERT':'CSC02-J-QGSP_BERT',
                  'ATLAS-GEO-05-QGSP_EMV':'CSC02-J-QGSP_EMV',
                  'ATLAS-GEO-05-QGSP_BERT':'CSC02-J-QGSP_BERT',
                  'ATLAS-GEO-06-QGSP_BERT':'CSC02-N-QGSP_BERT_BIRK',
                  'ATLAS-GEO-07-QGSP_BERT':'CSC02-N-QGSP_BERT_BIRK',
                  'ATLAS-GEO-08-QGSP_BERT':'CSC02-N-QGSP_BERT_BIRK',
                  'ATLAS-GEO-09-QGSP_BERT':'CSC02-N-QGSP_BERT_BIRK',
                  'ATLAS-GEO-10-QGSP_BERT':'CSC02-N-QGSP_BERT_BIRK',
                  'ATLAS-GEO-QGSP_BERT':'CSC02-N-QGSP_BERT_BIRK',
                  'ATLAS-GEO-QGSP_EMV':'CSC02-J-QGSP_EMV',
                  'ATLAS-Comm-QGSP_BERT':'CSC02-G-QGSP_BERT',
                  'ATLAS-Comm-QGSP_EMV':'CSC02-E-QGSP_EMV'
                  }

class DDVtoOnOffIdMCTag(JobProperty):
    ### explicit tag for some folders 
    ###
    statusOn=True
    allowedTypes=['dict']
    StoredValue= {#'Rome':'OnOffIdAtlas-000',
                  #'DC2':'OnOffIdAtlas-000',
                  #'ATLAS-DC3-00':'OnOffIdAtlas-000',
                  #'ATLAS-DC3-01':'OnOffIdAtlas-000',
                  #'ATLAS-DC3-02':'OnOffIdAtlas-000',
                  #'ATLAS-DC3-04':'OnOffIdAtlas-000',
                  'ATLAS-DC3-05':'LARIdentifierOnOffIdMap-002',
                  'ATLAS-CSC-01':'LARIdentifierOnOffIdMap-002',
                  'ATLAS-CSC-02':'LARIdentifierOnOffIdMap-012',
                  'ATLAS-CSC-03':'LARIdentifierOnOffIdMap-012',
                  'ATLAS-CSC-04':'LARIdentifierOnOffIdMap-012',
                  'ATLAS-CSC-05':'LARIdentifierOnOffIdMap-012',
                  'ATLAS-GEO':'LARIdentifierOnOffIdMap-012',
                  'ATLAS-IBL':'LARIdentifierOnOffIdMap-012',
                  'ATLAS-SLHC':'LARIdentifierOnOffIdMap-012',
                  'ATLAS':'LARIdentifierOnOffIdMap-012',
                  #'ATLAS-Comm':'OnOffIdAtlas-012',
                  }


class DDVtoCalibIdMCTag(JobProperty):
    ### explicit tag for some folders 
    ###
    statusOn=True
    allowedTypes=['dict']
    StoredValue= {'ATLAS-DC3-05':'LARIdentifierCalibIdMap-002',
                  'ATLAS-CSC-01':'LARIdentifierCalibIdMap-002',
                  'ATLAS-CSC-02':'LARIdentifierCalibIdMap-012',
                  'ATLAS-CSC-03':'LARIdentifierCalibIdMap-012',
                  'ATLAS-CSC-04':'LARIdentifierCalibIdMap-012',
                  'ATLAS-CSC-05':'LARIdentifierCalibIdMap-012',
                  'ATLAS-GEO':'LARIdentifierCalibIdMap-012',
                  'ATLAS-IBL':'LARIdentifierCalibIdMap-012',
                  'ATLAS-SLHC':'LARIdentifierCalibIdMap-012',
                  'ATLAS':'LARIdentifierCalibIdMap-012',
                  }


class DDVtoFebRodIdMCTag(JobProperty):
    ### explicit tag for some folders 
    ###
    statusOn=True
    allowedTypes=['dict']
    StoredValue= {#'Rome':'FebRodAtlas-000',
                  #'DC2':'FebRodAtlas-000',
                  #'ATLAS-DC3-00':'FebRodAtlas-000',
                  #'ATLAS-DC3-01':'FebRodAtlas-000',
                  #'ATLAS-DC3-02':'FebRodAtlas-000',
                  #'ATLAS-DC3-04':'FebRodAtlas-000',
                  'ATLAS-DC3-05':'LARIdentifierFebRodMap-005',
                  'ATLAS-CSC-01':'LARIdentifierFebRodMap-005',
                  'ATLAS-CSC-02':'LARIdentifierFebRodMap-005',
                  'ATLAS-CSC-03':'LARIdentifierFebRodMap-005',
                  'ATLAS-CSC-04':'LARIdentifierFebRodMap-005',
                  'ATLAS-CSC-05':'LARIdentifierFebRodMap-005',
                  'ATLAS-GEO':'LARIdentifierFebRodMap-005',
                  'ATLAS-IBL':'LARIdentifierFebRodMap-005',
                  'ATLAS-SLHC':'LARIdentifierFebRodMap-005',
                  'ATLAS-Comm-Sim':'LARIdentifierFebRodMap-005',
                  'ATLAS':'LARIdentifierFebRodMap-005',
                  #'ATLAS-Comm':'LARIdentifierFebRodAtlas-Comm-01',
                  }

class useOFCOnlyCoolChannelSelection(JobProperty):
    ###  Apply Cool channel selection only to OFC and Shape
    ###    and not to Pedestal and Ramp
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class ua2MeVFolder(JobProperty):
    ###  Allows to overwrite folder for ua2MeV, base path name is /LAR/ElecCalibOfl
    statusOn=True
    allowedType=['string']
    StoredValue=""

class MphysOverMcalFolder(JobProperty):
    ###  Allows to overwrite folder for MphysOverMcalFolder, base path name is /LAR/ElecCalibOfl
    statusOn=True
    allowedType=['string']
    StoredValue=""



class useLArFEBGainThresholds(JobProperty):
    ###  Use FEB configuration DB to decide if a channel is use only in MED/LOW gain
    ###  In this case the MED gain ramp offset is not used
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class LArCondFlags(JobPropertyContainer):
    ### The LAr conditions flag/job property  container.
    ###

    def config_ElecCalibMC(self) :
        ### set flags and tags for MC
        ###

        if "/LAR/BadChannels/BadChannels" in self.LArCondFolderTags():
            self._log.info(' using user specified tag for /LAR/BadChannels/BadChannels ' + self.LArCondFolderTags()['/LAR/BadChannels/BadChannels'])

        # don't override anymore, use default tag linked from global tag
        #else :
        #    self.LArCondFolderTags()['/LAR/BadChannels/BadChannels']='LARBadChannelsBadChannels-MC-empty'

        if "/LAR/ElecCalibMC" in self.LArCondFolderTags():
            self._log.info(' using user specified tag for /LAR/ElecCalibMC' + self.LArCondFolderTags()['/LAR/ElecCalibMC'])
            return

        from AthenaCommon.JobProperties import jobproperties
        theDDV =  jobproperties.Global.DetDescrVersion()
        self._log.info(theDDV)
        # set up sampling fraction according to physics list in digit configuration
        from AthenaCommon.DetFlags import DetFlags
        _digiFlagAvailable=True

        try:
            from Digitization.DigitizationFlags import jobproperties
        except Exception:
            self._log.warning(" failed to import Digitization.DigitizationFlags, will not get physics list from Digitization.DigitizationFlags ")
            _digiFlagAvailable=False              

        if _digiFlagAvailable  and DetFlags.digitize.any_on() :
            from Digitization.DigitizationFlags import jobproperties
            physicsList = jobproperties.Digitization.physicsList()
            self.LArfSamplG4Phys = 'QGSP_BERT'
            if (physicsList.find("EMV")>0):
                self.LArfSamplG4Phys = 'QGSP_EMV'


        DDVtype=""
        has_mphys=True
        do_override=False

        if theDDV.startswith("Rome"):
            DDVtype="Rome"
            has_mphys=False
            do_override=True
        elif theDDV.startswith("DC2") or theDDV.startswith("CTB"):
            DDVtype="DC2"
            has_mphys=False
            do_override=True
        elif theDDV.startswith("ATLAS-DC3"):
            DDVtype=theDDV
            has_mphys=False
            do_override=True
        elif theDDV.startswith("ATLAS-CSC"):
            DDVtype=theDDV[0:12] # for example "ATLAS-CSC-01"            
            try:    
                versionCSC = int(theDDV[10:12])
            except Exception:
                self._log.error('  Unknown CSC version '+jobproperties.Global.DetDescrVersion())
                versionCSC = -1
            do_override=True
            if versionCSC>=2 :
                DDVtype=DDVtype+'-'+self.LArfSamplG4Phys()
            if versionCSC<3 :
                has_mphys=False
            
        elif theDDV.startswith("ATLAS-GEO"):
            DDVtype = theDDV[0:9] + theDDV[ theDDV.find('-',8) : theDDV.find('-',8)+3 ]
            DDVtype=DDVtype+'-'+self.LArfSamplG4Phys()
            versionGEO = int(theDDV[ theDDV.find('-',8)+1 : theDDV.find('-',8)+3 ])
            self._log.info(' ATLAS-GEO version %d ',versionGEO)
            if versionGEO < 8 :
                do_override=True
        elif theDDV.startswith("ATLAS-Comm"):
            DDVtype=theDDV[0:10] # use "ATLAS-Comm"
            DDVtype=DDVtype+'-'+self.LArfSamplG4Phys()
            do_override=True
            has_mphys=False

        self.hasMphys=has_mphys
        
        # add to the map
        if do_override:
            if DDVtype=="" or DDVtype not in self.DDVtoElecCalibMCTag():

                self._log.error(' unable to find a proper DDV type  for LAr conditions data, DDV '+theDDV+', DDVtype='+DDVtype+',  set DDVtype to the latest ')
                DDVtype="ATLAS-GEO"+'-'+self.LArfSamplG4Phys()

            LArIOVDbTag = 'LARElecCalibMC-'+self.DDVtoElecCalibMCTag()[DDVtype]
            self._log.info(' apply /LAR/ElecCalibMC tag override with %s ',LArIOVDbTag)
            self.LArCondFolderTags()['/LAR/ElecCalibMC']=LArIOVDbTag
        else:
            self._log.info(' use global tag for all LArElecCalibMC constants ')

        return 


    def config_idmap_MC(self) :
        ### set flags and tags for for IdMap for MC
        ###

        from AthenaCommon.JobProperties import jobproperties
        theDDV =  jobproperties.Global.DetDescrVersion()

        DDVtype="ATLAS"

        if theDDV.startswith("Rome"):
            DDVtype="Rome"
        elif theDDV.startswith("DC2") or theDDV.startswith("CTB"):
            DDVtype="DC2"
        elif theDDV.startswith("ATLAS-DC3"):
            DDVtype=theDDV
        elif theDDV.startswith("ATLAS-CSC"):
            DDVtype=theDDV[0:12] # for example "ATLAS-CSC-01"            
            try:    
                int(theDDV[10:12])
            except Exception:
                self._log.error('  Unknown CSC version '+jobproperties.Global.DetDescrVersion())
            
        elif theDDV.startswith("ATLAS-GEO"):
            DDVtype=theDDV[0:9] # use "ATLAS-GEO"

        elif theDDV.startswith("ATLAS-IBL"):
            DDVtype=theDDV[0:9] # use "ATLAS-IBL"

        elif theDDV.startswith("ATLAS-SLHC"):
            DDVtype=theDDV[0:10] # use "ATLAS-SLHC"

        elif theDDV.startswith("ATLAS-Comm"):
            DDVtype=theDDV[0:10] # use "ATLAS-Comm"

        # set the tag for OnOffIdMap
        if "/LAR/Identifier/OnOffIdMap" in self.LArCondFolderTags():
            self._log.info(' using user specified tag for /LAR/Identifier/OnOffIdMap %s' , self.LArCondFolderTags()['/LAR/Identifier/OnOffIdMap'])
        else :
            if DDVtype=="" or DDVtype not in self.DDVtoOnOffIdMCTag():
                self._log.error(' unable to find a proper DDV type  for LArOnOffIdMap, DDV '+theDDV+', DDVtype='+DDVtype+',  set DDVtype to the latest ')
                DDVtype="ATLAS-GEO"

            tag = self.DDVtoOnOffIdMCTag()[DDVtype]
            self.LArCondFolderTags()['/LAR/Identifier/OnOffIdMap']=tag

        # set the tag for CalibIdMap
        if "/LAR/Identifier/CalibIdMap" in self.LArCondFolderTags():
            self._log.info(' using user specified tag for /LAR/Identifier/CalibIdMap %s' , self.LArCondFolderTags()['/LAR/Identifier/CalibIdMap'])
        else :
            if DDVtype=="" or DDVtype not in self.DDVtoCalibIdMCTag():
                self._log.error(' unable to find a proper DDV type  for LArCalibIdMap, DDV '+theDDV+', DDVtype='+DDVtype+',  set DDVtype to the latest ')
                DDVtype="ATLAS-GEO"

            tag = self.DDVtoCalibIdMCTag()[DDVtype]
            self.LArCondFolderTags()['/LAR/Identifier/CalibIdMap']=tag
            
            
        # set the tag for FebRodMap
        if "/LAR/Identifier/FebRodMap" in self.LArCondFolderTags():
            self._log.info(' using user specified tag for /LAR/Identifier/FebRodMap %s' , self.LArCondFolderTags()['/LAR/Identifier/FebRodMap'])

        else :
            if DDVtype=="" or DDVtype not in self.DDVtoFebRodIdMCTag():
                self._log.error(' unable to find a proper DDV type  for LArFebRodMap, DDV '+theDDV+', DDVtype='+DDVtype+',  set DDVtype to the latest ')
                DDVtype="ATLAS-GEO"

            # a special case:
            if DDVtype=="ATLAS-Comm" :
                if 'LArCosmicFlags' in dir() and hasattr(LArCosmicFlags,'Rel1300xSim') and LArCosmicFlags.Rel1300xSim :  # noqa: F821
                    # a special fix for rel 13 cosmic MC events
                    DDVtype=DDVtype+"Sim"

            tag = self.DDVtoFebRodIdMCTag()[DDVtype]
            self.LArCondFolderTags()['/LAR/Identifier/FebRodMap']=tag
            
        return 

    def addTag(self, folder,conddb) :
        ### if there is a tag for this folder, set it to conddb 
        if folder in self.LArCondFolderTags():
            tag = self.LArCondFolderTags()[folder]
            self._log.info(' setting folder '+ folder + ' with tag '+ tag )
            conddb.addOverride(folder,tag)


#############################
# fill in the container, add it to global 
######################################           

jobproperties.add_Container(LArCondFlags)

_list_LArCond=[LArfSamplG4Phys,LArDBConnection,
               LArCoolChannelSelection,LArForceIOVRunNumber,OFCShapeFolder,LArElecCalibSqlite,
               LArCondFolderTags,DDVtoElecCalibMCTag,hasMphys,hasHVCorr,SingleVersion,useShape,useMCShape,LArfSamplTag,
               DDVtoOnOffIdMCTag,DDVtoFebRodIdMCTag, DDVtoCalibIdMCTag, LoadElecCalib,useOFCOnlyCoolChannelSelection,ua2MeVFolder,useLArFEBGainThresholds,MphysOverMcalFolder
              ]


for j in _list_LArCond:
    jobproperties.LArCondFlags.add_JobProperty(j)

del _list_LArCond

larCondFlags =jobproperties.LArCondFlags
