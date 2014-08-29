"""
Job options template for ATLAS detector Geant4 simulation tests.

This is not a normal job option script: it mainly takes job specifications
via parameters passed with the '-c' athena command line option flag, for
convenience and reproducibility in RTT test definitions or performance monitoring.

Examples:
 * athena test_G4Atlas.py -c "RUNCONFIGNUM=110101"
 * athena test_G4Atlas.py -c "RUNCONFIG=dict(mode=\"readevgen\", nevents=300, evgenfiles=\"my.evgen.pool.root\", skipevents=0)"
"""

## Sort out collisions between steering variables
import sys
if "RTTRunNumber" in dir():
    if "RUNCONFIGNUM" in dir():
        sys.stderr.write("Both RUNCONFIGNUM and RTTRunNumber have been defined... that's not allowed\n")
        sys.exit(1)
    RUNCONFIGNUM = RTTRunNumber
if "RUNCONFIGNUM" in dir() and "RUNCONFIG" in dir():
    sys.stderr.write("Both RUNCONFIGNUM and RUNCONFIG have been defined... that's not allowed\n")
    sys.exit(1)


class RunConfig(object):
    """Convenient object for storing run details"""
    def __init__(self, mode, nevents, physicslist=None,
                 pid=None, energy=None, etarange=None,
                 evgenfiles=None, skipevents=0):
        if mode.lower() not in ("partgen", "readevgen"):
            raise Exception("Mode parameter '%s' is not an allowed value" % mode)
        self.mode = mode
        #
        self.nevents = nevents
        self.physicslist = physicslist
        #
        if self.mode == "partgen" and not all([pid, energy, etarange]):
            raise Exception("In partgen mode, the pid, energy, and etarange params must be supplied")
        self.partgen_pid = pid
        self.partgen_energy = energy
        self.partgen_etarange = etarange
        ## Convert single-float eta to a symmetric range 2-tuple
        if type(self.partgen_etarange) is float:
            self.partgen_etarange = (-self.partgen_etarange, self.partgen_etarange)
        #
        if self.mode == "readevgen" and evgenfiles is None:
            raise Exception("In readevgen mode, the evgenfile param must be supplied")
        self.readevgen_evgenfiles = evgenfiles
        if type(self.readevgen_evgenfiles) is str:
            self.readevgen_evgenfiles = [self.readevgen_evgenfiles]
        self.readevgen_skipevents = skipevents

    def __str__(self):
        """Render a human-readable version of this run config."""
        s = ""
        if self.mode == "partgen":
            s += "ParticleGenerator: "
            s += "pid=+-{pid:d}, ".format(pid=self.partgen_pid)
            s += "etarange=[{eta[0]:f}, {eta[1]:f}], ".format(eta=self.partgen_etarange)
            s += "energy={e:f} MeV, ".format(e=self.partgen_energy)
            s += "{nevts:d} events".format(nevts=self.nevents)
        elif self.mode == "readevgen":
            s += "ReadEvgen: {evgen:s}, ".format(evgen=self.readevgen_evgenfiles)
            s += "events {start:d} to {end:d}".format(start=self.readevgen_skipevents, end=self.readevgen_skipevents+self.nevents)
        else:
            raise Exception("Invalid mode")
        return s


## Construct a RunConfig object from the string or tuple value
if "RUNCONFIG" in dir():
    if type(RUNCONFIG) is str:
        eval("RUNCONFIG=%s" % RUNCONFIG)
    elif type(RUNCONFIG) is dict:
        RUNCONFIG = RunConfig(**RUNCONFIG)


## Map RTT codes to RunConfigs
if "RUNCONFIGNUM" in dir():
    ## Mapping run config numbers to config strings
    from os.path import join as joinpath
    NUMTOCONFIG = {
        ## Single particle gen-on-the-fly
        300101 : RunConfig("partgen", 200, pid=11, etarange=2.5, energy=50000),
        300201 : RunConfig("partgen", 1000, pid=13, etarange=3.0, energy=100000),
        300301 : RunConfig("partgen", 200, pid=211, etarange=5.0, energy=50000),
        300302 : RunConfig("partgen", 300, pid=211, etarange=5.0, energy=50000, physicslist="QGSP_BERT_EMV"),
        200101 : RunConfig("partgen", 300, pid=11, etarange=2.5, energy=50000),
        200201 : RunConfig("partgen", 300, pid=13, etarange=3.0, energy=50000),
        200301 : RunConfig("partgen", 300, pid=211, etarange=3.2, energy=50000)
        }
    ## Read evgen from file for several releases
    BASEDIR15 = "/afs/cern.ch/atlas/offline/ProdData/15.6.11.3"
    BASEDIR16 = "/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.3"
    for baseid, basedir in [(10000, BASEDIR15), (20000, BASEDIR16)]:
        extraconfig = {
            ## Single particle events from file
            baseid + 100101 : RunConfig("readevgen", 300, evgenfiles=joinpath(basedir, "mu_E5_eta0-60.evgen.pool.root"), skipevents=0),
            baseid + 100201 : RunConfig("readevgen", 300, evgenfiles=joinpath(basedir, "mu_E50_eta0-25.evgen.pool.root"), skipevents=0),
            baseid + 100301 : RunConfig("readevgen", 300, evgenfiles=joinpath(basedir, "mu_E200_eta0-60.evgen.pool.root"), skipevents=0),
            baseid + 100401 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "e_E5_eta0-60.evgen.pool.root"), skipevents=0),
            baseid + 100501 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "e_E50_eta0-25.evgen.pool.root"), skipevents=0),
            baseid + 100601 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "e_E200_eta0-60.evgen.pool.root"), skipevents=0),
            baseid + 100602 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "e_E200_eta0-60.evgen.pool.root"), skipevents=50),
            baseid + 100603 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "e_E200_eta0-60.evgen.pool.root"), skipevents=100),
            baseid + 100604 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "e_E200_eta0-60.evgen.pool.root"), skipevents=150),
            baseid + 100701 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "pi_E5_eta0-60.evgen.pool.root"), skipevents=0),
            baseid + 100801 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "pi_E50_eta0-25.evgen.pool.root"), skipevents=0),
            baseid + 100901 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "pi_E200_eta0-60.evgen.pool.root"), skipevents=0),
            baseid + 100902 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "pi_E200_eta0-60.evgen.pool.root"), skipevents=50),
            baseid + 100903 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "pi_E200_eta0-60.evgen.pool.root"), skipevents=100),
            baseid + 100904 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "pi_E200_eta0-60.evgen.pool.root"), skipevents=150),
            ## Full physics events from file
            baseid + 101001 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "J2_jetjet-pythia6-7000.evgen.pool.root"), skipevents=0),
            baseid + 101002 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "J2_jetjet-pythia6-7000.evgen.pool.root"), skipevents=50),
            baseid + 101003 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "J2_jetjet-pythia6-7000.evgen.pool.root"), skipevents=100),
            baseid + 101004 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "J2_jetjet-pythia6-7000.evgen.pool.root"), skipevents=150),
            baseid + 101005 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "J2_jetjet-pythia6-7000.evgen.pool.root"), skipevents=150, physicslist="QGSP_BERT_EMV"),
            baseid + 101101 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "H130zz4l-pythia6-7000.evgen.pool.root"), skipevents=0),
            baseid + 101102 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "H130zz4l-pythia6-7000.evgen.pool.root"), skipevents=50),
            baseid + 101103 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "H130zz4l-pythia6-7000.evgen.pool.root"), skipevents=100),
            baseid + 101104 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "H130zz4l-pythia6-7000.evgen.pool.root"), skipevents=150),
            # TODO: In the r16 version, it's called minbias_Inelastic...
            baseid + 101201 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "minbias-pythia8-7000.evgen.pool.root"), skipevents=0),
            baseid + 101202 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "minbias-pythia8-7000.evgen.pool.root"), skipevents=50),
            baseid + 101203 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "minbias-pythia8-7000.evgen.pool.root"), skipevents=100),
            baseid + 101204 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "minbias-pythia8-7000.evgen.pool.root"), skipevents=150),
            baseid + 101301 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "susy_SU3-herwigpp-7000.evgen.pool.root"), skipevents=0),
            baseid + 101302 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "susy_SU3-herwigpp-7000.evgen.pool.root"), skipevents=50),
            baseid + 101303 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "susy_SU3-herwigpp-7000.evgen.pool.root"), skipevents=100),
            baseid + 101304 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "susy_SU3-herwigpp-7000.evgen.pool.root"), skipevents=150),
            baseid + 101401 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "Zmumu-herwigpp-7000.evgen.pool.root"), skipevents=0),
            baseid + 101402 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "Zmumu-herwigpp-7000.evgen.pool.root"), skipevents=50),
            baseid + 101403 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "Zmumu-herwigpp-7000.evgen.pool.root"), skipevents=100),
            baseid + 101404 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "Zmumu-herwigpp-7000.evgen.pool.root"), skipevents=150),
            baseid + 101501 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "Zee-herwigpp-7000.evgen.pool.root"), skipevents=0),
            baseid + 101502 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "Zee-herwigpp-7000.evgen.pool.root"), skipevents=50),
            baseid + 101503 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "Zee-herwigpp-7000.evgen.pool.root"), skipevents=100),
            baseid + 101504 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "Zee-herwigpp-7000.evgen.pool.root"), skipevents=150),
            baseid + 101601 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "A3_Ztautau_filter-pythia6-7000.evgen.pool.root"), skipevents=0),
            baseid + 101602 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "A3_Ztautau_filter-pythia6-7000.evgen.pool.root"), skipevents=50),
            baseid + 101603 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "A3_Ztautau_filter-pythia6-7000.evgen.pool.root"), skipevents=100),
            baseid + 101604 : RunConfig("readevgen",  50, evgenfiles=joinpath(basedir, "A3_Ztautau_filter-pythia6-7000.evgen.pool.root"), skipevents=150),
            baseid + 101701 : RunConfig("readevgen",  10, evgenfiles=joinpath(basedir, "ttbar_muplusjets-pythia6-7000.evgen.pool.root"), skipevents=0)
            }
        NUMTOCONFIG.update(extraconfig)
    if not NUMTOCONFIG.has_key(RUNCONFIGNUM):
        sys.stderr.write("RUNCONFIGNUM=%d does not map to a known run configuration.:\n" % RUNCONFIGNUM)
        sys.stderr.write("Available configurations:\n")
        for rttnum, rc in sorted(NUMTOCONFIG.iteritems()):
            sys.stderr.write("%d : %s\n" % (rttnum, str(rc)))
        sys.exit(2)
    RUNCONFIG = NUMTOCONFIG[RTTRunNumber]
    del NUMTOCONFIG


## Use RunConfig object for configuration
if not "RUNCONFIG" in dir():
    sys.stderr.write("No run config has been provided\n")
    sys.stderr.write("You must specify either the RUNCONFIG dict or RUNCONFIGNUM integer via the 'athena -c FOO=BAR' command line\n")
    sys.exit(3)


##################

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## Output printout level
ServiceMgr.MessageSvc.OutputLevel = WARNING

## Number of events
theApp.EvtMax = RUNCONFIG.nevents

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags, jobproperties
athenaCommonFlags.PoolHitsOutput = "g4atlas.hits.pool.root"
#athenaCommonFlags.PoolHitsOutput.set_Off()

## Set global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-SDR-BS7T-05-03"

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout = 'ATLAS-GEO-18-01-00'

## Set the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_On()

## Physics list
if RUNCONFIG.physicslist is not None:
    simFlags.PhysicsList = RUNCONFIG.physicslist

## Mode-specific stuff
if RUNCONFIG.mode.lower() == "readevgen":
    jobproperties.AthenaCommonFlags.PoolEvgenInput = RUNCONFIG.readevgen_evgenfiles
    jobproperties.AthenaCommonFlags.SkipEvents = RUNCONFIG.readevgen_skipevents
elif RUNCONFIG.mode.lower() == "partgen":
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.PoolEvgenInput.set_Off()
    athenaCommonFlags.SkipEvents.set_Off()
    import AthenaCommon.AtlasUnixGeneratorJob
    spgorders = ['pdgcode: constant 999',
                 'vertX: constant 0.0',
                 'vertY: constant 0.0',
                 'vertZ: constant 0.0',
                 't: constant 0.0',
                 'phi: flat  0 6.28318',
                 'eta: flat -{eta[0]:f} {eta[1]:f}'.format(eta=RUNCONFIG.partgen_eta),
                 'pdgcode: sequence -{pid:d} {pid:d}'.format(pid=RUNCONFIG.partgen_pid),
                 'e: constant {e:f}'.format(e=RUNCONFIG.partgen_energy)]
    from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
    job += ParticleGenerator()
    job.ParticleGenerator.orders = sorted(spgorders)
    job.ParticleGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()
    print "ParticleGenerator orders = %r" % sorted(spgorders)
else:
    raise Exception("Unknown RunConfig mode: how did we even get here?")


from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
job += PyG4AtlasAlg()

## For saving seeds
#from G4AtlasApps import AtlasG4Eng
#pp = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
#pp.set_SaveOn()

## Set process ID as environment variable (why?)
os.putenv("PYPROCID", "%d" % os.getpid())

## Check of memory
from G4AtlasApps.atlas_utilities import MemorySnooper
job += MemorySnooper()
