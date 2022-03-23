#!/usr/bin/env python
#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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


def getTileModulePedestalsAndLFN(inputs):
    """ This function produces 1D summary histogram per Tile module with pedestals and LFN of all channels in the module """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    gain = inputs[0][0]['gain']
    gainName = {'hi' : 'high', 'lo' : 'low'}.get(gain)
    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    pedestalName = f'{module}_{gain}_ped'
    pedestalTitle = f'Run {run} {module} {gainName} gain: Pedestal[0]'
    pedestalHistogram = ROOT.TH1F(pedestalName, pedestalTitle, Tile.MAX_CHAN, -0.5, Tile.MAX_CHAN - 0.5)

    lfnName = f'{module}_{gain}_rms_lfr'
    lfnTitle = f'Run {run} {module} {gainName} gain: RMS noise low frequency'
    lfnHistogram = ROOT.TH1F(lfnName, lfnTitle, Tile.MAX_CHAN, -0.5, Tile.MAX_CHAN - 0.5)

    for input in inputs:

        inputHistogram = input[1][0]
        channel = int(input[0]['channel'])
        bin = int(channel) + 1

        rms = inputHistogram.GetRMS()
        pedestal = inputHistogram.GetMean()
        pedestalHistogram.SetBinContent(bin, pedestal)
        pedestalHistogram.SetBinError(bin, rms)

        lfnHistogram.SetBinContent(bin, rms)

    return [pedestalHistogram, lfnHistogram]


def getTileModuleHFN(inputs):
    """ This function produces 1D summary histogram per Tile module with HFN of all channels in the module """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    gain = inputs[0][0]['gain']
    gainName = {'hi' : 'high', 'lo' : 'low'}.get(gain)
    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    hfnName = f'{module}_{gain}_rms_hfr'
    hfnTitle = f'Run {run} {module} {gainName} gain: RMS noise high frequency'
    hfnHistogram = ROOT.TH1F(hfnName, hfnTitle, Tile.MAX_CHAN, -0.5, Tile.MAX_CHAN - 0.5)

    for input in inputs:

        inputHistogram = input[1][0]
        channel = int(input[0]['channel'])
        bin = int(channel) + 1

        hfn = inputHistogram.GetMean()
        hfnHistogram.SetBinContent(bin, hfn)

    return [hfnHistogram]


def getTileModuleCRC(inputs):
    """ This function produces 1D summary histogram per Tile module with CRC errors of all DMU in the module """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    crcName = f'{module}_crc'
    crcTitle = f'Run {run} {module}: CRC errors'
    crcHistogram = ROOT.TH1F(crcName, crcTitle, 16, -0.5, 15.5)

    for input in inputs:

        inputHistogram = input[1][0]
        dmu = int(input[0]['dmu'])
        bin = int(dmu) + 1

        underflow = inputHistogram.GetBinContent(0)
        crcGlobal = inputHistogram.GetBinContent(1)
        crcOk = inputHistogram.GetBinContent(2)
        crcFE = inputHistogram.GetBinContent(3)
        crcROD = inputHistogram.GetBinContent(4)
        crcFE_ROD = inputHistogram.GetBinContent(5)

        weight = -1
        if (underflow + crcFE + crcROD + crcFE_ROD) > 0:
            if (crcROD == 0) and (crcFE_ROD == 0):
                weight = 2 # FE CRC only errors
            elif (crcFE == 0) and (crcFE_ROD == 0):
                weight = 3 # ROD CRC only errors
            else:
                # If there are ROD and FE errors (not necessary at the same time), weight is 2.5
                weight = 2.5 #ROD and FE CRC errors
        else:
            if crcOk == 0:
                weight = 0 # all zeros
            elif crcGlobal == 0:
                weight = 1 # all OK
            else:
                # FIXME: weight = 1.0 - crcGlobal / nEvents # percentage of good events
                weight = 1
                if weight > 0.8:
                     weight = 0.8 # to see clearly even one event with zeros

        crcHistogram.SetBinContent(bin, weight)

    return [crcHistogram]



def getTileModuleBCID(inputs):
    """ This function produces 1D summary histogram per Tile module with BCID errors of all DMU in the module """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    gain = inputs[0][0]['gain']
    gainName = {'hi' : 'high', 'lo' : 'low'}.get(gain)
    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    bcidName = f'{module}_{gain}_bcid'
    bcidTitle = f'Run {run} {module} {gainName} gain: BCID errors'
    bcidHistogram = ROOT.TH1F(bcidName, bcidTitle, 16, -0.5, 15.5)

    for input in inputs:

        inputHistogram = input[1][0]
        dmu = int(input[0]['dmu'])
        bin = int(dmu) + 1

        underflow = inputHistogram.GetBinContent(0)
        bcidZero = inputHistogram.GetBinContent(1)
        bcidOk = inputHistogram.GetBinContent(2)
        bcidOther = inputHistogram.GetBinContent(3)

        weight = -1
        if (underflow + bcidOther) > 0:
            weight = 2 # all other errors in BCID
        else:
            if bcidOk == 0:
                weight = 0 # all zeros
            elif bcidZero == 0:
                weight = 1 # all OK
            else:
                # FIXME: weight = 1.0 - bcidZero / nEvents # percentage of good events
                weight = 1
                if weight > 0.8:
                     weight = 0.8 # to see clearly even one event with zeros

        bcidHistogram.SetBinContent(bin, weight)

    return [bcidHistogram]



def getTileStuckBitsNumber2(inputHistogram):
    """
    This function analyzes a histogram with samples from Tile channel and returns number of stuck bits

    New function implemented by Tibor Zenis, which checks how many bit flips happen compared to the expected ones
    It works for CIS and Laser because the adc range is not sufficiently excersized in the other run types.
    """

    import math
    nBins = inputHistogram.GetNbinsX()
    nBits = round(math.log2(nBins))

    bitValue = [1 << bit for bit in range(0, nBits)]
    nBitFlipsAtSignalRegion = [0 for bit in range(0, nBits)]
    nBitFlipsAtSignalEdges = [0 for bit in range(0, nBits)]

    nextBinContent = inputHistogram.GetBinContent(1)
    previousBinIsNotEmpty = nextBinContent > 0 # flip-flop of non-zero contents of previous bin

    currentBinContent = 2
    for currentBin in range(1, nBins):

        previousBinContent = currentBinContent
        currentBinContent = nextBinContent
        binIsNotEmpty = currentBinContent > 0
        nextBinContent = inputHistogram.GetBinContent(currentBin + 2) if currentBin < nBins - 1 else  0

        probability = 1.
        expectedContent = 0.125 * (nextBinContent + previousBinContent - math.sqrt(nextBinContent) - math.sqrt(previousBinContent))
        if binIsNotEmpty and currentBinContent < expectedContent:
            probability = 1. - currentBinContent / expectedContent
            binIsNotEmpty = False

        if binIsNotEmpty or previousBinIsNotEmpty: # count bit flips in bins with non-zero contents and adjacent bins
            for bit in range(0, nBits):
                if ((currentBin & bitValue[bit]) != ((currentBin - 1) & bitValue[bit])):
                    nBitFlipsAtSignalRegion[bit] += 1
                else:
                    break # higher bit can flip only if the previous one does

        if (binIsNotEmpty and previousBinIsNotEmpty) or not (binIsNotEmpty or previousBinIsNotEmpty):
            continue

        # bin content flips (zero - non-zero)
        for bit in range(0, nBits): # count bits flipped at this position
            if ((currentBin & bitValue[bit]) != ((currentBin - 1) & bitValue[bit])):
                if (currentBin & bitValue[bit]):
                    nBitFlipsAtSignalEdges[bit] += probability if binIsNotEmpty else -probability
                else:
                    nBitFlipsAtSignalEdges[bit] += -probability if binIsNotEmpty else probability
            else:
                break

        previousBinIsNotEmpty = not previousBinIsNotEmpty

    nStuckBits = 0
    for bit in range(0, nBits):
        if (((nBitFlipsAtSignalRegion[bit] > 2) and abs(nBitFlipsAtSignalEdges[bit]) == nBitFlipsAtSignalRegion[bit])
            or (abs(nBitFlipsAtSignalEdges[bit]) > 7 and abs(nBitFlipsAtSignalEdges[bit]) * 3 > nBitFlipsAtSignalRegion[bit])):
            nStuckBits += 1

    return nStuckBits



def getTileStuckBitsNumber(inputHistogram):
    """ This function analyzes a histogram with samples from Tile channel and returns number of stuck bits """

    import math
    nStuckBits = 0

    # If all bins at given hypothetical stuck value (0,1) and stuck bit (0,...,9,...)
    # are empty then this bin is stuck in this value. Do this procedure only for
    # bins between the first and the last non-zeroth one.

    # Find first/last non-zero bin in the histogram
    firstBin = inputHistogram.FindFirstBinAbove(0)
    if firstBin < 0:  # empty histogram, nothing to do
        return 0

    lastBin = inputHistogram.FindLastBinAbove(0)

    # bins in histogram are counted from 1 to max(ADC) + 1, but actual X value for those bins are 0 ... max(ADC)

    # if there is nothing in first half of histogram
    # or there is nothing in second half and there is a sharp edge at mid
    # it can be that upper most bit is stuck in 0 or 1

    nBins = inputHistogram.GetNbinsX() # max(ADC) + 1

    if (firstBin == nBins / 2 + 1) or (lastBin == nBins / 2 and inputHistogram.GetBinContent(lastBin) > 3):
        nStuckBits += 1 # "The last Bit is stuck"

    firstBin -= 1 # shift to zero bin
    lastBin += 1 # shift to zero bin

    nBits = round(math.log2(nBins))
    for stuckValue in (0, 1): # bit stuck at value 0 or 1
        for stuckBit in range(0, nBits): # which bit is stuck
            sumInBinsChecked = 0
            nBinsChecked = 0

            windowLength = (1 << stuckBit) # length of one interval with all zeros if bit is stuck
            beginBin = (1 - stuckValue) * windowLength # beginning of first interval to check

            while beginBin + 1 < lastBin:
                endBin = beginBin + windowLength # end of interval to check

                if endBin > firstBin:
                    for bin in range(beginBin, endBin):
                        # bin is from 0 to max(ADC) here - must be changed in GetBinConent to (bin+1)
                        if (firstBin < bin + 1) and (bin + 1 < lastBin):
                            sumInBinsChecked += inputHistogram.GetBinContent(bin + 1)
                            nBinsChecked += 1

                beginBin = endBin + windowLength # beginning of next interval

    if sumInBinsChecked == 0 and nBinsChecked > 0:
        nStuckBits += 1

    if (nStuckBits == 0):
        nStuckBits = getTileStuckBitsNumber2(inputHistogram)

    return nStuckBits


def getTileModuleStuckBits(inputs):
    """ Produce Tile 1D summary histograms per module with stuck bits and zeros for all channels in the module """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    gain = inputs[0][0]['gain']
    gainName = {'hi' : 'high', 'lo' : 'low'}.get(gain)
    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    bitsName = f'{module}_{gain}_bits'
    bitsTitle = f'Run {run} {module} {gainName} gain: Stuck bits and zero amplitudes'
    bitsHistogram = ROOT.TH1F(bitsName, bitsTitle, Tile.MAX_CHAN, -0.5, Tile.MAX_CHAN - 0.5)

    for input in inputs:

        inputHistogram = input[1][0]
        channel = int(input[0]['channel'])
        bin = int(channel) + 1

        nZeros = inputHistogram.GetBinContent(1)
        integral = inputHistogram.Integral(2, inputHistogram.GetNbinsX()) # first bin is excluded

        weight = 0
        if integral > 0: # if we have weight=0, then there is no non-zero amplitudes
            if nZeros > 0:
                weight = 0.5 # sometimes zeros
            else:
                weight = 1.0 # assume that all OK

        nStuckBits = getTileStuckBitsNumber(inputHistogram)
        if nStuckBits > 0:
            weight += 1.0 # some stuck bits

        bitsHistogram.SetBinContent(bin, weight)

    return [bitsHistogram]


def getTileModuleCovariance(inputs):
    """ Produce Tile 2D histograms per module with covariance and correlation of channels in the module. """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    import math

    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    gain = inputs[0][0]['gain']
    gainName = {'hi' : 'high', 'lo' : 'low'}.get(gain)
    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    covarianceName = f'{module}_{gain}_covar'
    covarianceTitle = f'Run {run} {module} {gainName} gain: Covariance'
    covarianceHistogram = ROOT.TH2F(covarianceName, covarianceTitle,
                                    Tile.MAX_CHAN, -0.5, Tile.MAX_CHAN - 0.5,
                                    Tile.MAX_CHAN, -0.5, Tile.MAX_CHAN - 0.5)

    correlationName = f'{module}_{gain}_corr'
    correlationTitle = f'Run {run} {module} {gainName} gain: Correlation'
    correlationHistogram = ROOT.TH2F(correlationName, correlationTitle,
                                     Tile.MAX_CHAN, -0.5, Tile.MAX_CHAN - 0.5,
                                     Tile.MAX_CHAN, -0.5, Tile.MAX_CHAN - 0.5)

    meanSampleHistogram = inputs[0][1][0]
    meanSample12Histogram = inputs[0][1][1]

    meanRMS = [0 for i in range(0,48)]
    covariance = [[0 for i in range(0,48)] for j in range(0,48)]

    for channel1 in range(0, Tile.MAX_CHAN):
        meanSample1 = meanSampleHistogram.GetBinContent(channel1 + 1)
        for channel2 in range(0, Tile.MAX_CHAN):
            meanSample2 = meanSampleHistogram.GetBinContent(channel2 + 1)
            meanSample12 = meanSample12Histogram.GetBinContent(channel1 + 1, channel2 + 1)
            covariance12 = meanSample12 - meanSample1*meanSample2
            covariance[channel1][channel2] = covariance12

        covariance11 = covariance[channel1][channel1]
        meanRMS[channel1] = 0 if covariance11 == 0 else covariance11 / math.sqrt(abs(covariance11))

    for channel1 in range(0, Tile.MAX_CHAN):
        for channel2 in range(0, Tile.MAX_CHAN):
            meanRMS1 = meanRMS[channel1]
            meanRMS2 = meanRMS[channel2]
            if (meanRMS1 == 0) or (meanRMS2 == 0):
                correlation = 0
            else:
                correlation = covariance[channel1][channel2] / (meanRMS1 * meanRMS2)
            correlationHistogram.SetBinContent(channel1 + 1, channel2 + 1, correlation)
            covarianceHistogram.SetBinContent(channel1 + 1, channel2 + 1, covariance[channel1][channel2])

    return [covarianceHistogram, correlationHistogram]



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
                                 '--prefix', '/', '-c'] + physConfigurations)

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
                                 '--prefix', '/', '-c'] + noiseConfigurations)

            subprocess.run(noisePostProcess)
