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
    ('TrigRoiDescriptorCollection#EMRoIs',                             'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#MURoIs',                             'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#METRoI',                             'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#JETRoI',                             'BS ESD AODFULL AODSLIM',  'Steer'),
    

    
    ('xAOD::TrigCompositeContainer#L1DecoderSummary',     'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1EM',                 'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1MU',                 'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1MET',                'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1J',                  'BS ESD AODFULL AODSLIM', 'Steer'),


    
    
    ('xAOD::TrigEMClusterContainer#L2CaloEMClusters',           'BS ESD AODFULL', 'Egamma', 'inViews:FastCaloEMRecoViews'), # last arg specifies in which view container the fragments are
    ('xAOD::TrigEMClusterAuxContainer#L2CaloEMClustersAux.',    'BS ESD AODFULL', 'Egamma'),
    ('xAOD::TrigPhotonContainer#L2Photons',                     'BS ESD AODFULL', 'Egamma', 'inViews:L2PhotonRecoViews'),
    ('xAOD::TrigPhotonAuxContainer#L2PhotonsAux.',              'BS ESD AODFULL', 'Egamma'),
    
    ('EventInfo#ByteStreamEventInfo',              'ESD', 'Misc'),
    ('ROIB::RoIBResult#*',                         'ESD', 'Misc'),
]

EDMDetails = {}

EDMDetails[ "TrigRoiDescriptorCollection" ]     = {'persistent':"TrigRoiDescriptorCollection_p3"}

EDMDetails[ "xAOD::TrigCompositeAuxContainer" ] = {'persistent':"xAOD::TrigCompositeAuxContainer_v2"}

EDMDetails[ "xAOD::TrigEMClusterAuxContainer" ] = {'persistent':"xAOD::TrigEMClusterAuxContainer_v2"}



def persistent( transient ):
    """
    Persistent EDM claass, for xAOD it is the actual class version
    
    Uses list defined above. If absent assumes v1
    """
    if transient in EDMDetails:
        return EDMDetails[transient]['persistent']
    return transient+"_v1"


def tpMap():
    """
    List 
    """
    l = {}
    for tr in EDMDetails.keys():
        if "xAOD" in tr:
            continue
        l[tr] = persistent(tr)
    return l
        


        
