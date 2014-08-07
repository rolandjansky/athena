# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class readconfig:
    def __init__(self,confile):
        com=open(confile).readlines()
        for i in range(0,com.count('\n')):
            com.remove('\n')
        self.myDict={}
        self.off1=123456
        self.off2=2345678
        for ll in com:
            # split at the first "="
            nn=ll.find("=")
            if nn != -1:
                lft=ll[0:nn].lstrip(' ')
                rgt=ll[nn+1:len(ll)].rstrip('<BR>\n')
                # Now do the crazy modifications
                for i in range(0,len(lft.split(";"))):
                    lft1=lft.split(";")[i]
                    rgt1=rgt.split(";")[i]
                    self.myDict[lft1]=self.pavelConvert(rgt1)
        print self.myDict   
    def pavelConvert(self,cmd):
        cmd=cmd.replace(" ",",")
        cmd=cmd.replace("%3B",";")        
        return cmd

    def setDigitOffset(self,off1=123456,off2=2345678):
        # To set digitization digits offset, untouched in testing
        self.off1=off1
        self.off2=off2
 
    def setEventNumber(self,evnt):
        self.myDict["events per job"]=evnt

    def listTrfs(self):
        allowedTrfs=['Reco_trf.py','Digi_trf.py','csc_digi_trf.py']
        trfs=self.myDict["transfromation"].split(",")
        goodTrfs=[]
        for trf in trfs:
            if trf in allowedTrfs:
                goodTrfs.append(trf)
        return goodTrfs
      
    def outputFormats(self):
        return self.myDict["formats"].split(",")

    def DBRelease(self):
        return "DBRelease-"+self.myDict["DBRelease"]+".tar.gz"
 
    def eventsPerJob(self):   
        return int(self.myDict["events per job"])

    def readDict(self,tag):
        if tag in self.myDict.keys():
            return self.myDict[tag]
        else:
            return None

    def digitTrf(self,myInput,myOutput="output.TMP.RDO.pool.root"):
        if 'Digi_trf.py' not in self.listTrfs():
            return 'ERROR'
        myDict=self.myDict
        # Here we reverse engineer the trf:
        allSet=[]
        # input file:
        allSet.append(" inputHitsFile="+myInput)
        # output file
        allSet.append(" outputRDOFile="+myOutput)
        # number of events
        allSet.append(" maxEvents="+myDict["events per job"])
        #Events to skip (set to zero for digi/reco)
        allSet.append(" skipEvents=0")
        #Geometry version
        allSet.append(" geometryVersion="+myDict["geometryVersion"])
        print 'appended geometry versin in digitTrf'
        # Digit offset
        allSet.append(" digiSeedOffset1="+str(self.off1)+" digiSeedOffset2="+str(self.off2))
        # MB input (assume no pileup for the time being!)
        if myDict["minbiasHits"] != "NONE":
            allSet.append(" minbiasHitsFile="+myDict["minbiasHits"])

        # job Config
#        allSet.append(" JobConfig="+myDict["JobConfig"])
        # Trigger config (set to None for digitisation)
        allSet.append(" triggerConfig=DEFAULT")
        # DB release
        allSet.append(" DBRelease=DBRelease-"+myDict["DBRelease"]+".tar.gz")
        # Conditions
        allSet.append(" conditionsTag="+myDict["conditionsTag"])
        # Random service
#        allSet.append(" digiRndmSvc="+myDict["DigiRndmSvc"])
        # sampling db
 #       allSet.append(" samplingFractionDbTag="+myDict["SamplingFractionDbTag"])
        # Calo Digit setting
#        allSet.append(" AddCaloDigi="+myDict["AddCaloDigi"])
        # noise setting
#        allSet.append(" doAllNoise="+myDict["NoiseControl"])
        # athena options and ignore errors, later please
        digicmd="Digi_trf.py "
        for settg in allSet:
            digicmd += settg
        return digicmd

    def recoTrf(self,myInput,myOutput="output.TMP",myRN="000000"):
        if 'Reco_trf.py' not in self.listTrfs():
            return 'ERROR'
        myDict=self.myDict
        # Here we reverse engineer the trf:
        allSet=[]
        # Pre include
#        if myDict["preInclude"] != "NONE":
#            allSet.append(" preInclude="+myDict["preInclude"])
        # Pre exec
#        if myDict["preExec"] != "NONE":
#            allSet.append(" preExec='"+myDict["preExec"]+"'")
        # Post include
#        if myDict["postInclude"] != "NONE":
#            allSet.append(" postInclude="+myDict["postInclude"])
        # Post Exec
#        if myDict["postExec"] != "NONE":
#            allSet.append(" postExec='"+myDict["postExec"]+"'")    
        # input file:
        allSet.append(" inputRDOFile="+myInput)
        # run number:
        allSet.append(" RunNumber="+myRN)
        # OutputESD file
        if "ESD" in self.outputFormats():
            allSet.append(" outputESDFile="+myOutput+".ESD.pool.root")
        # OutputAOD file
        if "AOD" in self.outputFormats():
            allSet.append(" outputAODFile="+myOutput+".AOD.pool.root")
        # Output HIST file
        if "HIST" in self.outputFormats():
            allSet.append(" HIST="+myOutput+".HIST.root")
        # Output NTUP_PIXELCALIB file
        if "NTUP_PIXELCALIB" in self.outputFormats():
            allSet.append(" outputPixelCalibNtup="+myOutput+".NTUP_PIXELCALIB.root")
        # Output NTUP_MUONCALIB file
        if "NTUP_MUONCALIB" in self.outputFormats():
            allSet.append(" outputMuonCalibNtup="+myOutput+".NTUP_MUONCALIB.root")
        # Output TAG_COMM file
        if "TAG_COMM" in self.outputFormats():
            allSet.append(" outputTAGComm="+myOutput+".TAG_COMM.pool.root")
        # Output NTUP_TRIG file
        if "NTUP_TRIG" in self.outputFormats():
            allSet.append(" outputNTUP_TRIG="+myOutput+".NTUP_TRIG.root")

        allSet.append(" autoConfiguration='everything'")
        # number of events
        allSet.append(" maxEvents="+myDict["events per job"])
        #Events to skip (set to zero for digi/reco)
        allSet.append(" skipEvents=0")
        #Geometry version
#        allSet.append(" geometryVersion="+myDict["geometryVersion"])
        #AMI tag
#        allSet.append(" AMITag="+myDict["AMITag"])
        # Trigger config
#        allSet.append(" triggerConfig='"+myDict["triggerConfig"]+"'")
        # DB release
#        allSet.append(" DBRelease=DBRelease-"+myDict["DBRelease"]+".tar.gz")
        # Conditions
#        allSet.append(" conditionsTag="+myDict["conditionsTag"])
        #ignore
#        allSet.append(" --ignoreerrors="+myDict["--ignoreerrors"])
        # autoconfiguration
        if "autoConfiguration" in myDict.keys():
            allSet.append(" autoConfiguration="+myDict["autoConfiguration"])

        recocmd='Reco_trf.py'
        for settg in allSet:
            recocmd += settg
        return recocmd

