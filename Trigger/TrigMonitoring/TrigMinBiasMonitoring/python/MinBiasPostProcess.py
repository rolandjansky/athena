# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def ratioOnlineOffline(inputs):
    matches, (online, offline) = inputs[0]

    ratio = online.Clone()
    ratio.Sumw2()
    ratio.Divide(offline)
    ratio.SetTitle(f'Track {matches["var"]} Online/Offline')
    ratio.GetXaxis().SetTitle(f'Track {matches["var"]}')
    ratio.GetYaxis().SetTitle('Online/Offline')

    return [ratio]
