# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @package DQHanConfMaker.hanwriter
# Module containing the tools to write a DQConfiguration in han format.
# @author: andrea.dotti@pi.infn.it
# @author: ponyisi@hep.uchicago.edu
# 15 August 2008
# @note: The content of this module is inspired by the xml.dom.minidom module
"""
This module contains tools to write a DQConfiguration in han format
"""
from __future__ import print_function

import DQHanConfMaker
from DQConfMakerBase.Helpers import BaseException, toList
from DQConfMakerBase.DQElements import DQBase
from functools import reduce

# Writing exception
# Han exception


class HanWritingError(BaseException):
    """
    This exception is thrown when an error writing the han configuration  is encountered
    """

    def __init__(self, reason):
        BaseException.__init__(
            self, "Cannot create the han file ("+str(reason)+")")

# Generic exception
# Han exception


class HanCannotCreateConf(BaseException):
    """
    This exception is thrown when an error creating the han configuration is encountered
    """

    def __init__(self, reason):
        BaseException.__init__(
            self, "Cannot create configuration ("+str(reason)+")")

# Han writer base class
# Each valid element that can be written in a han file must inherit from this class


class Node(DQHanConfMaker.Node):
    """
    This is the base class for each element that can be written in a valid han configuration file.
    Attributes:
        - subnodes : a list of Node instances
        - attributes : a list of attributes associated with the node
        - acceptChild : a list of Node types that can be accepted as sub nodes
        - nodeType : the type of the node
        - name : the name of the node
    """
    # Default constructor
    # @param name: the name of the element @c name: string

    def __init__(self, name=''):
        """
        Create a han node
        """
        self.subnodes = None
        self.attributes = None
        self.acceptChild = [Node.ALGORITHM, Node.DIR, Node.DOCUMENT, Node.HIST, Node.OUTPUT,
                            Node.REFERENCE, Node.THRESHOLD, Node.LIMIT, Node.COMPALG]
        self.nodeType = Node.UNKNOWN
        self.name = name

    # Creates a valid han block
    # @param encoding: a valid encoding identifier. @see: module codecs
    # @return: the han string
    def tohan(self, encoding=None):
        """
        convert the object in a valid han script block
        """
        return self.toprettyhan("", "", encoding)

    # Creates a valid han block
    # @param indent: used as indentation @c indent: string
    # @param newl: used as new line character @c newl: string
    # @param encoding: a valid encoding identifier @see: module codecs
    # @return: the string representing the han configuration
    def toprettyhan(self, indent="\t", newl="\n", encoding=None):
        """
        convert the object in a formatted han string
        """
        # restore the following in a future tdaq release
        # writer = DQHanConfMaker._get_StringIO()
        import io
        import six
        writer = io.BytesIO() if six.PY2 else io.StringIO()
        if encoding is not None:
            import codecs
            writer = codecs.lookup(encoding)[3](writer)
        self.writehan(writer, "", indent, newl)
        return writer.getvalue()

    # Writes a han block
    # @param writer: the writer helper @c writer: an object supporting the write method
    #    @param indent: used as indentation @c indent: string
    #    @param addindent: used as additional indentation for each sub node @c addindent: string
    #    @param newl: used as newline at the end of the element @c newl: string
    def writehan(self, writer, indent="", addindent="", newl=""):
        """
        Converts the object in a han string and writes it in the writer object
        """
        writer.write(
            indent+DQHanConfMaker._get_NodeType(self.nodeType)+" "+self.name)
        # if it is a document do not need to add { }
        if self.nodeType != Node.DOCUMENT:
            writer.write(" { %s" % (newl))
        if self.attributes:
            for key, attribute in self.attributes.items():
                writer.write("%s %s = %s%s" % (indent, key, attribute, newl))
        if self.subnodes:
            for node in self.subnodes:
                node.writehan(writer, indent+addindent, addindent, newl)
        if self.nodeType != Node.DOCUMENT:
            writer.write("%s}%s" % (indent, newl))

    # Adds a sub node
    # @param child: the node to add @c child: a Node object instance
    # @raise HanCannotCreaqteConf: in case of errors
    def appendChild(self, child):
        """
        Add a sub node to this node
        """
        try:
            if child.nodeType not in self.acceptChild:
                msg = " Node: "+DQHanConfMaker._get_NodeType(self.nodeType)
                msg += " Cannot have a child of type:", child.nodeType
                raise HanCannotCreateConf(msg)
        except Exception:
            raise HanCannotCreateConf(
                "Object:"+str(child)+" is not a valid Node")
        if not self.subnodes:
            self.subnodes = []
        self.subnodes += [child]

    # Adds an attribute to the node
    # @param key: the attribute identifier @c key: a string
    # @param attribute: the value for the attribute @c attribute: a string
    def setAttribute(self, key, attribute):
        """
        The attribute identified by key is added to this node
        """
        if not self.attributes:
            self.attributes = {}
        self.attributes[key] = attribute
    # Removes an attribute
    # @param key: the attribute key @c key: a string
    # @return: True on success

    def removeAttribute(self, key):
        """
        Removes attribute identified by key
        """
        if self.attributes and key in self.attributes:
            del self.attributes[key]
            return True
        return False
    # Gets the attribute identified by key
    # @param key: an attribute identifier @c key: string
    # @return: the corresponding attribute or None if not found

    def getAttribute(self, key):
        """
        Gets the attribute identified by the key, None if not found
        """
        if self.attributes and key in self.attributes:
            return self.attributes[key]
        return None
    # Gets a sub-node by name and type
    # @param name: the sub-node name
    # @param nodetype: the sub-node type
    # @return: the found Node or None if not found

    def getSubNode(self, name, nodetype):
        """
        Returns the sub-node identified by name and nodetype
        """
        if self.subnodes:
            for sn in self.subnodes:
                if sn.nodeType == nodetype and sn.name == name:
                    return sn
        return None

    def __str__(self):
        return "HanNode: "+self.name+" ("+DQHanConfMaker._get_NodeType(self.nodeType)+") Attributes:"+str(self.attributes)+" SubNodes: "+str(self.subnodes)


# HanHistogram class
# This class represents the configuration for a histogram
class HanHistogram(Node):
    """
    A han histogram element is a Node with attributes:
    - the histogram name
    - the algorithm to be used
    - the output where to store the dq result
    """
    # Default constructor
    # @param histogram: the histogram name @c histogram: a string
    # @param algorithm: the algorithm element name @c algorithm: a string
    # @param output: the output key @c output: a string
    # @param annotations: annotations for the DQRegion @c : a dict
    # @param attributes: attributes for the DQRegion @c : a dict
    # @note: the histogram name is the histogram name without the directory structure. Thus a HanHistogram is usually nested in a directory tree (@see: HanDir)

    def __init__(self, histogram, algorithm, annotations={}, output=DQHanConfMaker._default_output, attributes={}):
        """
        Creates a han histogram configuration element
        """
        Node.__init__(self, histogram)
        self.acceptChild = []
        self.nodeType = Node.HIST
        self.setAttribute('algorithm', algorithm)
        self.setAttribute('output', output)
        if 'Weight' in attributes:
            self.setAttribute('weight', attributes['Weight'][1])
        for annotation in annotations:
            self.setAttribute(annotation, annotations[annotation])

    # Gets the output attribute
    # @return: the output attribute string
    def getOutput(self):
        """
        Retrieves the output attribute
        """
        return self.getAttribute('output')

    # Gets the algorithm attribute
    # @return: the algorithm attribute string
    def getAlgorithm(self):
        """
        Retrieves the algorithm attribute
        """
        return self.getAttribute('algorithm')

# Class algorithm
# This class implements the han representation of a DQAlgorithm element


class HanAlgorithm(Node):
    """
    The han representation of a DQAlgorithm
    """
    # Default constructor
    # @param name: the identifier for the han algorithm @c name: a string
    # @param algoname: the dqm_algorithm name @c algoname: a string
    # @param libname: the library containing the algorithm @c libname: a string
    # @param reference: the reference object, can be None
    # @param thresholds: the thresholds objects, can be None
    # @param parameters: additional parameters, can be empty. @c parameters: a list

    def __init__(self, name, algoname, libname='libdqm_algorithms.so', reference=None, thresholds=None,
                 parameters={}):
        """
        Creates a han algorithm configuration element
        """
        Node.__init__(self, name)
        self.acceptChild = [Node.THRESHOLD, Node.REFERENCE]
        self.nodeType = Node.ALGORITHM
        self.setAttribute('name', algoname)
        if libname:
            self.setAttribute('libname', libname)
        if reference:
            self.setAttribute('reference', reference)
        if thresholds:
            self.setAttribute('thresholds', thresholds)
        for parameter in parameters:
            self.setAttribute(parameter, parameters[parameter])

# Class for composite algorithm
# This class implements the han representation of a DQCompositeAlgorithm


class HanCompositeAlgorithm(Node):
    """
    The han representation of a CompositeAlgorithm
    """
    # Default constructor
    # @param name: the identifier for the han algorithm @c name: a string
    # @param algoname: the dqm_algorithm name @c algoname: a string
    # @param libname: the library containing the algorithm @c libname: a string

    def __init__(self, name, subalgnames=[], libnames=[]):
        """
        Creates a han composite algorithm configuration element
        """
        Node.__init__(self, name)
        self.acceptChild = []
        self.nodeType = Node.COMPALG
        self.setAttribute('subalgs', ','.join(subalgnames))
        self.setAttribute('libnames', ','.join(libnames))

# Class for reference
# This class implements the han representation of a DQReference


class HanReference(Node):
    """
    The han representation of a DQReference
    """
    # Default constructor
    # @param name: the identifier for the han reference @c name: a string
    # @param histogramname: the histogram name @c histogramname: a string
    # @param file: the filename for the reference histogram @c file: a string

    def __init__(self, name, histogramname, file, info=None):
        """
        Creates a han reference configuration element
        """
        Node.__init__(self, name)
        self.setAttribute('name', histogramname)
        self.setAttribute('file', file)
        if info is not None:
            self.setAttribute('info', info)
        self.nodeType = Node.REFERENCE
        self.acceptChild = []

# Class for limit
# This class implements the han representation of a DQLimit


class HanLimit(Node):
    """
    The han representation of a limit for a HanThreshold
    """
    # Default constructor
    # @param name: the name of the limit @c name: a string
    # @param warning: the value for the warning level
    # @param error: the value for the error level

    def __init__(self, name, warning, error):
        """
        Creates a han limit object
        """
        Node.__init__(self, name)
        self.nodeType = Node.LIMIT
        self.acceptChild = []
        self.setAttribute('warning', warning)
        self.setAttribute('error', error)

# Class for threshold
# This class implements the han representation of a DQThreshold


class HanThreshold(Node):
    """
    The han representation of a DQThreshold
    """
    # Default constructor
    # @param name: the threshold identifier @c name: a string

    def __init__(self, name):
        """
        Creates a han threshold configuration element
        """
        Node.__init__(self, name)
        self.acceptChild = [Node.LIMIT]
        self.nodeType = Node.THRESHOLD

    def addLimit(self, name, warning, error):
        """
        Adds a limit to the threshold
        @param name: the limits name
        @param warning: the limit value for the warning level
        @param error: the limit value for the error level
        """
        if self.getSubNode(name, Node.LIMIT):
            raise HanCannotCreateConf(
                "The limit: "+name+" already exists for threshold: "+self.name)
        limit = HanLimit(name, warning, error)
        self.appendChild(limit)

# Class for han directory
# This class implements the han directory


class HanDir(Node):
    """
    Class representing a dir element
    A han dir element is a Node with sub nodes of type HanDir or HanHisto
    """
    # Default constructor
    # @param namne: the name of the directory @c name: a string

    def __init__(self, name):
        """
        Creates a han directory element
        """
        Node.__init__(self, name)
        self.acceptChild = [Node.DIR, Node.HIST]
        self.nodeType = Node.DIR

    # Adds a sub directory
    # @param name: the name of the subdirectory @c name: a string
    # @return: the created HanDir object
    def addSubDir(self, name):
        """
        Adds a subdirectory called name to the current directory and returns the newly created sub directory
        """
        subdir = HanDir(name)
        self.appendChild(subdir)
        return subdir
    # Adds a histogram to the current directory
    # @param histogram: the name of the histogram @c histogram: a string
    # @param algorithm: the algorithm name to be used for this check @c algorithm: a string
    # @param annotations: annotations for the DQRegion @c : a dict
    # @param attributes: attributes for the DQRegion @c : a dict
    # @param output: the output path for the result @c output: a string
    # @return: the created HanHistogram object

    def addHistogram(self, histogram, algorithm, annotations={}, output=DQHanConfMaker._default_output, attributes={}):
        """
        Adds to the directory a histogram specifying the histogram name, the algorithm and the output path.
        """
        histo = HanHistogram(histogram, algorithm,
                             annotations, output, attributes)
        self.appendChild(histo)
        return histo
    # Gets a sub-directory name
    # @param name: the subdirectory name @c name: a string

    def getSubDir(self, name):
        """
        returns the sub-directory called name
        """
        return self.getSubNode(name, Node.DIR)

# Class for han output
# This class implements the han output


class HanOutput(Node):
    """
    Class representing a output element
    A han output element is a Node with sub nodes of type output and with the optional attribute algorithm
    attribute:
        - father: the HanOutput that contains this HanOutput
    """
    # Default constructor
    # @param name: the output name @c name: a string
    # @param algorithm: the optional algorithm name for this summary @c algorithm: summary
    # @param father: the HanOutput containing this node
    # @param annotations: annotations for the DQRegion @c : a dict
    # @param attributes: attributes for the DQRegion @c : a dict

    def __init__(self, name, algorithm=None, father=None, annotations={}, attributes={}):
        """
        Creates an output han element
        """
        Node.__init__(self, name)
        self.acceptChild = [Node.OUTPUT]
        self.nodeType = Node.OUTPUT
        if algorithm:
            self.setAttribute('algorithm', algorithm)
        self.father = father
        if 'Weight' in attributes:
            self.setAttribute('weight', attributes['Weight'][1])
        for annotation in annotations:
            self.setAttribute(annotation, annotations[annotation])

    # Gets the algorithm
    # @return: the associated algorithm name
    def getAlgorithm(self):
        """
        Gets the algorithm attribute
        """
        if 'algorithm' in self.attributes:
            return self.getAttribute('algorithm')
    # Adds a sub-output
    # @param name: the sub output name @c name: a string
    # @param algorithm: the algorithm for the sub output @c : a string
    # @param annotations: annotations for the DQRegion @c : a dict
    # @param attributes: attributes for the DQRegion @c : a dict
    # @return: the created HanOutput object

    def addOutput(self, name, algorithm, annotations, attributes):
        """
        Adds a sub output element with name and algorithm
        """
        subnode = HanOutput(name, algorithm, self, annotations, attributes)
        self.appendChild(subnode)
        return subnode
    # Creates the complete output string
    # @param append: string to be appended to the name @c append: a string
    # @param delimter: the delimiting characters for the string concatenation @c delimiter: a string
    # @return: the complete path output string

    def getOutputPath(self, append=None, delimiter="/"):
        """
        Creates the complete path of this output directory
        """
        name = self.name
        if append:
            name = name + append
        if self.father:
            name = self.father.getOutputPath(delimiter+name, delimiter)
        return name

# Class document
# This class represents the han document.
# This class is a set of nodes


class Document(HanDir, HanOutput):
    """
    Class representing a han document
    """
    # Default constructor
    # @param top_level_algorithm: the algorithm name for the top level output

    def __init__(self, top_level_algorithm='WorstCaseSummary'):
        """
        Creates an empty han document with a top level output
        """
        HanDir.__init__(self, '')
        HanOutput.__init__(self, '')
        self.nodeType = Node.DOCUMENT
        self.acceptChild = [Node.ALGORITHM, Node.DIR, Node.HIST, Node.OUTPUT, Node.REFERENCE, Node.THRESHOLD,
                            Node.COMPALG]
        self.root_output_level = HanOutput('top_level', top_level_algorithm)
        self.appendChild(self.root_output_level)
    # Adds an output subdirectory
    # @param name: the sub output name @c name: a string
    # @param algorithm: the algorithm for the sub output @c algorithm : a string
    # @param annotations: annotations for the DQRegion @c annotations : a dict
    # @param attributes: attributes for the DQRegion @c attributes : a dict
    # @return: the created HanOutput object

    def addOutput(self, name, algorithm, annotations={}, attributes={}):
        """
        Adds a sub output element with name and algorithm
        """
        # forward to root region
        return HanOutput.addOutput(self.root_output_level, name, algorithm, annotations, attributes)
    # Gets the algorithm object
    # @param name: the name of the algorithm
    # @return: the algorithm associated to name

    def getAlgorithm(self, name):
        """
        retrieves the algorithm han object associated with name
        """
        return self.getSubNode(name, Node.ALGORITHM)
    # Gets the reference object
    # @param name: the reference name
    # @return: the HanReference object

    def getReference(self, name):
        """
        retrieves the han reference associated with name
        """
        return self.getSubNode(name, Node.REFERENCE)


# Add a DQParameter object to the HanDocument
# @param handocument: the document containing the configuration @c handocument: a HanDocument object
# @param dqparameter: the DQParameter object @c dqparameter: a DQParameter object
# @param output: the output element for the result
# @note: This function is used internally and should not be used by users of this module
def _hanAddDQParameter(handocument, dqparameter, output=DQHanConfMaker._default_output):
    """
    Add a DQParameter object to a HanDocument, Doing this it creates the tree of 
    dir and output.
    """
    try:
        if dqparameter.type != "DQParameter":  # This is not valid
            raise HanCannotCreateConf(
                "Object: "+str(dqparameter)+" type does not match 'DQParameter'")
        # gets the directory structure of the histogram and add it, if needed to the document
        histoname = dqparameter.getInput().split('/')
        theDir = handocument
        # add directories to han document if needed
        for directory in histoname[0:len(histoname)-1]:
            subdir = theDir.getSubDir(directory)
            if subdir is None:
                subdir = theDir.addSubDir(directory)
            theDir = subdir
        # prepare to build the han algorithm element
        algo = dqparameter.getAlgorithm()
        if algo is None:
            raise Exception('DQParameter without DQAlgorithm')
        dqalgoname, dqalgolibrary = algo.id, algo.getLibraryName()
        # Build the han reference, if any, associated to this DQParameter
        dqrefname, dqreference, refs = "", dqparameter.getReference(), []
        if dqreference != [None]:  # References are defined
            refs = '[%s]' % (','.join(_.id for _ in dqreference))
            for iref in dqreference:
                ref = iref.id
                # search if reference is already in the han document
                if handocument.getReference(ref) is None:
                    # it's not found, let's create it
                    refcompletename = iref.getReference()
                    filename = refcompletename.rpartition(':')[0]
                    refhistoname = refcompletename.rpartition(':')[
                        2].lstrip('/')
                    hanref = HanReference(
                        ref, histogramname=refhistoname, file=filename, 
                        info=iref.getAnnotation('info'))
                    handocument.appendChild(hanref)
                    dqrefname = '_'+ref
        # build the han thresholds
        # here it's quite tricky: we have to get two sets of DQThresholds and combine them in a single
        # han thresholds element
        warnings = toList(dqparameter.getWarnings())
        errors = toList(dqparameter.getErrors())
        thresholds, hanthreshold, thresholdsname = '', None, ''
        if (warnings != [None]) and (errors != [None]):
            # Found errors and warnings limits
            thresholds = dqparameter.id.replace(
                '_', '__').replace('/', '_')+'_thr'
            hanthreshold = HanThreshold(thresholds)
            thresholdsname = thresholds
            thresholds = '_'+thresholds

            def getCorrespondingRed(warningname):
                for e in errors:
                    if e.getAttribute('Name')[1] == warningname:
                        return e
                return None
            for warning in warnings:
                warningname = warning.getAttribute('Name')[1]
                error = getCorrespondingRed(warningname)
                if error is None:
                    raise HanCannotCreateConf(
                        'Cannot find corresponding error for warning names: '+warning.id)
                hanthreshold.addLimit(warningname, warning.getAttribute(
                    'Value')[1], error.getAttribute('Value')[1])
            #print DQHanConfMaker._get_NodeType(hanthreshold.nodeType),hanthreshold.name
            handocument.appendChild(hanthreshold)

        # build the parameter id string and key, value pairs
        parameterstr = '_'.join([x.id for x in toList(
            dqparameter.getAlgorithmParameters()) if x is not None])
        parameters = {}
        for x in toList(dqparameter.getAlgorithmParameters()):
            if x is None:
                continue
            parameters[x.getName()[1]] = x.getValue()[1][0]
        # create a unique string identifying this han algorithm element so far
        hanalgoname = ("%s_%s%s%s%s" % (dqalgolibrary, dqalgoname,
                                        dqrefname, thresholds.replace('/', '_'), parameterstr))
        # Finally add the histogram han element
        theDir.addHistogram(histogram=histoname[len(histoname)-1], algorithm=hanalgoname,
                            annotations=dqparameter.getAllAnnotations(), output=output, attributes=dqparameter.getAllAttributes())
        # search the han algorithm in the document
        if handocument.getAlgorithm(hanalgoname) is None:
            # This algorithm has never been added before in the document
            node = HanAlgorithm(name=hanalgoname, algoname=dqalgoname, libname=dqalgolibrary,
                                reference=refs, thresholds=thresholdsname, parameters=parameters)
            handocument.appendChild(node)
            # check for possible composite algorithms
            compalg = algo.getCompositeAlgorithm()
            if compalg is not None:
                node = HanCompositeAlgorithm(name=compalg.id, subalgnames=compalg.getSubAlgNames(),
                                             libnames=compalg.getLibNames())
                handocument.appendChild(node)
    except Exception as msg:
        raise HanCannotCreateConf(
            "Object: "+str(dqparameter)+" is not a valid DQParameter. Reason: "+str(msg))

# Finds the root han objects of the input collection
# @param input: the list of objects @c input: a list of DQBase objects
# @raise HanCannotCreateConf: in case of errors
# @return: the list of roots
# @note: This function is used internally and should not be used by users of this module


def _findRoots(input=[], objecttype="DQRegion"):
    """
    This helper return the list of objects that are the roots in the tree represented by input.
    """
    result = []
    for o in input:
        try:
            if o.type == objecttype:
                if len(o.isReferencedBy) == 0:
                    result += [o]
        except Exception:
            raise HanCannotCreateConf(
                "object %s is not a valid DQBase object" % o)
    return result

# Gets all DQBase objects that corresponds to the input han objects
# @param rootlist: a list of han object trees
# @return: the list of all DQBaseObjects associated to each node of the input tree
# @note: This function is used internally and should not be used by users of this module


def _findAllDQBaseObjects(rootlist):
    """
    This returns all DQBase objects reachable from the rootlist as a set.
    """
    import operator

    def recurse(dqbase):
        if dqbase is None:
            return set()
        if isinstance(dqbase, list):
            return reduce(operator.ior, [recurse(x) for x in dqbase])
        if not isinstance(dqbase, DQBase):
            raise ValueError(
                '%s is not a valid DQBase object; this should never happen' % dqbase)
        retset = {dqbase}
        for rel in dqbase.getAllRelatedObjects():
            retset |= recurse(rel)
        return retset

    topset = set()
    for dqbase in toList(rootlist):
        topset |= recurse(dqbase)
    return list(topset)

# Writes the han configuration to a file
# @param roots: the list of all roots of han trees


def writeHanConfiguration(filename='dq.han.config', roots=[]):
    """
    Writes the configuration to a han file. 
    The configuration has to be passed as a list of roots in han configuration.
    """
    configList = _findAllDQBaseObjects(roots)
    # First remove the configuration elements that cannot be saved as han
    for element in configList:
        try:
            if not DQHanConfMaker._isAceptedHanElement(element):
                print("==========WARNING===========")
                print("Object: " + \
                    str(element) + \
                    " does not have a valid han representation, cannot save")
                print("The object is referenced by:", element.isReferencedBy)
                print("You may need to manually edit the configuration")
                print("============================")
                configList.remove(element)
        except Exception:
            print(element.isReferencedBy)
            raise HanCannotCreateConf("Not valid object:" + str(element))
    # Let's create the han document
    doc = Document()
    # Find all root regions
    rootregions = _findRoots(configList)

    # Navigate through DQRegions tree and add it to the han document
    def _addchild(hanfather, dqregion, handocument):
        outputalgo = dqregion.getAlgorithm()
        if handocument.getAlgorithm(outputalgo.id) is None:
            # Add to the document the summary maker algorithm
            # summary makers do not have thresholds or parameters
            # @todo: in the future extend this to make in enough general to support parameter
            handocument.appendChild(HanAlgorithm(
                outputalgo.id, outputalgo.id, outputalgo.getLibraryName()))  # , reference, thresholds)
        output = hanfather.addOutput(dqregion.id, outputalgo.id, annotations=dqregion.getAllAnnotations(
        ), attributes=dqregion.getAllAttributes())
        # recursively add subregions
        for subregion in dqregion.getSubRegions():
            if subregion is not None:
                _addchild(output, subregion, doc)
        # Add each DQParameter to the han document with the helper
        for parameter in dqregion.getDQParameters():
            if parameter is not None:
                _hanAddDQParameter(handocument, parameter,
                                   output=output.getOutputPath())

    for region in rootregions:
        _addchild(doc, region, doc)

    fileout = open(filename, 'w')
    print(doc.toprettyhan(" "), file=fileout)
    