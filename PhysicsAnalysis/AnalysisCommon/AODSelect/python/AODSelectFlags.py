# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# =============================================================================
# Name:        AODSelectFlags
#
# Author:      Karsten Koeneke (CERN)
# Created:     October 2011
#
# Description: Here, all neccessary job flags for the AODSelect
#              are defined.
#
# =============================================================================

__doc__ = """Here, all neccessary job flags for the AODSelect are defined."""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@cern.ch>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
from RecExConfig.RecoFunctions import AddValidItemToList,RemoveValidItemFromList

# =====================================================================
# First define container for the D2PDMaker flags  
# =====================================================================
class AODSelectFlags(JobPropertyContainer):
    """ The D2PDMaker flag/job property  container."""
jobproperties.add_Container(AODSelectFlags)

#short-cut to get the AODSelectFlags container with this one line:
#'from D2PDMaker.AODSelectFlags import AODSelectFlags'
#Note that name has to be different to avoid problems with pickle
AODSelectFlags = jobproperties.AODSelectFlags

# =====================================================================
# Now define each flag and add it to the container
# =====================================================================

# ------------------------
# Flags to control the AODSelect scheduling
# ------------------------
class preFilterSequence(JobProperty):
    """ Only run AODSelect for events that pass this pre-filter sequence; add your pre-filter algs here """
    statusOn       = True
    allowedTypes   = ['list']
    StoredValue    = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(preFilterSequence)

class doAODSelect(JobProperty):
    """ Only run the intricate AODSelect python job configuration if any of the other flags is set to True """
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    pass
jobproperties.AODSelectFlags.add_JobProperty(doAODSelect)

class doAll(JobProperty):
    """ Turn on or off the creation of everything """
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    pass
jobproperties.AODSelectFlags.add_JobProperty(doAll)

class isSim(JobProperty):
    """ Turn on or off the creation of everything """
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    pass
jobproperties.AODSelectFlags.add_JobProperty(isSim)

class truthParticleContainer(JobProperty):
    """ Turn on or off the creation of everything """
    statusOn       = True
    allowedTypes   = ['str']
    StoredValue    = ""
    pass
jobproperties.AODSelectFlags.add_JobProperty(truthParticleContainer)

class mcEventCollection(JobProperty):
    """ Turn on or off the creation of everything """
    statusOn       = True
    allowedTypes   = ['str']
    StoredValue    = ""
    pass
jobproperties.AODSelectFlags.add_JobProperty(mcEventCollection)

class printAODSelect(JobProperty):
    """ If set to True, printing all available AODSelect containers """
    statusOn          = True
    allowedTypes      = ['bool']
    StoredValue       = False
    ExitAfterPrintout = False 
    pass
jobproperties.AODSelectFlags.add_JobProperty(printAODSelect)


# =====================================================================
# Create the base class job properties for the three different types of flags
# =====================================================================

class buildingBaseProperty(JobProperty):
    """ Base class for all job properties for building new containers in the AODSelect environment """
    statusOn                   = True
    allowedTypes               = ['bool']
    StoredValue                = False
    inputTypeAndKeyList        = []
    createOutputCollection     = False
    createOutputLinkCollection = False
    outputCollectionPrefix     = ""
    def add(self, inputItem="", createOutputCollection=None, createOutputLinkCollection=None ):
        if self.is_locked():
            self._log.info('The JobProperty %s is blocked' % self.__name__)
        else:
            if inputItem != "":
                if not self.inputTypeAndKeyList.__contains__(inputItem): self.inputTypeAndKeyList.append(inputItem)
                if createOutputCollection!=None: self.createOutputCollection = createOutputCollection or self.createOutputCollection
                if createOutputLinkCollection!=None: self.createOutputLinkCollection = createOutputLinkCollection or self.createOutputLinkCollection
                if len(self.inputTypeAndKeyList)>0 and ( self.createOutputCollection or self.createOutputLinkCollection ):
                    self.StoredValue = True
                    AODSelectFlags.doAODSelect = True
                    pass
                pass
            else:
                self._log.warning('We got an empty inputItem string for the JobProperty %s... doing nothing!' % self.__name__)
                pass
            pass
        return
    pass

class flaggingBaseProperty(JobProperty):
    """ Base class for all job properties for flagging an object in the AODSelect environment """
    statusOn            = True
    allowedTypes        = ['bool']
    StoredValue         = False
    inputTypeAndKeyList = []
    varNameList         = []
    def add(self, inputItem="", varNameList=[] ):
        if self.is_locked():
            self._log.info('The JobProperty %s is blocked' % self.__name__)
        else:
            if inputItem != "":
                self.StoredValue = True
                AODSelectFlags.doAODSelect = True
                if not self.inputTypeAndKeyList.__contains__(inputItem): self.inputTypeAndKeyList.append(inputItem)
                for varName in varNameList:
                    if not self.varNameList.__contains__(varName):
                        self.varNameList.append(varName)
                        pass
                    pass
                pass
            else:
                self._log.warning('We got an empty inputItem string for the JobProperty %s... doing nothing!' % self.__name__)
                pass
            pass
        return
    pass

class matchingBaseProperty(JobProperty):
    """ Base class for all job properties for matching one object to a list of others in the AODSelect environment """
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = False
    matchingDictionary = {}
    defaultDeltaR      = 0.6
    defaultMatchToItem = ""
    def add(self, inputItem="", matchToItemList=[], matchDistanceList=[] ):
        if self.is_locked():
            self._log.info('The JobProperty %s is blocked' % self.__name__)
        else:
            if inputItem == "":
                self._log.warning('We got an empty inputItem string for the JobProperty %s... doing nothing!' % self.__name__)
                return
            if len(matchToItemList)==0 and self.defaultMatchToItem == "" :
                self._log.warning('We got an empty matchToItemList for the JobProperty %s... doing nothing!' % self.__name__)
                return
            if len(matchToItemList)!=0 and len(matchDistanceList)!=0 and len(matchToItemList)!=len(matchDistanceList):
                self._log.warning("We got non-empty lists with different lenght for the ",
                                  "JobProperty %s... doing nothing! len(matchToItemList)=%s, len(matchDistanceList)=%s"
                                  % (self.__name__, len(matchToItemList), len(matchDistanceList)) )
                return
            # OK, we seem to have correct inputs... now, update the information
            self.StoredValue = True
            AODSelectFlags.doAODSelect = True
            # Prepare the final list of pairs of [ matchToItem, matchDistance ]
            finalMatchToItemList = []
            for idx in xrange( len(matchToItemList) ):
                matchToItem = matchToItemList[idx]
                if len(matchDistanceList)==0 :
                    finalMatchToItemList.append( [matchToItem, self.defaultDeltaR] )
                    pass
                else:
                    finalMatchToItemList.append( [matchToItem, matchDistanceList[idx]] )
                    pass
                pass
            if self.matchingDictionary.has_key(inputItem):
                valueList = self.matchingDictionary[inputItem]
                for newPair in finalMatchToItemList:
                    if not valueList.__contains__(newPair):
                        valueList.append(newPair)
                        pass
                    pass
                self.matchingDictionary[inputItem] = valueList
                pass
            else:
                self.matchingDictionary[inputItem] = finalMatchToItemList
                pass
            pass
        return
    pass




# =====================================================================
# All actual switches to produce (or not = default) certain things
# =====================================================================

# -----------------------------------------------------------------
# The flags to produce certain objects beforehand
# -----------------------------------------------------------------
class doRunJetFinding(JobProperty):
    """ Turn on or off the creation of low-pt jet finding """
    statusOn             = True
    allowedTypes         = ['bool']
    StoredValue          = False
    forceRerunJetFinding = False
    pass
jobproperties.AODSelectFlags.add_JobProperty(doRunJetFinding)


# -----------------------------------------------------------------
# The flagging flags: to attach new variables to muons
# -----------------------------------------------------------------
class doMuonSelectorFlagging(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doMuonSelectorFlagging)

class doCaloMuonSelectorFlagging(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doCaloMuonSelectorFlagging)

class doStandAloneMuonSelectorFlagging(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doStandAloneMuonSelectorFlagging)


# -----------------------------------------------------------------
# The flagging flags: to attach new variables to an egamma
# -----------------------------------------------------------------
class doEGammaCaloIsolationFlagging(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    varNameList         = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doEGammaCaloIsolationFlagging)

class doElectronMultiLeptonIDFlagging(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doElectronMultiLeptonIDFlagging)

class doElectronLikelihoodIDFlagging_VeryLoose(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doElectronLikelihoodIDFlagging_VeryLoose)

class doElectronLikelihoodIDFlagging_Loose(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doElectronLikelihoodIDFlagging_Loose)

class doElectronLikelihoodIDFlagging_Medium(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doElectronLikelihoodIDFlagging_Medium)

class doElectronLikelihoodIDFlagging_Tight(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doElectronLikelihoodIDFlagging_Tight)

class doElectronLikelihoodIDFlagging_VeryTight(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doElectronLikelihoodIDFlagging_VeryTight)

class doElectronLikelihoodIDFlagging_Loose_BL_Pix(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doElectronLikelihoodIDFlagging_Loose_BL_Pix)



# -----------------------------------------------------------------
# The flagging flags: to attach new variables to an object/particle
# -----------------------------------------------------------------
class doImpactParameterFlagging(flaggingBaseProperty):
    """ If set to True, calculate and flag """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doImpactParameterFlagging)


# -----------------------------------------------------------------
# The building flags: to build new output (link) containers
# -----------------------------------------------------------------
class doVeryLooseCleanJetBuilding(buildingBaseProperty):
    """ If set to True, calculate """
    inputTypeAndKeyList        = []
    createOutputCollection     = False
    createOutputLinkCollection = False
    outputCollectionPrefix     = "VeryLooseClean"
    pass
jobproperties.AODSelectFlags.add_JobProperty(doVeryLooseCleanJetBuilding)

class doLooseCleanJetBuilding(buildingBaseProperty):
    """ If set to True, calculate """
    inputTypeAndKeyList        = []
    createOutputCollection     = False
    createOutputLinkCollection = False
    outputCollectionPrefix     = "LooseClean"
    pass
jobproperties.AODSelectFlags.add_JobProperty(doLooseCleanJetBuilding)

class doMediumCleanJetBuilding(buildingBaseProperty):
    """ If set to True, calculate """
    inputTypeAndKeyList        = []
    createOutputCollection     = False
    createOutputLinkCollection = False
    outputCollectionPrefix     = "MediumClean"
    pass
jobproperties.AODSelectFlags.add_JobProperty(doMediumCleanJetBuilding)

class doTightCleanJetBuilding(buildingBaseProperty):
    """ If set to True, calculate """
    inputTypeAndKeyList        = []
    createOutputCollection     = False
    createOutputLinkCollection = False
    outputCollectionPrefix     = "TightClean"
    pass
jobproperties.AODSelectFlags.add_JobProperty(doTightCleanJetBuilding)


# -----------------------------------------------------------------
# The flagging flags: to attach new variables to an object/particle
# -----------------------------------------------------------------
class doVeryLooseCleanJetFlagging(flaggingBaseProperty):
    """ If set to True, calculate """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doVeryLooseCleanJetFlagging)

class doLooseCleanJetFlagging(flaggingBaseProperty):
    """ If set to True, calculate """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doLooseCleanJetFlagging)

class doMediumCleanJetFlagging(flaggingBaseProperty):
    """ If set to True, calculate """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doMediumCleanJetFlagging)

class doTightCleanJetFlagging(flaggingBaseProperty):
    """ If set to True, calculate """
    inputTypeAndKeyList = []
    pass
jobproperties.AODSelectFlags.add_JobProperty(doTightCleanJetFlagging)



# -----------------------------------------------------------------
# The matching flags: to match one object to another
# -----------------------------------------------------------------
class doOfflineMatching(matchingBaseProperty):
    """ If set to True, calculate """
    matchingDictionary = {}
    defaultDeltaR      = 0.6
    defaultMatchToItem = ""
    pass
jobproperties.AODSelectFlags.add_JobProperty(doOfflineMatching)


class doTriggerMatching(matchingBaseProperty):
    """ If set to True, calculate """
    matchingDictionary        = {}
    defaultDeltaR             = 0.15
    matchToPassedTriggersOnly = True
    defaultMatchToItem        = ""
    pass
jobproperties.AODSelectFlags.add_JobProperty(doTriggerMatching)


class doTruthMatching(matchingBaseProperty):
    """ If set to True, calculate """
    matchingDictionary = {}
    writeUserData      = True
    defaultMatchToItem = "SpclMC"
    pass
jobproperties.AODSelectFlags.add_JobProperty(doTruthMatching)

