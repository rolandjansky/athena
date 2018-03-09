# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
    def _msg(self):
        """
        Returns logging.
        """
        #=== One can use loggers but not as a datamember of configurables as loggers'
        #=== instances can't be pickled. Hence we use _msg as a property
         
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
            self._msg.info("setupCOOL already called previously, ignoring this repeated call!")
            return
        self._coolIsConfigured = True

        #=== connect all tools to COOL
        from AthenaCommon.AppMgr import ToolSvc
        
        self._msg.info("Changing default TileBadChanTool configuration to COOL source")
        from .TileCondToolConf import getTileBadChanTool
        ToolSvc += getTileBadChanTool('COOL')

        self._msg.info("Changing default TileCondToolEmscale configuration to COOL source")
        from .TileCondToolConf import getTileCondToolEmscale
        ToolSvc += getTileCondToolEmscale('COOL')

        self._msg.info("Changing default TileCondToolNoiseSample configuration to COOL source")
        from .TileCondToolConf import getTileCondToolNoiseSample
        ToolSvc += getTileCondToolNoiseSample('COOL')

#        self._msg.info("Changing default TileCondToolNoiseAutoCr configuration to COOL source")
#        from .TileCondToolConf import getTileCondToolAutoCr
#        ToolSvc += getTileCondToolAutoCr('COOL')
        
        #self._msg.info("Changing default TileCondToolNoiseRawChn configuration to COOL source")
        #from .TileCondToolConf import getTileCondToolNoiseRawChn
        #ToolSvc += getTileCondToolNoiseRawChn('COOL')
        
        self._msg.info("Changing default TileCondToolTiming configuration to COOL source")
        from .TileCondToolConf import getTileCondToolTiming
        ToolSvc += getTileCondToolTiming('COOL',type)
        
 #       self._msg.info("Changing default TileCondToolPulseShape configuration to COOL source")
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
                self._msg.info("setupCOOLOFC already called previously, ignoring this repeated call!")
                return True
            self._coolofcIsConfigured = True
        elif ofcType == 'OF1':
            if self._coolof1ofcIsConfigured:
                self._msg.info("setupCOOLOFC for OF1 already called previously, ignoring this repeated call!")
                return True
            self._coolof1ofcIsConfigured = True
            name = 'TileCondToolOfcCoolOF1'
        else:
            self._msg.info("setupCOOLOFC: not known OFC type: %s! Nothing will be done!" % ofcType)
            return False

        #=== connect TileCondToolOfcCool to COOL
        from AthenaCommon.AppMgr import ToolSvc
        from .TileCondToolConf import getTileCondToolOfcCool
        toolOfcCool = getTileCondToolOfcCool('COOL', type, ofcType, name )
        if toolOfcCool is not None:
            self._msg.info("Changing default TileCondToolOfcCool configuration to COOL source for %s" % ofcType)
            ToolSvc += toolOfcCool
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
                self._msg.info("setupCOOLTIME already called previously, ignoring this repeated call!")
                return
            self._cooltimeIsConfigured = True
        else:
            if self._coolonlinetimeIsConfigured:
                self._msg.info("setupCOOLTIME for online already called previously, ignoring this repeated call!")
                return
            self._coolonlinetimeIsConfigured = True
            name = 'TileCondToolOnlineTiming'

        #=== connect TileCondToolTiming to COOL
        from AthenaCommon.AppMgr import ToolSvc
        
        self._msg.info("Changing default TileCondToolTiming configuration to COOL source")
        from .TileCondToolConf import getTileCondToolTiming
        ToolSvc += getTileCondToolTiming('COOL', type, online, name )



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
            self._msg.info("setupCOOLCISPULSE already called previously, ignoring this repeated call!")
            return
        self._coolcispulseIsConfigured = True

        #=== connect all tools to COOL
        from AthenaCommon.AppMgr import ToolSvc
        
        self._msg.info("Changing default TileCondToolPulseShape configuration to COOL source")
        from .TileCondToolConf import getTileCondToolPulseShape
        ToolSvc += getTileCondToolPulseShape('COOL','CISPULSE100','TileCondToolPulseShape')
        ToolSvc += getTileCondToolPulseShape('COOL','CISPULSE5P2','TileCondToolPulse5p2Shape')
        ToolSvc += getTileCondToolPulseShape('COOL','CISLEAK100','TileCondToolLeak100Shape')
        ToolSvc += getTileCondToolPulseShape('COOL','CISLEAK5P2','TileCondToolLeak5p2Shape')

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
            self._msg.info("setupCOOLLASPULSE already called previously, ignoring this repeated call!")
            return
        self._coollaspulseIsConfigured = True

        #=== connect TileCondToolOfcCool to COOL
        from AthenaCommon.AppMgr import ToolSvc
        
        self._msg.info("Changing default TileCondToolPulseShape configuration to COOL source")
        from .TileCondToolConf import getTileCondToolPulseShape
        ToolSvc += getTileCondToolPulseShape('COOL','LAS','TileCondToolPulseShape')

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
            self._msg.info("setupCOOLPHYPULSE already called previously, ignoring this repeated call!")
            return
        self._coolphypulseIsConfigured = True

        #=== connect TileCondToolOfcCool to COOL
        from AthenaCommon.AppMgr import ToolSvc
        
        self._msg.info("Changing default TileCondToolPHYPULSECool configuration to COOL source")
        from .TileCondToolConf import getTileCondToolPulseShape
        ToolSvc += getTileCondToolPulseShape('COOL')

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
            self._msg.info("setupCOOLINTEGRATOR already called previously, ignoring this repeated call!")
            return
        self._coolintegratorIsConfigured = True

        #=== connect TileCondToolIntegrator to COOL
        from AthenaCommon.AppMgr import ToolSvc
        
        self._msg.info("Changing default TileCondToolIntegrator configuration to COOL source")
        from .TileCondToolConf import getTileCondToolIntegrator
        ToolSvc += getTileCondToolIntegrator('COOL','TileCondToolIntegrator')

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
            self._msg.info("setupCOOLMUID already called previously, ignoring this repeated call!")
            return
        self._coolmuidIsConfigured = True

        #=== connect TileCondToolMuID to COOL
        from AthenaCommon.AppMgr import ToolSvc
        
        self._msg.info("Changing default TileCondToolMuID configuration to COOL source")
        from .TileCondToolConf import getTileCondToolMuID
        ToolSvc += getTileCondToolMuID('COOL','TileCondToolMuID')


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
            self._msg.info("setupCOOLAutoCr already called previously, ignoring this repeated call!")
            return
        self._coolacrIsConfigured = True

        #=== connect TileCondToolMuID to COOL
        from AthenaCommon.AppMgr import ToolSvc
        
        self._msg.info("Changing default TileCondToolNoiseAutoCr configuration to COOL source")
        from .TileCondToolConf import getTileCondToolAutoCr
        ToolSvc += getTileCondToolAutoCr('COOL','TileCondToolAutoCr')

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
            self._msg.info("setupCOOLEMEXPERT already called previously, ignoring this repeated call!")
            return
        self._coolEmExpertIsConfigured = True

        #=== connect TileExpertToolEmscale to COOL
        from AthenaCommon.AppMgr import ToolSvc
        
        self._msg.info("Changing default TileExpertToolEmscale configuration to COOL source")
        from .TileCondToolConf import getTileExpertToolEmscale
        ToolSvc += getTileExpertToolEmscale('COOL')        

        return

    #_______________________________________________________________
    def setupCOOLDCS(self, defTag = "", dbConnection = "", useHV = True, useHVSET = False, useSTATUS = True):
        """
        Call this function to read DCS from COOL folder.
        Input parameters:
        - defTag          : Tag to be added to each folder tag (NGO change this to a hierarchical tag!)
        - dbConnection    : The DB connection string to use [default "": auto-initialization by CondDBSetup.py]
        """

        #=== prevent a second initialization
        if self._coolDCSIsConfigured:
            self._msg.info("setupCOOLDCS already called previously, ignoring this repeated call!")
            return
        self._coolDCSIsConfigured = True
        
        ##     #=== ensure the availability of TileDCSSvc
        from AthenaCommon.GlobalFlags import globalflags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        TileUseDCS = (not athenaCommonFlags.isOnline()) and globalflags.DataSource()=='data'
        ## get a handle to the ServiceManager
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if TileUseDCS and not hasattr(svcMgr, "TileDCSSvc"):
            self._msg.info("setting up DCS TileCal conditions data")
            dbConnStr = 'DCS_OFL'
            from IOVDbSvc.CondDB import conddb
            if useHV:     conddb.addFolder(dbConnStr, "/TILE/DCS/HV")
            if useHVSET:  conddb.addFolder(dbConnStr, "/TILE/DCS/HVSET")
            if useSTATUS: conddb.addFolder(dbConnStr, "/TILE/DCS/STATES")
            self._msg.info("Adding TileDCSSvc to ServiceMgr")
            svcMgr += CfgMgr.TileDCSSvc()

            from IOVDbSvc.CondDB import conddb
            if (conddb.GetInstance() == 'CONDBR2'):
                self._msg.info("setting up TileDCSSvc for RUN2")
                svcMgr.TileDCSSvc.Version=2
            else:
                self._msg.info("setting up TileDCSSvc for RUN1")
                svcMgr.TileDCSSvc.Version=1


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
            self._msg.info("setupCOOLDspThrehsold already called previously, ignoring this repeated call!")
            return True

        #=== connect TileCondToolMuID to COOL
        from AthenaCommon.AppMgr import ToolSvc
        from .TileCondToolConf import getTileCondToolDspThreshold
        toolDspThreshold = getTileCondToolDspThreshold('COOL','TileCondToolDspThreshold')
        if toolDspThreshold is not None:
            self._msg.info("Changing default TileCondToolDspThreshold configuration to COOL source")
            ToolSvc += toolDspThreshold
            self._coolDspThresholdIsConfigured = True
            return True
        else:
            return False

  
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
    if not svcName in theApp.CreateSvc:
        theApp.CreateSvc += [ svcName ]

    #=== ensure the availability of TileCablingSvc
    if not hasattr(svcMgr, "TileCablingSvc"):
        svc._msg.info("Adding TileCablingSvc to ServiceMgr")
        svcMgr += CfgMgr.TileCablingSvc()

    #=== ensure the configuration of all needed tools
    from AthenaCommon.AppMgr import ToolSvc

    if not hasattr(ToolSvc, "TileBadChanTool"):
        svc._msg.info("Adding default TileBadChanTool to ToolSvc")
        from .TileCondToolConf import getTileBadChanTool
        ToolSvc += getTileBadChanTool()
    if not hasattr(ToolSvc, "TileCondToolOfc"):
        svc._msg.info("Adding default TileCondToolOfc to ToolSvc")
        from .TileConditionsConf import TileCondToolOfc
        ToolSvc += TileCondToolOfc()
    if not hasattr(ToolSvc, "TileCondToolEmscale"):
        svc._msg.info("Adding default TileCondToolEmscale to ToolSvc")
        from .TileCondToolConf import getTileCondToolEmscale
        ToolSvc += getTileCondToolEmscale()
    #if not hasattr(ToolSvc, "TileCondToolIntegrator"):
    #    svc._msg.info("Adding default TileCondToolIntegrator to ToolSvc")
    #    from .TileCondToolConf import getTileCondToolIntegrator
    #    ToolSvc += getTileCondToolIntegrator()
    if not hasattr(ToolSvc, "TileCondToolNoiseSample"):
        svc._msg.info("Adding default TileCondToolNoiseSample to ToolSvc")
        from .TileCondToolConf import getTileCondToolNoiseSample
        ToolSvc += getTileCondToolNoiseSample()
 #   if not hasattr(ToolSvc, "TileCondToolNoiseAutoCr"):
 #       svc._msg.info("Adding default TileCondToolNoiseAutoCr to ToolSvc")
 #       from .TileCondToolConf import getTileCondToolAutoCr
 #       ToolSvc += getTileCondToolAutoCr()
    #if not hasattr(ToolSvc, "TileCondToolNoiseRawChn"):
    #    svc._msg.info("Adding default TileCondToolNoiseRawChn to ToolSvc")
    #    from .TileCondToolConf import getTileCondToolNoiseRawChn
    #    ToolSvc += getTileCondToolNoiseRawChn()
    if not hasattr(ToolSvc, "TileCondToolTiming"):
        svc._msg.info("Adding default TileCondToolTiming to ToolSvc")
        from .TileCondToolConf import getTileCondToolTiming
        ToolSvc += getTileCondToolTiming()
#    if not hasattr(ToolSvc, "TileCondToolPulseShape"):
#        svc._msg.info("Adding default TileCondToolPulseShape to ToolSvc")
#        from .TileCondToolConf import getTileCondToolPulseShape
#        ToolSvc += getTileCondToolPulseShape()
    #if not hasattr(ToolSvc, "TileCondToolOfcCool"):
    #    svc._msg.info("Adding default TileCondToolOfcCool to ToolSvc")
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
