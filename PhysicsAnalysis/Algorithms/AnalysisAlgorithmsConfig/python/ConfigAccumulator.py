# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def mapUserName (name) :
    """map an internal name to a name for systematics data handles

    Right now this just means appending a _%SYS% to the name."""
    return name + "_%SYS%"




class ContainerConfig :
    """all the auto-generated meta-configuration data for a single container

    This tracks the naming of all temporary containers, as well as all the
    selection decorations."""

    def __init__ (self, name, sourceName) :
        self.name = name
        self.sourceName = sourceName
        self.index = 0
        self.viewIndex = 1
        self.selection = {}
        self.selection[''] = []
        self.selectionBits = {}
        self.selectionBits[''] = []


    def indexName (self, index) :
        """the name of the (temporary) container at the processing
        step `index`"""

        if index == 0 :
            if not self.sourceName :
                raise Exception ("no source name defined for " + self.name)
            return self.sourceName
        if index == self.index :
            return mapUserName (self.name)
        return mapUserName(self.name + "_STEP" + str(index))




class ContainerReadRef :
    """a read-only reference to a container"""

    def __init__ (self, config, readIndex) :
        self._config = config
        self._readIndex = readIndex

    def sourceContainer (self):
        """get the original container for the container"""
        if not self._config.sourceName:
            raise Exception ('no source container for ' + self.config.name)
        return self._config.sourceName

    def input (self) :
        """get the name of the input container in the event store"""
        return self._config.indexName (self._readIndex)

    def wantCopy (self) :
        """whether a shallow copy should be configured with this reference"""
        return False




class ContainerUpdateRef :
    """a copy/update reference to a container (i.e. making a copy)"""

    def __init__ (self, config, readIndex, writeIndex) :
        self._config = config
        self._readIndex = readIndex
        self._writeIndex = writeIndex

    def sourceContainer (self):
        """get the original container for the container"""
        if not self._config.sourceName:
            raise Exception ('no source container for ' + self.config.name)
        return self._config.sourceName

    def input (self) :
        """get the name of the input container in the event store"""
        return self._config.indexName (self._readIndex)

    def output (self) :
        """get the name of the output container in the event store"""
        return self._config.indexName (self._writeIndex)

    def wantCopy (self) :
        """whether a shallow copy should be configured with this reference"""
        return True




class ContainerWriteRef :
    """a write reference to a container (creating a new container)"""

    def __init__ (self, config) :
        self._config = config

    def output (self) :
        """get the name of the output container in the event store"""
        return self._config.indexName (1)




class ContainerViewRef :
    """a view reference to a container (providing a view container based
    on a selection"""

    def __init__ (self, config, baseConfig, baseRef, selection) :
        self._config = config
        self._baseConfig = baseConfig
        self._baseRef = baseRef
        self._selection = selection
        self._scheduledAlg = False

    def sourceContainer (self):
        """get the original container for the container"""
        return self._baseRef.sourceContainer()

    def input (self) :
        """get the name of the input container in the event store"""

        if not self._scheduledAlg :
            viewIndex = self._config.viewIndex
            self._config.viewIndex += 1
            inputContainer = self._baseRef.input()
            outputContainer = mapUserName(self._config.name + "_VIEW" + str(viewIndex))
            self.outputContainer = outputContainer
            from AnaAlgorithm.DualUseConfig import createAlgorithm
            viewalg = createAlgorithm( 'CP::AsgViewFromSelectionAlg', self._config.name + 'View' + str (viewIndex) + 'Alg' )
            viewalg.selection = [ self._selection ]
            viewalg.input = inputContainer
            viewalg.output = outputContainer
            self._baseConfig.addAlg (viewalg)
            self._scheduledAlg = True

        return self.outputContainer




class ConfigAccumulator :
    """a class that accumulates a configuration from blocks into an
    algorithm sequence

    This is used as argument to the ConfigurationBlock methods, which
    need to be called in the correct order.  This class will track all
    meta-information that needs to be communicated between blocks
    during configuration, and also add the created algorithms to the
    sequence.

    Use/access of containers in the event store is handled via
    references that this class hands out.  This happens in a separate
    step before the algorithms are created, as the naming of
    containers will depend on where in the chain the container is
    used.
    """


    def __init__ (self, dataType, algSeq):
        if dataType not in ["data", "mc", "afii"] :
            raise ValueError ("invalid data type: " + dataType)
        self._dataType = dataType
        self._algSeq = algSeq
        self._containerConfig = {}

    def dataType (self) :
        """the data type we run on (data, mc, afii)"""
        return self._dataType

    def addAlg (self, alg) :
        """add an algorithm to the sequence being created"""
        self._algSeq += alg

    def addPublicTool (self, tool) :
        """add a public tool to the sequence being created"""
        try:
            # Try to access the ToolSvc, to see whethet we're in Athena mode:
            from AthenaCommon.AppMgr import ToolSvc  # noqa: F401
        except ImportError:
            # We're not, so let's remember this as a "normal" algorithm:
            self._algSeq += tool


    def readOrUpdateRef (self, containerName, sourceName) :
        """get a reference object to decorate the given container with
        an option to update it

        The idea is that for the first algorithm in the sequence we
        usually want to make a shallow copy, and usually do that by
        taking advantage of copy-handles that can optionally copy.  So
        for every algorithm that could be the first in a sequence we
        use this kind of reference which will either return a
        read-reference or an update reference based on the context in
        which it is used.

        Callers should check the wantCopy() member function on the
        reference to see whether a copy should be configured.
        """
        if containerName not in self._containerConfig :
            if not sourceName :
                raise Exception ("no source container for: " + containerName)
            self._containerConfig[containerName] = ContainerConfig (containerName, sourceName)
            config = self._containerConfig[containerName]
            config.index += 1
            return ContainerUpdateRef (config, config.index-1, config.index)
        else :
            config = self._containerConfig[containerName]
            return ContainerReadRef (config, config.index)


    def updateRef (self, containerName, sourceName) :
        """get a reference object to update/copy the given container"""
        if containerName not in self._containerConfig :
            if not sourceName :
                raise Exception ("no source container for: " + containerName)
            self._containerConfig[containerName] = ContainerConfig (containerName, sourceName)
        config = self._containerConfig[containerName]
        config.index += 1
        return ContainerUpdateRef (config, config.index-1, config.index)


    def writeRef (self, containerName) :
        """get a reference object to write/create the given container"""
        if containerName in self._containerConfig :
            raise Exception ("trying to create already existing container: " + containerName)
        self._containerConfig[containerName] = ContainerConfig (containerName, None)
        config = self._containerConfig[containerName]
        config.index = 1
        return ContainerWriteRef (config)


    def readRef (self, containerName) :
        """get a reference object to read the given container"""
        if containerName not in self._containerConfig :
            self._containerConfig[containerName] = ContainerConfig (containerName, containerName)
        config = self._containerConfig[containerName]
        return ContainerReadRef (config, config.index)


    def viewRef (self, containerName) :
        """get a reference object to read the given container with the given selection

        The idea here is that this allows the user to specify a given view
        as e.g. MyElectrons.tight and the reference object will then return
        the name of a view container in the event store that has the
        selection applied.  This brings the handling of algorithms with a
        view-based preselection in line with other algorithms.

        WARNING: Unlike the other references this may/will schedule an
        algorithm when queried (to create the view), so care must be
        taken to query the container name at the right time."""
        split = containerName.split ('.')
        if len(split) == 1:
            return self.readRef (containerName)
        if len(split) != 2:
            raise Exception ('invalid name for a view input: ' + containerName)
        baseRef = self.readRef(split[0])
        config = self._containerConfig[split[0]]
        return ContainerViewRef (config, self, baseRef, split[1])


    def getSelection (self, containerName, selectionName) :
        """get the selection string for the given selection on the given
        container"""
        if containerName not in self._containerConfig :
            return ""
        config = self._containerConfig[containerName]
        selection = config.selection['']
        if selectionName != '' and selectionName in config.selection :
            selection = selection + config.selection[selectionName]
        return '&&'.join (selection)


    def addSelection (self, containerName, selectionName, selectionDecoration, selectionBits) :
        """add another selection decoration to the selection of the given
        name for the given container

        This also takes the number of bits in the selection decoration,
        which is needed to make object cut flows."""
        if containerName not in self._containerConfig :
            self._containerConfig[containerName] = ContainerConfig (containerName, containerName)
        config = self._containerConfig[containerName]
        if selectionName not in config.selection :
            config.selection[selectionName] = []
            config.selectionBits[selectionName] = []
        config.selection[selectionName].append (selectionDecoration)
        config.selectionBits[selectionName].append (selectionBits)
