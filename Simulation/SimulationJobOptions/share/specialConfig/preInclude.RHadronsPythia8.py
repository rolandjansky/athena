#########################################################################
#      preInclude.RHadronsPythia8.py
#         Re-write of preInclude.RHadrons.py , relying on Pythia8 and
#         new mass spectrum infrastructure and helper functions
#########################################################################

import os

from AthenaCommon import Logging
rhlog = Logging.logging.getLogger('RHadronConfig')

def create_rhadron_particles_file(input_param_card='SLHA_INPUT.DAT',spectrum=1):
    """Create a list of particles for custom particle creation"""
    # Just use our helper function
    from RHadrons.RHadronMasses import update_particle_table
    update_particle_table(input_param_card, 'particles.txt', mass_spectrum=spectrum)


def create_rhadron_pdgtable(input_param_card='SLHA_INPUT.DAT',spectrum=1):
    """Add lines to the PDG table"""

    # Download generic PDGTABLE (overwrite existing one if it exists)
    os.system('get_files -remove -data PDGTABLE.MeV')

    # Update the PDG table using our helper function
    from RHadrons.RHadronMasses import update_PDG_table
    update_PDG_table('SLHA_INPUT.DAT','PDGTABLE.MeV',spectrum)


def addLineToPhysicsConfiguration(KEY, VALUE):
    """Add lines to the physics configuration"""
    os.system('touch PhysicsConfiguration.txt')
    newphysconfig = "{key} = {value}".format(key=KEY, value=VALUE)
    os.system('echo "%s" >> PhysicsConfiguration.txt' % newphysconfig)


def load_files_for_rhadrons_scenario(input_param_card='SLHA_INPUT.DAT',spectrum=1):
    """ Load all the files needed for a given scenario"""
    # Create custom PDGTABLE.MeV file
    create_rhadron_pdgtable(input_param_card,spectrum)
 
    # Create particles.txt file
    create_rhadron_particles_file(input_param_card,spectrum)

    from RHadrons.RHadronMasses import get_interaction_list
    get_interaction_list(input_param_card, interaction_file='ProcessList.txt', mass_spectrum=spectrum)

    # Remove existing physics configuration file ([MDJ]: FIXME: Is this happening earlier, or is it needed?)
    if os.path.isfile('PhysicsConfiguration.txt'):
        rhlog.warning("load_files_for_rhadrons_scenario() Found pre-existing PhysicsConfiguration.txt file - deleting.")
        os.remove('PhysicsConfiguration.txt')

  
# Whether we're in sim or digi
doG4SimConfig = True

# Keep the run number, aka DSID
runNumber = -1
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import PyUtils.AthFile as af
try:
    f = af.fopen(athenaCommonFlags.FilesInput()[0])
    if 'generators' in f.infos['tag_info']:
        usePythia8 = 'Py8' in f.infos['tag_info']['generators'] or 'Pythia8' in f.infos['tag_info']['generators']
        if not usePythia8: raise RuntimeError('Pythia8 not found in generator metadata - will abort')
    # MC Channel Number.  Try the standard two spots, and fall back to the run number for evgen
    if 'mc_channel_number' in f.infos and len(f.infos['mc_channel_number'])>0:
        runNumber = f.infos['mc_channel_number'][0]
    if runNumber<=0 and 'mc_channel_number' in f.infos['tag_info']:
        runNumber = f.infos['tag_info']['mc_channel_number']
    if runNumber<=0:
        runNumber = f.infos['run_number'][0]
    print 'ZLM:',f.infos
    # This is also used for digitization, so protect in case we're there
    if "StreamHITS" in f.infos["stream_names"]:
        from Digitization.DigitizationFlags import digitizationFlags
        simdict = digitizationFlags.specialConfiguration.get_Value()
        doG4SimConfig = False
    else:
        from G4AtlasApps.SimFlags import simFlags
        simdict = simFlags.specialConfiguration.get_Value()
except:
    from G4AtlasApps.SimFlags import simFlags
    simdict = simFlags.specialConfiguration.get_Value()

# Add the appropriate physics tool
from G4AtlasApps.SimFlags import simFlags
simFlags.PhysicsOptions += ["RHadronsPythia8PhysicsTool"]


# From the run number, load up the configuration.  Not the most beautiful thing, but this works.
from glob import glob
# Default position: look in cvmfs for job options
cvmfs_mc15 = '/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/'
JO = glob(cvmfs_mc15+'/share/DSID'+str(runNumber/1000)+'xxx/MC15.'+str(runNumber)+'*.py')
if len(JO)>0:
    JO = JO[0]
else:
    # Miss.  Try local
    JO = glob('MC15.'+str(runNumber)+'*.py')
    if len(JO)>0: JO=JO[0]
    else:
        # Miss.  Try one directory deeper
        JO = glob('*/MC15.'+str(runNumber)+'*.py')
        if len(JO)>0: JO=JO[0]
        else:
            # Miss.  Fall back to datapath
            for adir in os.environ['DATAPATH'].split(":"):
                JO = glob(adir+'/MC15.'+str(runNumber)+'*.py')
                if len(JO)>0:
                    JO=JO[0]
                    break
if not JO:
    raise RuntimeError('Could not locate job options for DSID '+str(runNumber))
# add any necessary elements to the runArgs here!
runArgs.jobConfig = [JO.split('/')[-1] if '/' in JO else JO]
runArgs.runNumber = runNumber
# Set up evgenLog logger - use this one
evgenLog=rhlog
# Set up evgenConfig just for a holder
class dummyClass():
    def __init(self):
        pass
    keywords = [] # So that they can be +='d in
evgenConfig = dummyClass()
evgenConfig.generators = []
evgenConfig.auxfiles = []
# Set up a fake pythia8...
genSeq = dummyClass()
genSeq.Pythia8 = dummyClass()
# Standard list of commands stolen from the Pythia8 base fragment
genSeq.Pythia8.Commands = [
    "6:m0 = 172.5",
    "23:m0 = 91.1876",
    "23:mWidth = 2.4952",
    "24:m0 = 80.399",
    "24:mWidth = 2.085",
    "StandardModel:sin2thetaW = 0.23113",
    "StandardModel:sin2thetaWbar = 0.23146",
    "ParticleDecays:limitTau0 = on",
    "ParticleDecays:tau0Max = 10.0"]
# Set up a fake TestHepMC
testSeq = dummyClass()
testSeq.TestHepMC = dummyClass()
# Block includes that we don't want running
include.block('MC15JobOptions/MadGraphControl_SimplifiedModelPostInclude.py')
include.block('MC15JobOptions/Pythia8_Base_Fragment.py')
include.block('MC15JobOptions/Pythia8_EvtGen.py')
include.block('MC15JobOptions/Pythia8_LHEF.py')

# Make sure all the files can be found
from EvgenJobTransforms.jo_proxy import mk_jo_proxy
mk_jo_proxy(cvmfs_mc15, "MC15JobOptions", "_joproxy15")

# Updating JOBOPTSEARCHPATH env var on the athena side
import re,os
from AthenaCommon import Include
Include.optionsPathEnv = os.environ[ 'JOBOPTSEARCHPATH' ]
Include.optionsPath = re.split( ',|' + os.pathsep, Include.optionsPathEnv )

# Include the job options themselves
include(JO)

# Build the param card, aka SLHA file
from MadGraphControl.MadGraphUtils import build_param_card
build_param_card(param_card_old='param_card.SM.%s.%s.dat'%(gentype,decaytype),param_card_new='SLHA_INPUT.DAT',masses=masses,decays=decays)

# Get the spectrum number if it's in the metadata
spectrum = 1 if 'SPECTRUM' not in simdict else simdict['SPECTRUM']

# Last step, load up the files
load_files_for_rhadrons_scenario('SLHA_INPUT.DAT',spectrum)


# Add any lines that were missing
# In case we want to use Pythia8 for decays during simulation
lifetime = float(simdict['LIFETIME']) if simdict.has_key("LIFETIME") else -1.
if lifetime<1. and hasattr(runArgs,'outputEVNT_TRFile'):
    rhlog.warning('Lifetime specified at <1ns, but you are writing stopped particle positions.')
    rhlog.warning('Assuming that you mean to use infinite lifetimes, and ignoring the setting')
else:
    addLineToPhysicsConfiguration("DoDecays","1")
    addLineToPhysicsConfiguration("HadronLifeTime", str(lifetime))
# If we reading particle records, and the lifetime is short, stop them as well
if lifetime<1. and hasattr(runArgs,'inputEVNT_TRFile'):
    addLineToPhysicsConfiguration("DoDecays","1")
    addLineToPhysicsConfiguration("HadronLifeTime", 0.000001)

# Capture Pythia8 commands
# Set up R-hadron masses in Pythia8
from RHadrons.RHadronMasses import get_Pythia8_commands
genSeq.Pythia8.Commands += get_Pythia8_commands('SLHA_INPUT.DAT',spectrum)
f = open('PYTHIA8_COMMANDS.TXT','w')
f.write('\n'.join(genSeq.Pythia8.Commands))
f.close()
# Done with the Pythia8 setup
