#!/usr/bin/python

import commands,sys,os,logging


class memleak:
    def __init__(self,log,logLevel):
        if log=='':
            self.logger = logging.getLogger('memleak')
            hdlr = logging.FileHandler('memleak.log',"w")
            formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
            hdlr.setFormatter(formatter)
            self.logger.addHandler(hdlr)
            self.logger.setLevel(logLevel)
        else:
            self.logger = log

    def run(self):

        log=commands.getoutput("ls Calo*_RTT*_log")
        try:
            f=open(log,'r')
            f.close()
        except:
            self.logger.error(log+' not found')
            return -1
        
        com='grep \"only second half of the job\" '+log+'| grep INFO'
        res,out=commands.getstatusoutput(com)
        if res!=0:
            self.logger.error(out)
            return -1

        tmp=out.split('only second half of the job:')
        tmp2=tmp[1].strip().split('MByte/event')
        if float(tmp2[0].strip())>0.1:
            self.logger.warning('MEM LEAK ! :'+tmp[1].strip())
            return -1
        return 0
        
    
                
#a=memleak('',logging.INFO)
#a.run()
    
