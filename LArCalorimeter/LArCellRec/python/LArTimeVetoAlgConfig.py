# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory 
from IOVDbSvc.IOVDbSvcConfig import addFolders 

def LArTimeVetoAlgCfg(flags):
    
    result=ComponentAccumulator()
    
    result.merge(addFolders(flags,"/LAR/BadChannelsOfl/EventVeto","LAR_OFL",className="AthenaAttributeList"))
    
    result.addEventAlgo(CompFactory.LArTimeVetoAlg())

    return result

