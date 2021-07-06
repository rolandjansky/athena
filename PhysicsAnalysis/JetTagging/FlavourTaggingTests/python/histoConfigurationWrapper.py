# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def getHistogram1D( myMap,chains=[""] ):
    output = []

    for el in myMap:
        print( 'Looking at',el )
        definition = myMap[el]
        if definition[ 'path' ].get('ART',None) is None:
            continue

        probabilities = [""]
        if 'forEach' in definition:
            if len( definition[ 'forEach' ] ) > 0:
                probabilities = definition[ 'forEach' ]
        print( 'forEach:',probabilities )
        flavours = [""]
        if 'forEachTruthType' in definition:
            if len( definition[ 'forEachTruthType' ] ) > 0:
                flavours = definition[ 'forEachTruthType' ]
        print( 'FLAVS:',flavours )
        output += createHistogram1D( el,definition,probabilities,flavours,chains )

    return output

def getHistogram2D( myMap,chains=[""] ):
    output = []

    for el in myMap:
        definition = myMap[el]
        if definition[ 'path' ].get('ART',None) is None:
            continue

        probabilities = [""]
        if 'forEach' in definition:
            if len( definition[ 'forEach' ] ) > 0:
                probabilities = definition[ 'forEach' ]
        print( 'forEach:',probabilities )
        flavours = [""]
        if 'forEachTruthType' in definition:
            if len( definition[ 'forEachTruthType' ] ) > 0:
                flavours = definition[ 'forEachTruthType' ]
        output += createHistogram2D( el,definition,probabilities,flavours,chains )

    return output

def createHistogram1D( el,definition,probabilities,flavours,chains=[""] ):
    output = []

    for chain in chains:
        for prob in probabilities:
            for flav in flavours:
                name = str(el)
                histoPath = str(definition['path']['ART'])
    
                if  str(prob) != "":
                    name += "_" + str(prob)

                if  str(flav) != "":
                    name = str(flav) + "_" + name
                    histoPath = str(flav)+"/"+str(definition['path']['ART'])
    
                if  str(chain) != "":
                    name = str(chain) + "_" + name
                    histoPath = str(chain) +"/"+ histoPath
    
                toAdd = [ str(definition['type']),
                          name,
                          str(definition['title']+";"+definition['xtitle']+";"+definition['ytitle']),
                          histoPath,
                          str(definition['xbins']),
                          str(definition['xmin']),
                          str(definition['xmax']) ]
                output.append( toAdd )

    return output 

def createHistogram2D( el,definition,probabilities,flavours,chains=[""] ):
    output = []

    for chain in chains:
        for prob in probabilities:
            for flav in flavours:
                name = str(el)
                histoPath = str(definition['path']['ART'])

                if  str(prob) != "":
                    name += "_" + str(prob)
    
                if  str(flav) != "":
                    name = str(flav) + "_" + name
                    histoPath = str(flav)+"/"+str(definition['path']['ART'])
    
                if  str(chain) != "":
                    name = str(chain) + "_" + name
                    histoPath = str(chain) +"/"+ histoPath
    
                toAdd = [ str(definition['type']),
                          name,
                          str(definition['title']+";"+definition['xtitle']+";"+definition['ytitle']),
                          histoPath,
                          str(definition['xbins']),
                          str(definition['xmin']),
                          str(definition['xmax']),
                          str(definition['ybins']),
                          str(definition['ymin']),
                          str(definition['ymax']) ]
                output.append( toAdd )

    return output 

###################################################

def getHistoDefs( category = 'ART',chains=[""] ):
    import json
    import os
    from AthenaCommon.Utils.unixtools import FindFile

    path = FindFile("JetTagDQA/PhysValBtag_VariablesMenu.json", os.environ["DATAPATH"].split(os.pathsep), os.R_OK)

    inFile = open( path,'r' )
    mappa = json.load( inFile )


    variables1D = []
    variables2D = []

    # Taking 1D histograms  
    variables1D += getHistogram1D( mappa['GLOBAL'],chains )
    variables1D += getHistogram1D( mappa['JET'],chains )
    variables1D += getHistogram1D( mappa['TAGGER'],chains )
    variables1D += getHistogram1D( mappa['TRACKS'],chains )
    variables1D += getHistogram1D( mappa['SV'],chains )
    # Taking 2D histograms  
    variables2D += getHistogram2D( mappa[ 'CombinedQuantities' ],chains )

    return [ variables1D,variables2D ]
