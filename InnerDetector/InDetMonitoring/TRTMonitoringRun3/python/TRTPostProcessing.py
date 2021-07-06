#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
def occupancyS(inputs):
    import ROOT
    rh = ROOT.TH1F('hOccupancyS', 'Straw Occupancy Distribution', 201, 0, 1.005)
    plots = [_[1][0] for _ in inputs]  # all plots passed as first element of list
    for plot in plots:
        for j in range(201):
            rh.Fill(plot.GetEfficiency(j+1))
    return [rh]
    
def occupancyC(inputs):
    import ROOT
    rh = ROOT.TH1F('hOccupancyC', 'Chip Occupancy Distribution', 201, 0, 1.005)
    plots = [_[1][0] for _ in inputs]  # all plots passed as first element of list
    for plot in plots:
        for j in range(201):
            rh.Fill(plot.GetEfficiency(j+1))
    return [rh]