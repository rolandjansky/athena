# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import TopExamples

#Moving the definition of ptags here, so we only have to modify them in a
#single place from now on

DefaultMCSkimmedPtag   = 'p2411'
DefaultMCUnskimmedPtag = 'p2413'


#https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopDerivations#Production_Caches
def InteractiveSubmission():
    samples = TopExamples.grid.AvailableDatasets()

    keys = sorted(samples.keys())

    for i, s in enumerate(keys):
        nsamples = len(samples[s].datasets)
        txt = ''
        if nsamples != 1:
            txt = 's'
        print('  (%d) %s (%s sample%s)' % (i, s, nsamples, txt))

    print('')
    print('Type the numbers you want to submit, separated by a comma e.g. 0,1,2,3')
    data = input('Type it here -> : ')

    txt_list = data.strip().split(',')
    names = [keys[int(x)] for x in txt_list]

    derivation = 'DAOD_TOPQ1'
    print('Which derivation would you like [{0}]? '.format(derivation))
    input_data = input('Type it here -> : ')
    if len(input_data) > 0:
        derivation = input_data

    ptag = DefaultMCSkimmedPtag

    print('Which ptag would you like {0} = skimmed, {1} = not skimmed [{2}]'.format(
        DefaultMCSkimmedPtag, DefaultMCUnskimmedPtag, ptag))
    input_data = input('Type it here -> : ')
    if len(input_data) > 0:
        ptag = input_data

    print(names, derivation, ptag)
    return names, derivation, ptag
