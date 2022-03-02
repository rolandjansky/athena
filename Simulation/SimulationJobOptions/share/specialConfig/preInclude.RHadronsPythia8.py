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

    from G4AtlasApps.SimFlags import simFlags
    from ExtraParticles.PDGHelpers import getPDGTABLE
    if getPDGTABLE(simFlags.ExtraParticlesPDGTABLE.get_Value()):
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
    elif 'mc_channel_number' in f.infos['tag_info']:
        runNumber = f.infos['tag_info']['mc_channel_number']
    else:
        runNumber = f.infos['run_number'][0]

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
simFlags.PhysicsOptions += ["RHadronsPhysicsTool"]


# From the run number, load up the configuration.  Not the most beautiful thing, but this works.
from glob import glob
# Default position: look in cvmfs for job options

if(runNumber ==  449497 ):
   runNumber =  421442
   print('run number changed from 449497 to ',str(runNumber))

cvmfs_mc16 = '/cvmfs/atlas.cern.ch/repo/sw/Generators/MC16JobOptions/'

JO = glob(cvmfs_mc16+str(int(runNumber/1000))+'xxx/'+str(runNumber)+'/mc.'+'*.py')

JO_path = cvmfs_mc16+str(int(runNumber/1000))+'xxx/'+str(runNumber)
os.environ["JOBOPTSEARCHPATH"] = JO_path +":"+os.environ["JOBOPTSEARCHPATH"]
os.environ["DATAPATH"] = JO_path +":"+os.environ["DATAPATH"]
if len(JO)>0:
    JO = JO[0]
else:
    # Miss.  Try local in dir=DSID
    JO = glob(str(runNumber)+'/mc.'+'*.py')
    if len(JO)>0: JO=JO[0]
    else:
        # Miss.  Try one directory deeper (any name)
        JO = glob('*/mc.'+str(runNumber)+'*.py')
        if len(JO)>0: JO=JO[0]
        else:
            # Miss.  Try local
            JO = glob('mc.'+'*.py')
            if len(JO)>0: JO=JO[0]
            else:
                # Miss.  Fall back to datapath
                for adir in os.environ['DATAPATH'].split(":"):
                   JO = glob(adir+'/mc.'+'*.py')
                   if len(JO)>0:
                      JO=JO[0]
                      break
if not JO:
    raise RuntimeError('Could not locate job options for DSID '+str(runNumber))
# add any necessary elements to the runArgs here!
runArgs.jobConfig = [JO.split('/')[-2] if '/' in JO else JO]
runArgs.runNumber = runNumber
runArgs.ecmEnergy = 13000.
runArgs.randomSeed = 1234
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
include.block('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')
include.block('Pythia8_i/Pythia8_Base_Fragment.py')
include.block('Pythia8_i/Pythia8_EvtGen.py')
include.block('Pythia8_i/Pythia8_LHEF.py')

# Updating JOBOPTSEARCHPATH env var on the athena side
import re,os
from AthenaCommon import Include
Include.optionsPathEnv = os.environ[ 'JOBOPTSEARCHPATH' ]
Include.optionsPath = re.split( ',|' + os.pathsep, Include.optionsPathEnv )

# Include the job options themselves
include(JO)

# Build the param card, aka SLHA file
from MadGraphControl.MadGraphUtils import modify_param_card
modify_param_card(param_card_input='param_card.SM.%s.%s.dat'%(gentype,decaytype),params={'MASS': masses,'DECAY':decays},output_location='SLHA_INPUT.DAT')

# Get the spectrum number if it's in the metadata
spectrum = 1 if 'SPECTRUM' not in simdict else simdict['SPECTRUM']

# Last step, load up the files
load_files_for_rhadrons_scenario('SLHA_INPUT.DAT',spectrum)

# Add any lines that were missing
# In case we want to use Pythia8 for decays during simulation
lifetime = float(simdict['LIFETIME']) if "LIFETIME" in simdict else -1.
if lifetime>0.:
    if lifetime<1. and hasattr(runArgs,'outputEVNT_TRFile'):
        rhlog.warning('Lifetime specified at <1ns, but you are writing stopped particle positions.')
        rhlog.warning('Assuming that you mean to use infinite lifetimes, and ignoring the setting')
    else:
        addLineToPhysicsConfiguration("DoDecays","1")
        addLineToPhysicsConfiguration("HadronLifeTime", str(lifetime))
    # If we are reading particle records, and the lifetime is short, stop them as well
    if lifetime<1. and hasattr(runArgs,'inputEVNT_TRFile'):
        addLineToPhysicsConfiguration("DoDecays","1")
        addLineToPhysicsConfiguration("HadronLifeTime", 0.000001)
else:
    # Stable case. Can be unset lifetime or lifetime=0 or lifetime=-1
    addLineToPhysicsConfiguration("DoDecays","0")
    addLineToPhysicsConfiguration("HadronLifeTime", -1)


# Capture Pythia8 commands
# Set up R-hadron masses in Pythia8
from RHadrons.RHadronMasses import get_Pythia8_commands
genSeq.Pythia8.Commands += get_Pythia8_commands('SLHA_INPUT.DAT',spectrum)
f = open('PYTHIA8_COMMANDS.TXT','w')
f.write('\n'.join(genSeq.Pythia8.Commands))
f.close()
# Done with the Pythia8 setup
