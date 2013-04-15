# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# =============================================================================
# Name:        AODSelectFlags
#
# Author:      Karsten Koeneke (CERN)
# Created:     October 2011
#
# Description: Here, some helper functions for AODSelect are defined.
#
# =============================================================================

__doc__ = """Here, some helper functions for AODSelect are defined."""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@cern.ch>"

# Create a logger
from AthenaCommon.Logging import logging
logAODSelect_helpers = logging.getLogger( 'AODSelect_helpers' )
#logAODSelect_helpers.setLevel(logging.DEBUG)

# Import the AODSelectFlags to steer the job
from AODSelect.AODSelectFlags import AODSelectFlags
from AthenaCommon import CfgMgr

# Import the objKeyStore to be able to see what collections are available
from RecExConfig.ObjKeyStore import objKeyStore


class AODSelectConfiguration(object):
    """
    Class to hold the final configuration of the AODSelect framework
    """

    def __init__(self, name="AODSelectConfiguration"):
        object.__init__(self)
        self.name = name
        self._algConfigList = []
        self.algDict = { "JetCollection" : CfgMgr.D2PDJetSelector,
                         "Analysis::TauJetContainer" : CfgMgr.D2PDTauSelector,
                         "Analysis::MuonContainer" : CfgMgr.D2PDMuonSelector,
                         "ElectronContainer" : CfgMgr.D2PDElectronSelector,
                         "PhotonContainer" : CfgMgr.D2PDPhotonSelector,
                         "Rec::TrackParticleContainer" : CfgMgr.D2PDTrackParticleSelector,
                         "CaloClusterContainer" : CfgMgr.D2PDINavigable4MomentumSelector,
                         "INavigable4Momentum" : CfgMgr.D2PDINavigable4MomentumSelector,
                         "INav4MomLinkContainer" : CfgMgr.D2PDINavigable4MomentumSelector
                         }
        return


    def doPrint(self):
        for algConfig in self._algConfigList:
            algConfig.doPrint()
            pass
        return



    def addSelectorToolList(self, aodSelectFlag=None, selectorToolList=[] ):
        logAODSelect_helpers.debug( "Entering addSelectorToolList" )
        for selectorTool in selectorToolList:
            retVal = self.addSelectorTool(aodSelectFlag, selectorTool)
            if not retVal: return False
            pass
        logAODSelect_helpers.debug( "Leaving addSelectorToolList" )
        return True

    def addSelectorTool(self, aodSelectFlag=None, selectorTool=None ):
        logAODSelect_helpers.debug( "Entering addSelectorTool" )
        if not aodSelectFlag:
            logAODSelect_helpers.warning( "You gave me an empty flag container... this won't work!" )
            return False
        logAODSelect_helpers.debug( "Have inputTypeAndKeyList=%s, outputCollectionPrefix=%s, createOutputCollection=%s, createOutputLinkCollection=%s, selectorTool=%s "
                                    % (aodSelectFlag.inputTypeAndKeyList,
                                       aodSelectFlag.outputCollectionPrefix, aodSelectFlag.createOutputCollection,
                                       aodSelectFlag.createOutputLinkCollection, selectorTool ) )
        for inputTypeAndKey in aodSelectFlag.inputTypeAndKeyList:
            # Prepare the names for this inputTypeAndKey
            inputType, inputKey = inputTypeAndKey.split("#")
            outPrefix      = aodSelectFlag.outputCollectionPrefix
            outCollKey     = ""
            outLinkCollKey = ""
            if aodSelectFlag.createOutputCollection: outCollKey = outPrefix+inputKey
            if aodSelectFlag.createOutputLinkCollection: outLinkCollKey = outPrefix+"Link"+inputKey

            # Now, find an existing algConfig that can be used or create a new one
            foundAlgConfig = False
            index = 0
            while not foundAlgConfig:
                algConfigName = inputTypeAndKey
                if index > 0: algConfigName = algConfigName + ("_%s" % index)
                algConfig = self.findAlgConfig(algConfigName)
                if algConfig != None:
                    # Check if it already has output (link) collections scheduled
                    if algConfig.outputCollection == algConfig.outputLinkCollection == "":
                        logAODSelect_helpers.debug( "Modifying an existing AODSelectAlgConfig with name: %s" % algConfigName )
                        algConfig.selectorToolList.append(selectorTool)
                        algConfig.outputCollection     = outCollKey
                        algConfig.outputLinkCollection = outLinkCollKey
                        if logAODSelect_helpers.isEnabledFor(logging.DEBUG): algConfig.doPrint()
                        foundAlgConfig = True
                        pass
                    else: # We need to create a new one
                        index += 1
                        foundAlgConfig = False
                        pass
                    pass
                else:
                    # Create a new one and add it
                    logAODSelect_helpers.debug( "Adding a new AODSelectAlgConfig with name: %s" % algConfigName )
                    algConfig = AODSelectAlgConfig(algConfigName)
                    algConfig.setInputTypeAndKey(inputTypeAndKey)
                    algConfig.selectorToolList.append(selectorTool)
                    algConfig.outputCollection     = outCollKey
                    algConfig.outputLinkCollection = outLinkCollKey
                    if logAODSelect_helpers.isEnabledFor(logging.DEBUG): algConfig.doPrint()
                    self.addAlgConfig(algConfig)
                    foundAlgConfig = True
                    pass
                pass
            pass
        logAODSelect_helpers.debug( "Leaving addSelectorTool" )
        return True

    


    def addUserDataToolList(self, aodSelectFlag=None, userDataToolList=[] ):
        logAODSelect_helpers.debug( "Entering addUserDataToolList" )
        for userDataTool in userDataToolList:
            retVal = self.addUserDataTool(aodSelectFlag, userDataTool)
            if not retVal: return False
            pass
        logAODSelect_helpers.debug( "Leaving addUserDataToolList" )
        return True

    def addUserDataTool(self, aodSelectFlag=None, userDataTool=None ):
        logAODSelect_helpers.debug( "Entering addUserDataTool" )
        if not aodSelectFlag:
            logAODSelect_helpers.warning( "You gave me an empty flag container... this won't work!" )
            return False
        for inputTypeAndKey in aodSelectFlag.inputTypeAndKeyList:
            algConfig = self.findAlgConfig(inputTypeAndKey)
            if algConfig == None:
                # Create a new one and add it
                logAODSelect_helpers.debug( "Adding a new AODSelectAlgConfig with name: %s" % inputTypeAndKey )
                algConfig = AODSelectAlgConfig(inputTypeAndKey)
                logAODSelect_helpers.debug( "Adding a new user data tool: %s" % userDataTool )
                algConfig.userDataToolList.append(userDataTool)
                if logAODSelect_helpers.isEnabledFor(logging.DEBUG): algConfig.doPrint()
                self.addAlgConfig(algConfig)
                pass
            else:
                logAODSelect_helpers.debug( "Adding a new user data tool of name %s to AODSelectAlgConfig with name: %s"
                                            % (userDataTool, inputTypeAndKey) )
                algConfig.userDataToolList.append(userDataTool)
                if logAODSelect_helpers.isEnabledFor(logging.DEBUG): algConfig.doPrint()
                pass
            pass
        logAODSelect_helpers.debug( "Leaving addUserDataTool" )
        return True



    def addAssociationToolList(self, inputTypeAndKey="", associationToolList=[] ):
        logAODSelect_helpers.debug( "Entering addAssociationToolList" )
        for assoTool in associationToolList:
            retVal = self.addAssociationTool(inputTypeAndKey, assoTool)
            if not retVal: return False
            pass
        logAODSelect_helpers.debug( "Leaving addAssociationToolList" )
        return True


    def addAssociationTool(self, inputTypeAndKey="", associationTool=None ):
        logAODSelect_helpers.debug( "Entering addAssociationTool" )
        if inputTypeAndKey=="":
            logAODSelect_helpers.warning( "You gave me an empty inputTypeAndKey string... this won't work!" )
            return False
        if not inputTypeAndKey.__contains__("#"):
            logAODSelect_helpers.warning( "The inputTypeAndKey string doesn't contain a '#', but it should: %s" % inputTypeAndKey )
            return False
        if not associationTool:
            logAODSelect_helpers.warning( "You gave me an empty associationTool... this won't work!" )
            return False
        # Create the name of the associationMap container that will be created
        _tmpName = ((associationTool.getName()).split("__"))[-1]
        _outAssoName = (inputTypeAndKey.split("#"))[1] + "_MatchTo_" + _tmpName
        logAODSelect_helpers.debug( "using inputTypeAndKey=%s, _tmpName=%s, _outAssoName=%s" % (inputTypeAndKey, _tmpName, _outAssoName ) )
        # Find the right AODSelectAlgConfig object
        algConfig = self.findAlgConfig(inputTypeAndKey)
        if algConfig == None:
            # Create a new one and add it
            logAODSelect_helpers.debug( "Creating a new AODSelectAlgConfig with name=%s" % (inputTypeAndKey) )
            algConfig = AODSelectAlgConfig(inputTypeAndKey)
            algConfig.associationToolList.append(associationTool)
            algConfig.outputAssociationList.append(_outAssoName)
            self.addAlgConfig(algConfig)
            pass
        else:
            logAODSelect_helpers.debug( "Using an existing AODSelectAlgConfig with name=%s" % (inputTypeAndKey) )
            algConfig.associationToolList.append(associationTool)
            algConfig.outputAssociationList.append(_outAssoName)
            pass
        logAODSelect_helpers.debug( "Leaving addAssociationTool" )
        return True



    def addAlgConfig(self, algConfig):
        self._algConfigList.append(algConfig)
        return


    def findAlgConfig(self, name=""):
        if name=="":
            logAODSelect_helpers.warning( "Trying to find an AODSelectAlgConfig with empty string... this won't work!" )
            return None
        for algConfig in self._algConfigList:
            if algConfig.name == name:
                return algConfig
            pass
        return None


    def scheduleAllAlgs(self, seq=None):
        logAODSelect_helpers.debug( "Entering scheduleAllAlgs" )
        if not seq:
            logAODSelect_helpers.warning( "You are trying to schedule all algorithms, but didn't give me a sequence... this won't work!" )
            return False
        for algConfig in self._algConfigList:
            logAODSelect_helpers.debug( "Values of current AODSelectAlgConfig: " )
            if logAODSelect_helpers.isEnabledFor(logging.DEBUG): algConfig.doPrint()
            algName = "AODSelect_" + algConfig.name.split("#")[1] + "_SelectionAlg"
            logAODSelect_helpers.debug( "Now creating an algorithm with name %s" % algName )
            algClass = self.algDict[algConfig.inputType]
            alg = algClass( algName,
                            inputCollection                = algConfig.inputKey,
                            outputCollection               = algConfig.outputCollection,
                            outputLinkCollection           = algConfig.outputLinkCollection,
                            selectorToolList               = algConfig.selectorToolList,
                            userDataCalcToolList           = algConfig.userDataToolList,
                            associationToolList            = algConfig.associationToolList,
                            outputAssociationContainerList = algConfig.outputAssociationList
                            )
            seq += alg
            pass
        logAODSelect_helpers.debug( "Leaving scheduleAllAlgs" )
        return True

    pass





class AODSelectAlgConfig(object):
    """
    Class to hold the final configuration of one algorithm for the AODSelect framework
    """

    def __init__(self, name="" ):
        object.__init__(self)
        self.name                  = name
        self.inputTypeAndKey       = name
        self.inputType, self.inputKey = name.split("#")
        self.outputCollection      = ""
        self.outputLinkCollection  = ""
        self.selectorToolList      = []
        self.userDataToolList      = []
        self.associationToolList   = []
        self.outputAssociationList = []
        return


    def setInputTypeAndKey(self, inputTypeAndKey=None, inputKey=None):
        if inputTypeAndKey == None:
            logAODSelect_helpers.warning( "Got an empty value in setInputTypeAndKey... exiting!" )
            return
        if inputTypeAndKey.__contains__("#"):
            self.inputTypeAndKey          = inputTypeAndKey
            self.inputType, self.inputKey = inputTypeAndKey.split("#")
            logAODSelect_helpers.debug( "Setting inputTypeAndKey=%s, inputType=%s, inputKey=%s"
                                        % (self.inputTypeAndKey, self.inputType, self.inputKey) )
            return
        elif inputKey != None:
            self.inputTypeAndKey = inputTypeAndKey + "#" + inputKey
            self.inputType       = inputTypeAndKey
            self.inputKey        = inputKey
            logAODSelect_helpers.debug( "Setting inputTypeAndKey=%s, inputType=%s, inputKey=%s"
                                        % (self.inputTypeAndKey, self.inputType, self.inputKey) )
            return
        logAODSelect_helpers.warning( "Could not understand what to do with inputTypeAndKey=%s and inputKey=%s"
                                      % (inputTypeAndKey, inputKey) )
        return


    def doPrint(self):
        logAODSelect_helpers.info( "Found an AODSelectAlgConfig with name = %s" % self.name )
        logAODSelect_helpers.info( "   and inputTypeAndKey       = %s" % self.inputTypeAndKey )
        logAODSelect_helpers.info( "   and inputType             = %s" % self.inputType )
        logAODSelect_helpers.info( "   and inputKey              = %s" % self.inputKey )
        logAODSelect_helpers.info( "   and outputCollection      = %s" % self.outputCollection )
        logAODSelect_helpers.info( "   and outputLinkCollection  = %s" % self.outputLinkCollection )
        logAODSelect_helpers.info( "   and selectorToolList      = %s" % self.selectorToolList )
        logAODSelect_helpers.info( "   and userDataToolList      = %s" % self.userDataToolList )
        logAODSelect_helpers.info( "   and associationToolList   = %s" % self.associationToolList )
        logAODSelect_helpers.info( "   and outputAssociationList = %s" % self.outputAssociationList )
        return

    pass








# Get all available collection keys in the input file and in the transient data store
def allAvailableObjectsLists():
    allAvailableObjectsTypeKeyList = objKeyStore['transient'].list() + objKeyStore['inputFile'].list()
    logAODSelect_helpers.verbose( "All available input collections: %s" % allAvailableObjectsTypeKeyList )
    allAvailableObjectsKeyList = []
    for typekey in allAvailableObjectsTypeKeyList:
        allAvailableObjectsKeyList.append( (typekey.split("#"))[1] )
        pass
    return allAvailableObjectsTypeKeyList, allAvailableObjectsKeyList



# Method to print all available AODSelect containers
def printAODSelect():
    print "========================================================================================"
    print "Showing below the all available AODSelect containers for each available input container:"
    print "    (format: <container type>#<container key>)"

    """
    # Get the input types and keys
    inOutDict = getInputOutputDictionary()
    inputTypeAndKeyList = inOutDict.keys()
    inputTypeAndKeyList.sort()

    # Print the available outputs for each input
    for inputTypeAndKey in inputTypeAndKeyList:
        print
        print "    Available AODSelect containers for input container %s are:" % inputTypeAndKey
        
        # Get the values of outputType and outputKeys
        outputTypeAndKeyList = inOutDict[inputTypeAndKey]
        outputTypeAndKeyList.sort()
        for outputTypeAndKey in outputTypeAndKeyList:
            print "        %s" % outputTypeAndKey
            pass
        pass
"""
    print "========================================================================================"

    return
