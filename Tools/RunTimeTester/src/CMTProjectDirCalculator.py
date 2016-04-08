# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CMTtools     import decodeCMTCONFIG
from exc2string2  import exc2string2
from ShellCommand import ShellCommand
import os.path
class CMTProjectDirCalculator:
    def __init__(self,
                 topProject,
                 originalBranch,
                 release,
                 otherProject,
                 isNightly,
                 cmtConfig,
                 cmtHomeDir,
                 logger
                 ):
        
        self.topProject = topProject
        self.originalBranch   = originalBranch
        self.release          = release
        self.otherProject     = otherProject
        self.isNightly        = isNightly
        self.cmtConfig        = cmtConfig
        self.cmtHomeDir       = cmtHomeDir
        self.logger           = logger
         
        self.projectCMTDirs = {}

        self.findProjectPaths()
        
    def findProjectPaths(self):
        self.projectCMTDirs = {self.topProject:{'Release':'', 'RunTime':''}}

        for thing in ['Release','RunTime']:
            if self.isNightly:
                topProjectPath = os.path.join('/afs/cern.ch/atlas/software/builds/nightlies/',
                                              self.originalBranch,
                                              self.topProject,
                                              self.release,
                                              self.topProject+thing,
                                              'cmt')
            else:
                topProjectPath = os.path.join('/afs/cern.ch/atlas/software/builds/',
                                              self.topProject,
                                              self.release,
                                              self.topProject+thing,
                                              'cmt')
                
            self.projectCMTDirs[self.topProject][thing] = topProjectPath
                    

        cmtInfo = decodeCMTCONFIG(self.cmtConfig)
                
        cmd = ['cd %s' % self.cmtHomeDir,
               'source setup.sh -tag=%s,%s,%s,%s,%s,%s,%s' % (self.topProject,
                                                              self.otherProject,
                                                              cmtInfo['build'],
                                                              self.originalBranch,
                                                              cmtInfo['compiler'],
                                                              cmtInfo['nbits'],
                                                              self.release),
               'cd %s;source setup.sh;cmt show projects' % self.projectCMTDirs[self.topProject]['Release']
               ]

        try:
            sc = ShellCommand(cmd, self.logger, 100, None, True)
        except Exception, e:
            m = 'Unable to obtain project dirs. cmd:\n %s\nException:%s\nTraceback:%s' % (cmd, str(e),exc2string2())
            self.logger.critical(m)
            return

        self.logger.debug('printing directories start')
        reply = sc.getReply()
            
        for l in reply:
            
            if l.find('No such')>-1:
                self.logger.error('error getting project directories. cmd:\n %s\n%s\n%s' % (cmd,str(reply),l))
                break
            
            l.strip()
            tokens = l.split()
            if len(tokens) < 4: continue
            project = tokens[0]
            directory = tokens[3][:-1]

            self.logger.debug(project)
            if not (project.startswith('Atlas') or project.startswith('DetCommon')): continue

            self.projectCMTDirs[project] = {'Release':'', 'RunTime':''}
            for thing in self.projectCMTDirs[project].keys():
                dir = os.path.join(directory, project+thing, 'cmt')
                if os.path.exists(dir): self.projectCMTDirs[project][thing] =  dir

        self.logger.debug('printing directories stop')
        self.logger.debug('Project dirs:')
        for proj, dict in self.projectCMTDirs.items():
            self.logger.debug('  %-20s %s' % (proj, dict['Release']))
            self.logger.debug('  %-20s %s' % (proj, dict['RunTime']))

