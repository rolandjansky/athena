#!/usr/bin/env python

from Tkinter import *
from Tkinter import _setit
import getopt, sys, os, re, time, tempfile, subprocess
import __main__


class Option:
    def __init__(self, name, value, description):
        self.name        = name
        self.description = description
        self.type        = type(value)
        self.noRead      = False
        if self.type==str: self.tkvar       = StringVar()
        else :             self.tkvar       = IntVar()
        self.tkvar.set(value)
        
    def __call__(self,*args,**kwargs):
        return self.tkvar.get()

    def __str__(self):
        rep = "### %s\n" % self.description
        rep += "option['%s'] = ( " % self.name
        if self.type==str:
            rep += "'" + str(self.tkvar.get()) + "'"
        else:
            rep += str(self.tkvar.get())
        rep += ", '%s')" % self.description
        rep += '\n'
        return rep

    def set(self,value):
        self.tkvar.set(value)
        
    
        
class Config:
    def __init__(self, configFile=None):
        self.options = {}
        self.tmpoptions = {}
        self.inputFile = configFile
        self.setupDefault()
        self.read()
        
    def __getattr__(self,name):
        if self.options.has_key(name):
            return self.options[name]
        elif self.tmpoptions.has_key(name):
            return self.tmpoptions[name]
        else:
            raise KeyError, "'%s' is not an option" % name

    def addOption(self, name, value, description):
        self.options[name] = Option(name, value, description);

    def addTmpOption(self, name, value, description):
        self.tmpoptions[name] = Option(name, value, description);

    def setupDefault(self):
        import os
        tempdir = os.environ["TMPDIR"] if "TMPDIR" in os.environ else (tempfile.gettempdir() + '/' + os.environ["USER"])
        # red 
        self.addTmpOption('release','',"The release tag");
        self.addTmpOption('patch','',"The patch tag");
        self.addTmpOption('swpath','',"The software path");
        self.addOption('tt','/afs/cern.ch/user/a/attrgcnf/TriggerTool/TriggerTool-new.jar',"The TriggerTool jar-file");
        # green
        self.addOption('jobOption','',"The python job option file");
        self.addOption('userPreCommand','',"The users precommands to be run");
        self.addOption('setupDir','./setup',"Directory for setup files");
        self.addOption('tmpDir',tempdir,"Directory for temporary results");
        self.addOption('jobName','Default',"Job name");
        self.addOption('nEvt',10,"Number of Events");
        self.addOption('inputL2','input.data',"Input data file L2");
        self.addOption('inputEF','',"Input data file EF");
        self.addOption('l1menu','',"LVL1 menu file");
        self.addOption('hltefmenu','',"HLT EF menu file");
        self.addOption('hltl2menu','',"HLT L2 menu file");
        self.addOption('setupL2','',"Setup file L2");
        self.addOption('setupEF','',"Setup file EF");
        self.addOption('dbAlias','TRIGGERDBREPR',"Alias for TriggerDB connection");
        self.addOption('dbPW','',"Password for writer accound (not needed by experts)");
        self.addOption('cfgKey',0,"Configuration key");
        self.addOption('l1psKey',0,"LVL1 prescale key");
        self.addOption('hltpsKey',0,"HLT prescale key");
        self.addOption('onlineMonitoring',True,"Online Monitoring switch");
        self.addTmpOption('cafConfig',False,"Configuration for CAF");  # tmp option to avoid accidental use for online
        #self.addOption('newPropertyExtraction',True,"New Properties switch");
        # blue
        self.addOption('athenaMTjo',True,"Run athenaMT from JobOptions");
        self.addOption('athenaPTjo',True,"Run athenaPT from JobOptions");
        self.addOption('setupCnv',  True,"Run setup conversion script");
        self.addOption('uploadDb',  True,"Load TriggerDB");
        self.addOption('athenaMTdb',True,"Run athenaMT from TriggerDB");
        self.addOption('athenaPTdb',True,"Run athenaPT from TriggerDB");
        self.addOption('checkRes',  True,"Check the results by comparing counts");
        self.addOption('mtJoLog',   '',"Log file for athenMT from JO");
        self.addOption('ptJoLog',   '',"Log file for athenPT from JO");
        self.addOption('cnvLog',    '',"Log file for conversion script");
        self.addOption('loadDbLog', '',"Log file for loading TriggerDB");
        self.addOption('mtDbLog',   '',"Log file for athenMT from TriggerDB");
        self.addOption('ptDbLog',   '',"Log file for athenPT from TriggerDB");

    def read(self):
        if self.inputFile==None: return
        if not os.path.exists(self.inputFile):
            print "Configuration file %s can not be accessed!" % self.inputFile
            return
        print "Reading configuration from %s!" % self.inputFile
        execfile(self.inputFile,__main__.__dict__)
        for o, v in option.iteritems():
            #if o=='jobOption': continue
            self.addOption(o, v[0], v[1])

    def write(self):
        outputfile = 'myPref_%s.py' % self.jobName()
        print "Saving configuration to %s !" % outputfile
        f = open(outputfile, 'w')
        print >>f, 'option = {}\n'
        for opt in self.options:
            print >>f, self.options[opt]
        f.close()





class App:
    def __init__(self, master, inputFile):
        self.master = master
        self.config = Config(inputFile)
        self.testresults = {}
        self.SUMMARY = None
        self.guessMenuInPreCommand()

        # coral frame
        envFrame = Frame(master,bg='lightcoral')
        envFrame.columnconfigure(1,weight=1)
        envFrame.pack(side=TOP,pady=5,ipady=5,padx=10,fill=X)

        self.envFrame = {}
        self.envFrame['rel'] = ( Label(envFrame,text='Release:'),
                                 Label(envFrame,textvariable=self.config.release.tkvar) )
        self.envFrame['pat'] = ( Label(envFrame,text='Patch:'),
                                 Label(envFrame,textvariable=self.config.patch.tkvar) )
        self.envFrame['sw']  = ( Label(envFrame,text='Software path:'),
                                 Label(envFrame,textvariable=self.config.swpath.tkvar) )
        self.envFrame['tt']  = ( Label(envFrame,text='TriggerTool:'),
                                 Entry(envFrame,textvariable=self.config.tt.tkvar) )

        display = ['rel', 'pat', 'sw', 'tt']
        for i,key in enumerate(display):
            (label,entry)   = self.envFrame[key]
            label['bg']     = 'lightcoral'
            label.grid(row=i, padx=5, column=0, sticky=W)
            entry['width']  = 50
            entry['bg']     = 'white'
            entry['relief'] = 'flat'
            if key != 'tt':
                entry['anchor']  = W
                entry['bg']      = 'lightcoral'
                entry.grid(padx=5, pady=1, row=i, column=2, sticky=E)
            else:
                self.chTT = Checkbutton(envFrame, text='Stable',variable=self.config.tt.tkvar);
                self.chTT.grid(row=i,column=1,sticky=E);
                self.chTT['bg']='lightcoral'
                self.chTT['selectcolor']='darkblue'
                self.chTT['activebackground']='coral'
                self.chTT['highlightthickness']=0
                self.chTT['onvalue']='/afs/cern.ch/user/a/attrgcnf/TriggerTool/TriggerTool.jar'
                self.chTT['offvalue']='/afs/cern.ch/user/a/attrgcnf/TriggerTool/TriggerTool-new.jar'
                entry['highlightthickness']=0
                entry.grid(padx=5, pady=1, row=i, column=2, sticky=E)




        # green frame
        self.configFrame = Frame(master,bg='lightgreen')
        self.configFrame.columnconfigure(2,weight=1)
        self.configFrame.pack(side=TOP,pady=5,ipady=5,padx=10,fill=X)

        self.cfgFrame = {}
        self.cfgFrame['jo']  = ( Label(self.configFrame,text='Joboption file:'),
                                 Entry(self.configFrame,textvariable=self.config.jobOption.tkvar) )
        self.config.jobOption.tkvar.trace("w",self.recheckMenuOptions)
        self.cfgFrame['pc']  = ( Label(self.configFrame,text='Pre-command:'),
                                 Entry(self.configFrame,textvariable=self.config.userPreCommand.tkvar) )
        self.cfgFrame['st']  = ( Label(self.configFrame,text='Setup directory:'),
                                 Entry(self.configFrame,textvariable=self.config.setupDir.tkvar) )
        self.cfgFrame['td']  = ( Label(self.configFrame,text='Temp directory:'),
                                 Entry(self.configFrame,textvariable=self.config.tmpDir.tkvar) )
        self.cfgFrame['db']  = ( Label(self.configFrame,text='DB connection alias:'),
                                 Entry(self.configFrame,textvariable=self.config.dbAlias.tkvar) )
        self.cfgFrame['pw']  = ( Label(self.configFrame,text='DB write password (DEV1/2):'),
                                 Entry(self.configFrame,textvariable=self.config.dbPW.tkvar) )
        self.cfgFrame['ne']  = ( Label(self.configFrame,text='Number of Events:'),
                                 Entry(self.configFrame,textvariable=self.config.nEvt.tkvar) )
        self.cfgFrame['jn']  = ( Label(self.configFrame,text='Job name:'),
                                 Entry(self.configFrame,textvariable=self.config.jobName.tkvar) )
        self.cfgFrame['il2'] = ( Label(self.configFrame,text='MT input data:'),
                                 Entry(self.configFrame,textvariable=self.config.inputL2.tkvar) )
        self.cfgFrame['ief'] = ( Label(self.configFrame,text='PT input data:'),
                                 Entry(self.configFrame,textvariable=self.config.inputEF.tkvar) )
        self.cfgFrame['ml1'] = ( Label(self.configFrame,text='LVL1 menu:'),
                                 Entry(self.configFrame,textvariable=self.config.l1menu.tkvar) )
        self.cfgFrame['ml2'] = ( Label(self.configFrame,text='L2 menu:'),
                                 Entry(self.configFrame,textvariable=self.config.hltl2menu.tkvar) )
        self.cfgFrame['mef'] = ( Label(self.configFrame,text='EF menu:'),
                                 Entry(self.configFrame,textvariable=self.config.hltefmenu.tkvar) )
        self.cfgFrame['sl2'] = ( Label(self.configFrame,text='L2 setup:'),
                                 Entry(self.configFrame,textvariable=self.config.setupL2.tkvar) )
        self.cfgFrame['sef'] = ( Label(self.configFrame,text='EF setup:'),
                                 Entry(self.configFrame,textvariable=self.config.setupEF.tkvar) )
        self.cfgFrame['key'] = ( Label(self.configFrame,text='Configuration key:'),
                                 Entry(self.configFrame,textvariable=self.config.cfgKey.tkvar) )
        self.cfgFrame['l1pk']= ( Label(self.configFrame,text='LVL1 prescale key:'),
                                 Entry(self.configFrame,textvariable=self.config.l1psKey.tkvar) )
        self.cfgFrame['hpk'] = ( Label(self.configFrame,text='HLT prescale key:'),
                                 Entry(self.configFrame,textvariable=self.config.hltpsKey.tkvar) )

        display = ['jn', 'jo', 'pc', 'st', 'td', 'db', 'pw', 'ne', 'il2', 'ief', 'ml1', 'ml2', 'mef', 'sl2', 'sef', 'key', 'l1pk', 'hpk']
        # find the correct runHLT_standalone
        if(self.config.jobOption()==''):
            for p in os.getenv('JOBOPTSEARCHPATH').split(os.pathsep)+['./']:
                if not os.path.exists(p+'/TriggerRelease/runHLT_standalone.py'): continue
                self.config.jobOption.set(p+'/TriggerRelease/runHLT_standalone.py')
                break
        else:
            self.recheckMenuOptions()

        # create the optionmenu
        for i,key in enumerate(display):
            (label,entry) = self.cfgFrame[key]
            label['bg']    = 'lightgreen'
            if key=='pc':
                label.grid(row=i, padx=5, column=0, sticky=W)
            else:
                label.grid(row=i, padx=5, column=0, columnspan=2, sticky=W)
            entry['bg']    = 'white'
            entry['width'] = 50
            entry.grid(padx=5, row=i, column=2,sticky=E)
            if key=='pc':
                self.om.grid(row=i,column=1,padx=5,sticky=E)


        row=len(display)
        cbFrame = Frame(self.configFrame,bg='lightgreen')
        cbFrame.columnconfigure(2,weight=1)
        self.ch1 = Checkbutton(cbFrame, text='Run online monitoring',variable=self.config.onlineMonitoring.tkvar);
        self.ch1.grid(row=0,sticky=W)
        self.ch1['bg']='lightgreen'
        self.ch1['activebackground']='green'
        self.ch1['selectcolor']='darkblue'
        self.ch1['highlightthickness']=0

        #self.ch2 = Checkbutton(cbFrame, text='Use new conversion',variable=self.config.newPropertyExtraction.tkvar);
        #self.ch2.grid(row=0,column=1,sticky=W)
        #self.ch2['bg']='lightgreen'
        #self.ch2['activebackground']='green'
        #self.ch2['selectcolor']='darkblue'
        #self.ch2['highlightthickness']=0

        self.ch3 = Checkbutton(cbFrame, text='Configuration for CAF',variable=self.config.cafConfig.tkvar,command=self.toggleCAFOption);
        self.ch3.grid(row=0,column=1,sticky=W)
        self.ch3['bg']='lightgreen'
        self.ch3['activebackground']='green'
        self.ch3['selectcolor']='darkblue'
        self.ch3['highlightthickness']=0
        
        self.save = Button(cbFrame,text="SAVE", command=self.config.write)
        self.save.grid(row=0, column=2, padx=20, sticky=E)

        cbFrame.grid(sticky=W, row=row, column=0, padx=5, pady=5, columnspan=3)

        ### blue frame
        runningFrame = Frame(master,bg='lightblue')
        runningFrame.pack(side=TOP,pady=5,ipady=5,padx=10,fill=X)
        runningFrame.columnconfigure(0,weight=1)
        self.rfch = {}
        self.rfch['mtjo'] = ( Checkbutton(runningFrame, text='athenaMT from joboptions',variable=self.config.athenaMTjo.tkvar, command=self.highlightOptions),
                              Entry(runningFrame,textvariable=self.config.mtJoLog.tkvar),
                              Button(runningFrame,text='Clear',command=self.clearMTjo) )
        self.rfch['ptjo'] = ( Checkbutton(runningFrame, text='athenaPT from joboptions',variable=self.config.athenaPTjo.tkvar, command=self.highlightOptions),
                              Entry(runningFrame,textvariable=self.config.ptJoLog.tkvar),
                              Button(runningFrame,text='Clear',command=self.clearPTjo))
        self.rfch['cnv']  = ( Checkbutton(runningFrame, text='setup conversion', variable=self.config.setupCnv.tkvar, command=self.highlightOptions),
                              Entry(runningFrame,textvariable=self.config.cnvLog.tkvar),
                              Button(runningFrame,text='Clear',command=self.clearCnv))
        self.rfch['ldb']  = ( Checkbutton(runningFrame, text='load TriggerDB', variable=self.config.uploadDb.tkvar, command=self.highlightOptions),
                              Entry(runningFrame,textvariable=self.config.loadDbLog.tkvar),
                              Button(runningFrame,text='Clear',command=self.clearLoadDb))
        self.rfch['mtdb'] = ( Checkbutton(runningFrame, text='athenaMT from TriggerDB', variable=self.config.athenaMTdb.tkvar, command=self.highlightOptions),
                              Entry(runningFrame,textvariable=self.config.mtDbLog.tkvar),
                              Button(runningFrame,text='Clear',command=self.clearMTdb))
        self.rfch['ptdb'] = ( Checkbutton(runningFrame, text='athenaPT from TriggerDB', variable=self.config.athenaPTdb.tkvar, command=self.highlightOptions),
                              Entry(runningFrame,textvariable=self.config.ptDbLog.tkvar),
                              Button(runningFrame,text='Clear',command=self.clearPTdb))
        self.rfch['res']  = ( Checkbutton(runningFrame, text='Check results', variable=self.config.checkRes.tkvar),
                              Button(runningFrame,text='Show MT Result',command=self.displayResultMT),
                              Button(runningFrame,text='Show PT Result',command=self.displayResultPT))

        row=0
        display = ['mtjo', 'ptjo', 'cnv', 'ldb', 'mtdb', 'ptdb', 'res']
        for i in range(len(display)):
            row=row+1
            (ch,en,bu) = self.rfch[display[i]]
            if isinstance(ch,Checkbutton):
                ch['bg'] = 'lightblue'
                ch['selectcolor'] = 'darkblue'
                ch['activebackground']='blue'
                ch['highlightthickness']=0
                ch.grid(padx=5, row=row, column=0,sticky=W)
            elif isinstance(ch,Button):
                ch.grid(padx=5, pady=1, row=row, column=0,sticky=E)
            if not isinstance(en,Button):
                en['bg']     = 'grey'
                en['width']  = 40
                en['relief'] = 'flat'
                en['state']  = DISABLED
            if en:
                en.grid(padx=5, pady=1,row=row, column=1,sticky=E)
            if bu:
                bu.grid(padx=5, pady=1,row=row, column=2,sticky=E)

        self.frame = Frame(master)
        self.frame.pack(side=BOTTOM)

        self.bb = [ Button(self.frame, text='Clear Errors',command=self.clearErrors),
                    Button(self.frame, text='Clear All',   command=self.clearAll),
                    Button(self.frame, text='Run',         command=self.run),
                    Button(self.frame, text="Quit", command=self.quit) ]
        for b in self.bb:
            b.pack(side=LEFT)

        ### done with the GUI setup

        self.setFrameName('Prepare for setting Default')

        self.checkReleaseAndTT()



    def recheckMenuOptions(self, *dummy):
        jofile = self.config.jobOption()
        fileexists = os.path.exists(jofile)
        self.cfgFrame['jo'][1]['foreground'] = 'black' if fileexists else 'red'
        self.chosenMenu = StringVar()
        values = self.readMenus(self.config.jobOption())

        if not hasattr(self,"om"):
            self.om = OptionMenu(self.configFrame, self.chosenMenu, command=self.selectMenu, *values )
            self.om.grid(row=2,column=1,padx=5,sticky=E)
            
        if fileexists:
            self.om.__init__(self.configFrame, self.chosenMenu, command=self.selectMenu, *values )
        else:
            self.om["state"] = DISABLED
                           
        return True

    def guessMenuInPreCommand(self):
        pc = self.config.userPreCommand().strip(';')
        mods = {} if not pc else dict([tuple(m.split('=')) for m in pc.split(';')])
        p = re.compile("test.*")
        menucands = filter(p.match,mods.keys())
        self.menuInPreCommand = menucands[0][4:] if menucands else None
        self.setPreCommandModifier('caf', None)


    def setPreCommandModifier(self, name, value):
        pc = self.config.userPreCommand().strip(';')
        
        mods = {} if not pc else dict([tuple(m.split('=')) for m in pc.split(';')])

        if value==None:
            if name in mods: mods.pop(name)
        else:
            mods[name] = value

        pc = ';'.join(['='.join([k,v]) for k,v in sorted(mods.iteritems())])
        self.config.userPreCommand.set(pc)            
        
    def toggleCAFOption(self):
        v = 'True' if self.config.cafConfig()==True else None        
        self.setPreCommandModifier('caf',v)        
        
    def readMenus(self,filename):
        try:
            f = open(filename)
            lines = f.readlines()
            f.close()
            begin=0
            end=0
            for i,l in enumerate(lines):
                if begin==0:
                    if not l.startswith("menuMap={"): continue
                    begin = i
                cl = l.find("}")
                co = l.find("#")
                if cl>=0 and (co>cl or co<0):
                    end=i+1
                    break

            exec(''.join(lines[begin:end]))
            menus = sorted(menuMap.keys())
        except:
            return ["None found"]

        index=0
        if self.menuInPreCommand:
            index = menus.index(self.menuInPreCommand)
        elif "PhysicsV2" in menus:
            index = menus.index("PhysicsV2")

        if not menus: menus = ["None found"]
        if index>=0:
            self.chosenMenu.set(menus[index])
            self.selectMenu(menus[index])

            
        return menus


    def selectMenu(self, *args, **kwargs):
        if self.menuInPreCommand!=None:
            self.setPreCommandModifier('test%s' % self.menuInPreCommand, None)
            
        self.setPreCommandModifier('test%s' % args[0], 'True')
        self.menuInPreCommand = self.chosenMenu.get()
        

    def highlightOptions(self):
        color = 'white' if (self.config.athenaMTjo() or self.config.athenaPTjo()) else 'grey'
        for e in [ self.cfgFrame['jo'], self.cfgFrame['pc'] ]:
            e[1]['bg'] = color
        color = 'white' if (self.config.athenaMTjo() or self.config.athenaPTjo() or self.config.setupCnv() or self.config.uploadDb()) else 'grey'
        for e in [ self.cfgFrame['st'] ]:
            e[1]['bg'] = color
        color = 'white' if self.config.uploadDb() else 'grey'
        for e in [ self.envFrame['tt'], self.cfgFrame['db'], self.cfgFrame['pw'] ]:
            e[1]['bg'] = color
        color = 'white' if (self.config.athenaMTdb() or self.config.athenaPTdb()) else 'grey'
        for e in [ self.cfgFrame['db'], self.cfgFrame['pw'], self.cfgFrame['key'], self.cfgFrame['l1pk'], self.cfgFrame['hpk'] ]:
            e[1]['bg'] = color
        


    def write(self,s):
        if not self.SUMMARY:
            base = self.config.jobName()
            self.SUMMARY=open('%s/log%sSummary' % (self.config.tmpDir(),base), "w")
        #s=s.rstrip()
        print s,
        print >>self.SUMMARY, s,
        self.SUMMARY.flush()

    def quit(self):
        self.config.write()
        self.frame.quit()

    def setFrameName(self,name):
        self.master.master.master.title(name)

    def checkReleaseAndTT(self):
        self.config.release.set(os.getenv('AtlasVersion'))
        self.config.swpath.set(os.getenv('AtlasBaseDir'))
        if os.getenv('AtlasProject') in ['AtlasP1HLT','AtlasCAFHLT','AtlasProduction']:
            self.config.patch.set(os.getenv('AtlasProject'))
        
        if self.config.tt()=='':
            for p in os.getenv('DATAPATH').split(os.pathsep):
                if not os.path.exists(p+'/TriggerTool/TriggerTool.jar'): continue
                self.config.tt.set(p+'/TriggerTool/TriggerTool.jar')
                break

    def update(self):
        for v in self.cfgFrame.values():
            v[1].update_idletasks()
        for v in self.rfch.values():
            if v[1]:
                v[1].update_idletasks()
        
    def clearErrors(self):
        for (ch,en,bu) in self.rfch.values():
            if ch:
                ch['selectcolor'] = 'darkblue'
        self.ch1['selectcolor']='darkblue'

    def run(self):
        if (self.config.athenaMTjo() or \
            self.config.athenaPTjo() or \
            self.config.athenaMTdb() or \
            self.config.athenaPTdb() ) and \
            self.config.onlineMonitoring():
            if not self.checkAthenaMon():
                self.setFrameName('Failure')
                return
        if self.config.athenaMTjo() and not self.runMTjo():
            self.setFrameName('Failure')
            return
        if self.config.athenaPTjo() and not self.runPTjo():
            self.setFrameName('Failure')
            return
        if self.config.setupCnv() and not self.runSetupCnv():
            self.setFrameName('Failure')
            return
        if self.config.uploadDb() and not self.runUploadDb():
            self.setFrameName('Failure')
            return
        if self.config.athenaMTdb() and not self.runMTdb():
            self.setFrameName('Failure')
            return
        if self.config.athenaPTdb() and not self.runPTdb():
            self.setFrameName('Failure')
            return
        if self.config.checkRes() and not self.runCheckResult():
            self.setFrameName('Failure')
            return
        self.setFrameName('Success')

    def checkAthenaMon(self):
        self.ch1['selectcolor']='green'
        return True
                

    def checkFile(self,caller, filename,filedesc):
        if filename=='':
            print 'ERROR: <%s> No %s configured' % (caller, filedesc)
            return False
        if not os.path.exists(filename):
            print 'ERROR: <%s> %s %s does not exist in %s' % (caller, filedesc, filename, os.path.realpath('.'))
            return False
        return True


    def clearAll(self):
        self.clearErrors()
        self.clearMTjo()
        self.clearPTjo()
        self.clearCnv()
        self.clearLoadDb()
        self.clearMTdb()
        self.clearPTdb()



    #####################################################
    ###
    ### Running athenaMT from the JO
    ###
    #####################################################
    def clearMTjo(self):
        base = self.config.jobName()
        outputData = "%s-1._0001.data" % base
        logfile = '%s/log%sMT' % (self.config.tmpDir(),base)
        try: os.remove(outputData)
        except OSError: pass
        try: os.remove("%s.writing" % outputData)
        except OSError: pass
        try: os.remove(logfile)
        except OSError: pass
        self.config.l1menu.set('')
        self.config.hltl2menu.set('')
        self.config.hltefmenu.set('')
        self.config.setupL2.set('')
        self.config.inputEF.set('')
        self.config.mtJoLog.set('')        
        self.update()
    def runMTjo(self):
        self.setFrameName('MT JO')
        self.clearMTjo()
        if self.rfch['mtjo'][0]['selectcolor']=='red':
            print 'INFO: <runMTjo> clear ERROR first'
            return;
        base = self.config.jobName()
        if not self.checkFile('runMTjo', self.config.inputL2(), "Input file"):
            self.rfch['mtjo'][0]['selectcolor']='red'
            return False
        
        logfile    = '%s/log%sMT' % (self.config.tmpDir(),base)
        outputData = "%s-1._0001.data" % base
        setupfile  = './l2_%s_setup.txt' % base
        l1menu     = './outputLVL1.xml' # extract from logfile
        args       = "-c doDBConfig=True -c trigBase='\"%s\"'" % base
        if str(self.config.userPreCommand()) != str(''):
            args += " -c '" + self.config.userPreCommand().strip() + "'"
        # remove the old data file
        subprocess.call(("rm -f %s*" % outputData).split())

        cmd  = 'athenaMT.py -f %s' % self.config.inputL2()
        if self.config.onlineMonitoring(): cmd += ' --oh-monitoring'
        cmd += ' -n %i -o %s %s %s' % (self.config.nEvt(),base,args,(self.config.jobOption()))
        print >>self, '%s &>! %s' % (cmd,logfile)
        
        self.config.mtJoLog.set(logfile)
        self.update()
        try:
            FD = open(logfile,'w')
            retCode = subprocess.call(cmd.split(),stdout=FD,stderr=FD)
        except OSError, v:
            print 'ERROR: <runMTjo>',v
            self.rfch['mtjo'][0]['selectcolor']='red'
            FD.close()
            return False
        FD.close()
        if retCode!=0:
            print 'ERROR: <runMTjo> job returned',retCode
            self.rfch['mtjo'][0]['selectcolor']='red'
            return False

        l1menu    = subprocess.Popen("egrep 'DBCONFIG LVL1XML' %s" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1]
        hltl2menu = subprocess.Popen("egrep 'DBCONFIG HLTXML' %s" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1]

        self.config.l1menu.set(l1menu)
        self.config.hltl2menu.set(hltl2menu)
        self.config.setupL2.set(setupfile)
        self.config.inputEF.set(outputData)
        self.rfch['mtjo'][0]['selectcolor']='green'
        print >>self, "===>>> Success"
        return True



    #####################################################
    ###
    ### athenaPT from JobOptions
    ###
    #####################################################
    def clearPTjo(self):
        base = self.config.jobName()
        logfile = '%s/log%sPT' % (self.config.tmpDir(),base)
        setupfile = './ef_%s_setup.txt' % base
        try: os.remove(logfile)
        except OSError: pass
        try: os.remove(setupfile)
        except OSError: pass
        self.config.setupEF.set('')
        self.config.ptJoLog.set('')
        self.update()
    def runPTjo(self):
        self.setFrameName('PT JO')
        self.clearPTjo()
        if self.rfch['ptjo'][0]['selectcolor']=='red':
            print 'INFO: <runMTjo> clear ERROR first'
            return;
        if self.config.inputEF()=='':
            print 'ERROR: <runPTjo> no input file configured'
            self.rfch['ptjo'][0]['selectcolor']='red'
            return False
        if not os.path.exists(self.config.inputEF()):
            print 'ERROR: <runPTjo> input file %s can not be accessed' % self.config.inputEF()
            self.rfch['ptjo'][0]['selectcolor']='red'
            return False
        
        base = self.config.jobName()

        args = "-c doDBConfig=True"
        args += " -c trigBase='\"%s\"'" % base
        if str(self.config.userPreCommand()) != str(''):
            args += " -c '" + self.config.userPreCommand().strip() + "'"

        logfile = '%s/log%sPT' % (self.config.tmpDir(),base)
        self.config.ptJoLog.set(logfile)
        self.update()

        cmd  = 'athenaPT.py'
        if self.config.onlineMonitoring(): cmd += ' --oh-monitoring'
        cmd += ' -f %s -n %i %s %s' % (self.config.inputEF(),self.config.nEvt(),args,(self.config.jobOption()))
        print >>self, '%s &>! %s' % (cmd,logfile)

        try:
            FD = open(logfile,'w')
            retCode = subprocess.call(cmd.split(),stdout=FD,stderr=FD)
        except OSError, v:
            print "ERROR: <runPTjo>",v
            self.rfch['ptjo'][0]['selectcolor']='red'
            return False
        FD.close()
        if retCode!=0:
            print 'ERROR: <runPTjo> job returned',retCode
            self.rfch['ptjo'][0]['selectcolor']='red'
            return False

        print "Running"
        print "egrep 'DBCONFIG HLTXML' %s" % logfile

        hltefmenu = subprocess.Popen("egrep 'DBCONFIG HLTXML' %s" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1]

        setupfile = './ef_%s_setup.txt' % base
        self.config.hltefmenu.set(hltefmenu)
        self.config.setupEF.set(setupfile)
        self.rfch['ptjo'][0]['selectcolor']='green'
        print >>self, "===>>> Success"
        return True



    #####################################################
    ###
    ### setup conversion
    ###
    #####################################################
    def clearCnv(self):
        fullSetupDir = '%s/%s' % (self.config.setupDir(), self.config.jobName());
        subprocess.call(('rm -r %s' % fullSetupDir).split())
        try: os.remove(self.config.cnvLog())
        except OSError: pass
        self.config.cnvLog.set('')
        self.update()
    def runSetupCnv(self):
        self.setFrameName('Convert')
        if self.rfch['cnv'][0]['selectcolor']=='red':
            print 'INFO: <cnvSetup> clear ERROR first'
            return False;
        if not self.checkFile('cnvSetup', self.config.setupL2(), 'L2 setup file') or \
           not self.checkFile('cnvSetup', self.config.setupEF(), 'EF setup file') or \
           not self.checkFile('cnvSetup', self.config.hltl2menu(), 'HLT L2 menu file') or \
           not self.checkFile('cnvSetup', self.config.hltefmenu(), 'HLT EF menu file'):
            self.rfch['cnv'][0]['selectcolor']='red'
            return False
        if self.config.l1menu()=='':
            self.rfch['cnv'][0]['selectcolor']='red'
            print 'ERROR: <cnvSetup> No L1 menu configured'
            return False

        # check for level 1 menu
        l1MenuCheckedOut=False
        l1menuName = self.config.l1menu()
        if not os.path.exists(os.path.basename(l1menuName)):
            subprocess.call(('get_files -xmls %s' % os.path.basename(l1menuName)).split())
        if not os.path.exists(os.path.basename(l1menuName)):
            self.rfch['cnv'][0]['selectcolor']='red'
            print "ERROR: <cnvSetup> Could not retrieve l1 menu '%s' from release" % l1menuName
            return False
        l1menuName = os.path.basename(l1menuName)
        
        base = self.config.jobName()

        fullSetupDir = '%s/%s' % (self.config.setupDir(), self.config.jobName());
        subprocess.call(['mkdir', '-p', fullSetupDir])

        subprocess.call(['cp', self.config.setupL2(), fullSetupDir+'/l2setup.txt'])
        subprocess.call(['cp', self.config.setupEF(), fullSetupDir+'/efsetup.txt'])
        subprocess.call(['cp', l1menuName,            fullSetupDir+'/lvl1Menu.xml'])
        subprocess.call(['cp', self.config.hltl2menu(), fullSetupDir+'/hltL2Menu.xml'])
        subprocess.call(['cp', self.config.hltefmenu(), fullSetupDir+'/hltEFMenu.xml'])
        #if self.config.newPropertyExtraction():
        subprocess.call(['cp', self.config.setupL2().replace('.txt','_setup.txt'), fullSetupDir+'/l2setuppy.txt'])
        subprocess.call(['cp', self.config.setupEF().replace('.txt','_setup.txt'), fullSetupDir+'/efsetuppy.txt'])


        logfile = '%s/log%sConv' % (self.config.tmpDir(),base)
        self.config.cnvLog.set(logfile)
        self.update()

        cwd = os.getcwd()
        os.chdir(fullSetupDir)

        cmd = 'menumerge.py --l2menu hltL2Menu.xml --efmenu hltEFMenu.xml --output hltMenu.xml'
        print '%s &>! %s' % (cmd,logfile)
        try:
            FD = open(logfile,'w')
            retCode = subprocess.call(cmd.split(),stdout=FD,stderr=FD)
        except OSError, v:
            print 'ERROR: <cnvSetup>',v
            self.rfch['cnv'][0]['selectcolor']='red'
            return False
        FD.close()
        if retCode!=0:
            print 'ERROR: <cnvSetup> menu merging failed'
            self.rfch['cnv'][0]['selectcolor']='red'
            return False


        if True: # self.config.newPropertyExtraction():
            cmd = 'ConvertHLTSetup_txt2xml.py l2setup.txt l2setuppy.txt'
        else:
            cmd = 'ConvertHLTSetup_txt2xml_Old.py l2setup.txt'
        print >>self,'%s &>>! %s' % (cmd,logfile)
        try:
            FD = open(logfile,'a')
            retCode = subprocess.call(cmd.split(),stdout=FD,stderr=FD)
        except OSError, v:
            print >>self, 'ERROR: <cnvSetup>',v
            self.rfch['cnv'][0]['selectcolor']='red'
            os.chdir(cwd)
            return False
        FD.close()
        if retCode!=0:
            print 'ERROR: <cnvSetup> L2 setup conversion failed'
            self.rfch['cnv'][0]['selectcolor']='red'
            os.chdir(cwd)
            return False

        if True:  # self.config.newPropertyExtraction()
            cmd = 'ConvertHLTSetup_txt2xml.py efsetup.txt efsetuppy.txt'
        else:
            cmd = 'ConvertHLTSetup_txt2xml_Old.py efsetup.txt'
        print >>self,'%s &>>! %s' % (cmd,logfile)
        try:
            FD = open(logfile,'a')
            retCode = subprocess.call(cmd.split(),stdout=FD,stderr=FD)
        except OSError, v:
            print 'ERROR: <cnvSetup>',v
            self.rfch['cnv'][0]['selectcolor']='red'
            os.chdir(cwd)
            return False
        FD.close()
        if retCode!=0:
            print 'ERROR: <cnvSetup> L2 setup conversion failed'
            self.rfch['cnv'][0]['selectcolor']='red'
            os.chdir(cwd)
            return False
        os.chdir(cwd)
        self.rfch['cnv'][0]['selectcolor']='green'
        print >>self, "===>>> Success"
        return True

    #####################################################
    ###
    ### Upload into TriggerDB
    ###
    #####################################################
    def writeReleaseXml(self):
        FD = open('release.xml','w')
        print >>FD,'''<?xml version="1.0" encoding="UTF-8"?>
<!--This file is generated by OnlinePrep.py -->
<release name="%s" version="1" patch="%s"/>''' % (self.config.release(),self.config.patch())
        FD.close()
    def clearLoadDb(self):
        logfile = self.config.loadDbLog()
        try: os.remove(logfile)
        except OSError: pass
        self.config.loadDbLog.set('')
        self.config.cfgKey.set(0)
        self.update()    
    def runUploadDb(self):
        self.setFrameName('Load DB')
        if self.rfch['ldb'][0]['selectcolor']=='red':
            print 'INFO: <uploadDb> clear ERROR first'
            return;
        java='/afs/cern.ch/sw/lcg/external/Java/JDK/1.6.0/ia32/jre/bin/java'
        TT=self.config.tt()

        # check files in the setupdir
        fullSetupDir = '%s/%s' % (self.config.setupDir(), self.config.jobName());
        cwd = os.getcwd()
        os.chdir(fullSetupDir)
        if ( not self.checkFile('uploadDb', 'l2setup.xml', 'L2 setup file') or \
             not self.checkFile('uploadDb', 'efsetup.xml', 'EF setup file') or \
             not self.checkFile('uploadDb', 'hltMenu.xml', 'HLT menu file') or \
             not self.checkFile('uploadDb', 'lvl1Menu.xml','LVL1 menu file') ):
            self.rfch['ldb'][0]['selectcolor']='red'
            os.chdir(cwd)
            return False
        os.chdir(cwd)

        base = self.config.jobName()
        
        logfile = '%s/log%sUpload' % (self.config.tmpDir(),base)
        self.config.loadDbLog.set(logfile)
        self.update()

        cmd  = '%s -jar %s' % (java,TT)
        if self.config.dbPW()=="":
            cmd += ' -db %s' % self.config.dbAlias()
        else:
            # tt short (not such a nice hack, but allows us to specify the password in the gui)
            alias = self.config.dbAlias()
            if alias=="TRIGGERDBDEV1": shortAlias = "DEV1"
            elif alias=="TRIGGERDBDEV2": shortAlias = "DEV2"
            elif alias=="TRIGGERDBMC": shortAlias = "DBMC"
            else: shortAlias = alias
            cmd += ' -db %s' % shortAlias
            cmd += ' -pw %s' % self.config.dbPW()
        cmd += ' -up'
        cmd += ' -l1  %s/lvl1Menu.xml' % fullSetupDir
        cmd += ' -hlt %s/hltMenu.xml'  % fullSetupDir
        cmd += ' -l2s %s/l2setup.xml'  % fullSetupDir
        cmd += ' -efs %s/efsetup.xml'  % fullSetupDir
        cmd += ' -rel %s'  % self.config.release()
        cmd += ' -o %sTT'  % logfile
        cmd += ' -n %s' % base
        print >>self,'%s &>! %s' % (cmd,logfile)
        try:
            FD = open(logfile,'w')
            retCode = subprocess.call(cmd.split(),stdout=FD,stderr=FD)
        except OSError, v:
            print 'ERROR: <uploadDb>',v
            self.rfch['ldb'][0]['selectcolor']='red'
            return False
        FD.close()
        # go back to working dir
        os.chdir(cwd)
        if retCode!=0:
            print 'ERROR: <uploadDb> error uploading into database'
            self.rfch['ldb'][0]['selectcolor']='red'
            return False

        cfgKey = int(subprocess.Popen("egrep 'UPLOAD Supermasterkey' %sTT" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1])
        l1Key  = int(subprocess.Popen("egrep 'UPLOAD LVL1prescalekey' %sTT" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1])
        hltKey = int(subprocess.Popen("egrep 'UPLOAD HLTprescalekey' %sTT" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1])


        self.config.cfgKey.set(cfgKey)
        self.config.l1psKey.set(l1Key)
        self.config.hltpsKey.set(hltKey)
        self.rfch['ldb'][0]['selectcolor']='green'
        print >>self, "===>>> Success"
        return True
        


    #####################################################
    ###
    ### Running athenaMT from the DB
    ###
    #####################################################
    def clearMTdb(self):
        logfile = self.config.mtDbLog()
        base = self.config.jobName()
        try: os.remove(logfile)
        except OSError: pass
        try: os.remove("%s-1._0001.data" % base)
        except OSError: pass
        try: os.remove("%s-1._0001.data.writing" % base)
        except OSError: pass
        self.config.mtDbLog.set('')
        self.update()
    def runMTdb(self):
        self.setFrameName('MT DB')
        self.clearMTdb()
        if self.rfch['mtdb'][0]['selectcolor']=='red':
            print 'INFO: <runMTdb> clear ERROR first'
            return;
        if self.config.cfgKey()==0:
            print 'ERROR: <runMTdb> configuration key is 0'
            self.rfch['mtdb'][0]['selectcolor']='red'
            return False

        base = self.config.jobName()

        logfile = '%s/log%sMTdb' % (self.config.tmpDir(),base)

        cmd  = 'athenaMT.py'
        if self.config.onlineMonitoring(): cmd += ' --oh-monitoring'
        cmd += ' -J TrigConf::HLTJobOptionsSvc'
        cmd += ' -f %s -n %i -o %s -b' % (self.config.inputL2(),self.config.nEvt(),base)
        cmd += ' DBServer=%s:DBSMKey=%i:DBHLTPSKey=%i:DBLVL1PSKey=%i:Instance=L2' % (self.config.dbAlias(),self.config.cfgKey(),self.config.hltpsKey(),self.config.l1psKey())
        print >>self,'%s &>! %s' % (cmd,logfile)

        self.config.mtDbLog.set(logfile)
        self.update()
        try:
            FD = open(logfile,'w')
            retCode = subprocess.call(cmd.split(),stdout=FD,stderr=FD)
        except OSError, v:
            print 'ERROR: <runMTdb>',v
            self.rfch['mtdb'][0]['selectcolor']='red'
            return False
        FD.close()
        if retCode!=0:
            print 'ERROR: <runMTdb> job returned',retCode
            self.rfch['mtdb'][0]['selectcolor']='red'
            return False
        
        self.rfch['mtdb'][0]['selectcolor']='green'
        print >>self, "===>>> Success"
        return True



    #####################################################
    ###
    ### Running athenaPT from the DB
    ###
    #####################################################
    def clearPTdb(self):
        logfile = self.config.ptDbLog()
        try: os.remove(logfile)
        except OSError: pass
        self.config.ptDbLog.set('')
        self.update()
    def runPTdb(self):
        self.setFrameName('PT DB')
        if self.rfch['ptdb'][0]['selectcolor']=='red':
            print 'INFO: <runPTdb> clear ERROR first'
            return;
        if self.config.cfgKey()==0:
            print 'ERROR: <runPTdb> configuration key is 0'
            self.rfch['ptdb'][0]['selectcolor']='red'
            return False

        base = self.config.jobName()

        logfile = '%s/log%sPTdb' % (self.config.tmpDir(),base)

        cmd  = 'athenaPT.py'
        if self.config.onlineMonitoring(): cmd += ' --oh-monitoring'
        cmd += ' -J TrigConf::HLTJobOptionsSvc'
        cmd += ' -f %s -n %i -b' % (self.config.inputEF(),self.config.nEvt())
        cmd += ' DBServer=%s:DBSMKey=%i:DBHLTPSKey=%i:DBLVL1PSKey=%i:Instance=EF' % (self.config.dbAlias(),self.config.cfgKey(),self.config.hltpsKey(),self.config.l1psKey())
        print >>self,'%s &>! %s' % (cmd,logfile)

        self.config.ptDbLog.set(logfile)
        self.update()
        try:
            FD = open(logfile,'w')
            retCode = subprocess.call(cmd.split(),stdout=FD,stderr=FD)
        except OSError, v:
            print 'ERROR: <runPTdb>',v
            self.rfch['ptdb'][0]['selectcolor']='red'
            return False
        FD.close()
        if retCode!=0:
            print 'ERROR: <runPTdb> job returned',retCode
            self.rfch['ptdb'][0]['selectcolor']='red'
            return False
        
        self.rfch['ptdb'][0]['selectcolor']='green'
        print >>self, "===>>> Success"
        return True



    #####################################################
    ###
    ### Running the Result Checker
    ###
    #####################################################
    def clearCheckResult(self):
        logfile = self.config.CheckLog()
        try: os.remove(logfile)
        except OSError: pass
        self.config.CheckLog.set('')
        self.update()

    def runCheckResult(self):
        self.setFrameName('Check')
        self.testresults = {}
        resMtJo = self.getResultFromLog(self.config.mtJoLog())
        resMtDb = self.getResultFromLog(self.config.mtDbLog())
        self.testresults['mtjo'] = resMtJo
        self.testresults['mtdb'] = resMtDb
        print >>self,"Checking L2\n==========="
        successMT = self.checkResultInLevel(resMtJo, resMtDb)
        
        resPtJo = self.getResultFromLog(self.config.ptJoLog())
        resPtDb = self.getResultFromLog(self.config.ptDbLog())
        self.testresults['ptjo'] = resPtJo
        self.testresults['ptdb'] = resPtDb
        print >>self,"Checking EF\n==========="
        successPT = self.checkResultInLevel(resPtJo, resPtDb)

        success = successMT and successPT
        if success:
            print >>self,"Check successful! This database key returns the correct answers and can be used online!"
        else:
            print >>self,"Check failed! This database key leads to different results and can NOT be used online!"

        if True:
            logfile = '%s/log%sCheck' % (self.config.tmpDir(),self.config.jobName())
            cmd  = 'buildConfigInstaller.sh %s %s' % (self.config.jobName(), self.config.release())
            print >>self,'%s &>! %s' % (cmd,logfile)
            try:
                success = ( 0 == subprocess.call(cmd.split()) )
            except OSError, v:
                print 'ERROR: <runCheckResult>',v
                return False
        return success


    def checkResultInLevel(self, resJo, resDb):

        chainsJo=resJo.keys(); chainsJo.sort()
        chainsDb=resDb.keys(); chainsDb.sort()

        # common: [x for x in a for y in b if x==y]
        # only in one or the other: [x for x in a if x not in b]+[x for x in b if x not in a]
        chainsNotInBoth = [x for x in chainsJo if x not in chainsDb]+[x for x in chainsDb if x not in chainsJo]
        if len(chainsNotInBoth):
            print 'ERROR: <check> there are chains that are not in both'
            print chainsNotInBoth
            self.rfch['res'][0]['selectcolor']='red'
            return False

        failureChain = False
        for c in chainsJo:
            jo = resJo[c]
            db = resDb[c]
            if jo['raw']  != db['raw']  or \
               jo['ps']   != db['ps']   or \
               jo['pspt'] != db['pspt']:
                if jo['raw']  != db['raw']:
                    print >>self, 'Chain %s has different raw acceptance in JO (%s) and DB (%s)' % (c,jo['raw'],db['raw'])
                if jo['ps']  != db['ps']:
                    print >>self, 'Chain %s has different acceptance after ps in JO (%s) and DB (%s)' % (c,jo['ps'],db['ps'])
                if jo['pspt']  != db['pspt']:
                    print >>self, 'Chain %s has different acceptance after ps and pt in JO (%s) and DB (%s)' % (c,jo['pspt'],db['pspt'])
                failureChain = True
        if failureChain:
            print 'ERROR: <check> found mismatch'
            self.rfch['res'][0]['selectcolor']='red'
            return False

        self.rfch['res'][0]['selectcolor']='green'
        print >>self, "===>>> Success"
        print >>self, "Done"
        return True
            
    def getResultFromLog(self, fn):
        #self.clearCheckResult()
        p1 = subprocess.Popen(['grep', 'TrigChainMoniOnline', fn],stdout=subprocess.PIPE)
        p2 = subprocess.Popen(['grep', 'REGTEST events accepted'], stdin=p1.stdout, stdout=subprocess.PIPE)
        output = p2.communicate()[0].split('\n')
        p = re.compile('.*?events accepted by chain: (?P<chain>\w+?)\s+active: (?P<active>\d+?)\s+run :(?P<run>\d+?)\s+PS: (?P<ps>\d+?)\s+raw: (?P<raw>\d+?)\s+accepted after PS and PT: (?P<pspt>\d+?)\s+rerun: (?P<rerun>\d+?)')
        d = {}
        for l in output:
            m = p.match(l)
            if m:
                d[m.group('chain')] = m.groupdict()
                # to fake an error during testing
                #if fn==self.config.mtJoLog() and m.group('chain') == 'L2_te250': d[m.group('chain')]['raw']="18"
                #if fn==self.config.mtJoLog() and m.group('chain') == 'L2_2FJ70': d[m.group('chain')]['ps']="89"
        return d

    def displayResultMT(self):
        self.displayResult('mt')
        
    def displayResultPT(self):
        self.displayResult('pt')

    def displayResult(self, name):
        if name!='mt' and name!='pt':
            return
        if not (self.testresults.has_key('%sjo' % name) and self.testresults.has_key('%sdb' % name)):
            print "Results for %s are missing" % name
            return

        resJo=self.testresults['%sjo'% name]
        resDb=self.testresults['%sdb'% name]
        chainsJo=resJo.keys()
        chainsDb=resDb.keys()

        chains = chainsJo
        chains += [x for x in chainsDb if x not in chainsJo]
        chains.sort()

        maxlen=0
        for c in chains:
            if len(c)>maxlen: maxlen=len(c)

        # define header line
        space = ''
        while len(space)<maxlen-9: space += ' '
        head = "| idx |    ChainName %s| %10s | %10s | %10s | %10s | %10s | %10s |\n" % (space,'raw JO','ps JO','pspt JO','raw DB','ps DB','pspt DB')
        # define space line
        space = ''
        while len(space)<maxlen: space += '-'
        ul = "|-----|-----%s|------------|------------|------------|------------|------------|------------|\n" % space


        w = Toplevel(bg='lightblue')
        w.title('%s Comparison' % name)
        t = Text(w, width= len(head),height=44)
        t.pack(side=TOP,fill=BOTH,expand=1)
        t['bg'] = 'white'

        t.tag_config('error',background='yellow', foreground='red')
        t.tag_config('marker',background='grey95')

        currentLine = 1
        
        t.insert("1.0", ul)
        t.insert("%s+1l" % INSERT, head)
        t.insert("%s+1l" % INSERT, ul)
        for i,c in enumerate(chains):
            jo = resJo[c]
            db = resDb[c]
            joraw  = jo['raw']
            jops   = jo['ps']
            jopspt = jo['pspt']
            dbraw  = db['raw']
            dbps   = db['ps']
            dbpspt = db['pspt']
            tag = ''
            if joraw!=dbraw or jops!=dbps or jopspt!=dbpspt:
                tag = 'error'
            else:
                if (i%5)==4: tag = 'marker'
            space=''            
            while len(c)+len(space)<maxlen: space += ' '
            line = "| %3i |    %s%s | %10s | %10s | %10s | %10s | %10s | %10s |\n" % (i,c,space,jo['raw'],jo['ps'],jo['pspt'],db['raw'],db['ps'],db['pspt'])
            t.insert("%s+1l" % INSERT, line,tag)
            if (i % 40)==39:
                t.insert("%s+1l" % INSERT, ul)
                t.insert("%s+1l" % INSERT, head)
                t.insert("%s+1l" % INSERT, ul)
        t.insert("%s+1l" % INSERT, ul)

            






def usage():
    """Prints a help message"""
    print "usage:"
    print "%s [options] " % sys.argv[0]
    print "options are:"
    print "  -h|--help               this output"
    return
 


def main():

    short_opt = "h?F:"
    long_opt = ['help', 'options=']

    try:
        opts, args = getopt.getopt(sys.argv[1:], short_opt, long_opt)
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    # check for afs token
    if not "afs@cern.ch" in subprocess.Popen("tokens".split(),stdout=subprocess.PIPE).communicate()[0]:
        print "No afs token, will not be able to run!"
        sys.exit(0)

    inputFile = None
    # read in the options
    for o, v in opts:
        if o in ('-h', '-?', '--help'):
            usage()
            sys.exit(0)
        if o in ('-F', 'options'):
            inputFile = v

 
    root = Tk()
    
    canv=Canvas(root, width=640, height=850, relief=SUNKEN)
    canv.config(scrollregion=(0,0,400,850))
    canv.config(highlightthickness=0)
    sbary=Scrollbar(root)
    sbary.config(command=canv.yview)
    canv.config(yscrollcommand=sbary.set)
    sbary.pack(side=RIGHT, fill=Y)
    canv.pack(side=LEFT, expand=YES, fill=BOTH)
    frame = Frame(canv)
    frame.rowconfigure(1, weight=1)
    frame.columnconfigure(1, weight=1)
    canv.create_window(0, 0, anchor=NW, window=frame)
    frame.update_idletasks()

    app = App(frame,inputFile)
    root.mainloop()


if __name__ == '__main__':
    main()

