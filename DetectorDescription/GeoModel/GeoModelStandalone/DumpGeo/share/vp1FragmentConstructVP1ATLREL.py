#/*
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#*/

#Construct VP1ATLREL integer based on AtlasVersion environment
#variable, needed to make script work across releases. As an example
#14.0.1 becomes 140001. Any parts not recognised are replaced with 99.
if not 'VP1ATLREL' in dir():
    import os
    versionMajor=99; versionMinor=99; versionPatch=99
    if os.getenv("AtlasVersion") == None:
        VP1ATLREL=999999
    else:
        atlVerParts=os.getenv("AtlasVersion").split('.')
        if len(atlVerParts)>=1 and atlVerParts[0].isdigit():
            versionMajor=eval(atlVerParts[0])
            if len(atlVerParts)>=2 and atlVerParts[1].isdigit():
                versionMinor=eval(atlVerParts[1])
                if len(atlVerParts)>=3 and atlVerParts[2].isdigit():
                    versionPatch=eval(atlVerParts[2])
        if versionMinor>99: versionMinor=99
        if versionMinor==99 or versionPatch>99: versionPatch=99
        VP1ATLREL=10000*versionMajor+100*versionMinor+versionPatch
        print ("VP1ATLREL: "+str(VP1ATLREL))
