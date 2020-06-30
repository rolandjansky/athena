# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##############################################################
#
#  TileInfoConfigurator class
#
#  used to set up the TileInfoLoader service
#
# Author:   nils.gollub@cern.ch
# Modified: lukas.pribyl@cern.ch
#############################################################


from AthenaCommon import CfgMgr
TileInfoLoader = CfgMgr.TileInfoLoader

class _TileInfoConfigurator( TileInfoLoader ):
    """
    TileInfoLoader with additional state
    """
    __slots__ = {
        '_coolIsConfigured' : False,
        '_coolofcIsConfigured': False,
        '_coolof1ofcIsConfigured': False,
        '_coolcispulseIsConfigured': False,
        '_coolintegratorIsConfigured': False,
        '_coolmuidIsConfigured': False,
        '_coolEmExpertIsConfigured' : False,
        '_coolDCSIsConfigured' : False,
        '_coolacrIsConfigured' : False,
        '_coolphypulseIsConfigured' : False,
        '_coollaspulseIsConfigured' : False,
        '_cooltimeIsConfigured' : False,
        '_coolonlinetimeIsConfigured' : False,
        '_coolDspThresholdIsConfigured' : False,
        '_msg': None
        }


    #_______________________________________________________________
    def __init__(self, name="TileInfoLoader", **kwargs):

        #=== set the single service name
        svcName = name

        #=== call baseclass constructor
        super(_TileInfoConfigurator, self).__init__(svcName, **kwargs)
        return

    @property
    #_______________________________________________________________
    def msg(self):
        """
        Returns logging.
        """
        #=== One can use loggers but not as a datamember of configurables as loggers'
        #=== instances can't be pickled. Hence we use msg as a property

        #=== initialize logger with INFO level
        import AthenaCommon.Logging as L
        msg = L.logging.getLogger('TileInfoConf.')
        #        msg.setLevel(L.logging.INFO)
        return msg

    #_______________________________________________________________
    def setupCOOL(self, defTag = "", dbConnection = "", type = "PHY"):
        """
        Call this function to read calibration constants from the COOL DB (default is reading from text files).
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coolIsConfigured:
            self.msg.info("setupCOOL already called previously, ignoring this repeated call!")
            return
        self._coolIsConfigured = True

        #=== connect all tools to COOL

        self.msg.info("Changing default TileBadChanTool configuration to COOL source")
        from .TileCondToolConf import getTileBadChanTool
        getTileBadChanTool('COOL')

        self.msg.info("Changing default TileCondToolEmscale configuration to COOL source")
        from .TileCondToolConf import getTileCondToolEmscale
        getTileCondToolEmscale('COOL')

        self.msg.info("Changing default TileCondToolNoiseSample configuration to COOL source")
        from .TileCondToolConf import getTileCondToolNoiseSample
        getTileCondToolNoiseSample('COOL')

#        self.msg.info("Changing default TileCondToolNoiseAutoCr configuration to COOL source")
#        from .TileCondToolConf import getTileCondToolAutoCr
#        ToolSvc += getTileCondToolAutoCr('COOL')

        #self.msg.info("Changing default TileCondToolNoiseRawChn configuration to COOL source")
        #from .TileCondToolConf import getTileCondToolNoiseRawChn
        #ToolSvc += getTileCondToolNoiseRawChn('COOL')

        self.msg.info("Changing default TileCondToolTiming configuration to COOL source")
        from .TileCondToolConf import getTileCondToolTiming
        getTileCondToolTiming('COOL',type)

 #       self.msg.info("Changing default TileCondToolPulseShape configuration to COOL source")
 #       from .TileCondToolConf import getTileCondToolPulseShape
 #       ToolSvc += getTileCondToolPulseShape('COOL')


        #in future will need this tool as well
        #from .TileCondToolConf import getTileCellNoiseTool

        return

    #_______________________________________________________________
    def setupCOOLOFC(self, defTag = "", dbConnection = "", type = "PHY", ofcType = 'OF2'):
        """
        Call this function to read OFCs from Online COOL folder.
        Returns if the corresponding tool is configured successfully
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        - type              Pulse type - PHY or LAS or CIS
        - ofcType         : Optimal filtering methods - OF1 or OF2
        """

        name = 'TileCondToolOfcCool'
        #=== prevent a second initialization
        if ofcType == 'OF2':
            if self._coolofcIsConfigured:
                self.msg.info("setupCOOLOFC already called previously, ignoring this repeated call!")
                return True
            self._coolofcIsConfigured = True
        elif ofcType == 'OF1':
            if self._coolof1ofcIsConfigured:
                self.msg.info("setupCOOLOFC for OF1 already called previously, ignoring this repeated call!")
                return True
            self._coolof1ofcIsConfigured = True
            name = 'TileCondToolOfcCoolOF1'
        else:
            self.msg.info("setupCOOLOFC: not known OFC type: %s! Nothing will be done!", ofcType)
            return False

        #=== connect TileCondToolOfcCool to COOL
        from .TileCondToolConf import getTileCondToolOfcCool
        toolOfcCool = getTileCondToolOfcCool('COOL', type, ofcType, name )
        if toolOfcCool is not None:
            self.msg.info("Changing default TileCondToolOfcCool configuration to COOL source for %s", ofcType)
            return True
        elif ofcType == 'OF1':
            self._coolof1ofcIsConfigured = False
        else:
            self._coolofcIsConfigured = False
        return False



    #_______________________________________________________________
    def setupCOOLTIME(self, defTag = "", dbConnection = "", type = "PHY", online = False):
        """
        Call this function to read Time from COOL folder.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        - type              Pulse type - PHY or LAS or CIS
        - online          : Read time from online COOL folder always
        """

        name = 'TileCondToolTiming'
        #=== prevent a second initialization
        if not online:
            if self._cooltimeIsConfigured:
                self.msg.info("setupCOOLTIME already called previously, ignoring this repeated call!")
                return
            self._cooltimeIsConfigured = True
        else:
            if self._coolonlinetimeIsConfigured:
                self.msg.info("setupCOOLTIME for online already called previously, ignoring this repeated call!")
                return
            self._coolonlinetimeIsConfigured = True
            name = 'TileCondToolOnlineTiming'

        #=== connect TileCondToolTiming to COOL
        self.msg.info("Changing default TileCondToolTiming configuration to COOL source")
        from .TileCondToolConf import getTileCondToolTiming
        getTileCondToolTiming('COOL', type, online, name )



    #_______________________________________________________________
    def setupCOOLPULSE(self, defTag = "", dbConnection = "", type = "PHY"):
        """
        Call this function to read pulse shape from from Online COOL folder.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        - type              Pulse type - PHY or LAS or CIS
        """

        if type=="CIS":
            self.setupCOOLCISPULSE(defTag,dbConnection)
        elif type=="LAS":
            self.setupCOOLLASPULSE(defTag,dbConnection)
        else:
            self.setupCOOLPHYPULSE(defTag,dbConnection)

    #_______________________________________________________________
    def setupCOOLCISPULSE(self, defTag = "", dbConnection = ""):
        """
        Call this function to read calibration constants from the COOL DB (default is reading from text files).
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coolcispulseIsConfigured:
            self.msg.info("setupCOOLCISPULSE already called previously, ignoring this repeated call!")
            return
        self._coolcispulseIsConfigured = True

        #=== connect all tools to COOL
        self.msg.info("Changing default TileCondToolPulseShape configuration to COOL source")
        from .TileCondToolConf import getTileCondToolPulseShape
        getTileCondToolPulseShape('COOL','CISPULSE100','TileCondToolPulseShape')
        getTileCondToolPulseShape('COOL','CISPULSE5P2','TileCondToolPulse5p2Shape')
        getTileCondToolPulseShape('COOL','CISLEAK100','TileCondToolLeak100Shape')
        getTileCondToolPulseShape('COOL','CISLEAK5P2','TileCondToolLeak5p2Shape')

    #_______________________________________________________________
    def setupCOOLLASPULSE(self, defTag = "", dbConnection = ""):
        """
        Call this function to read LAS pulse shape from from Online COOL folder.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coollaspulseIsConfigured:
            self.msg.info("setupCOOLLASPULSE already called previously, ignoring this repeated call!")
            return
        self._coollaspulseIsConfigured = True

        #=== connect TileCondToolOfcCool to COOL
        self.msg.info("Changing default TileCondToolPulseShape configuration to COOL source")
        from .TileCondToolConf import getTileCondToolPulseShape
        getTileCondToolPulseShape('COOL','LAS','TileCondToolPulseShape')

    #_______________________________________________________________
    def setupCOOLPHYPULSE(self, defTag = "", dbConnection = ""):
        """
        Call this function to read PHY pulse shape from from Online COOL folder.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coolphypulseIsConfigured:
            self.msg.info("setupCOOLPHYPULSE already called previously, ignoring this repeated call!")
            return
        self._coolphypulseIsConfigured = True

        #=== connect TileCondToolOfcCool to COOL
        self.msg.info("Changing default TileCondToolPHYPULSECool configuration to COOL source")
        from .TileCondToolConf import getTileCondToolPulseShape
        getTileCondToolPulseShape('COOL')

    #_______________________________________________________________
    def setupCOOLINTEGRATOR(self, defTag = "", dbConnection = ""):
        """
        Call this function to read integrator calibration constants from the COOL DB.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coolintegratorIsConfigured:
            self.msg.info("setupCOOLINTEGRATOR already called previously, ignoring this repeated call!")
            return
        self._coolintegratorIsConfigured = True

        #=== connect TileCondToolIntegrator to COOL
        self.msg.info("Changing default TileCondToolIntegrator configuration to COOL source")
        from .TileCondToolConf import getTileCondToolIntegrator
        getTileCondToolIntegrator('COOL','TileCondToolIntegrator')

    #_______________________________________________________________
    def setupCOOLMUID(self, defTag = "", dbConnection = ""):
        """
        Call this function to read MuID thresholds from the COOL DB.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coolmuidIsConfigured:
            self.msg.info("setupCOOLMUID already called previously, ignoring this repeated call!")
            return
        self._coolmuidIsConfigured = True

        #=== connect TileCondToolMuID to COOL
        self.msg.info("Changing default TileCondToolMuID configuration to COOL source")
        from .TileCondToolConf import getTileCondToolMuID
        getTileCondToolMuID('COOL','TileCondToolMuID')


    #_______________________________________________________________
    def setupCOOLAutoCr(self, defTag = "", dbConnection = ""):
        """
        Call this function to read MuID thresholds from the COOL DB.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coolacrIsConfigured:
            self.msg.info("setupCOOLAutoCr already called previously, ignoring this repeated call!")
            return
        self._coolacrIsConfigured = True

        #=== connect TileCondToolMuID to COOL
        self.msg.info("Changing default TileCondToolNoiseAutoCr configuration to COOL source")
        from .TileCondToolConf import getTileCondToolAutoCr
        getTileCondToolAutoCr('COOL','TileCondToolAutoCr')

    #_______________________________________________________________
    def setupCOOLEMEXPERT(self, defTag = "", dbConnection = ""):
        """
        Call this function to read EM calibration constants from the COOL DB (default is reading from text files).
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coolEmExpertIsConfigured:
            self.msg.info("setupCOOLEMEXPERT already called previously, ignoring this repeated call!")
            return
        self._coolEmExpertIsConfigured = True

        #=== connect TileExpertToolEmscale to COOL
        self.msg.info("Changing default TileExpertToolEmscale configuration to COOL source")
        from .TileCondToolConf import getTileExpertToolEmscale
        getTileExpertToolEmscale('COOL')

        return

    #_______________________________________________________________
    def setupCOOLDCS(self, defTag = "", dbConnection = "", useHV = True, useHVSET = False, useSTATUS = True):
        """
        Call this function to read DCS from COOL folder.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """
        from AthenaCommon.AlgSequence import AthSequencer
        condSequence = AthSequencer("AthCondSeq")

        #=== prevent a second initialization
        if self._coolDCSIsConfigured:
            self.msg.info("setupCOOLDCS already called previously, ignoring this repeated call!")
            return
        self._coolDCSIsConfigured = True

        ##     #=== ensure the availability of TileDCS information
        from AthenaCommon.GlobalFlags import globalflags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        TileUseDCS = (not athenaCommonFlags.isOnline()) and globalflags.DataSource()=='data'
        ## get a handle to the ServiceManager

        dcs = 'TileDCS'
        dcsCondAlg = dcs + 'CondAlg'
        if TileUseDCS and not hasattr(condSequence, dcsCondAlg):
            self.msg.info("setting up DCS TileCal conditions data")
            dbConnStr = 'DCS_OFL'
            from IOVDbSvc.CondDB import conddb
            if useHV:
                conddb.addFolder(dbConnStr, "/TILE/DCS/HV", className = 'CondAttrListCollection')
            if useHVSET:
                conddb.addFolder(dbConnStr, "/TILE/DCS/HVSET", className = 'CondAttrListCollection')
            if useSTATUS:
                conddb.addFolder(dbConnStr, "/TILE/DCS/STATES", className = 'CondAttrListCollection')

            from TileConditions.TileConditionsConf import TileDCSCondAlg
            condSequence += TileDCSCondAlg(name = dcsCondAlg,
                                           ReadHV = useHV,
                                           ReadHVSET = useHVSET,
                                           ReadSTATES = useSTATUS,
                                           TileDCS = dcs)


    #_______________________________________________________________
    def setupCOOLDspThreshold(self, defTag = "", dbConnection = ""):
        """
        Call this function to read DSP thresholds from the COOL DB.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coolDspThresholdIsConfigured:
            self.msg.info("setupCOOLDspThrehsold already called previously, ignoring this repeated call!")
            return True

        #=== connect TileCondToolMuID to COOL
        from .TileCondToolConf import getTileCondToolDspThreshold
        toolDspThreshold = getTileCondToolDspThreshold('COOL','TileCondToolDspThreshold')
        if toolDspThreshold is not None:
            self.msg.info("Changing default TileCondToolDspThreshold configuration to COOL source")
            self._coolDspThresholdIsConfigured = True
            return True
        else:
            return False


    #_______________________________________________________________
    def setupAdcRange(self, Nbits = 10):
        """
        Call this function to setup maximum range of ADC counts
        Input parameters:
        - Nbits : Number of bits of an ADC (either 10 or 12)
        """
        self.msg.info("Setting {0}-bit Tile ADC".format(Nbits))
        if Nbits == 10:
            self.ADCmax = 1023
            self.ADCmaskValue = 2047
        elif Nbits == 12:
            self.ADCmax = 4095
            self.ADCmaskValue = 4800
        # NOTE: For self.ADCmaskValue, value 4800 should work both for 10-bit and 12-bit settings.
        #       It has to be a value between the interval of possible opt filter output values and
        #       the interval of possible fit method output values. In case of 12-bit settings:
        #         - opt filter values: (-500, 4600)
        #         - fit method values: (5000, 9095) (here, 5000 is added to the output of the fit method itself to indicate that the fit method was used)
        return

    #_______________________________________________________________
def TileInfoConfigurator(name="TileInfoLoader", **kwargs):
    """
    factory function to create readily configured TileInfoLoader instances
    """

    svcName = name

    #=== check if already there...
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if hasattr( svcMgr , svcName ):
        svc = getattr (svcMgr, svcName)
        #--- trying to create duplicate, returning original
        return svc

    svc = _TileInfoConfigurator(svcName, **kwargs)

    from AthenaCommon import CfgMgr

    #=== add to ServiceMgr
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr( svcMgr , svcName ):
        svcMgr += svc

    #=== ensure the creation of service
    from AthenaCommon.AppMgr import theApp
    if svcName not in theApp.CreateSvc:
        theApp.CreateSvc += [ svcName ]

    #=== ensure the availability of TileCablingSvc
    if not hasattr(svcMgr, "TileCablingSvc"):
        svc.msg.info("Adding TileCablingSvc to ServiceMgr")
        svcMgr += CfgMgr.TileCablingSvc()

    #=== ensure the configuration of all needed tools


#    if not hasattr(ToolSvc, "TileBadChanTool"):
#        svc.msg.info("Adding default TileBadChanTool to ToolSvc")
#        from .TileCondToolConf import getTileBadChanTool
#        ToolSvc += getTileBadChanTool()
#    if not hasattr(ToolSvc, "TileCondToolOfc"):
#        svc.msg.info("Adding default TileCondToolOfc to ToolSvc")
#        from .TileConditionsConf import TileCondToolOfc
#        ToolSvc += TileCondToolOfc()
#    if not hasattr(ToolSvc, "TileCondToolEmscale"):
#        svc.msg.info("Adding default TileCondToolEmscale to ToolSvc")
#        from .TileCondToolConf import getTileCondToolEmscale
#        ToolSvc += getTileCondToolEmscale()


    #if not hasattr(ToolSvc, "TileCondToolIntegrator"):
    #    svc.msg.info("Adding default TileCondToolIntegrator to ToolSvc")
    #    from .TileCondToolConf import getTileCondToolIntegrator
    #    ToolSvc += getTileCondToolIntegrator()


#    if not hasattr(ToolSvc, "TileCondToolNoiseSample"):
#        svc.msg.info("Adding default TileCondToolNoiseSample to ToolSvc")
#        from .TileCondToolConf import getTileCondToolNoiseSample
#        ToolSvc += getTileCondToolNoiseSample()


 #   if not hasattr(ToolSvc, "TileCondToolNoiseAutoCr"):
 #       svc.msg.info("Adding default TileCondToolNoiseAutoCr to ToolSvc")
 #       from .TileCondToolConf import getTileCondToolAutoCr
 #       ToolSvc += getTileCondToolAutoCr()
    #if not hasattr(ToolSvc, "TileCondToolNoiseRawChn"):
    #    svc.msg.info("Adding default TileCondToolNoiseRawChn to ToolSvc")
    #    from .TileCondToolConf import getTileCondToolNoiseRawChn
    #    ToolSvc += getTileCondToolNoiseRawChn()


#    if not hasattr(ToolSvc, "TileCondToolTiming"):
#        svc.msg.info("Adding default TileCondToolTiming to ToolSvc")
#        from .TileCondToolConf import getTileCondToolTiming
#        ToolSvc += getTileCondToolTiming()


#    if not hasattr(ToolSvc, "TileCondToolPulseShape"):
#        svc.msg.info("Adding default TileCondToolPulseShape to ToolSvc")
#        from .TileCondToolConf import getTileCondToolPulseShape
#        ToolSvc += getTileCondToolPulseShape()
    #if not hasattr(ToolSvc, "TileCondToolOfcCool"):
    #    svc.msg.info("Adding default TileCondToolOfcCool to ToolSvc")
    #    from .TileCondToolConf import getTileCondToolOfcCool
    #    ToolSvc += getTileCondToolOfcCool()

    #in future will need this tool as well
    #from .TileCondToolConf import getTileCellNoiseTool

    #=== by default COOL is not configured
    svc._coolIsConfigured = False
    svc._coolofcIsConfigured = False
    svc._coolof1ofcIsConfigured = False
    svc._coolcispulseIsConfigured = False
    svc._coolintegratorIsConfigured = False
    svc._coolmuidIsConfigured = False
    svc._coolEmExpertIsConfigured = False
    svc._coolDCSIsConfigured = False
    svc._coolacrIsConfigured = False
    svc._coolphypulseIsConfigured = False
    svc._coollaspulseIsConfigured = False
    svc._cooltimeIsConfigured = False
    svc._coolonlinetimeIsConfigured = False
    svc._coolDspThresholdIsConfigured = False
    return svc
