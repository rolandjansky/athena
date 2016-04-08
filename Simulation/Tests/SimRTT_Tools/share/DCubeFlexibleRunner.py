##
# @module DCubeFlexibleRunner
# @author Krzysztof Daniel Ciba
# @date 4 Sep 2007
# @brief Glue module to plugin DCube to Atlas RTT.
#
# * Feb 4, 2007 - switch to DCubeClient-00-00-13
#                 and DCubeServer-00-00-08
# * Apr 10, 2008 - switch to DCubeServer-00-00-10
# * Nov 7, 2008 - switch to new DCubeServer-00-00-11
# * Nov 11, 2008 - switch to new DCubeClient-00-00-19
# * and DCubeServer-00-00-12
# * Feb 09 2011 - PEP8 formatting PS
## some imports
# import xml.dom.minidom as minidom
import os
import re

from ExeRunnerBase import ExeRunnerBase
## @class DCubeFlexibleRunner
#  @brief prepares and runs shell script with DCube command


class DCubeFlexibleRunner(ExeRunnerBase):

    ## DCubeClient API classes
    __reAPIClasses = [re.compile("DCubeApp"),
                      re.compile("DCubeConfig"),
                      re.compile("DCubeTester"),
                      re.compile("DCubePlotter"),
                      re.compile("DCubePHPWriter")]

    ## base directory for references
    # baseRef = "/afs/cern.ch/atlas/project/RTT/prod/Results/reference/"

    ## DCubeServer directory
    # dcubeServer = "/afs/cern.ch/atlas/project/RTT/prod/Results/dcube/latest/"

    ## DCubeClient directory
    # dcubeClient = "/afs/cern.ch/atlas/project/RTT/DCubeClient/latest/python"

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param argDict RTT action runner dictionary
    def __init__(self, argDict):

        try:
            from Logger import Logger
            self.logger = Logger()
        except ImportError:
            self.logger = False

        error = None

        msg = "DCubeFlexibleRunner constructor is getting args from dictionary..."
        self.debug(msg)

        self.cfgFile = None
        if ("DCubeCfg" in argDict):
            self.cfgFile = argDict["DCubeCfg"]
        else:
            msg = "*** No DCubeCfg in DCubeFlexibleRunner action dictionary ***"
            self.info(msg)
            error = "No DCubeCfg in DCubeFlexibleRunner argDict"

        self.refFile = None
        self.localRefFile = None
        if ("DCubeRef" in argDict):
            self.refFile = argDict["DCubeRef"]
        elif ("DCubeLocalRef" in argDict):
            self.localRefFile = argDict["DCubeLocalRef"]
        else:
            msg = "*** No DCubeRef or DCubeLocalRef in DCubeFlexibleRunner action dictionay,"
            msg += "will use value from config XML file ***"
            self.info(msg)

        self.monFile = None
        if ("DCubeMon" in argDict):
            self.monFile = argDict["DCubeMon"]
        else:
            msg = "*** No DCubeMon in DCubeFlexibleRunner action dictionary ***"
            self.info(msg)
            error = "No DCubeMon in DCubeFlexibleRunner argDict"

        if ("DCubeJobId" in argDict):
            self.jobId = argDict["DCubeJobId"]
        else:
            self.jobId = None

        jobDtor = argDict.get('JobDescriptor', None)

        if (jobDtor != None):
            # all args now passed to __init__
            # self.DCubeForRTTCfg = jobDtor.paths.dCubeCfgFile
            # PS note: outputPath is used by DCube
            #    ExeRunnerBase to writes captured stdout to outputFile.
            self.outputPath = jobDtor.runPath

            # self.installArea = jobDtor.paths.installArea
            self.cvsPath = os.path.join('offline',
                                        jobDtor.paths.containerPackage,
                                        jobDtor.package)

            self.cmtPath = jobDtor.paths.cmtPath
            self.nightly = jobDtor.paths.branch  # branch
            self.install = jobDtor.paths.runType  # install
            self.cmtconfig = jobDtor.paths.cmtConfig  # CMTCONFIG
            self.project = jobDtor.paths.topProject  # Atlas project name
            self.release = jobDtor.paths.release  # release name

            # base for directories used by users to place ref files
            self.refBaseDir = jobDtor.paths.dcubeRefBaseDir
            self.dcubeClient = jobDtor.paths.dcubeClient  # dcubeClient dir
            self.dcubeServer = jobDtor.paths.dcubeServer  # dcubeServer dir
        else:
            # dummy values for testing
            #self.DCubeForRTTCfg = "./DCubeForRTTCfg.xml"
            self.outputPath = "/DUMMY/RUN/PATH/FROM/JOB/DESCRIPTOR"
            # self.installArea = "/DUMMY/INSTALL/AREA/FROM/JOB/DESCRIPTOR"
            self.cvsPath = "/DUMMY/CONTAINER/PACKAGE/FROM/JOB/DESCRIPTOR"
            self.cmtPath = "/DUMMY/CMT/PATH/FROM/JOB/DESCRIPTOR"
            self.nightly = "dev"
            self.install = "build"
            self.cmtconfig = "i686-slc4-gcc34-opt"
            self.project = "AtlasProduction"
            self.release = "rel_1"

        # msg = "[01] will use '%s' as DCubeClient configuration"
        # msg += "file for RTT"
        # msg = msg % self.DCubeForRTTCfg

        # self.debug(msg)
        self.debug("[02] will use '%s' as output path" % self.outputPath)

        # msg = "[03] will use '%s' as install area path" % self.installArea
        # self.debug(msg)

        self.debug("[04] will use '%s' as package CVS path" % self.cvsPath)
        self.debug("[05] will use '%s' as cmt path" % self.cmtPath)

        msg = "[06] will use DCubeClient monitored file '%s'"
        msg = msg % self.monFile
        self.debug(msg)

        if self.localRefFile:
            msg = "[07] will use DCubeClient local reference file '%s'"
            msg = msg % self.localRefFile
            self.debug(msg)
        elif self.refFile:
            msg = "[07] will use DCubeClient reference file '%s'"
            msg = msg % self.refFile
            self.debug(msg)
        else:
            msg = "[07] No DCubeClient reference file set"
            self.debug(msg)

        msg = "[08] will use DCubeClient configuration file '%s'"
        msg = msg % self.cfgFile
        self.debug(msg)

        msg = "[09] will use '%s' as DCubeClient branch name"
        msg = msg % self.nightly
        self.debug(msg)

        msg = "[10] will use '%s' as DCubeClient install name"
        msg = msg % self.install
        self.debug(msg)

        msg = "[11] will use '%s' as DCubeClient cmtconfig name"
        msg = msg % self.cmtconfig
        self.debug(msg)

        msg = "[12] will use '%s' as DCubeClient project name"
        msg = msg % self.project
        self.debug(msg)

        if (self.jobId):
            self.debug(
                  "[13] will use '%s' as DCubeClient jobId" % self.jobId)
        else:
            self.debug("[13] DCubeClient jobId not set!")

        msg = "[14] will use '%s' as base reference directory"
        msg = msg % self.refBaseDir
        self.debug(msg)

        msg = "[15] will use DCubeServer installed in %s"
        msg = msg % self.dcubeServer
        self.debug(msg)

        commands = []
        if (error == None):
            commands = self.__parseDCubeRTTCfg()
        command = ';'.join(commands)

        hline = '-' * 20
        lines = ['command to run is:', hline]
        lines.extend(commands)
        lines.append(hline)
        msg = '\n'.join(lines)
        self.debug(msg)

        self.debug("DCubeFlexibleRunner __init__ end")

        if error:
            # will cause a FailureTest tobe created.
            raise RuntimeError(error)

        testIdentifier = argDict['testIdentifierObj']
        outputFile = argDict['testOutputFile']
        ExeRunnerBase.__init__(self,
                               cmd=command,
                               testIdentifier=testIdentifier,
                               outputFile=outputFile)

    # run method
    # @param self "Me, myself and Irene"
    def run(self):

        status = ExeRunnerBase.run(self)

        if status:
            with open(self.outputFile) as ifile:
                output = ifile.read()

            self.debug(
                '%s.run bad status %d on issuing cmd %s\no/p:\n%s' % (
                    self.__class__.__name__,
                    status,
                    self.command,
                    output
                    )
                )

            return status

        reLine = re.compile("OVERALL STATISTICS STATUS")
        status = 1

        with open(self.outputFile) as ifile:
            for line in ifile:
                for apiClass in self.__reAPIClasses:
                    if (apiClass.search(line)):
                        self.debug(line)
                        if (reLine.search(line)):
                            if ("OK" in line):
                                status = 0

        self.debug("DCubeFlexibleRunner run end, returning status %i" % status)

        return status

    # XML parsing of DCubeForRTTCfg.xml
    # @param self "Me, myself and Irene"
    def __parseDCubeRTTCfg(self):

        # config information for DCube is now entirely
        # passed to __init__. Previously, some was passed in
        # and some was in a config file.

        # msg = "parsing DCubeFlexibleRunner configuration file %s"
        # msg = msg % self.DCubeForRTTCfg
        # self.debug(msg)
        # try:
        #    xmldoc = minidom.parse(self.DCubeForRTTCfg)
        #
        #    # self.refBaseDir = self.getText(
        #    # xmldoc.getElementsByTagName("ref_dir")[0].childNodes)

        #    msg = "[14] will use '%s' as base reference directory"
        #    msg = msg % self.refBaseDir
        #    self.debug(msg)

        #    self.dcubeServer = self.getText(
        #    xmldoc.getElementsByTagName("server")[0].childNodes)

        #   msg = "[15] will use DCubeServer installed in %s"
        #    msg = msg % self.dcubeServer
        #    self.debug(msg)

        # except:
        #    self.error = "error when parsing DCubeFlexibleRunner configuration "
        #    self.error += "file, no run will be preformed"
        #    return [""]

        self.debug("constructing command to run DCube...")
        out = []
        # words = self.cmtconfig.split("-")
        # if (words[0] == "i686"):
        #    bin = "32"
        # else:
        #    bin = "64"

        # gcc = words[2]
        # opt = words[3]
        # slc = words[1]

        # pars = [self.release,
        #       bin,
        #       slc,
        #       gcc,
        #       opt,
        #       self.project,
        #       "forceConfig",
        #       "setup"]
        # tag = ",".join(pars)

        # PS 3/2011
        # 1. now should use atlas setup
        # 2. already done in run script
        # 3. self.dcubeClient = RTTLibDir - already on path, python path
        # out.append("export CMTCONFIG=" + self.cmtconfig + "\n")
        # out.append("source ~/cmthome/setup.sh -tag=%s\n" % (tag))
        # out.append("export PATH=" + self.dcubeClient + ":${PATH}")

        # txt = "export PYTHONPATH=" + self.dcubeClient + ":${PYTHONPATH}"
        # out.append(txt)

        # changing the outputPath
        monPath = self.outputPath

        if self.localRefFile:
          self.outputPath += os.sep + "DCube-" + self.localRefFile
        else:
          self.outputPath += os.sep + "DCube-" + self.refFile

        # creating DCube subdirectory
        out.append("mkdir -p " + self.outputPath)

        dcubePath = self.dcubeClient

        dcubeConvert = os.path.join(dcubePath, "./dcubeConvert.py")

        run = os.path.join(dcubePath, "./dcube.py")
        run += " -p "  # generate plot
        run += " --branch " + self.nightly  # set branch
        run += " --install " + self.install  # set install
        run += " --cmtconfig " + self.cmtconfig  # set cmtconfig
        run += " --project " + self.project  # set project
        # set jobId
        if (self.jobId):
            run += " --jobId " + self.jobId

        run += " -l " + self.outputPath + os.sep + self.monFile
        run += ".dcube.log"  # save log to monitored.dcube.log

        # path to the DCubeServer installation
        run += " -s " + self.dcubeServer
        run += " -x " + self.outputPath + os.sep + self.monFile
        run += ".dcube.xml "  # save output to monitored.dcube.xml

        # dangerous hacking at the moment...
        if (os.path.isabs(self.cfgFile) or "DUMMY-TEST" in self.cfgFile):
            run += " -c " + self.cfgFile
            config = self.cfgFile
        else:
            # checking cfg file in reference dir
            refCfg = "/afs/cern.ch/atlas/project/RTT/Results/reference/"
            refCfg += self.cvsPath + os.sep + self.cfgFile

            msg = "will check the timestamp of DCube configuration file "
            msg += "(reference volume): " + refCfg
            self.debug(msg)

            refCfgStat = None

            if (os.path.exists(refCfg)):
                refCfgStat = os.stat(refCfg)
                msg = "File %s modified %d" % (refCfg, refCfgStat[8])
                self.debug(msg)
            else:
                msg = "DCube configuration file %s not found on "
                msg += "'reference' volume"
                msg = msg % self.cfgFile
                self.debug(msg)

                # checking cfg file in cvs co dir

            cvsCfg = self.cmtPath + os.sep
            cvsCfg += self.cvsPath.lstrip("offline") + "/test/"
            cvsCfg += self.cfgFile

            msg = "will check the timestamp of DCube configuration file "
            msg += "(CVS checkout volume):" + cvsCfg
            self.debug(msg)

            cvsCfgStat = None
            if (os.path.exists(cvsCfg)):
                cvsCfgStat = os.stat(cvsCfg)
                msg = "File %s modified %d" % (cvsCfg, cvsCfgStat[8])
                self.debug(msg)
            else:
                msg = "DCube configuration file %s not found in CVS "
                msg += "checkout directory"
                msg = msg % self.cfgFile
                self.debug(msg)

            # choose configuration file based on latest timestamp
            config = None

            # both are present, check time stamps
            if (None not in (refCfgStat, cvsCfgStat)):
                # install area newer
                if (refCfgStat[8] <= cvsCfgStat[8]):
                    config = cvsCfg
                else:
                    config = refCfg

            # reference is present
            elif (None != refCfgStat):
                config = refCfg
            # install area is present
            elif (None != cvsCfgStat):
                config = cvsCfg
            # both are absent
            else:
                pass

            if (config != None):
                run += " -c " + config  # add configuration file
            else:
                msg = "DCube configuration file %s not found on afs "
                msg += "reference  volume or InstallArea directories"
                msg = msg % self.cfgFile
                self.debug(msg)
                return [""]

        if (self.localRefFile != None):
            # overwrite reference
            run += " -r " + os.path.join(monPath, self.localRefFile)
            # add monitored file at the end
            run += " " + os.path.join(monPath, self.monFile)
        elif (self.refFile != None):
            # overwrite reference
            run += " -r " + os.path.join(
                  os.path.join(self.refBaseDir, self.cvsPath),
                  self.refFile)

            # add monitored file at the end
            run += " " + os.path.join(monPath, self.monFile)

        if ("DUMMY-TEST" in self.cfgFile):
            dcubeConvert += " %s " % os.path.abspath(self.cfgFile)
        else:
            dcubeConvert += " %s " % os.path.abspath(config)

        if (self.localRefFile != None):
            dcubeConvert += " %s" % os.path.join(monPath, self.localRefFile)
        else:
            dcubeConvert += " %s" % os.path.join(
                os.path.join(self.refBaseDir, self.cvsPath),
                self.refFile)
        out.append(dcubeConvert)
        out.append(run)

        return out

    # logger level INFO
    # @param self "Me, myself and Irene"
    # @param msg message
    def info(self, msg):
        if (bool(self.logger)):
            self.logger.info(str(msg))
        else:
            print "DCubeFlexibleRunner INFO  " + str(msg)

    # logger level DEBUG
    # @param self "Me, myself and Irene"
    # @param msg message
    def debug(self, msg):
    #     if (bool(self.logger)):
      #       self.logger.debug(str(msg))
        # else:
        print "DCubeFlexibleRunner DEBUG " + str(msg)

    # get CDATA section
    # @param self "me, myself and Irene"
    # @param nodelist list of XML DOM Nodes
    def getText(self, nodelist):
        out = ""
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                out = out + node.data
        return out

    # split path
    # @param self "Me, myself and Irene"
    # @param path path to split
    # @param rest splitted tail
    def pathSplit(self, path, rest=[]):
        (head, tail) = os.path.split(path)

        if len(head) < 1:
            return [tail] + rest

        if len(tail) < 1:
            return [head] + rest

        return self.pathSplit(head, [tail] + rest)

    # find common names in path
    # @param self "Me, myself and Irene"
    # @param l1 first list with splitted path
    # @param l2 second  lust with splitted path
    # @param common list of common dirnames
    def commonPath(self,  l1, l2, common=[]):
        if len(l1) < 1:
            return (common, l1, l2)

        if len(l2) < 1:
            return (common, l1, l2)

        if l1[0] != l2[0]:
            return (common, l1, l2)

        return self.commonPath(l1[1:], l2[1:], common + [l1[0]])

    # finds relative path between p1 and p2
    # @param self "Me, myself and Irene"
    # @param p1 first path
    # @param p2 second path
    def relPath(self, p1, p2):
        (common, l1, l2) = self.commonPath(self.pathSplit(p1),
                                           self.pathSplit(p2))
        p = []
        if len(l1) > 0:
            p = ['../' * len(l1)]
        p = p + l2
        return os.path.join(*p)


# dummy test running
if __name__ == "__main__":
    print "running DCubeFlexibleRunner..."

    class Paths:
        def __init__(self):
            self.dcubeRefBaseDir = '/afs/cern.ch/atlas/project/RTT/DCubeReference'
            self.dcubeClient = '/afs/cern.ch/atlas/project/RTT/Externals/DCubeClient/latest/python'
            self.dcubeServer = '/afs/cern.ch/atlas/project/RTT/prod/Results/dcube/latest'
            self.cmtPath = '/afs/cern.ch/atlas/software/builds/nightlies/devval/AtlasAnalysis/rel_4'
            self.branch = 'devval'
            self.runType = 'build'
            self.cmtConfig = 'i686-slc5-gcc43-opt'
            self.topProject = 'AtlasProduction'  # Atlas project name
            self.release = 'rel_4'
            self.containerPackage = 'TileCalorimeter/TileExample'

    class Descriptor:
        def __init__(self):
            self.paths = Paths()
            self.package = 'TileRecEx'
            self.runPath = '/afs/cern.ch/atlas/project/RTT/data/brinick/dcubetest'

    testCfg = {"DCubeCfg": "/afs/cern.ch/atlas/software/builds/nightlies/devval/AtlasAnalysis/rel_4/TileCalorimeter/TileExample/TileRecEx/test/TilePhysAllTestParamsOnOfaRttDCubeConfiguration.xml",
               "DCubeMon": "TilePhysAllTestParamsOnOfaRttHistograms.root",
               "DCubeRef": "TilePhysAllTestParamsOnOfaRttHistograms_20110516.root",
               "JobDescriptor": Descriptor()}
    dc = DCubeFlexibleRunner(testCfg)
    print 'running run'
    dc.run()
