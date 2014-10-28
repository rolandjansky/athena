# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from sys import settrace

from l1topo.TopoAlgos import TopoAlgo
from l1topo.L1TopoMenu import L1TopoMenu
from l1topo.L1TopoFlags import L1TopoFlags

from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerConfigL1Topo")

class TriggerConfigL1Topo:
    
    current = None
    def __init__(self, outputFile = None , inputFile = None , menuName = None ):
        current = self
        from TriggerJobOpts.TriggerFlags import TriggerFlags

        if menuName:
            TriggerFlags.triggerMenuSetup = menuName
        self.menuName = TriggerFlags.triggerMenuSetup()

        self.inputFile     = inputFile
        self.outputFile    = outputFile
        print self.outputFile, self.menuName
        
        # all registered algos
        self.registeredAlgos = {}

        # menu
        self.menu = L1TopoMenu(self.menuName)
        
        if self.inputFile != None:
            """Read menu from XML"""
            print "Menu input is not implemented!!"
        else:
            """Build menu from menu name"""

            # defines the menu (algo only for now)
            self.defineMenu(self.menuName)
        
            # registers all items ever defined
            self.registerMenu()

    def registerAlgo(self, algo):
        """ Add a L1Topo algo to the set of algos which are registered for further use"""

        name = algo.name
        
        if name in self.registeredAlgos:
            log.error('L1Topo algo %s is already registered' % name)
            return self.registeredAlgos[name]
            
        self.registeredAlgos[name] = algo
        log.info("Added %s in the algo list: " % name)
        return algo

    def getRegisteredAlgo(self, name):

        if name in self.registeredAlgos:
            return self.registeredAlgos[name]
        return None

    def findRegisteredSortingAlgoByOutput(self, requiredOutput):
        """returns a list of all sorting algorithms that are needed to
        produce the required output. Even one missing will raise a runtime exception"""
        sortingAlgs = []
        missingOutput = []
        for output in requiredOutput:
            foundOutput = False
            for name, alg in self.registeredAlgos.items():
                if output in alg.outputs:
                    sortingAlgs += [alg]
                    foundOutput = True
                    break
            if not foundOutput:
                missingOutput += [output]
        if missingOutput:
            raise RuntimeError("Not for all required outputs a sorting algorithm could be found: missing lists are %r" % missingOutput)
        return sortingAlgs
            
    def writeXML(self):
        """ Writes L1Topo XML file"""
        if self.outputFile is None:
            log.warning("Can't write xml file since no name was provided")
            return
        FH = open( self.outputFile, mode="wt" )
        FH.write( self.menu.xml() )
        FH.close()
        log.info("Wrote %s " % self.outputFile)

    @staticmethod
    def defineMenu(menuName = None ):
        """
        Defines the list if algo names that will be in the menu

        Calls defineMenu() of the correct module 'Menu_<menuname>.py'

        Menu.defineMenu() defines the menu via L1TopoFlags
        """
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        menumodule = __import__('l1topomenu.Menu_%s' % menuName, globals(), locals(), ['defineMenu'], -1)
        menumodule.defineMenu()
        log.warning("%s menu contains %i algos." % ( menuName, len(L1TopoFlags.algos()) )) 
        
    def registerMenu(self):
        """
        Calls registerTopoAlgos() of the correct module 'TopoalgoDef.py'
        
        Has to run AFTER defineMenu
        """
        algodefmodule = __import__('l1topomenu.TopoAlgoDef', globals(), locals(), ['TopoAlgoDef'], -1)

        algodefmodule.TopoAlgoDef.registerTopoAlgos(self)
        log.warning("Registered %i algos." % ( len(self.registeredAlgos) ) )

        #for algo in self.registeredAlgos:
            #print "Registered algo: ", algo
            #print "xml: ", algo.xml()
        
    def generateMenu(self):
        """
        Generates the menu structure from the list of algo names in the L1TopoFlags

        Always to be called after defineMenu()
        """
        
        from AthenaCommon.Logging import logging
        log = logging.getLogger('L1Topo.generateMenu')

        # add the algos to the menu
        undefined_alg = False 
        for topooutput in L1TopoFlags.algos():
            topooutput.algo = self.getRegisteredAlgo(topooutput.algoname)

            if topooutput.algo is None:
                raise RuntimeError("L1Topo algo of name '%s' is not defined in L1Topo algo definition file TopoAlgoDef.py." % topoline.algoname )

            topooutput.sortingAlgos = self.findRegisteredSortingAlgoByOutput(topooutput.algo.inputs)

            self.menu += topooutput
