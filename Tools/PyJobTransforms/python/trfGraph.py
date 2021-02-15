# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfGraph
## @brief Transform graph utilities
#  @details Graph which represents transform executors (nodes) connected vis data types (edges)
#  @author atlas-comp-transforms-dev@cern.ch
#  @note  There are a few well established python graph implementations, but none seem to be in the ATLAS
#  release (NetworkX, igraph). Our needs are so basic that we might well be able to just take a few well
#  known routines and have them in this module. See, e.g., http://www.python.org/doc/essays/graphs.html
#  @note Basic idea is to have nodes representing athena jobs (sub-steps), with edges representing data.
#  (It turns out this works, which is not true for the reverse representation when a job requires
#  multiple inputs, e.g., DQHist merging needs the HIST_ESD and the HIST_AOD inputs, as edges can only
#  connect 2 nodes; in contrast nodes can have an arbitrary number of edge connections.)
#  The nodes have multiple input data types. Having multiple data types generally means that the
#  node can execute is any of them are present (e.g., RDO or RAW). However, when multiple inputs
#  are needed to execute these are bound into a tuple. 
#  @note We do not track one path through the graph - we track one path for each data type we need
#  to produce and record which nodes get hit. Then each hit node is executed in order. Need to record
#  which data objects are going to be produced ephemerally. One of the most important problems is that
#  we are trying to minimise the cost of all the paths we need to take. To do this we start with
#  data sorted in topological order (i.e., data which is produced earliest in the process first).
#  Each path is traced back to the starting node and the cheapest is taken. Once nodes have been 
#  switched on for one data type they are assumed free for other data types.
#  @note We represent a dataless edge with the fake data inputs and outputs {in,out}NULL. These
#  are used by the graph tracer, but then removed from the input/output of the actual substeps.


import copy
import os

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfExceptions as trfExceptions

from PyJobTransforms.trfExitCodes import trfExit




## @brief Simple graph object describing the links between executors
class executorGraph(object):
    
    ## @brief Initialise executor graph
    #  @param executorSet Set of executor instances
    #  @param inputData Iterable with input data for this transform's execution
    #  @param outputData Iterable with output data for this transform's execution
    def __init__(self, executorSet, inputData = set([]), outputData = set([])):
        
        # Set basic node list
        self._nodeDict = {}
        
        msg.info('Transform graph input data: {0}; output data {1}'.format(inputData, outputData))
        
        if len(executorSet) == 1:
            # Single executor - in this case inData/outData is not mandatory, so we set them to the 
            # input/output data of the transform
            executor = list(executorSet)[0]
            if len(executor._inData) == 0 and len(executor._outData) == 0:
                executor.inData = inputData
                executor.outData = outputData
        
        for executor in executorSet:
            self.addNode(executor)
            
        self._inputData = set(inputData)
        self._outputData = set(outputData)
        
        # It's forbidden for a transform to consume and produce the same datatype
        dataOverlap = self._inputData & self._outputData
        if len(dataOverlap) > 0:
            raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_GRAPH_ERROR'), 
                                                        'Transform definition error, you cannot produce and consume the same datatypes in a transform. Duplicated input/output types {0}.'.format(' '.join(dataOverlap)))

        # Add a pseudo-start/stop nodes, from which input data flows and output data finally arrives
        # This makes the graph 'concrete' for this job
        # This is useful as then data edges all connect properly to a pair of nodes
        # We add a node for every possible output as this enables topo sorting of the graph
        # nodes for any intermediate data end nodes as well
        pseudoNodes = dict()
        pseudoNodes['_start'] = graphNode(name='_start', inData=[], outData=self._inputData, weight = 0)
        for node in self._nodeDict.values():
            for dataType in node.outputDataTypes:
                endNodeName = '_end_{0}'.format(dataType)
                pseudoNodes[endNodeName] = graphNode(name=endNodeName, inData=[dataType], outData=[], weight = 0)
        self._nodeDict.update(pseudoNodes)
        
        # Toposort not yet done
        self._toposort = []
        self._toposortData = []

        # Now find connections between nodes
        self.findConnections()
    
    @property
    def inputData(self):
        return self._inputData
    
    @inputData.setter
    def inputData(self, inputData):
        self._inputData = set(inputData)
    
    @property
    def outputData(self):
        return self._outputData
    
    @outputData.setter
    def outputData(self, outputData):
        self._outputData = set(outputData)
        
    ## @brief Return a list of execution nodes with their data inputs/outputs
    @property
    def execution(self):
        exeList = []
        for nodeName in self._toposort:
            # Start and end nodes are not real - they never actually execute
            if nodeName.startswith(('_start', '_end')):
                continue
            if self._execution[nodeName]['enabled'] is True:
                exeList.append({'name': nodeName, 'input': self._execution[nodeName]['input'], 
                                'output': self._execution[nodeName]['output']})
        return exeList
    
    ## @brief Return a list of all data used in this execution
    @property
    def data(self):
        dataset = set()
        for nodeName in self._toposort:
            # Start and end nodes are not real - they never actually execute
            if nodeName.startswith(('_start', '_end')):
                continue
            if self._execution[nodeName]['enabled'] is True:
                dataset.update(self._execution[nodeName]['input'])
                dataset.update(self._execution[nodeName]['output'])
        return dataset
    
    ## @brief Add an executor node to the graph
    def addNode(self, executor):
        self._nodeDict[executor.name] = executorNode(executor)
    
    
    ## @brief Remove an executor node from the graph
    def deleteNote(self, executor):
        if executor.name in self._nodeDict:
            del(self._nodeDict[executor.name])
    
    
    def _resetConnections(self):
        for node in self._nodeDict.values():
            node.resetConnections()
    
    ## @brief Look at executor nodes and work out how they are connected
    #  @note Anything better than n^2? Should be ok for our low numbers of nodes, but could be optimised
    def findConnections(self):
        self._resetConnections()
        for nodeNameA, nodeA in self._nodeDict.items():
            for nodeNameB, nodeB in self._nodeDict.items():
                if nodeNameA == nodeNameB:
                    continue
                dataIntersection = list(set(nodeA.outputDataTypes) & set(nodeB.inputDataTypes))
                msg.debug('Data connections between {0} and {1}: {2}'.format(nodeNameA, nodeNameB, dataIntersection))
                if len(dataIntersection) > 0:
                    nodeA.addConnection(nodeNameB, dataIntersection, direction='out')
                    nodeB.addConnection(nodeNameA, dataIntersection, direction='in')
                    
        msg.debug('Graph connections are: \n{0}'.format(self))
                    
    ## @brief Find a topologically sorted list of the graph nodes
    #  @note If this is not possible, the graph is not a DAG - not supported
    #  @note See http://en.wikipedia.org/wiki/Topological_sorting
    def doToposort(self):
        # We will manipulate the graph, so deepcopy it
        graphCopy = copy.deepcopy(self._nodeDict)
        # Find all valid start nodes in this graph - ones with no data dependencies themselves
        startNodeNames = []
        for nodeName, node in graphCopy.items():
            if len(node.connections['in']) == 0:
                startNodeNames.append(nodeName)

        if len(startNodeNames) == 0:
            raise trfExceptions.TransformGraphException(trfExit.nameToCode('TRF_GRAPH_ERROR'), 
                                                        'There are no starting nodes in this graph - non-DAG graphs are not supported')

        msg.debug('Found this list of start nodes for toposort: {0}'.format(startNodeNames))

        # The startNodeNames holds the list of nodes with their dependencies now satisfied (no input edges anymore)
        while len(startNodeNames) > 0:
            # Take the next startNodeName and zap it from the graph
            theNodeName = startNodeNames.pop()
            theNode = graphCopy[theNodeName]
            self._toposort.append(theNodeName)
            del graphCopy[theNodeName]
            
            # Now delete the edges this node was a source for
            msg.debug('Considering connections from node {0}'.format(theNodeName))
            for connectedNodeName in theNode.connections['out']:
                graphCopy[connectedNodeName].delConnection(toExe = theNodeName, direction = 'in')
                # Look for nodes which now have their dependencies satisfied
                if len(graphCopy[connectedNodeName].connections['in']) == 0:
                    startNodeNames.append(connectedNodeName)
        
        # If there are nodes left then the graph has cycles, which means it's not a DAG        
        if len(graphCopy) > 0:
            raise trfExceptions.TransformGraphException(trfExit.nameToCode('TRF_GRAPH_ERROR'), 
                                                        'Graph topological sort had no more start nodes, but nodes were left {0} - non-DAG graphs are not supported'.format(list(graphCopy)))
            
        msg.debug('Topologically sorted node order: {0}'.format(self._toposort))
        
        # Now toposort the input data for nodes
        self._toposortData = []
        for nodeName in self._toposort:
            # First add input data, then output data
            for dataType in self._nodeDict[nodeName].inputDataTypes:
                if dataType not in self._toposortData:
                    self._toposortData.append(dataType)
            for dataType in self._nodeDict[nodeName].outputDataTypes:
                if dataType not in self._toposortData:
                    self._toposortData.append(dataType)
                    
        msg.debug('Topologically sorted data order: {0}'.format(self._toposortData))
    
    
    ## @brief Find the graph's execution nodes, from input to output data types
    #  with each activated step and the inputs/outputs
    #  @param @c outputDataTypes Data to produce
    #  @param @c inputDataTypes Data available as inputs
    def findExecutionPath(self):        
        # Switch off all nodes, except if we have a single node which is not data driven...
        self._execution = {}
        for nodeName, node in self._nodeDict.items():
            if len(self._nodeDict) == 1 and node.inputDataTypes == set() and node.inputDataTypes == set():
                self._execution[nodeName] = {'enabled' : True, 'input' : set(), 'output' : set()}
            else:
                self._execution[nodeName] = {'enabled' : False, 'input' : set(), 'output' : set()}

        dataToProduce = copy.deepcopy(self._outputData)
        dataAvailable = copy.deepcopy(self._inputData)
                
        # Consider the next data type in topo order
        while len(dataToProduce) > 0:
            nextDataType = None
            for dataType in self._toposortData:
                if dataType in dataToProduce:
                    nextDataType = dataType
                    dataToProduce.remove(nextDataType)
                    dataAvailable.update([nextDataType])
                    break

            if not nextDataType:
                msg.error('Still have to produce data type(s) {0}, but did not find anything in the toposorted data list ({1}).' 
                          ' Transform parameters/graph are broken so aborting.'.format(dataToProduce, self._toposortData))
                raise trfExceptions.TransformGraphException(trfExit.nameToCode('TRF_GRAPH_ERROR'), 
                                                            'Data type graph error')

            msg.debug('Next data type to try is {0}'.format(nextDataType))
            bestPath = self._bestPath(nextDataType, dataAvailable)
            
            msg.debug('Found best path for {0}: {1}'.format(nextDataType, bestPath))

            ## @note Use @c modPath to construct an array which we iterate over in pairs of (currentNode, nextNode)
            modPath = bestPath.path + [None]
            for (nodeName, nextNodeName) in [ (n, modPath[modPath.index(n)+1]) for n in bestPath.path ]:
                self._execution[nodeName]['enabled'] = True
                # Add the necessary data types to the output of the first node and the input of the next
                if nodeName in bestPath.newData:
                    self._execution[nodeName]['output'].update(bestPath.newData[nodeName])
                    for newData in bestPath.newData[nodeName]:
                        if newData not in dataAvailable:
                            dataToProduce.update([newData])
                if nextNodeName:
                    self._execution[nextNodeName]['input'].update(bestPath.newData[nodeName])
                    if nextNodeName in bestPath.extraData:
                        self._execution[nextNodeName]['input'].update(bestPath.extraData[nodeName])
                # Add any extra data we need (from multi-exit nodes) to the data to produce list
                for extraNodeData in bestPath.extraData.values():
                    for extra in extraNodeData:
                        if extra not in dataAvailable:
                            dataToProduce.update([extra])
                            
        # Now remove the fake data objects from activated nodes
        for node, props in self._execution.items():
            msg.debug('Removing fake data from node {0}'.format(node))
            props['input'] -= set(['inNULL', 'outNULL'])
            props['output'] -= set(['inNULL', 'outNULL'])

        msg.debug('Execution dictionary: {0}'.format(self._execution))
    
    
    ## @brief Find the best path from a end to a start node, producing a certain type of data
    #  given the set of currently available data and the current set of activated nodes
    #  @param data Data to produce
    #  @param dataAvailable Data types which can be used as sources
    #  @param startNodeName Find the path to this node (default '_start')
    #  @param endNodeName Find the path from this node (default '_end_DATATYPE')
    #  @details We can always ask the algorithm to trace the part from end to start 
    #  for this data type (this data is in endnode by construction). If we have to go
    #  along an edge where the data is not yet available then we need to add this data to
    #  our list of data to produce.
    def _bestPath(self, data, dataAvailable, startNodeName = '_start', endNodeName = None):
        
        if endNodeName is None:
            endNodeName = '_end_{0}'.format(data)
        
        if endNodeName not in self._nodeDict:
            raise trfExceptions.TransformGraphException(trfExit.nameToCode('TRF_GRAPH_ERROR'), 
                'Node {0} was not found - the transform data connection definition is broken'.format(endNodeName))

        
        # Set of all considered paths
        # Initialise this with our endNode name - algorithm works back to the start
        pathSet = [graphPath(endNodeName, data),]
        
        msg.debug('Started path finding with seed path {0}'.format(pathSet[0]))
        
        # Halting condition - only one path and its first element is startNodeName
        while len(pathSet) > 1 or pathSet[0].path[0] != startNodeName:
            msg.debug('Starting best path iteration with {0} paths in {1}'.format(len(pathSet), pathSet))
            # Copy the pathSet to do this, as we will update it
            for path in pathSet[:]:
                msg.debug('Continuing path finding with path {0}'.format(path))
                currentNodeName = path.path[0]
                if currentNodeName == startNodeName:
                    msg.debug('Path {0} has reached the start node - finished'.format(path))
                    continue
                # If there are no paths out of this node then it's a dead end - kill it
                if len(self._nodeDict[currentNodeName].connections['in']) == 0:
                    msg.debug('Path {0} is a dead end - removing'.format(path))
                    pathSet.remove(path)
                    continue
                # If there is only one path out of this node, we extend it
                if len(self._nodeDict[currentNodeName].connections['in']) == 1:
                    msg.debug('Single exit from path {0} - adding connection to {1}'.format(path, list(self._nodeDict[currentNodeName].connections['in'])[0]))
                    self._extendPath(path, currentNodeName, list(self._nodeDict[currentNodeName].connections['in'])[0])
                    continue
                # Else we need to clone the path for each possible exit
                msg.debug('Multiple exits from path {0} - will clone for each extra exit'.format([path]))
                for nextNodeName in list(self._nodeDict[currentNodeName].connections['in'])[1:]:
                    newPath = copy.deepcopy(path)
                    msg.debug('Cloned exit from path {0} to {1}'.format(newPath, nextNodeName))             
                    self._extendPath(newPath, currentNodeName, nextNodeName)
                    pathSet.append(newPath)
                # Finally, use the original path to extend along the first node exit
                msg.debug('Adding exit from original path {0} to {1}'.format(path, list(self._nodeDict[currentNodeName].connections['in'])[0]))
                self._extendPath(path, currentNodeName, list(self._nodeDict[currentNodeName].connections['in'])[0])

            # Now compare paths which made it to the end - only keep the shortest
            lowestCostPath = None
            for path in pathSet[:]:
                currentNodeName = path.path[0]
                if currentNodeName == startNodeName:
                    if lowestCostPath is None:
                        lowestCostPath = path
                        continue
                    if path.cost >= lowestCostPath.cost:
                        msg.debug('Path {0} is no cheaper than best path {1} - removing'.format(path, lowestCostPath))
                        pathSet.remove(path)
                    else:
                        msg.debug('Path {0} is cheaper than previous best path {1} - removing previous'.format(path, lowestCostPath))
                        pathSet.remove(lowestCostPath)
                        lowestCostPath = path
    
            # Emergency break
            if len(pathSet) == 0:
                raise trfExceptions.TransformGraphException(trfExit.nameToCode('TRF_GRAPH_ERROR'), 
                                                            'No path found between {0} and {1} for {2}'.format(startNodeName, endNodeName, data))
        return pathSet[0]

    
    ## @brief Connect a path to a particular node
    #  @param path graphPath instance
    #  @param nextNodeName Node to connect to
    def _extendPath(self, path, currentNodeName, nextNodeName):
        edgeData = self._nodeDict[currentNodeName].connections['in'][nextNodeName]
        msg.debug('Connecting {0} to {1} with data {2}'.format(currentNodeName, nextNodeName, edgeData))
        
        extraData = set()
        if self._execution[currentNodeName]['enabled'] is True:
            extraCost = 0
        else:
            for edgeDataElement in edgeData:
                # Simple case - one data connection only
                if edgeDataElement in self._nodeDict[currentNodeName].inData:
                    extraCost = self._nodeDict[currentNodeName].weights[edgeDataElement]
                else:
                    # Complex case - the start requirement for this node must be multi-data
                    # Only the first match in the dataIn lists is considered
                    # This will break if there are multiple overlapping dataIn requirements
                    for nodeStartData in self._nodeDict[currentNodeName].inData:
                        if isinstance(nodeStartData, (list, tuple)) and edgeDataElement in nodeStartData:
                            extraCost = self._nodeDict[currentNodeName].weights[nodeStartData]
                            msg.debug('Found multi-data exit from {0} to {1} - adding {2} to data requirements'.format(currentNodeName, nextNodeName, nodeStartData))
                            extraData.update(nodeStartData)
                            break
            # Remove data which is on the edge itself
            extraData.difference_update(edgeData)
            
        msg.debug('Updating path {0} with {1}, {2}, {3}, {4}'.format(path, nextNodeName, edgeData, extraData, extraCost))
        path.addToPath(nextNodeName, edgeData, extraData, extraCost)

                    
    ## @brief Nodes in topologically sorted order, if available, else sorted name order
    def __str__(self):
        nodeStrList = []
        if len(self._toposort) > 0:
            nodeNames = self._toposort
        else:
            nodeNames = list(self._nodeDict)
            nodeNames.sort()
        for nodeName in nodeNames:
            if not nodeName.startswith('_'): 
                nodeStrList.append(str(self._nodeDict[nodeName]))
        return os.linesep.join(nodeStrList)
    

    ## @brief Nodes in topologically sorted order, if available, else sorted name order
    def __repr__(self):
        nodeStrList = []
        if len(self._toposort) > 0:
            nodeNames = self._toposort
        else:
            nodeNames = list(self._nodeDict)
            nodeNames.sort()
        for nodeName in nodeNames:
            nodeStrList.append(repr(self._nodeDict[nodeName]))
        return os.linesep.join(nodeStrList)


## @brief Vanilla graph node
class graphNode(object):
    
    ## @brief Graph node constructor
    #  @param name Name of this node
    #  @param indata Iterable containing input data connections for this node
    #  @param outdata Iterable containing output data connections for this node
    #  @param weight Weights (relative execution cost) for each input connection to this node
    #  @note For @c inData and @c outData a list, tuple or set is acceptable. Multiple input data types
    #  should be expressed as lists or tuples themselves, e.g., <tt>[('HIST_AOD', 'HIST_ESD')]</tt>. They
    #  cannot be sets themselves as python sets cannot contain other sets.
    def __init__(self, name, inData, outData, weight = None):
        self._name = name
        self._inData = set(inData)
        self._outData = set(outData)

        ## @note @c _inWeights takes the form of a dictionary, keyed by input data type
        #  and giving the relative cost of executing this node with those input data types.
        self._inWeights = {}
        if weight is None:
            for data in self._inData:
                self._inWeights[data] = 1
        elif isinstance(weight, int):
            for data in self._inData:
                self._inWeights[data] = weight
        else:
            # Must be a dictionary with its keys equal to the _inData elements
            self._inWeights = weight

        self._inputDataTypes = self._flattenSet(self._inData)
        self._outputDataTypes = self._flattenSet(self._outData)
        
        # Connections dictionary will hold incoming and outgoing edges - the incoming connections
        # are very useful for topological ordering. Nested dictionary with 'in', 'out' keys, where
        # the values are dictionaries with nodeName keys and set(dataTypes) as values.
        # e.g., {'out': {'_end_HIST': set(['HIST'])}, 'in': {'ESDtoAOD': set(['HIST_AOD']), 'RAWtoESD': set(['HIST_ESD'])}}
        self._connections = {'in': {}, 'out': {}}

    @property
    def name(self):
        return self._name
        
    @property
    def inData(self):
        return self._inData

    @property
    def outData(self):
        return self._outData
    
    @property
    def inputDataTypes(self):
        return self._flattenSet(self.inData)
    
    @property
    def outputDataTypes(self):
        return self._flattenSet(self._outData)
    
    @property
    def connections(self):
        return self._connections
    
    @property
    def weights(self):
        return self._inWeights
    
    ## @brief Add a new edge connection for this node
    #  @param @c toExe Other node for this edge
    #  @param @c data Data which connects these nodes (iterable), converted to set object
    #  @param @c direction If this is an incoming or outgoing edge for this node
    def addConnection(self, toExe, data, direction = 'out'):
        self._connections[direction][toExe] = set(data)
    
    ## @brief Delete a connection from this node
    #  @param @c toExe Other node for this vertex
    #  @param @c direction If this is an incoming or outgoing edge for this node    
    def delConnection(self, toExe, direction = 'out'):
        del self._connections[direction][toExe]
        
    ## @brief Delete all connections
    def resetConnections(self):
        self._connections = {'in': {}, 'out': {}}

    ## @brief Take a list and return all simple members plus the members of any list/tuples in the set (i.e.,
    #  flatten out multiple input tuples)
    def _flattenSet(self, startSet):
        flatData = set()
        for data in startSet:
            if isinstance(data, (list, tuple)):
                flatData.update(data)
            else:
                flatData.update([data])
        return flatData
        
    def __str__(self):
        return '{0} (dataIn {1} -> dataOut {2})'.format(self._name, self._inData, self._outData)
    
    def __repr__(self):
        return '{0} (dataIn {1}, weights {2}; dataOut {3}; connect {4})'.format(self._name, self._inData, self._inWeights, self._outData, self._connections)


## @brief Initialise a graph node from an executor
class executorNode(graphNode):
    ## @brief executorNode constructor
    #  @note This is just a wrapper to call the parent class's constructor with the extracted
    #  input and output data types
    #  @param executor Executor used to make this node
    #  @param weight Weight specifier
    def __init__(self, executor = None, weight = None):
        super(executorNode, self).__init__(executor.name, executor.inData, executor.outData, weight)
    
## @brief Path object holding a list of nodes and data types which trace a single path through the graph
class graphPath(object):
    
    ## @brief graphPath constructor
    #  @param endNodeName The node name where this path will end
    #  @param data The data type which this path will make
    #  @param cost The starting cost of this path (default 0)
    def __init__(self, endNodeName, data, cost = 0):
        self._path = [endNodeName]
        self._data = data
        self._cost = cost
        
        ## @note These are dictionaries, keyed by substep, with the data which needs to be produced in that substep, i.e.,
        #  it is the output data for that step and the input data for the downstream step. To avoid data duplication
        #  the values are sets. @c extraData gives data which is not made in this path itself, but is needed in order
        #  to satisfy multi-data entry criteria. @c newData is data which is produced along this path.
        self._newData = dict()
        self._extraData = dict()
        
    @property
    def path(self):
        return self._path
    
    @property
    def cost(self):
        return self._cost
    
    @property
    def newData(self):
        return self._newData
    
    @property
    def extraData(self):
        return self._extraData
    
    def addToPath(self, newNodeName, newData = set(), extraData = set(), extraCost = 0):
        self._path.insert(0, newNodeName)
        self._newData[newNodeName] = newData
        self._cost += extraCost
        self._extraData[newNodeName] = extraData
        
    def addCost(self, cost):
        self._cost += cost
        
    def __str__(self):
        return '{0}: path {1}; cost {2}, newData {3}, extraData {4}'.format(self._data, self._path, self._cost, self._newData, self._extraData)
    
