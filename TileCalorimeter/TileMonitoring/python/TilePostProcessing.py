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


def getTileModuleHistogram(inputs, name, title):
    """ This function returns 1D histogram for Tile module """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    gain = inputs[0][0]['gain']
    gainName = {'hi' : 'high', 'lo' : 'low'}.get(gain)
    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    fullName = f'{module}_{gain}_{name}'
    fullTitle = f'Run {run} {module} {gainName} gain: {title}'
    histogram = ROOT.TH1F(fullName, fullTitle, Tile.MAX_CHAN, 0.0, Tile.MAX_CHAN)

    return histogram


def setTileModuleHistogramStyle(histogram, markerStyle=None, markerSize=None, markerColor=None,
                                labelSize=None, ymin=None, ymax=None, delta=0.1):
    """ This function set up stype of Tile module monitoring histogram """

    if ymin and (histogram.GetMinimum() > ymin + delta * abs(ymin)):
        histogram.SetMinimum(ymin)
    if ymax and (histogram.GetMaximum() < (1.0 - delta) * ymax):
        histogram.SetMaximum(ymax)
    if markerStyle:
        histogram.SetMarkerStyle(markerStyle)
    if markerColor:
        histogram.SetMarkerColor(markerColor)
    if markerSize:
        histogram.SetMarkerSize(markerSize)
    if labelSize:
        histogram.SetLabelSize(labelSize, "X")
        histogram.SetLabelSize(labelSize, "Y")


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

    nBinsX = inputProfile.GetNbinsX()
    nBinsY = inputProfile.GetNbinsY()

    outputHistogram = inputProfile.Clone()
    outputHistogram.Reset()
    outputHistogram.SetTitle(fullTitle)
    outputHistogram.SetName(fullName)
    for binX in range(0, nBinsX + 1):
         for binY in range(0, nBinsY + 1):
             rms = inputProfile.GetBinError(binX, binY)
             if rms != 0:
                 outputHistogram.SetBinContent(binX, binY, rms)
                 outputHistogram.SetBinEntries(inputProfile.GetBin(binX, binY), 1)

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

    pedestalHistogram = getTileModuleHistogram(inputs, 'ped', 'Pedestal[0]')
    lfnHistogram = getTileModuleHistogram(inputs, 'rms_lfr', 'RMS noise low frequency')

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

    hfnHistogram = getTileModuleHistogram(inputs, 'rms_hfr', 'RMS noise high frequency')

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
    crcHistogram = ROOT.TH1F(crcName, crcTitle, 16, 0.0, 16.0)

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
    bcidHistogram = ROOT.TH1F(bcidName, bcidTitle, 16, 0.0, 16.0)

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

    bitsHistogram = getTileModuleHistogram(inputs, 'bits', 'Stuck bits and zero amplitudes')

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
                                    Tile.MAX_CHAN, 0.0, Tile.MAX_CHAN,
                                    Tile.MAX_CHAN, 0.0, Tile.MAX_CHAN)

    correlationName = f'{module}_{gain}_corr'
    correlationTitle = f'Run {run} {module} {gainName} gain: Correlation'
    correlationHistogram = ROOT.TH2F(correlationName, correlationTitle,
                                     Tile.MAX_CHAN, 0.0, Tile.MAX_CHAN,
                                     Tile.MAX_CHAN, 0.0, Tile.MAX_CHAN)

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


def getTileModuleChi2VsAmplitude(inputs):
    """ This function produces 2D summary histogram per Tile module with Chi2 vs amplitude of all channels in the module """

    gain = inputs[0][0]['gain']
    gainName = {'hi' : 'high', 'lo' : 'low'}.get(gain)
    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    chi2AmpName = f'{module}_{gain}_summary_chi2_amp'
    chi2AmpTitle = f'Run {run} {module} {gainName} gain: Summary DSP #chi^{2} vs Amplitude'
    inputHistogram = inputs[0][1][0]
    chi2AmpHistogram = inputHistogram.Clone()
    chi2AmpHistogram.Reset()
    chi2AmpHistogram.SetTitle(chi2AmpTitle)
    chi2AmpHistogram.SetName(chi2AmpName)

    for input in inputs:
        inputHistogram = input[1][0]
        if inputHistogram.GetEntries() > 0:
            chi2AmpHistogram.Add(inputHistogram)

    setTileModuleHistogramStyle(chi2AmpHistogram, markerStyle=21, markerSize=0.75, labelSize=0.08, ymin=-0.05, ymax=10.0)

    return [chi2AmpHistogram]

def getTileModuleAmplitudeAndSigmaWithRMS(inputs):
    """ This function produces 1D summary histogram per Tile module with amplitude, sigma, and RMS of all channels in the module """

    import math
    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    gain = inputs[0][0]['gain']

    amplitudeHistogram = getTileModuleHistogram(inputs, 'amp', 'Average Amplitude and RMS')
    rmsHistogram = getTileModuleHistogram(inputs, 'rms', 'RMS of Amplitude')
    sigmaHistogram = getTileModuleHistogram(inputs, 'sigma', 'Sigma of Amplitude from Gaussian fit')

    fitFunction = ROOT.TF1("g", 'gaus')
    inputHistogram = inputs[0][1][0]
    overlapHistogram = inputHistogram.GetBinWidth(1) == inputHistogram.GetBinWidth(100)

    for input in inputs:

        inputHistogram = input[1][0]
        nEntries = inputHistogram.GetEntries()
        if nEntries > 0:
            channel = int(input[0]['channel'])
            bin = int(channel) + 1

            rms = inputHistogram.GetRMS()
            amplitude = inputHistogram.GetMean()
            amplitudeHistogram.SetBinContent(bin, amplitude)
            amplitudeHistogram.SetBinError(bin, rms / math.sqrt(nEntries))

            rmsHistogram.SetBinContent(bin, rms)
            rmsHistogram.SetBinError(bin, rms / math.sqrt(2.0 * nEntries))

            if nEntries > 90:
                if overlapHistogram:
                    inputHistogram.Scale(1, 'width')
                inputHistogram.Fit(fitFunction, 'Q')
                sigma = fitFunction.GetParameter(2)
                sigmaError = fitFunction.GetParError(2)
                sigmaHistogram.SetBinContent(bin, sigma)
                sigmaHistogram.SetBinError(bin, sigmaError)

    setTileModuleHistogramStyle(amplitudeHistogram, markerStyle=21, markerSize=0.75, labelSize=0.08,
                                ymin={'hi' : -0.05, 'lo' : -1.0}.get(gain),
                                ymax={'hi' : 0.05, 'lo' : 1.0}.get(gain))

    yMin = min(0.0, rmsHistogram.GetMinimum(), sigmaHistogram.GetMinimum())
    yMax = max({'hi' : 0.05, 'lo' : 1.0}.get(gain), rmsHistogram.GetMaximum(), sigmaHistogram.GetMaximum())

    setTileModuleHistogramStyle(rmsHistogram, markerStyle=21, markerSize=0.75, labelSize=0.08, ymin=yMin, ymax=yMax)
    setTileModuleHistogramStyle(sigmaHistogram, markerStyle=25, markerSize=0.75, labelSize=0.08,ymin=yMin, ymax=yMax)

    return [amplitudeHistogram, sigmaHistogram, rmsHistogram]


def fillTileModuleMeanAndRMS(histogram, inputs):
    """ This function fills 1D summary histogram per Tile module with mean value and RMS of all channels in the module """

    for input in inputs:

        inputHistogram = input[1][0]
        nEntries = inputHistogram.GetEntries()
        if nEntries > 0:
            channel = int(input[0]['channel'])
            bin = int(channel) + 1

            rms = inputHistogram.GetRMS()
            mean = inputHistogram.GetMean()
            histogram.SetBinContent(bin, mean)
            histogram.SetBinError(bin, rms)


def getTileModuleSumHistogram(inputs, name, title):
    """ This function produces 1D summary histogram per Tile module adding all histograms per channel in the module """

    gain = inputs[0][0]['gain']
    gainName = {'hi' : 'high', 'lo' : 'low'}.get(gain)
    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    fullName = f'{module}_{gain}_{name}'
    fullTitle = f'Run {run} {module} {gainName} gain: {title}'
    inputHistogram = inputs[0][1][0]
    sumHistogram = inputHistogram.Clone()
    sumHistogram.Reset()
    sumHistogram.SetTitle(fullTitle)
    sumHistogram.SetName(fullName)

    for input in inputs:
        inputHistogram = input[1][0]
        nEntries = inputHistogram.GetEntries()
        if nEntries > 0:
            sumHistogram.Add(inputHistogram)

    return sumHistogram


def getTileModuleTime(inputs):
    """ This function produces 1D summary histogram per Tile module with time of all channels in the module """

    timeHistogram = getTileModuleHistogram(inputs, 'time', 'Average Time and RMS')
    fillTileModuleMeanAndRMS(timeHistogram, inputs)
    setTileModuleHistogramStyle(timeHistogram, markerStyle=21, markerSize=0.75, ymin=-25.0, ymax=25.0, labelSize=0.08)

    return [timeHistogram]


def getTileModuleTimeCorrected(inputs):
    """ This function produces 1D summary histogram per Tile module with time corrected of all channels in the module """

    timeHistogram = getTileModuleHistogram(inputs, 'time_corr', 'Average Time corrected and RMS')
    fillTileModuleMeanAndRMS(timeHistogram, inputs)
    setTileModuleHistogramStyle(timeHistogram, markerStyle=21, markerSize=0.75, ymin=-25.0, ymax=25.0, labelSize=0.08)

    return [timeHistogram]


def getTileModuleAmplitudeDiffBetweenOfflineAndDSP(inputs):
    """ This function produces 1D summary histogram per Tile module with amplitude difference between offline and DSP of all channels in the module """

    ampDiffHistogram = getTileModuleHistogram(inputs, 'dspfit_ampdiff', '(DSP-OFFLINE)/OFFLINE Amplitude and RMS')
    fillTileModuleMeanAndRMS(ampDiffHistogram, inputs)
    setTileModuleHistogramStyle(ampDiffHistogram, markerStyle=21, markerSize=0.75, labelSize=0.08, ymin=-0.05, ymax=0.05)

    sumAmpDiffHistogram = getTileModuleSumHistogram(inputs, 'dspfit_amphbar', '(DSP-OFFLINE)/OFFLINE Amplitude for all channels')

    return [ampDiffHistogram, sumAmpDiffHistogram]


def getTileModuleAmplitudeDSP(inputs):
    """ This function produces 1D summary histogram per Tile module with DSP amplitudes of all channels in the module """

    amplitudeHistogram = getTileModuleHistogram(inputs, 'dspamp', 'Average DSP Amplitude and RMS')
    fillTileModuleMeanAndRMS(amplitudeHistogram, inputs)

    return [amplitudeHistogram]


def getTileModuleTimeDiffBetweenOfflineAndDSP(inputs):
    """ This function produces 1D summary histogram per Tile module with time difference between offline and DSP of all channels in the module """

    timeDiffHistogram = getTileModuleHistogram(inputs, 'dspfit_timediff', 'DSP-OFFLINE Time and RMS')
    fillTileModuleMeanAndRMS(timeDiffHistogram, inputs)
    setTileModuleHistogramStyle(timeDiffHistogram, markerStyle=21, markerSize=0.75, labelSize=0.08, ymin=-0.5, ymax=0.5)

    sumTimeDiffHistogram = getTileModuleSumHistogram(inputs, 'dspfit_amphbar', 'DSP-OFFLINE Time for all channels')

    return [timeDiffHistogram, sumTimeDiffHistogram]


def channelToPMT(module, channel):
    """
    This function converts Tile channel number to PMT number (hole/position in drawer)

       Negative number means that PMT is not connected!
    """

    position = [ 1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
                 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
                 27, 26, 25, 30, 29, 28,-33,-32, 31, 36, 35, 34,
                 39, 38, 37, 42, 41, 40, 45,-44, 43, 48, 47, 46,

                 1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
                 13, 14, 15, 16, 17, 18,-19,-20, 21, 22, 23, 24,
                 -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34,
                 44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46 ]

    if module.startswith('EB'):
        channel += 48 # Extended barrel
    pmt = position[channel]
    # One need to do something for special modules EBA15 and EBC18 where
    # the first 4 channels are not connected, but E3 and E4 gap scintilators
    # are connected to channels 18 and 19 correspondingly
    if module in ['EBA15', 'EBC18'] and (channel < 4 or channel in [18, 19]):
        pmt = -pmt

    return pmt


def getTileModuleAmplitudeVarianceOverMean(inputs):
    """ This function produces 1D summary histogram per Tile module with amplitude variance over mean of all channels in the module """

    import math
    module = inputs[0][0]['module']

    emptyPMTHistogram = getTileModuleHistogram(inputs, 'pmtGain_empty', 'Variance/Mean')
    setTileModuleHistogramStyle(emptyPMTHistogram, markerStyle=21, markerSize=0.75, labelSize=0.08, ymin=-0.2, ymax=2.2)

    evenPMTHistogram = getTileModuleHistogram(inputs, 'pmtGain_even', 'Variance/Mean')
    setTileModuleHistogramStyle(evenPMTHistogram, markerStyle=22, markerSize=0.75, labelSize=0.08, markerColor=2, ymin=-0.2, ymax=2.2)

    oddPMTHistogram = getTileModuleHistogram(inputs, 'pmtGain_odd', 'Variance/Mean')
    setTileModuleHistogramStyle(oddPMTHistogram, markerStyle=23, markerSize=0.75, markerColor=4, labelSize=0.08, ymin=-0.2, ymax=2.2)

    kapa = 1.30e-3

    for input in inputs:

        inputHistogram = input[1][0]
        nEntries = inputHistogram.GetEntries()
        if nEntries > 0:
            channel = int(input[0]['channel'])
            bin = int(channel) + 1
            pmt = channelToPMT(module, channel)
            if pmt < 0: # PMT is not connected
                emptyPMTHistogram.SetBinContent(bin, 0.01)
                emptyPMTHistogram.SetBinError(bin, 0.01)
            else:
                mean = inputHistogram.GetMean()
                rms = inputHistogram.GetRMS()

                meanError = rms / math.sqrt(nEntries)
                rmsError = rms / math.sqrt(2 * nEntries)

                if mean > 10e-3 and rms > 0.:
                    varianceOverMean = rms * rms / mean - kapa * mean
                    varianceOverMeanError = (rms * rms / mean) * math.sqrt(4 * (rmsError / rms) * (rmsError / rms) +
                                                                           (meanError / mean) * (meanError / mean))
                else:
                    varianceOverMean = -0.5
                    varianceOverMeanError = 0.001

                if pmt % 2 == 0:
                    evenPMTHistogram.SetBinContent(bin, varianceOverMean)
                    evenPMTHistogram.SetBinError(bin, varianceOverMeanError)
                else:
                    oddPMTHistogram.SetBinContent(bin, varianceOverMean)
                    oddPMTHistogram.SetBinError(bin, varianceOverMeanError)


    maxY = max(1.05 * oddPMTHistogram.GetMaximum(), 1.05 * evenPMTHistogram.GetMaximum(), 0.20)
    minY = min(oddPMTHistogram.GetMinimum() - 0.05 * abs(oddPMTHistogram.GetMinimum()),
               evenPMTHistogram.GetMinimum() - 0.05 * abs(evenPMTHistogram.GetMinimum()), -0.10)

    emptyPMTHistogram.GetYaxis().SetRangeUser(minY, maxY)

    return [evenPMTHistogram, oddPMTHistogram, emptyPMTHistogram]


def getTileModuleAmplitudeOverChargeRatio(inputs):
    """ This function produces 1D summary histogram graph per Tile module with amplitude over charge ratio of all channels in the module """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    capactior = str(inputs[0][1][0].GetName()).split('_').pop()
    amplitudeOverQHistogram = getTileModuleHistogram(inputs, f'qratio_{capactior}', f'Amplitude/Q Ratio for {capactior} pF')

    gain = inputs[0][0]['gain']
    gainName = {'hi' : 'high', 'lo' : 'low'}.get(gain)
    module = inputs[0][0]['module']
    run = str(inputs[0][1][0].GetTitle()).split(' ')[1]

    amplitudeOverQGraphName = f'{module}_{gain}_tails_{capactior}'
    amplitudeOverQGraphTitle = f'Run {run} {module} {gainName} gain: Mean Amplitude/Q and up/down tails for {capactior} pF'
    amplitudeOverQGraph = ROOT.TGraphAsymmErrors(Tile.MAX_CHAN)
    amplitudeOverQGraph.SetName(amplitudeOverQGraphName)
    amplitudeOverQGraph.SetTitle(amplitudeOverQGraphTitle)

    downLimit = 0.7
    upLimit = 1.3

    lowIntegralLimit = 0.005
    mediumIntegralLimit = 0.010
    highIntegralLimit = 0.015

    for input in inputs:

        inputHistogram = input[1][0]
        channel = int(input[0]['channel'])
        nEntries = inputHistogram.GetEntries()
        if nEntries > 0:
            bin = int(channel) + 1

            mean = inputHistogram.GetMean()
            amplitudeOverQHistogram.SetBinContent(bin, mean)

            # The following code calculates the error bars of amplitude over Q summary plots.
            # The error bar depends on the percentage of events outside a certain limit.
            # If the  percentage of events outside a certain limit is below the threshold,
            # then the error bar length is set to 0.
            # There are 3 different thresholds and a lower and an upper limit.
            minBin = inputHistogram.FindFirstBinAbove(0)
            maxBin = inputHistogram.FindLastBinAbove(0)
            xMin = inputHistogram.GetBinCenter(minBin)
            xMax = inputHistogram.GetBinCenter(maxBin)
            if downLimit < mean < upLimit:
                if xMin > downLimit:
                    xMin = mean
                else:
                    downLimitBin = inputHistogram.FindBin(downLimit)
                    # We also want underflow, in case... Integral() is calculated including the contents of both limiting bins.
                    # As 0.7 falls in the middle of a bin, we are calculating the integral by excess
                    integral = inputHistogram.Integral(0, downLimitBin)
                    ratio = integral / nEntries
                    if ratio > highIntegralLimit:
                        # Larger error bar
                        xMin = mean - 1.05
                    elif ratio > mediumIntegralLimit:
                        # Intermediate error bar
                        xMin = mean - 0.7
                    elif ratio > lowIntegralLimit:
                        # Small error bar
                        xMin = mean - 0.35
                    else:
                        xMin = mean

                if xMax < upLimit:
                    xMax = mean
                else:
                    upLimitBin = inputHistogram.FindBin(upLimit)
                    # We also want overflow, in case...  Integral() is calculated including the contents of both limiting bins.
                    # As 1.3 falls in the middle of a bin, we are calculating the integral by excess.
                    integral = inputHistogram.Integral(upLimitBin, (inputHistogram.GetNbinsX() + 1))
                    ratio = integral / nEntries
                    if ratio > highIntegralLimit:
                        # Larger error bar
                        xMax = mean + 1.05
                    elif ratio > mediumIntegralLimit:
                        # Intermediate error bar
                        xMax = mean + 0.7
                    elif ratio > lowIntegralLimit:
                        # Small error bar
                        xMax = mean + 0.35
                    else:
                        xMax = mean

            amplitudeOverQGraph.SetPoint(channel, channel + 0.5, mean)
            amplitudeOverQGraph.SetPointError(channel, 0.0, 0.0, max(0.0, mean - xMin), max(0.0, xMax - mean))
        else:
            amplitudeOverQGraph.SetPoint(channel, channel + 0.5, 0.0)

    amplitudeOverQHistogram.SetStats(ROOT.kFALSE)
    setTileModuleHistogramStyle(amplitudeOverQHistogram, markerStyle=21, markerSize=0.75, ymin=-0.2, ymax=2.2, labelSize=0.06)

    amplitudeOverQGraph.GetXaxis().SetRangeUser(0, 48)
    setTileModuleHistogramStyle(amplitudeOverQGraph, markerStyle=21, markerSize=0.75, ymin=-0.2, ymax=2.2)


    return [amplitudeOverQHistogram, amplitudeOverQGraph]


def getTileModuleTimeSlopeAndOffset(inputs):
    """ This function produces 1D summary histogram per Tile module with amplitude over charge ratio of all channels in the module """

    import ROOT
    # Force batch mode
    ROOT.gROOT.SetBatch(True)

    capactior = str(inputs[0][1][0].GetName()).split('_').pop()
    timeSlopeHistogram = getTileModuleHistogram(inputs, f'tslope_{capactior}', f'Time slope for {capactior} pF')
    timeOffsetHistogram = getTileModuleHistogram(inputs, f'toffset_{capactior}', f'Time offset for {capactior} pF')

    fitFunction = ROOT.TF1("polfun", "pol1", 0., 25.)

    for input in inputs:

        inputHistogram = input[1][0]

        nEntries = inputHistogram.GetEntries()
        if nEntries > 0:
            profile = inputHistogram.ProfileX()
            nBins = profile.GetNbinsX()
            histogram = ROOT.TH1S("histogram", "TMP Histogram", nBins, profile.GetBinLowEdge(1), profile.GetBinLowEdge(nBins + 1))

            timeShift = 0
            lastBin = 0
            lastBinContent = -99.0
            for xBin in range(1, nBins + 1):
                if profile.GetBinError(xBin) > 1e-7:
                    binContent = profile.GetBinContent(xBin)
                    # Allow only 1 shift and only of negative sign
                    if (timeShift < 1) and (binContent - (lastBinContent + (xBin - lastBin))) < -15.0:
                        timeShift = 25.
                    lastBin = xBin
                    lastBinContent = binContent
                    histogram.SetBinContent(xBin, binContent + timeShift)
                    histogram.SetBinError(xBin, profile.GetBinError(xBin))

            if histogram.GetEntries() > 1:
                histogram.Fit(fitFunction, "NQ")

                channel = int(input[0]['channel'])
                bin = int(channel) + 1

                # Shift by -25 ns to be consistent with previous definition
                timeOffset = fitFunction.GetParameter(0) - 25.0
                timeOffsetError = min(5.0, fitFunction.GetParError(0))
                timeOffsetHistogram.SetBinContent(bin, timeOffset)
                timeOffsetHistogram.SetBinError(bin, timeOffsetError)

                timeSlope = fitFunction.GetParameter(1)
                timeSlopeError = min(5.0, fitFunction.GetParError(1))
                timeSlopeHistogram.SetBinContent(bin, timeSlope)
                timeSlopeHistogram.SetBinError(bin, timeSlopeError)

    setTileModuleHistogramStyle(timeSlopeHistogram, markerStyle=21, markerSize=0.75, ymin=-0.1, ymax=1.4, labelSize=0.1)
    setTileModuleHistogramStyle(timeOffsetHistogram, markerStyle=21, markerSize=0.75, ymin=-15, ymax=15, labelSize=0.1)

    return [timeSlopeHistogram, timeOffsetHistogram]


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
