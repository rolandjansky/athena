# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Helper functions for the configuration of pileup reweighting
reco_campaigns = { 
 'mc16a' : ['r9280','r9287','r9364'],
 'mc16d' : ['r10069','r10201','r10211','r10212'],
 'mc16e' : ['r10724','r10726']
}

# Function to get the campaign
def getCampaign():
    # Attempt auto-configuration
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary is not None:
        ami_tag = inputFileSummary['tag_info']['AMITag']
        my_campaign = None
        for c in reco_campaigns:
            for r in reco_campaigns[c]:
                if r in ami_tag:
                    return c
        project = inputFileSummary['tag_info']['project_name']
        # MC-equivalent projects for data
        if 'data18' in project: return 'mc16e'
        elif 'data17' in project: return 'mc16d'
        elif 'data16' in project or 'data15' in project: return 'mc16a'
    return None

# Function to get the data lumicalc files
def getLumiCalcFiles():
    # Attempt auto-configuration
    my_campaign = getCampaign()
    if my_campaign=='mc16a':
        return ["GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root",
                "GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root"]
    elif my_campaign=='mc16d':
        return ["GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root" ]
    elif my_campaign=='mc16e':
        return ["GoodRunsLists/data18_13TeV/20190708/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root" ]
    return []

def getMCMuFiles():
    # Attempt auto-configuration
    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary is not None and 'IS_SIMULATION' in inputFileSummary['evt_type']:
        # We are in an MC file - get the AMI tag
        ami_tag = inputFileSummary['tag_info']['AMITag']
        my_campaign = getCampaign()
        mc_channel_number = str(inputFileSummary['mc_channel_number'][0])
        sim_flavor = inputFileSummary['metadata']['/Simulation/Parameters']['SimulationFlavour']
        sim_type = 'FS' if sim_flavor in ['FullG4'] else 'AFII'
        defaultDirectory = 'dev/PileupReweighting/share'
        inputFile = defaultDirectory+'/DSID'+mc_channel_number[:3]+'xxx/pileup_'+my_campaign+'_dsid'+mc_channel_number+'_'+sim_type+'.root'
        return [inputFile]
    return []
