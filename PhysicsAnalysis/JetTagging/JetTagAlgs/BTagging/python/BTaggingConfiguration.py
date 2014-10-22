# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Python function implementation of B-tagging configuration
# Wouter van den Wollenberg (2013-2014)
# E-mail: wouter.van.den.wollenberg@cern.ch, wwollenb@nikhef.nl

from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.BTaggingConfiguration_LoadTools import DefaultSetupForJetCollection
from RecExConfig.RecFlags import rec

# -- Import all the functions that set up all the tools --

# (TOOLS NOT PORTED TO RELEASE 19 ARE COMMENTED OUT)

# Common tools:
from BTagging.BTaggingConfiguration_CommonTools import *
from BTagging.BTaggingConfiguration_InDetVKalVxInJetTool import *
#from BTagging.BTaggingConfiguration_InDetVKalVxNegativeTagInJetTool import *

# GbbNN tagger
#from BTagging.BTaggingConfiguration_GbbNNTag import *

# IP taggers
#from BTagging.BTaggingConfiguration_IP1DTag import *
#from BTagging.BTaggingConfiguration_IP2DFlipTag import *
#from BTagging.BTaggingConfiguration_IP2DNegTag import *
#from BTagging.BTaggingConfiguration_IP2DPosTag import *
#from BTagging.BTaggingConfiguration_IP2DSpcFlipTag import *
#from BTagging.BTaggingConfiguration_IP2DSpcNegTag import *
#from BTagging.BTaggingConfiguration_IP2DSpcPosTag import *
#from BTagging.BTaggingConfiguration_IP2DSpcTag import *
from BTagging.BTaggingConfiguration_IP2DTag import *
#from BTagging.BTaggingConfiguration_IP3DFlipTag import *
#from BTagging.BTaggingConfiguration_IP3DNegTag import *
#from BTagging.BTaggingConfiguration_IP3DPosTag import *
#from BTagging.BTaggingConfiguration_IP3DSpcFlipTag import *
#from BTagging.BTaggingConfiguration_IP3DSpcNegTag import *
#from BTagging.BTaggingConfiguration_IP3DSpcPosTag import *
#from BTagging.BTaggingConfiguration_IP3DSpcTag import *
from BTagging.BTaggingConfiguration_IP3DTag import *

# Jet fitter taggers
#from BTagging.BTaggingConfiguration_JetFitterCOMB import *
#from BTagging.BTaggingConfiguration_JetFitterTagCOMBNNIP3DNeg import *
#from BTagging.BTaggingConfiguration_JetFitterTagCOMBNNIP3DPos import *
##from BTagging.BTaggingConfiguration_JetFitterTagCOMBNN import *
#from BTagging.BTaggingConfiguration_JetFitterTagNNFlip import *
##from BTagging.BTaggingConfiguration_JetFitterTagNN import *
#from BTagging.BTaggingConfiguration_JetFitterTag import *

# New jet fitter taggers
#from BTagging.BTaggingConfiguration_NewJetFitterCOMB import *
#from BTagging.BTaggingConfiguration_NewJetFitterFlipCOMB import *
#from BTagging.BTaggingConfiguration_NewJetFitterTagFlip import *
##from BTagging.BTaggingConfiguration_NewJetFitterTag import *

from BTagging.BTaggingConfiguration_NewJetFitterCollection import * #I collected all jetfitter functions currently needed for rel. 19 into here (Wouter)

# Lepton taggers
#from BTagging.BTaggingConfiguration_SoftElectronTag import *
#from BTagging.BTaggingConfiguration_SoftMuonTagChi2 import *
#from BTagging.BTaggingConfiguration_SoftMuonTag import *

# Track counting
#from BTagging.BTaggingConfiguration_TrackCountingFlip import *
#from BTagging.BTaggingConfiguration_TrackCounting import *

# SV taggers
from BTagging.BTaggingConfiguration_SV0Tag import *
#from BTagging.BTaggingConfiguration_SV1FlipTag import *
from BTagging.BTaggingConfiguration_SV1Tag import *
#from BTagging.BTaggingConfiguration_SV2FlipTag import *
#from BTagging.BTaggingConfiguration_SV2Tag import *

# JetProb taggers
#from BTagging.BTaggingConfiguration_JetProbFlipTag import *
#from BTagging.BTaggingConfiguration_JetProbTag import *

# MultiSV tagger
#from BTagging.BTaggingConfiguration_MultiSVTag import *
from BTagging.BTaggingConfiguration_MultiSVbb1Tag import *
from BTagging.BTaggingConfiguration_MultiSVbb2Tag import *

# MV taggers
#from BTagging.BTaggingConfiguration_MV1FlipTag import *
from BTagging.BTaggingConfiguration_MV1Tag import *
from BTagging.BTaggingConfiguration_MVbTag import *
#from BTagging.BTaggingConfiguration_MV2FlipTag import *
from BTagging.BTaggingConfiguration_MV2Tag import *
from BTagging.BTaggingConfiguration_MV1cTag import *
from BTagging.BTaggingConfiguration_MV2c00Tag import *
from BTagging.BTaggingConfiguration_MV2c10Tag import *
from BTagging.BTaggingConfiguration_MV2c20Tag import *

# This global dictionary keeps track of tools/taggers that have been initialized
# the key is the type of the tool, the value is the actual tool. All functions 
# that set up tools should add to this dictionary!
_BTaggingConfig_InitializedTools = {}

# This dictionary keeps track of all btagtools for each jet collection.
_BTaggingConfig_JetCollections = {}
_BTaggingConfig_RemovalTools = {}
_BTaggingConfig_SecVtxTools = {}
_BTaggingConfig_MainAssociatorTools = {}

# This dictionary keeps track of all BJetBuilders for each jet collection.
_BTaggingConfig_BJetBuilders = {}

# This dictionary keeps track of all JetBTaggerTools for each jet collection.
_BTaggingConfig_JetBTaggerTools = {}

# These dictionaries keeps track of all particle associators for each jet collection. Photon associators are counted
# as electron associators.
_BTaggingConfig_TrackAssociators = {}
_BTaggingConfig_MuonAssociators = {}
_BTaggingConfig_ElectronAssociators = {}

# These dictionaries keep track of CollectionNames and ContainerNames. Technically we could also put them into
# the above dictionaries. But this should be clearer.
_BTaggingConfig_ElectronConCol = {}
_BTaggingConfig_PhotonConCol = {}
_BTaggingConfig_TrackConCol = {}
_BTaggingConfig_MuonConCol = {}

# Global dictionary keeping track of tools tied to which jet collections and which tracks.
_BTaggingConfig_SetUpCombinations = {}

# Global dictionary keeping track of all tool collections
_BTaggingConfig_ToolCollections = {}

# Boolean to toggle behaviour where various flags are not checked to see whether a tagger could run,
# (this happens in the taggerIsPossible() function) and includes DetFlags. This currently seems needed for retagging? Users
# can toggle this using the doNotCheckForTaggerObstacles() function.
IgnoreTaggerObstacles = False

# We also need deepcopy here (for cloning metadata when cloning tools since these include nested dictionaries)
from copy import deepcopy

def doNotCheckForTaggerObstacles():
    global IgnoreTaggerObstacles
    IgnoreTaggerObstacles = True

def addToolTypeToToolCollection(tool_type, ToolCollection):
    """Adds a given tool_type to a given tool collection. This also sets the metadata on the tool accordingly for consistency."""
    global _BTaggingConfig_ToolCollections
    changeToolMetadata(tool_type, "ToolCollection", ToolCollection)
    if ToolCollection in _BTaggingConfig_ToolCollections:
        _BTaggingConfig_ToolCollections[ToolCollection] += [tool_type, ]
    else:
        _BTaggingConfig_ToolCollections[ToolCollection] = [tool_type, ]

def getToolCollection(tool_type):
    """Returns the name of the tool collection the current tool is in."""
    return getToolMetadata(tool_type, "ToolCollection")

def getToolCollectionSize(ToolCollection):
    """Returns the size of a given tool collection. Returns 0 if it does not exist."""
    if ToolCollection in _BTaggingConfig_ToolCollections:
        return len(_BTaggingConfig_ToolCollections[ToolCollection])
    else:
        return 0

def checkToolCollectionStructure(CheckTransToolCollectionDependencies = True):
    """Computationally intensive function which does a check on on consistency of the tools. 
    If checks whether there are no tools that depend on themselves, whether all tool types 
    that a part of a tools PassByPointer metadata are also in its DependsOn metadata and whether 
    all tools mentioned in dependencies actually exist. 

    If CheckTransToolCollectionDependencies is True then it will print warnings for every instance 
    where a tool from a tool collection depends on another tool from another tool collection whose 
    size is bigger than one."""
    print '#BTAG# - DEBUG - Checking tool collection structure.'
    for tcoll in _BTaggingConfig_ToolCollections:
        for tool in _BTaggingConfig_ToolCollections[tcoll]:
            # This loops over all tools
            Dependencies = getToolMetadata(tool, "DependsOn")
            PassByPointer = getToolMetadata(tool, "PassByPointer")
            if Dependencies:
                for d in Dependencies:
                    tcoll2 = getToolCollection(d)
                    if tcoll2:
                        if getToolCollectionSize(tcoll2) > 1 and CheckTransToolCollectionDependencies:
                            if tcoll2 != tcoll:
                                print '#BTAG# - DEBUG - Trans tool collection dependency: '+tool+' ('+tcoll+') on '+d+' ('+tcoll2+')'
                    else:
                        print '#BTAG# - WARNING - Tool of type '+tool+' depends on tool of type '+d+' but '+d+' is not part of any tool collection.'
                    if d == tool:
                        print '#BTAG# - ERROR - Tool of type '+tool+' depends on itself!'
                        raise ValueError
            if PassByPointer:
                for p in PassByPointer:
                    if not PassByPointer[p] in Dependencies:
                        print '#BTAG# - WARNING - Tool of type '+tool+' needs a tool of type '+PassByPointer[p]+' passed to it by pointer but '+PassByPointer[p]+' is not one of '+tool+"'s dependencies."
    print '#BTAG# - DEBUG - Check completed.'

def getAllToolsThatDependOn(tool_type):
    """Returns a list of tool types of all tools that directly depend on a tool of a given tool type. This operation
    can be computationally intensive. And is mainly used for debugging purposes."""
    returnlist = []
    for tcoll in _BTaggingConfig_ToolCollections:
        for tname in _BTaggingConfig_ToolCollections[tcoll]:
            Dependencies = getToolMetadata(tname, "DependsOn")
            if Dependencies:
                if tool_type in Dependencies:
                    returnlist.append(tname)
    return returnlist

def getToolCollections():
    """Returns a dictionary containing all tool collections."""
    return _BTaggingConfig_ToolCollections

def printAllTools(PrintBJetBuilders = True, PrintJetBTaggerTools = True, PrintOtherTools = True):
    """Prints all tools. Printing tools during set up is not very useful since attributes are
    added dynamically, so the print-out would not be representative. Therefore we have this function
    which you can call at the end of the setup (or any other moment you like =) )."""
    if PrintBJetBuilders:
        print '--- BJetBuilders ---'
        for tool in _BTaggingConfig_BJetBuilders:
            print str(tool)+': ',_BTaggingConfig_BJetBuilders[tool]
    if PrintJetBTaggerTools:
        print '--- JetBTaggerTools ---'
        for tool in _BTaggingConfig_JetBTaggerTools:
            print str(tool)+': ',_BTaggingConfig_JetBTaggerTools[tool]
    if PrintBJetBuilders or PrintJetBTaggerTools:
        print '--- Main Associator Tools ---'
        for tool in _BTaggingConfig_MainAssociatorTools:
            print str(tool)+': ',_BTaggingConfig_MainAssociatorTools[tool]
    if PrintOtherTools:
        print '--- Associators ---'
        for tool in _BTaggingConfig_TrackAssociators:
            print str(tool)+': ',_BTaggingConfig_TrackAssociators[tool]
        for tool in _BTaggingConfig_MuonAssociators:
            print str(tool)+': ',_BTaggingConfig_MuonAssociators[tool]
        for tool in _BTaggingConfig_ElectronAssociators:
            print str(tool)+': ',_BTaggingConfig_ElectronAssociators[tool]
        print '--- BTagTools ---'
        for tool in _BTaggingConfig_JetCollections:
            print str(tool)+': ',_BTaggingConfig_JetCollections[tool]
        print '--- BTagTool support tools ---'
        for tool in _BTaggingConfig_RemovalTools:
            print str(tool)+': ',_BTaggingConfig_RemovalTools[tool]
        for tool in _BTaggingConfig_SecVtxTools:
            print str(tool)+': ',_BTaggingConfig_SecVtxTools[tool]
        print '--- Other tools ---'
        for tool in _BTaggingConfig_InitializedTools:
            print str(tool)+': ',_BTaggingConfig_InitializedTools[tool]

def cloneTool(ToolType, Suffix):
    """Clones a single tool, creating a copy with the same name except with a suffix. It returns the name of the new tool.
    This function does not update metadata; it is therefore recommended to use the cloneToolCollection() function for most
    purposes instead. The tool will be placed in its own tool collection."""
    if 'tool'+ToolType+Suffix in dir():
            print '#BTAG# - WARNING - Attempting to clone tool of type '+ToolType+' with suffix '+Suffix+' but this already exists'
    try:
        exec 'tool'+ToolType+Suffix+' = tool'+ToolType in globals()
        exec 'meta'+ToolType+Suffix+' = deepcopy(meta'+ToolType+')' in globals()
    except:
        print '#BTAG# - ERROR - cloneToolCollection() called but one of the tools does not exist, is not implemented correctly, or does not allow cloning: '+ToolType
        raise
    addToolTypeToToolCollection(ToolType+Suffix, ToolType+Suffix)
    changeToolMetadata(ToolType+Suffix, "IsAClone", True)
    return 'tool'+ToolType+Suffix

def cloneToolCollection(ToolCollection, Suffix):
    """Clones a tool collection, creating copies with the same name except with a suffix. Returns a list of the names of
    the new tools. The metadata for the tools will be adjusted within the tool collection as well, so that dynamic passing
    of tools remains working within the tool collection.

    This is useful if you want to set up multiple tools of the same type but with different settings. See the documentation
    for more information."""
    if not ToolCollection in _BTaggingConfig_ToolCollections:
        print '#BTAG# - ERROR - Trying to clone a tool collection called '+ToolCollection+' but this does not exist'
        options = []
        tcollections = getToolCollections()
        for tcol in tcollections:
            options.append(tcol)
        print '#BTAG# - ERROR - Valid tool collections are: '+str(options)
        raise RuntimeError
    ToolsToClone = _BTaggingConfig_ToolCollections[ToolCollection]
    returnlist = []
    for tool in ToolsToClone:
        if 'tool'+tool+Suffix in dir():
            print '#BTAG# - WARNING - Attempting to clone tool of type '+tool+' with suffix '+Suffix+' but this already exists, this operation will overwrite the previous clone'
        try:
            exec 'tool'+tool+Suffix+' = tool'+tool in globals()
            exec 'meta'+tool+Suffix+' = deepcopy(meta'+tool+')' in globals()
            # Update metadata
            PassByPointers = {}
            PassByNames = {}
            DependsOn = {}
            exec 'if "PassByPointer" in meta'+tool+Suffix+':\n\tPassByPointers = meta'+tool+Suffix+'["PassByPointer"]'
            exec 'if "PassByName" in meta'+tool+Suffix+':\n\tPassByNames = meta'+tool+Suffix+'["PassByName"]'
            exec 'if "DependsOn" in meta'+tool+Suffix+':\n\tDependsOn = meta'+tool+Suffix+'["DependsOn"]'
            # Here we use the Python property that everything is a reference
            for pointer in PassByPointers:
                if PassByPointers[pointer] in ToolsToClone:
                    PassByPointers[pointer] += Suffix
            for name in PassByNames:
                if PassByNames[name] in ToolsToClone:
                    PassByNames[name] += Suffix
            for dependency in DependsOn:
                if dependency in ToolsToClone:
                    DependsOn[DependsOn.index(dependency)] += Suffix
            # Add to new tool collection
            addToolTypeToToolCollection(tool+Suffix, ToolCollection+Suffix)
            changeToolMetadata(tool+Suffix, "IsAClone", True)
        except:
            print '#BTAG# - ERROR - cloneToolCollection() called but one of the tools does not exist, is not implemented correctly, or does not allow cloning: '+tool
            raise
    return returnlist

def updateBTaggers(JetCollection):
    """Updates the JetBTaggerTool or BJetBuilder tool for the selected JetCollection if they already exist. This is in order to keep
    them up-to-date, as someone might set up a tool after these main tools already exist. And such a tool might require a new associator
    which we need to add to the main associator tool."""
    if JetCollection in _BTaggingConfig_JetBTaggerTools:
        setupJetBTaggerTools(JetCollections=JetCollection)
    if JetCollection in _BTaggingConfig_BJetBuilders:
        setupBJetBuilders(JetCollections=JetCollection)

def setupJetBTaggerTool(ToolSvc=None, JetCollection="", topSequence=None, NoDynamicName=False, Verbose = False, AddToToolSvc = False, options={}):
    """Convenience function which takes only a single jet collection and returns an instance instead
    of a list; see setupJetBTaggerTools for more info."""
    btagger = setupJetBTaggerTools(ToolSvc, [JetCollection,], topSequence, NoDynamicName, Verbose, AddToToolSvc, options)
    return btagger[0]

def setupJetBTaggerTools(ToolSvc=None, JetCollections=[], topSequence=None, NoDynamicName=False, Verbose = False, AddToToolSvc = False, options={}):
    """Sets up JetBTaggerTool tools and adds them to the topSequence (one per jet collection). This function just updates
    the tool if such a tool already exists for the specified jet collections. This function should only be used for
    jet collections that one need reconstruction. Note that it is allowed to set topSequence to None,
    in this case the tool will not be added to the topSequence. (This is actually used in the default set up). Note
    that we do need the ToolSvc, this is needed to set up the associator tool.

    input: ToolSvc:          The ToolSvc. (not needed when updating)
           JetCollections:   List of jet collection name (can also accept string in the case of one collection).
           topSequence:      The topSequence. (not needed when updating)
           NoDynamicName:    If True the name of the tool will be set to 'JetBTagger' and not 'JetBTagger_X'
                             where X is the jet collection.
           Verbose:          Whether to print some additional information.
           AddToToolSvc:     Whether to add the JetBTaggerTool to the ToolSvc or not.
           options:          List of options to be passed to the JetBTaggerTools. This has the following default:

    OutputLevel:                                  default: BTaggingFlags.OutputLevel

    output: List of JetBTaggerTools (this includes those for which no tool was set up because it already existed)."""
    global _BTaggingConfig_JetBTaggerTools
    global _BTaggingConfig_MainAssociatorTools
    if BTaggingFlags.OutputLevel < 3:
        print '#BTAG# - DEBUG - Setting up JetBTaggerTools for jet collections: '+str(JetCollections)
    returnlist = []
    # Allow for string input
    if type(JetCollections) == str:
        JetCollections = [JetCollections,]
    for jetcol in JetCollections:
        # Make sure we have this jet collection; if not use defaults
        DefaultSetupForJetCollection(jetcol)
        if jetcol in _BTaggingConfig_BJetBuilders:
            print '#BTAG# - ERROR - Attempting to setup reconstruction for jet collection "'+jetcol+'" but this jet collection is already scheduled for retagging.'
            raise RuntimeError
        if jetcol in _BTaggingConfig_JetBTaggerTools:
            returnlist.append(_BTaggingConfig_JetBTaggerTools.get(jetcol, None))
            print '#BTAG# - INFO - Updating JetBTaggerTool for '+jetcol
            ConfigureMainAssociatorTool(_BTaggingConfig_MainAssociatorTools[jetcol], jetcol)
            continue
        # Check if this jet collection has been correctly set up.
        if not jetcol in _BTaggingConfig_JetCollections:
            print '#BTAG# - ERROR - setupJetBTaggerTools() called for jet collection with name "'+jetcol+'" but this jet collection has not been initialized.'
            raise RuntimeError
        print '#BTAG# - INFO - Adding JetBTaggerTool for '+jetcol
        from BTagging.BTaggingConf import Analysis__JetBTaggerTool as JetBTaggerTool
        options = dict(options)
        options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
        # setup the Analysis__BTagTrackAssociation tool
        # Note that this tool is tied to the JetBTaggerTool
        thisBTagTrackAssociation = setupBTagTrackAssociation('thisBTagTrackAssociation_'+jetcol, ToolSvc, Verbose = Verbose)
        _BTaggingConfig_MainAssociatorTools[jetcol] = thisBTagTrackAssociation
        options.setdefault('BTagTrackAssocTool', thisBTagTrackAssociation)
        # setup the secondary vertexing tool
        options.setdefault('BTagSecVertexing', getJetCollectionSecVertexingTool(jetcol))
        # Set remaining options
        if NoDynamicName:
            options.setdefault('name', "JetBTagger")
        else:
            options.setdefault('name', "JetBTagger"+jetcol)
        options['BTagTool'] = _BTaggingConfig_JetCollections.get(jetcol, None)
        jetbtaggertool = JetBTaggerTool(**options)
        # Setup the associator tool
        ConfigureMainAssociatorTool(thisBTagTrackAssociation, jetcol)
        # -- add tool to topSequence
        if not topSequence is None:
            topSequence += jetbtaggertool
        # -- add tool to ToolSvc
        if AddToToolSvc:
            ToolSvc += jetbtaggertool
        if Verbose:
            print jetbtaggertool
            print '#BTAG# - INFO - Attached to the beforementioned JetBTaggerTool we have the following BTagTool:'
            print _BTaggingConfig_JetCollections.get(jetcol, None)
        # Register
        _BTaggingConfig_JetBTaggerTools[jetcol] = jetbtaggertool
        returnlist.append(jetbtaggertool)
    return returnlist

def setupBJetBuilders(topSequence=None, ToolSvc=None, JetCollections=[], Verbose = False, options={}):
    """Sets up BJetBuilder tools and adds them to the topSequence (one per jet collection). This function only updates
    the tool if such a tool already exists for the specified jet collections. This function should only be used
    for jet collections to be retagged.

    input: topSequence:      The topSequence. (not needed when updating)
           ToolSvc:          The ToolSvc. (not needed when updating)
           JetCollections:   List of jet collection names (can also accept strings in the case of only one collection).
           Verbose:          Whether to print some info after creation.
           options:          List of options to be passed to the BJetBuilders. This has the following defaults:

    Runmodes:                                     default: BTaggingFlags.Runmodus
    KeepTruthInfo:                                default: not BTaggingFlags.ConvertParticleJets
    BTagAssociation:                              default: BTaggingFlags.doStandardAssoc
    TrackParticleContainerName:                   default: BTaggingFlags.TrackParticleCollectionName
    OutputLevel:                                  default: BTaggingFlags.OutputLevel
    
    output: List of BJetBuilders (this includes those for which no tool was set up because it already existed)."""
    global _BTaggingConfig_BJetBuilders
    global _BTaggingConfig_MainAssociatorTools
    if BTaggingFlags.OutputLevel < 3:
        print '#BTAG# - DEBUG - Setting up BJetBuilders for jet collections: '+str(JetCollections)
    returnlist = []
    # Allow for string input
    if type(JetCollections) == str:
        JetCollections = [JetCollections,]
    for jetcol in JetCollections:
        # Make sure we have this jet collection; if not use defaults
        DefaultSetupForJetCollection(jetcol)
        if jetcol in _BTaggingConfig_JetBTaggerTools:
            print '#BTAG# - ERROR - Attempting to setup retagging for jet collection "'+jetcol+'" but this jet collection is already scheduled for reconstruction.'
            raise RuntimeError
        if jetcol in _BTaggingConfig_BJetBuilders:
            # BJetBuilder already exists; setup associator tool anew in case new associators have been added later on
            returnlist.append(_BTaggingConfig_BJetBuilders.get(jetcol, None))
            print '#BTAG# - INFO - Updating BJetBuilder for '+jetcol
            ConfigureMainAssociatorTool(_BTaggingConfig_MainAssociatorTools[jetcol], jetcol)
            continue
        # Check if this jet collection has been correctly set up.
        if not jetcol in _BTaggingConfig_JetCollections:
            print '#BTAG# - ERROR - setupBTagBuilder() called for jet collection with name "'+jetcol+'" but this jet collection has not been initialized.'
            raise RuntimeError
        print '#BTAG# - INFO - Adding BJetBuilder for '+jetcol
        from BTagging.BTaggingConf import Analysis__BJetBuilder
        options = dict(options)
        options.setdefault('Runmodus', BTaggingFlags.Runmodus)
        options.setdefault('KeepTruthInfo', not BTaggingFlags.ConvertParticleJets)
        options.setdefault('BTagAssociation', BTaggingFlags.doStandardAssoc)
        options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
        # setup the Analysis__BTagTrackAssociation tool
        # Note that this tool is tied to the BJetBuilder
        thisBTagTrackAssociation = setupBTagTrackAssociation('thisBTagTrackAssociation_'+jetcol, ToolSvc, Verbose = Verbose)
        _BTaggingConfig_MainAssociatorTools[jetcol] = thisBTagTrackAssociation
        options.setdefault('BTagTrackAssocTool', thisBTagTrackAssociation)
        jetBasis = "Cells"
        if jetcol.find('Track') >= 0:
            jetBasis = "Tracks"
        if Verbose:
            print '#BTAG# - INFO - creating ',"BJetBuilder"+jetcol,' ',jetBasis
        # Set all the options
        options['name'] = "BJetBuilder"+jetcol
        options['BTagTool'] = _BTaggingConfig_JetCollections.get(jetcol, None)
        options['jetFinderBasedOn'] = jetBasis
        options['JetCollectionName'] = jetcol+'Jets'
        bjetbuilder = Analysis__BJetBuilder(**options)
        # Setup the associator tool
        ConfigureMainAssociatorTool(thisBTagTrackAssociation, jetcol)
        # -- add tool to topSequence
        topSequence += bjetbuilder
        if Verbose:
            print bjetbuilder
            print '#BTAG# - INFO - Attached to the beforementioned BJetBuilder we have the following BTagTool:'
            print _BTaggingConfig_JetCollections.get(jetcol, None)
        # Register
        _BTaggingConfig_BJetBuilders[jetcol] = bjetbuilder
        returnlist.append(bjetbuilder)
    return returnlist

def ConfigureMainAssociatorTool(AssocTool, JetCollection):
    """Configures the given associator tool for the given jet collection.

    input: AssocTool:       The main association tool.
           JetCollection:   The jet collection."""
    # Get association data
    muonassoc = getMuonAssociatorData(JetCollection)
    electronassoc = getElectronAssociatorData(JetCollection)
    trackassoc = getTrackAssociatorData(JetCollection)
    # Clean current properties
    AssocTool.TrackToJetAssocNameList = []
    AssocTool.TrackToJetAssociatorList = []
    AssocTool.TrackContainerNameList = []
    AssocTool.MuonToJetAssocNameList = []
    AssocTool.MuonToJetAssociatorList = []
    AssocTool.MuonContainerNameList = []
    AssocTool.ElectronToJetAssocNameList = []
    AssocTool.ElectronContainerNameList = []
    AssocTool.ElectronToJetAssociatorList = []
    AssocTool.PhotonToJetAssocNameList = []
    AssocTool.PhotonContainerNameList = []
    # Tracks
    for tdata in trackassoc:
        AssocTool.TrackToJetAssocNameList += [tdata[2], ]
        AssocTool.TrackToJetAssociatorList += [tdata[0], ]
        AssocTool.TrackContainerNameList += [tdata[1], ]
    # Muons
    for mdata in muonassoc:
        AssocTool.MuonToJetAssocNameList += [mdata[2], ]
        AssocTool.MuonToJetAssociatorList += [mdata[0], ]
        AssocTool.MuonContainerNameList += [mdata[1], ]
    # Electrons
    for edata in electronassoc:
        AssocTool.ElectronToJetAssocNameList += [edata[2], ]
        AssocTool.ElectronToJetAssociatorList += [edata[0], ]
        AssocTool.ElectronContainerNameList += [edata[1], ]
        AssocTool.PhotonToJetAssocNameList += [edata[3], ]
        AssocTool.PhotonContainerNameList += [edata[4], ]

def registerTool(tool_type, tool, track = "", jetcol = "", ToolSvc = None, Verbose = False, MuonCollection='Muons', ElectronCollection='Electrons', PhotonCollection='Photons'):
    """Registers a tool so it can be used in order to figure out dependencies.
    This should be called after a tool is configured. It automatically adds it to
    ToolSvc if it is specified. 

    If ToolSvc is not specified prerequisites will not be set up and the tool will
    (obviously) not be added to ToolSvc. It will still be registered though and other
    tools that depend on this one will recognize it.

    input:   tool_type:          The type of tool, should correspond to a b-tagging group tool name (like IP3DTag).
             tool:               Actual tool.
             ToolSvc:            The ToolSvc.
             track:              Track association the tool is runnning on.
             jetcol:             Jet collection the tool is running on.
             Verbose:            Verbose or not.
             MuonCollection:     Muon collection (can be left blank if not needed or using default).
             ElectronCollection: Electron collection (can be left blank if not needed or using default).
             PhotonCollection:   Photon collection (can be left blank if not needed or using default)."""
    if len(track) == 0:
        if getToolName(tool_type, "OroroMonroe", jetcol) != getToolName(tool_type, "TriangleWizard", jetcol):
            print '#BTAG# - WARNING - Attempting to register tool "'+tool_type+'" without specifying required track association!'
            raise ValueError
    if len(jetcol) == 0:
        if getToolName(tool_type, track, "CarlSagan") != getToolName(tool_type, track, "RichardFeynman"):
            print '#BTAG# - WARNING - Attemping to register tool "'+tool_type+'" without specifying required jet collection!'
            raise ValueError
    if tool is None:
        print '#BTAG# - WARNING - A tool of type '+tool_type+' has type None!'
        raise ValueError
    global _BTaggingConfig_InitializedTools
    if getToolName(tool_type, track, jetcol) in _BTaggingConfig_InitializedTools:
        print '#BTAG# - WARNING - A tool named '+getToolName(tool_type, track, jetcol)+' has already been set up before!'
        raise ValueError
    if BTaggingFlags.OutputLevel < 3:
        print '#BTAG# - DEBUG - Registering tool: '+getToolName(tool_type, track, jetcol)
    if ToolSvc is None:
        if BTaggingFlags.OutputLevel < 3:
            print '#BTAG# - DEBUG - INFO: ToolSvc is None during registration. This could be on purpose if want to add the tool to ToolSvc yourself.'
    if not ToolSvc is None:
        setupPrerequisites(tool_type, ToolSvc, TrackCollection = track, JetCollection = jetcol, Verbose = Verbose,
                           MuonCollection=MuonCollection, ElectronCollection=ElectronCollection, PhotonCollection=PhotonCollection)
    _BTaggingConfig_InitializedTools[getToolName(tool_type, track, jetcol)] = tool
    if not ToolSvc is None:
        ToolSvc += tool

def getTool(tool_type, TrackCollection="", JetCollection=""):
    """Returns a reference to a tool of type tool_type for the track association and
    jet collection specified. Returns None if it does not exist.

    input: tool_type:       The type of the tool one wants to get.
           TrackCollection: The track association. Can be left out for tools that are not set up per track collection.
           JetCollection:   The jet collection. Can be left out for tools that are not set up per jet collection.
    output: The tool."""
    name = getToolName(tool_type, TrackCollection, JetCollection)
    if name in _BTaggingConfig_InitializedTools:
        return _BTaggingConfig_InitializedTools[name]
    else:
        return None

def getTools(tool_type, jetcol):
    """Returns a list of tools set up with tool_type for jet collection jet col."""
    returnlist = []
    for name in _BTaggingConfig_InitializedTools:
        # We check whether the tool has the correct jet collection, or has no specific jet collection assigned
        if name.find(tool_type)!=-1 and (name.find(jetcol)!=-1 or name.find(tool_type)+len(tool_type) == len(name)):
            if name.find(tool_type)==0:
                returnlist.append(_BTaggingConfig_InitializedTools[name])
            elif name[name.find(tool_type)-1] == '_':
                returnlist.append(_BTaggingConfig_InitializedTools[name])
    return returnlist

def getRegisteredTools():
    """Returns a list of all tools that have been set up and registered.

    output: List of tools that have been set up."""
    returnlist = []
    for name in _BTaggingConfig_InitializedTools:
        returnlist.append(name)
    return returnlist

def getToolAllMetadata(tool_type):
    """Returns the metadata of a tool as a Python dictionary about a tool of type tool_type.

    input: tool:          The tool one wishes to check.

    output: The metadata as a dictionary."""
    try:
        exec('D = meta'+tool_type)
    except:
        print '#BTAG# - ERROR - Tool type "'+tool_type+'" has no metadata dictionary or does not exist!'
        raise NotImplementedError
    return D

def getToolMetadata(tool_type, meta):
    """Returns a specific piece of metadata about a tool of type tool_type.

    input: tool:          The tool one wishes to check.
           meta:          The name of the metadata.

    output: The metadata or None if it is not defined."""
    D = getToolAllMetadata(tool_type)
    if meta in D:
        return D[meta]
    else:
        return None

def changeClonedToolCollectionMetadata(ToolCollection, Suffix, metakey, metavalue):
    """Changes a piece of metadata on all tools which are clones from a given ToolCollection and have a given Suffix. This
    is useful to set, for example, the 'OnePerTrackAssociationAndJetCollection' on all tools. Modifying certain types
    of metadata (such as 'DependsOn') en masse can be disastrous.

    Note, you can actually also use this on the default (i.e. non-cloned) tool collections by settings the Suffix to "", but
    this is not recommended."""
    if not ToolCollection in _BTaggingConfig_ToolCollections:
        print '#BTAG# - ERROR - Attempting to modify metadata for cloned tool collection for a tool collection that does not exist: '+ToolCollection
        raise ValueError
    for tool in _BTaggingConfig_ToolCollections[ToolCollection]:
        changeToolMetadata(tool + Suffix, metakey, metavalue)

def changeToolMetadata(tool_type, metakey, metavalue):
    """Changes a piece of metadata for a tool of type tool_type. Only do this before such a tool has been set up. For
    permanent changes adjust the actual metadata in the tool's source file."""
    try:
        exec('global meta'+tool_type)
        exec('meta'+tool_type+'[metakey] = metavalue')
    except:
        print '#BTAG# - ERROR - Trying to adjust metadata for a tool that does not exist or has no metadata: '+tool_type
        raise ValueError

def setToolDefaultTracks(tool_type, default):
    """Sets the default tracks for a certain tool of type tool_type to be equal to default. This is useful in that
    tools running on their default tracks do not have the tracks prefix to their name. So by doing this you can change
    the track collection for a certain tool but still use the same name. Should only be done before the tool has been set
    up (it will not apply this change retroactively).

    For permanent changes it is better to adjust the actual metadata."""
    changeToolMetadata(tool_type, 'DefaultTracks', default)
    print '#BTAG# - INFO - Default track association for tool of type "'+tool_type+'" changed to "'+default+'".'
    
def getToolName(tool, track, jetcol):
    """Returns the name of the tool with the collections. This is used because some
    tools should not be replicated (so there should be one even if using multiple prefix chains.
    To facilitate this, this function will return the bare non-prefixed version if the tool does
    not need a prefix (and should this be available for all prefix chains).

    input: tool:          The tagger/tool name one wishes to check or set up.
           track:         The track association.
           jetcol:        The jet collection.
    output: The name of the tool."""
    # Some tools only need exactly one in ToolSvc
    if getToolMetadata(tool, 'OneInTotal'):
        return tool
    # Special case for certain tools (one per jet collection)
    if tool in ('myBTagTool',):
        return jetcol+'_'+tool
    # Some tools need one per track association
    if not getToolMetadata(tool, 'OnePerTrackAssociationAndJetCollection'):
        # Append track association if non-default
        defaulttracks = getToolMetadata(tool, 'DefaultTracks')
        if not defaulttracks:
            defaulttracks = 'BTagTrackToJetAssociator'
        if track == defaulttracks:
            return tool
        return tool+'_'+track
    # All others require one per track and per jet collection
    return jetcol+'_'+tool+'_'+track

def getToolDependencies(tool_type):
    """Returns a list of all dependencies of a tool with type tool_type."""
    Dependencies = getToolMetadata(tool_type, 'DependsOn')
    if Dependencies:
        return Dependencies
    else:
        return []

def checkPrerequisites(tool_type, TrackCollection="", JetCollection=""):
    """Checks whether all prerequisites are met for the tool/tagger. It returns a list of
    tools that have not yet been added in the order in which they should be added.

    input: tool_type:           The type of tagger/tool name that one wishes to set up.
           TrackCollection:     Requested track association.
           JetCollection:       Requested jet collection.
    output: Ordered list of tools needed which are not yet in ToolSvc."""
    # Loop over the tool and all its dependencies and then their dependencies etc.
    to_check = [tool_type,]
    required = []
    while len(to_check) > 0:
        needed = checkPrerequisitesOneLevel(to_check[len(to_check)-1], 
                                            IgnoreList=required, 
                                            TrackCollection=TrackCollection, 
                                            JetCollection=JetCollection)
        to_check.pop()
        to_check += needed
        required += needed
    # Now that we have a list of all tools yet to be set up, we need to make sure it is ordered properly
    # basically tool N's index in this list must be higher than that of all tools it depends on.
    if len(required) < 2:
        # No need to sort.
        return required
    stop_sorting = False
    highest_index = -1
    DeadSwitch = 2**len(required) # this is the maximum amount of permutations ever needed; if surpassed we have a cyclic dependency!
    while not stop_sorting:
        for n in range(len(required)):
            Dependencies = getToolDependencies(required[n])
            highest_index = -1
            for m in range(len(required)-n-1):
                if required[n+m+1] in Dependencies:
                    if highest_index < n+m+1:
                        highest_index = n+m+1
            if highest_index > -1:
                # move the current tool to just passed the highest_index
                required.insert(highest_index+1, required[n])
                required.pop(n)
                break;
        if highest_index < 0:
            # we did not need to sort anything; therefore we are done
            stop_sorting = True
        else:
            DeadSwitch -= 1
            if DeadSwitch < 0:
                print '#BTAG# - ERROR - Dependencies for tool of type "'+tool_type+'" cannot be sorted; possible cyclic dependency!'
                raise ValueError
    return required

def checkPrerequisitesOneLevel(tool_type, IgnoreList, TrackCollection="", JetCollection=""):
    """Checks whether all prerequisites are met for the tool/tagger. It returns a list of
    tools that have not yet been added. Only looks one level down. Users should use the
    checkPrerequisites() function instead.

    input: tool_type:           The type of tagger/tool that one wishes to set up.
           IgnoreList:          List of tools/taggers to ignore.
           TrackCollection:     The tracks to run on.
           JetCollection:       The jet collection to be used.
    output: Ordered list of tools needed which are not yet in ToolSvc."""
    if (len(TrackCollection) == 0 or len(JetCollection) == 0) and not getToolMetadata(tool_type, 'OneInTotal'):
        print '#BTAG# - ERROR - checkPrerequisitesOneLevel() called without specifying required track/jet collection for a tool does depend on this.'
        raise ValueError
    required = []
    Dependencies = getToolDependencies(tool_type)
    for tool in Dependencies:
        if (not tool in IgnoreList) and (not tool in required):
            # Check if this tool has been set up already
            if _BTaggingConfig_SetUpCombinations.get((tool, JetCollection, TrackCollection), None) is None:
                required.append(tool)
    return required

def setupPrerequisites(tool_type, ToolSvc, TrackCollection="", JetCollection="", Verbose = False, MuonCollection = 'Muons', ElectronCollection = 'Electrons', PhotonCollection = 'Photons'):
    """Adds all tools that are not yet in ToolSvc and yet are needed by a tool of type tool_type
    Such tools are set up using default settings and BTaggingFlags. If this is not desired one
    should set these tools up prior to setting up taggers that depend on them.

    input: tool_type:          The type of tool that one wishes to set up.
           ToolSvc:            The ToolSvc.
           TrackCollection:    The tracks to run on.
           JetCollection:      The jet collection to be used.
           Verbose:            Whether to print all prerequisites that are being set up.
           MuonCollection:     The muon collection.
           ElectronCollection: The electron collection.
           PhotonCollection:   The photon collection.
    output: Returns whether anything has been set up."""
    required = checkPrerequisites(tool_type, TrackCollection=TrackCollection, JetCollection=JetCollection)
    if len(required) < 1:
        return False
    print '#BTAG# - INFO - '+tool_type+' requires the following tools that have not yet been set up: '+str(required)
    print '#BTAG# - INFO - Setting them up now with default settings:'
    for r in range(len(required)):
        if (len(TrackCollection) == 0 or len(JetCollection) == 0) and not getToolMetadata(required[r], 'OneInTotal'):
            print '#BTAG# - ERROR - Setting up a tool without jet/track collection which depends on a tool that does depend on such collections.'
            raise ValueError
        print '#BTAG# - INFO - Setting up '+required[r]+' for '+TrackCollection+' tracks and the '+JetCollection+' jet collection...'
        # Note that we do not check for prerequisites because we just did so
        addTool(required[r], ToolSvc, TrackCollections=TrackCollection, 
                JetCollections=JetCollection, CheckPrerequisites = False, Verbose = Verbose,
                MuonCollection=MuonCollection, ElectronCollection=ElectronCollection, PhotonCollection=PhotonCollection)
    return True

def addTool(tool_type, ToolSvc, TrackCollections=[], JetCollections=[], options={}, Verbose = False, CheckPrerequisites = True, name="", addInfoPlus = True, MuonCollection = "Muons", ElectronCollection = "Electrons", PhotonCollection = "Photons", SuppressNonCloneWarning=False):
    """Sets up tools of the specified type for certain jet collections and track associations.

    input: tool_type:          The type of tool to be set up.
           ToolSvc:            The ToolSvc instance.
           TrackCollections:   List of all tracks for which this tool should run. One can also enter a single track association
                               as a string.
           JetCollections:     List of all jet collections on which this tool should run. One can also enter a single jet
                               collection as a string.
           options:            Python dictionary of options to be passed to the tool.
           Verbose:            Whether to print detailed information of the tool to be set up.
           CheckPrerequisites: Whether all prerequisite tools should also be automatically set up
                               using default settings.
           name:               The desired name of the tool. If not specified a default name is used which
                               is a combination of the prefix and the tool type. Note that it is
                               generally ill-advised to choose your own name. The reason is that some
                               prerequisite tools refer to other tools by-name and not via a pointer.
                               It is possible to break things by forcing your own name. Only do so when
                               you are sure there will be no conflict or are not using the dependency
                               system.
                               You must use this if you want multiple taggers of the same type running on the same jet collection
                               (or different ones for different jet collections if the metadata is not set accordingly). In this case
                               you are responsible for setting up the required prerequisites.
                               If you are sure no additional taggers will be set up in your run after your jobOptions, you can also
                               just use the changeToolMetadata() and switch the OnePerTrackAssociationAndJetCollection flag on
                               for your tool. This way you can set up a different one for each jet collection/track collection combination. Note
                               that you should modify metadata only prior to setting up your tool.
           addInfoPlus:        Some taggers also add detailed information about what information was used
                               by the tagger. This toggle ensures that this data is stored. (some tools do require you
                               to toggle a flag in the tool as well (usually called writeInfoPlus))
           MuonCollection:     The name of the muon collection to be used. Only used when the tool actually requires such a muon collection.
           ElectronCollection: The name of the electron collection to be used. Only used when the tool actually requires such an electron collection.
           PhotonCollection:   The name of the photon collection to be used. Only used when the tool actually requires a photon collection.

    There is one additional setting. This has to do with the following. If you change the default settings of a tool you might cause confusion
    especially if other clients will use this tool later on (and they don't know about your change). Therefore if you want a tool with non-default
    settings it is generally safer to clone it first because then it will be clear that it is the non-default tool. Setting up a non-cloned
    tool with non-default settings will therefore cause a warning. You can toggle this behavior with the following argument:

           SuppressNonCloneWarning: If True then no warning will be printed is a non-cloned tool is set up without default settings.

    output: The last of the tools having been set up. (For convenience; you can use other functions to retrieve the others if needed)"""
    if not SuppressNonCloneWarning:
        if not getToolMetadata(tool_type, 'IsAClone') is None:
            if len(options) > 0 and (getToolMetadata(tool_type, 'IsAClone') == False):
                print '#BTAG# - WARNING - You are changing the settings for an instance of '+tool_type+'. It is generally safer to first clone the tool and then use the non-default settings for the clone to avoid any confusion later on. You can also suppress this warning by running addTool with the SuppressNonCloneWarning set to True.'
        elif len(options) > 0:
            print '#BTAG# - WARNING - You are changing the settings for an instance of '+tool_type+'. It is generally safer to first clone the tool and then use the non-default settings for the clone to avoid any confusion later on. You can also suppress this warning by running addTool with the SuppressNonCloneWarning set to True.'
    if type(TrackCollections) == str:
        if len(TrackCollections) == 0:
            TrackCollections = []
        else:
            TrackCollections = [TrackCollections,]
    if type(JetCollections) == str:
        if len(JetCollections) == 0:
            JetCollections = []
        else:
            JetCollections = [JetCollections,]
    # Check if track associations and jet collections are actually lists
    if type(TrackCollections) != list or type(JetCollections) != list:
        print '#BTAG# - ERROR - addTool() requires TrackCollections and JetCollections as lists or strings!'
        raise ValueError
    if ToolSvc is None:
        print '#BTAG# - ERROR - addTool() requires ToolSvc to be specified.'
        raise ValueError
    # First check if a btagtool exists for each of the desired jet collections
    for jetcol in JetCollections:
        addJetCollectionTool(jetcol, ToolSvc, Verbose)
    tool = None
    # Special routine for tools of which there is only one (these need no track/jet collections to be specified)
    # and only for the initial set up (for dependency calls track/jetcollections will be specified)
    if len(JetCollections) == 0 or len(TrackCollections) == 0:
        if getToolMetadata(tool_type, 'OneInTotal'):
            if CheckPrerequisites:
                setupPrerequisites(tool_type, ToolSvc, Verbose = Verbose, MuonCollection=MuonCollection, ElectronCollection=ElectronCollection,
                                   PhotonCollection=PhotonCollection)
            # Check if it already exists
            tool = getTool(tool_type)
            if tool is None:
                tool = setupDefaultTool(tool_type, ToolSvc, Verbose=Verbose,
                                        name=name, options=options,
                                        addInfoPlus=addInfoPlus, MuonCollection=MuonCollection,
                                        ElectronCollection=ElectronCollection,
                                        PhotonCollection=PhotonCollection)
                # add it to the empty collection
                addToolToJetCollection(tool_type, tool, jetcol="", track="", addInfoPlus=addInfoPlus)
            return tool
        else:
            print '#BTAG# - ERROR - No track/jetcollection specified for tool that does not have the "OneInTotal" metadata.'
            raise ValueError
    # Otherwise run over all the desired tracks and jet collections
    for track in TrackCollections:
        for jetcol in JetCollections:
            # Prerequisites
            if CheckPrerequisites:
                setupPrerequisites(tool_type, ToolSvc, TrackCollection=track, JetCollection=jetcol, Verbose = Verbose,
                                   MuonCollection=MuonCollection, ElectronCollection=ElectronCollection,
                                   PhotonCollection=PhotonCollection)
            # Do the correct track associators exist for this collection of tracks?
            if getTrackAssociator(track, jetcol) is None:
                print '#BTAG# - Track associator not found, setting up default'
                setupTrackAssociator(track, jetcol, ToolSvc, Verbose=Verbose)
            if getToolMetadata(tool_type, 'NeedsElectronAssociator'):
                if getElectronAssociator(ElectronCollection, jetcol) is None:
                    print '#BTAG# - Electron associator not found, setting up default'
                    setupElectronAssociator(ElectronCollection, jetcol, ToolSvc, Verbose=Verbose,PhotonCollectionName=PhotonCollection)
            if getToolMetadata(tool_type, 'NeedsMuonAssociator'):
                if getMuonAssociator(MuonCollection, jetcol) is None:
                    print '#BTAG# - Muon associator not found, setting up default'
                    setupMuonAssociator(MuonCollection, jetcol, ToolSvc, Verbose=Verbose)
            # Check if it already exists
            tool = getTool(tool_type, TrackCollection = track, JetCollection = jetcol)
            if tool is None:
                # setup the tool
                tool = setupDefaultTool(tool_type, ToolSvc, Verbose=Verbose, track=track, 
                                        jetcol=jetcol, name=name, options=options,
                                        addInfoPlus=addInfoPlus, MuonCollection=MuonCollection, 
                                        ElectronCollection=ElectronCollection,PhotonCollection=PhotonCollection)
            # Now it exists; we need to add it to the required jet collection
            addToolToJetCollection(tool_type, tool, jetcol, track, addInfoPlus=addInfoPlus)
            updateBTaggers(jetcol)
    return tool

def setupMuonAssociator(MuonCollectionName, JetCollection, ToolSvc, options={}, Verbose = False, ContainerName = ""):
    """Sets up a muon associator (and adds it to the ToolSvc).

    input: MuonCollectionName:  Muon collection name. Must be unique.
           JetCollection:       The jet collection name.
           ToolSvc:             The ToolSvc instance.
           options:             Python dictionary with options to be passed to the associator.
           Verbose:             Whether to print the associator settings afterwards.
           ContainerName:       Name of the muon container in SG. If left blank BTaggingFlags.MuonCollectionName will be used.

    output: The tool."""
    options = dict(options)
    options['name'] = getToolName('BTagMuonToJetAssociator', MuonCollectionName, JetCollection)
    tool = getTool('BTagMuonToJetAssociator', MuonCollectionName, JetCollection)
    if tool is None:
        print '#BTAG# - INFO - Setting up MuonAssociator for combination: '+str((MuonCollectionName, JetCollection))
        tool = toolBTagMuonToJetAssociator(**options)
        registerTool('BTagMuonToJetAssociator', tool, track = MuonCollectionName, jetcol = JetCollection, ToolSvc = ToolSvc)
    else:
        print '#BTAG# - INFO - Adding MuonAssociator for '+MuonCollectionName+' which already exists now also to jet collection: '+JetCollection
    global _BTaggingConfig_MuonAssociators
    global _BTaggingConfig_MuonConCol
    if _BTaggingConfig_MuonAssociators.get((MuonCollectionName, JetCollection), None) is None:
        _BTaggingConfig_MuonAssociators[(MuonCollectionName, JetCollection)] = tool
        if len(ContainerName) == 0:
            ContainerName = BTaggingFlags.MuonCollectionName
        _BTaggingConfig_MuonConCol[(MuonCollectionName, JetCollection)] = (ContainerName, MuonCollectionName)
    else:
        print '#BTAG# - WARNING - SetupMuonAssociator() called for combination which already exists: '+str((MuonCollectionName,JetCollection))
    return tool

def setupElectronAssociator(ElectronCollectionName, JetCollection, ToolSvc, options={}, Verbose = False, ElectronContainerName = "", PhotonContainerName = "", PhotonCollectionName = "Photons"):
    """Sets up an electron associator (and adds it to the ToolSvc).

    input: ElectronCollectionName: Name of the output electron collection. Should be unique.
           JetCollection:          The jet collection name.
           ToolSvc:                The ToolSvc instance.
           options:                Python dictionary with options to be passed to the associator.
           Verbose:                Whether to print the associator settings afterwards.
           ElectronContainerName:  Name of the electron container in SG. If left blank BTaggingFlags.ElectronCollectionName will be used.
           PhotonContainerName:    Name of the photon container in SG. If left blank BTaggingFlags.PhotonCollectionName will be used.
           PhotonCollectionName:   Name of the output photon collection. If left blank 'Photons' will be used. However this name should be unique
                                   therefore if an electron associator already exists the given name (including the default 'Photons') the name
                                   will instead be 'Photons_JETCOL_ELECCOL', where JETCOL is JetCollection and ELECCOL is ElectronCollection.
                                   A warning will be printed in this case. If this name is also already taken (this means someone is royally
                                   messing things up) an exception will be raised.
    output: The tool."""
    options = dict(options)
    options['name'] = getToolName('BTagElectronToJetAssociator', ElectronCollectionName, JetCollection)
    tool = getTool('BTagElectronToJetAssociator', ElectronCollectionName, JetCollection)
    if tool is None:
        print '#BTAG# - INFO - Setting up ElectronAssociator for combination: '+str((ElectronCollectionName, JetCollection))
        tool = toolBTagElectronToJetAssociator(**options)
        registerTool('BTagElectronToJetAssociator', tool, track = ElectronCollectionName, jetcol = JetCollection, ToolSvc = ToolSvc)
    else:
        print '#BTAG# - INFO - Adding ElectronAssociator for '+ElectronCollectionName+' which already exists now also to jet collection: '+JetCollection
    global _BTaggingConfig_ElectronAssociators
    global _BTaggingConfig_ElectronConCol
    global _BTaggingConfig_PhotonConCol
    if _BTaggingConfig_ElectronAssociators.get((ElectronCollectionName, JetCollection), None) is None:
        _BTaggingConfig_ElectronAssociators[(ElectronCollectionName, JetCollection)] = tool
        if len(ElectronContainerName) == 0:
            ElectronContainerName = BTaggingFlags.ElectronCollectionName
        if len(PhotonContainerName) == 0:
            PhotonContainerName = BTaggingFlags.PhotonCollectionName
        # Is the requested photon collection name unique?
        Unique = True
        for x in _BTaggingConfig_PhotonConCol:
            if x[1] == PhotonCollectionName:
                Unique = False
        if Unique:
            _BTaggingConfig_PhotonConCol[(ElectronCollectionName, JetCollection)] = (PhotonContainerName, PhotonCollectionName)
        else:
            PhotonCollectionName = 'Photons_'+JetCollection+'_'+ElectronCollectionName
            print '#BTAG# - WARNING - Photon collection name changed to '+PhotonContainerName+' because the given value was not unique.'
            for x in _BTaggingConfig_PhotonConCol:
                if x[1] == PhotonCollectionName:
                    print '#BTAG# - ERROR - Photon collection name is not unique and default is also in use.'
                    raise ValueError
            _BTaggingConfig_PhotonConCol[(ElectronCollectionName, JetCollection)] = (PhotonContainerName, PhotonCollectionName)
    else:
        print '#BTAG# - WARNING - SetupElectronAssociator() called for combination which already exists: '+str((ElectronCollectionName,JetCollection))
    return tool

def setupTrackAssociator(TrackCollection, JetCollection, ToolSvc, options={}, Verbose = False, ContainerName = ""):
    """Sets up a track associator (and adds it to the ToolSvc).

    input: TrackCollection:        The track association for which to set up the associator.
           JetCollection:          The jet collection name.
           ToolSvc:                The ToolSvc instance.
           options:                Python dictionary with options to be passed to the associator.
           Verbose:                Whether to print the associator settings afterwards.
           ContainerName:          Name of the track container in SG. If left blank BTaggingFlags.TrackParticleCollectionName will be used.
    output: The tool."""
    options = dict(options)
    options['name'] = getToolName('BTagTrackToJetAssociator', TrackCollection, JetCollection)
    tool = getTool('BTagTrackToJetAssociator', TrackCollection, JetCollection)
    if tool is None:
        print '#BTAG# - INFO - Setting up TrackAssociator for combination: '+str((TrackCollection, JetCollection))
        tool = toolBTagTrackToJetAssociator(**options)
        registerTool('BTagTrackToJetAssociator', tool, track = TrackCollection, jetcol = JetCollection, ToolSvc = ToolSvc)
    else:
        print '#BTAG# - INFO - Adding TrackAssociator for '+TrackCollection+' which already exists now also to jet collection: '+JetCollection
    global _BTaggingConfig_TrackAssociators
    global _BTaggingConfig_TrackConCol
    if _BTaggingConfig_TrackAssociators.get((TrackCollection, JetCollection), None) is None:
        _BTaggingConfig_TrackAssociators[(TrackCollection, JetCollection)] = tool
        if len(ContainerName) == 0:
            ContainerName = BTaggingFlags.TrackParticleCollectionName
        _BTaggingConfig_TrackConCol[(TrackCollection, JetCollection)] = (ContainerName, TrackCollection)
    else:
        print '#BTAG# - WARNING - SetupTrackAssociator() called for combinations which already exists: '+str((TrackCollection,JetCollection))
    return tool

def getMuonAssociatorData(JetCollection):
    """Returns a list of tuples containing data needed for the specified jet collection. The following format is used:
    (tool, MuonContainerName, MuonCollectionName)."""
    returnlist = []
    for assoc in _BTaggingConfig_MuonAssociators:
        if JetCollection == assoc[1]:
            x = [_BTaggingConfig_MuonAssociators[assoc],]
            for concol in _BTaggingConfig_MuonConCol:
                if concol[1] == JetCollection and concol[0] == assoc[0]:
                    x += [_BTaggingConfig_MuonConCol[concol][0], _BTaggingConfig_MuonConCol[concol][1] ]
            if len(x) == 3:
                returnlist.append(x)
            else:
                print '#BTAG# - ERROR - getMuonAssociatorData() does not find enough metadata for jet collection: '+JetCollection
                raise ValueError
    return returnlist

def getElectronAssociatorData(JetCollection):
    """Returns a list of tuples containing data needed for the specified jet collection. The following format is used:
    (tool, ElectronContainerName, ElectronCollectionName, PhotonContainerName, PhotonCollectionName)."""
    returnlist = []
    for assoc in _BTaggingConfig_ElectronAssociators:
        if JetCollection == assoc[1]:
            x = [_BTaggingConfig_ElectronAssociators[assoc],]
            for concol in _BTaggingConfig_ElectronConCol:
                if concol[1] == JetCollection and concol[0] == assoc[0]:
                    x += [_BTaggingConfig_ElectronConCol[concol][0], _BTaggingConfig_ElectronConCol[concol][1] ]
            for concol in _BTaggingConfig_PhotonConCol:
                if concol[1] == JetCollection and concol[0] == assoc[0]:
                    x += [_BTaggingConfig_PhotonConCol[concol][0], _BTaggingConfig_PhotonConCol[concol][1] ]
            if len(x) == 5:
                returnlist.append(x)
            else:
                print '#BTAG# - ERROR - getElectronAssociatorData() does not find enough metadata for jet collection: '+JetCollection
                raise ValueError
    return returnlist

def getTrackAssociatorData(JetCollection):
    """Returns a list of tuples containing data needed for the specified jet collection. The following format is used:
    (tool, TrackContainerName, TrackCollectionName)."""
    returnlist = []
    for assoc in _BTaggingConfig_TrackAssociators:
        if JetCollection == assoc[1]:
            x = [_BTaggingConfig_TrackAssociators[assoc],]
            for concol in _BTaggingConfig_TrackConCol:
                if concol[1] == JetCollection and concol[0] == assoc[0]:
                    x += [_BTaggingConfig_TrackConCol[concol][0], _BTaggingConfig_TrackConCol[concol][1] ]
            if len(x) == 3:
                returnlist.append(x)
            else:
                print '#BTAG# - ERROR - getTrackAssociatorData() does not find enough metadata for jet collection: '+JetCollection
                raise ValueError
    return returnlist

def getMuonAssociator(MuonCollection, JetCollection):
    """Returns the muon associator for the specified muon and jet collection combination."""
    return _BTaggingConfig_MuonAssociators.get((MuonCollection, JetCollection), None)

def getElectronAssociator(ElectronCollection, JetCollection):
    """Returns the electron associator for the specified electron and jet collection combination."""
    return _BTaggingConfig_ElectronAssociators.get((ElectronCollection, JetCollection), None)

def getTrackAssociator(TrackCollection, JetCollection):
    """Returns the track associator for the specified track and jet collection combination."""
    return _BTaggingConfig_TrackAssociators.get((TrackCollection, JetCollection), None)

def addToolToJetCollection(tool_type, tool, jetcol, track, addInfoPlus = True):
    """Adds a tool to a jet collection if not done so already.

    input: tool_type:          The type of the tool to add.
           tool:               The actual tool.
           jetcol:             The jet collection.
           track:              The track selection this tool is running on.
           addInfoPlus:        Some taggers also add detailed information about what information was used
                               by the tagger. This toggle ensure that this data is stored."""
    global _BTaggingConfig_SetUpCombinations
    if (tool_type, jetcol, track) in _BTaggingConfig_SetUpCombinations:
        print '#BTAG# - WARNING - A tool of type "'+tool_type+'" was already set up for the combination '+str((track, jetcol))+' ignoring duplicate...'
        return
    if getToolMetadata(tool_type, 'IsATagger'):
        btagger = getJetCollectionTool(jetcol)
        btagger.TagToolList += [tool,]
    if getToolMetadata(tool_type, 'IsAVertexFinder'):
        # now we need the removal and secvtxfinder tools
        SecVertexingTool = getJetCollectionSecVertexingTool(jetcol)
        #RemovalTool = getJetCollectionRemovalTool(jetcol)
        # register our new vertex finder to them
        SecVertexingTool.SecVtxFinderList += [tool,]
        SecVertexingTool.SecVtxFinderTrackNameList += [track,]
        if not getToolMetadata(tool_type, 'VertexFinderxAODBaseName'):
            print '#BTAG# - ERROR - Tool '+getToolName(tool_type, track, jetcol)+' is set as a vertex finder, but no xAODBaseName was specified in the metadata.'
            raise ValueError
        SecVertexingTool.SecVtxFinderxAODBaseNameList += [getToolMetadata(tool_type, 'VertexFinderxAODBaseName'), ]
        VertexFinderPassByPointer = getToolMetadata(tool_type, 'VertexFinderPassByPointer')
        if VertexFinderPassByPointer:
            for element in VertexFinderPassByPointer:
                ToPass = _BTaggingConfig_InitializedTools.get(getToolName(VertexFinderPassByPointer[element],track,jetcol), None)
                if ToPass is None:             
                    print '#BTAG# - ERROR - Tool of type '+VertexFinderPassByPointer[element]+' to be passed as pointer to tool of type '+tool_type+' to the SecVertexingTool'
                    print '#BTAG# - ERROR - but this tool was not found/registered.'
                    raise ValueError
                setattr(SecVertexingTool, element, ToPass)
        #RemovalTool.listOfSvxToRemove += [getToolName(tool_type, track, jetcol),]
        #RemovalTool.listOfTracksToRemove += [track,]
    _BTaggingConfig_SetUpCombinations[(tool_type, jetcol, track)] = tool
    if BTaggingFlags.OutputLevel < 3:
        print '#BTAG# - DEBUG - Added tool '+getToolName(tool_type, track, jetcol)+' to jet collection: '+jetcol
    # Add to the tool also the jet collection (needed for reference running)
    jetcollist = getToolMetadata(tool_type, 'JetCollectionList')
    jetcolplus = getToolMetadata(tool_type, 'JetWithInfoPlus')
    if jetcollist:
        if not jetcol in getattr(tool, jetcollist):
            if BTaggingFlags.OutputLevel < 3:
                print '#BTAG# - DEBUG - Adding to tool '+getToolName(tool_type, track, jetcol)+' the jet collection: '+jetcol
            getattr(tool, jetcollist).append(jetcol)
    if jetcolplus and addInfoPlus:
        if not jetcol in getattr(tool, jetcolplus):
            if BTaggingFlags.OutputLevel < 3:
                print '#BTAG# - DEBUG - Adding to tool '+getToolName(tool_type, track, jetcol)+' the jet collection plus: '+jetcol
            getattr(tool, jetcolplus).append(jetcol)
    if BTaggingFlags.OutputLevel < 3:
        print '#BTAG# - DEBUG - '+str((tool_type, jetcol, track))+' -> '+getToolName(tool_type, track, jetcol)+' added'

def getJetCollectionMainAssociatorTool(jetcol):
    """Returns the main associator tool corresponding to the specified jet collection.

    input: jetcol:             Desired jet collection.

    output: The tool."""
    tool = _BTaggingConfig_MainAssociatorTools.get(jetcol, None)
    if tool is None:
        print '#BTAG# - ERROR - getJetCollectionMainAssociatorTool() returns none for '+jetcol
        raise ValueError
    return tool

def getJetCollectionRemovalTool(jetcol):
    """Returns the removal tool corresponding to the specified jet collection.

    input: jetcol:             Desired jet collection.

    output: The tool."""
    tool = _BTaggingConfig_RemovalTools.get(jetcol, None)
    if tool is None:
        print '#BTAG# - ERROR - getJetCollectionRemovalTool() returns none for '+jetcol
        raise ValueError
    return tool

def getJetCollectionSecVertexingTool(jetcol):
    """Returns the SecVtx tool corresponding to the specified jet collection.

    input: jetcol:             Desired jet collection.

    output: The tool."""
    tool = _BTaggingConfig_SecVtxTools.get(jetcol, None)
    if tool is None:
        print '#BTAG# - ERROR - getJetCollectionSecVertexingTool() returns none for '+jetcol
        raise ValueError
    return tool

def jetCollectionExists(JetCollection):
    """Returns whether a jet collection already has some configuration; in particular whether it has an assigned BTagTool."""
    if _BTaggingConfig_JetCollections.get(JetCollection, None) is None:
        return False
    return True

def getJetCollectionTool(JetCollection):
    """Returns the btagtool corresponding to the specified jet collection.

    input: JetCollection:             Desired jet collection.

    output: The btagtool."""
    btagger = _BTaggingConfig_JetCollections.get(JetCollection, None)
    if btagger is None:
        if BTaggingFlags.OutputLevel < 3:
            print '#BTAG# - DEBUG - getJetCollectionTool() returns none for '+JetCollection
    return btagger

def addJetCollectionTool(JetCollection, ToolSvc, Verbose = False, options={}):
    """Adds a jet collection, and sets up the corresponding btagtool, removal and secondary vertexing tools. 
    If it already exists it is simply returned.

    input: JetCollection:      The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           Verbose:            Whether to print detailed information about the tool.
           options:            Python dictionary of options to be passed to the BTagTool.
    output: The btagtool for thhe desired jet collection."""
    if _BTaggingConfig_JetCollections.get(JetCollection, None) is None:
        return setupBTagTool(JetCollection, ToolSvc, Verbose = Verbose, options=options)
    else:
        # The tool already exists, we just return it
        if len(options) > 0:
            print '#BTAG# - WARNING - A JetCollection called "'+JetCollection+'" has already been set up; ignoring new btagtool options.'
        return _BTaggingConfig_JetCollections.get(JetCollection, None)

def setupBTagTrackAssociation(name, ToolSvc, Verbose = False, options={}):
    """Adds a BTagTrackAssociation instance and registers it.

    input: name:               The tool's name.
           ToolSvc:            The ToolSvc instance.
           Verbose:            Whether to print detailed information about the tool.
           options:            Python dictionary of options to be passed to the tool.

    The following defaults exist for the options:

    BTagAssociation                              default: BTaggingFlags.doStandardAssoc

    output: The tool."""
    options = dict(options)
    options['name'] = name
    options.setdefault('BTagAssociation', BTaggingFlags.doStandardAssoc)
    from BTagging.BTaggingConf import Analysis__BTagTrackAssociation
    tool = Analysis__BTagTrackAssociation(**options)
    ToolSvc += tool
    return tool

def setupRemovalTool(name, ToolSvc, Verbose = False, options={}):
    """Adds a RemovalTool instance and registers it.

    input: name:               The tool's name.
           ToolSvc:            The ToolSvc instance.
           Verbose:            Whether to print detailed information about the tool.
           options:            Python dictionary of options to be passed to the RemovalTool.

    The following defaults exist for the options:

    storeSecondaryVerticesInJet                default: BTaggingFlags.writeSecondaryVertices

    output: The tool."""
    options = dict(options)
    options.setdefault('listOfTracksToRemove', [])
    options.setdefault('listOfSvxToRemove', [])
    options['name'] = name
    options.setdefault('storeSecondaryVerticesInJet', BTaggingFlags.writeSecondaryVertices)
    from BTagging.BTaggingConf import Analysis__BTagRemoving
    tool = Analysis__BTagRemoving(**options)
    ToolSvc += tool
    return tool

def setupSecVtxTool(name, ToolSvc, Verbose = False, options={}):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The tool's name.
           ToolSvc:            The ToolSvc instance.
           Verbose:            Whether to print detailed information about the tool.
           options:            Python dictionary of options to be passed to the SecVtxTool.
    output: The tool."""
    options = dict(options)
    options.setdefault('SecVtxFinderList', [])
    options.setdefault('SecVtxFinderTrackNameList', [])
    options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
    options['name'] = name
    from BTagging.BTaggingConf import Analysis__BTagSecVertexing
    tool = Analysis__BTagSecVertexing(**options)
    ToolSvc += tool
    return tool

def setupBTagTool(jetcol, ToolSvc, Verbose = False, options={}):
    """Adds a new myBTagTool instance and registers it.

    input: jetcol:             The name of the jet collections.
           ToolSvc:            The ToolSvc instance.
           Verbose:            Whether to print detailed information about the tool.
           options:            Python dictionary of options to be passed to the BTagTool.
           (note the options storeSecondaryVerticesInJet is passed to the removal tool instead)

    The following default options exist:
    
    BTagLabelingTool                       default: None
    storeSecondaryVerticesInJet            default: BTaggingFlags.writeSecondaryVertices

    output: The btagtool for the desired jet collection."""
    options = dict(options)
    # Setup a removal tool for it
    options.setdefault('storeSecondaryVerticesInJet', BTaggingFlags.writeSecondaryVertices)
    thisSecVtxTool = setupSecVtxTool('thisBTagSecVertexing_'+jetcol, ToolSvc, Verbose)
    #thisRemovalTool = setupRemovalTool('thisBTagRemoving_'+jetcol, 
    #                                   ToolSvc, 
    #                                   Verbose,
    #                                   options = {'storeSecondaryVerticesInJet' : options['storeSecondaryVerticesInJet']})
    #global _BTaggingConfig_RemovalTools
    global _BTaggingConfig_SecVtxTools
    #_BTaggingConfig_RemovalTools[jetcol] = thisRemovalTool
    _BTaggingConfig_SecVtxTools[jetcol] = thisSecVtxTool
    #options['BTagSecVertexingTool'] = thisSecVtxTool # MOVED TO JETBTAGGERTOOL
    #options['BTagRemovingTool'] = thisRemovalTool
    del options['storeSecondaryVerticesInJet'] # we don't want it passed to the main b-tag tool
    options['name'] = 'myBTagTool_'+jetcol
    options.setdefault('BTagLabelingTool', None)
    btagtool = toolMainBTaggingTool(**options)
    if BTaggingFlags.OutputLevel < 3:
        print '#BTAG# - DEBUG - Setting up BTagTool for jet collection: '+jetcol
    global _BTaggingConfig_JetCollections
    if _BTaggingConfig_JetCollections.get(jetcol, None) is None:
        # Store this one to the jet collections
        _BTaggingConfig_JetCollections[jetcol] = btagtool
        ToolSvc += btagtool
    else:
        print '#BTAG# - ERROR - Setting up BTagTool for a jet collection which was already set up previously!'
        raise ValueError
    return btagtool

def setupDefaultTool(tool_type, ToolSvc, track="", jetcol="", Verbose = False, name="", addInfoPlus = False, options={}, MuonCollection='Muons', ElectronCollection='Electrons',PhotonCollection='Photons'):
    """Sets up the specified tool (by tool_type) using default configuration.

    input: tool_type:          The type of the tool to be set up.
           ToolSvc:            The ToolSvc (enter None if you don't want to register the tool).
           track:              The name of the tracks to be used.
           jetcol:             The name of the jet collection to be used.
           Verbose:            Whether to print the tool and all those that are pre set up.
           CheckPrerequisites: Whether to set up default prerequisites as well (if needed).
           Prefix:             Prefix to be used for this tool and all tools set up by default.
           name:               The name of the tool. If not specified a default name is used which
                               is a combination of the prefix and the tool type. Note that it is 
                               generally ill-advised to choose your own name. The reason is that some
                               prerequisite tools refer to other tools by-name and not via a pointer.
                               It is possible to break things by forcing your own name. Only do so when
                               you are sure there will be no conflict or are not using the dependency
                               system.
           addInfoPlus:        Some taggers also add detailed information about what information was used
                               by the tagger. This toggle ensure that this data is stored.
           options:            Python dictionary (!) with options that will be passed to the tool,
                               options for which there is no key in the dictionary will use defaults.
           MuonCollection:     Muon collection. Can be left blank if not needed or using defaults.
           ElectronCollection: Electron collection. Can be left blank if not needed or using defaults.
           PhotonCollection:   Photon collection. Can be left blank if not needed or using defaults.
    output: The tool itself."""
    # This is to hold the arguments that need to be passed to the b-tagging tool,
    # this is needed because the tool will only be added to ToolSvc at the end and
    # it is not allowed to add non-ToolSvc tools to the b-tagging tool.
    if len(name) < 1:
        name = getToolName(tool_type, track, jetcol)
    options = dict(options) # Make a new instance (otherwise we will change the actual dict)
    options['name'] = name
    # Add some dynamic settings to the options dictionary (this will raise a NotImplementedError is metadata is not found)
    PassByPointer = getToolMetadata(tool_type, 'PassByPointer')
    PassByName = getToolMetadata(tool_type, 'PassByName')
    PassTracksAs = getToolMetadata(tool_type, 'PassTracksAs')
    CalibrationFolders = getToolMetadata(tool_type, 'CalibrationFolders')
    PassMuonCollectionAs = getToolMetadata(tool_type, 'PassMuonCollectionAs')
    PassElectronCollectionAs = getToolMetadata(tool_type, 'PassElectronCollectionAs')
    PassPhotonCollectionAs = getToolMetadata(tool_type, 'PassPhotonCollectionAs')
    if PassMuonCollectionAs:
        options.setdefault(PassMuonCollectionAs, MuonCollection)
    if PassElectronCollectionAs:
        options.setdefault(PassElectronCollectionAs, ElectronCollection)
    if PassPhotonCollectionAs:
        options.setdefault(PassPhotonCollectionAs, PhotonCollection)
    if PassByPointer:
        for element in PassByPointer:
            # Find the tool
            ToPass = _BTaggingConfig_InitializedTools.get(getToolName(PassByPointer[element],track,jetcol), None)
            if ToPass is None and not element in options: # allow for options overwrite
                print '#BTAG# - ERROR - Tool of type '+PassByPointer[element]+' to be passed as pointer to tool of type '+tool_type
                print '#BTAG# - ERROR - but this tool was not found/registered. (Can be overwritten using options passed to the set up script).'
                raise ValueError
            options.setdefault(element, ToPass)
    if PassByName:
        for element in PassByName:
            # No check on if these tools actually exist; PassByName is commonly used to link to tools that have yet to be set up
            options.setdefault(element, getToolName(PassByName[element],track,jetcol))
    if PassTracksAs:
        if len(track) == 0:
            print '#BTAG# - ERROR - setupDefaultTool() called for a tool that has PassTracksAs metadata but without specifying a track collection.'
            raise ValueError
        options.setdefault(PassTracksAs, track)
    if CalibrationFolders:
        if BTaggingFlags.CalibrationSingleFolder is False:
            print '#BTAG# - ERROR - BTaggingFlags.CalibrationSingleFolder is False, the new calibration scheme does not support this!'
            print '#BTAG# - ERROR - Calibrations will not function!'
            raise ValueError
        else:
            broker = _BTaggingConfig_InitializedTools.get(getToolName('BTagCalibrationBrokerTool',track,jetcol), None)
            if broker is None:
                print '#BTAG# - ERROR - CalibrationBroker not found; calibrations will not function!'
                raise ValueError
            elif not hasattr(broker, 'folders'):
                print '#BTAG# - ERROR - CalibrationBroker does not have "folders" as an attribute; calibrations will not function!'
                raise ValueError
            for folder in CalibrationFolders:
                ToAdd = BTaggingFlags.CalibrationFolderRoot + folder
                if not ToAdd in broker.folders:
                    broker.folders.append(ToAdd)                
    # Set up the actual tool
    try:
        exec('tool = tool'+tool_type+'(**options)')
    except:
        print '#BTAG# - ERROR - Attempting to set up a tool of type "'+tool_type+'" but this tool was not/incorrectly implemented.'
        raise
    # Fill in jet collection lists if applicable
    jetcollist = getToolMetadata(tool_type, 'JetCollectionList')
    jetcolplus = getToolMetadata(tool_type, 'JetWithInfoPlus')
    if jetcollist:
        setattr(tool, jetcollist, [jetcol,])
    if jetcolplus and addInfoPlus:
        setattr(tool, jetcolplus, [jetcol,])
    # Register it with the dependency system
    registerTool(tool_type, tool, track, jetcol, ToolSvc = ToolSvc, Verbose = Verbose)
    return tool

def toolMainBTaggingTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up the main B-tagging tool and returns it.
   
    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTaggingFlags.Runmodus
    PrimaryVertexName                   default: BTaggingFlags.PrimaryVertexCollectionName
    BaselineTagger                      default: BTaggingFlags.BaselineTagger
    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'Runmodus'                     : BTaggingFlags.Runmodus,
                     'PrimaryVertexName'            : BTaggingFlags.PrimaryVertexCollectionName,
                     'BaselineTagger'               : BTaggingFlags.BaselineTagger,
                     'OutputLevel'                  : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from BTagging.BTaggingConf import Analysis__BTagTool
    return Analysis__BTagTool(**options)

metaBTagTrackToJetAssociator = {}

def toolBTagTrackToJetAssociator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagTrackToJetAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__ParticleToJetAssociator
    return Analysis__ParticleToJetAssociator(**options)

metaBTagMuonToJetAssociator = { 'DefaultTracks' : 'Muons' } #This is a slight misuse of notation, but it works

def toolBTagMuonToJetAssociator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagMuonToJetAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    trackCone                           default: 1.0 (for the time being... has to be studied)
    useVariableSizedTrackCone           default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'               : BTaggingFlags.OutputLevel,
                     'trackCone'                 : 1.0,
                     'useVariableSizedTrackCone' : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__ParticleToJetAssociator
    return Analysis__ParticleToJetAssociator(**options)

metaBTagElectronToJetAssociator = { 'DefaultTracks' : 'Electrons' } #This is a slight misuse of notation, but it works

def toolBTagElectronToJetAssociator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagElectronToJetAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    trackCone                           default: 0.4 (for the time being... has to be studied)
    useVariableSizedTrackCone           default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'               : BTaggingFlags.OutputLevel,
                     'trackCone'                 : 0.4,
                     'useVariableSizedTrackCone' : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__ParticleToJetAssociator
    return Analysis__ParticleToJetAssociator(**options)

def listTaggers():
    """Returns a list of all taggers."""
    return BTaggingFlags._tags

def checkFlagsUsingBTaggingFlags():
    """Runs over all taggers and disables them if various detector components were switched off, we have a specific scoping level or running in reference mode.

    output: Returns whether b-tagging is to be active or not."""
    if not BTaggingFlags.Active:
        print '#BTAG# - Disabling whole b-tagging since BTaggingFlags.Active is False...'
        return False
    print '#BTAG# - B-tagging descoping:  level = ',rec.ScopingLevel()
    from AthenaCommon.BFieldFlags import jobproperties
    if not jobproperties.Beam.beamType == "collisions":
        print '#BTAG# - Disabling whole b-tagging since beamType is not collisions...'
        BTaggingFlags.Active = False
        return False
    if jobproperties.BField.solenoidOn() == False:
        print '#BTAG# - Disabling whole b-tagging since solenoid is OFF...'
        BTaggingFlags.Active = False
        return False
    if not IgnoreTaggerObstacles:
        for tagger in BTaggingFlags._tags:
            if not taggerIsPossible(tagger):
                BTaggingFlags.Disable(tagger)
    else:
        print '#BTAG# - WARNING - Not checking for obstacles for taggers (DetFlags etc.)!'
    return True

def disableAllTaggers():
    """Disables all BTaggingFlags that are tagger flags."""
    for tagger in BTaggingFlags._tags:
        BTaggingFlags.Disable(tagger)

def getActiveTaggers():
    """Returns a list of taggers that are active according to the BTaggingFlags module."""
    returnlist = []
    for tagger in BTaggingFlags._tags:
        if(BTaggingFlags.IsEnabled(tagger)):
            returnlist.append(tagger)
    return returnlist

def taggerIsPossible(tagger):
    """Returns whether a tagger can be used depending on various criteria."""
    # disable tagger is it is unknown
    if not tagger in BTaggingFlags._tags:
        print '#BTAG# - WARNING - Unknown tagger ('+tagger+') encountered, disabling...'
        return False
    # disable all taggers if B-tagging is not active
    if not BTaggingFlags.Active:
        print '#BTAG# - Disabling '+tagger+' tagger because B-tagging has been disabled...'
        return False
    # disable some taggers if we are running in reference mode
    if BTaggingFlags.Runmodus == 'reference':
        if tagger in ['lifetime1D',
                      'lifetime2D',
                      'lifetime3D',
                      'secVtxFitTD',
                      'secVtxFitBU',
                      'JetProbFlip',
                      'IP2DFlip',
                      'IP2DPos',
                      'IP2DNeg',
                      'IP2DSpcFlip',
                      'IP2DSpcPos',
                      'IP2DSpcNeg',
                      'IP3DFlip',
                      'IP3DPos',
                      'IP3DNeg',
                      'IP3DSpcFlip',
                      'IP3DSpcPos',
                      'IP3DSpcNeg',
                      'SV1Flip',
                      'SV2Flip',
                      'TrackCountingFlip',
                      'MultiSVTag',
                      'GbbNNTag',
                      'MV1',
                      'MV2',
                      'MV1Flip',
                      'MV2Flip' ]:
            print '#BTAG# - Disabling '+tagger+' tagger due to reference run modus...'
            return False
    # disable taggers accordingly to reconstruction scoping levels
    if rec.ScopingLevel() >= 2:
        if tagger in BTaggingFlags.LowPriorityTaggers:
            print '#BTAG# - Disabling low priority '+tagger+' tagger due to scoping level >= 2'
            return False
    if rec.ScopingLevel() >= 3:
        if tagger in BTaggingFlags.MediumPriorityTaggers:
            print '#BTAG# - Disabling medium priority '+tagger+' tagger due to scoping level >= 3'
            return False
    if rec.ScopingLevel() >= 4:
        if tagger in BTaggingFlags.HighPriorityTaggers:
            print '#BTAG# - Disabling high priority '+tagger+' tagger due to scoping level >= 4'
            return False
    # disable specific taggers if geometry is not defined
    from AthenaCommon.DetFlags import DetFlags
    if not DetFlags.detdescr.ID_on():
        print '#BTAG# - Disabling '+tagger+' tagger because ID detdescr. was not found'
        return False
    if tagger in ['SoftEl']:
        if not DetFlags.detdescr.Calo_on():
            print '#BTAG# - Disabling '+tagger+' tagger because calo detdescr. was not found'
            return False
    if tagger in ['SoftMu',
                  'SoftMuChi2',
                  'SecondSoftMu',
                  'SecondSoftMuChi2']:
        if not DetFlags.detdescr.Muon_on():
            print '#BTAG# - Disabling '+tagger+' tagger because muon detdescr. was not found'
            return False
    return True

# Fill up the tool collections during import
# DO NOT DELETE THIS SECTION; IT IS VITAL!
for tool in dir():
    if tool.find('meta') == 0:
        tname = tool[4:]
        exec 'metadict = meta'+tname
        tcoll = tname
        if 'ToolCollection' in metadict:
            tcoll = metadict['ToolCollection']
        addToolTypeToToolCollection(tname, tcoll)
