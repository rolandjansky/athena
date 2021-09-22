#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file TilePostProcessing.py
@brief Functions for post-processing of histograms from TileMonitoring for the Run III
'''

import os, sys, signal, subprocess
from AthenaCommon.Utils.unixtools import find_datafile

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


def getTilePartitionRawChannelNoise(inputs):
    """ This function produces 2D histograms with Tile raw channel noise per partition """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    from TileMonitoring.TileMonitoringCfgHelper import getLabels
    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    def _getTilePartitionHistogram2D(name, title, partition, run):
        fullName = 'TileRawChannelNoise_{}_{}'.format(name, partition)
        fullTitle = 'Run {} Partition {}: {}'.format(run, partition, title)
        histogram = ROOT.TH2F(fullName, fullTitle,
                              Tile.MAX_DRAWER, 0.5, Tile.MAX_DRAWER + 0.5,
                              Tile.MAX_CHAN, -0.5, Tile.MAX_CHAN - 0.5)
        moduleLables = getLabels(('modules'), partition)
        channelLables = getLabels(('channels'), partition)
        for axis,labels in ((histogram.GetXaxis(), moduleLables), (histogram.GetYaxis(), channelLables)):
            for bin in range(0, len(labels)):
                axis.SetBinLabel(bin + 1, labels[bin])
        return histogram


    partition = inputs[0][0]['module'][:3]
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    rmsHistogram = _getTilePartitionHistogram2D('RMS', 'RMS of gaussians', partition, run)
    sigmaHistogram = _getTilePartitionHistogram2D('Sigma', 'Sigma of gaussians', partition, run)
    chi2Histogram = _getTilePartitionHistogram2D('Chi2', 'Chi2 of gaussians', partition, run)
    probabilityHistogram = _getTilePartitionHistogram2D('Prob', 'Probability of gaussians', partition, run)
    rmsOverSigmaHistogram = _getTilePartitionHistogram2D('RmsOverSigma', 'RMS/Sigma of gaussians', partition, run)

    fitfunction = ROOT.TF1("total", "gaus(0)")
    fitfunction.SetLineColor(2)

    for input in inputs:

        module = int(input[0]['module'][3:])
        channel = int(input[0]['channel'])

        plot = input[1][0]

        nEntries = plot.GetEntries()

        if nEntries > 100:

            rms = plot.GetRMS()
            rmsHistogram.Fill(module, channel, rms)

            fitfunction.SetParameters(0.1 * nEntries, 0., 0.7 * rms)

            binWidth = plot.GetBinWidth(0)
            lowLimit = binWidth * 0.5
            highLimit = max(rms * 1.05, binWidth * 2.0)

            fitfunction.SetParLimits(0, 0., nEntries)
            fitfunction.FixParameter(1, 0.)
            fitfunction.SetParLimits(2, lowLimit, highLimit)

            plot.Fit(fitfunction, "BQ")

            sigma = fitfunction.GetParameter(2)
            chi2 = fitfunction.GetChisquare()
            prob = fitfunction.GetProb()

            sigmaHistogram.Fill(module, channel, sigma)
            chi2Histogram.Fill(module, channel, chi2)
            probabilityHistogram.Fill(module, channel, prob)
            rmsOverSigmaHistogram.Fill(module, channel, rms/sigma)

    return [rmsHistogram, sigmaHistogram, chi2Histogram, probabilityHistogram, rmsOverSigmaHistogram]


if __name__== '__main__':

    import argparse
    parser= argparse.ArgumentParser()

    parser.add_argument('--stateless', action="store_true", help='Run Online Tile Post Processing in partition')
    parser.add_argument('--tmdb', action="store_true", help='Run Tile TMDB Post Processing')
    parser.add_argument('--digi-noise', action="store_true", dest='digiNoise', help='Run Tile Digi Noise Post Processing')
    parser.add_argument('--raw-chan-noise', action="store_true", dest='rawChanNoise', help='Run Tile Digi Noise Post Processing')
    parser.add_argument('--interval', type=int, default=60, help='Interval (in seconds) to run Tile Monitoring Post Processing')
    parser.add_argument('--noise-interval', type=int, default=300, dest='noiseInterval',
                        help='Interval (in seconds) to run Tile Noise Monitoring Post Processing')

    args = parser.parse_args()

    if args.stateless:

        server = 'Histogramming'
        partition = os.getenv("TDAQ_PARTITION", "ATLAS")

        dataPath = find_datafile('TileMonitoring')

        def sigtermHandler(signal, frame):
            print('Terminating postporcessing ...')
            sys.exit(0)

        signal.signal(signal.SIGTERM, sigtermHandler)

        if args.tmdb:

            inputPath = f'{partition};{server};TileMIG:TileGatherer'
            outputPath = f'{partition};{server};TilePT-stateless-PP'

            physConfigurations = []
            if args.tmdb:
                physConfigurations += [os.path.join(dataPath, 'TileTMDBPostProc.yaml')]

            physPostProcess = (['histgrinder', inputPath, outputPath,
                                 '--inmodule', 'DQOnlinePostprocessing.atlas_oh.OHInputModule',
                                 '--outmodule', 'DQOnlinePostprocessing.atlas_oh.OHOutputModule',
                                 '-c'] + physConfigurations)

            subprocess.run(physPostProcess)

        elif any([args.digiNoise, args.rawChanNoise]):

            inputPath = f'{partition};{server};TilePT-stateless-noise-01'
            outputPath = f'{partition};{server};TilePT-stateless-noise-PP'

            noiseConfigurations = []
            if args.digiNoise:
                noiseConfigurations += [os.path.join(dataPath, 'TileDigiNoisePostProc.yaml')]
            if args.rawChanNoise:
                noiseConfigurations += [os.path.join(dataPath, 'TileRawChanNoisePostProc.yaml')]

            noisePostProcess = (['histgrinder', inputPath, outputPath,
                                 '--inmodule', 'DQOnlinePostprocessing.atlas_oh.OHInputModule',
                                 '--outmodule', 'DQOnlinePostprocessing.atlas_oh.OHOutputModule',
                                 '-c'] + noiseConfigurations)

            subprocess.run(noisePostProcess)
