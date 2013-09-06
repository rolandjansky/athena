#!/usr/bin/python

import commands,sys,os,logging


class addHTML:
    def __init__(self,log,logLevel,plotname,filename):
        self.filename=filename
        self.plotname=plotname
        if log=='':
            self.logger = logging.getLogger('addHTML')
            hdlr = logging.FileHandler('addHTML.log',"w")
            formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
            hdlr.setFormatter(formatter)
            self.logger.addHandler(hdlr)
            self.logger.setLevel(logLevel)
        else:
            self.logger = log
            
    def run(self):

       
        file = open('./'+self.filename+'.html','w')
        txt='<html><head><link rel=\"StyleSheet\" href=\"rtt.css\" type=\"text/css\" /><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"><meta http-equiv=\"Content-Language\" content=\"en\"></head><body>'
        txt+='<br><br><br><br>'
        txt+='<img src=\"'+self.plotname+'.gif\">'+'</center></body></html>'
        file.write(txt)
        file.close()

    
        
#a=addHTML('',logging.INFO,'plotAllCaloNoise13','ElectronicNoise')
#a.run()
    
