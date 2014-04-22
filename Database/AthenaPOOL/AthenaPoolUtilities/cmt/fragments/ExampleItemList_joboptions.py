# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

try:
    fullItemList += [ "${item}#*" ]
except:
    fullItemList = []
    fullItemList += [ "${item}#*" ]
    
