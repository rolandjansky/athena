# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

"""core module for an interactive analysis

Examples are in `PyAnalysisExamples/PlotTest.py`_ and `PyAnalysisExamples/HistTest.py`_

.. _PyAnalysisExamples/HistTest.py: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/PhysicsAnalysis/PyAnalysis/PyAnalysisExamples/share/HistTest.py?rev=HEAD&content-type=text/vnd.viewcvs-markup

.. _PyAnalysisExamples/PlotTest.py: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/PhysicsAnalysis/PyAnalysis/PyAnalysisExamples/share/PlotTest.py?rev=HEAD&content-type=text/vnd.viewcvs-markup
    
:author: Tadashi Maeno
:contact: tmaeno@bnl.gov

"""
__docformat__ = "restructuredtext en"

# remove these lines when epydoc
#"""
import re
import types
import cppyy
from math import *
from AthenaCommon.SystemOfUnits import *

# global name space
GNS = cppyy.gbl

#"""

# Event Loop Types
_PreProcess    = 0
_NormalProcess = 1
_HybridProcess = 2

# dummy class for TAG and AANT
class _DummyClass: pass
setattr (GNS, 'AttributeList', _DummyClass)
setattr (GNS, 'AANT',          _DummyClass)
del _DummyClass

storeGate = None
detStore = None


# initialize core
def init (v_theApp, v_rootStream=None):
    """Initialize core

    This method is called in `PyKernel/InitPyKernel.py`_.
    
    :param v_theApp: reference to the application manager. theApp

    **examples**::

      athena> PyKernel.init(theApp)

    .. _PyKernel/InitPyKernel.py: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/Control/PyKernel/share/InitPyKernel.py?rev=HEAD&content-type=text/vnd.viewcvs-markup  
      
    """
    # application manager
    global theApp
    theApp = v_theApp
    # root stream
    global rootStream
    rootStream = v_rootStream
    # event loop type
    global eventLoopType
    eventLoopType = _HybridProcess
    # event counter
    global curEvent
    curEvent = 0
    if hasattr (theApp,  'curEvent'):
        # patch some methods
        method = "nextEvent"
        mobj = _SetEventCounter(method)
        setattr(theApp.__class__,method,mobj)
        method = "run"
        mobj = _SetEventCounter(method)
        setattr(theApp.__class__,method,mobj)


# a function object to set event counter in ROOT stream
class _SetEventCounter:
    """Function object to set event counter in ROOT stream.
    This class is used to patch, e.g., theApp.nextEvent(), run() ...

    :param methodObj: reference to method to be patched

    """
    def __init__(self,methodName):
        self.methodObj   = getattr(theApp,methodName)
    # method emulation    
    def __call__ (self,*var):
        global curEvent
        # set event counter. For pre-process internal curEvent is used
        if eventLoopType != _PreProcess:
            curEvent = theApp.curEvent()
        # get ROOT entry    
        if eventLoopType != _NormalProcess and rootStream:
            rootStream.GetEntry(curEvent)
        # return if pre-process
        if eventLoopType == _PreProcess:
            # increment event counter
            curEvent = curEvent+1
            return GNS.StatusCode(1)
        # call original method
        return apply(self.methodObj,var)
    

# retrieve object from StoreGate
def retrieve (aClass, aKey=None):
    """Retrieve object from StoreGate
    
    :param aClass: type of the class
    :param aKey: key of the object

    **examples**::

      athena> obj = PyKernel.retrieve(g.MyClass,'mykey')
      athena> obj = PyKernel.retrieve(g.MyClass)  # when only one MyClass obj is in SG
          where the prefix 'g' is the global namespace provided by cppyy
          g = cppyy.gbl

    """
    #import workaround
    if aClass == GNS.AttributeList:
        return rootStream        
    if aClass == GNS.AANT:
        return rootStream
    global storeGate
    if storeGate == None:
        import StoreGateBindings.Bindings
        storeGate = StoreGateBindings.Bindings.StoreGate.pointer()
    if aKey:
        ret = storeGate.retrieve(aClass,aKey)
    else:
        ret = storeGate.retrieve(aClass)
    return ret


# retrieve object from DetectorStore
def retrieveDet (aClass, aKey=None):
    """Retrieve object from DetectorStore
    
    :param aClass: type of the class
    :param aKey: key of the object

    **examples**::
    
      athena> obj = PyKernel.retrieveDet(g.MyClass,'mykey')
      athena> obj = PyKernel.retrieveDet(g.MyClass) # when only one MyClass obj is in SG
          where the prefix 'g' is the global namespace provided by cppyy
          g = cppyy.gbl
          
    """
    #import workaround    
    global detStore
    if detStore == None:
        import StoreGateBindings.Bindings
        detStore = StoreGateBindings.Bindings.StoreGate.pointer("DetectorStore")
    if aKey:
        ret = detStore.retrieve(aClass,aKey)
    else:
        ret = detStore.retrieve(aClass)        
    return ret


# fill a histogram
def fill (hist, classAndKey, value, criteria="True", nEvent=100):
    '''
    Fill 1D-histogram
    
    :param hist: reference to AIDA or ROOT histogram
    :param classAndKey: combination of class name and key separeted with "#". "Class#Key"
    :param value: physics parameter in string
    :param criteria: selection criteria
    :param nEvent: number of event to be processed

    **examples**::

      athena> fill(h,"ElectronContainer#ElectronCollection","$x.pt()")
          fill hist with pt of electrons
         "$x" denotes an element of "Class#Key", if "Class#Key" is a collection
       
      athena> fill(h,"MissingET#MET_Calib","$x.sumet()")
          fill hist with et of MissingET.
          "$x" denotes "Class#Key" itself, if "Class#Key" is not a vector-like class
          
      athena> fill(h,"ElectronContainer#ElectronCollection","$x.pt()","$x.pz()>0")
          apply a selection criteria
          
    For more detail of parameters, see `PyAnalysisExamples/PlotTest.py`_

    .. _PyAnalysisExamples/PlotTest.py: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/PhysicsAnalysis/PyAnalysis/PyAnalysisExamples/share/PlotTest.py?rev=HEAD&content-type=text/vnd.viewcvs-markup
    
    '''

    # number of buffered events
    bufEvent = nEvent
    if nEvent > 100:
        bufEvent = 100

    # convert class&key to a store gate access
    commandSG = "None"
    if classAndKey != "":
        commandSG = _parseString(classAndKey)

    # build commands
    if callable(value):
        # if function pointer
        commandV = "value()"
    else:
        # if string, parse value/criteria     
        commandV = _parseString(value)
    if callable(criteria):
        # if function pointer        
        commandC = "criteria()"
    else:
        # if string, parse value/criteria             
        commandC = _parseString(criteria)

    # initialize application mgr
    theApp.initialize()

    # buffer to determine x-range of histgram
    buf = []

    # loop over nEvent
    for iE in range(nEvent):
        # get object from SG
        theApp.nextEvent()
        try:
            obj = eval(commandSG)

            # check if the obj is a vector-like class
            if hasattr(obj,'size') and hasattr(obj,'__getitem__'):
                lSize = obj.size()
                isCollection = True
            else:
                lSize = 1
                isCollection = False
            # if NULL
            if obj == 0:
                lSize = 0                
        except:
            lSize = 0

        # loop over all elements
        for iC in range(lSize):
            # parameter name "x" must be consistent with the parsed strings
            if isCollection:
                x = obj[iC]
            else:
                x = obj

            # eval value/criteria commands
            try:
                vX = eval(commandV)
                vC = eval(commandC)

                # evaluate vC/vX. "vC" and "vX" must be consistent with commands
                if vC:
                    if iE < bufEvent:
                        buf.append(vX)
                    else:
                        h.Fill(vX)
            except:
                pass

            # if not a collection escape from loop
            if not isCollection:
                break
            
        # create Histogram
        if (iE+1) == bufEvent:
            if len(buf)==0:
                minX=0
            else:    
                minX = min(buf)
                
            if minX<0:
                minX *= 1.1
            elif minX>0:
                minX *= 0.9
            else:
                minX = -1

            if len(buf)==0:
                maxX=0
            else:    
                maxX = max(buf)
                
            if maxX<0:
                maxX *= 0.9
            elif maxX>0:
                maxX *= 1.1
            else:
                maxX = 1

            # create histogram if hist is None
            if hist == None:
                lpath = '/stat/tmp/PyKernelHist'
                unregister(lpath)
                # determine title of histo
                if callable(value):
                    title = value.__name__
                else:
                    title = value
                h = book(lpath, title, 100, minX, maxX)
            else:
                h = hist

    # buffered elements
    for vB in buf:
        h.Fill(vB)

    return h


# plot a histogram
def plot (classAndKey, value="$x", criteria="True", nEvent=100):
    """Plot 1D-histogram
    
    :param classAndKey: combination of class name and key separeted with '#'. 'Class#Key'
    :param value: physics parameter in string
    :param criteria: selection criteria
    :param nEvent: number of event to be processed

    **examples**::

      athena> plot('ElectronContainer#ElectronCollection','$x.pt()')
          plot pt of electrons
          
    For detail, see `PyAnalysisExamples/PlotTest.py`_

    .. _PyAnalysisExamples/PlotTest.py: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/PhysicsAnalysis/PyAnalysis/PyAnalysisExamples/share/PlotTest.py?rev=HEAD&content-type=text/vnd.viewcvs-markup
    
    """
  
    # fill a histogram
    h = fill (None, classAndKey, value, criteria, nEvent)
    # draw 
    h.Draw()
    # this return is needed to draw up the canvas.
    # note that PyKernel._hSave = h doesn't work although it keeps the hist in memory  
    return h


# fill 2D histogram
def fill2 (hist, classAndKey, valueX, valueY, criteria="True", nEvent=100):
    """Fill 2D-histogram
    
    :param hist: reference to AIDA or ROOT histogram
    :param classAndKey: combination of class name and key separeted with '#', 'Class#Key'
    :param valueX: physics parameter for X in string
    :param valueY: physics parameter for Y in string
    :param criteria: selection criteria
    :param nEvent: number of event to be processed

    For detail, see `fill`
    
    """

    # number of buffered events
    bufEvent = nEvent
    if nEvent > 100:
        bufEvent = 100

    # convert class&key to a store gate access
    commandSG = "None"
    if classAndKey != "":
        commandSG = _parseString(classAndKey)
    
    # build commands
    if callable(valueX):
        # if function pointer
        commandX = "valueX()"
    else:
        # if string, parse value/criteria     
        commandX = _parseString(valueX)
    if callable(valueY):
        # if function pointer
        commandY = "valueY()"
    else:
        # if string, parse value/criteria     
        commandY = _parseString(valueY)
    if callable(criteria):
        # if function pointer        
        commandC = "criteria()"
    else:
        # if string, parse value/criteria             
        commandC = _parseString(criteria)

    # initialize application mgr
    theApp.initialize()

    # buffer to determine xy-range of histgram
    bufX = []
    bufY = []    
    
    # loop over nEvent
    for iE in range(nEvent):
        # get object from SG
        theApp.nextEvent()        
        try:
            obj = eval(commandSG)

            # check if the obj is a vector-like class
            if hasattr(obj,'size') and hasattr(obj,'__getitem__'):
                lSize = obj.size()
                isCollection = True
            else:
                lSize = 1
                isCollection = False
            # if NULL
            if obj == 0:
                lSize = 0                
        except:
            lSize = 0            
        
        # loop over all elements
        for iC in range(lSize):
            # parameter name "x" must be consistent with the parsed strings
            if isCollection:
                x = obj[iC]
            else:
                x = obj

            # eval value/criteria commands
            try:
                vX = eval(commandX)
                vY = eval(commandY)
                vC = eval(commandC)

                # evaluate vC/vX. "vC" and "vX" must be consistent with commands
                if vC:
                    if iE < bufEvent:
                        bufX.append(vX)
                        bufY.append(vY)                        
                    else:
                        h.Fill(vX,vY)
            except:
                pass
                            
            # if not a collection escape from loop
            if not isCollection:
                break

        # create Histogram
        if (iE+1) == bufEvent:
            if len(bufX)==0:
                minX=0
            else:    
                minX = min(bufX)
                
            if minX<0:
                minX *= 1.1
            elif minX>0:
                minX *= 0.9
            else:
                minX = -1
                
            if len(bufX)==0:
                maxX=0
            else:    
                maxX = max(bufX)
                
            if maxX<0:
                maxX *= 0.9
            elif maxX>0:
                maxX *= 1.1
            else:
                maxX = 1

            if len(bufY)==0:
                minY=0
            else:    
                minY = min(bufY)
                
            if minY<0:
                minY *= 1.1
            elif minY>0:
                minY *= 0.9
            else:
                minY = -1

            if len(bufY)==0:
                maxY=0
            else:    
                maxY = max(bufY)
                
            if maxY<0:
                maxY *= 0.9
            elif maxY>0:
                maxY *= 1.1
            else:
                maxY = 1

            # create histogram if hist is None
            if hist == None:
                lpath = '/stat/tmp/PyKernelHist'
                unregister(lpath)
                # determine title of histo
                if callable(valueX):
                    titleX = valueX.__name__
                else:
                    titleX = valueX                    
                if callable(valueY):
                    titleY = valueY.__name__
                else:
                    titleY = valueY
                h = book(lpath, titleY+" vs "+titleX, 100, minX, maxX, 100, minY, maxY)
            else:
                h = hist

    # buffered elements
    for iB in range(len(bufX)):
        vX = bufX[iB]
        vY = bufY[iB]
        h.Fill(vX,vY)
        
    return h    


# plot 2D histogram
def plot2 (classAndKey, valueX="$x", valueY="$x", criteria="True", nEvent=100):
    """Plot 2D-histogram
    
    :param classAndKey: combination of class name and key separeted with '#', 'Class#Key'
    :param valueX: physics parameter for X in string
    :param valueY: physics parameter for Y in string
    :param criteria: selection criteria
    :param nEvent: number of event to be processed

    For detail, see `plot`
    
    """
    h = fill2 (None, classAndKey, valueX, valueY, criteria, nEvent)
    # draw 
    h.Draw('BOX')
    # this return is needed to draw up the canvas.
    # note that PyKernel._hSave = h doesn't work although it keeps the hist in memory  
    return h

    
# fill profile histogram
def fillProf (hist, classAndKey, valueX, valueY, criteria="True", nEvent=100):
    """Fill profile-histogram
    
    :param hist: reference to AIDA or ROOT histogram
    :param classAndKey: combination of class name and key separeted with '#', 'Class#Key'
    :param valueX: physics parameter for X in string
    :param valueY: physics parameter for Y in string
    :param criteria: selection criteria
    :param nEvent: number of event to be processed

    For detail, see `fill`
    
    """

    # number of buffered events
    bufEvent = nEvent
    if nEvent > 100:
        bufEvent = 100

    # convert class&key to a store gate access
    commandSG = "None"
    if classAndKey != "":
        commandSG = _parseString(classAndKey)
    
    # build commands
    if callable(valueX):
        # if function pointer
        commandX = "valueX()"
    else:
        # if string, parse value/criteria     
        commandX = _parseString(valueX)
    if callable(valueY):
        # if function pointer
        commandY = "valueY()"
    else:
        # if string, parse value/criteria     
        commandY = _parseString(valueY)
    if callable(criteria):
        # if function pointer        
        commandC = "criteria()"
    else:
        # if string, parse value/criteria             
        commandC = _parseString(criteria)

    # initialize application mgr
    theApp.initialize()

    # buffer to determine xy-range of histgram
    bufX = []
    bufY = []    
    
    # loop over nEvent
    for iE in range(nEvent):
        # get object from SG
        theApp.nextEvent()        
        try:
            obj = eval(commandSG)

            # check if the obj is a vector-like class
            if hasattr(obj,'size') and hasattr(obj,'__getitem__'):
                lSize = obj.size()
                isCollection = True
            else:
                lSize = 1
                isCollection = False
            # if NULL
            if obj == 0:
                lSize = 0                
        except:
            lSize = 0            
        
        # loop over all elements
        for iC in range(lSize):
            # parameter name "x" must be consistent with the parsed strings
            if isCollection:
                x = obj[iC]
            else:
                x = obj

            # eval value/criteria commands
            try:
                vX = eval(commandX)
                vY = eval(commandY)
                vC = eval(commandC)

                # evaluate vC/vX. "vC" and "vX" must be consistent with commands
                if vC:
                    if iE < bufEvent:
                        bufX.append(vX)
                        bufY.append(vY)                        
                    else:
                        h.Fill(vX,vY)
            except:
                pass
                            
            # if not a collection escape from loop
            if not isCollection:
                break

        # create Histogram
        if (iE+1) == bufEvent:
            if len(bufX)==0:
                minX=0
            else:    
                minX = min(bufX)
                
            if minX<0:
                minX *= 1.1
            elif minX>0:
                minX *= 0.9
            else:
                minX = -1
                
            if len(bufX)==0:
                maxX=0
            else:    
                maxX = max(bufX)
                
            if maxX<0:
                maxX *= 0.9
            elif maxX>0:
                maxX *= 1.1
            else:
                maxX = 1

            # create histogram if hist is None
            if hist == None:
                lpath = '/stat/tmp/PyKernelHist'
                unregister(lpath)
                # determine title of histo
                if callable(valueX):
                    titleX = valueX.__name__
                else:
                    titleX = valueX                    
                if callable(valueY):
                    titleY = valueY.__name__
                else:
                    titleY = valueY                    
                h = bookProf(lpath, titleY+" vs "+titleX, 100, minX, maxX)
            else:
                h = hist

    # buffered elements
    for iB in range(len(bufX)):
        vX = bufX[iB]
        vY = bufY[iB]
        h.Fill(vX,vY)
        
    return h    


# plot profileD histogram
def plotProf (classAndKey, valueX="$x", valueY="$x", criteria="True", nEvent=100):
    """Plot profile-histogram
    
    :param classAndKey: combination of class name and key separeted with '#', 'Class#Key'
    :param valueX: physics parameter for X in string
    :param valueY: physics parameter for Y in string
    :param criteria: selection criteria
    :param nEvent: number of event to be processed

    For detail, see `plot`
    
    """
    h = fillProf (None, classAndKey, valueX, valueY, criteria, nEvent)
    # draw 
    h.Draw()
    # this return is needed to draw up the canvas.
    # note that PyKernel._hSave = h doesn't work although it keeps the hist in memory  
    return h

    
# parse string
def _parseString (str):
    # remove $
    str = re.sub("\$", "", str)
    # replace XXX#YYY with StoreGate access
    cK = re.findall(r'([\w_]+)#([\w_\*]+)',str)
    for iCK in cK:
        # when XXX#*
        if iCK[1]=='*':
            bStr = iCK[0]+"#\*"
            aStr = 'retrieve(GNS.'+iCK[0]+')'
        else:
            bStr = iCK[0]+"#"+iCK[1]
            aStr = 'retrieve(GNS.'+iCK[0]+',"'+iCK[1]+'")'
        str = re.sub(bStr, aStr, str)
    return str    


# dump objects in StoreGate
def dumpSG ():
    '''
    Dump objects in StoreGate

    **examples**::

      athena> dumpSG()
      
    '''
    print (GNS.StoreGate.pointer().dump())


# unregister histogram from HistogramSvc
def unregister (path):
    '''
    Unregister histogram from HistogramSvc
    
    :param path: path to the histogram

    **examples**::

      athena> unregister("/stat/tmpHist")
      
    '''
    return theApp.histSvc().unregisterObject(path)


# dump histograms in HistogramSvc
def dumpHist ():
    '''
    Dump histograms in HistogramSvc

    **examples**::

      athena> dumpHist()
      
    '''
    theApp.histSvc().dump()


# set event loop type to pre-process
def preProcess ():
    '''
    Set event loop type to pre-process

    '''
    global curEvent
    curEvent = theApp.curEvent()
    global eventLoopType    
    eventLoopType = _PreProcess


# set event loop type to normal-process
def normalProcess ():
    '''
    Set event loop type to normal-process

    '''
    global eventLoopType    
    eventLoopType = _NormalProcess
    

# set event loop type to hybrid-process
def hybridProcess ():
    '''
    Set event loop type to hybrid-process

    '''
    global eventLoopType
    eventLoopType = _HybridProcess
