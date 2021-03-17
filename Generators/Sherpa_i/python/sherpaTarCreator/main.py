# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys,os,glob
from argparse import ArgumentParser

#dependency big picture:
#
#  getOpenLoops
#  |-ok: createLibs
#  |-ok: makelibs
#  |-ok                           integrate                    integrate                     ...
#                                 |-ok: makeTarball            |-ok: makeTarball             ...
#                                 |-ok: evgen        ...       |-ok: evgen         ...       ...


def main():
    parser = ArgumentParser(description='create gridpackage from job option')
    parser.add_argument("jobOptionDir",type=str, nargs='+',help="Path to job option directory. If multiple are given only the calculation of MinEvents is performed for the additional setups (meant e.g. for different filters).")
    parser.add_argument('-b', '--batchSystem', type=str, dest='batchSystem', default='htcondor_lxplus', help='Batch system to use (default = %(default)s, alternatives: htcondor_naf, slurm_taurus, sge_grace)')
    parser.add_argument('-c', '--ecm', type=float, dest='ecm', nargs="+", default=[13.0], help='center of mass energy')
    parser.add_argument('-o', '--only', type=str, dest='performOnly', nargs="+", default=["all"], help='steps which can be performed, default:all, there are: getOpenLoops, createLibs, makelibs, integrate, makeTarball, evgen')
    parser.add_argument('-n', '--maxCores', type=int, dest='maxCores', default="100000", help='maximal number of cores for the integration')
    parser.add_argument('-r', '--RAM', type=int, dest='RAM', default="-1", help='reserved RAM in MB for the integration')
    parser.add_argument('-l', '--createLibsRAM', type=int, dest='createLibsRAM', default="2", help='reserved RAM in MB for createLibs, values 1 and 2 are translated into normal RAM usage and extended RAM usage respectively')
    parser.add_argument('-m', '--ncoresMakelibs', type=int, dest='ncoresMakelibs', default="4", help='maximal number of cores for Makelibs')
    parser.add_argument('-s', '--ncoresScons', type=int, dest='ncoresScons', default="4", help='maximal number of cores for OpenLoops installation')
    parser.add_argument('-e', '--nEvts', type=int, dest='nEvts', default="100", help='number of events which get generated in the test run')
    parser.add_argument('-t', '--tarballVersion', type=str, dest='tarballVersion', default="1", help='version of tarball on grid')
    parser.add_argument('-p', '--local-sherpa', dest='sherpaInstallPath', default=None, help="Path to custom-built local Sherpa installation. If None, use cvmfs version.")
    parser.add_argument('-v', '--athenaVersion', type=str, dest='athenaVersion', default=None, help='Overwrite Athena version to be used.')
    parser.add_argument('--OLskipcvmfs', dest='OLskipcvmfs', default=False, action='store_true', help="Skip precompiled OpenLoops libraries from cvmfs.")
    parser.add_argument('--OLbranch', dest='OLbranch', type=str, default="OpenLoops-2.1.1", help="OpenLoops branch to use from https://gitlab.com/openloops/OpenLoops (e.g. OpenLoops-2.1.1 or public_beta)")
    parser.add_argument('--OLprocessrepos', dest='OLprocessrepos', type=str, default="ATLAS,public_beta,public", help="OpenLoops process repositories to use with ./openloops libinstall")
    parser.add_argument('-d', '--dryRun', action='store_true', default=False, dest='dryRun', help="Do not actually submit jobs to cluster but just print them.")
    options = parser.parse_args()

    import importlib
    options.batchSystemModule = importlib.import_module("Sherpa_i.sherpaTarCreator."+options.batchSystem)

    for i in range(len(options.jobOptionDir)):
        options.jobOptionDir[i] = os.path.abspath(options.jobOptionDir[i])
    os.chdir(options.jobOptionDir[0])

    if not options.athenaVersion:
        if "stable" in os.environ['AtlasReleaseType']:
            options.athenaVersion = os.environ['AtlasVersion']+","+os.environ['AtlasProject']
        elif "nightly" in os.environ['AtlasReleaseType']:
            options.athenaVersion = os.environ['AtlasBuildBranch']+","+os.environ['AtlasProject']+",r"+os.environ['AtlasBuildStamp']
        else:
            print ("ERROR: Did not find Athena environment. Did you run asetup?")
            sys.exit(1)

    # Load information from JO file
    from . import readjo
    readjo.readJO(options)
    
    from . import jobDefinitions

    for jodir in options.jobOptionDir:
        if not os.path.isdir(jodir):
            print ("ERROR: JO folder not found.")
            sys.exit(2)
        for ecm in options.ecm:
            folder = jodir+"/ecm"+('{0:g}'.format(ecm)).replace(".","p")+"TeV"
            #create folder
            if not os.path.isdir(folder):
                os.makedirs(folder)
            #link Process directory in new folder
            if not os.path.islink(os.path.join(folder,"Process")):
                os.symlink(os.path.join("..","Process"), os.path.join(folder,"Process"))

            for f in options.Sherpa_i.ExtraFiles:
                fi = os.path.basename(os.path.normpath(os.path.abspath(f)))
                if not os.path.exists(fi):
                    print ("ERROR: The extraFile '"+f+"' does not exist, but is required according to the joboption file")
                    sys.exit(3)
                if not os.path.islink(os.path.join(folder,fi)):
                    os.symlink(os.path.join(options.jobOptionDir[0],fi), os.path.join(folder,fi))

            if not os.path.islink(os.path.join(folder,"libSherpa_iPlugin.so")):
                if options.Sherpa_i.PluginCode is not "":
                    os.symlink(os.path.join(options.jobOptionDir[0],"libSherpa_iPlugin.so"), os.path.join(folder,"libSherpa_iPlugin.so"))
    
    prevJob = None
    getOpenLoopsJob = jobDefinitions.mkGetOpenLoopsJob(options)
    if getOpenLoopsJob:
        prevJob = getOpenLoopsJob
    
    createLibsJob = jobDefinitions.mkCreateLibsJob(options, prevJob)
    if createLibsJob:
        prevJob = createLibsJob
    
    makelibsJob = jobDefinitions.mkMakelibsJob(options, prevJob)
    if makelibsJob:
        prevJob = makelibsJob
    
    for ecm in options.ecm:
    
        ecmPrevJob = prevJob
        integrationJob = jobDefinitions.mkIntegrateJob(options, ecm, ecmPrevJob)
        if integrationJob:
            ecmPrevJob = integrationJob
    
        tarballmakerJob = jobDefinitions.mkTarballmakerJob(options, ecm, ecmPrevJob)
        if tarballmakerJob:
            ecmPrevJob = tarballmakerJob
    
        for jodir in options.jobOptionDir:
            jobDefinitions.mkEvntGenTestJob(options, ecm, jodir, ecmPrevJob)
    
    options.batchSystemModule.finalizeJobs(options.dryRun)

if __name__ == "__main__":
    main()
