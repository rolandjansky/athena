#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: extractCTestBuild.py 711151 2015-11-27 12:00:55Z krasznaa $
#
# This script is used by the nightly build CTest script to split a single
# Build.xml file into multiple ones that can be used to upload build information
# about the individual sub-projects (packages).
#

# Python import(s):
import optparse
import xml.dom.minidom

## C/C++ style main function
def main():

    # Read the command line options:
    parser = optparse.OptionParser( usage = "%prog [options]",
                                    description = "Script used to extract the " \
                                    "build results of a single subproject/" \
                                    "package out of the full build log of a " \
                                    "project." )
    parser.add_option( "-i", "--input", dest = "input", action = "store",
                       type = "string",
                       help = "Name of the input Build.xml file" )
    parser.add_option( "-o", "--output", dest = "output", action = "store",
                       type = "string",
                       help = "Name of the output Build.xml file" )
    parser.add_option( "-l", "--label", dest = "label", action = "store",
                       type = "string", default = "",
                       help = "Label/subproject/package to extract" )
    parser.add_option( "-n", "--no-label", dest = "no_label",
                       action = "store_true",
                       help = "Collect the warnings/errors with no labels" )
    parser.add_option( "-v", "--verbose", dest = "verbose",
                       action = "store_true", help = "Print verbose output" )
    ( options, unknown ) = parser.parse_args()

    # Check for errors:
    if len( unknown ):
        parser.error( "The following options were not recognised: %s" % \
                      str( unknown ) )
        return 1
    if options.label != "" and options.no_label:
        parser.error( "Can't use --label and --no-label at the same time" )
        return 1

    # Greet the user:
    if options.verbose:
        print( "CDash build log extractor" )
        print( "  input  = %s" % options.input )
        print( "  output = %s" % options.output )
        if options.no_label:
            print( "  no_label" )
        else:
            print( "  label  = %s" % options.label )
            pass
        pass

    # Parse the input XML file:
    inputXml = xml.dom.minidom.parse( options.input )
    topElement = inputXml.documentElement

    # Access the build node:
    buildNode = topElement.getElementsByTagName( "Build" )[ 0 ]

    # Add a subproject label to the file, if we're in that mode:
    if not options.no_label:

        # Add a Subproject node to it with the specified label:
        subPNode = inputXml.createElement( "Subproject" )
        topElement.insertBefore( subPNode, buildNode )
        subPNode.setAttribute( "name", options.label )

        # Add a Labels node to it with a single label, the one specified
        # on the command line.
        labelsNode = inputXml.createElement( "Labels" )
        topElement.insertBefore( labelsNode, buildNode )
        labelNode = inputXml.createElement( "Label" )
        labelsNode.appendChild( labelNode )
        label = inputXml.createTextNode( options.label )
        labelNode.appendChild( label )
        pass

    # Loop over the failures, and remove the ones not associated to the
    # specified label. Or all failures that have any label associated.
    # Depending on what the user asked for...
    failures = buildNode.getElementsByTagName( "Failure" )
    if options.verbose:
        print( "Total number of failures: %i" % len( failures ) )
        pass
    for failure in failures:
        labels = failure.getElementsByTagName( "Labels" )
        if options.no_label:
            if len( labels ) == 1:
                buildNode.removeChild( failure )
                continue
        else:
            if len( labels ) != 1:
                buildNode.removeChild( failure )
                continue
            label = labels[ 0 ].getElementsByTagName( "Label" )
            if len( label ) != 1:
                buildNode.removeChild( failure )
                continue
            if label[ 0 ].childNodes[ 0 ].data != options.label:
                buildNode.removeChild( failure )
                continue
            pass
        pass

    # Tell the user what happened:
    if options.verbose:
        print( "Failures selected: %i" % \
               len( buildNode.getElementsByTagName( "Failure" ) ) )
        pass

    # Write out the modified XML into the specified output file:
    outputXml = open( options.output, "w" )
    outputXml.write( inputXml.toxml( encoding = "UTF-8" ) )
    outputXml.close()

    # Return gracefully:
    return 0

# Execute the main function:
if __name__ == "__main__":
    import sys
    sys.exit( main() )
