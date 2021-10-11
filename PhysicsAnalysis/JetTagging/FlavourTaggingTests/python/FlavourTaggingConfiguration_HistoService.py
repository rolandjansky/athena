# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 

def getHistoServiceCfg( outFileName ):

    if outFileName == '':
        return None

    options = {}
    options['Output'] = ["MYSTREAM DATAFILE='"+ outFileName +"' OPT='RECREATE'"]

    from AthenaConfiguration.ComponentFactory import CompFactory
    FTAGValidation__HistService = CompFactory.THistSvc( **options )

    return FTAGValidation__HistService

