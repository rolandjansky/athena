# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl OutputHandler
#  Translate raw process output into athena-logged output
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

##  Print single buffer to screen
def print_process_buffer( output_prefix, output_stream, logger ) :
  while True :
    output = output_stream.readline().rstrip()
    if len(output) == 0 : break
    logger.info( '{0}{1}'.format(output_prefix,output) )


##  Print output from all processes to screen
def write_output( process_list, logger ) :
  n_processes = len(process_list)
  while process_list :
    # Write output buffer if any
    for process, output_prefix in process_list :
      print_process_buffer( output_prefix, process.stdout, logger )
    # Check for finished processes and remove them from the list
    for process, output_prefix in process_list :
      if process.poll() is not None : # process has ended
        # Flush buffer and print final output (if any) to screen
        process.stdout.flush()
        print_process_buffer( output_prefix, process.stdout, logger )
        # Close output stream and remove process from list
        process.stdout.close()
        process_list.remove( (process,output_prefix) )
        logger.info( '{0}Process finished: there are now {1}/{2} running'.format(output_prefix,len(process_list),n_processes) )
