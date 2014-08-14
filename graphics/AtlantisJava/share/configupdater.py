"""
author Zdenek Maxa
description:
    Automatic configuration updater.
    Script runs over all customised configuration files as found in the
    directory DIRECTORY_WITH_CONFIGURATIONS_TO_UPDATE  (pattern config-*.xml)
    and calls internal Atlantis XML configuration files updater from the class
    AConfigUpdater. Then runs Atlantis with each of the updated configuration
    files.
"""


# following two specified as command line arguments
ATLANTIS_DISTRIBUTION_PATH = ""
DIRECTORY_WITH_CONFIGURATIONS_TO_UPDATE = "" # will use mask CONFIG_FILES_MASK

CONFIG_FILES_MASK = "config-*.xml"
ATLANTIS_JAR_FILE = "atlantis.jar"
ATLANTIS_LIB_DIR = "lib"


print "Jython configupdater.py (Atlantis configurations) script running ..."


try:
    print "Importing Jython modules ..."
    import os
    import sys
    import fnmatch
    import traceback
    import getopt
    import popen2
    import threading
    import time
    print "Importing Java libraries / classes (from Java installation) ..."
    # must be imported in this form
    from java.lang import Throwable
    from java.lang import Exception
    from java.lang import NoClassDefFoundError
    from java.lang import Runtime
    from java.lang import System
    from java.lang import Runnable
    from java.lang import Thread
except (NameError, ImportError), ex:
    print ("Error during import. Missing either Jython libraries or Java is "
           "improperly installed or set up (JAVA_HOME var.), reason: %s"
            % ex)
    sys.exit(1)
else:
    print "Import successful."



BASE_CONFIGURATION = os.path.join("configuration", "config.xml")


class StreamReader(Runnable):
    """Used for reading standard output and standard error output of the
       process.
    """
    def __init__(self, stream, logger):
        self.stream = stream
        self.logger = logger
        logger.debug("StreamReader init ...")

    def run(self):
        self.logger.debug("StreamReader thread started ...")
        line = ""
        while 1:
            data = self.stream.read()
            
            if data == -1:
                break

            # data is a character by character from the stream
            # data contains ASCII value
            char = chr(data)
            if char != '\n':
                line = "".join([line, char]) # restore a line
            else:
                self.logger.info(line)
                line = ""




# get mandatory command line arguments --------------------------------------

try:
    print "Processing command line arguments ..."
    options, arguments = getopt.getopt(sys.argv[1:], "ha:u:",
            ["help", "atlantis=", "update="])
except getopt.GetoptError, ex:
    print "Error while processing command line arguments, reason: ", ex
    print "-a, --atlantis <path> - path to the atlantis distribution"
    print "-u, --update <path> - path to customised config files"
    sys.exit(1)
else:
    for opt, arg in options:
        if opt in ("-h", "--help"):
            print "-a, --atlantis <path> - path to the atlantis distribution"
            print "-u, --update <path> - path to customised config files"
            sys.exit(1)
        elif opt in ("-a", "--atlantis"):
            ATLANTIS_DISTRIBUTION_PATH = arg
        elif opt in ("-u", "--update"):
            DIRECTORY_WITH_CONFIGURATIONS_TO_UPDATE = arg
    if(not ATLANTIS_DISTRIBUTION_PATH or
       not DIRECTORY_WITH_CONFIGURATIONS_TO_UPDATE):
           print "Mandatory arguments not set, exit. See --help"
           sys.exit(1)
    else:
        print ("Command line arguments correct.\n"
               "\tAtlantis distribution path: \"%s\"\n"
               "\tdirectory with config files to update: \"%s\"" %
               (ATLANTIS_DISTRIBUTION_PATH,
                DIRECTORY_WITH_CONFIGURATIONS_TO_UPDATE))


# load classes from the Atlantis distributon --------------------------------

# add libraries (jar files) into sys.path, so it's possible to import from
# need to add all atlantis libraries into class path, otherwise calling some
# method which interacts deep down with external library (only import is
# enough) will result into NoClassDefFoundError
# adding atlantis.jar into path
print "Loading classes from Atlantis distribution ..."
libs = []
atlantisPath = os.path.join(ATLANTIS_DISTRIBUTION_PATH, ATLANTIS_JAR_FILE)
if not os.path.exists(atlantisPath):
    print "%s file does not exist, exit." % atlantisPath
    sys.exit(1)
libs.append(atlantisPath)
libDir = os.path.join(ATLANTIS_DISTRIBUTION_PATH, ATLANTIS_LIB_DIR)
allLibs = os.listdir(libDir)
for i in allLibs:
    libFile = os.path.join(libDir, i)
    if os.path.isfile(libFile) and not os.path.islink(libFile) \
            and fnmatch.fnmatch(i, "*.jar"):
        libs.append(libFile)
print "Adding %s and all libraries from %s into path" % (atlantisPath, libDir)
# add all libraries from libs
[sys.path.append(i) for i in libs]

try:
    print "Importing Atlantis classes ..."
    from atlantis.config import AConfigUpdater
    from atlantis.utils import AAtlantisException
    from atlantis.utils import ALogger
except (NameError, ImportError), ex:
    print ("Can't import Atlantis classes, likely wrong path %s, reason: %s"
            % (atlantisPath, ex))
    sys.exit(1)
except NoClassDefFoundError, ex:
    print "NoClassDefFoundError while importing, reason: %s" % ex
    sys.exit(1)
else:
    print "Import successful."



# initialise logger from Atlantis -------------------------------------------

try:
    ALogger.initialize("DEBUG",None)
    logger = ALogger.getLogger("configupdater.py")
    logger.info("Logger for configupdater.py initialised.")
except Exception, ex:
    print "Exception while creating logger, exit.\nReason: %s " % ex
    sys.exit(1)



# get list of customised configurations to update / synchronise with the 
# base configuration from the atlantis distribution -------------------------
try:
    logger.info("Getting list of customised configurations from %s" %
            DIRECTORY_WITH_CONFIGURATIONS_TO_UPDATE)
    everything = os.listdir(DIRECTORY_WITH_CONFIGURATIONS_TO_UPDATE)
except OSError, ex:
    logger.fatal("Can't access the directory, reason: %s" % ex)
    sys.exit(1)
configs = []
for i in everything:
    file = os.path.join(DIRECTORY_WITH_CONFIGURATIONS_TO_UPDATE, i)
    if os.path.isfile(file) and not os.path.islink(file) \
            and fnmatch.fnmatch(i, CONFIG_FILES_MASK):
        configs.append(file)
if len(configs) < 1:
    logger.error("Configuration files list is empty, nothing to update.");
    sys.exit(0)
  


# process - update customised configuration files ---------------------------
logger.info("Updating %s customised configuration(s) ..." % len(configs))
baseConfig = os.path.join(ATLANTIS_DISTRIBUTION_PATH, BASE_CONFIGURATION)
logger.info("Base (master) configuration file: %s" % baseConfig)
if not os.path.exists(baseConfig):
    logger.fatal("Base configuration file %s does not exist." % baseConfig)
    sys.exit(1)
for userConfig in configs:
    try:
        logger.debug("Processing file %s ..." % userConfig)
        AConfigUpdater.updateConfigurationFile(baseConfig, userConfig)
        logger.debug("File sucessfully updated.");
    except (Exception, AAtlantisException), ex:
        logger.fatal("Error while updating file %s, exit now. Reason: %s" %
                (userConfig, ex))
        sys.exit(1)
logger.info("All %s files seem to have been successfully updated." %
        len(configs))



# now run atlantis over all newly updated configuration files to make sure
# atlantis at least starts with them ----------------------------------------
logger.info("Starting Atlantis with all newly update config files ...")
for userConfig in configs:
    operSystem = System.getProperty("os.name")
    if operSystem.lower().startswith("windows"):
        logger.info("Running on Windows platform ...")
        # windows accepts the command in this format
        command = ("""java -jar "%s" --config "%s" --debug DEBUG""" %
                  (atlantisPath, userConfig))
    else:
        # linux accepts the command in this format
        logger.info("Running on non-Windows platform ...")
        command = ("java -jar %s --config %s --debug DEBUG" %
                  (atlantisPath, userConfig))
    
    logger.info("Running command: \"%s\"" % command)
    # jython's os module doesn't define fork(), kill(), etc, then must
    # use Java here
    try:
        rt = Runtime.getRuntime()
        p = rt.exec(command)
        logger.info("Waiting for the Atlantis process ...")
        # (2008-12-10) on Windows: the above message is written out fine, 
        # Atlantis shows its splash window but hangs during start up.
        # Suggestion is that input/output is not handled properly and should
        # be read ...

        # standard output ; standard error output streams handling
        Thread(StreamReader(p.getInputStream(), logger)).start() # stdout
        Thread(StreamReader(p.getErrorStream(), logger)).start() # stderr
        # p.getOutputStream() - returns Output to the stream - is piped into
        # the standard input stream (reference to the standard input of the
        # process)
       
        p.waitFor()
    
    except Throwable, t:
        print "Error occured, reason: %s" % t
        t.printStackTrace()

    # different approach to creating the external processes (and likely the
    # best) would be to run like this:
    # args = ["--config", "userConfig"]
    # Atlantis.main(args)
    # and catching exceptions, however, static class references like in
    # Atlantis.getHomeDirectory():
    # Atlantis.class.getResource("Atlantis.class").getFile() fails when called
    # from Jython

logger.info("Backup files were created in \"%s\" directory erase them " 
            "manually if all files were updated all right." %
            DIRECTORY_WITH_CONFIGURATIONS_TO_UPDATE)


