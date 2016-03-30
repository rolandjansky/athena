# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl AfterburnerReweighting
#  Reweight pre-generated Powheg events
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import collections, os, shutil, subprocess, time
from GenerateSingleCore import *
from ..utility import FileParser, RepeatingTimer

## Initialise and validate reweighting lists
def initialise_reweighting(configurator) :
  configurator.logger.info( 'Initialising PDF/scale variations' )

  # Construct and sanitise PDF and scale lists
  configurator.PDF = map( int, configurator.PDF ) if isinstance(configurator.PDF,list) else [ configurator.PDF ]
  configurator.mu_F = map( float, configurator.mu_F ) if isinstance(configurator.mu_F,list) else [ configurator.mu_F ]
  configurator.mu_R = map( float, configurator.mu_R ) if isinstance(configurator.mu_R,list) else [ configurator.mu_R ]
  assert( len(configurator.mu_F) == len(configurator.mu_R) ), 'List of mu_F variations must be the same length as list of mu_R variations'

  # Construct nominal (assuming that it is first)
  configurator.logger.info( 'Using PDF={}, mu_F={}, mu_R={} as nominal sample'.format( configurator.PDF[0], configurator.mu_F[0], configurator.mu_R[0] ) )

  # Enable #rwgt information lines - these cause Pythia to crash if present in the final output
  FileParser('powheg.input').text_replace( 'storeinfo_rwgt 0', 'storeinfo_rwgt 1' )
  shutil.copy( 'powheg.input', 'powheg_nominal.input' )


## Run reweighting as an afterburner to existing events
def afterburner_reweighting(configurator) :
  # Tuples to hold reweighting information
  ScalePDFWeightSpecifier = collections.namedtuple( 'ScalePDFWeightSpecifier', ['PDF', 'mu_F', 'mu_R', 'weight_ID', 'weight_description', 'group_combination_method', 'group_description'] )
  ArbitraryWeightSpecifier = collections.namedtuple( 'ArbitraryWeightSpecifier', ['parameter_settings', 'weight_ID', 'weight_description', 'group_combination_method', 'group_description'] )

  # Backup of generation statistics
  if os.path.isfile( 'pwgcounters.dat' ) :
    shutil.copy( 'pwgcounters.dat', 'pwgcounters.dat.bak' )

  # Construct scale/PDF variations (ignoring first entry)
  configurator.variations = [ ScalePDFWeightSpecifier( PDF=configurator.PDF[0], mu_F=configurator.mu_F[0], mu_R=configurator.mu_R[0], weight_ID=0, weight_description='nominal', group_combination_method='', group_description='nominal' ) ]
  for idx, (mu_F, mu_R) in enumerate( zip( configurator.mu_F[1:], configurator.mu_R[1:] ) ) :
    configurator.variations.append( ScalePDFWeightSpecifier( PDF=configurator.PDF[0], mu_F=mu_F, mu_R=mu_R, weight_ID=1001+idx, weight_description='muR = {}, muF = {}'.format(mu_R,mu_F), group_combination_method='envelope', group_description='scale_variation' ) )
  for idx, PDF in enumerate( configurator.PDF[1:] ) :
    configurator.variations.append( ScalePDFWeightSpecifier( PDF=PDF, mu_F=configurator.mu_F[0], mu_R=configurator.mu_R[0], weight_ID=2001+idx, weight_description='PDF set = {0:d}'.format(PDF), group_combination_method='hessian', group_description='PDF_variation' ) )

  # Construct arbitrary variations
  weight_ID = 3001
  for group_name, group_variations in configurator.event_weight_groups.items() :
    for variation_name in [ name for name in group_variations.keys() if not name == 'parameter_names' ] :
      configurator.variations.append( ArbitraryWeightSpecifier( parameter_settings=group_variations[variation_name], weight_ID=weight_ID, weight_description=variation_name, group_combination_method='none', group_description=group_name ) )
      weight_ID += 1

  # Iterate over variations: they are cast to int to mitigate vulnerability from shell=True
  for idx_variation, variation in enumerate( configurator.variations ) :
    if idx_variation == 0 : continue # skip nominal variation
    configurator.logger.info( 'Preparing to run weight variation {0}/{1}'.format(idx_variation, len(configurator.variations)-1) )
    configurator.logger.info( '... weight name is:     {}'.format( variation.weight_description ) )
    configurator.logger.info( '... weight index ID is: {}'.format( variation.weight_ID ) )
    shutil.copy( 'powheg_nominal.input', 'powheg.input' )

    # As the nominal process has already been run, turn on compute_rwgt
    FileParser('powheg.input').text_replace( 'compute_rwgt 0', 'compute_rwgt 1' )

    # Ensure that manyseeds is turned off,  as this would cause the reweighting to crash
    FileParser('powheg.input').text_replace( 'manyseeds .*', 'manyseeds 0' )
    FileParser('powheg.input').text_replace( 'parallelstage .*', 'parallelstage -1' )

    # Apply scale/PDF variation settings
    if isinstance( variation, ScalePDFWeightSpecifier ) :
      configurator.logger.info( 'Weight variation type: scale/PDF' )
      FileParser('powheg.input').text_replace( 'lhans1 {}'.format(configurator.PDF[0]), 'lhans1 {0}'.format(variation.PDF) )
      FileParser('powheg.input').text_replace( 'lhans2 {}'.format(configurator.PDF[0]), 'lhans2 {0}'.format(variation.PDF) )
      configurator.logger.info( '... setting PDF to {}'.format( variation.PDF ) )
      FileParser('powheg.input').text_replace( 'facscfact {}'.format(configurator.mu_F[0]), 'facscfact {}'.format(variation.mu_F) )
      configurator.logger.info( '... setting factorisation scale to {}'.format( variation.mu_F ) )
      FileParser('powheg.input').text_replace( 'renscfact {}'.format(configurator.mu_R[0]), 'renscfact {}'.format(variation.mu_R) )
      configurator.logger.info( '... setting renormalisation scale to {}'.format( variation.mu_R ) )

    # Apply arbitrary settings
    if isinstance( variation, ArbitraryWeightSpecifier ) :
      configurator.logger.info( 'Weight variation type: user-defined weight group' )
      for (parameter,value) in variation.parameter_settings :
        try :
          for powheg_parameter in configurator.configurable_to_parameters[parameter] :
            FileParser('powheg.input').text_replace( '^{}.*'.format(powheg_parameter), '{} {}'.format(powheg_parameter,value) )
          configurator.logger.info( '... setting {} to {}'.format( parameter, value ) )
        except KeyError :
          self.logger.warning( 'Parameter "{}" not recognised. Cannot reweight!'.format(parameter) )

    # Set reweighting metadata
    FileParser('powheg.input').text_replace( "lhrwgt_descr \'nominal\'", "lhrwgt_descr \'{}\'".format(variation.weight_description) )
    FileParser('powheg.input').text_replace( "lhrwgt_id \'0\'", "lhrwgt_id \'{0}\'".format(variation.weight_ID) )
    FileParser('powheg.input').text_replace( "lhrwgt_group_combine \'none\'", "lhrwgt_group_combine \'{0}\'".format(variation.group_combination_method) )
    FileParser('powheg.input').text_replace( "lhrwgt_group_name \'none\'", "lhrwgt_group_name \'{0}\'".format(variation.group_description) )

    # Run the process until termination
    time_start = time.time()
    generate_single_core( configurator )
    shutil.move( 'pwgevents-rwgt.lhe', 'pwgevents.lhe' )
    configurator.logger.info( 'Finished weight variation {0}/{1} in {2}'.format(idx_variation, len(configurator.variations)-1, RepeatingTimer.human_readable_time_interval(time.time() - time_start)) )

  # Remove rwgt lines, which crash Pythia and are not needed if lhrwgt xml is present
  FileParser('pwgevents.lhe').text_remove( '^#rwgt' )
  FileParser('pwgevents.lhe').text_remove( '^#new weight' )

  # Correct LHE version identification; otherwise Pythia will treat all files as v1
  FileParser('pwgevents.lhe').text_replace( 'LesHouchesEvents version="1.0"', 'LesHouchesEvents version="3.0"' )

  # Restore generation statistics and initial runcard
  shutil.move( 'powheg_nominal.input', 'powheg.input' )
  if os.path.isfile( 'pwgcounters.dat.bak' ) :
    shutil.move( 'pwgcounters.dat.bak', 'pwgcounters.dat' )
