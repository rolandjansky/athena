#!/usr/bin/python

import commands,sys,os,logging

class myperfmon:
    def __init__(self,log,logLevel):
        if log=='':
            self.logger = logging.getLogger('myperfmon')
            hdlr = logging.FileHandler('myperfmon.log',"w")
            formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
            hdlr.setFormatter(formatter)
            self.logger.addHandler(hdlr)
            self.logger.setLevel(logLevel)
        else:
            self.logger = log

    def run(self):
        res,output=commands.getstatusoutput("./perfmon.py -i ntuple.pmon.gz")
        if res!=0:
            self.logger.error('problem to run perfmon.py on ntuple.pmon.gz :'+output)
            return -1
        return 0



#a=myperfmon('','')
#a.run()
