# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys, os, glob


def include(filename):
    #needed for the inclusion of additional files in the joboptions
    if "Filter" in filename:
      return None
    if os.path.exists(filename):
        exec(compile(open(filename, "rb").read(), filename, 'exec'), globals())
    else:
        found = False
        for jopath in os.environ['JOBOPTSEARCHPATH'].split(":"):
            fullpath = jopath+"/"+filename
            if os.path.exists(fullpath):
                found = True
                exec(compile(open(fullpath, "rb").read(), fullpath, 'exec'), globals())
        if not found:
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
    mainJOfile = glob.glob("mc.*.py")
    if len(mainJOfile)!=1:
        print ("ERROR: found 0 or multiple mc.*.py files in main jobOptionDir")
        sys.exit(2)
    options.mainJOfile = mainJOfile[0]

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
    from . import dummy
    sys.modules["Sherpa_i.Sherpa_iConf"] = dummy
    sys.modules["Sherpa_i"] = dummy

    sys.modules["GeneratorFilters.GeneratorFiltersConf"] = dummy
    sys.modules["GeneratorFilters"] = dummy

    exec(compile(open(options.mainJOfile, "rb").read(), options.mainJOfile, 'exec'), globals())

    if genSeq.Sherpa_i.RunCard == "":
        print ("ERROR: no runCard found in the JO-File")
        sys.exit(11)

    # output information into options
    options.Sherpa_i = genSeq.Sherpa_i
