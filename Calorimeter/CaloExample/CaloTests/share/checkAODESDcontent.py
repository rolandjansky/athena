#!/usr/bin/python

import commands,sys,os,logging

class checkAODESDcontent:
    def __init__(self,log,logLevel):
        if log=='':
            self.logger = logging.getLogger('AODESD')
            hdlr = logging.FileHandler('checkAODESD.log',"w")
            formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
            hdlr.setFormatter(formatter)
            self.logger.addHandler(hdlr)
            self.logger.setLevel(logLevel)
        else:
            self.logger = log

    def run(self):
        res1,output=commands.getstatusoutput("./checkFile.py ESD.pool.root >& ESDAOD.log")
        if res1!=0:
            self.logger.error('problem to run checkFile.py on ESD.pool.root')
        res2,output=commands.getstatusoutput("./checkFile.py AOD.pool.root >> ESDAOD.log")
        if res2!=0:
            self.logger.error('problem to run checkFile.py on AOD.pool.root')

        if res1!=0 and res2!=0:
            self.logger.error('no files to check')
            return -1
        
        com = "cat MYRTTINFOS.txt" + "|grep BEGIN_RTTINFO | cut -d= -f2| tail -n1"
        res,testName=commands.getstatusoutput(com)
        if res!=0:
            self.logger.error(testName)
            return -1
        reflog='ref'+testName.strip()+'AODESD.log'

        com='wget http://cern.ch/CaloRTT/'+reflog
        res,out=commands.getstatusoutput(com)
        if res!=0:
            self.logger.error(out)
            return -1

        try:
            f=open(log,'r')
            fclose(log)
        except:
            self.logger.error(log+' not found')
            return -1
        

        keysList=[]
        file = open(reflog,'r')
        for line in file:
            t=line.split()
            if len(t)==8 and t[1]=='kb' and t[3]=='kb' and t[5]=='kb':
                keysList.append(t[7])
        file.close()

        for k in keysList:
            com1='grep '+k+' '+log+' | grep -v Tree'
            res1,out1=commands.getstatusoutput(com1)
            if res1!=0:
                self.logger.warning(k+' NOT FOUND')
                continue
            tmpNew=out1.split()

            com2='grep '+k+' '+reflog+' | grep -v Tree'
            res2,out2=commands.getstatusoutput(com2)
            if res2!=0:
                continue
            tmpRef=out2.split()

            if float(tmpNew[4]) > float(tmpRef[4]):
                self.logger.warning(k +' is higher : '+'NEW='+tmpNew[4]+'/'+'REF='+tmpRef[4]+' (kb)')
                
        return 0
        
    
