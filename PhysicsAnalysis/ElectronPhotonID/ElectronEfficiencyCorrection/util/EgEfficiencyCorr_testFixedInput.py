#! /usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import subprocess

if __name__ == "__main__":

	recFile="ElectronEfficiencyCorrection/2015_2017/rel21.2/Summer2017_Prerec_v1/offline/efficiencySF.offline.LooseAndBLayerLLH_d0z0_v13.root"
	models=["FULL","COMBMCTOYS"]
	etas=["-2.469","0.0","2.469"]
	pts=["7001","10001","20001","100001"]
	runnumbers=["999888"]
	for runnumber in runnumbers:
		for model in models:
			for eta in etas:
				for pt in pts:
					cmdopts = ["EgEfficiencyCorr_testEgEfficiencyCorrWithoutFile",
							"-f" + recFile,
							"-tFullSim",
							"-c"+model,
							"-r"+ runnumber,
							"-e" + eta,
							"-p" +pt ]
					rawoutput=subprocess.check_output(cmdopts,universal_newlines=True)
					maplines=rawoutput.split('\n')
					print(str(maplines[1]))
