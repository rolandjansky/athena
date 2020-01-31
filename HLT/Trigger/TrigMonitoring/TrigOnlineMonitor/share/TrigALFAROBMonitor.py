#**************************************************************
#
# TrigAlfaROBMonitor.py
#
#==============================================================
from AthenaCommon.Constants import *
from AthenaCommon import CfgMgr
ALFAROBMonitor = CfgMgr.TrigALFAROBMonitor("ALFAROBMonitor")

#--------------------------------------------------------------
# Add the algorithm to the topSequence
# (in L2 it should be running after steering, so that the ROB
#  cache is already filled)
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += ALFAROBMonitor

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
monTool = GenericMonitoringTool('monTool')
monTool.HistPath="ALFAROBMonitor/python"

# common
monTool.defineHistogram("com-LB,goodDataAssessmentLB15", type='TH2F', path='EXPERT', title='goodDataAssessmentLB15', xbins=1000, xmin=-0.5, xmax=999.5, ybins=2, ymin=0.5, ymax=2.5 )
monTool.defineHistogram("com-LB,goodDataAssessmentLB18", type='TH2F', path='EXPERT', title='goodDataAssessmentLB18', xbins=1000, xmin=-0.5, xmax=999.5, ybins=2, ymin=0.5, ymax=2.5 )
monTool.defineHistogram("com-goodDataAssessment",        type='TH1F', path='EXPERT', title='goodDataAssessment',     xbins=10,   xmin=-0.5, xmax=9.5 )

#detector
monTool.defineHistogram("det-B7L1U-Channel,PMF", type='TH2F', path='EXPERT', title='B7L1U activity', xbins=64, xmin=0., xmax=64., ybins=23, ymin=1.,ymax=24.)
monTool.defineHistogram("det-B7L1L-Channel,PMF", type='TH2F', path='EXPERT', title='B7L1L activity', xbins=64, xmin=0., xmax=64., ybins=23, ymin=1.,ymax=24.)
monTool.defineHistogram("det-A7L1U-Channel,PMF", type='TH2F', path='EXPERT', title='A7L1U activity', xbins=64, xmin=0., xmax=64., ybins=23, ymin=1.,ymax=24.)
monTool.defineHistogram("det-A7L1L-Channel,PMF", type='TH2F', path='EXPERT', title='A7L1L activity', xbins=64, xmin=0., xmax=64., ybins=23, ymin=1.,ymax=24.)
monTool.defineHistogram("det-A7R1U-Channel,PMF", type='TH2F', path='EXPERT', title='A7R1U activity', xbins=64, xmin=0., xmax=64., ybins=23, ymin=1.,ymax=24.)
monTool.defineHistogram("det-A7R1L-Channel,PMF", type='TH2F', path='EXPERT', title='A7R1L activity', xbins=64, xmin=0., xmax=64., ybins=23, ymin=1.,ymax=24.)
monTool.defineHistogram("det-B7R1U-Channel,PMF", type='TH2F', path='EXPERT', title='B7R1U activity', xbins=64, xmin=0., xmax=64., ybins=23, ymin=1.,ymax=24.)
monTool.defineHistogram("det-B7R1L-Channel,PMF", type='TH2F', path='EXPERT', title='B7R1L activity', xbins=64, xmin=0., xmax=64., ybins=23, ymin=1.,ymax=24.)

monTool.defineHistogram("trk-full-elastic-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f elastic', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-elastic-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f elastic', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-elastic-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f elastic', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-elastic-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f elastic', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-elastic-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f elastic', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-elastic-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f elastic', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-elastic-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f elastic', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-elastic-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f elastic', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-elastic_ALFA_BG-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f elastic ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-elastic_ALFA_BG-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f elastic ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-elastic_ALFA_BG-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f elastic ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-elastic_ALFA_BG-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f elastic ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-elastic_ALFA_BG-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f elastic ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-elastic_ALFA_BG-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f elastic ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-elastic_ALFA_BG-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f elastic ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-elastic_ALFA_BG-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f elastic ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-singleDiffr-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-singleDiffr-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-singleDiffr-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-singleDiffr-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-singleDiffr-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-singleDiffr-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-singleDiffr-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-singleDiffr-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-ALFA_MBTS_singleDiffr-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f ALFA_MBTS_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_MBTS_singleDiffr-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f ALFA_MBTS_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_MBTS_singleDiffr-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f ALFA_MBTS_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_MBTS_singleDiffr-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f ALFA_MBTS_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_MBTS_singleDiffr-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f ALFA_MBTS_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_MBTS_singleDiffr-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f ALFA_MBTS_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_MBTS_singleDiffr-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f ALFA_MBTS_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_MBTS_singleDiffr-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f ALFA_MBTS_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-ALFA_LUCID_singleDiffr-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f ALFA_LUCID_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_LUCID_singleDiffr-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f ALFA_LUCID_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_LUCID_singleDiffr-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f ALFA_LUCID_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_LUCID_singleDiffr-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f ALFA_LUCID_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_LUCID_singleDiffr-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f ALFA_LUCID_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_LUCID_singleDiffr-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f ALFA_LUCID_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_LUCID_singleDiffr-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f ALFA_LUCID_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_LUCID_singleDiffr-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f ALFA_LUCID_singleDiffr', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-ALFA_EM3-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f ALFA_EM3', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_EM3-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f ALFA_EM3', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_EM3-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f ALFA_EM3', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_EM3-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f ALFA_EM3', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_EM3-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f ALFA_EM3', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_EM3-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f ALFA_EM3', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_EM3-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f ALFA_EM3', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_EM3-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f ALFA_EM3', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-ALFA_J12-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f ALFA_J12', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_J12-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f ALFA_J12', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_J12-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f ALFA_J12', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_J12-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f ALFA_J12', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_J12-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f ALFA_J12', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_J12-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f ALFA_J12', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_J12-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f ALFA_J12', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_J12-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f ALFA_J12', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-ALFA_TRT-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f ALFA_TRT', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_TRT-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f ALFA_TRT', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_TRT-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f ALFA_TRT', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_TRT-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f ALFA_TRT', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_TRT-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f ALFA_TRT', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_TRT-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f ALFA_TRT', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_TRT-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f ALFA_TRT', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_TRT-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f ALFA_TRT', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-ANY-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f ANY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f ANY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ANY-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f ANY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f ANY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ANY-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f ANY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f ANY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ANY-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f ANY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f ANY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-ANY_UNPAIRED_ISO-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f ANY_UNPAIRED_ISO', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY_UNPAIRED_ISO-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f ANY_UNPAIRED_ISO', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ANY_UNPAIRED_ISO-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f ANY_UNPAIRED_ISO', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY_UNPAIRED_ISO-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f ANY_UNPAIRED_ISO', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ANY_UNPAIRED_ISO-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f ANY_UNPAIRED_ISO', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY_UNPAIRED_ISO-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f ANY_UNPAIRED_ISO', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ANY_UNPAIRED_ISO-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f ANY_UNPAIRED_ISO', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY_UNPAIRED_ISO-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f ANY_UNPAIRED_ISO', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-ANY_ALFA_BG-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f ANY_ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY_ALFA_BG-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f ANY_ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ANY_ALFA_BG-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f ANY_ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY_ALFA_BG-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f ANY_ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ANY_ALFA_BG-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f ANY_ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY_ALFA_BG-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f ANY_ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ANY_ALFA_BG-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f ANY_ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ANY_ALFA_BG-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f ANY_ALFA_BG', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("trk-full-ALFA_EMPTY-B7L1U-x,y", type='TH2F', path='EXPERT', title='B7L1U f ALFA_EMPTY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_EMPTY-B7L1L-x,y", type='TH2F', path='EXPERT', title='B7L1L f ALFA_EMPTY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_EMPTY-A7L1U-x,y", type='TH2F', path='EXPERT', title='A7L1U f ALFA_EMPTY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_EMPTY-A7L1L-x,y", type='TH2F', path='EXPERT', title='A7L1L f ALFA_EMPTY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_EMPTY-A7R1U-x,y", type='TH2F', path='EXPERT', title='A7R1U f ALFA_EMPTY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_EMPTY-A7R1L-x,y", type='TH2F', path='EXPERT', title='A7R1L f ALFA_EMPTY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)
monTool.defineHistogram("trk-full-ALFA_EMPTY-B7R1U-x,y", type='TH2F', path='EXPERT', title='B7R1U f ALFA_EMPTY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=0.,ymax=35.)
monTool.defineHistogram("trk-full-ALFA_EMPTY-B7R1L-x,y", type='TH2F', path='EXPERT', title='B7R1L f ALFA_EMPTY', xbins=260, xmin=-23., xmax=23., ybins=175, ymin=-35.,ymax=0.)

monTool.defineHistogram("od-B7L1U-RP_1_0 position", type='TH1F', path='EXPERT', title='B7L1U RP_1_0 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-B7L1U-RP_1_1 position", type='TH1F', path='EXPERT', title='B7L1U RP_1_1 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-B7L1U-distance_1_2_side_0", type='TH1F', path='EXPERT', title='B7L1U distance_1_2_side_0', xbins=401, xmin=-20.05, xmax=20.05)
monTool.defineHistogram("od-B7L1U-distance_1_2_side_1", type='TH1F', path='EXPERT', title='B7L1U distance_1_2_side_1', xbins=401, xmin=-20.05, xmax=20.05)

monTool.defineHistogram("od-B7L1L-RP_2_0 position", type='TH1F', path='EXPERT', title='B7L1L RP_2_0 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-B7L1L-RP_2_1 position", type='TH1F', path='EXPERT', title='B7L1L RP_2_1 position', xbins=200, xmin=-145., xmax=-125.)

monTool.defineHistogram("od-A7L1U-RP_3_0 position", type='TH1F', path='EXPERT', title='A7L1U RP_3_0 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-A7L1U-RP_3_1 position", type='TH1F', path='EXPERT', title='A7L1U RP_3_1 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-A7L1U-distance_3_4_side_0", type='TH1F', path='EXPERT', title='A7L1U distance_3_4_side_0', xbins=401, xmin=-20.05, xmax=20.05)
monTool.defineHistogram("od-A7L1U-distance_3_4_side_1", type='TH1F', path='EXPERT', title='A7L1U distance_3_4_side_1', xbins=401, xmin=-20.05, xmax=20.05)

monTool.defineHistogram("od-A7L1L-RP_4_0 position", type='TH1F', path='EXPERT', title='A7L1L RP_4_0 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-A7L1L-RP_4_1 position", type='TH1F', path='EXPERT', title='A7L1L RP_4_1 position', xbins=200, xmin=-145., xmax=-125.)

monTool.defineHistogram("od-A7R1U-RP_5_0 position", type='TH1F', path='EXPERT', title='A7R1U RP_5_0 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-A7R1U-RP_5_1 position", type='TH1F', path='EXPERT', title='A7R1U RP_5_1 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-A7R1U-distance_5_6_side_0", type='TH1F', path='EXPERT', title='A7R1U distance_5_6_side_0', xbins=401, xmin=-20.05, xmax=20.05)
monTool.defineHistogram("od-A7R1U-distance_5_6_side_1", type='TH1F', path='EXPERT', title='A7R1U distance_5_6_side_1', xbins=401, xmin=-20.05, xmax=20.05)

monTool.defineHistogram("od-A7R1L-RP_6_0 position", type='TH1F', path='EXPERT', title='A7R1L RP_6_0 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-A7R1L-RP_6_1 position", type='TH1F', path='EXPERT', title='A7R1L RP_6_1 position', xbins=200, xmin=-145., xmax=-125.)

monTool.defineHistogram("od-B7R1U-RP_7_0 position", type='TH1F', path='EXPERT', title='B7R1U RP_7_0 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-B7R1U-RP_7_1 position", type='TH1F', path='EXPERT', title='B7R1U RP_7_1 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-B7R1U-distance_7_8_side_0", type='TH1F', path='EXPERT', title='B7R1U distance_7_8_side_0', xbins=401, xmin=-20.05, xmax=20.05)
monTool.defineHistogram("od-B7R1U-distance_7_8_side_1", type='TH1F', path='EXPERT', title='B7R1U distance_7_8_side_1', xbins=401, xmin=-20.05, xmax=20.05)

monTool.defineHistogram("od-B7R1L-RP_8_0 position", type='TH1F', path='EXPERT', title='B7R1L RP_8_0 position', xbins=200, xmin=-145., xmax=-125.)
monTool.defineHistogram("od-B7R1L-RP_8_1 position", type='TH1F', path='EXPERT', title='B7R1L RP_8_1 position', xbins=200, xmin=-145., xmax=-125.)

#ALFAROBMonitor.MonTool = monTool

topSequence.ALFAROBMonitor.MonTool = monTool
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#
# --- do ROB checksum test
#
#ALFAROBMonitor.TestROBChecksum            = True
#
# --- Histogram definitions for ROB checksum test
#
#MuCTPiROBMonitor.HistFailedChecksumForROB   = ('FailedChecksumForROB', 0, 50, 50)
#MuCTPiROBMonitor.HistFailedChecksumForSD    = ('FailedChecksumForSubDetector', 0, 60, 60)
#
# --- put events with checksum failures on debug stream
#
#MuCTPiROBMonitor.SetDebugStream             = True
#
# --- debug stream name for events with checksum failures
#
#MuCTPiROBMonitor.DebugStreamName            = 'MuCTPiROBErrorStream'
#
# --- plot generic/specific status bits for ROB fragments
#
#MuCTPiROBMonitor.TestROBStatus              = True
#
# ---- ROB Id for CTP/muCTPi ROB from L1 in Level-2
#
#MuCTPiROBMonitor.Lvl1CTPROBid               = 0x770001
#MuCTPiROBMonitor.Lvl1MuCTPiROBid            = 0x760001
#
# ---- ROB Id for CTP/muCTPi ROB from L1 in DAQ
#
#MuCTPiROBMonitor.DaqCTPROBid                = 0x770000
#MuCTPiROBMonitor.DaqMuCTPiROBid             = 0x760000
#
# --- Number of Sectors/Units per side for Barrel/Endcap/Forward
#
#MuCTPiROBMonitor.NumberOfBarrelUnits        = 32
#MuCTPiROBMonitor.NumberOfEndcapUnits        = 48
#MuCTPiROBMonitor.NumberOfForwardUnits       = 24
#
# --- Histogram definitions for muCTPi monitor
#
#MuCTPiROBMonitor.HistNumberOfRoIs            = ('Number_of_RoIs', 0, 50, 50)
#MuCTPiROBMonitor.HistSectorID_Barrel_muCTPi  = ('muCTPi_SectorID_Barrel',  0, 32, 32)
#MuCTPiROBMonitor.HistSectorID_Endcap_muCTPi  = ('muCTPi_SectorID_Endcap',  0, 48, 48)
#MuCTPiROBMonitor.HistSectorID_Forward_muCTPi = ('muCTPi_SectorID_Forward', 0, 24, 24)
#MuCTPiROBMonitor.HistDifferenceRoIs_RoIB_DAQ_muCTPi = ('muCTPi_differenceRoIs_RoIB_DAQ', -5, 5, 11)
#MuCTPiROBMonitor.HistBarrelHashforProblemRoIs  = ('muCTPi_Barrel_Hash_for_RoIs_only_in' ,0, 100, 100)
#MuCTPiROBMonitor.HistEndcapHashforProblemRoIs  = ('muCTPi_Endcap_Hash_for_RoIs_only_in' ,0, 100, 100)
#MuCTPiROBMonitor.HistForwardHashforProblemRoIs = ('muCTPi_Forward_Hash_for_RoIs_only_in',0, 100, 100)
#
# --- do timing for muCTPi monitoring algorithm
#
#MuCTPiROBMonitor.DoTiming                   = True
#MuCTPiROBMonitor.HistTimeMuCTPiMonitor      = ('Time_MuCTPi_Monitor' ,0.,5.,50)
#==============================================================
#
# End of TrigAlfaMonitor.py
#
#**************************************************************
