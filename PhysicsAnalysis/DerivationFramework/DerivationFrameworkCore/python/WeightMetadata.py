# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ReweightUtils.ReweightUtilsConf import * 


def addWeights(seq, pdfMap = {'CT14nlo':20}, pref = ""):

  from AthenaCommon.AppMgr import ToolSvc

  #Load standard generator event weight
  mcEventWeightNom = McEventWeight(name = pref+"mcWNom")
  ToolSvc += mcEventWeightNom

  sumOfWeightsTool = SumOfWeightsTool(name = pref+"PDFSumWeightsTool")
  sumOfWeightsTool.CutBookKeeperContainterName = "PDFSumOfWeights"
  sumOfWeightsTool.CutBookKeeperNamePrefix = ""
  sumOfWeightsTool.maxMuBins = 1

  #Load a potential generator variation event weight
  listTools = [mcEventWeightNom]
  for p in pdfMap:
    pdfName = p
    pdfN = pdfMap[p]

    pdfWeight = []
    product = []
    provTools = []
    for i in range(0, pdfN+1):
      pdfWeight.append(None)
      product.append(None)
      provTools.append(None)

      pdfWeight[i] = PDFWeight(name = pref+"pdfOnlyWeight"+pdfName+"_"+str(i), PDFName = pdfName, index = i)
      ToolSvc += pdfWeight[i]

      #Multiplies event weights
      product[i] = WeightsAgregator(name = pref+pdfName+"_"+str(i))
      product[i].Ingredients = [mcEventWeightNom, pdfWeight[i]]
      ToolSvc += product[i]

      listTools.append(product[i])

  sumOfWeightsTool.WeightTools = listTools
  seq += sumOfWeightsTool

  from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
  MSMgr.AddMetaDataItemToAllStreams("xAOD::CutBookkeeperContainer#*")
  MSMgr.AddMetaDataItemToAllStreams("xAOD::CutBookkeeperAuxContainer#*")

def addWeightsHist(stream, seq, svcMgr, CfgMgr, ToolSvc, derivName, fileName, pdfMap = {}):

  svcMgr += CfgMgr.THistSvc()
  svcMgr.THistSvc.Output += ["METADATA DATAFILE='%s' OPT='SHARE'" % (fileName) ]
  svcMgr.THistSvc.PrintAll = True
  stream.OtherAlgs += svcMgr.THistSvc 

  #Load standard generator event weight
  mcEventWeightNom = McEventWeight(name = derivName+"_mcEventWeightNom")
  ToolSvc += mcEventWeightNom

  # Setting up the PileupReweighting tool
  nomTool = CfgMgr.CP__PileupReweightingTool(derivName+"_pilenomWeight", WeightTool=mcEventWeightNom)
  nomTool.Prefix = "deriv_nom"
  nomTool.ConfigFiles = []
  nomTool.LumiCalcFiles = []
  ToolSvc += nomTool
  seq += CfgMgr.CP__PileupReweightingProvider(derivName+"_pileProvNomWeight",Tool=nomTool, ConfigOutputStream="METADATA/mcPileNom")

  #Load a potential generator variation (QCDscale, PDF,...) event weight
  for p in pdfMap:
    pdfName = p
    pdfN = pdfMap[p]

    pdfWeight = []
    product = []
    provTools = []
    for i in range(0, pdfN+1):
      pdfWeight.append(None)
      product.append(None)
      provTools.append(None)

      pdfWeight[i] = PDFWeight(name = derivName+"_pdfWeight"+pdfName+"_"+str(i), PDFName = pdfName, index = i)
      ToolSvc += pdfWeight[i]

      #Multiplies event weights
      product[i] = WeightsAgregator(name = pdfName+"_pdfWeightAg"+pdfName+"_"+str(i))
      product[i].Ingredients = [mcEventWeightNom, pdfWeight[i]]
      ToolSvc += product[i]

      # Setting up the PileupReweighting tool with an alternate name
      provTools[i] = CfgMgr.CP__PileupReweightingTool(derivName+"_pdfWeightProd"+pdfName+"_"+str(i),WeightTool=product[i])
      provTools[i].Prefix = "deriv_"+pdfName+"_"+str(i)
      provTools[i].ConfigFiles = []
      provTools[i].LumiCalcFiles = []
      ToolSvc += provTools[i]
      seq += CfgMgr.CP__PileupReweightingProvider(derivName+"_pdfProvWeight"+pdfName+"_"+str(i),Tool=provTools[i],ConfigOutputStream="METADATA/pdfWeight"+pdfName+"_"+str(i))


from DerivationFrameworkCore.DerivationFrameworkMaster import *
#addWeights(DerivationFrameworkJob, {'CT10nlo':52, 'MSTW2008nlo68cl': 40, 'NNPDF23_lo_as_0130_qed': 100, 'NNPDF30_nlo_as_0118': 100}, "")
listPDF = {}
#listPDF["CT14nlo"] = 56
#listPDF["CT14nlo_NF4"] = 0
#listPDF["NNPDF30_nlo_as_0118"] = 100
#listPDF["NNPDF30_nlo_as_0118_nf_4"] = 100
listPDF["MMHT2014nlo68cl"] = 50
#listPDF["MMHT2014nlo68cl_nf4"] = 50
#listPDF["PDF4LHC15_nlo_100"] = 100
listPDF["PDF4LHC15_nlo_30"] = 30
#listPDF["PDF4LHC15_nlo_mc"] = 100
#listPDF["PDF4LHC15_nlo_asvar"] = 1
#listPDF["NNPDF23_lo_as_0119_qed"] = 100
#listPDF["NNPDF23_nlo_as_0118_qed"] = 100
#listPDF["NNPDF30_nnlo_as_0118"] = 100
listPDF["NNPDF30_nlo_as_0118"] = 100
listPDF["CT10"] = 52
#listPDF["cteq66"] = 44
#listPDF["MSTW2008nlo68cl"] = 40
#listPDF["cteq6l1"] = 0
#listPDF["abm12lhc_5_nnlo"] = 28
#listPDF["HERAPDF20_NLO_EIG"] = 28
#listPDF["HERAPDF20_NLO_VAR"] = 13
#listPDF["ATLAS-epWZ12-EIG"] = 30
#listPDF["ATLAS-epWZ12-VAR"] = 11
#listPDF["abm12lhc_4_nnlo"] = 28

# skip this in data
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource() == 'geant4':
    addWeights(DerivationFrameworkJob, listPDF, "")

