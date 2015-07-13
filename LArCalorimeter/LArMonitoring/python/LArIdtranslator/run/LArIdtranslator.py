# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=================================================================================
## Name:        LArIdtranslator.py
##
## Description: Generate LArIdtranslator db file
##==================================================================================
__author__="Mathieu Plamondon <Mathieu.Plamondon@cern.ch>"
__doc__ = "An algorithm to generate LArIdtranslator db file"

from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena

import PyCintex as PyLCGDict
for i in ['LArToolsDict','TrigT1CaloEventDict','LArMonitoringDict']: PyLCGDict.loadDict(i)
from ROOT import HWIdentifier, Identifier, IdentifierHash, CaloDetDescrManager#, LArBadChannel

def getONL(be,pn,ft,sl):
  onl=0
  onl += ((sl-1) << 15)
  onl += (ft << 19)
  onl += (pn << 24)
  onl += (be << 25)
  onl += (14 << 26)
  return onl

ONL2IDs = {}
f = file('global_ONLD_FEB_DSP_PU_ROL_ROD_ROS.dat','r')
for i in f:
  x = i.split()
  ONL2IDs[eval(x[0])] = [x[1],x[2],x[3],x[4],x[5],x[6]]
f.close()

class LArIdtranslator(PyAthena.Alg):
    """
    Generate LArIdtranslator db file
    """
    
    def __init__(self, name="LArIdtranslator", **kw):
        ## init the base class
        kw['name'] = name
        super(LArIdtranslator,self).__init__(**kw)
        self.nEvts=0
        return

    def initialize(self):
        # -----------------------------------------------------------
        # Store gate  service
        self.msg.info('initializing [%s]...',self.name())
        self.msg.debug('retrieve StoreGateSvc pointer...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if self.sg is None:
          self.msg.error('Problem retrieving StoreGateSvc pointer !')
          return StatusCode.Failure
        else:
          self.msg.info('retrieved [%s]', self.sg.name())

        # -----------------------------------------------------------
        # Message service
        self.msg.info('initializing [%s]...',self.name())
        self.msg.debug('retrieve MessageSvc pointer...')
        self.msgSvc = PyAthena.py_svc('MessageSvc')
        if self.msgSvc is None:
          self.msg.error('Problem retrieving MessageSvc pointer !')
          return StatusCode.Failure
        else:
          self.msg.info('retrieved [%s]', self.msgSvc.name())

        # -----------------------------------------------------------
        # DetectorStore
        from StoreGateBindings.Bindings import StoreGate
        self._detStore = StoreGate.pointer("DetectorStore")
        if self._detStore is None:
            self.msg.error("Failed to get DetectorStore")
            return StatusCode.Failure
        # Get LArOnlineID helper class
        self.onlineID=self._detStore.retrieve("LArOnlineID","LArOnlineID")
        if self.onlineID is None:
            self.msg.error("Failed to get LArOnlineID")
            return StatusCode.Failure
        # Get CaloCell_ID
        self.offlineID=self._detStore.retrieve("CaloCell_ID","CaloCell_ID")
        if self.offlineID is None:
            self.msg.error("Failed to get CaloCell_ID")
            return StatusCode.Failure
        # Get OnlineID_SuperCell    
        self.onlineID_SC=self._detStore.retrieve("LArOnline_SuperCellID","LArOnline_SuperCellID")
        if self.onlineID_SC is None:
            self.msg.error("Failed to get LArOnline_SuperCellID")
            return StatusCode.Failure

        # -----------------------------------------------------------
        # Initialize LArCabling service
        self.larCablingSvc=PyAthena.py_tool("LArCablingService")
        if self.larCablingSvc is None:
            self.msg.error('Problem retrieving LArCablingService pointer !')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved LArCablingService')

        self.LArHVMapTool=PyAthena.py_tool("LArHVMapTool")
        if self.LArHVMapTool is None:
            self.msg.error('Problem retrieving LArHVMapTool')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.LArHVMapTool.name())
 
        self.LArHVCablingTool=PyAthena.py_tool("LArHVCablingTool")
        if self.LArHVCablingTool is None:
            self.msg.error('Problem retrieving LArHVCablingTool')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.LArHVCablingTool.name())
 
        self.CaloTriggerTowerService=PyAthena.py_tool("CaloTriggerTowerService")
        if self.CaloTriggerTowerService is None:
            self.msg.error('Problem retrieving CaloTriggerTowerService pointer')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.CaloTriggerTowerService.name())

        self.CaloSuperCellIDTool=PyAthena.py_tool("CaloSuperCellIDTool",iface='ICaloSuperCellIDTool')
        if self.CaloSuperCellIDTool is None:
            self.msg.error('Problem retrieving CaloSuperCellIDTool')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.CaloSuperCellIDTool.name())

        self.CaloSuperCellCablingTool=PyAthena.py_tool("LArSuperCellCablingTool",iface='LArSuperCellCablingTool')
        #self.CaloSuperCellCablingTool=PyAthena.py_tool("LArSuperCellCablingTool")
        if self.CaloSuperCellCablingTool is None:
            self.msg.error('Problem retrieving LArSuperCellCablingTool')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.CaloSuperCellCablingTool.name())

        """
        self.badChannelTool=PyAthena.py_tool("LArBadChanTool",iface="ILArBadChanTool")
        if self.badChannelTool is None:
            self.msg.error('Problem retrieving LArBadChanTool pointer !')
            return StatusCode.Failure
        else:
            self.msg.info('retrieved [%s]', self.badChannelTool.name())

        self.class_larBCBitPacking=PyLCGDict.makeClass("LArBadChanBitPacking")
        self.bc_packing=self.class_larBCBitPacking()

        f = ServiceMgr.IOVDbSvc.dbConnection.getFolder('/LAR/BadChannelsOfl/BadChannels')
        tag1 = f.resolveTag('COMCOND-BLKPST-004-01')
        objs = f.browseObjects(cool.ValidityKeyMin,cool.ValidityKeyMax,cool.ChannelSelection.all(),tag1)

        for obj in objs:
          print obj.since()>>32,obj.until()>>32,obj.channelId()

        import sys
        sys.exit()
        """

        try:
          self.caloDDM = CaloDetDescrManager.instance()
        except:
          print "Failed to retrieve CaloDDM"
          return StatusCode.Failure
 
        return StatusCode.Success
        
    def execute(self):
        self.msg.info('running execute...')

        # initialize the LArCablingSvc
        #self.larCablingSvc.isLArOnOffIdValid()
        
        try:
          self.larhvScaleCorr=self._detStore.retrieve("ILArHVScaleCorr","LArHVScaleCorr")
        except Exception,e:
          print "WARNING: Failed to retrieve LArHVScaleCorr from DetStore"
          print e
          self.larhvScaleCorr=None
               
        if self.nEvts==0:
          # Process one 'dummy' event to make sure all DB connections get closed
          self.nEvts+=1
          self.LArHVCablingTool.getLArHVLineID(self.larCablingSvc.createSignalChannelIDFromHash(IdentifierHash(0)))
          return StatusCode.Success

        self.onlineID.set_do_checks(True)
        self.offlineID.set_do_checks(True)
       
        ######################################################
        import os,sqlite3,math
        if os.path.exists('./LArId_tmp.db'): os.system('rm -f ./LArId_tmp.db')
        conn = sqlite3.connect('./LArId_tmp.db')
        c = conn.cursor()

        c.execute('''create table LARID (
          OFF_ID INTEGER,
          ONL_ID INTEGER PRIMARY KEY,
          DET SMALLINTEGER,
          AC SMALLINTEGER, 
          FT SMALLINTEGER,
          SL SMALLINTEGER,
          CH SMALLINTEGER,
          SAM SMALLINTEGER,
          ETA FLOAT,
          PHI FLOAT,
          HVLINES TEXT,
          HVCORR FLOAT,
          CL TEXT,
          TT_COOL_ID INTEGER,
          SC_OFFL_ID INTEGER,
          SC_ONL_ID INTEGER,
          IETA SMALLINTEGER,
          IPHI SMALLINTEGER,
          R FLOAT,
          Z FLOAT,
          FTNAME TEXT,
          P1_ID TEXT,
          FEB TEXT,
          DSP TEXT,
          PU TEXT,
          ROL TEXT,
          ROD TEXT,
          ROS_ROL TEXT,
          FEB_ID INTEGER
        )''')

        c.execute('''create index LARID_index ON LARID(
	OFF_ID,
	ONL_ID,
	DET,
	AC,
	FT,
	SL,
	CH,
	SAM,
	ETA,
	PHI,
	HVLINES,
	HVCORR,
	CL,
	TT_COOL_ID,
        SC_OFFL_ID,
        SC_ONL_ID,
	IETA,
	IPHI,
	R,
	Z,
	FTNAME,
	P1_ID,
	FEB,
	DSP,
	PU,
	ROL,
	ROD,
	ROS_ROL,
	FEB_ID
	)''')

        cmd='insert into LARID values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)'

        febtypes = {0:['','PS','F0','F1','F2','F3','F4','F5','F6','B0','B1','M0','M1','M2','M3','- '],
                    1:['','PS','F0','F1','F2','F3','F4','F5','B0','B1','M0','M1','M2','M3','- ','- '],
                    2:['','I1','I2','-','-' ,'L1','L2','M1','M2','H1','H2','-','-','-','-','-'],
                    3:['','F1,00','F1,01','F1,02','F1,03','F1,04','F1,05','F1,06','-','F1,07','F2,00','F2,01','F2,02','F2,03','F3,00','F3,01'],
                    4:['',"PS", "F0", "M0", "M1", "F1", "F2", "F3", "F4", "B0", "M2", "M3", "F5", "B1", "M4", "M5"] }

        for idH in range(self.offlineID.calo_cell_hash_max()):
          if idH%10000==0: print idH
          idHash = IdentifierHash(idH) 
          chid=self.larCablingSvc.createSignalChannelIDFromHash(idHash)
          id = self.larCablingSvc.cnvToIdentifier(chid)  
          if self.offlineID.is_lar(id):
            OFF_ID = int(id.get_compact()>>32)
            ONL_ID = int(chid.get_compact()>>32)
            #TT_OFF_ID = self.CaloTriggerTowerService.whichTTID(id).get_compact()
            TTOnlineID = self.CaloTriggerTowerService.whichTTChannelID(chid)
            #TT_ONL_ID = TTOnlineID.get_compact()
            TT_COOL_ID = int((self.CaloTriggerTowerService.createL1CoolChannelId(TTOnlineID)).id())
            SC_OFFL_ID = int(self.CaloSuperCellIDTool.offlineToSuperCellID(id).get_compact()>>32)
            #SC_ONL_ID=int(self.CaloSuperCellCablingTool.createSignalChannelID(id).get_compact()>>32)
            SC_ONL_ID=int(self.CaloSuperCellCablingTool.createSignalChannelID(self.CaloSuperCellIDTool.offlineToSuperCellID(id)).get_compact()>>32)
            DET = self.offlineID.calo_sample(id)
            if DET<4: DET=0
            elif DET<8: DET=1
            elif DET<12: DET=2
            else: DET=3
            AC = self.offlineID.pos_neg(id)
            if AC>0: AC=1
            else:    AC=-1
            #REG = self.offlineID.region(id)
            #if REG>10: REG=9
            FT = self.onlineID.feedthrough(chid)
            SL = self.onlineID.slot(chid)
            CH = self.onlineID.channel(chid)
            IETA = self.offlineID.eta(id)
            IPHI = self.offlineID.phi(id)
            DDE = self.caloDDM.get_element(idHash)
            #ETAF = DDE.eta()
            #PHIF = DDE.phi()
            SAM = self.offlineID.sampling(id)
            ETA = DDE.eta_raw()
            PHI = DDE.phi_raw()
            X = DDE.x()
            Y = DDE.y()
            R = math.sqrt(X*X+Y*Y)  
            Z = DDE.z()
            #STATUS = self.badChannelTool.status(chid).packedData()

            # EXPERT INFO
            hv = self.LArHVMapTool.GetHVLines(id)
            HVLINES = ''
            for i in hv: HVLINES += '%06d ' % (i)
            if HVLINES=='': print DET,AC,FT,SL,CH,SAM
            HVLINES = HVLINES.rstrip(' ')
            CLs = []
            CL = self.larCablingSvc.calibSlotLine(chid)
            for cl in CL: CLs.append( self.onlineID.channel(cl) )
            CLs.sort()
            CL = ''
            for cl in CLs: CL += str(cl)+' '
            CL = CL.rstrip(' ')
            HVCORR = self.larhvScaleCorr.HVScaleCorr(chid)

            FTNAME = self.onlineID.feedthrough_name(chid) 
            P1_ID = FTNAME[1:]+'_'
            if DET<1: P1_ID += febtypes[0][SL] # EMB
            else:
              if FT in [3,10,16,22]:
                if SL>4 and SL<11: P1_ID += febtypes[2][SL] # HEC
                else: P1_ID += febtypes[2][SL]  # EMEC
              elif FT==6: P1_ID += febtypes[3][SL] # FCAL 
              else:
                if FT in [2,9,15,21]: P1_ID += febtypes[4][SL] # EMEC SPC
                else: P1_ID += febtypes[1][SL] # EMEC STD    
           
            A = [OFF_ID,ONL_ID,DET,AC,FT,SL,CH,SAM,ETA,PHI,HVLINES,HVCORR,CL,TT_COOL_ID,SC_OFFL_ID,SC_ONL_ID,IETA,IPHI,R,Z,FTNAME,P1_ID]
            BE = 0
            if DET>0: BE = 1
            PN = 0
            if AC>0: PN = 1
            FEB_ID = getONL(BE,PN,FT,SL)
            for i in ONL2IDs[FEB_ID]: A.append(i)
            A.append(FEB_ID)
            c.execute(cmd,tuple(A))

        conn.commit()
        c.close()

        return StatusCode.Success
