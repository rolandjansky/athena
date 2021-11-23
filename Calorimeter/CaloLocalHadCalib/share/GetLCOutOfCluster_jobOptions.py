# $Id: GetLCOutOfCluster_jobOptions.py,v 1.1.1.1 2008-11-04 08:56:11 menke Exp $
#
# File: CaloLocalHadCalib/share/GetLCOutOfCluster_jobOptions.py
# Created: October 2008, S.Menke
# Purpose: Generate OutOfCluster Histos in a athena job
#

from CaloLocalHadCalib.CaloLocalHadCalibConf import GetLCOutOfCluster
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.AlgSequence import AlgSequence
from math import ( pi as m_pi, log10 as m_log10 )

GetLCO = GetLCOutOfCluster("GetLCO")
GetLCO.OutputFileName = outFileNameLCO
#
# Example how to set dimensions for out-of-cluster corrections:
#
#GetLCO.OutOfClusterDimensions = {
#    'side': ('side',-1.5,1.5,1),
#    '|eta|': ('|eta|',0.,5.,50),
#    'phi': ('phi',-m_pi,m_pi,1),
#    'log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(200*MeV),m_log10(1*TeV),22),
#    'log10(lambda_clus (mm))': ('log10(lambda_clus (mm))',0.0,4.0,20),
#    'weight': ('weight',0.,5.,1)}
#
GetLCO.ClusterCollectionName = "CaloTopoClusters"

topSequence += GetLCO
