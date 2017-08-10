
import PyUtils.Logging as L
msg = L.logging.getLogger('TrigCollQuery')
msg.setLevel(L.logging.INFO)


class TrigTAGFilePeeker:
    def __init__(self, run_attr_name='RunNumber'):
        self.run_attr_name = run_attr_name
        self.all_runs = set()
        self.file_cache = {}

    def get_runs_from_tagfile(self, fname):
        # check own cache for this file
        if fname in self.file_cache.keys():
            return self.file_cache[fname]
        # check file type with AthFile - this should avoid reopening files more times than necessary
        msg.debug("Checking file %s" % fname)
        import PyUtils.AthFile as athFile
        fileinfos = athFile.fopen(fname).fileinfos
        if not 'TAG' in fileinfos['stream_names']:
            return []
        # this is a TAG file, open it and read all run numbers
        # fileinfos have only the run number from the first TAG
        import PyUtils.Helpers as H
        import re
        with H.ShutUp(filters=[re.compile('TClass::TClass:0: RuntimeWarning: no dictionary for.*'),
                               re.compile('.*duplicate entry.*')]):
            msg.debug("Opening TAG file %s" % fname)
            import PyUtils.RootUtils as ru
            f = ru.import_root().TFile.Open(fname, "read")
            if f is None or not f:
                msg.warning("Failed to open TAG file %s" % fname)
                return []
            coll_tree = f.Get('POOLCollectionTree')
            run_numbers = set()
            if coll_tree is not None:
                for row in xrange(0, coll_tree.GetEntries()):
                    coll_tree.GetEntry(row)
                    run_numbers.add( getattr(coll_tree, self.run_attr_name) )
                del coll_tree
            f.Close()
            del f
            self.file_cache[fname] = run_numbers
            msg.info("TAG file: %s, found runs: %s" % (fname, str(run_numbers)))
            return run_numbers

    def get_runs_from_all_taginput(self):
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        # ifiles = athenaCommonFlags.PoolTAGInput()   # not set correctly!
        ifiles = athenaCommonFlags.FilesInput()
        if ifiles is None or ifiles == []:
            ifiles = ServiceMgr.EventSelector.InputCollections

        self.all_runs = set()
        for file in ifiles:
            runs = self.get_runs_from_tagfile(file)
            self.all_runs |= set(runs)
        return self.all_runs

    def get_all_runs_as_str(self):
        run_string = ','.join( [str(run) for run in self.get_runs_from_all_taginput()] )
        return run_string
    

from TrigCollQuery.TrigCollQueryConf import TrigCollQueryTool
Tool = TrigCollQueryTool("TrigCollQueryTool")

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.EventSelector.HelperTools += [ Tool ]

if ServiceMgr.EventSelector.Query.find('Trigger'):
    msg.info('Found Trigger keyword in the query string, scanning TAG input files for run numbers')
    peeker = TrigTAGFilePeeker()
    run_string = peeker.get_all_runs_as_str()
    Tool.QueryRuns = run_string
    msg.info("Total runs from TAGs: " + run_string)
    del peeker

del msg


# Example usage of Trigger selection in the EventSelector:
#
#ServiceMgr.EventSelector.Query = "NLooseElectron>1 && TriggerPassed(L2_e105)"


# TrigCollQuery properties:
# see:  https://twiki.cern.ch/twiki/bin/view/Atlas/TagTriggerQueries
#
#Tool.QueryRuns = "119267-119269,119274-119280,119302-119325"

# this is the default in .cpp
#Tool.TriggerMapWebService = "https://atlas-tagservices.cern.ch/tagservices/triggerDecoder/getDecodedTriggerInfo.php"

# Needed for MC, don't use for Data
#Tool.Project = ""
#Tool.AMITag = ""

# default value
#Tool.RunNumberAttribute = "RunNumber"

# outdated - these files are old, probably useless
#Tool.TriggerMapDir = "/afs/cern.ch/user/a/attrgcnf/TagChainMaps"

#Tool.UseCTPWord = False
