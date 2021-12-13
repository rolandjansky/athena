# $Id: GetLCWeights_jobOptions.py,v 1.2 2008-12-08 16:57:41 menke Exp $
#
# File: CaloLocalHadCalib/share/GetLCWeights_jobOptions.py
# Created: October 2008, S.Menke
# Purpose: Generate Weight Histos in a athena job
#

from CaloLocalHadCalib.CaloLocalHadCalibConf import GetLCWeights
from AthenaCommon.SystemOfUnits import deg, GeV, MeV, TeV
from AthenaCommon.AlgSequence import AlgSequence
from math import ( pi as m_pi, log10 as m_log10 )

GetLCW = GetLCWeights("GetLCW")
GetLCW.OutputFileName = outFileNameLCW
GetLCW.UseInversionMethod = True
#
# Example how to set dimensions for two samplings (here EMB1 & 2):
#
# GetLCW.SamplingDimensions = {
# 'EMB1:EMB1': ('EMB1',0.5,1.5,1),
# 'EMB1:side': ('side',-1.5,1.5,1),
# 'EMB1:|eta|': ('|eta|',0.,1.6,16),
# 'EMB1:phi': ('phi',-m_pi,m_pi,1),
# 'EMB1:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(200*MeV),m_log10(1*TeV),22),
# 'EMB1:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
# 'EMB1:weight': ('weight',-2.0,3.0,1),
# 'EMB2:EMB2': ('EMB2',1.5,2.5,1),
# 'EMB2:side': ('side',-1.5,1.5,1),
# 'EMB2:|eta|': ('|eta|',0.,1.6,16),
# 'EMB2:phi': ('phi',-m_pi,m_pi,1),
# 'EMB2:log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(200*MeV),m_log10(1*TeV),22),
# 'EMB2:log10(rho_cell (MeV/mm^3))': ('log10(rho_cell (MeV/mm^3))',-7.0,1.0,20),
# 'EMB2:weight': ('weight',-2.0,3.0,1)}
#
GetLCW.ClusterCollectionName = "CaloTopoClusters"
GetLCW.CalibrationHitContainerNames = ["LArCalibrationHitInactive"
                                       ,"LArCalibrationHitActive"
                                       ,"TileCalibHitInactiveCell"
                                       ,"TileCalibHitActiveCell"]
topSequence += GetLCW
