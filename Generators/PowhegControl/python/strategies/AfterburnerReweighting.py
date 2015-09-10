# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl AfterburnerReweighting
#  Reweight pre-generated Powheg events
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import collections, shutil, subprocess, time
from GenerateSingleCore import *
from ..utility import RepeatingTimer
import os

## Initialise and validate reweighting lists
def initialise_reweighting(configurator) :
  configurator.logger.info( 'Initialising PDF/scale variations' )

  ## Construct and sanitise PDF list
  if isinstance(configurator.PDF,list) :
    configurator.PDF = map( int, configurator.PDF )
  else :
    configurator.PDF = [ configurator.PDF ]

  ## Construct and sanitise scale lists
  if isinstance(configurator.mu_F,list) and isinstance(configurator.mu_R,list) :
    configurator.mu_F, configurator.mu_R = map( float, configurator.mu_F ), map( float, configurator.mu_R ) # sanitise input
  else :
    configurator.mu_F, configurator.mu_R = [ configurator.mu_F ], [ configurator.mu_R ]
  assert( len(configurator.mu_F) == len(configurator.mu_R) ), 'List of mu_F variations must be the same length as list of mu_R variations'

  ## Construct nominal (assuming that it is first)
  configurator.logger.info( 'Using PDF={0}, mu_F={1}, mu_R={2} as nominal sample'.format(configurator.PDF[0],configurator.mu_F[0],configurator.mu_R[0]) )

  ## Enable #rwgt information lines - these cause Pythia to crash if present in the final output
  subprocess.call( 'sed "s/storeinfo_rwgt 0/storeinfo_rwgt 1/g" powheg.input > powheg_nominal.input', shell=True )
  shutil.copy( 'powheg_nominal.input', 'powheg.input' )


## Run reweighting as an afterburner to existing events
def afterburner_reweighting(configurator) :
  ## Tuples to hold reweighting information
  ScalePDFWeightSpecifier = collections.namedtuple( 'ScalePDFWeightSpecifier', ['PDF', 'mu_F', 'mu_R', 'weight_ID', 'weight_description', 'group_combination_method', 'group_description'] )
  ArbitraryWeightSpecifier = collections.namedtuple( 'ArbitraryWeightSpecifier', ['parameter_settings', 'weight_ID', 'weight_description', 'group_combination_method', 'group_description'] )

  ## Backup of generation statistics
  if os.path.isfile( 'pwgcounters.dat' ) :
    shutil.copy( 'pwgcounters.dat', 'pwgcounters.dat.bak' )

  ## Construct scale/PDF variations (ignoring first entry)
  configurator.variations = [ ScalePDFWeightSpecifier( PDF=configurator.PDF[0], mu_F=configurator.mu_F[0], mu_R=configurator.mu_R[0], weight_ID=0, weight_description='nominal', group_combination_method='', group_description='nominal' ) ]
  for idx, (mu_F, mu_R) in enumerate( zip( configurator.mu_F[1:], configurator.mu_R[1:] ) ) :
    configurator.variations.append( ScalePDFWeightSpecifier( PDF=configurator.PDF[0], mu_F=mu_F, mu_R=mu_R, weight_ID=1001+idx, weight_description='muR={0:E} muF={1:E}'.format(mu_R,mu_F), group_combination_method='envelope', group_description='scale_variation' ) )
  for idx, PDF in enumerate( configurator.PDF[1:] ) :
    configurator.variations.append( ScalePDFWeightSpecifier( PDF=PDF, mu_F=configurator.mu_F[0], mu_R=configurator.mu_R[0], weight_ID=2001+idx, weight_description='pdfset= {0:d}'.format(PDF), group_combination_method='hessian', group_description='PDF_variation' ) )

  ## Construct arbitrary variations
  weight_ID = 3001
  for group_name, group_variations in configurator.reweight_groups.items() :
    for variation_name in sorted( group_variations.keys() ) :
      configurator.variations.append( ArbitraryWeightSpecifier( parameter_settings=group_variations[variation_name], weight_ID=weight_ID, weight_description=variation_name, group_combination_method='none', group_description=group_name ) )
      weight_ID += 1

  ## Iterate over variations: they are cast to int to mitigate vulnerability from shell=True
  for idx_variation, variation in enumerate( configurator.variations ) :
    if idx_variation == 0 : continue # skip nominal variation
    configurator.logger.info( 'Now running weight variation {0}/{1}'.format(idx_variation, len(configurator.variations)-1) )

    ## As the nominal process has already been run, turn on compute_rwgt
    subprocess.call( 'sed "s/compute_rwgt 0/compute_rwgt 1/g" powheg_nominal.input > powheg.input', shell=True )

    ## Ensure that manyseeds is turned off,  as this would cause the reweighting to crash
    subprocess.call( 'sed -i "s/manyseeds .*/manyseeds 0/g" powheg.input', shell=True )
    subprocess.call( 'sed -i "s/parallelstage .*/parallelstage -1/g" powheg.input', shell=True )

    ## Apply scale/PDF variation settings
    if isinstance( variation, ScalePDFWeightSpecifier ) :
      subprocess.call( 'sed -i "s/lhans1 {0}/lhans1 {1}/g" powheg.input'.format(configurator.PDF[0], variation.PDF), shell=True )
      subprocess.call( 'sed -i "s/lhans2 {0}/lhans2 {1}/g" powheg.input'.format(configurator.PDF[0], variation.PDF), shell=True )
      subprocess.call( 'sed -i "s/facscfact {0}/facscfact {1}/g" powheg.input'.format(configurator.mu_F[0], variation.mu_F), shell=True )
      subprocess.call( 'sed -i "s/renscfact {0}/renscfact {1}/g" powheg.input'.format(configurator.mu_R[0], variation.mu_R), shell=True )

    ## Apply arbitrary settings
    if isinstance( variation, ArbitraryWeightSpecifier ) :
      for (parameter,value) in variation.parameter_settings :
        configurator.logger.info( 'Setting {0} to {1}'.format( parameter, value ) )
        subprocess.call( 'sed -i "s/{0}.*/{0} {1}/g" powheg.input'.format(parameter,value), shell=True )

    ## Set reweighting metadata
    subprocess.call( 'sed -i "s/lhrwgt_descr \'nominal\'/lhrwgt_descr \'{0}\'/g" powheg.input'.format(variation.weight_description), shell=True )
    subprocess.call( 'sed -i "s/lhrwgt_id \'0\'/lhrwgt_id \'{0}\'/g" powheg.input'.format(variation.weight_ID), shell=True )
    subprocess.call( 'sed -i "s/lhrwgt_group_combine \'none\'/lhrwgt_group_combine \'{0}\'/g" powheg.input'.format(variation.group_combination_method), shell=True )
    subprocess.call( 'sed -i "s/lhrwgt_group_name \'none\'/lhrwgt_group_name \'{0}\'/g" powheg.input'.format(variation.group_description), shell=True )

    ## Run the process until termination
    time_start = time.time()
    generate_single_core( configurator )
    shutil.move( 'pwgevents-rwgt.lhe', 'pwgevents.lhe' )
    configurator.logger.info( 'Finished weight variation {0}/{1} in {2}'.format(idx_variation, len(configurator.variations)-1, RepeatingTimer.human_readable_time_interval(time.time() - time_start)) )

  ## Remove rwgt lines, which crash Pythia and are not needed if lhrwgt xml is present
  subprocess.call( 'sed -i "/^#rwgt/d" pwgevents.lhe', shell=True )
  subprocess.call( 'sed -i "/^#new weight/d" pwgevents.lhe', shell=True )

  ## Correct LHE version identification; otherwise Pythia will treat all files as v1
  subprocess.call( '''sed -i 's/LesHouchesEvents version="1.0"/LesHouchesEvents version="3.0"/g' pwgevents.lhe''', shell=True )

  ## Restore generation statistics and initial runcard
  shutil.move( 'powheg_nominal.input', 'powheg.input' )
  if os.path.isfile( 'pwgcounters.dat.bak' ) :
    shutil.move( 'pwgcounters.dat.bak', 'pwgcounters.dat' )
