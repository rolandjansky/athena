#!/usr/bin/env python3

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# $Id: aprof.py 783179 2016-11-09 11:13:54Z limosani $
#
# This is a helper script for quickly interpreting the profile file(s)
# produced by gathena.py.
#

##
# @short C++ style main function
#
# This function takes care of running pprof with the correct arguments
#
# @returns The status code returned by pprof
#
def main():

    # Set up a message logger:
    import sys
    import os.path
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( os.path.splitext( os.path.basename( sys.argv[ 0 ] ) )[ 0 ] )

    # Read in the command line options:
    import optparse
    desc = "This is a simple helper script for easily turning profile files into " \
           "something that's more easily readable. It doesn't provide too many " \
           "options, but it prints everyhing that it's doing. So it's easy to " \
           "tweak a bit what it did. Usage instructions can be found on: " \
           "[Put TWiki here]"
    vers = "$Revision: 783179 $"
    parser = optparse.OptionParser( description = desc,
                                    version = vers,
                                    usage = "%prog [options] <profile file> <output file>" )
    parser.add_option( "-v", "--verbosity", dest = "verbosity",
                       action = "store", type = "int", default = 3,
                       help = "Script message verbosity level" )

    ( options, files ) = parser.parse_args()

    # A little sanity check:
    if len( files ) != 2:
        logger.error( "You have to provide one input and output file for the script!" )
        parser.print_help()
        return 1

    # Set the logging level:
    logging.getLogger( "" ).setLevel( options.verbosity )

    # Extract the file names:
    input_file  = files[ 0 ]
    output_file = files[ 1 ]

    # Make the output file based on the extension of it:
    if output_file.endswith( ".pdf" ):
        return makePdf( input_file, output_file )
    elif output_file.endswith( ".callgrind" ):
        return makeCallgrind( input_file, output_file )
    else:
        logger.error( "Output format not recognized" )
        return 1

##
# @short Function making a PDF file from the profile file
#
# This function is used to make a meaningful PDF from the profile
# file produced by gathena.py.
#
# @param input_file  Profile file created in a profiled job
# @param output_file Name of the output PDF file to be created
# @returns The status code returned by the pprof command
def makePdf( input_file, output_file ):

    # Set up a logger object:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "makePdf" )

    # Construct and run the command:
    import os
    command = "pprof --pdf --nodecount=200 --nodefraction=0.001 " \
              "--edgefraction=0.0002 `which python` " + input_file + " > " + output_file
    logger.info( "Running command: " + command )
    return os.system( command )

##
# @short Function making a callgrind file from the profile file
#
# This function is used to make a callgrind file from the profile
# file produced by gathena.py.
#
# @param input_file  Profile file created in a profiled job
# @param output_file Name of the output PDF file to be created
# @returns The status code returned by the pprof command
def makeCallgrind( input_file, output_file ):

    # Set up a logger object:
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "makeCallgrind" )

    # Construct and run the command:
    import os
    command = "pprof --callgrind `which python` " + \
              input_file + " > " + output_file
    logger.info( "Running command: " + command )
    return os.system( command )

#
# Execute the main() function:
#
if __name__ == "__main__":
    import sys
    sys.exit( main() )
    pass
