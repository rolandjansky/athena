# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from G4AtlasApps.PyG4Atlas      import G4AtlasEngine,PyG4AtlasAlg,DetFacility,PhysicsReg
from G4AtlasApps.SimAtlasKernel import AtlasSimSkeleton
from AthenaCommon.AppMgr import theApp
from AthenaCommon.BeamFlags import jobproperties
from G4AtlasApps import AtlasG4Eng
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Include import include
from AthenaPython import PyAthena
        
class G4ConfigTool(PyAthena.AlgTool):

    """
    Python tool which configures Geant4. 
    """

    def __init__(self, name="G4ConfigTool", sim_module=None):

        """
        name                  --> must be 'G4ConfigTool'
        sim_module            --> name of a Python module which describes the simulation

        NB. If sim_module is an empty string or None, the sim skeleton contained
        in G4AtlasEng.G4Eng.Dict() will be run.
        """
        
        print "in G4ConfigTool __init__"

        super(G4ConfigTool, self).__init__(name=name)

        self.doFirstEventG4SeedsCheck = True
        self.sim_module = sim_module
        self.RndG4Menu = None

        ## Set up the appropriate sim skeleton in the G4 engine
        from G4AtlasApps import AtlasG4Eng
        from G4AtlasApps.SimFlags import simFlags
        if "ATLAS" in simFlags.SimLayout():

            AtlasG4Eng.G4Eng.log.info('Configuring ATLAS simulations')

            from ISF_Geant4Tools.ISF_AtlasSimSkeleton import ISF_AtlasSimSkeleton
            dummy = ISF_AtlasSimSkeleton()
            #dummy = AtlasSimSkeleton()

        ## Import sim module if requested
        # TODO: is this ever used?
        if self.sim_module:
            G4AtlasEngine.log.info("The kernel simulation Python module which describes the simulation is: %s" % self.sim_module)
            try:
                __import__(self.sim_module, globals(), locals())
            except:
                G4AtlasEngine.log.fatal("The kernel simulation Python module '%s' was not found!" % self.sim_module)
                raise RuntimeError('G4ConfigTool: G4ConfigTool: initialize()')

        ## Call the sim skeleton pre-init method
        AtlasG4Eng.G4Eng.Dict['simu_skeleton']._do_PreInit()

        ## Start profiler
        AtlasG4Eng.G4Eng._app_profiler('G4ConfigTool before initialize')

    def initialize(self):
        print "in G4ConfigTool initialize"

        from G4AtlasApps import AtlasG4Eng

        from time import gmtime, strftime
        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
        G4AtlasEngine.log.info('G4ConfigTool starting at (UTC): %s' % timestr)
        AtlasG4Eng.G4Eng._app_profiler('G4ConfigTool begin of initialize')
        AtlasG4Eng.G4Eng.Dict['simu_skeleton']._do_All()
        AtlasG4Eng.G4Eng._init_Simulation()

        # *AS* HACK, as "G4AtlasControl/SimControl.cxx" fails dynamic cast
        # see also G4AtlasRunManager
        AtlasG4Eng.G4Eng.gbl.G4Commands().run.verbose(2)
        AtlasG4Eng.G4Eng._ctrl.G4Command("/run/initialize")
        #AtlasG4Eng.G4Eng.gbl.G4Commands().tracking.verbose(1)
        
        AtlasG4Eng.G4Eng._app_profiler('G4ConfigTool end of initialize')
        self.RndG4Menu = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.EvtMax.statusOn and theApp.EvtMax == -1:
            theApp.EvtMax = athenaCommonFlags.EvtMax()
        return True


    #def execute(self):
#    def start(self):
#        from G4AtlasApps import AtlasG4Eng
#        from G4AtlasApps.SimFlags import simFlags
#
#        if self.doFirstEventG4SeedsCheck :
#            if simFlags.SeedsG4.statusOn:
#                rnd = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
#                rnd.set_Seed(simFlags.SeedsG4.get_Value())
#                self.doFirstEventG4SeedsCheck = False
#                if self.RndG4Menu.SaveStatus:
#                    self.RndG4Menu.Menu.saveStatus('G4Seeds.txt')
#
#        return True


    def finalize(self):
        #import AtlasG4Eng
        from G4AtlasApps import AtlasG4Eng
        AtlasG4Eng.G4Eng._app_profiler('G4ConfigTool at finalize  ')

        from time import gmtime, strftime
        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
        G4AtlasEngine.log.info('G4ConfigTool ending at (UTC): %s' % timestr)

        return True
