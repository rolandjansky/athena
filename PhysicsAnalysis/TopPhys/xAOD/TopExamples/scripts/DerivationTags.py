# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TopExamples

#Moving the definition of ptags here, so we only have to modify them in a
#single place from now on

#DefaultDataPtag        = 'p2411'
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
        print '  (%d) %s (%s sample%s)' % (i, s, nsamples, txt)

    print ''
    print 'Type the numbers you want to submit, separated by a comma e.g. 0,1,2,3'
    data = raw_input('Type it here -> : ')

    txt_list = data.strip().split(',')
    names = [keys[int(x)] for x in txt_list]

    derivation = 'DAOD_TOPQ1'
    print 'Which derivation would you like [%s]? ' % derivation
    input = raw_input('Type it here -> : ')
    if len(input) > 0:
        derivation = input

    ptag = DefaultMCSkimmedPtag

    print 'Which ptag would you like %s = skimmed, %s = not skimmed [%s] ' % (DefaultMCSkimmedPtag, DefaultMCUnskimmedPtag, ptag)
    input = raw_input('Type it here -> : ')
    if len(input) > 0:
        ptag = input

    print names, derivation, ptag
    return names, derivation, ptag
