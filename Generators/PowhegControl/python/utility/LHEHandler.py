# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl LHEHandler
#  PowhegControl Generic LHE file utilities
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

## Generic LHE file utilities
class LHEHandler :

  ## Initialise with a logging handler
  def __init__(self, logger=None):
    self._logger = logger


  ## Write to the logger with appropriate log-level
  def log(self, message, level='info') :
    if self._logger is not None :
      getattr( self._logger, level )( message )


  ## Merge many input LHE files into a single output file
  def merge( self, output_file, input_file_list ) :
    if len(input_file_list) < 1 : raise IOError( 'No input LHE files provided. Aborting merge!' )
    self.log( 'Preparing to create {0} from {1} input files'.format( output_file, len(input_file_list) ) )
    nEvents = 0

    # Open output file
    with open(output_file, 'ab') as f_output :
      # Start with the first file
      self.log( '... working on {0}'.format(input_file_list[0]) )
      output_footer = []
      with open(input_file_list[0],'rb') as f_input :
        in_LesHouchesEvents = True
        for line in f_input :
          if '<event>' in line : nEvents += 1
          if '</LesHouchesEvents>' in line : in_LesHouchesEvents = False
          if in_LesHouchesEvents :
            f_output.write( line )
          else :
            output_footer.append( line )

      # Now append other files in turn
      for file_name in input_file_list[1:] :
        self.log( '... working on {0}'.format(file_name) )
        in_event = False
        with open(file_name,'rb') as f_input :
          for line in f_input :
            if '<event>' in line : in_event = True; nEvents += 1
            if in_event :
              f_output.write( line )
            if '</event>' in line : in_event = False

      # Finally add the footer
      for line in output_footer :
        f_output.write( line )

    self.log( 'Wrote {0} events to {1}'.format( nEvents, output_file ) )


  ## Get new-style event weights from an input event string
  @staticmethod
  def weights_from_event(input_event) :
    comment_lines = input_event[input_event.find('#'):].replace('\n',' ').replace('</event>','')
    weight_lines = [ ' '.join(line.split()) for line in comment_lines.split('#') if 'new weight,renfact,facfact,pdf1,pdf2' in line ]
    return [ (line.split(' ')[-1],line.split(' ')[2]) for line in weight_lines ]
