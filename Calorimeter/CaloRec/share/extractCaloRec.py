import os, sys, getopt, datetime
#import os, sys, getopt

from future import standard_library
standard_library import aliases()
import subprocess


class LogClass:
    """
    Classe pour rediriger les sorties standards vers un fichier
    et eventuellement à l'ecran.
    Les arguments sont :
    - 'nomfich.txt' : le nom du fichier de sortie
    - DupStd : execute une copie à l'ecran de la sortie.
      Les valeurs autorisees sont :
      1 ou 2: activation de la copie a l'ecran (defaut)
      0 : absence de copie a l'ecran
    """
    def __init__ (self, Filename=None, DupStd=1):
        self.mess2=""
        if (Filename != None):
            try: self._file = open(Filename,"a")
            except: self._file = open(Filename,"w")
        else:
            self._file = sys.stdout
        if DupStd:
            self._dupstd = sys.stdout
        else:
            self._dupstd = None

    def write(self, Message):
        message = ""
        message = message + Message
        self.mess2=self.mess2+Message
        self._file.write  (message)
        self._file.flush ()
        if self._dupstd:
            self._dupstd.write(message)
            self._dupstd.flush()

    def log(self, Message):
        self.write(Message)
        self.write("\n")
        #if self._dupstd:
        #    self._dupstd.write(Message)
        #    self._dupstd.write("\n")

    def close(self):
        self._file.close()

    def __del__(self):
        self._file.close()

class extract:

    def __init__(self,paramDict={}):
        self.dict =paramDict
        self.logDict={'1103_T1_McAtNLO_top':'reflogRec1103_T1_McAtNLO_top','1002_H_2e2mu_q02dig_1004':'reflogRec1002_H_2e2mu_q02dig_1004','1002_H_2e2mu_q02dig_1004ESDAOD':'reflogRecESDAOD1002_H_2e2mu_q02dig_1004','1004_Photons_100GeV':'reflogRec1004_Photons_100GeV','1050_H_2e2mu_q02dig_1004':'reflogRec1050_H_2e2mu_q02dig_1004','1050_H_2e2mu_q02dig_1004ESDAOD':'reflogRecESDAOD1050_H_2e2mu_q02dig_1004','1103_T1_McAtNLO_topESDAOD':'reflogRecESDAOD1103_T1_McAtNLO_top'}
        
        # from auditors svc
        self.CaloList=['LAr37ClusterMaker','LAr33ClusterMake','LArSWClusterMaker','LAr35ClusterMaker','CaloSWClusterMaker','CaloCellMaker','CaloTopoClusterMaker','TruthCone4Jets','TruthConeJets','CaloCell2TopoClusterMapper','EMTopoClusterMaker','TruthKtJets','TowerMaker','KtJets','ConeJets','Cone4Jets','METAlg']
        # from personal chronosvc
        self.extraCaloList=['CmbTowerBldr.TileCmbTwrBldr','CaloCellMaker_CaloCellContainerChecke','CaloCellMaker_CaloCellContainerFinali','CmbTowerBldr.FCalCmbTwrBldr','CaloCellMaker_LArCellBuilderLAr','CaloCellMaker_TileCellBuilder','CmbTowerBldr.LArCmbTwrBldr','TowerMaker.LArTowerBuilder','CmbTowerBldr.LArCmbTwrBldrChronoCellLoop','CmbTowerBldr.LArCmbTwrBldrChronobuild','TowerMaker.LArTowerBuilderChronobuild','TowerMaker.LArTowerBuilderChronoCellLoop']
        self.debugLevel=0
        # in sec
        self.diffTime=0.05
        self.reflog="reflogRec1002_H_2e2mu_q02dig_1004"
        self.reflogESDAOD="reflogRecESDAOD1002_H_2e2mu_q02dig_1004"
        
        #self.emails=['bernardet@cppm.in2p3.fr']
        self.emails=[]
        self.wwwemails="http://marwww.in2p3.fr/~kbernard/caloperf/CaloRec_mails2.txt"
        self.testName="CaloRec"
        # if type = 1 use of the list above for the algo
        self.type=0
        self.typePerso=0
        # Size of RDO.pool.root = XXX ko         

        self.filesToCheck=["ESD.pool.root","AOD.pool.root"]

        # release, time info
        self.release=""
        self.branch=""
        self.date=""

        self.mailSubjectProblem=""
        self.mailSubjectReport=""
        self.switchoffRTTpart=False
        #####################
        self.alertMess=""
        self.alertMessOk=False
        self.alertReportOk=True
        self.checkFile=0

    def alert(self,mess,mylog):

        if mess=="SEND":

            # get the mails list
            com= "wget "+self.wwwemails
            os.system(com)

            try:
                file = open('CaloRec_mails2.txt','r')
                self.emails=file.readlines()
                file.close()
            except:
                com="mail bernardet@cppm.in2p3.fr "  + " -s \"Problem with extractCaloRec.py\""
                os.popen(com, 'w').write("File mails.txt not found")
                exit
            
            for email in self.emails:

                    if self.alertMess !="":
                        com1="mail " + email.strip() + " -s "+self.mailSubjectProblem
                        
                        com2=self.alertMess + "\n\n"
                        os.popen(com1, 'w').write(com2)
                    
                    if self.alertReportOk==True: 
                        com1="mail " + email.strip() + " -s "+self.mailSubjectReport
                        
                        com2= mylog.mess2+ "\n\n"
                        os.popen(com1, 'w').write(com2)
        else :
            self.alertMess=self.alertMess+mess+"\n"
            self.alertMessOk=True


            
####  CPU Time

    def extractCPUTimeInitializeAll(self,file,type,mylog):

        tmp=[]
        if type !=1:
            com="grep \"Time User\" " + file + " | grep initi"

            if self.debugLevel:
                mylog.log(com)
        
            res=subprocess.getoutput(com)
            tmp= res.splitlines() 
        else:
            for i in self.CaloList:
                com="grep \"Time User\" " + file + " | grep \"\<" +i+":initi\""
                if self.debugLevel:
                    mylog.log(com)
                res=subprocess.getoutput(com)
                tmp += res.splitlines()
            
        return tmp
            
    def extractCPUTimeInitializeAlgo(self,file, algo,mylog):

        com="grep \"Time User\" " + file + " | grep initi" + " | grep  " + algo +" | awk '{print $7" "$8}' "

        if self.debugLevel:
            mylog.log(com)
        
        res=subprocess.getoutput(com)    
        res2= res.splitlines() 
        return res2

    def extractCPUTimeExecuteAll(self,file,type,mylog):
    
        tmp=[]
        if type !=1:
            com="grep \"Time User\" " + file + " | grep execute"

            if self.debugLevel:
                mylog.log(com)
        
            res=subprocess.getoutput(com)    
            tmp= res.splitlines() 
        else:
            for i in self.CaloList:
                com="grep \"Time User\" " + file + " | grep \"\<" +i
                if self.debugLevel:
                    mylog.log(com)
                res=subprocess.getoutput(com)
                tmp += res.splitlines()

        return tmp

# pour les algo perso, ajouter un espace apres le nom de l algo pour le grep sinon pb en cas de noms du genre : AlgoPerso,AlgoPerso2 ...


    def extractCPUTimeExecutePersoAll(self,file,type,mylog):
    
        tmp=[]
        if type !=1:
            com="grep \"Time User\" " + file + " | grep -v execute| grep -v initialize | grep -v finalize"

            if self.debugLevel:
                mylog.log(com)
        
            res=subprocess.getoutput(com)    
            tmp= res.splitlines() 
        else:
            for i in self.extraCaloList:
                com="grep \"Time User\" " + file + " | grep '" +i+" '" 
                if self.debugLevel:
                    mylog.log(com)
                res=subprocess.getoutput(com)
                tmp += res.splitlines()

        return tmp


    def extractCPUTimeExecuteAlgo(self,file, algo,mylog):

        com="grep \"Time User\" " + file + " | grep  '\<" + algo +"' | awk '{print $7" "$8}' "

        if self.debugLevel:
            mylog.log(com)
        
        res=subprocess.getoutput(com)    
        res2= res.splitlines() 
        return res2

    def extractCPUTimeExecutePersoAlgo(self,file, algo,mylog):

        com="grep \"Time User\" " + file + " | grep  '\<" + algo +" ' | awk '{print $7" "$8}' "

        if self.debugLevel:
            mylog.log(com)
        
        res=subprocess.getoutput(com)

        if self.debugLevel:
            mylog.log(res)

        res2= res.splitlines() 
        return res2


    def extractNbEventsExecuteAlgo(self,file, algo,mylog):

        com="grep \"Time User\" " + file + " | grep  '\<" + algo +"'| cut -d\# -f 2 | cut -d = -f 2 "

        if self.debugLevel:
            mylog.log(com)
        
        res=subprocess.getoutput(com)    
        res2= res.splitlines() 
        return res2

    def extractNbEventsExecutePersoAlgo(self,file, algo, mylog):

        com="grep \"Time User\" " + file + " | grep  '\<" + algo +" '| cut -d\# -f 2 | cut -d = -f 2 "

        if self.debugLevel:
            mylog.log(com)
        
        res=subprocess.getoutput(com)    
        res2= res.splitlines() 
        return res2


    def getNbEvents(self,file,mylog):

        com="grep \"INFO Total Virtual memory\" " + file +" | wc -l"

        if self.debugLevel:
            mylog.log(com)
        
        res=subprocess.getoutput(com)    
        res2= res.splitlines() 
        return res2


### Memory
    
    def extractMemoryCBNTInitializeAll(self,file,mylog):

        tmp=[]
        # virtual memory
        com="grep \"End of initialization : Total Virtual memory\" " + file +" | awk '{print $11}'"

        if self.debugLevel:
            mylog.log(com)
        res=subprocess.getoutput(com)    
        tmp +=res.splitlines()

        # real memory
        com="grep \"End of initialization : Total Virtual memory\" " + file +" | awk '{print $13}'"

        if self.debugLevel:
            mylog.log(com)
        res=subprocess.getoutput(com)    
        tmp +=res.splitlines()
    
        return tmp

    def extractMemoryCBNTExecute(self,file,eventNumber,mylog):

        tmp=[]

        # virtual memory
        com="grep \"INFO Total Virtual memory\" " + file +" | awk '{print $7}'"

        if self.debugLevel:
            mylog.log(com)
        
        res=subprocess.getoutput(com)
        res2= res.splitlines()
        if len(res2)==0:
            return tmp
        tmp.append(res2[eventNumber-1])
        if self.debugLevel:
            mess='Virtual memory = '+res2[eventNumber-1]+'Mb'
            mylog.log(mess)

        # real memory
        com="grep \"INFO Total Virtual memory\" " + file +" | awk '{print $9}'"
        if self.debugLevel:
            mylog.log(com)

        res=subprocess.getoutput(com)
        res2= res.splitlines()
        if len(res2)==0:
            return tmp
        tmp.append(res2[eventNumber-1])
        if self.debugLevel:
            mess='Real memory = '+res2[eventNumber-1]+'Mb'
            mylog.log(mess)

        return tmp


    def compareCPUTimeInitialize(self,file,reffile,type,mylog):
        # 
        CPUTimeRef=[]
        alg=[]
        CPUTimeRef=self.extractCPUTimeInitializeAll(reffile,type,mylog)
        for i in CPUTimeRef:
            t=i.lstrip()
            tmp=t.split(" ")
            alg.append(tmp[0])
        #printfunc (alg)
        for j in alg:
            tref=self.extractCPUTimeInitializeAlgo(reffile,str(j),mylog)
            tnew=self.extractCPUTimeInitializeAlgo(file,str(j),mylog)
            if len(tnew)==0:
                    mess=j+" not found in " + file
                    mylog.log(mess)
                    continue
            #if str(j)=="LArSWClusterMaker:initialize":
            #    printfunc (str(j)," ", tref,"  ",tnew)
            #if self.CPUTimeUnity(str(tref[0]))==self.CPUTimeUnity(str(tnew[0])):
            trefUnity=self.CPUTimeUnity(str(tref[0]))
            tnewUnity=self.CPUTimeUnity(str(tnew[0]))
            ttnew=tnew[0].split('[')
            ttref=tref[0].split('[')
            ttref2=self.convertCPUTime(float(ttref[0]),trefUnity)      
            ttnew2=self.convertCPUTime(float(ttnew[0]),tnewUnity)
            #ttnew3=ttnew2[0].split('[')
            #ttref3=ttref2[0].split('[')

            if ttnew2-ttref2 > self.diffTime:
                mess="WARNING : "+j+" REF : "+str(ttref[0])+self.CPUTimeUnity(str(tref[0]))+" NOW : "+str(ttnew[0])+self.CPUTimeUnity(str(tref[0]))
                mylog.log(mess)
                self.alert(mess,mylog)
            else:
                mess="GOOD : "+j+" REF : "+str(ttref[0])+self.CPUTimeUnity(str(tref[0]))+" NOW : "+str(ttnew[0])+self.CPUTimeUnity(str(tref[0]))
                mylog.log(mess)
            
            
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

    def compareCPUTimeExecute(self,file,reffile,type,mylog):
        # 
        CPUTimeRef=[]
        alg=[]
        CPUTimeRef=self.extractCPUTimeExecuteAll(reffile,type,mylog)
        for i in CPUTimeRef:
            t=i.lstrip()
            tmp=t.split(" ")
            alg.append(tmp[0])
        for j in alg: 
                tref=self.extractCPUTimeExecuteAlgo(reffile,str(j),mylog)
                nbEvtRef=self.extractNbEventsExecuteAlgo(reffile,str(j),mylog)
                tnew=self.extractCPUTimeExecuteAlgo(file,str(j),mylog)
                nbEvtNew=self.extractNbEventsExecuteAlgo(file,str(j),mylog)
                #printfunc (j," ", tref,"  ",tnew)
                #printfunc ('KARIM2 ',j," ",nbEvtRef ,"  ",nbEvtNew)
                if len(tnew)==0:
                    mess=j+" not found in " + file
                    mylog.log(mess)
                    continue
                
                # convert en secondes
                trefUnity=self.CPUTimeUnity(str(tref[0]))
                tnewUnity=self.CPUTimeUnity(str(tnew[0]))
                ttnew=tnew[0].split('[')
                ttref=tref[0].split('[')
                ttref2=self.convertCPUTime(float(ttref[0]),trefUnity)      
                ttnew2=self.convertCPUTime(float(ttnew[0]),tnewUnity)
                normtref=float(ttref2)/float(nbEvtRef[0])
                normtnew=float(ttnew2)/float(nbEvtNew[0])
            
                if normtnew-normtref > self.diffTime:
                    mess="WARNING : "+j+" REF : "+str(normtref)+" s "+" NOW : "+str(normtnew)+" s "+ "  (per event)"
                    mylog.log(mess)
                    # alert
                    self.alert(mess,mylog)
                else:
                    mess="GOOD : "+j+" REF : "+str(normtref)+" s "+" NOW : "+str(normtnew)+" s"+" (per event)"
                    mylog.log(mess)
        return


    def compareCPUTimeExecutePerso(self,file,reffile,type,mylog):
        # 
        CPUTimeRef=[]
        alg=[]
        #CPUTimeRef=self.extractCPUTimeExecutePersoAll(reffile,type,mylog)
        CPUTimeRef=self.extractCPUTimeExecutePersoAll(file,type,mylog)
        
        for i in CPUTimeRef:
            t=i.lstrip()
            tmp=t.split(" ")
            alg.append(tmp[0])
        for j in alg: 
                tref=self.extractCPUTimeExecutePersoAlgo(reffile,str(j),mylog)
                
                nbEvtRef=self.extractNbEventsExecutePersoAlgo(reffile,str(j),mylog)
                tnew=self.extractCPUTimeExecutePersoAlgo(file,str(j),mylog)
                nbEvtNew=self.extractNbEventsExecutePersoAlgo(file,str(j),mylog)
                isok=1
                if len(tnew)==0:
                    mess=j+" not found in " + file
                    mylog.log(mess)
                    tmp=tref[0].split('[')
                    mess=j+" REF : "+str(tmp[0])+self.CPUTimeUnity(str(tref[0]))
                    mylog.log(mess)
                    isok=0
                if len(tref)==0:
                    mess=j+" not found in " + reffile
                    mylog.log(mess)
                    tmp=tnew[0].split('[')
                    mess=j+ " NEW : "+str(tmp[0])+self.CPUTimeUnity(str(tnew[0]))
                    mylog.log(mess)
                    isok=0

                if isok==0:
                    continue
                    
                # printfunc (j," ", tref,"  ",tnew)
                # printfunc (j," ",nbEvtRefb ,"  ",nbEvtNewb)
                
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
                    mylog.log(mess)
                    
                    self.alert(mess,mylog)
                else:
                    mess="GOOD : "+j+" REF : "+str(normtref)+" s "+" NOW : "+str(normtnew)+" s"+" (per event)"
                    mylog.log(mess)
        return



    def compareCBNTMemoryInitialize(self,file,reffile,mylog):
        #
        meminitref=self.extractMemoryCBNTInitializeAll(reffile,mylog)
        meminit=self.extractMemoryCBNTInitializeAll(file,mylog)

        if len(meminit)<1:
            mess='Info about memory initialize not found'
            mylog.log(mess)
            return
        
        # RSS
        if float(meminit[1])>float(meminitref[1]):
            mess='WARNING RSS : REF: '+str(meminitref[1])+' NOW: '+str(meminit[1])
            mylog.log(mess)
            if float(meminit[1])-float(meminitref[1])>10 :
                            # alert
                            self.alert(mess,mylog)
        else:
            mess='GOOD RSS : REF: '+str(meminitref[1])+' NOW: '+str(meminit[1])
            mylog.log(mess)
        # VSS
        if float(meminit[0])>float(meminitref[0]):
            mess='WARNING VSS : REF: '+str(meminitref[0])+' NOW: '+str(meminit[0])
            mylog.log(mess)
            if float(meminit[0])-float(meminitref[0])>10 :
                            # alert
                            self.alert(mess,mylog)
        else:
            mess='GOOD VSS : REF: '+str(meminitref[0])+' NOW: '+str(meminit[0])
            mylog.log(mess)

        return 

    def compareCBNTMemoryExecute(self,file,reffile,mylog):
        
        nbEvents=self.getNbEvents(reffile,mylog)
        for i in range(1,int(nbEvents[0])+1):
            #mess="After event "+str(i)+" :"
            #self.mylog.log(mess)
            try:
                memexeref=self.extractMemoryCBNTExecute(reffile,i,mylog)
            except:
                #mess="memory info not found for ref log"
                #mylog.log(mess)
                #return
                continue
            try:
                memexe=self.extractMemoryCBNTExecute(file,i,mylog)
            except:
                #mess="memory info not found for new log"
                #mylog.log(mess)
                #return
                continue
            # printfunc (memexeref, memexe)
            if len(memexe)==0:
                #mess='Info about memory execute not found'
                #mylog.log(mess)
                #return
                continue
            # RSS
            if float(memexe[1])>float(memexeref[1]):
                mess="After event "+str(i)+" :"+' WARNING RSS : REF: '+str(memexeref[1])+' NOW: '+str(memexe[1])
                mylog.log(mess)
                if float(memexe[1])-float(memexeref[1])>10 :
                            # alert
                            self.alert(mess,mylog)
            else:
                mess="After event "+str(i)+" : " +'GOOD RSS : REF: '+str(memexeref[1])+' NOW: '+str(memexe[1])
                mylog.log(mess)
            # VSS
            if float(memexe[0])>float(memexeref[0]):
                mess="After event "+str(i)+" : "+' WARNING VSS : REF: '+str(memexeref[0])+' NOW: '+str(memexe[0])
                mylog.log(mess)
                if float(memexe[0])-float(memexeref[0])>10 :
                            # alert
                            self.alert(mess,mylog)
            else:
                mess="After event "+str(i)+" : "+' GOOD VSS : REF: '+str(memexeref[0])+' NOW: '+str(memexe[0])
                mylog.log(mess)



        # leak
            
        mylog.log("leak ? ")

        # find the first event with memory info
        #for i in range(1,int(nbEvents[0])+1):
        #    printfunc (i)
        #    try:
        #        memexeFIRST=self.extractMemoryCBNTExecute(file,i,mylog)
        #        firstEvForLeak=memexeFIRST
        #        break
        #    except:
        #        printfunc ('pipo')
                
        # find the last event with memory info
        #for i in range(int(nbEvents[0])+1,1):
        #    printfunc (i)
        #    try:
        #        memexeLAST=self.extractMemoryCBNTExecute(file,i,mylog)
        #        lastEvForLeak=memexeLAST
        #        break
        #    except:
        #        printfunc ('pipo')

        #printfunc ("first event : ", memexeFIRST)
        #printfunc ("last event : ", memexeLAST)
        

        #leakRSS=float(memexeLAST[1])-float(memexeFIRST[1])
        #leakVSS=float(memexeLAST[0])-float(memexeFIRST[0])

        com="grep \"only second half of the job\" " + file + " | awk '{print $9}'"
        
        res=subprocess.getoutput(com)
        res2= res.splitlines()
        
        mess="leak = "+str(res2[0])+" MByte/event"
        mylog.log(mess)
        if float(res2[0])>0.01:
            # alert
            self.alert(mess,mylog)
        
            
        return

#########

    def logExtract(self,file,chain,mylog):

        com="egrep -i \""+chain + "\"  "  + file

        if self.debugLevel:
            mylog.log(com)
        
        res=subprocess.getoutput(com)    
        res2= res.splitlines() 
        return res2

##########

### Files size

    def extractFileSizeRef(self,file,mylog):

        tmp=[]
        if len(self.filesToCheck)>0:
            for i in self.filesToCheck:
                com="grep \"^Size of "+str(i)+"\" " + file + " | awk '{print $5}'"
                if self.debugLevel:
                    mylog.log(com)
                res=subprocess.getoutput(com)
                tmp += res.splitlines()
            return tmp
        else:
            com="No size to check"
            mylog.log(com)
            return
    
    def extractFileSize(self,file,mylog):

        tmp=[]
        if len(self.filesToCheck)>0:
            for i in self.filesToCheck:

                try:
                    file = open(str(i))
                    file.close()
                except IOError:
                    mess=str(i)+' not found'
                    mylog.log(mess)
                    return
                
                com="ls -l  "+str(i)+" " + " | awk '{print $5}'"
                if self.debugLevel:
                    mylog.log(com)
                res=subprocess.getoutput(com)
                tmp += res.splitlines()
            return tmp
        else:
            com="No size to check"
            mylog.log(com)
            return
    
    def compareSizeOfFiles(self,file,reffile,mylog):
        # 
        SizeOfFilesRef=[]
        SizeOfFilesNew=[]
        files=[]
        SizeOfFilesRef=self.extractFileSizeRef(reffile,mylog)
        
        SizeOfFilesNew=self.extractFileSize(file,mylog)
        for j in range(0,len(SizeOfFilesNew)):

            mess="Size of the ref file " + self.filesToCheck[j] +" = " +str(SizeOfFilesRef[j])
            mylog.log(mess)
            
            mess="Size of the new file " + self.filesToCheck[j] +" = " +str(SizeOfFilesNew[j])
            mylog.log(mess)
            


        for j in range(0,len(SizeOfFilesRef)):
                        
            nbEventsRef=self.extractNbEventsExecuteAlgo(reffile,"CaloCellMaker:execute",mylog)
            nbEvents=self.extractNbEventsExecuteAlgo(file,"CaloCellMaker:execute",mylog)

            #printfunc ('KARIM',nbEventsRef,' ', nbEvents)

            SizeOfFilesNew[j]=float(SizeOfFilesNew[j])/float(nbEvents[0])
            SizeOfFilesRef[j]=float(SizeOfFilesRef[j])/float(nbEventsRef[0])

            if float(SizeOfFilesNew[j])==0:
                continue
            if float(SizeOfFilesNew[j])>float(SizeOfFilesRef[j]):
                    mess="WARNING : "+self.filesToCheck[j]+" REF : "+str(SizeOfFilesRef[j])+" NOW : "+str(SizeOfFilesNew[j]) +" (bytes per event)"
                    mylog.log(mess)
                    if float(SizeOfFilesNew[j])-float(SizeOfFilesRef[j])>10 :
                        # alert
                        self.alert(mess,mylog)
            else:
                mess="GOOD : "+self.filesToCheck[j]+" REF : "+str(SizeOfFilesRef[j])+" NOW : "+str(SizeOfFilesNew[j]) +" (bytes per event)"
                mylog.log(mess)
        return


##     def extractLog(self,log):

##         com="grep ^BEGIN_RTTINFO "+log
##         res=subprocess.getoutput(com)    
##         res2=res.splitlines()
##         printfunc (res2)
##         names=[]
##         for i in res2:
##             name=i.split('=')
##             name[1]=name[1].strip()
##             names.append(name[1])
##             printfunc (name[1])
##             com="grep -n \"^BEGIN_RTTINFO = "+name[1]+"\" "+log+"| cut -d: -f1"
##             printfunc (com)
##             n1=subprocess.getoutput(com)
##             com="grep -n \"^END_RTTINFO = "+name[1]+"\" "+log+" | cut -d: -f1"
##             n2=subprocess.getoutput(com)
##             nblines=int(n2)-int(n1)
##             newlog=name[1]+".log"
##             com="grep -A"+str(nblines)+" \"^BEGIN_RTTINFO = "+name[1]+"\" "+log +" >"+newlog
##             os.system(com)
##         return names

    def extractLog(self,log):


        com="wc -l "+log +" | awk '{print $1}'"
        res=subprocess.getoutput(com)    
        res2=res.splitlines()
        nbAllLines=int(res2[0])

        #printfunc (nbAllLines)
        
        com="grep ^BEGIN_RTTINFO "+log
        res=subprocess.getoutput(com)    
        res2=res.splitlines()
        
        names=[]
        for i in res2:
            name=i.split('=')
            name[1]=name[1].strip()
            names.append(name[1])
            
        for i in range(len(names)):
            #printfunc (i,names[i])
            #com="grep -n BEGIN_RTTINFO="+names[i]+" "+log+" | cut -d: -f1"
            com="grep -n \"^BEGIN_RTTINFO = "+names[i]+"\" "+log+"| cut -d: -f1"
            n1=subprocess.getoutput(com)
            try:
                #com="grep -n BEGIN_RTTINFO="+names[i+1]+" "+log+" | cut -d: -f1"
                com="grep -n \"^BEGIN_RTTINFO = "+names[i+1]+"\" "+log+"| cut -d: -f1"
                
                n2=subprocess.getoutput(com)
            except:
                n2=nbAllLines
            
            nblines=int(n2)-int(n1)-1
            #printfunc (nblines)
            newlog=names[i]+".log"
            #com="grep -A"+str(nblines)+" BEGIN_RTTINFO="+names[i]+" "+log +" >"+newlog
            com="grep -A"+str(nblines)+" \"^BEGIN_RTTINFO = "+names[i]+"\" "+log +" >"+newlog
            os.system(com)
        return names


###########

    def run(self,params={}):
        
        # arg 1 = log
        # arg 2 = reflog

        isATN=0
# fix for the RTT bug to retrieve the good logfile                
        try:
 #           rttDescriptor = self.dict.get('JobDescriptor')
#            log=rttDescriptor.log
            res2=subprocess.getoutput("ls CaloRec_RTT_*_log")
            log=res2
        except:
            log=sys.argv[1]
            isATN=1
#        log=sys.argv[1]
#        isATN=1		
# end of the fix

        names=self.extractLog(log)
	
        
       ##  mylog.log(rttDescriptor.log)
##         mylog.log(rttDescriptor.elog)
##         mylog.log(rttDescriptor.identifiedName)
##         mylog.log(rttDescriptor.runPath)
##         mylog.log(rttDescriptor.resPath)
##         mylog.log(rttDescriptor.jobOptions)
##         mylog.log(str(rttDescriptor.actions))
##         mylog.log(str(rttDescriptor.tests))


        for test in names:
            com="rm resultats.txt"
            subprocess.getoutput(com)    
               
            mylog=LogClass("resultats.txt",1)

            icheckFileSize=1

            log=test+".log"
            com = "cat " +log + "|grep BEGIN_RTTINFO | cut -d= -f2| tail -n1" 

            res=subprocess.getoutput(com)
            jobopt=res.strip()

            mylog.log(str(res))

            # ESD, AOD test ?
            res2=res.strip()
            if res2.find('ESD')<0:
                # no ESD, AOD job
                icheckFileSize=0

            # extract reflog info 
            ok=0
            for i in self.logDict.keys():
                if i==res.strip():
                    printfunc ("Found : ",self.logDict[i])
                    ok=1
                    self.reflog=self.logDict[i]

                    break

            if ok==0:
                printfunc ("logfile not in dict !!!")
                return              


            try:
                file = open(self.reflog)
                file.close()
            except IOError:
                mess=self.reflog+' not found'
                mylog.log(mess)
                return

            # date
            self.date=str(datetime.datetime.now().strftime("%Y-%m-%d %X"))
            mylog.log(self.date)


            if self.switchoffRTTpart==True:
                self.release="test"
                self.branch="test"
            else:
                rttDescriptor = self.dict.get('JobDescriptor')
                self.release=rttDescriptor.paths.release
                self.branch=rttDescriptor.paths.branch


            self.mailSubjectProblem="\"[RTT test, "+jobopt+","+self.branch+","+self.release+"] : extractCaloRec problem \""
            self.mailSubjectReport="\"[RTT test, "+jobopt+","+self.branch+","+self.release+"] : extractCaloRec report \""

            mylog.log("----------------------------------------------------------------------------------")
            mylog.log("Computing node informations : ")

            com="cat /etc/redhat-release"
            res=subprocess.getoutput(com)
            mylog.log("Operating system : "+str(res))

            com="cat /proc/cpuinfo | grep \"model name\" | tail -n1 | cut -d: -f2"
            res=subprocess.getoutput(com)
            mylog.log("CPU :"+str(res))
                
            com="cat /proc/meminfo | grep MemTotal | cut -d: -f2"
            res=subprocess.getoutput(com)
            mylog.log("MEMORY :"+str(res))
            mylog.log("----------------------------------------------------------------------------------")

            mess="Reference log = " + self.reflog
            mylog.log(mess)
            mess="Log to check = "+log
            mylog.log(mess)

            mylog.log("********************************")
            mylog.log("CHECK WARNING")
            res=self.logExtract(log,"warning",mylog)
            for i in res:
                mylog.log(str(i))
            mylog.log("********************************")
            mylog.log("********************************")
            mylog.log("CHECK ERROR")
            res=self.logExtract(log,"error",mylog)
            for i in res:
                mylog.log(str(i))
            mylog.log("********************************")
            mylog.log("********************************")
            mylog.log("CHECK STATUSCODE")
            res=self.logExtract(log,"\|",mylog)
            for i in res:
                mylog.log(str(i))
            mylog.log("********************************")

            mylog.log("********************************")
            mylog.log("CHECK INITIALIZE TIME")
            self.compareCPUTimeInitialize(log,self.reflog,self.type,mylog)
            mylog.log("********************************")
            mylog.log("CHECK EXECUTE TIME")
            self.compareCPUTimeExecute(log,self.reflog,self.type,mylog)
            self.compareCPUTimeExecutePerso(log,self.reflog,self.typePerso,mylog)   
            mylog.log("********************************")
            mylog.log("CHECK MEMORY")
            mylog.log("INIT MEMORY")
            self.compareCBNTMemoryInitialize(log,self.reflog,mylog)
            mylog.log("EXE MEMORY")
            self.compareCBNTMemoryExecute(log,self.reflog,mylog)

            if icheckFileSize==1:
                mylog.log("********************************")
                mylog.log("CHECK SIZE OF OUTPUT FILES")
                try:
                    self.compareSizeOfFiles(log,self.reflog,mylog)
                except:
                    mylog.log("Problem with compareSizeOfFiles")
                mylog.log("********************************")

            com="ls -ltr"
            mylog.log(com)
            mylog.log(str(subprocess.getoutput(com)))


            mylog.close()

            self.alert("SEND",mylog)

    #a=extract()
    #a.run("")
