#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import PyJobTransforms.trfArgClasses as trfArgClasses

def addStdEvgenArgs(parser):
    parser.defineArgGroup("Evgen", "Event generator options")

    # TODO: Try to automatically determine the run number from the JO name, if not specified
    parser.add_argument('--runNumber', '--RunNumber', group='Evgen',
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                         help='Manual run number setting')

    parser.add_argument("--ecmEnergy", "--EcmEnergy", group="Evgen",
                        default=trfArgClasses.argFloat(8000, runarg=True),
                        help="centre-of-mass energy parameter in GeV",
                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True))

    parser.add_argument("--firstEvent", "--FirstEvent", group="Evgen",
                        default=trfArgClasses.argInt(1, runarg=True),
                        help="the number of the first event in the output data file",
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True))

    parser.add_argument("--randomSeed", "--RandomSeed", "--seed", group="Evgen", nargs="+",
                        default=trfArgClasses.argSubstepInt("all:12345"),
                        help="a seed for the MC generator random number engines",
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepInt))

    parser.add_argument("--printEvts", group="Evgen",
                        default=trfArgClasses.argInt(5, runarg=True),
                        help="number of full event dumps to print out for debugging",
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True))

    parser.add_argument("--jobConfig", "--JobConfig", "--jo", group="Evgen", nargs="+",
                        help="a comma-separated list of job configuration script files",
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepList, runarg=True))

    # TODO: Add "--eventFile"?
    parser.add_argument("--inputGeneratorFile", "--inputGenEventFile", "--inputGenFile", "--InputGeneratorFile", group="Evgen",
                        help="optional parton-level events file to be processed",
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True))

    # TODO: Add "--confFile"?
    parser.add_argument("--inputGenConfFile", "--InputGenConfFile", group="Evgen",
                        help="optional generator configuration file",
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True))

    # parser.add_argument("--inputHepMCFile", group="Evgen",
    #                     help="optional fully showered HepMC input event file",
    #                     type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True))

    parser.add_argument("--inputEVNTFile", group="Evgen",
                        help="input EVNT file (for use with e.g. Rivet)",
                        type=trfArgClasses.argFactory(trfArgClasses.argEVNTFile, io='input', runarg=True))

    parser.add_argument("--inputEVNT_PreFile", group="Evgen",
                        help="input evgen file for processing with afterburner, e.g. EvtGen",
                        type=trfArgClasses.argFactory(trfArgClasses.argEVNTFile, io='input', runarg=True))

    parser.add_argument("--evgenJobOpts", "--EvgenJobOpts", group="Evgen",
                        help="download and install the EvgenJobOpts tarball with the given name",
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True))

    # TODO: make optional: by default auto-determine from the JO name. If NONE, don't write?
    parser.add_argument("--outputEVNTFile", "--OutputEVNTFile", group="Evgen",
                        help="POOL file into which generated events will be written",
                        type=trfArgClasses.argFactory(trfArgClasses.argEVNTFile, io='output', runarg=True))

    parser.add_argument("--outputEVNT_PreFile", "--OutputEVNT_PreFile", group="Evgen",
                        help="POOL file into which pre-afterburner generated events will be written",
                        type=trfArgClasses.argFactory(trfArgClasses.argEVNTFile, io='output', runarg=True))

    parser.add_argument('--outputNTUP_TRUTHFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, io='output', treeNames=['truth']),
                        help='Output NTUP_TRUTH file', group='Evgen')

    parser.add_argument("--outputYODAFile", group="Evgen",
                        help="Name of YODA file for Rivet histo output",
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True))

    parser.add_argument("--rivetAnas", group="Evgen",
                        help="a comma-separated list of Rivet analyses to run on the resulting events",
                        type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True))

    parser.add_argument("--outputTXTFile", group="Evgen",
                        help="optional output TXT file for LHEF events, default is None",
                        type=trfArgClasses.argFactory(trfArgClasses.argLHEFile, io='output', runarg=True))
    parser.add_argument("--generatorRunMode", group="Evgen",
                        help="Run mode to pass on to the event generator",
                        default=trfArgClasses.argString("run", runarg=True),
                        choices=[trfArgClasses.argString("read", runarg=True),
                                 trfArgClasses.argString("build", runarg=True),
                                 trfArgClasses.argString("integrate", runarg=True),
                                 trfArgClasses.argString("mergegrids", runarg=True),
                                 trfArgClasses.argString("run", runarg=True)],
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True))

    parser.add_argument("--generatorJobNumber", group="Evgen",
                        help="Job number to pass on to the event generator",
                        default=trfArgClasses.argInt(0, runarg=True),
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True))

    parser.add_argument('--lheOnly', '--lheOnly', group='Evgen',
                        default=trfArgClasses.argInt(0, runarg=True),
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Set to 1 if Evgen file should be removed')

    parser.add_argument("--localPath", group="Evgen",
                        help="Name of local path to JOs",
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True))

    parser.add_argument('--saveList', '--saveList', group='Evgen',
                        help="a comma-separated list of files to be saved in output or a single digit if the whole output directory is to be saved",
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstepList, runarg=True))

    parser.add_argument('--cleanOut', '--cleanOut', group='Evgen',
                        default=trfArgClasses.argInt(0, runarg=True),
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='Set to any digit !=0 if the output files should be cleaned up --saveList may help to save selected files')
