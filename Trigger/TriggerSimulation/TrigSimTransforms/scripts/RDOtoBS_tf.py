# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#-------------------------------------------------------------------------------
def add_RDOtoBS_args(parser):
    import PyJobTransforms.trfArgClasses as trfArgClasses

    parser.defineArgGroup('RDOtoBS_trf', 'RDO to BS specific options')

    parser.add_argument('--inputRDOFile', group = 'RDOtoBS_trf',
                nargs = '+',
                type = trfArgClasses.argFactory(
                    trfArgClasses.argPOOLFile,
                    io = 'input',
                    runarg = True,
                    type = 'rdo'
                ),
                help = 'Input RDO file'
            )

    parser.add_argument('--outputBSFile', group = 'RDOtoBS_trf',
                type = trfArgClasses.argFactory(
                    trfArgClasses.argBSFile,
                    io = 'output',
                    runarg = True,
                    type = 'bs'
                    ),
                help = 'Output BS file'
            )

    parser.add_argument('--triggerConfig', group = 'RDOtoBS_trf',
                type = trfArgClasses.argFactory(
                        trfArgClasses.argString,
                        runarg = True
                    ),
                help = 'Trigger config to be used'
            )

#-------------------------------------------------------------------------------
def get_transform():
    from PyJobTransforms.transform import transform
    from PyJobTransforms.trfExe import athenaExecutor
    trf =  transform(executor = athenaExecutor(
                name = 'RDOtoBS',
                skeletonFile = 'TrigSimTransforms/skeleton.RDOtoBS.py',
                substep = 'r2b',
            ))

    from PyJobTransforms.trfArgs import addAthenaArguments
    addAthenaArguments(trf.parser)

    add_RDOtoBS_args(trf.parser)

    return trf

#-------------------------------------------------------------------------------
if __name__ == "__main__":
    import sys
    from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

    @stdTrfExceptionHandler
    @sigUsrStackTrace
    def main():
        trf = get_transform()
        trf.parseCmdLineArgs(sys.argv[1:])
        trf.execute()
        trf.generateReport()

        sys.exit(trf.exitCode)



    main()

#-------------------------------------------------------------------------------
