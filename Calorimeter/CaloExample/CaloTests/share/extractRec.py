#!/usr/bin/env python


import os, sys, commands,getopt, datetime
import stat,logging


class extractRec:

    def __init__(self,log,logLevel):   

        
        self.loggerAlert = logging.getLogger('Alert')
        self.hdlrAlert = logging.FileHandler('Alert.txt',"w")
        formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
        self.hdlrAlert.setFormatter(formatter)
        self.loggerAlert.addHandler(self.hdlrAlert)
        self.loggerAlert.setLevel(logLevel)

        if log=='':
            self.loggerInfo = logging.getLogger('Info')
            self.hdlrInfo = logging.FileHandler('Info.txt',"w")
            formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
            self.hdlrInfo.setFormatter(formatter)
            self.loggerInfo.addHandler(self.hdlrInfo)
            self.loggerInfo.setLevel(logLevel)
        else:
            self.loggerInfo = log
        
        self.tests=["Photons_calib0_csc11_007062_singlepart_gamma_E50","Electrons_calib0_csc11_007070_singlepart_e_E5","Photons_calib0_csc11_007080_singlepart_gamma_E5","Photons_calib0_csc11_007085_singlepart_gamma_E500","top_calib1_csc11_005200_T1_McAtNlo_Jimmy","Electrons_calib1_csc11_007061_singlepart_e_E100","Photons_calib1_csc11_007063_singlepart_gamma_E100"]
        
        # from auditors svc
        self.CaloList=['LAr37ClusterMaker','LAr33ClusterMake','LArSWClusterMaker','LAr35ClusterMaker','CBNT_TileCell','CaloSWClusterMaker','CBNT_CaloCluster','CBNT_CaloCluster37','CBNT_CaloInfo','CBNT_CaloCluster35','CaloCellMaker','CaloTopoClusterMaker','TruthCone4Jets','TruthConeJets','CBNT_LArCell','CaloCell2TopoClusterMapper','EMTopoClusterMaker','TruthKtJets','TowerMaker','KtJets','ConeJets','Cone4Jets','METAlg']
        # from personal chronosvc
        self.extraCaloList=['CmbTowerBldr.TileCmbTwrBldr','CaloCellMaker_CaloCellContainerChecke','CaloCellMaker_CaloCellContainerFinali','CmbTowerBldr.FCalCmbTwrBldr','CaloCellMaker_LArCellBuilderLAr','CaloCellMaker_TileCellBuilder','CmbTowerBldr.LArCmbTwrBldr','TowerMaker.LArTowerBuilder','CmbTowerBldr.LArCmbTwrBldrChronoCellLoop','CmbTowerBldr.LArCmbTwrBldrChronobuild','TowerMaker.LArTowerBuilderChronobuild','TowerMaker.LArTowerBuilderChronoCellLoop']
        self.debugLevel=0
        # in sec
        self.diffTime=0.05
        
        #self.emails=['bernardet@cppm.in2p3.fr']
        self.emails=[]
        self.wwwemails="http://cern.ch/CaloRTT/CaloRecEx_mails2.txt"
        self.testName="CaloRecEx"
        # if type = 1 use of the list above for the algo
        self.type=0
        self.typePerso=0
        # Size of RDO.pool.root = XXX ko         


        # release, time info
        self.release=""
        self.branch=""
        self.date=""

        self.mailSubjectProblem=""
        self.mailSubjectReport=""
        #self.switchoffRTTpart=True
        #####################
        self.alertMess=""
        self.alertMessOk=False
        self.alertReportOk=True
        self.checkFile=0



            
####  CPU Time

    def extractCPUTimeInitializeAll(self,file,type):

        tmp=[]
        if type !=1:
            com="grep \"Time User\" " + file + " | grep initi"

            self.loggerInfo.debug(com)
        
            res=commands.getoutput(com)
            tmp= res.splitlines() 
        else:
            for i in self.CaloList:
                com="grep \"Time User\" " + file + " | grep \"\<" +i+":initi\""
                self.loggerInfo.debug(com)
                res=commands.getoutput(com)
                tmp += res.splitlines()
            
        return tmp
            
    def extractCPUTimeInitializeAlgo(self,file, algo):

        com="grep \"Time User\" " + file + " | grep initi" + " | grep  " + algo +" | awk '{print $7" "$8}' "

        self.loggerInfo.debug(com)
        
        res=commands.getoutput(com)    
        res2= res.splitlines() 
        return res2

    def extractCPUTimeExecuteAll(self,file,type):
    
        tmp=[]
        if type !=1:
            com="grep \"Time User\" " + file + " | grep execute"

            self.loggerInfo.debug(com)
        
            res=commands.getoutput(com)    
            tmp= res.splitlines() 
        else:
            for i in self.CaloList:
                com="grep \"Time User\" " + file + " | grep \"\<" +i
                self.loggerInfo.debug(com)
                res=commands.getoutput(com)
                tmp += res.splitlines()

        return tmp

# pour les algo perso, ajouter un espace apres le nom de l algo pour le grep sinon pb en cas de noms du genre : AlgoPerso,AlgoPerso2 ...


    def extractCPUTimeExecutePersoAll(self,file,type):
    
        tmp=[]
        if type !=1:
            com="grep \"Time User\" " + file + " | grep -v execute| grep -v initialize | grep -v finalize"

            self.loggerInfo.debug(com)
        
            res=commands.getoutput(com)    
            tmp= res.splitlines() 
        else:
            for i in self.extraCaloList:
                com="grep \"Time User\" " + file + " | grep '" +i+" '" 
                self.loggerInfo.debug(com)
                res=commands.getoutput(com)
                tmp += res.splitlines()

        return tmp


    def extractCPUTimeExecuteAlgo(self,file, algo):

        com="grep \"Time User\" " + file + " | grep  '\<" + algo +"' | awk '{print $7" "$8}' "
        self.loggerInfo.debug(com)
        res=commands.getoutput(com)    
        res2= res.splitlines() 
        return res2

    def extractCPUTimeExecutePersoAlgo(self,file, algo):

        com="grep \"Time User\" " + file + " | grep  '\<" + algo +" ' | awk '{print $7" "$8}' "

        self.loggerInfo.debug(com)
        
        res=commands.getoutput(com)

        self.loggerInfo.debug(res)

        res2= res.splitlines() 
        return res2


    def extractNbEventsExecuteAlgo(self,file, algo):

        com="grep \"Time User\" " + file + " | grep  '\<" + algo +"'| cut -d\# -f 2 | cut -d = -f 2 "

        self.loggerInfo.debug(com)
        
        res=commands.getoutput(com)    
        res2= res.splitlines() 
        return res2

    def extractNbEventsExecutePersoAlgo(self,file, algo):

        com="grep \"Time User\" " + file + " | grep  '\<" + algo +" '| cut -d\# -f 2 | cut -d = -f 2 "

        self.loggerInfo.debug(com)
        
        res=commands.getoutput(com)    
        res2= res.splitlines() 
        return res2


    def getNbEvents(self,file):

        com="grep \"INFO Total Virtual memory\" " + file +" | wc -l"

        self.loggerInfo.debug(com)
        
        res=commands.getoutput(com)    
        res2= res.splitlines() 
        return res2

    def compareCPUTimeInitialize(self,file,reffile,type):
        # 
        CPUTimeRef=[]
        alg=[]
        CPUTimeRef=self.extractCPUTimeInitializeAll(reffile,type)
        for i in CPUTimeRef:
            t=i.lstrip()
            tmp=t.split(" ")
            alg.append(tmp[0])
        #print alg
        for j in alg:
            tref=self.extractCPUTimeInitializeAlgo(reffile,str(j))
            tnew=self.extractCPUTimeInitializeAlgo(file,str(j))
            if len(tnew)==0:
                    mess=j+" not found in " + file
                    self.loggerInfo.info(mess)
                    continue

            #print j,tref,tnew
            trefUnity=self.CPUTimeUnity(str(tref[0]))
            tnewUnity=self.CPUTimeUnity(str(tnew[0]))
            #print j,trefUnity,tnewUnity
            ttnew=tnew[0].split('[')
            ttref=tref[0].split('[')
            ttref2=self.convertCPUTime(float(ttref[0]),trefUnity)      
            ttnew2=self.convertCPUTime(float(ttnew[0]),tnewUnity)
            #print j,ttref2,ttnew2
            if ttnew2-ttref2 > self.diffTime:
                mess="WARNING : "+j+" REF : "+str(ttref[0])+self.CPUTimeUnity(str(tref[0]))+" NOW : "+str(ttnew[0])+self.CPUTimeUnity(str(tnew[0]))
                self.loggerInfo.info(mess)
                self.loggerAlert.info(mess)
            else:
                mess="GOOD : "+j+" REF : "+str(ttref[0])+self.CPUTimeUnity(str(tref[0]))+" NOW : "+str(ttnew[0])+self.CPUTimeUnity(str(tref[0]))
                self.loggerInfo.info(mess)
            
            
        return

      
    def convertCPUTime(self,time,unity):
       
        if unity=="min":
            return time*60
        elif unity=="ms": 
            return time*0.001
        elif unity=="us":
            return time*0.000001
        else:
            return time

    def CPUTimeUnity(self,string):
        
        tmp=string.split('[')
        tmp2=tmp[1].split(']')
        return tmp2[0]

    def compareCPUTimeExecute(self,file,reffile,type):
        # 
        CPUTimeRef=[]
        alg=[]
        CPUTimeRef=self.extractCPUTimeExecuteAll(reffile,type)
        for i in CPUTimeRef:
            t=i.lstrip()
            tmp=t.split(" ")
            alg.append(tmp[0])
        for j in alg: 
                tref=self.extractCPUTimeExecuteAlgo(reffile,str(j))
                nbEvtRef=self.extractNbEventsExecuteAlgo(reffile,str(j))
                tnew=self.extractCPUTimeExecuteAlgo(file,str(j))
                nbEvtNew=self.extractNbEventsExecuteAlgo(file,str(j))
                #print j," ", tref,"  ",tnew
                #print 'KARIM2 ',j," ",nbEvtRef ,"  ",nbEvtNew
                if len(tnew)==0:
                    mess=j+" not found in " + file
                    self.loggerInfo.info(mess)
                    continue
                
                # convert en secondes
                trefUnity=self.CPUTimeUnity(str(tref[0]))
                tnewUnity=self.CPUTimeUnity(str(tnew[0]))
                ttnew=tnew[0].split('[')
                ttref=tref[0].split('[')
                ttref2=self.convertCPUTime(float(ttref[0]),trefUnity)      
                ttnew2=self.convertCPUTime(float(ttnew[0]),tnewUnity)

                if int(nbEvtRef[0])>0:
                    normtref=float(ttref2)/float(nbEvtRef[0])
                else:
                    self.loggerInfo.warning("Ref "+j+" "+nbEvtRef[0])
                if int(nbEvtNew[0])>0:
                    normtnew=float(ttnew2)/float(nbEvtNew[0])
                else:
                    self.loggerInfo.warning("New "+j+" "+nbEvtNew[0])

                if int(nbEvtRef[0])>0 and int(nbEvtNew[0])>0: 
                    if normtnew-normtref > self.diffTime:
                        mess="WARNING : "+j+" REF : "+str(normtref)+" s "+" NOW : "+str(normtnew)+" s "+ "  (per event)"
                        self.loggerInfo.info(mess)
                        # alert
                        self.loggerAlert.info(mess)
                    else:
                        mess="GOOD : "+j+" REF : "+str(normtref)+" s "+" NOW : "+str(normtnew)+" s"+" (per event)"
                        self.loggerInfo.info(mess)
                else:
                   mess="WARNING : can not check "+j
                   self.loggerInfo.info(mess) 
        return


    def compareCPUTimeExecutePerso(self,file,reffile,type):
        # 
        CPUTimeRef=[]
        alg=[]
        #CPUTimeRef=self.extractCPUTimeExecutePersoAll(reffile,type)
        CPUTimeRef=self.extractCPUTimeExecutePersoAll(file,type)
        
        for i in CPUTimeRef:
            t=i.lstrip()
            tmp=t.split(" ")
            alg.append(tmp[0])
        for j in alg: 
                tref=self.extractCPUTimeExecutePersoAlgo(reffile,str(j))
                
                nbEvtRef=self.extractNbEventsExecutePersoAlgo(reffile,str(j))
                tnew=self.extractCPUTimeExecutePersoAlgo(file,str(j))
                nbEvtNew=self.extractNbEventsExecutePersoAlgo(file,str(j))
                isok=1
                if len(tnew)==0:
                    mess=j+" not found in " + file
                    self.loggerInfo.info(mess)
                    tmp=tref[0].split('[')
                    mess=j+" REF : "+str(tmp[0])+self.CPUTimeUnity(str(tref[0]))
                    self.loggerInfo.info(mess)
                    isok=0
                if len(tref)==0:
                    mess=j+" not found in " + reffile
                    self.loggerInfo.info(mess)
                    tmp=tnew[0].split('[')
                    mess=j+ " NEW : "+str(tmp[0])+self.CPUTimeUnity(str(tnew[0]))
                    self.loggerInfo.info(mess)
                    isok=0

                if isok==0:
                    continue
                    
                # print j," ", tref,"  ",tnew
                # print j," ",nbEvtRefb ,"  ",nbEvtNewb
                
                # convert en secondes
                trefUnity=self.CPUTimeUnity(str(tref[0]))
                tnewUnity=self.CPUTimeUnity(str(tnew[0]))
                ttnew=tnew[0].split('[')
                ttref=tref[0].split('[')
                ttref2=self.convertCPUTime(float(ttref[0]),trefUnity)      
                ttnew2=self.convertCPUTime(float(ttnew[0]),tnewUnity)
                normtref=float(ttref2)/float(nbEvtRef[0])
                normtnew=float(ttnew2)/float(nbEvtNew[0])
            
                if normtnew - normtref > self.diffTime:
                    mess="WARNING : "+j+" REF : "+str(normtref)+" s "+" NOW : "+str(normtnew)+" s "+ "  (per event)"
                    self.loggerInfo.info(mess)
                    self.loggerAlert.info(mess)
                else:
                    mess="GOOD : "+j+" REF : "+str(normtref)+" s "+" NOW : "+str(normtnew)+" s"+" (per event)"
                    self.loggerInfo.info(mess)
        return

    def logExtract(self,file,chain):

        com="egrep \""+chain + "\"  "  + file
        self.loggerInfo.debug(com)
        
        res=commands.getoutput(com)    
        res2= res.splitlines() 
        return res2


    def extractLog(self,log):


        com="wc -l "+log +" | awk '{print $1}'"
        res=commands.getoutput(com)    
        res2=res.splitlines()
        nbAllLines=int(res2[0])

        #print nbAllLines
        
        com="grep ^BEGIN_RTTINFO "+log
        res=commands.getoutput(com)    
        res2=res.splitlines()
        
        names=[]
        for i in res2:
            name=i.split('=')
            name[1]=name[1].strip()
            names.append(name[1])
            
        for i in range(len(names)):
            #print i,names[i]
            #com="grep -n BEGIN_RTTINFO="+names[i]+" "+log+" | cut -d: -f1"
            com="grep -n \"^BEGIN_RTTINFO = "+names[i]+"\" "+log+"| cut -d: -f1"
            n1=commands.getoutput(com)
            try:
                #com="grep -n BEGIN_RTTINFO="+names[i+1]+" "+log+" | cut -d: -f1"
                com="grep -n \"^BEGIN_RTTINFO = "+names[i+1]+"\" "+log+"| cut -d: -f1"
                
                n2=commands.getoutput(com)
            except:
                n2=nbAllLines
            
            nblines=int(n2)-int(n1)-1
            #print nblines
            newlog='./'+names[i]+".log"
            #com="grep -A"+str(nblines)+" BEGIN_RTTINFO="+names[i]+" "+log +" >"+newlog
            com="grep -A"+str(nblines)+" \"^BEGIN_RTTINFO = "+names[i]+"\" "+log +" >"+newlog
            os.system(com)
        return names


###########

    def run(self):
        log=commands.getoutput("ls Calo*Ex_RTT*_log")
        names=self.extractLog(log)
	
        print names

        for test in names:
            com="rm "+"./resultats.txt"
            commands.getoutput(com)    

            com = "cat MYRTTINFOS.txt|grep BEGIN_RTTINFO | cut -d= -f2| tail -n1"
            jobopt=commands.getoutput(com)
            self.loggerInfo.info('Test name : '+jobopt)
            found=0
            for opt in self.tests:
                if opt==jobopt.strip():
                    found=1
                    break	

            if found==0:
                self.loggerInfo.info("test not in dict !!!")
                return -1

            self.reflog='ref'+jobopt.strip()+'.log'
            com='wget http://cern.ch/CaloRTT/'+self.reflog
            print com
            os.system(com)

            try:
                file = open(self.reflog)
                file.close()
            except IOError:
                self.loggerInfo.info(self.reflog+' not found')
                return -1

            # date
            self.date=str(datetime.datetime.now().strftime("%Y-%m-%d %X"))
            self.loggerInfo.info(self.date)

            com = "cat env.log|grep AtlasVersion | cut -d= -f2| tail -n1"
            self.release=commands.getoutput(com)
            self.branch="Dev"

            self.mailSubjectProblem="\"[RTT test, "+jobopt+","+self.branch+","+self.release+"] : extractRec problem \""
            self.mailSubjectReport="\"[RTT test, "+jobopt+","+self.branch+","+self.release+"] : extractRec report \""

            mess="Reference log = " + self.reflog
            self.loggerInfo.info(mess)
            mess="Log to check = "+log
            self.loggerInfo.info(mess)

     
            self.loggerInfo.info("********************************")
            self.loggerInfo.info("CHECK STATUSCODE")
            
            res=self.logExtract(log,"\|*\| lib*")
            for i in res:
                self.loggerInfo.info(str(i))     
            self.loggerInfo.info("********************************")


            ## self.loggerInfo.info("********************************")
##             self.loggerInfo.info("CHECK WARNING")
            
##             res=self.logExtract(log," WARNING ")
##             for i in res:
##                 self.loggerInfo.info(str(i))     
##             self.loggerInfo.info("********************************")
            
            
##             self.loggerInfo.info("********************************")
##             self.loggerInfo.info("CHECK ERROR")
##             res=self.logExtract(log," ERROR ")
##             for i in res:
##                 self.loggerInfo.info(str(i))     
##             self.loggerInfo.info("********************************")
            

            
            self.loggerInfo.info("********************************")
            
            self.loggerInfo.info("CHECK INITIALIZE TIME")
            
            self.compareCPUTimeInitialize(log,self.reflog,self.type)
            
            self.loggerInfo.info("********************************")
            
            self.loggerInfo.info("CHECK EXECUTE TIME")
            
            self.compareCPUTimeExecute(log,self.reflog,self.type)
            self.compareCPUTimeExecutePerso(log,self.reflog,self.typePerso)
            
            self.loggerInfo.info("********************************")

            return 0
#a=extractRec()
#a.run()
