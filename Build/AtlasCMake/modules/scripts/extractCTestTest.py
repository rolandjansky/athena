#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: extractCTestTest.py 711151 2015-11-27 12:00:55Z krasznaa $
#
# This script is used by the nightly build CTest script to split a single
# Test.xml file into multiple ones that can be used to upload test information
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
                                    "test results of a single subproject/" \
                                    "package out of the full test log of a " \
                                    "project." )
    parser.add_option( "-i", "--input", dest = "input", action = "store",
                       type = "string",
                       help = "Name of the input Test.xml file" )
    parser.add_option( "-o", "--output", dest = "output", action = "store",
                       type = "string",
                       help = "Name of the output Test.xml file" )
    parser.add_option( "-l", "--label", dest = "label", action = "store",
                       type = "string", default = "",
                       help = "Label/subproject/package to extract" )
    parser.add_option( "-v", "--verbose", dest = "verbose",
                       action = "store_true", help = "Print verbose output" )
    ( options, unknown ) = parser.parse_args()

    # Check for errors:
    if len( unknown ):
        parser.error( "The following options were not recognised: %s" % \
                      str( unknown ) )
        return 1

    # Greet the user:
    if options.verbose:
        print( "CDash test log extractor" )
        print( "  input  = %s" % options.input )
        print( "  output = %s" % options.output )
        print( "  label  = %s" % options.label )
        pass

    # Parse the input XML file:
    inputXml = xml.dom.minidom.parse( options.input )
    topElement = inputXml.documentElement

    # Access the testing node:
    testingNode = topElement.getElementsByTagName( "Testing" )[ 0 ]

    # Add a Subproject node to it with the specified label:
    subPNode = inputXml.createElement( "Subproject" )
    topElement.insertBefore( subPNode, testingNode )
    subPNode.setAttribute( "name", options.label )

    # Add a Labels node to it with a single label, the one specified
    # on the command line.
    labelsNode = inputXml.createElement( "Labels" )
    topElement.insertBefore( labelsNode, testingNode )
    labelNode = inputXml.createElement( "Label" )
    labelsNode.appendChild( labelNode )
    label = inputXml.createTextNode( options.label )
    labelNode.appendChild( label )

    # Loop over the results, and remove the ones not associated to the
    # specified label. Or all results that don't have any label associated.
    # Depending on what the user asked for...
    tests = testingNode.getElementsByTagName( "Test" )
    if options.verbose:
        print( "Total number of tests: %i" % len( tests ) )
        pass
    for test in tests:
        labels = test.getElementsByTagName( "Labels" )
        if len( labels ) != 1:
            # We found an element in TestList...
            continue
        label = labels[ 0 ].getElementsByTagName( "Label" )
        if len( label ) != 1:
            testingNode.removeChild( test )
            continue
        if label[ 0 ].childNodes[ 0 ].data != options.label:
            testingNode.removeChild( test )
            continue
        pass

    # Tell the user what happened:
    if options.verbose:
        print( "Tests selected: %i" % \
               len( testingNode.getElementsByTagName( "Test" ) ) )
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
