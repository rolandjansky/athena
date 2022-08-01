# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon import Logging
import os

rhlog = Logging.logging.getLogger('RHadronConfig')

def create_rhadron_particles_file(input_param_card='SLHA_INPUT.DAT',spectrum=1):
    """Create a list of particles for custom particle creation"""
    # Just use our helper function
    from RHadrons.RHadronMasses import update_particle_table
    update_particle_table(input_param_card, 'particles.txt', mass_spectrum=spectrum)


def create_rhadron_pdgtable(input_param_card='SLHA_INPUT.DAT',spectrum=1):
    """Add lines to the PDG table"""

    from ExtraParticles.PDGHelpers import getPDGTABLE
    if getPDGTABLE('PDGTABLE.MeV'): # FIXME make configurable
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


def SG_StepNtupleTool(flags, name="G4UA::SG_StepNtupleTool", **kwargs):
    result = ComponentAccumulator()
    if flags.Concurrency.NumThreads >1:
        log=Logging.logging.getLogger(name)
        log.fatal(' Attempt to run '+name+' with more than one thread, which is not supported')
        return False
    # Get the PDG IDs for RHadrons
    from RHadronMasses import offset_options
    kwargs.setdefault('RHadronPDGIDList',offset_options.keys())
    ## if name in simFlags.UserActionConfig.get_Value().keys(): ## FIXME missing functionality
    ##     for prop,value in simFlags.UserActionConfig.get_Value()[name].items():
    ##         kwargs.setdefault(prop,value)
    result.setPrivateTools( CompFactory.G4UA__SG_StepNtupleTool(name, **kwargs) )
    return result


def RHadronsPhysicsTool(flags, name='RHadronsPhysicsTool', **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools( CompFactory.RHadronsPhysicsTool(name,**kwargs) )
    return result
