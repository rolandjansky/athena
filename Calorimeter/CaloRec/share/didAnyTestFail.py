import os, sys,getopt, datetime
import re
import logging

from future import standard_library
standard_library import aliases()
import subprocess

def file_exists(fpath):
        return os.path.exists(fpath)

    
class CheckForFail:

    def __init__(self,paramDict={}):
        self.dict =paramDict
        #self.tests=["CaloRecEx_RTT_Photons_100GeV.py","CaloRecEx_RTT_H_2e2mu_q02dig_1004.py","CaloRecEx_RTT_T1_McAtNLO_top.py"]
	self.tests=["1004_Photons_100GeV","1050_H_2e2mu_q02dig_1004","1103_T1_McAtNLO_top"]
        self.emails="http://marwww.in2p3.fr/~kbernard/caloperf/CaloRec_mails.txt"
	self.date=""
	self.release=""
	self.branch=""
	self.switchoffRTTpart=False

	
    def run(self,params={}):

        result=0
	
	self.date=str(datetime.datetime.now().strftime("%Y-%m-%d %X"))
        logger = logging.getLogger('didAnyTestFail')
        hdlr = logging.FileHandler('didAnyTestFail.txt',"w")
        formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
        hdlr.setFormatter(formatter)
        logger.addHandler(hdlr)
        logger.setLevel(logging.DEBUG)

	#logger.info("value of self.switchoffRTTpart  : "+str(self.switchoffRTTpart))
	

	if self.switchoffRTTpart==True:
		self.release="test"
		self.branch="test"
	else:
		rttDescriptor = self.dict.get('JobDescriptor')
		self.release=rttDescriptor.paths.release
		self.branch=rttDescriptor.paths.branch
		
	
	jobopt=rttDescriptor.jobOptions
	logger.info(jobopt)

	#Name=rttDescriptor.name
	#logger.info(Name)
	## for opt in self.tests:
## 		if jobopt.find(opt)<0:
## 			break
## 		return 1

	# FIX : since rttDescriptor.name doesn't work correctly
	log=subprocess.getoutput("ls CaloRec_RTT_*_log")
	com = "cat " +log + "|grep BEGIN_RTTINFO | cut -d= -f2| tail -n1"
	jobopt=subprocess.getoutput(com)
	logger.info(jobopt)
	found=0
	for opt in self.tests:
		if opt==jobopt.strip():
			found=1
			break
		
	if found==0:
		return 0

        if self.switchoffRTTpart==True:
		respath="test"
	else:	
		respath=rttDescriptor.resPath
	n=respath.find("opt")
	tmp=""
	for i in range(n,len(respath)):
		tmp=tmp+respath[i]
	wwwtest="http://www.hep.ucl.ac.uk/atlas/RTTprojects/"+tmp

	mailSubject="\"[RTT test, "+jobopt+","+self.branch+","+self.release+"] : Validation report \""
    
        # get the mails list
        com= "wget "+self.emails
        os.system(com)

        try:
            file = open('mails.txt','r')
        except:
            logger.error("File mails.txt not found")
            file = open('didAnyTestFail.txt','r')
            txt=file.read()
            file.close()
            com="mail bernardet@cppm.in2p3.fr "  + " -s "+mailSubject
            os.popen(com, 'w').write(txt)
	    result=-1
            return result

        mails=file.readlines()
        file.close()

        logger.info("Results mail sent to :")
        logger.info(mails)

        # almostIdentical.txt
        logger.info("CHECK ALMOSTIDENTICAL")
        if not file_exists("almostIdentical.txt"):
            logger.error("almostIdentical.txt not found")
            logger.error("Does CaloRec_histComp.C run ?")
            file = open('didAnyTestFail.txt','r')
            txt=file.read()
            file.close()
            for mail in mails:
		    com="mail " + mail.strip() + " -s "+mailSubject
                    os.popen(com, 'w').write(txt)
            result=-1
            return result

        file = open('almostIdentical.txt','r')
        lines=file.readlines()
        file.close()
	
        
        for i in range(len(lines)):
            if re.search('TEST FAILED',lines[i]):
	        logger.error('-----------------------------------')
                logger.error(lines[i-2].strip()+" FAILED : ")
                logger.error(lines[i-1].strip())
                logger.error('-----------------------------------')
		result=-1
        file.close()
	logger.info("CHECK ALMOSTIDENTICAL DONE")  

        #truth.txt
        logger.info("CHECK TRUTH")
        if not file_exists("truth.txt"):
            logger.error("truth.txt not found")
            logger.error("Does CaloRec_truth.C run ?")
            file = open('didAnyTestFail.txt','r')
            txt=file.read()
            file.close()
            for mail in mails:
		    com="mail " + mail.strip() + " -s "+mailSubject
                    os.popen(com, 'w').write(txt)
	    result=-1
            return result

        file = open('truth.txt','r')
        lines=file.readlines()
        file.close()

        #logger.error('-----------------------------------')
        for i in range(len(lines)):
            if re.search('TEST FAILED',lines[i]):
	        logger.error('-----------------------------------')
                logger.error(lines[i-6].strip()+" FAILED : ")
                logger.error(lines[i-5].strip())
                logger.error(lines[i-4].strip())
                logger.error(lines[i-3].strip())
                logger.error(lines[i-2].strip())
                logger.error(lines[i-1].strip())
                logger.error('-----------------------------------')
                result=-1
        file.close()
	logger.info("CHECK TRUTH DONE")  

        # TO FIX : the info returned by the jobDescriptor is wrong if several jobs
	logger.info("Please check the results at : "+wwwtest)
	
        # send the mails
        file = open('didAnyTestFail.txt','r')
        txt=file.read()
        file.close()
        for mail in mails:
		com="mail " + mail.strip() + " -s "+mailSubject
		os.popen(com, 'w').write(txt)
        return result


#a=CheckForFail()
#print a.run()
