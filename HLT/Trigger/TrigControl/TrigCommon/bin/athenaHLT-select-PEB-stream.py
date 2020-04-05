#!/usr/bin/env tdaq_python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# select events for a given stream name from an input file and write them in an outfile
# the output file obeys the conventions used by the SFO in P1

from __future__ import print_function

import sys
import os

def peb_writer(argv):
  """Runs the splitting routines"""

  import eformat, logging
  import EventApps.myopt as myopt

  option = {}

  # run mode options
  option['start-event'] = {'short': 'a', 'arg': True,
                           'default': 0,
                           'group': 'Run mode',
                           'description': 'Number of events which should be skipped from the begin'}

  option['max-events'] = {'short': 'n', 'arg': True,
                          'default': 0,
                          'group': 'Run mode',
                          'description': 'Maximum number of events in the output file. 0 means, all useful events from the input.'}

  option['verbosity'] = {'short': 'v', 'arg': True,
                         'default': logging.INFO,
                         'group': 'Run mode',
                         'description': 'Log verbosity'}

  option['progress-bar'] = {'short': 'P', 'arg': False,
                            'default': None,
                            'group': 'Run mode',
                            'description': 'Show progress bar when running interactively'}

  option['output-dir'] = {'short': 'd', 'arg': True,
                          'default': '.',
                          'group': 'Run mode',
                          'description': 'Directory in which the output file should be written'}

  # stream tag options
  option['stream-name'] = {'short': 's', 'arg': True,
                           'default': None,
                           'group': 'Stream Tag',
                           'description': 'Name of stream which should be written out'}

  option['project-tag'] = {'short': 'p', 'arg': True,
                           'default': None,
                           'group': 'Stream Tag',
                           'description': 'Project tag which should be used for the output file'}

  option['lumi-block'] = {'short': 'l', 'arg': True,
                          'default': -1,
                          'group': 'Stream Tag',
                          'description': 'Lumiblock number used for the output file. Use 0 if multiple LB in file.'}

  parser = myopt.Parser(extra_args=True)
  for (k,v) in option.items():
    parser.add_option(k, v['short'], v['description'], v['arg'], v['default'],v['group'])
 
  if len(sys.argv) == 1:
    print (parser.usage('global "%s" options:' % sys.argv[0]))
    sys.exit(1)

  # process the global options
  (kwargs, extra) = parser.parse(sys.argv[1:], prefix='global "%s" options:' % sys.argv[0])

  # global defaults
  logging.getLogger('').name = os.path.splitext(os.path.basename(sys.argv[0]))[0]
  logging.getLogger('').setLevel(kwargs['verbosity'])

  # input data stream
  stream = eformat.istream(extra)
  # input event counter
  totalEvents_in = 0

  # get metadata from inputfile
  dr = eformat.EventStorage.pickDataReader(extra[0])

  # interpret input file name
  df = eformat.EventStorage.RawFileName(extra[0])

  # extract some parameters from meta-data 
  projectTag      = dr.projectTag()
  lumiBlockNumber = dr.lumiblockNumber()
  applicationName = 'athenaHLT'
  streamType      = 'unknown' # the real stream type will be extracted from the matching stream tag
  if df.hasValidCore() :
    productionStep  = df.productionStep()
  else:
    productionStep  = 'unknown'

  # input parameters for building the output file name
  runNumber       = dr.runNumber() 
  outputDirectory = kwargs['output-dir']
  streamName      = kwargs['stream-name']
  if kwargs['project-tag'] is not None:
    projectTag      = kwargs['project-tag']
  if kwargs['lumi-block'] != -1:
    lumiBlockNumber = kwargs['lumi-block']  # if output file can have multiple lumi blocks, use 0 

  if (lumiBlockNumber==0):
    productionStep  = 'merge'

  # check the output directory if it exists
  if (not os.path.exists(outputDirectory)) or (not os.path.isdir(outputDirectory)): 
    logging.fatal(' Output directory %s does not exist ' % outputDirectory)
    sys.exit(1)

  # output event counter
  totalEvents_out = 0

  # counter of skipped events 
  totalEvents_skipped = 0

  # Loop over events
  for e in stream:
    totalEvents_in += 1

    # select events
    if kwargs['start-event'] > 0:
      kwargs['start-event'] -= 1
      totalEvents_skipped += 1
      continue

    if kwargs['max-events'] > 0 and totalEvents_in >= kwargs['max-events']:
      logging.info(' Maximum number of events reached : %d', kwargs['max-events'])
      break

    # find StreamTags and see if there is a match
    streamTags = e.stream_tag()
    logging.debug(' === New Event nr = %s (Run,Global ID) = (%d,%d) === ', totalEvents_in,e.run_no(),e.global_id())
    for tag in streamTags:
      if tag.name == streamName:
        # the event should be written out        
        logging.debug(' Matching event found for stream tag = %s', tag)
        logging.debug('      Stream Tag:Robs = %s', [hex(r) for r in tag.robs])
        logging.debug('      Stream Tag:Dets = %s', [hex(d) for d in tag.dets])

        # check the lumi block number from the event against the lumi block number defined for the file
        # this check is only done if the lumi block number for the file is different from 0
        if lumiBlockNumber > 0:
          if e.lumi_block() != lumiBlockNumber:
            logging.error(' Event (Run,Global ID) = (%d,%d) has a lumi block number %d,'
                          ' which is different from LB = %d for the output file. Event skipped.',
                          (e.run_no(),e.global_id(),e.lumi_block(),lumiBlockNumber))
            continue

        # check that all events have the same run number as the output file indicates otherwise skip event
        if e.run_no() != runNumber:
            logging.error(' Event (Run,Global ID) = (%d,%d) has a run number,'
                          ' which is different from the run number = %d for the output file. Event skipped.',
                          (e.run_no(),e.global_id(),runNumber))
            continue

        # set the overall tag type for the first match
        if streamType != tag.type:
          streamType = tag.type
          logging.debug(' streamType set to = %s', streamType)
          # create the RAW output file name
          outRawFile = eformat.EventStorage.RawFileName(projectTag,
                                                        runNumber,
                                                        streamType,
                                                        streamName,
                                                        lumiBlockNumber,
                                                        applicationName,
                                                        productionStep)
          logging.debug(' set output file name = %s', outRawFile.fileNameCore())

          # create the output stream
          ostream = eformat.ostream(directory=outputDirectory,
                                    core_name=outRawFile.fileNameCore(),
                                    run_number=dr.runNumber(), 
                                    trigger_type=dr.triggerType(),
                                    detector_mask=dr.detectorMask(), 
                                    beam_type=dr.beamType(),
                                    beam_energy=dr.beamEnergy())
        
        # decide what to write out
        if (len(tag.robs)==0 and len(tag.dets)==0):
          # write out the full event fragment
          pbev = eformat.write.FullEventFragment(e)  
          logging.debug(' Write full event fragment ')
        else:
          # select ROBs to write out
          rob_output_list = []
          logging.debug(' Write partial event fragment ')
          for rob in e: 
            if rob.source_id().code() in tag.robs:
              rob_output_list.append(rob)
            if rob.source_id().subdetector_id() in tag.dets:
              rob_output_list.append(rob)
          # write out the partial event fragment
          pbev = eformat.write.FullEventFragment()
          pbev.copy_header(e)
          for out_rob in rob_output_list: 
            pbev.append_unchecked(out_rob)
        
        # put the event onto the output stream
        ostream.write(pbev)
        if (logging.getLogger('').getEffectiveLevel() > logging.DEBUG) and kwargs['progress-bar']:
          sys.stdout.write('.')
          sys.stdout.flush()

        # increase output event counter
        totalEvents_out += 1

  # print final statistics
  logging.info('Total number of events processed          = %d ', totalEvents_in)
  logging.info('Number of events skipped at the beginning = %d ', totalEvents_skipped)
  logging.info('Number of events written to output file   = %d ', totalEvents_out)
  if totalEvents_out > 0:
    logging.info('Output file                               = %s ', ostream.last_filename())
  else:
    logging.error('No events selected so no output file created')
    sys.exit(1)

  sys.exit(0)

if __name__ == "__main__":
  peb_writer(sys.argv)
