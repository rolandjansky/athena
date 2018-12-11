# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from IOVDbSvc.CondDB import conddb

# Inner Detector alignment
conddb.addFolder("INDET","/Indet/Align")
conddb.addFolder("TRT","/TRT/Align")

# Pixel module distortions
conddb.addFolder("INDET","/Indet/PixelDist")
