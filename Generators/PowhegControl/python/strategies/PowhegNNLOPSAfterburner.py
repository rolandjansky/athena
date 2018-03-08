# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegNNLOPSAfterburner
#  NNLOPS afterburner to reweight Powheg events
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from OutputHandler import write_output
import glob, os, shutil, subprocess

def runPowhegNNLOPSAfterburner(configurator) :
  configurator.logger.info( 'Running NNLOPS reweighting' )
  LHEF_name = 'pwgevents.lhe'

  ## Strip comment strings from input LHEF file - NNLOPS reweighter will crash otherwise
  os.system( "sed -i 's/\!.*$//g' {0}".format( LHEF_name ) )

  ## Construct weight descriptor sets for consistent ordering
  NNLOPS_weight_descriptors = []
  for idx, (weight_ID, weight_description) in enumerate( sorted( configurator.NNLOPS_weights.items() ), start=4001 ) :
    for replacement in [ x for x in configurator.variations if x.weight_description in weight_description ] :
      weight_description = weight_description.replace( "\'{0}\'".format(replacement.weight_description), "\'{0}\'".format(replacement.weight_ID) )
    NNLOPS_weight_descriptors.append( (idx, weight_ID, weight_description) )


  ## Prepare the nnlopsreweighter runcard
  with open( 'nnlopsreweighter.input', 'wb' ) as f :
    ## Name of the input LHE file
    f.write( "lhfile {0}\n\n".format(LHEF_name) )

    ## Weights present in the lhfile: #mtinf, mt, mtmb, mtmb-bminlo
    ## a line with: 'nnlofiles' followed by a quoted label and the name of a HNNLO output file.
    f.write( "nnlofiles\n" )
    for label, HNNLO_file_name in configurator.NNLOPS_input.items() :
      f.write( "'{0}' {1}\n".format( label, HNNLO_file_name ) )
      if not os.path.isfile( HNNLO_file_name ) :
        configurator.logger.info( 'Looking for {0}'.format( HNNLO_file_name ) )
        try :
          shutil.copy( glob.glob( '{0}/../AuxFiles/*/{1}'.format( configurator.powheg_directory, HNNLO_file_name ) )[0], HNNLO_file_name )
        except OSError :
          configurator.logger.warning( '{0} does not exist!'.format( HNNLO_file_name ) )          
    f.write( "\n" )

    ## NNLOPS weights, in LHEv3 format: can be grouped as prefered with arbitrary IDs
    ## Description line tells reweighter how to calculate weights:
    ##  *) loops through the weight IDs in the LHEF file and through the labels of the nnlofiles.
    ##  *) if description contains a weight-label and an nnlofile-label:
    ##     - compute new weight by reweighting the corresponding weights in the input file with the result from the nnlofile
    f.write( "<initrwgt>\n" )
    f.write( "<weightgroup name='NNLOPS'>\n" )
    for idx, weight_ID, weight_description in NNLOPS_weight_descriptors : #configurator.NNLOPS_weights.items() :
      f.write( "<weight id='{0}'> {1} </weight>\n".format( weight_ID, weight_description ) )
    f.write( "</weightgroup>\n" )
    f.write( "</initrwgt>\n" )

  os.system( 'ls {0}/../AuxFiles/*/'.format( configurator.powheg_directory) )
  NNLOPS_executable = (configurator._powheg_executable).replace('pwhg_main','nnlopsreweighter')

  configurator.running_processes.append( (subprocess.Popen( [NNLOPS_executable,''], stdout=subprocess.PIPE, stdin=None, stderr=subprocess.STDOUT ), '') )
  write_output( configurator.running_processes, configurator.logger )

  ## Rename NNLOPS weights to fit ATLAS conventions - could be rewritten to use XML parsing, but this may be overkill
  configurator.logger.info( 'Reformatting NNLOPS reweighting output' )
  NNLOPS_weight_sets = [ (idx, ID_string, description) for idx, (ID_string, description) in enumerate( sorted(configurator.NNLOPS_weights.items()), start=4001 ) ]
  shutil.move( LHEF_name, '{0}.native'.format(LHEF_name) )
  with open( LHEF_name, 'wb' ) as f_output :
    with open( 'pwgevents.lhe.nnlo', 'rb' ) as f_input :
      for input_line in f_input :
        output_line = input_line
        for idx, weight_ID, weight_description in NNLOPS_weight_descriptors :
          if "id='{0}'".format(weight_ID) in output_line :
            # Switch weight ID/description naming
            output_line = output_line.replace( weight_ID, str(idx) ).replace( weight_description, weight_ID )
        if "wgt id=" in output_line :
          # Convert exponential delimiter
          output_line = output_line.replace( 'D+', 'E+' ).replace( 'D-', 'E-' )
        if len(output_line) > 1 : # strip empty lines
          f_output.write( output_line )
