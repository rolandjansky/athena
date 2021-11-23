# $Id: GetLCClassification_jobOptions.py,v 1.1.1.1 2008-11-04 08:56:11 menke Exp $
#
# File: CaloLocalHadCalib/share/GetLCClassification_jobOptions.py
# Created: October 2008, S.Menke
# Purpose: Generate Classification Histos in a athena job
#

from CaloLocalHadCalib.CaloLocalHadCalibConf import GetLCClassification
from AthenaCommon.SystemOfUnits import deg, GeV, MeV
from AthenaCommon.AlgSequence import AlgSequence
from math import ( pi as m_pi, log10 as m_log10 )

GetLCC = GetLCClassification("GetLCC")
GetLCC.OutputFileName = outFileNameLCC
#
# Example how to set dimensions for classiciation:
#
#GetLCC.ClassificationDimensions = {
#    'side': ('side',-1.5,1.5,1),
#    '|eta|': ('|eta|',0.,5.,25),
#    'phi': ('phi',-m_pi,m_pi,1),
#    'log10(E_clus (MeV))': ('log10(E_clus (MeV))',m_log10(200*MeV),m_log10(1*TeV),13),
#    'log10(<rho_cell (MeV/mm^3)>)-log10(E_clus (MeV))': ('log10(<rho_cell (MeV/mm^3)>)-log10(E_clus (MeV))',-9.0,-4.0,20),
#    'log10(lambda_clus (mm))': ('log10(lambda_clus (mm))',0.0,4.0,20)}
#
GetLCC.ClusterCollectionName = "CaloTopoClusters"

topSequence += GetLCC
