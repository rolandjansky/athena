# IOVDbSvc/CondDBConfig.py
# define conditions database connection configuration class
# Richard Hawkings, 6/12/06

include.block("IOVDbSvc/CondDBConfig.py")
include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
include( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

class CondDBConfig:
    
    def __init__(self,tech,user,dbname):
        self.DBTech=tech
        self.DBUser=user
        self.DBInstance=dbname
        # setup all access strings, for offline and online CondDB
        self.INDET=self.SetAcc('COOLONL_INDET')
        self.INDET_ONL=self.SetAcc('COOLONL_INDET')
        self.INDET_OFL=self.SetAcc('COOLOFL_INDET')
        self.PIXEL=self.SetAcc('COOLONL_PIXEL')
        self.PIXEL_ONL=self.SetAcc('COOLONL_PIXEL')
        self.PIXEL_OFL=self.SetAcc('COOLOFL_PIXEL')
        self.SCT=self.SetAcc('COOLONL_SCT')
        self.SCT_ONL=self.SetAcc('COOLONL_SCT')
        self.SCT_OFL=self.SetAcc('COOLOFL_SCT')
        self.TRT=self.SetAcc('COOLONL_TRT')
        self.TRT_ONL=self.SetAcc('COOLONL_TRT')
        self.TRT_OFL=self.SetAcc('COOLOFL_TRT')
        self.LAR=self.SetAcc('COOLONL_LAR')
        self.LAR_ONL=self.SetAcc('COOLONL_LAR')
        self.LAR_OFL=self.SetAcc('COOLOFL_LAR')
        self.TILE=self.SetAcc('COOLONL_TILE')
        self.TILE_ONL=self.SetAcc('COOLONL_TILE')
        self.TILE_OFL=self.SetAcc('COOLOFL_TILE')
        self.MUON=self.SetAcc('COOLONL_MUON')
        self.MUON_ONL=self.SetAcc('COOLONL_MUON')
        self.MUON_OFL=self.SetAcc('COOLOFL_MUON')
        self.MUONALIGN=self.SetAcc('COOLONL_MUONALIGN')
        self.MUONALIGN_ONL=self.SetAcc('COOLONL_MUONALIGN')
        self.MUONALIGN_OFL=self.SetAcc('COOLOFL_MUONALIGN')
        self.MDT=self.SetAcc('COOLONL_MDT')
        self.MDT_ONL=self.SetAcc('COOLONL_MDT')
        self.MDT_OFL=self.SetAcc('COOLOFL_MDT')
        self.RPC=self.SetAcc('COOLONL_RPC')
        self.RPC_ONL=self.SetAcc('COOLONL_RPC')
        self.RPC_OFL=self.SetAcc('COOLOFL_RPC')
        self.TGC=self.SetAcc('COOLONL_TGC')
        self.TGC_ONL=self.SetAcc('COOLONL_TGC')
        self.TGC_OFL=self.SetAcc('COOLOFL_TGC')
        self.CSC=self.SetAcc('COOLONL_CSC')
        self.CSC_ONL=self.SetAcc('COOLONL_CSC')
        self.CSC_OFL=self.SetAcc('COOLOFL_CSC')
        self.TDAQ=self.SetAcc('COOLONL_TDAQ')
        self.TDAQ_ONL=self.SetAcc('COOLONL_TDAQ')
        self.TDAQ_OFL=self.SetAcc('COOLOFL_TDAQ')
        self.DCS_OFL=self.SetAcc('COOLOFL_DCS')
        self.GLOBAL=self.SetAcc('COOLONL_GLOBAL')
        self.GLOBAL_ONL=self.SetAcc('COOLONL_GLOBAL')
        self.GLOBAL_OFL=self.SetAcc('COOLOFL_GLOBAL')
        self.TRIGGER=self.SetAcc('COOLONL_TRIGGER')
        self.TRIGGER_ONL=self.SetAcc('COOLONL_TRIGGER')
        self.TRIGGER_OFL=self.SetAcc('COOLOFL_TRIGGER')
        self.CALO=self.SetAcc('COOLONL_CALO')
        self.CALO_ONL=self.SetAcc('COOLONL_CALO')
        self.CALO_OFL=self.SetAcc('COOLOFL_CALO')
        self.FWD=self.SetAcc('COOLONL_FWD')
        self.FWD_ONL=self.SetAcc('COOLONL_FWD')
        self.FWD_OFL=self.SetAcc('COOLOFL_FWD')
        self.LOCAL="<dbConnection>impl=cool;techno=sqlite;schema=mycool.db;X:"+self.DBInstance+"</dbConnection> "
        # setup default connection to local file
        IOVDbSvc=Service("IOVDbSvc")
        IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mycool.db;X:"+self.DBInstance
        # setup POOL file catalogue access depending on database name
        self.SetPoolCat()
        #
        log.info('Conditions database access configured for instance %s using connection type %s and account %s' % (self.DBInstance,self.DBTech,self.DBUser))

    def SetAcc(self,schema):
        # helper function to setup access string
        if (self.DBTech=="oracle"):
            res="impl=cool;techno=oracle;schema=ATLAS_"+schema+";ATLAS_COOLPROD:"+self.DBInstance+":"+self.DBUser
        elif (self.DBTech=="sqlite"):
            res="impl=cool;techno=sqlite;schema=sqlite130/"+self.DBInstance+".db;X:"+self.DBInstance
        elif (self.DBTech=="logical"):
            res="impl=cool;techno=logical;schema="+schema+";X:"+self.DBInstance
        else:
            res=""
        return "<dbConnection>"+res+"</dbConnection>"
    
    def SetPoolCat(self):
        # helper function to set catalogue depending on database instance name
        # note use of stem names without COOL version id - COOL1.4 ready
        poolcats=[]
        if (self.DBInstance[0:4]=="OFLP"): poolcats+=['oflcond']
        if (self.DBInstance[0:4]=="COMP"): poolcats+=['comcond']
        if (self.DBInstance[0:4]=="CMCP"): poolcats+=['cmccond']
        if (self.DBInstance[0:4]=="TBDP"): poolcats+=['tbcond']
        if (self.DBInstance[0:4]=="TMCP"): poolcats+=['tbcond','oflcond']
        PoolSvc=Service("PoolSvc")
        for i in poolcats:
            PoolSvc.ReadCatalog+= [ "prfile:poolcond/PoolCat_"+i+".xml" ]
