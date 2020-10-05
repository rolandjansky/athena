# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Helper functions for the configuration of pileup reweighting
reco_campaigns = { 
 'mc16a' : ['r9280','r9287','r9364'],
 'mc16d' : ['r10069','r10201','r10211','r10212'],
 'mc16e' : ['r10724','r10726']
}

# Function to get the campaign
def getCampaign(ami_tag = None, project = None):
    # Attempt auto-configuration
    if ami_tag is None or project is None:
        # either is not set, get unset from InputFilePeeker
        from RecExConfig.InputFilePeeker import inputFileSummary
        if inputFileSummary is not None:
            ami_tag = inputFileSummary['tag_info']['AMITag'] if ami_tag is None else ami_tag
            project = inputFileSummary['tag_info']['project_name'] if project is None else project

    assert ami_tag is not None
    assert project is not None

    for c in reco_campaigns:
        for r in reco_campaigns[c]:
            if r in ami_tag:
                return c

    # MC-equivalent projects for data
    if 'data18' in project: return 'mc16e'
    elif 'data17' in project: return 'mc16d'
    elif 'data16' in project or 'data15' in project: return 'mc16a'

    return None

# Function to get the data lumicalc files
def getLumiCalcFiles(campaign = None):
    # Attempt auto-configuration
    campaign = campaign or getCampaign()
    if campaign=='mc16a':
        return ["GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root",
                "GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root"]
    elif campaign=='mc16d':
        return ["GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root" ]
    elif campaign=='mc16e':
        return ["GoodRunsLists/data18_13TeV/20190708/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root" ]
    return []

def getMCMuFiles(data_type = None, campaign = None, dsid = None):
    # Attempt auto-configuration
    defaultDirectory = 'dev/PileupReweighting/share'

    if data_type is None or campaign is None or dsid is None:
        # one is not set, get unset from InputFilePeeker
        from RecExConfig.InputFilePeeker import inputFileSummary
        if inputFileSummary is not None and 'IS_SIMULATION' in inputFileSummary['evt_type']:
            # We are in an MC file - get the AMI tag
            ami_tag = inputFileSummary['tag_info']['AMITag']
            campaign = getCampaign(ami_tag=ami_tag)
            dsid = str(inputFileSummary['mc_channel_number'][0]) if dsid is None else dsid
            sim_flavor = inputFileSummary['metadata']['/Simulation/Parameters']['SimulationFlavour']
            sim_type = 'FS' if sim_flavor in ['FullG4'] else 'AFII'
            inputFile = defaultDirectory+'/DSID'+dsid[:3]+'xxx/pileup_'+campaign+'_dsid'+dsid+'_'+sim_type+'.root'
            return [inputFile]
    else:
        # everything set explicitly
        # data_type as in pileup analysis sequence: either 'data' or ('mc' or 'afii')
        if data_type == "data":
            return []
        if data_type == "mc":
            sim_type = "FS"
        elif data_type == "afii":
            sim_type = "AFII"
        else:
            raise ValueError("Invalid data_type %s" % data_type)

        inputFile = defaultDirectory+'/DSID'+dsid[:3]+'xxx/pileup_'+campaign+'_dsid'+dsid+'_'+sim_type+'.root'
        return [inputFile]
    return []
