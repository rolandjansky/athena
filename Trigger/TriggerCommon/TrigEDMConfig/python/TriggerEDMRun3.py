# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


# definition of trigger EDM for the Run 3, mostly similar to Run 2
# however there are different set of EDM to stream and different functionalities required
from AthenaCommon.Logging import logging
__log = logging.getLogger('TriggerEDMRun3Config')
def recordable( name, onlyWarning=False ):
    """
    Verify that the name is in the list of recorded objects
    
    In Run 2 it was a delicate process to configure correctly what get recorded 
    as it had to be set in the produce arlgoirhm as well in here in a consistent manner.
    
    For Run 3 every alg input/output key can be crosschecked agains the list of objects to record defined here.
    I.e. in the configuration alg developer would do this:
    from TriggerEDM.TriggerEDMRun3 import recordable
    
    alg.outputKey = recordable("SomeKey")
    If the names are correct the outputKey is assigned with SomeKey, if there is a missmatch an exception is thrown.
    """


    for entry in TriggerHLTList:
        if entry[0].split( "#" )[1] == name:
            return name
    msg = "The collection name {0} is not declared to be stored by HLT".format( name )
    __log.error( msg )
    raise RuntimeError( msg )


TriggerHLTList = [
 
    #framework/steering
    ('TrigRoiDescriptorCollection#L1EM',                             'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#L1MU',                             'BS ESD AODFULL AODSLIM',  'Steer'),
    
    ('xAOD::TrigCompositeContainer#HLT_TrigEFDielectronMassFex',     'BS ESD AODFULL AODSLIM',  'Steer'),
    
]

EDMDetails = {}

EDMDetails[ "TrigRoiDescriptor" ]         = {'persistent':"TrigRoiDescriptorCollection_p3",     'typealias':'Roi', 'collection':'TrigRoiDescriptorCollection' }



        
