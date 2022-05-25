# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

class CalibPath(JobProperty):
    """ path to calibration files on cvmfs
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="TrigTauRec/00-11-02"

class MvaTESConfig(JobProperty):
    """ config file for MvaTESEvaluator
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue='OnlineMvaTES_BRT_v2.weights.root'

class JetRNNIDConfig(JobProperty):
    """ config files for TauJetRNNEvaluator jet ID
    """
    statusOn=True
    allowedTypes=[['string']]
    StoredValue=[ 'rnnid_config_0p_v3.json', 'rnnid_config_1p_v3.json', 'rnnid_config_mp_v3.json' ]

class JetRNNIDWPConfig(JobProperty):
    """ config files for TauWPDecorator for RNN jet ID
    """
    statusOn=True
    allowedTypes=[['string']]
    StoredValue=[ 'rnnid_flat_0p_v4.root', 'rnnid_flat_1p_v4.root', 'rnnid_flat_mp_v4.root' ]

class JetRNNIDConfigLLP(JobProperty):
    """ config files for TauJetRNNEvaluator jet ID for LLP chains
    """
    statusOn=True
    allowedTypes=[['string']]
    StoredValue=[ 'llpdev/net_experimental_llz_0p.json', 'llpdev/net_experimental_llz_1p.json', 'llpdev/net_experimental_llz_mp.json' ]

class JetRNNIDWPConfigLLP(JobProperty):
    """ config files for TauWPDecorator for RNN jet ID for LLP chains
    """
    statusOn=True
    allowedTypes=[['string']]
    StoredValue=[ 'llpdev/rnnid_flat_llp_llz0p_050621-v1.root', 'llpdev/rnnid_flat_llp_llz1p_050621-v1.root', 'llpdev/rnnid_flat_llp_llzmp_050621-v1.root' ]

class FTFTauCoreBDTConfig(JobProperty):
    """ BDT config file for TrigTauTrackRoiUpdater
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue='FTF_tauCore_BDT_v1.root'


# define a container for the TrigTauRec tool flags
class TrigTauRecFlags(JobPropertyContainer):
    """ tau trigger flags (to be replaced by component accumulator) """

# add the container to the top container 
jobproperties.add_Container(TrigTauRecFlags)

# register the flags
for j in [CalibPath, MvaTESConfig, JetRNNIDConfig, JetRNNIDWPConfig, JetRNNIDConfigLLP, JetRNNIDWPConfigLLP, FTFTauCoreBDTConfig]:
    jobproperties.TrigTauRecFlags.add_JobProperty(j)

TrigTauFlags = jobproperties.TrigTauRecFlags
