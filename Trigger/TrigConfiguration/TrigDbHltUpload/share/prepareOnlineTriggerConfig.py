#!/usr/bin/env python

import sys, subprocess, tempfile, os, re, signal
import __main__
from PyQt4 import QtGui, QtCore
from PyQt4.QtGui import QLabel, QCheckBox,QMessageBox

class StringVar:
    def __init__(self,value=""):
        self.value = value
    def set(self, value):
        self.value = str(value)
    def get(self):
        return self.value
    def __str__(self):
        return "'%s'" % self.value
    

class IntVar:
    def __init__(self,value=0):
        self.value = value
    def set(self, val):
        self.value = 0 if val=='' else int(val)
    def get(self):
        return self.value
    def __str__(self):
        return "%i" % self.value


class Option:
    def __init__(self, name, value, description, label, qtw=None):
        self.name        = name
        self.w_label     = QLabel(label)
        self.w_label.setToolTip(description)
        self.description = description
        self.type        = type(value)
        self.noRead      = False
        self.tkvar       = StringVar(value) if self.type==str else IntVar(value)
        if qtw!=None:
            self.qtw = qtw
        elif self.name=='userPreCommand':
            self.qtw = QtGui.QTextEdit()
            self.qtw.setMinimumHeight(60)
            self.qtw.textChanged.connect(self.textChanged)
        else:
            self.qtw = QtGui.QLineEdit()
            self.qtw.textEdited.connect(self.textEdited)
        self.set(value)

    def textEdited(self,value):
        self.tkvar.set(value)
    def textChanged(self):
        self.tkvar.set(self.qtw.toPlainText())
    def __call__(self,*args,**kwargs):
        return self.tkvar.get()
    def __str__(self):
        return "### %s\noption['%s'] = ( %s, '%s')\n" % (self.description, self.name, self.tkvar, self.description)
    def set(self,value):
        self.tkvar.set(value)
        if value!=None:
            self.qtw.setText(value if self.type==str else str(value))
    def clear(self):
        self.set('' if self.type==str else 0)
        
class BoolOption:
    def __init__(self, name, isChecked, description, label):
        self.name        = name
        self.cb          = QCheckBox(label)
        self.description = description
        self.cb.setCheckState(QtCore.Qt.Checked if isChecked else QtCore.Qt.Unchecked)
        self.cb.setToolTip(description)
    def __call__(self,*args,**kwargs):
        return self.cb.checkState() == QtCore.Qt.Checked
    def __str__(self):
        return "### %s\noption['%s'] = (%i, '%s')\n" % (self.description, self.name, self.__call__(), self.description)
    def set(self,isChecked):
        self.cb.setCheckState(QtCore.Qt.Checked if isChecked else QtCore.Qt.Unchecked)

class Process:
    def __init__(self, name, description, label):
        self.name        = name
        self.description = description
        self.w_label     = QCheckBox(label)
        self.w_label.setCheckState(QtCore.Qt.Checked)
        self.w_state     = QLabel()
        self.w_logfile   = QLabel()
        if name == 'jobCheckRes':
            self.w_showMT  = QtGui.QPushButton("Show MT Result")
            self.w_showPT  = QtGui.QPushButton("Show PT Result")
        self.w_clearbutton = QtGui.QPushButton("Clear")

    def __str__(self):
        return "### %s\nprocess['%s'] = (%i, '%s', '%s')\n" % (self.description, self.name, self.enabled, self.w_state.text(), self.logfile)
    @property
    def label(self):
        return self.w_label.text()
    @property
    def enabled(self):
        return self.w_label.checkState() == QtCore.Qt.Checked
    @property
    def logfile(self):
        return self.w_logfile.text()
    def clear(self):
        self.w_logfile.setText('')
    def setRunning(self):
        self.w_state.setText('running')
    def setSuccess(self):
        self.w_state.setText('success')
    def setError(self):
        self.w_state.setText('error')
    def setSkipped(self):
        self.w_state.setText('skipped')
    def setKilled(self):
        self.w_state.setText('killed')
    def clearError(self):
        self.w_state.setText('')
    def setState(self,state):
        self.w_state.setText(state)
    def inError(self):
        return self.w_state.text() == 'error'
    def isRunning(self):
        return self.w_state.text() == 'running'
    def isKilled(self):
        return self.w_state.text() == 'killed'
    def setLogfile(self,logfile):
        self.w_logfile.setText(logfile)
    def setEnabled(self, enabled):
        self.w_label.setCheckState(QtCore.Qt.Checked if enabled else QtCore.Qt.Unchecked)
        
class Config:
    def __init__(self, configFile=None):
        self.options = {}
        self.processes = {}
        self.menuFromJOfile = None
        self.menus = None
        self.inputFile = configFile
        self.excempt = set()
        self.setupDefault()
        self.read()
        self.findJobOptions()

    def __getattr__(self,name):
        if name in self.options:      return self.options[name]
        elif name in self.processes:  return self.processes[name]
        else:
            raise KeyError, "'%s' is not an option or process" % name

    def __setattr__(self,name,value):
        if 'options' in self.__dict__ and name in self.options:
            self.options[name].set(value)
        else:
            self.__dict__[name] = value
    
    def addOption(self, name, value, description, label, qtw=None):
        self.options[name] = Option(name, value, description, label, qtw)

    def addBoolOption(self, name, label, description, isChecked):
        self.options[name] = BoolOption(name, 1 if isChecked else 0, description, label)

    def addProcess(self,name,description, label):
        self.processes[name] = Process(name, description, label)

    def dontStore(self,*names):
        for name in names:
            self.excempt.add(name)

    def setupDefault(self):
        import os
        tempdir = os.environ["TMPDIR"] if "TMPDIR" in os.environ else (tempfile.gettempdir() + '/' + os.environ["USER"])
        # red 
        self.addOption('release','',"The release tag",  'Release:'      , QtGui.QLabel())
        self.addOption('patch',  '',"The patch tag",    'Patch:'        , QtGui.QLabel())
        self.addOption('swpath','',"The software path", 'Software path:', QtGui.QLabel())
        self.addOption('tt','/afs/cern.ch/user/a/attrgcnf/TriggerTool/TriggerTool-new.jar',"The TriggerTool jar-file", 'TriggerTool:')
        # green
        self.addOption('jobName', 'Default',"Job name", 'Job name:')
        self.addOption('jobOption','',"The python job option file", 'Joboption file')
        self.addOption('userPreCommand','',"The users precommands to be run", 'Pre-command:')
        self.addOption('setupDir','./setup',"Directory for setup and xml files (a subdir for each job is created)", 'Output directory:')
        self.addOption('tmpDir',tempdir,"Directory for temporary results", 'Temp directory:')
        self.addOption('nEvt',10,"Number of Events", 'Number of Events:')
        self.addOption('inputL2','input.data',"Input data file L2", 'MT input data:')
        self.addOption('inputEF','',          "Input data file EF", 'PT input data:')
        self.addOption('l1menu',   '',"LVL1 menu file",   'L1 menu:')
        self.addOption('hltl2menu','',"HLT L2 menu file", 'L2 menu:')
        self.addOption('hltefmenu','',"HLT EF menu file", 'EF menu:')
        self.addOption('setupL2','',"Setup file L2", 'L2 setup:')
        self.addOption('setupEF','',"Setup file EF", 'EF setup:')
        self.addOption('dbAlias','TRIGGERDBREPR',"Alias for TriggerDB connection", 'DB connection')
        self.addOption('dbPW','',"Password for writer accound (not needed by experts)", 'DB password')
        self.addOption('cfgKey',0,"Configuration key", 'Configuration key:')
        self.addOption('l1psKey',0,"LVL1 prescale key", 'LVL1 prescale key:')
        self.addOption('hltpsKey',0,"HLT prescale key", 'HLT prescale key:')
        self.addBoolOption('onlineMonitoring', 'Online Monitoring', 'Online Monitoring switch (e.g. online histogram service)', True)
        self.addBoolOption('cafConfig', 'Reprocessing', 'Configuration for reprocessings (enable additional monitoring)', False)
        self.addBoolOption('l1rerun', 'Rerun Level 1', 'Rerun level 1 simulation (should be on for CAF)', False)
        self.dontStore('cafConfig', 'l1rerun', 'release', 'patch', 'swpath') # excempt from storage option to avoid accidental use for online
        # blue
        self.addProcess('jobMTjo',     "Run athenaMT from JobOptions", 'run athenaMT from JO')
        self.addProcess('jobPTjo',     "Run athenaPT from JobOptions", 'run athenaPT from JO')
        self.addProcess('jobSetupCnv', "Run setup conversion script",  'convert setup')
        self.addProcess('jobUploadDb', "Load TriggerDB",               'load TriggerDB')
        self.addProcess('jobMTdb',     "Run athenaMT from TriggerDB",  'run athenaMT from DB')
        self.addProcess('jobPTdb',     "Run athenaPT from TriggerDB",  'run athenaPT from DB')
        self.addProcess('jobCheckRes', "Compare counts",               'check results')
        self.jobOption.qtw.textChanged.connect(self.readMenus)
        self.cafConfig.cb.stateChanged.connect(self.toggleModifierCAF)
        self.l1rerun.cb.stateChanged.connect(self.toggleL1RerunOption)

    def toggleModifierCAF(self):
        self.setPreCommandModifier('caf', 'True' if self.cafConfig() else None)

    def toggleL1RerunOption(self):
        self.setPreCommandModifier('rerunLVL1', 'True' if self.l1rerun() else None)


    def read(self):
        if self.inputFile==None: return
        if not os.path.exists(self.inputFile):
            print "Configuration file %s can not be accessed!" % self.inputFile
            return
        print "Reading configuration from %s!" % self.inputFile
        execfile(self.inputFile,__main__.__dict__)
        for name, (value, desc) in option.iteritems():
            if name in ['athenaMTjo','athenaMTdb','athenaPTjo','athenaPTdb','checkRes','uploadDb','setupCnv','mtJoLog', 'ptJoLog','mtDbLog', 'ptDbLog', 'cnvLog', 'loadDbLog']: continue  # for backward compatibility
            self.options[name].set(value)
        if 'process' in dir(__main__):
            for name, content in process.iteritems():
                if len(content)==3:
                    (enabled, state, logfile) = content
                    self.processes[name].setEnabled(enabled)
                    self.processes[name].setState(state)
                    self.processes[name].setLogfile(logfile)
                else:
                    (enabled, logfile) = content
                    self.processes[name].setEnabled(enabled)
                    self.processes[name].setState('')
                    self.processes[name].setLogfile(logfile)
        self.toggleModifierCAF()
        self.toggleL1RerunOption()
        
    def findJobOptions(self):
        if self.jobOption()=='':
            for p in os.getenv('JOBOPTSEARCHPATH').split(os.pathsep)+['./']:
                if not os.path.exists(p+'/TriggerRelease/runHLT_standalone.py'): continue
                self.jobOption = p+'/TriggerRelease/runHLT_standalone.py'
                break
            
    def readMenus(self):
        filename = self.jobOption()
        if not os.path.exists(filename): return  # JO files doesn't exist
        if self.menuFromJOfile==filename: return # menu from these JO already loaded
        self.menuFromJOfile=filename # keep track of what is loaded
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
            self.menus = sorted(menuMap.keys())
        except:
            self.menus = None

    def setPreCommandModifier(self, name, value):
        pc = self.userPreCommand().strip(';')
        mods = {} if not pc else dict([tuple(m.split('=')) for m in pc.split(';')])
        if value==None:
            if name in mods: mods.pop(name)
        else:
            mods[name] = value
        pc = ';'.join(['='.join([k,v]) for k,v in sorted(mods.iteritems())])
        self.userPreCommand.set(pc)            

    def write(self):
        outputfile = 'myPref_%s.py' % self.jobName()
        print "Saving configuration to %s !" % outputfile
        f = open(outputfile, 'w')
        print >>f, 'option = {}'
        print >>f, 'process = {}\n'
        for opt in self.options:
            if opt in self.excempt: continue
            print >>f, self.options[opt]
        for pro in self.processes:
            if pro in self.excempt: continue
            print >>f, self.processes[pro]
        f.close()


class KeyPrep(QtGui.QScrollArea):
    def __init__(self, configFile, parent=None):
        super(KeyPrep,self).__init__(parent)
        self.config = Config(configFile)
        self.summarylog = None
        self.guessMenuInPreCommand()
        self.elements()
        self.layout()
        self.checkReleaseAndTT()
        self.updateMenuComboBox()
        self.runThread = KeyPrep.RunThread(self)
        self.currentSubprocess = None

    def position(self, w=550, h=870, m=30):
        desktop = QtGui.QApplication.desktop()
        dw = desktop.availableGeometry().width()
        dh = desktop.availableGeometry().height()
        h = min(h,dh-2*m)
        #self.setGeometry(700, 30, 550, 750)
        self.setGeometry(dw-m-w, m, w, h)
        
    def elements(self):
        self.position()
        self.setWindowTitle('Prepare Trigger Configuration')

        self.gr_relconf = {
            'box' : QtGui.QGroupBox("Release configuration"),
            'cont': [ self.config.release, self.config.patch, self.config.swpath, self.config.tt ]
            }
        self.gr_relconf['box'].setStyleSheet("QGroupBox { background-color: #8CDBFF; }");


        self.gr_setup = {
            'box' : QtGui.QGroupBox("Job configuration"),
            'cont': [ self.config.jobName, self.config.jobOption, self.config.userPreCommand, self.config.setupDir,
                      self.config.tmpDir, self.config.dbAlias, self.config.dbPW, self.config.nEvt, self.config.inputL2,
                      self.config.inputEF, self.config.l1menu, self.config.hltl2menu, self.config.hltefmenu,
                      self.config.setupL2, self.config.setupEF, self.config.cfgKey, self.config.l1psKey, self.config.hltpsKey ]
            }
        self.gr_setup['box'].setStyleSheet("QGroupBox { background-color: #FFE430; }");

        self.gr_run = {
            'box' : QtGui.QGroupBox("Run state"),
            'cont': [ self.config.jobMTjo, self.config.jobPTjo, self.config.jobSetupCnv, self.config.jobUploadDb,
                      self.config.jobMTdb, self.config.jobPTdb, self.config.jobCheckRes ]
            }
        self.gr_run['box'].setStyleSheet("QGroupBox { background-color: #A5FF00; }");

        self.config.jobCheckRes.w_showMT.clicked.connect(self.displayResultMT)
        self.config.jobCheckRes.w_showPT.clicked.connect(self.displayResultPT)

        self.d_menu = QtGui.QComboBox()
        self.b_save = QtGui.QPushButton("Save",self)
        self.b_kill = QtGui.QPushButton("Kill Process",self)
        self.b_run  = QtGui.QPushButton("Run",self)
        self.b_quit = QtGui.QPushButton("Quit",self)
        self.connect(self.b_run,  QtCore.SIGNAL('clicked()'), self.run)
        self.connect(self.b_kill, QtCore.SIGNAL('clicked()'), self.killCurrentProcess)
        self.connect(self.b_save, QtCore.SIGNAL('clicked()'), self.config.write)
        self.connect(self.b_quit, QtCore.SIGNAL('clicked()'), self.quit)

    def layout(self):
        top = QtGui.QVBoxLayout()

        # release info
        grid = QtGui.QGridLayout()
        for i,c in enumerate(self.gr_relconf['cont']):
            grid.addWidget(c.w_label,i,0,1,1)
            grid.addWidget(c.qtw,i,1,1,3)
        self.gr_relconf['box'].setLayout(grid)
        top.addWidget(self.gr_relconf['box'])

        # job setup
        grid = QtGui.QGridLayout()
        grid.setVerticalSpacing(0)
        line = 0
        for c in self.gr_setup['cont']:
            if c.name=='userPreCommand':
                grid.addWidget(c.w_label,line,0) # label
                grid.addWidget(self.d_menu,line+1,0,QtCore.Qt.AlignTop) # menu combo box
                grid.addWidget(c.qtw,line,1,2,7) # field
                line += 2
            else:
                grid.addWidget(c.w_label,line,0) # label
                grid.addWidget(c.qtw,line,1,1,7) # field
                line += 1
        grid.addItem(QtGui.QSpacerItem(10,10),line,0,1,4) # space
        hl = QtGui.QHBoxLayout() # horizontal box for buttons
        hl.addWidget(self.config.onlineMonitoring.cb)
        hl.addWidget(self.config.cafConfig.cb)
        hl.addWidget(self.config.l1rerun.cb)
        hl.addStretch(1)
        hl.addWidget(self.b_save)
        grid.addLayout(hl,line+1,0,1,8)
        self.gr_setup['box'].setLayout(grid)
        top.addWidget(self.gr_setup['box'])

        # job progress
        grid = QtGui.QGridLayout()
        grid.setVerticalSpacing(0)
        line = 0
        for c in self.gr_run['cont']:
            grid.addWidget(c.w_label,line,0) # label
            grid.addWidget(c.w_state,line,1) # state
            if c.name != 'jobCheckRes':
                grid.addWidget(c.w_logfile,line,2,1,6) # field
                #c.w_logfile.setStyleSheet("QLineEdit { background-color: rgb(255, 176, 165); }");
            else:
                grid.addWidget(c.w_showMT,line,6) # show res button
                grid.addWidget(c.w_showPT,line,7) # show res button
            #grid.addWidget(c.w_clearbutton,line,9) # clear button
            line += 1
        self.gr_run['box'].setLayout(grid)
        top.addWidget(self.gr_run['box'])

        actions = QtGui.QHBoxLayout()
        #actions.addWidget(self.b_style)
        actions.addWidget(self.b_kill)
        actions.addStretch(1)
        actions.addWidget(self.b_run)
        actions.addWidget(self.b_quit)
        top.addLayout(actions)

        #self.setLayout(top)

        frame = QtGui.QFrame()
        frame.setLayout(top)
        self.setWidget(frame)
        self.setWidgetResizable(True)



    def updateMenuComboBox(self):
        menus = self.config.menus
        try:
            self.d_menu.currentIndexChanged.disconnect() # avoid signals during filling of combo-box
        except: pass
        self.d_menu.clear()
        for m in menus:
            self.d_menu.addItem(m)
        self.d_menu.currentIndexChanged[str].connect(self.selectMenu)

        index=-1
        if self.menuInPreCommand:
            index = menus.index(self.menuInPreCommand)
        elif "PhysicsV4" in menus:
            index = menus.index("PhysicsV4")            
        elif "PhysicsV3" in menus:
            index = menus.index("PhysicsV3")
        elif "PhysicsV2" in menus:
            index = menus.index("PhysicsV2")
        if index>=0:
            self.d_menu.setCurrentIndex(index)
          
    def selectMenu(self, menu):
        if self.menuInPreCommand!=None: # remove the old one first
            self.config.setPreCommandModifier('test%s' % self.menuInPreCommand, None)
        self.config.setPreCommandModifier('test%s' % menu, 'True') # set the selected one
        self.menuInPreCommand = menu

    def quit(self):
        self.config.write()
        QtGui.qApp.quit()

    def setFrameTitle(self,title):
        self.setWindowTitle(title)

    def addFrameTitle(self,title):
        self.setWindowTitle(self.windowTitle() + title)


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

    def guessMenuInPreCommand(self):
        pc = self.config.userPreCommand().strip(';')
        mods = {} if not pc else dict([tuple(m.split('=')) for m in pc.split(';')])
        p = re.compile("test.*")
        menucands = filter(p.match,mods.keys())
        self.menuInPreCommand = menucands[0][4:] if menucands else None

    def write(self,s):
        if not self.summarylog:
            base = self.config.jobName()
            mode = "w" if self.config.jobMTjo.enabled else "a"
            self.summarylog=open('%s/log%sSummary' % (self.config.tmpDir(),base), mode)
        print s,
        print >>self.summarylog, s,
        self.summarylog.flush()

    class RunThread(QtCore.QThread):
        def __init__(self, gui):
            super(KeyPrep.RunThread,self).__init__()
            self.gui = gui
            self.success = True

        def run(self):
            for job in ['MTjo', 'PTjo', 'SetupCnv', 'UploadDb', 'MTdb', 'PTdb', 'CheckRes']:
                proc_conf = getattr(self.gui.config, "job%s" % job)
                if not proc_conf.enabled:
                    proc_conf.setSkipped()
                    continue # skip unchecked jobs

                clearproc  = eval("self.gui.clear%s" % job)
                prereqproc = eval("self.gui.checkPreReq%s" % job)
                proc       = eval("self.gui.run%s" % job)
                self.gui.setFrameTitle(job)
                proc_conf.clearError()
                clearproc() # clear
                proc_conf.setRunning()
                if not prereqproc(proc_conf): # check for input
                    if proc_conf.inError():
                        self.gui.addFrameTitle(' -> Failure')
                    self.success = False
                    return
                if not proc(proc_conf): # exec
                    if proc_conf.inError(): self.gui.addFrameTitle(' -> Failure')
                    if proc_conf.isKilled(): self.gui.addFrameTitle(' -> Killed')
                    self.success = False
                    return
                proc_conf.setSuccess()
                print >>self.gui, "===>>> Success"

            print "===>>> Success"
            self.gui.setFrameTitle('Success')
            self.success = True
            return

    def run(self):
        import os
        tmpdir = self.config.tmpDir()
        if os.path.exists(tmpdir) and not os.path.isdir(tmpdir):
            raise RuntimeError("%s exists but is not a directory and can't be used for writing out temporary files" % tmpdir)
        if not os.path.exists(tmpdir): # create tmp dir
            try:
                os.makedirs(tmpdir)
            except OSError, e:
                raise RuntimeError("Can't create temporary directory %s (reason: %s)" % (tmpdir,e) )
        self.runThread.start() # starts the thread

    def killCurrentProcess(self):
        if self.runThread.isRunning():
            runningProcess = None
            for pr in self.config.processes.values():
                if pr.isRunning():
                    runningProcess=pr
                    break
            if QMessageBox.question(None, '', "Are you sure you want to kill \nthe process '' ?" % runningProcess.label,
                                    QMessageBox.Yes | QMessageBox.No,
                                    QMessageBox.No) == QMessageBox.Yes:
                if self.currentSubprocess:
                    self.currentSubprocess.kill()
        else:
            QMessageBox.information(None, '', "No process running!")



    # helper functions
    # check existence of file
    def checkFile(self,proc_conf, filename,filedesc):
        caller = proc_conf.name
        if filename=='':
            print 'ERROR: <%s> No %s configured' % (caller, filedesc)
            proc_conf.setError()
            return False
        if not os.path.exists(filename):
            print 'ERROR: <%s> %s %s does not exist in %s' % (caller, filedesc, filename, os.path.realpath('.'))
            proc_conf.setError()
            return False
        return True

    # check input field
    def checkInput(self,proc_conf, field, value):
        if getattr(self.config,field)() == value:
            print "ERROR: <%s> sensible value for input parameter '%s' expected, but it is '%r'" % (proc_conf.name, field, value)
            proc_conf.setError()
            return False
        return True

    # delete file in working dir
    def deleteLocalFile(self, filename):
        try: os.remove(filename)
        except OSError: return False
        return True

    # delete file in tmp dir
    def deleteTmpFile(self, filename):
        try: os.remove("%s/%s" % (self.config.tmpDir(), filename))
        except OSError: return False
        return True

    # clear all options in optlist
    def clearOptions(self, optlist):
        for opt in optlist:
            getattr(self.config,opt).clear()

    # execute cmd (string)
    def executeProcess(self, cmd, proc_conf):
        try:
            FD = open(proc_conf.logfile,'w')
            self.currentSubprocess = subprocess.Popen(cmd.split(),stdout=FD,stderr=FD)
            self.currentSubprocess.wait()
            retCode = self.currentSubprocess.returncode
            self.currentSubprocess = None
        except OSError, v:
            print 'ERROR: <%s> %r' % (proc_conf.name,v)
            proc_conf.setError()
            FD.close()
            return False
        FD.close()
        if retCode==-9:
            proc_conf.setKilled()
            return False
        if retCode!=0:
            print 'ERROR: <%s> job failed with exit code: %r' % (proc_conf.name, retCode)
            proc_conf.setError()
            return False
        return True

    #####################################################
    ###
    ### Running athenaMT from the JO
    ###
    #####################################################
    def clearMTjo(self):
        base = self.config.jobName()
        self.deleteLocalFile("%s-1._0001.data" % base)
        self.deleteLocalFile("%s-1._0001.data.writing" % base)
        self.deleteTmpFile('log%sMT' % base)
        self.clearOptions( ['l1menu', 'hltl2menu', 'hltefmenu', 'setupL2', 'inputEF', 'jobMTjo'] )

    def checkPreReqMTjo(self,proc_conf):
        return self.checkInput(proc_conf, 'inputL2', '') and \
               self.checkFile(proc_conf, self.config.inputL2(), "Input file")

    def runMTjo(self, proc_conf):
        base    = self.config.jobName()
        logfile = '%s/log%sMT' % (self.config.tmpDir(),base)
        proc_conf.setLogfile(logfile)


        args = [ '-n %i' % self.config.nEvt(),
                 '-o %s' % base,
                 "-c doDBConfig=True",
                 "-c trigBase='\"%s\"'" % base,
                 "" if self.config.userPreCommand().strip()=='' else " -c '%s'"  % self.config.userPreCommand().strip(),
                 '-f %s' % self.config.inputL2(),
                 '--oh-monitoring' if self.config.onlineMonitoring() else '' ]
        cmd = 'athenaMT.py %s %s' % ( " ".join(args), self.config.jobOption() )
        print >>self, '%s &>! %s' % (cmd,logfile)

        if not self.executeProcess(cmd, proc_conf): return False

        try:
            self.config.l1menu     = subprocess.Popen("egrep 'DBCONFIG LVL1XML' %s" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1]
            self.config.hltl2menu  = subprocess.Popen("egrep 'DBCONFIG HLTXML' %s" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1]
            self.config.inputEF    = "%s-1._0001.data" % base
            self.config.setupL2    = './l2_%s_setup.txt' % base
        except:
            return False
        return True

    #####################################################
    ###
    ### athenaPT from JobOptions
    ###
    #####################################################
    def clearPTjo(self):
        base = self.config.jobName()
        self.deleteLocalFile("ef_%s_setup.txt" % base)
        self.deleteLocalFile("%s-1._0001.data.writing" % base)
        self.deleteTmpFile('log%sPT' % base)
        self.clearOptions( ['hltefmenu', 'setupEF', 'jobPTjo'] )

    def checkPreReqPTjo(self,proc_conf):
        return self.checkInput(proc_conf, 'inputEF', '') and \
               self.checkFile(proc_conf, self.config.inputEF(), "Input file")

    def runPTjo(self, proc_conf):
        base = self.config.jobName()
        logfile = '%s/log%sPT' % (self.config.tmpDir(),base)
        proc_conf.setLogfile(logfile)

        args = [ '-n %i' % self.config.nEvt(),
                 '-c doDBConfig=True',
                 "-c trigBase='\"%s\"'" % base,
                 "" if self.config.userPreCommand().strip()=='' else " -c '%s'"  % self.config.userPreCommand().strip(),
                 '-f %s' % self.config.inputEF(),
                 '--oh-monitoring' if self.config.onlineMonitoring() else '' ]
        cmd = 'athenaPT.py %s %s' % ( " ".join(args), self.config.jobOption() )
        print >>self, '%s &>! %s' % (cmd,logfile)

        if not self.executeProcess(cmd, proc_conf): return False

        try:
            self.config.hltefmenu = subprocess.Popen("egrep 'DBCONFIG HLTXML' %s" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1]
            self.config.setupEF   = './ef_%s_setup.txt' % base
        except:
            return False
        return True


    #####################################################
    ###
    ### setup conversion
    ###
    #####################################################
    def clearSetupCnv(self):
        base = self.config.jobName()
        fullSetupDir = '%s/%s' % (self.config.setupDir(), base)
        subprocess.call(('rm -r %s' % fullSetupDir).split())
        subprocess.call(['mkdir', '-p', fullSetupDir])
        self.deleteTmpFile('log%sConv' % base)
        self.clearOptions( ['jobSetupCnv'] )

    def checkPreReqSetupCnv(self,proc_conf):
        allfiles = \
                 self.checkFile(proc_conf, self.config.setupL2(), "L2 setup file") and \
                 self.checkFile(proc_conf, self.config.setupEF(), 'EF setup file') and \
                 self.checkFile(proc_conf, self.config.hltl2menu(), 'HLT L2 menu file') and \
                 self.checkFile(proc_conf, self.config.hltefmenu(), 'HLT EF menu file')
        if not allfiles: return False
        return self.checkInput(proc_conf, 'l1menu', '')

    def runSetupCnv(self, proc_conf):
        # check for level 1 menu
        base = self.config.jobName()
        fullSetupDir = '%s/%s' % (self.config.setupDir(), base)
        l1menuName = os.path.basename(self.config.l1menu())
        subprocess.call( ('cp -f %s .' % self.config.l1menu()).split() ) 
        #l1menuName = os.path.basename(self.config.l1menu())
        #if not os.path.exists(l1menuName):
        #    subprocess.call(('get_files -xmls %s' % l1menuName).split())
        if not self.checkFile(proc_conf, l1menuName, "L1 menu file after running get_files"):
            return False
        
        logfile = '%s/log%sConv' % (self.config.tmpDir(),base)
        proc_conf.setLogfile(logfile)

        subprocess.call(['cp', self.config.setupL2(), fullSetupDir+'/l2setup.txt'])
        subprocess.call(['cp', self.config.setupEF(), fullSetupDir+'/efsetup.txt'])
        subprocess.call(['cp', l1menuName,            fullSetupDir+'/lvl1Menu.xml'])
        subprocess.call(['cp', self.config.hltl2menu(), fullSetupDir+'/hltL2Menu.xml'])
        subprocess.call(['cp', self.config.hltefmenu(), fullSetupDir+'/hltEFMenu.xml'])
        subprocess.call(['cp', self.config.setupL2().replace('.txt','_setup.txt'), fullSetupDir+'/l2setuppy.txt'])
        subprocess.call(['cp', self.config.setupEF().replace('.txt','_setup.txt'), fullSetupDir+'/efsetuppy.txt'])

        cwd = os.getcwd()
        os.chdir(fullSetupDir)

        cmd = 'menumerge.py --l2menu hltL2Menu.xml --efmenu hltEFMenu.xml --output hltMenu.xml'
        print '%s &>! %s' % (cmd,logfile)
        if not self.executeProcess(cmd, proc_conf): return False


        cmd = 'ConvertHLTSetup_txt2xml.py l2setup.txt l2setuppy.txt'
        print >>self,'%s &>>! %s' % (cmd,logfile)
        if not self.executeProcess(cmd, proc_conf): return False

        cmd = 'ConvertHLTSetup_txt2xml.py efsetup.txt efsetuppy.txt'
        print >>self,'%s &>>! %s' % (cmd,logfile)
        if not self.executeProcess(cmd, proc_conf): return False

        os.chdir(cwd)
        return True


    #####################################################
    ###
    ### Upload into TriggerDB
    ###
    #####################################################
    def clearUploadDb(self):
        base = self.config.jobName()
        self.deleteTmpFile('log%sUpload' % base)
        self.clearOptions( ['jobUploadDb','cfgKey'] )

    def checkPreReqUploadDb(self,proc_conf):
        fullSetupDir = '%s/%s' % (self.config.setupDir(), self.config.jobName())
        cwd = os.getcwd()
        os.chdir(fullSetupDir)
        allfiles = \
                 self.checkFile(proc_conf, 'l2setup.xml', 'L2 setup file') and \
                 self.checkFile(proc_conf, 'efsetup.xml', 'EF setup file') and \
                 self.checkFile(proc_conf, 'hltMenu.xml', 'HLT menu file') and \
                 self.checkFile(proc_conf, 'lvl1Menu.xml','LVL1 menu file')
        os.chdir(cwd)
        return allfiles

    def runUploadDb(self,proc_conf):
        base = self.config.jobName()
        fullSetupDir = '%s/%s' % (self.config.setupDir(),base)
        java='/afs/cern.ch/sw/lcg/external/Java/JDK/1.6.0/ia32/jre/bin/java -Xms512m -Xmx2000m'
        TT=self.config.tt()
        
        logfile = '%s/log%sUpload' % (self.config.tmpDir(),base)
        proc_conf.setLogfile(logfile)

        if self.config.dbPW()=="":
            args = [' -db %s' % self.config.dbAlias()]
        else:
            alias = self.config.dbAlias()
            if alias=="TRIGGERDBDEV1": shortAlias = "DEV1"
            elif alias=="TRIGGERDBDEV2": shortAlias = "DEV2"
            elif alias=="TRIGGERDBMC": shortAlias = "DBMC"
            else: shortAlias = alias
            args = ['-db %s' % shortAlias,
                    '-pw %s' % self.config.dbPW()]
        args += ['-up',
                 '-l1  %s/lvl1Menu.xml' % fullSetupDir,
                 '-hlt %s/hltMenu.xml'  % fullSetupDir,
                 '-l2s %s/l2setup.xml'  % fullSetupDir,
                 '-efs %s/efsetup.xml'  % fullSetupDir,
                 '-rel %s'  % self.config.release(),
                 '-o %sTT'  % logfile ]

        cmd  = '%s -jar %s %s' % (java,TT,' '.join(args))
        print >>self,'%s &>! %s' % (cmd,logfile)
        if not self.executeProcess(cmd, proc_conf): return False

        self.config.cfgKey   = int(subprocess.Popen("egrep 'UPLOAD Supermasterkey' %sTT" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1])
        self.config.l1psKey  = int(subprocess.Popen("egrep 'UPLOAD LVL1prescalekey' %sTT" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1])
        self.config.hltpsKey = int(subprocess.Popen("egrep 'UPLOAD HLTprescalekey' %sTT" % logfile, shell=True,stdout=subprocess.PIPE).communicate()[0].split()[-1])
        return True



    #####################################################
    ###
    ### Running athenaMT from the DB
    ###
    #####################################################
    def clearMTdb(self):
        base = self.config.jobName()
        self.deleteLocalFile("%s-1._0001.data" % base)
        self.deleteLocalFile("%s-1._0001.data.writing" % base)
        self.deleteTmpFile('log%sMTdb' % base)
        self.clearOptions( ['jobMTdb'] )
    def checkPreReqMTdb(self,proc_conf):
        return self.checkInput(proc_conf, 'cfgKey', 0) and \
                   self.checkInput(proc_conf, 'l1psKey', 0) and \
                   self.checkInput(proc_conf, 'hltpsKey', 0)
    def runMTdb(self,proc_conf):
        base = self.config.jobName()
        logfile = '%s/log%sMTdb' % (self.config.tmpDir(),base)
        proc_conf.setLogfile(logfile)

        args = [ '-f %s' % self.config.inputL2(),
                 '-n %i' % self.config.nEvt(),
                 '-o %s' % base,
                 '--oh-monitoring' if self.config.onlineMonitoring() else '',
                 '-J TrigConf::HLTJobOptionsSvc',
                 '-b DBServer=%s:DBSMKey=%i:DBHLTPSKey=%i:DBLVL1PSKey=%i:Instance=L2' % (self.config.dbAlias(),self.config.cfgKey(),self.config.hltpsKey(),self.config.l1psKey()) ]
        cmd  = 'athenaMT.py %s' % ' '.join(args)
        print >>self,'%s &>! %s' % (cmd,logfile)
        if not self.executeProcess(cmd, proc_conf): return False
        return True

    #####################################################
    ###
    ### Running athenaPT from the DB
    ###
    #####################################################
    def clearPTdb(self):
        base = self.config.jobName()
        self.deleteTmpFile('log%sPTdb' % base)
        self.clearOptions( ['jobPTdb'] )

    def checkPreReqPTdb(self,proc_conf):
        return self.checkInput(proc_conf, 'cfgKey', 0)

    def runPTdb(self,proc_conf):
        base = self.config.jobName()
        logfile = '%s/log%sPTdb' % (self.config.tmpDir(),base)
        proc_conf.setLogfile(logfile)

        args = [ '-f %s' % self.config.inputEF(),
                 '-n %i' % self.config.nEvt(),
                 '--oh-monitoring' if self.config.onlineMonitoring() else '',
                 '-J TrigConf::HLTJobOptionsSvc',
                 '-b DBServer=%s:DBSMKey=%i:DBHLTPSKey=%i:DBLVL1PSKey=%i:Instance=EF' % (self.config.dbAlias(),self.config.cfgKey(),self.config.hltpsKey(),self.config.l1psKey()) ]
        cmd  = 'athenaPT.py %s' % ' '.join(args)
        print >>self,'%s &>! %s' % (cmd,logfile)
        if not self.executeProcess(cmd, proc_conf): return False
        return True


    #####################################################
    ###
    ### Running the Result Checker
    ###
    #####################################################
    def clearCheckRes(self):
        base = self.config.jobName()
        self.deleteTmpFile('log%sCheckRes' % base)
        self.clearOptions( ['jobCheckRes'] )

    def checkPreReqCheckRes(self, proc_conf):
        allfiles = \
                 self.checkFile(proc_conf, self.config.jobMTjo.logfile, "L2 JO log file") and \
                 self.checkFile(proc_conf, self.config.jobMTdb.logfile, 'L2 DB log file') and \
                 self.checkFile(proc_conf, self.config.jobPTjo.logfile, "EF JO log file") and \
                 self.checkFile(proc_conf, self.config.jobPTdb.logfile, 'EF DB log file')
        return allfiles

    def runCheckRes(self, proc_conf):
        base = self.config.jobName()
        resMtJo = self.getResultFromLog(self.config.jobMTjo.logfile)
        resMtDb = self.getResultFromLog(self.config.jobMTdb.logfile)
        self.testresults = {
            'mtjo' : resMtJo,
            'mtdb' : resMtDb
            }
        print >>self,"Checking L2\n==========="
        successMT = self.checkResultInLevel(proc_conf, resMtJo, resMtDb)
        
        resPtJo = self.getResultFromLog(self.config.jobPTjo.logfile)
        resPtDb = self.getResultFromLog(self.config.jobPTdb.logfile)
        self.testresults['ptjo'] = resPtJo
        self.testresults['ptdb'] = resPtDb
        print >>self,"Checking EF\n==========="
        successPT = self.checkResultInLevel(proc_conf, resPtJo, resPtDb)

        success = successMT and successPT
        if success:
            print >>self,"Check successful! This database key returns the correct answers and can be used online!"
        else:
            print >>self,"Check failed! This database key leads to different results and can NOT be used online!"

        if True:
            logfile = '%s/log%sCheckRes' % (self.config.tmpDir(),base)
            cmd  = 'buildConfigInstaller.sh %s %s' % (self.config.jobName(), self.config.release())
            print >>self,'%s &>! %s' % (cmd,logfile)
            try:
                successBuildInstaller = ( 0 == subprocess.call(cmd.split()) )
            except OSError, v:
                print 'ERROR: <runCheckResult>',v
                return False
        return success and successBuildInstaller


    def checkResultInLevel(self, proc_conf, resJo, resDb):

        chainsJo=resJo.keys(); chainsJo.sort()
        chainsDb=resDb.keys(); chainsDb.sort()

        # common: [x for x in a for y in b if x==y]
        # only in one or the other: [x for x in a if x not in b]+[x for x in b if x not in a]
        chainsNotInBoth = [x for x in chainsJo if x not in chainsDb]+[x for x in chainsDb if x not in chainsJo]
        if len(chainsNotInBoth):
            print 'ERROR: <%s> there are chains that are not in both' % proc_conf.name
            print chainsNotInBoth
            proc_conf.setError()
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
            print 'ERROR: <%s> found mismatch' % proc_conf.name
            proc_conf.setError()
            return False
        return True
            
    def getResultFromLog(self, fn):
        f = open(fn)
        output = [l.rstrip() for l in f.readlines() if 'TrigChainMoniOnline' in l and 'REGTEST events accepted' in l]
        f.close()
        p = re.compile('.*?events accepted by chain: (?P<chain>\w+?)\s+active: (?P<active>\d+?)\s+run :(?P<run>\d+?)\s+PS: (?P<ps>\d+?)\s+raw: (?P<raw>\d+?)\s+accepted after PS and PT: (?P<pspt>\d+?)\s+rerun: (?P<rerun>\d+?)')
        d = {}
        for l in output:
            m = p.match(l)
            if m:
                d[m.group('chain')] = m.groupdict()
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
        head = "| idx |    ChainName %s|| %10s | %10s | %10s || %10s | %10s | %10s |" % (space,'raw JO','ps JO','pspt JO','raw DB','ps DB','pspt DB')
        print head
        # define space line
        space = ''
        while len(space)<maxlen: space += '-'
        ul = "|-----|-----%s||------------|------------|------------||------------|------------|------------|" % space
        

        #w = Toplevel(bg='lightblue')
        #w.title('%s Comparison' % name)
        #t = Text(w, width= len(head),height=44)
        #t.pack(side=TOP,fill=BOTH,expand=1)
        #t['bg'] = 'white'

        #t.tag_config('error',background='yellow', foreground='red')
        #t.tag_config('marker',background='grey95')

        currentLine = 1
        
        #t.insert("1.0", ul)
        #t.insert("%s+1l" % INSERT, head)
        #t.insert("%s+1l" % INSERT, ul)
        print ul
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
            line = "| %3i |    %s%s | %10s | %10s | %10s | %10s | %10s | %10s |" % (i,c,space,jo['raw'],jo['ps'],jo['pspt'],db['raw'],db['ps'],db['pspt'])
            #t.insert("%s+1l" % INSERT, line,tag)
            print line
            if (i % 40)==39:
                #    t.insert("%s+1l" % INSERT, ul)
                #    t.insert("%s+1l" % INSERT, head)
                #    t.insert("%s+1l" % INSERT, ul)
                print ul
        #t.insert("%s+1l" % INSERT, ul)
        print ul





def parse_programm_options():
    import getopt
    short_opt = "h?F:"
    long_opt = ['help', 'options=']

    try:
        opts, args = getopt.getopt(sys.argv[1:], short_opt, long_opt)
    except getopt.GetoptError, x:
        print x
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
    return inputFile


def main():
    signal.signal(signal.SIGINT, sigint_handler)
    inputFile = parse_programm_options()
    app = QtGui.QApplication(sys.argv)
    timer = QtCore.QTimer()
    timer.start(500)  # interval after which timeout signal is called
    timer.timeout.connect(lambda: None)
    qb = KeyPrep(inputFile)
    qb.show()
    sys.exit(app.exec_())

def sigint_handler(*args):
    """Handler for the SIGINT signal."""
    #sys.stderr.write('\r')
    #if QMessageBox.question(None, '', "Are you sure you want to quit?",
    #                        QMessageBox.Yes | QMessageBox.No,
    #                        QMessageBox.No) == QMessageBox.Yes:
    QtGui.QApplication.quit()

if __name__ == "__main__":
    main()
