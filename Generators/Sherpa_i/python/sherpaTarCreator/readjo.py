# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys, os


def include(filename):
    global athenaVersion
    #needed for the inclusion of additional files in the joboptions
    if "Filter" in filename:
      return None
    fullpath = "/cvmfs/atlas.cern.ch/repo/sw/software/21.6/AthGeneration/%s/InstallArea/x86_64-slc6-gcc62-opt/jobOptions/%s" % (athenaVersion.split(",")[0], filename)
    if os.path.exists(filename):
        exec(compile(open(filename, "rb").read(), filename, 'exec'), globals())
    elif os.path.exists(fullpath):
        exec(compile(open(fullpath, "rb").read(), fullpath, 'exec'), globals())
    else:
        print ("Did not find "+fullpath)
        print ("ERROR: file not found. Possibly no cvmfs access.")
        sys.exit(10)

#general functions
class dotdict(dict):
    """dot.notation access to dictionary attributes"""
    __getattr__ = dict.get
    __setattr__ = dict.__setitem__
    __delattr__ = dict.__delitem__
    def __iadd__(self, val):
        return self

def readJO(options):
    global athenaVersion
    athenaVersion = options.athenaVersion
    filename = options.optionfile[0]

    #load job option file and additional option file with execfile
    #important output variables
    global genSeq
    global evgenConfig
    genSeq = dotdict({"Sherpa_i":dotdict({
                    "Parameters":[],
                    "OpenLoopsLibs":[],
                    "NCores":24,
                    "RunCard":"",
                    "ExtraFiles":[],
                    "MemoryMB":1,
                    "PluginCode":"",
                    })})
    evgenConfig = dotdict({"generators":[]})

    #some input variables which effect genSeq.Sherpa_i.Parameters
    global runArgs
    global pdfgridpath
    runArgs = dotdict({"jobConfig":["Run.dat"]})
    pdfgridpath = "todo"

    #some dummy variables
    global testSeq
    global filtSeq
    global HeavyFlavorBHadronFilter
    HeavyFlavorBHadronFilter = True
    global HeavyFlavorCHadronPt4Eta3_Filter
    HeavyFlavorCHadronPt4Eta3_Filter = True
    testSeq = dotdict({
            "TestHepMC":dotdict({
                    "NoDecayVertexStatuses":[]
                    }),
            })
    testSeq = 0 # temp workaround as long as base fragment still removes TestHepMC
    filtSeq = dotdict({
            "DecaysFinalStateFilter":dotdict({
                    "NChargedLeptons":True,
                    "NQuarks":2,
                    "PDGAllowedParents":[23]
                    }),
            "Expression":"",
            })

    #import dummy.py which is later included as replacement for Sherpa_i.Sherpa_iConf
    import dummy
    sys.modules["Sherpa_i.Sherpa_iConf"] = dummy
    sys.modules["Sherpa_i"] = dummy

    sys.modules["GeneratorFilters.GeneratorFiltersConf"] = dummy
    sys.modules["GeneratorFilters"] = dummy

    exec(compile(open(filename, "rb").read(), filename, 'exec'), globals())

    if genSeq.Sherpa_i.RunCard == "":
        print ("ERROR: no runCard found in the JO-File")
        sys.exit(11)

    # output information into options
    options.Sherpa_i = genSeq.Sherpa_i
