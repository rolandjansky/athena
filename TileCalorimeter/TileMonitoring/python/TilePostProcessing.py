#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file TilePostProcessing.py
@brief Functions for post-processing of histograms from TileMonitoring for the Run III
'''

def getProfile2D_RMS(inputs, title, name):
    """ This function produces 2D histogram with RMS from Profile2D per Tile partition """

    assert len(inputs) == 1

    inputProfile = inputs[0][1][0]
    assert "TProfile2D" in str(inputProfile.ClassName())
    inputProfile.SetErrorOption("s")

    fullName = '{}_{}'.format(name, inputs[0][0]['partition'])
    if 'gain' in inputs[0][0]:
        fullName += '_' + inputs[0][0]['gain']

    fullTitle = str(inputProfile.GetTitle()).split(':')[0] + ': ' + title

    outputHistogram = inputProfile.ProjectionXY(fullName,"C=E")
    outputHistogram.SetTitle(fullTitle)
    inputProfile.GetXaxis().Copy(outputHistogram.GetXaxis())
    inputProfile.GetYaxis().Copy(outputHistogram.GetYaxis())

    return [outputHistogram]
