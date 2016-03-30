# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl AfterburnerNNLOReweighting
#  NNLOPS afterburner to reweight Powheg events
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..utility import LHEHandler, ProcessHandling
from xml.etree import ElementTree
import glob, mmap, os, shutil

## Convenience function to set up an instance of the reweighting class
def afterburner_NNLO_reweighting(configurator) :
  reweighter = AfterburnerNNLOReweighting( configurator )
  reweighter.run_NNLO_executable()
  reweighter.postprocess_NNLO_events()


## Handler for reweighting functions
class AfterburnerNNLOReweighting :

  def __init__(self, configurator) :
    self._logger = configurator.logger
    self._logger.info( 'Initialising NNLO reweighting' )

    # Read-in inputs and outputs
    self._LHEF_name = 'pwgevents.lhe'
    self._LHEF_nnlo_name = 'pwgevents.lhe.nnlo'
    self._NNLO_reweighting_inputs = configurator.NNLO_reweighting_inputs
    self._NNLO_output_weights = configurator.NNLO_output_weights
    self._nominal_group_name = [ x[1] for x in configurator.fixed_parameters if x[0] == 'lhrwgt_group_name' ][0]

    # Strip comment strings from input LHEF file - reweighter will crash otherwise
    self._logger.info( 'Removing comments from input LHE file' )
    os.system( "sed -i 's/\!.*$//g' {0}".format( self._LHEF_name ) )

    # Construct weight descriptor sets for consistent ordering
    self._logger.info( 'Constructing list of weights' )
    self._NNLO_weight_descriptors = []
    for idx, (weight_ID, weight_description) in enumerate( sorted( self._NNLO_output_weights.items() ), start=4001 ) :
      if hasattr( configurator, 'variations' ) :
        for replacement in [ x for x in configurator.variations if x.weight_description in weight_description ] :
          weight_description = weight_description.replace( "\'{0}\'".format(replacement.weight_description), "\'{0}\'".format(replacement.weight_ID) )
      self._NNLO_weight_descriptors.append( (idx, weight_ID, weight_description) )

    # Get NNLO executable
    self._NNLO_executable = self.get_executable( configurator._powheg_executable )
    self._logger.info( 'Using NNLO reweighter at {0}'.format(self._NNLO_executable ) )

    # Get reweighting file
    for NNLO_reweighting_file_name in self._NNLO_reweighting_inputs.values() :
      if os.path.isfile( NNLO_reweighting_file_name ) :
        self._logger.info( 'Using local configuration file {}'.format( NNLO_reweighting_file_name ) )
      else :
        self._logger.info( 'Trying to acquire configuration file {}'.format( NNLO_reweighting_file_name ) )
        try :
          shutil.copy( glob.glob( '{}/../AuxFiles/*/{}'.format( configurator.powheg_directory, NNLO_reweighting_file_name ) )[0], NNLO_reweighting_file_name )
        except OSError :
          self._logger.warning( '{} does not exist!'.format( NNLO_reweighting_file_name ) )
          self._logger.warning( '(1) if you generated this file then please ensure it is visible to Powheg on-the-fly.' )
          self._logger.warning( '(2) if the twiki says this file is provided by Powheg on-the-fly, please submit a bug report.' )


  ## Run the NNLO executable
  def run_NNLO_executable(self) :
    # Run NNLOPS
    if 'nnlopsreweighter' in self._NNLO_executable  :
      self.construct_NNLOPS_runcard()
      processes = [ ProcessHandling.SingleProcessThread( self._NNLO_executable , self._logger ) ]
      manager = ProcessHandling.ProcessManager( processes )
      while manager.monitor( self._logger ) : pass

    # Run DYNNLO
    elif 'minnlo' in self._NNLO_executable  :
      # Stage 1 - produce MINLO-W*-denom.top files
      stage_1_command = self.construct_DYNNLO_argument_list()
      self._logger.info( 'Running reweighting stage 1: denominator calculation' )
      processes = [ ProcessHandling.SingleProcessThread( stage_1_command, self._logger ) ]
      manager = ProcessHandling.ProcessManager( processes )
      while manager.monitor( self._logger ) : pass

      # Stage 2 - produce MINLO-W*-denom.top files
      stage_2_command = stage_1_command + [ 'MINLO-W{0}-denom.top'.format(idx) for idx in range(1,len(self._NNLO_reweighting_inputs)+1) ]
      self._logger.info( 'Running reweighting stage 2: reweighting with pre-calculated denominators' )
      processes = [ ProcessHandling.SingleProcessThread( stage_2_command, self._logger ) ]
      manager = ProcessHandling.ProcessManager( processes )
      while manager.monitor( self._logger ) : pass

      # Rename output
      shutil.move( 'pwgevents.lhe-nnlo', 'pwgevents.lhe.nnlo' )


  ## Postprocessing for NNLOPS and DYNNLO events
  def postprocess_NNLO_events(self) :
    shutil.move( self._LHEF_name, '{0}.native'.format(self._LHEF_name) )
    self._logger.info( 'Reformatting NNLO reweighting output' )
    # Run NNLOPS
    if 'nnlopsreweighter' in self._NNLO_executable  :
      self.postprocess_NNLOPS_events()
    # Run DYNNLO
    elif 'minnlo' in self._NNLO_executable  :
      self.postprocess_DYNNLO_events()


  ## Determine the path to the appropriate executable
  def get_executable( self, powheg_executable ) :
    process = powheg_executable.split('/')[-2]
    if process == 'HJ' : relative_path = 'nnlopsreweighter'
    elif process == 'Wj' : relative_path = 'DYNNLOPS/WNNLOPS/Reweighter/minnlo'
    elif process == 'Zj' : relative_path = 'DYNNLOPS/ZNNLOPS/Reweighter/minnlo'
    return powheg_executable.replace('pwhg_main',relative_path)


  ## Construct the argument list needed by DYNNLOPS reweighter
  def construct_DYNNLO_argument_list(self) :
    DYNNLO_top_files = self._NNLO_reweighting_inputs.values()
    return [ self._NNLO_executable, 'pwgevents.lhe', len(DYNNLO_top_files) ] + DYNNLO_top_files


  ## Construct the run card needed by NNLOPSreweighter
  def construct_NNLOPS_runcard(self) :
    self._logger.info( 'Constructing NNLOPS run card' )
    # Prepare the nnlopsreweighter runcard
    with open( 'nnlopsreweighter.input', 'wb' ) as f :
      # Name of the input LHE file
      f.write( "lhfile {0}\n\n".format(self._LHEF_name) )

      # Weights present in the lhfile: #mtinf, mt, mtmb, mtmb-bminlo
      # a line with: 'nnlofiles' followed by a quoted label and the name of a HNNLO output file.
      f.write( "nnlofiles\n" )
      for label, NNLO_reweighting_file_name in self._NNLO_reweighting_inputs.items() :
        f.write( "'{0}' {1}\n".format( label, NNLO_reweighting_file_name ) )
      f.write( "\n" )

      # NNLOPS weights, in LHEv3 format: can be grouped as prefered with arbitrary IDs
      # Description line tells reweighter how to calculate weights:
      #  *) loops through the weight IDs in the LHEF file and through the labels of the nnlofiles.
      #  *) if description contains a weight-label and an nnlofile-label:
      #     - compute new weight by reweighting the corresponding weights in the input file with the result from the nnlofile
      f.write( "<initrwgt>\n" )
      f.write( "<weightgroup name='NNLOPS'>\n" )
      for idx, weight_ID, weight_description in self._NNLO_weight_descriptors :
        f.write( "<weight id='{0}'> {1} </weight>\n".format( weight_ID, weight_description ) )
      f.write( "</weightgroup>\n" )
      f.write( "</initrwgt>\n" )


  def postprocess_NNLOPS_events(self) :
    self._logger.info( 'Renaming NNLO weights' )
    ## Rename NNLO weights to fit ATLAS conventions - could be rewritten to use XML parsing, but this may be overkill
    NNLO_weight_sets = [ (idx, ID_string, description) for idx, (ID_string, description) in enumerate( sorted(self._NNLO_output_weights.items()), start=4001 ) ]
    with open( self._LHEF_name, 'wb' ) as f_output :
      with open( self._LHEF_nnlo_name, 'rb' ) as f_input :
        for input_line in f_input :
          output_line = input_line
          for idx, weight_ID, weight_description in self._NNLO_weight_descriptors :
            # Switch weight ID/description naming
            if "id='{0}'".format(weight_ID) in output_line :
              output_line = output_line.replace( weight_ID, str(idx) ).replace( weight_description, weight_ID )
          # Convert exponential delimiter
          if "wgt id=" in output_line :
            output_line = output_line.replace('D+','E+').replace('D-','E-')
          if len(output_line) > 1 : # strip empty lines
            f_output.write( output_line )


  def postprocess_DYNNLO_events(self) :
    self._logger.info( 'Converting output to LHEv3 format' )
    # Extract header, footer and representative event
    with open( self._LHEF_nnlo_name, 'rb' ) as f_input :
      input_string = mmap.mmap( f_input.fileno(), 0, access=mmap.ACCESS_READ )
      if input_string.find('<header>') != -1 :
        intro = input_string[ : input_string.find('<header>') ]
        header = input_string[ input_string.find('<header>') : input_string.find('</header>')+9 ]
      else :
        intro = input_string[ : input_string.find('<init>') ]
        header = '<header></header>'
      init = input_string[ input_string.find('<init>') : input_string.find('</init>')+8 ]
      if input_string.find('<LesHouchesEvents>') != -1 :
        footer = input_string[ input_string.find('</LesHouchesEvents>') : ]
      else :
        footer = '</LesHouchesEvents>'
      first_event = input_string[ input_string.find('<event>') : input_string.find('</event>')+8 ]

    ## Get weight names and header
    weights = LHEHandler.weights_from_event(first_event)
    header_elem = ElementTree.fromstring(header)
    if header_elem.find('initrwgt') is None :
      header_elem.append( ElementTree.fromstring('<initrwgt></initrwgt>') )

    ## Add nominal and DYNNLO weightgroups to the header
    weightgroups = [ x for x in header_elem.find('initrwgt').findall('weightgroup') ]
    for weightgroup_name in [ self._nominal_group_name, 'dynnlo' ] :
      if not any( [ weightgroup.attrib['name'] == weightgroup_name for weightgroup in weightgroups ] ) :
        header_elem.find('initrwgt').append( ElementTree.fromstring('<weightgroup combine="none" name="{0}"></weightgroup>'.format(weightgroup_name)) )
        self._logger.info( 'Adding LHEv3 weightgroup: {0}'.format(weightgroup_name) )

    ## Add nominal weight name to header
    weightgroup = [ x for x in header_elem.find('initrwgt').findall('weightgroup') if x.attrib['name'] == self._nominal_group_name ][0]
    if not any( [ weight.attrib['id'] == '0' for weight in weightgroup.findall('weight') ] ) :
      weightgroup.append( ElementTree.fromstring("<weight id='0'> nominal </weight>") )
      self._logger.info( 'Adding LHEv3 weight: nominal' )

    ## Add DYNNLO weight names to header
    try :
      weight_elems = header_elem.iter(tag='weight') # needs python >= 2.7
    except AttributeError :
      weight_elems = header_elem.getiterator(tag='weight')
    existing_weights = sorted( [ int(weight_elem.attrib['id']) for weight_elem in weight_elems ] )
    weight_number_offset = max( [ w for w in existing_weights if 4000<=w<5000 ]+[4000] ) + 1
    weightgroup = [ x for x in header_elem.find('initrwgt').findall('weightgroup') if x.attrib['name'] == 'dynnlo' ][0]
    weight_name_to_id = {}
    for idx, weight in enumerate(weights) :
      weight_name_to_id[weight[0]] = weight_number_offset+idx
      weightgroup.append( ElementTree.fromstring('<weight id="{0}"> {1} </weight>'.format(weight_name_to_id[weight[0]],weight[0])) )
      self._logger.info( 'Adding LHEv3 weight: {0}'.format(weight[0]) )
    self.reindent(header_elem)

    ## Process input events
    with open( self._LHEF_name, 'wb' ) as f_output :
      f_output.write( intro )
      ElementTree.ElementTree( header_elem ).write( f_output )
      f_output.write( init )
      with open( self._LHEF_nnlo_name, 'rb' ) as f_input :
        in_event, event_lines = False, ''
        for line in f_input :
          if '<event>' in line : in_event = True
          if in_event : event_lines += line
          if '</event>' in line :
            # Construct LHEv3 weight lines
            if '#new weight,renfact,facfact,pdf1,pdf2' in event_lines :
              if not '<rwgt>' in event_lines :
                nominal_weight = [ x for x in event_lines.split('\n')[1].split(' ') if len(x) > 0 ][2]
                event_lines = event_lines[:event_lines.find('#')] + "<rwgt>\n<wgt id='0'> {0} </wgt>\n</rwgt>\n".format(nominal_weight) + event_lines[event_lines.find('#'):]
              weight_lines = ''.join( [ "<wgt id='{0}'> {1} </wgt>\n".format( weight_name_to_id[weight[0]], weight[1] ) for weight in LHEHandler.weights_from_event( event_lines ) ] )
              output_lines = event_lines[:event_lines.find('</rwgt>')] + weight_lines + event_lines[event_lines.find('</rwgt>'):event_lines.find('#new weight,renfact,facfact,pdf1,pdf2')]+'</event>\n'
            else :
              output_lines = event_lines
            f_output.write( output_lines )
            in_event, event_lines = False, ''
      f_output.write( footer )


  def reindent( self, elem ) :
    if len(elem) :
      if not elem.text or not elem.text.strip() :
        elem.text = '\n'
      if not elem.tail or not elem.tail.strip() :
        elem.tail = '\n'
      for subelem in elem :
        self.reindent(subelem)
      if not elem.tail or not elem.tail.strip() :
        elem.tail = '\n'
    else :
      if not elem.tail or not elem.tail.strip() :
        elem.tail = '\n'
