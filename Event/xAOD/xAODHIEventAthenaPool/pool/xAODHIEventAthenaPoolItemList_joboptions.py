# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file xAODHIEventAthenaPoolItemList_joboptions.py
## @brief Adds items for generated converters to full item list of
##        POOL persistifiable objects
## @author RD Schaffer <R.D.Schaffer@cern.ch>
## $Id: ExampleItemList_joboptionsHdr.py,v 1.1 2005-08-17 09:31:19 schaffer Exp $

    

try:
    fullItemList += [ "xAOD::HIEventShapeAuxContainer#*" ]
except:
    fullItemList = []
    fullItemList += [ "xAOD::HIEventShapeAuxContainer#*" ]
    

try:
    fullItemList += [ "xAOD::HIEventShapeContainer#*" ]
except:
    fullItemList = []
    fullItemList += [ "xAOD::HIEventShapeContainer#*" ]
    

try:
    fullItemList += [ "xAOD::HIEventShape#*" ]
except:
    fullItemList = []
    fullItemList += [ "xAOD::HIEventShape#*" ]
    

