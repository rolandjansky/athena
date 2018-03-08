# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Run a single Powheg process with multiple passes
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
import collections, shutil, subprocess, time
from PowhegSingleThread import *
from OutputHandler import write_output
from ..utility import RepeatingTimer

###############################################################################
#
#  Register a single Powheg process
#
###############################################################################
def runPowhegV2Multiweight(configurator) :
  configurator.logger().info( 'Running in single core mode with PDF/scale variations' )

  # Construct PDF list
  if isinstance(configurator.PDF,list) :
    PDF_list = [ int(x) for x in configurator.PDF ] # sanitise input
  else :
    PDF_list = [ configurator.PDF ]

  # Construct scale lists
  if isinstance(configurator.mu_F,list) and isinstance(configurator.mu_R,list) :
    mu_F_list, mu_R_list = [ float(x) for x in configurator.mu_F ], [ float(x) for x in configurator.mu_R ] # sanitise input
  else :
    mu_F_list, mu_R_list = [ configurator.mu_F ], [ configurator.mu_R ]
  assert( len(mu_F_list) == len(mu_R_list) ), 'List of mu_F variations must be the same length as list of mu_R variations'

  # Tuple to hold reweighting information
  WeightSpecifier = collections.namedtuple( 'WeightSpecifier', ['PDF', 'mu_F', 'mu_R', 'weight_ID', 'weight_description', 'group_combination_method', 'group_description'] )

  # Construct nominal (assuming that it is first)
  configurator.logger().info( 'Using PDF={0}, mu_F={1}, mu_R={2} as nominal sample'.format(PDF_list[0],mu_F_list[0],mu_R_list[0]) )
  variations = [ WeightSpecifier( PDF=PDF_list[0], mu_F=mu_F_list[0], mu_R=mu_R_list[0], weight_ID=0, weight_description='nominal', group_combination_method='', group_description='' ) ]

  # Construct variations (ignoring first entry)
  for idx, (mu_F, mu_R) in enumerate( zip( mu_F_list[1:], mu_R_list[1:] ) ) :
    variations.append( WeightSpecifier( PDF=PDF_list[0], mu_F=mu_F, mu_R=mu_R, weight_ID=1001+idx, weight_description='muR={0:E} muF={1:E}'.format(mu_R,mu_F), group_combination_method='envelope', group_description='scale_variation' ) )
  for idx, PDF in enumerate( PDF_list[1:] ) :
    variations.append( WeightSpecifier( PDF=PDF, mu_F=mu_F_list[0], mu_R=mu_R_list[0], weight_ID=2001+idx, weight_description='pdfset= {0:d}'.format(PDF), group_combination_method='hessian', group_description='PDF_variation' ) )

  # Enable #rwgt information lines - these cause Pythia to crash if present in the final output
  subprocess.call( 'sed "s/storeinfo_rwgt 0/storeinfo_rwgt 1/g" powheg.input > powheg_nominal.input', shell=True )

  # Iterate over variations
  for idx_variation, variation in enumerate( variations ) :
    configurator.logger().info( 'Now running weight variation {0}/{1}'.format(idx_variation, len(variations)) )
    # Use default runcard for nominal, turn on compute_rwgt otherwise
    if idx_variation == 0 : shutil.copy( 'powheg_nominal.input', 'powheg.input' )
    else : subprocess.call( 'sed "s/compute_rwgt 0/compute_rwgt 1/g" powheg_nominal.input > powheg.input', shell=True )
    # NB. shell=True is unsafe when combined with user input: to guard against bad input PDF_list, mu_F_list and mu_R_list are forced into numeric form above
    subprocess.call( 'sed -i "s/lhans1 {0}/lhans1 {1}/g" powheg.input'.format(PDF_list[0], variation.PDF), shell=True )
    subprocess.call( 'sed -i "s/lhans2 {0}/lhans2 {1}/g" powheg.input'.format(PDF_list[0], variation.PDF), shell=True )
    subprocess.call( 'sed -i "s/facscfact {0}/facscfact {1}/g" powheg.input'.format(mu_F_list[0], variation.mu_F), shell=True )
    subprocess.call( 'sed -i "s/renscfact {0}/renscfact {1}/g" powheg.input'.format(mu_R_list[0], variation.mu_R), shell=True )
    subprocess.call( 'sed -i "s/lhrwgt_descr \'nominal\'/lhrwgt_descr \'{0}\'/g" powheg.input'.format(variation.weight_description), shell=True )
    subprocess.call( 'sed -i "s/lhrwgt_id \'0\'/lhrwgt_id \'{0}\'/g" powheg.input'.format(variation.weight_ID), shell=True )
    subprocess.call( 'sed -i "s/lhrwgt_group_combine \'none\'/lhrwgt_group_combine \'{0}\'/g" powheg.input'.format(variation.group_combination_method), shell=True )
    subprocess.call( 'sed -i "s/lhrwgt_group_name \'none\'/lhrwgt_group_name \'{0}\'/g" powheg.input'.format(variation.group_description), shell=True )

    # Run the process until termination
    time_start = time.time()
    runPowhegSingleThread( configurator )
    if idx_variation > 0 : shutil.move( 'pwgevents-rwgt.lhe', 'pwgevents.lhe' )
    configurator.logger().info( 'Finished weight variation {0}/{1} in {2}'.format(idx_variation, len(variations), RepeatingTimer.human_readable_time_interval(time.time() - time_start)) )

  # Remove rwgt lines, which are not needed if lhrwgt xml is present
  subprocess.call( 'sed -i "/^#rwgt/d" pwgevents.lhe', shell=True )
  subprocess.call( 'sed -i "/^#new weight/d" pwgevents.lhe', shell=True )
