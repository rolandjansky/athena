# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

import os

# Needed to correct ROOT behavior; see below
CWD = os.getcwd()
# Importing gSystem may change the current directory to one of the
# command-line arguments; chdir to original directory to have
# predictable behavior
from ROOT import gSystem

# Import the ROOT library for reading han results
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils

from DataQualityUtils.hancool_histo_algdef import (ExtractNEntries, ExtractAverage, ExtractNBins, ExtractMean, ExtractEMean, ExtractRMS, ExtractERMS, ExtractPol0, ExtractPol1, ExtractMeanG, ExtractRMSG, ExtractConstG, ExtractSideBands, ExtractTotInt, ExtractXMean, ExtractYMean, ExtractXRMS, ExtractYRMS, ExtractOnlyNBins)
# Mapping han<->COOL
from DataQualityUtils.hancool_histo_foldermap import folderMapHisto


os.chdir(CWD)

# LumiBlock length (in minutes)
LBlength = 1.0

# Mapping Status<->Code
codeMap = {
    "Undefined": 0,
    "Red": 1,
    "Yellow": 2,
    "Green": 3
}

# Mapping different interval types
intervalType = {
    0: "_minutes30_",
    1: "_minutes10_",
    2: "ERROR"
}

# Generates LB from filename


def getLimits(name):
    global low_limit
    global hi_limit
    if (name.find('minutes10_') > -1):
        t = name.split('minutes10_')
        digit = float(((t[len(t)-1]).split('_'))[0])
        low_limit = int((digit-1.0)*10.0/LBlength+1)-1
        hi_limit = int(digit*10.0/LBlength)
    elif (name.find('minutes30_') > -1):
        t = name.split('minutes30_')
        digit = float(((t[len(t)-1]).split('_'))[0])
        low_limit = int((digit-1.0)*30.0/LBlength+1)-1
        hi_limit = int(digit*30.0/LBlength)
    else:
        low_limit = 0
        hi_limit = 4294967295
    return {"since": low_limit, "until": hi_limit}


# Looks up the result in the HanOutputFile
def stringGetResult(file, rootFolder):
    rootFolder = file+":"+rootFolder
    of = dqutils.HanOutputFile(file)
    result = of.getStringName(rootFolder)
    return result


def stringGetInfo(file, rootFolder):
    rootFolder = file+":"+rootFolder
    of = dqutils.HanOutputFile(file)
    info = of.getInfo(rootFolder)
    if (len(info) > 0):
        return info
    else:
        return 'null result'


def ListHistoAssessments(xml, channel):
    the_result_histo = "Undefined"
    g = open(xml, 'r')
    while True:
        line = g.readline()
        if not line:
            break
        if line.find('run') > -1:
            gg = line.split(':')[1]
            ww = gg.split(' ')
            res = ''.join(ww)
            if res.find(channel) > -1:
                the_result_histo = res
    g.close()
    return the_result_histo


def AllHistograms(file):
    of = dqutils.HanOutputFile(file)
    resultstrH = of.stringAllHistograms()
    return resultstrH


def AllDQAssessments(file):
    of = dqutils.HanOutputFile(file)
    resultstr = of.stringAllDQAssessments()
    return resultstr


def MakeHistoAssessmentsFile(file):
    of = dqutils.HanOutputFile(file)
    resultstr = of.stringHistoAssessments()
    g = open('Allhisto.list', 'w')
    g.write(resultstr)
    g.close()


def NameHisto_Splitting(channel):
    t = channel.split('/')
    N = int(len(t)-1)
    histo_name = channel.split('/')[N]
    return histo_name


def NamePath_Splitting(channel):
    t = channel.split('/')
    N = int(len(t)-1)
    list_name = channel.split('/')[0:N]
    path_name = '/'.join(list_name)
    return path_name

# -------------------------------------------------------------
# hancool_histo()
# -------------------------------------------------------------


def hancool_histo(inputFilePath="", input_run=-1, dbConnectionHisto="", dqmfOflHisto="", dbTagName=""):

    if (input_run == -1):
        input_run = 52280

    if (len(inputFilePath) < 1):
        inputFilePath = "/afs/cern.ch/user/a/atlasdqm/dqmdisk1/han_results/tier0/FDR2b/1/physics_Express/"
    if (inputFilePath.rfind("/") != (len(inputFilePath)-1)):
        inputFilePath += "/"
    if (len(dbConnectionHisto) < 1):
        dbConnectionHisto = "sqlite://;schema=/afs/cern.ch/user/a/atlasdqm/w0/users/onofrio/DQMF_08112008/14.2.10/WorkArea/python/MyCOOL_histo_FDR2c.db;dbname=OFLP200"

    if (len(dqmfOflHisto) < 1):
        dqmfOflHisto = "/GLOBAL/DETSTATUS/DQMFOFLH"
    if (len(dbTagName) < 1):
        dbTagName = "DetStatusDQMFOFLH-FDR2-02"

    # pdb.set_trace()
    modH = dqutils.HistogramDataCOOL(
        dbConnectionHisto, dqmfOflHisto, 0, 0, 0, 0)
    path = inputFilePath
    run = int(input_run)
    filename = "run_"+str(run)+"_han.root"

    # filling OFLH DB for histos (global run)
    for pair in folderMapHisto.items():
        i = 0
        number = 0
        print(pair[0], pair[1])
        # print("--------------------------------------")
        histo_string = NameHisto_Splitting(pair[0])
        path_string = NamePath_Splitting(pair[0])
        # print("--------------------------------------")
        while ((os.path.isfile(path+filename)) and (i < 2)):
            while os.path.isfile(path+filename):
                if (getLimits(filename)["until"] == 4294967295):
                    modH.setIOV(run, getLimits(filename)["since"],
                                run, getLimits(filename)["until"])
                    MakeHistoAssessmentsFile(path+filename)
                    xml = 'Allhisto.list'
                    the_result_histo = ListHistoAssessments(xml, pair[0])
                    print(the_result_histo)

                    if (the_result_histo == "Undefined"):

                        print(" This histogram DOES NOT have a corresponding channel in the DB! ")
                        print(" Info cannot be inserted ")

                    else:
                        histo_nameonly = the_result_histo.split(
                            '/')[int(len(the_result_histo.split('/')))-1]
                        print(histo_nameonly)
                        resultH = stringGetResult(
                            path+filename, pair[0]+"_/Results/Status")
                        print(resultH)
                        algo = stringGetResult(
                            path+filename, pair[0]+"_/Config/name")
                        print(algo)
                        entries = ExtractNEntries(
                            path+filename, path_string, histo_string)
                        print(entries)
                        mean_rms = stringGetInfo(
                            path+filename, pair[0]+"_/Results")
                        print(mean_rms)

                        par1 = '0'
                        par2 = '0'
                        par3 = '0'
                        par4 = '0'
                        par5 = '0'

                        if (algo.find('Bins_Diff_FromAvg') > -1 and mean_rms.find('null') < 0):
                            par1 = ExtractAverage(mean_rms)
                            par2 = ExtractNBins(mean_rms)
                        elif (algo.find('GatherData') > -1 and mean_rms.find('null') < 0):
                            if (mean_rms.find('Error') > -1):  # new GatherData
                                par1 = ExtractMean(mean_rms)
                                par2 = ExtractEMean(mean_rms)
                                par3 = ExtractRMS(mean_rms)
                                par4 = ExtractERMS(mean_rms)
                            else:  # old GatherData
                                par1 = ExtractMean(mean_rms)
                                par2 = ExtractRMS(mean_rms)
                        elif (algo.find('Simple_pol1_Fit') > -1 and mean_rms.find('null') < 0):
                            par1 = ExtractPol0(mean_rms)
                            par2 = ExtractPol1(mean_rms)
                        elif (algo.find('Simple_gaus_Fit') > -1 and mean_rms.find('null') < 0):
                            par1 = ExtractMeanG(mean_rms)
                            par2 = ExtractRMSG(mean_rms)
                            par3 = ExtractConstG(mean_rms)
                        elif (algo.find('SideBand') > -1 and mean_rms.find('null') < 0):
                            par1 = ExtractSideBands(mean_rms)
                            par2 = ExtractTotInt(mean_rms)
                        elif (algo.find('CheckHisto_Mean') > -1 and mean_rms.find('null') < 0):
                            par1 = ExtractXMean(mean_rms)
                            par2 = ExtractYMean(mean_rms)
                        elif (algo.find('CheckHisto_RMS') > -1 and mean_rms.find('null') < 0):
                            par1 = ExtractXRMS(mean_rms)
                            par2 = ExtractYRMS(mean_rms)
                        elif (algo.find('Bin') > -1) and mean_rms.find('null') < 0 and ((algo.find('Threshold') > -1) or (algo.find('ContentComp') > -1)):
                            output = ExtractOnlyNBins(histo_nameonly, mean_rms)
                            par1 = output[0]
                            if (len(output) > 1):
                                par2 = output[1]
                                par3 = output[2]
                                par4 = output[3]
                                par5 = output[4]
                            else:
                                par2 = '0'
                        else:
                            par1 = '0'
                            par2 = '0'
# combined algorithms:
                        # CheckHisto_Mean&GatherData
                        if (algo.find('CheckHisto_Mean') > -1 and algo.find('GatherData') > -1 and mean_rms.find('null') < 0):
                            comb_mean_rms = mean_rms.split('XMean: ')
                            if (comb_mean_rms[0].find('Error') > -1):  # new GatherData
                                par1 = ExtractMean(comb_mean_rms[0])
                                par2 = ExtractEMean(comb_mean_rms[0])
                                par3 = ExtractRMS(comb_mean_rms[0])
                                par4 = ExtractERMS(comb_mean_rms[0])
                                par5 = ExtractXMean(comb_mean_rms[1])
                            else:  # old GatherData
                                par1 = ExtractMean(comb_mean_rms[0])
                                par2 = ExtractRMS(comb_mean_rms[0])
                                par3 = ExtractXMean(comb_mean_rms[1])
                                par4 = ExtractYMean(comb_mean_rms[1])
                        # BinContentComp&GatherData
                        if (algo.find('BinContentComp') > -1 and algo.find('GatherData') > -1 and mean_rms.find('null') < 0):
                            if (mean_rms.find('Error') > -1):  # new GatherData
                                par1 = ExtractMean(mean_rms)
                                par2 = ExtractEMean(mean_rms)
                                par3 = ExtractOnlyNBins(
                                    histo_nameonly, mean_rms)
                                par4 = ExtractRMS(mean_rms)
                                par5 = ExtractERMS(mean_rms)
                            else:  # old GatherData
                                par1 = ExtractMean(mean_rms)
                                par2 = ExtractRMS(mean_rms)
                                par3 = ExtractOnlyNBins(
                                    histo_nameonly, mean_rms)

                        codeH = codeMap[resultH]

                        try:
                            float(par1)
                            float(par2)
                            float(par3)
                            float(par4)
                            float(par5)
                            modH.insertH(pair[1], codeH, str(algo), int(entries), float(
                                par1), float(par2), float(par3), float(par4), float(par5), dbTagName)
                            print("----------- global: Insertion successfull! --------------------")
                            print(dbTagName)
                        except Exception:
                            modH.insertH(pair[1], codeH, str(algo), int(
                                entries), 0, 0, 0, 0, 0, dbTagName)
                            print('Could not upload parameters in DB, they are not numbers! Check the algorithm!')

                    number = number+1
                    filename = "run_" + \
                        str(run)+intervalType[i]+str(number)+"_han.root"
                number = 1
                i = i+1
                filename = "run_" + \
                    str(run)+intervalType[i]+str(number)+"_han.root"
            filename = "run_"+str(run)+"_han.root"
        print("--------------------------------------")
