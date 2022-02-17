#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# access the list of configured chains and expand the analysis chains 
# to be actual configued entries


# get the list of configured chains matching the regex 
# input parameter

import re


# get a chain configuration of the form "some_regex:key=Track:vtx=Vertex" etc
# parse to determine the trigger chain pattern, determine the chains
# matching the pattern, then recreate the full analysis string using all the
# matched chains - can take an individual analysis pattern, or a list of them

def getchains( analyses ):

    if isinstance( analyses, list ):
        a = analyses
    else:
        a = [ analyses ] 

    chains = []

    for analysis in a:
        if ":" in analysis:
        
            parts   = analysis.split( ":", 1 )    
            cchains = getconfiguredchains( parts[0] )

            for c in cchains:
                if parts[1] is not None: 
                    chain = c + ":" + parts[1]
                else:
                    chain = c

                chains.append( chain )

    return chains



# cached full chain list

_chains = None

def getconfiguredchains( regex ):

#   print ("getconfiguredchains: ", regex )

    global _chains

#   from datetime import datetime
  
    if _chains is None :
#       print( datetime.now(), "getting trigger configuration" )

        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from TrigConfigSvc.TriggerConfigAccess import getHLTMenuAccess

        _chains = getHLTMenuAccess( ConfigFlags )

#       print( datetime.now(), "configured chains: ", len(_chains) )

    chains = []    
    for c in _chains:
        # print( c )
        chain = re.findall( regex, c )
        for a in chain: 
            if a is not None and c == a :
                chains.append( a )

    return chains



