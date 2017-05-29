# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from l1topo.L1TopoMenu import L1TopoMenu
from l1topo.L1TopoFlags import L1TopoFlags

from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerConfigL1Topo")

class TriggerConfigL1Topo:
    
    current = None
    def __init__(self, outputFile = None , inputFile = None , menuName = None ):
        """
        inputFile: if set the topo menu will be read from this xml file (not yet implemented)
        outputFile: if no input file is specified the topo menu will be generated and written to outputFile
        menuName: ignored now
        """
        from TriggerJobOpts.TriggerFlags import TriggerFlags

        self.menuName = TriggerConfigL1Topo.getMenuBaseName(TriggerFlags.triggerMenuSetup())

        self.inputFile     = inputFile
        self.outputFile    = outputFile
        
        # all registered algos
        self.registeredAlgos = {}

        self.runningid=0
        
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


    # remove prescale suffixes
    @staticmethod
    def getMenuBaseName(menuName):
        import re 
        pattern = re.compile('_v\d+|DC14+|_PhaseII')
        patternPos = pattern.search(menuName)
        if patternPos:
            menuName=menuName[:patternPos.end()]
        else:
            log.info('Can\'t find pattern to shorten menu name, either non-existent in name or not implemented.')
        return menuName         




    def registerAlgo(self, algo):
        """ Add a L1Topo algo to the set of algos which are registered for further use"""

        if algo.name in self.registeredAlgos:
            raise RuntimeError('L1Topo algo %s is already registered' % algo.name)
            
        self.registeredAlgos[algo.name] = algo
        log.debug("Added in the algo list: {0}, ID:{1}" .format(algo.name,algo.algoId))
        return algo


    
    def getRegisteredAlgo(self, name):
        if name in self.registeredAlgos:
#            print "Returning algo: {0}, ID:{1}, reassigning to {2}" .format(self.registeredAlgos[name].name,self.registeredAlgos[name].algoId,self.runningid )
#            self.registeredAlgos[name].algoId=self.runningid
            self.runningid+=1
            return self.registeredAlgos[name]
        
        return None


    def findRegisteredSortingAlgoByOutput(self, algo):
        """returns a list of all sorting algorithms that are needed to
        produce the required output. Even one missing will raise a runtime exception"""
        sortingAlgs = []
        missingOutput = []
        for requiredInput in algo.inputs:
            foundOutput = False
            for name, alg in self.registeredAlgos.items():
                if type(alg.outputs)==list:
                    foundOutput = (requiredInput in alg.outputs)
                else:
                    foundOutput = (requiredInput == alg.outputs)
                if foundOutput:
                    sortingAlgs += [alg]
                    break
            if not foundOutput:
                missingOutput += [(algo.name, requiredInput)]
        if missingOutput:
            raise RuntimeError("For some algorithms not all inputs could be found. Missing are: %r" % ["%s <- %s" % (algname, inputname) for (algname, inputname) in missingOutput] )
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

        menuName = TriggerConfigL1Topo.getMenuBaseName(menuName)
        menumodule = __import__('l1topomenu.Menu_%s' % menuName, globals(), locals(), ['defineMenu'], -1)
        menumodule.defineMenu()
        log.info("%s menu contains %i algos." % ( menuName, len(L1TopoFlags.algos()) )) 
        

    def registerMenu(self):
        """
        Calls registerTopoAlgos() of the correct module 'TopoalgoDef.py'
        
        Has to run AFTER defineMenu
        """
        algodefmodule = __import__('l1topomenu.TopoAlgoDef', globals(), locals(), ['TopoAlgoDef'], -1)
        algodefmodule.TopoAlgoDef.registerTopoAlgos(self)
        log.info("Registered %i algos." % ( len(self.registeredAlgos) ) )

        
    def generateMenu(self):
        """
        Generates the menu structure from the list of algo names in the L1TopoFlags

        Always to be called after defineMenu()
        """
        
        # add the algos to the menu
        for topooutput in L1TopoFlags.algos():
            topooutput.algo = self.getRegisteredAlgo(topooutput.algoname)
            if topooutput.algo is None:
                raise RuntimeError("L1Topo algo of name '%s' is not defined in L1Topo algo definition file TopoAlgoDef.py." % topooutput.algoname )

            topooutput.sortingAlgos = self.findRegisteredSortingAlgoByOutput(topooutput.algo)

            #print "For decision alg %s with inputs %r found the following sorting algs %r" % (topooutput.algo.name, topooutput.algo.inputs, [x.name for x in topooutput.sortingAlgos])

            self.menu += topooutput

        if not self.menu.check():
            raise RuntimeError("Menu check failed")


        
