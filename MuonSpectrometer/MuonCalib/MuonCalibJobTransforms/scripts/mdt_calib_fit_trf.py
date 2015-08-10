#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Produce calibration fits.
Take the calibration ntuples as input files and produce fitted data."""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from MuonCalibJobTransforms.MDTCalibFitConfig import mdtCalibFitConfig
from MuonCalibJobTransforms.MDTCalibFitConfig import CalibrationAlgorithmArg
from MuonCalibJobTransforms.MDTCalibFitConfig import SelectChamberArg
from MuonCalibJobTransforms.MDTCalibFitConfig import CalibNtupleFileListArg
from MuonCalibJobTransforms.MDTCalibFitConfig import CalibSiteArg
from MuonCalibJobTransforms.MDTCalibFitConfig import WriteToDBArg
from MuonCalibJobTransforms.MDTCalibFitConfig import T0FitByArg
from MuonCalibJobTransforms.MDTCalibFitConfig import ADCFitByArg
from MuonCalibJobTransforms.MDTCalibFitConfig import MuonRecoArg
from MuonCalibJobTransforms.MDTCalibFitConfig import RefineT0Arg
from MuonCalibJobTransforms.MDTCalibFitConfig import SegmentRefitArg
from MuonCalibJobTransforms.MDTCalibFitConfig import InputFromDBArg
from MuonCalibJobTransforms.MDTCalibFitConfig import InputFromFileArg
from MuonCalibJobTransforms.MDTCalibFitConfig import OutputToDBArg
from MuonCalibJobTransforms.MDTCalibFitConfig import OutputToFileArg
from MuonCalibJobTransforms.MDTCalibFitConfig import HeadIdArg
from MuonCalibJobTransforms.MDTCalibFitConfig import MdtDqaFileArg
from MuonCalibJobTransforms.MDTCalibFitConfig import FillHistosArg
from MuonCalibJobTransforms.MDTCalibFitConfig import DoFinalizeArg
from MuonCalibJobTransforms.MDTCalibFitConfig import DoEfficiencyArg
from MuonCalibJobTransforms.MDTCalibFitConfig import DoGlobalTimeFitArg
from MuonCalibJobTransforms.MDTCalibFitConfig import SuppressRecalibrationArg
from MuonCalibJobTransforms.MDTCalibFitConfig import SegsFromRawArg
from MuonCalibJobTransforms.MDTCalibFitConfig import CurvedSegsArg
from MuonCalibJobTransforms.MDTCalibFitConfig import BfieldCorrArg
from MuonCalibJobTransforms.MDTCalibFitConfig import HitResidArg
from MuonCalibJobTransforms.MDTCalibFitConfig import DeadElAlgArg
from MuonCalibJobTransforms.MDTCalibFitConfig import ADCcutArg
from MuonCalibJobTransforms.MDTCalibFitConfig import EffNsigArg
from MuonCalibJobTransforms.MDTCalibFitConfig import EffChiCutArg
from MuonCalibJobTransforms.MDTCalibFitConfig import EffUseDefResArg
from MuonCalibJobTransforms.MDTCalibFitConfig import EffiGtfArg
from MuonCalibJobTransforms.MDTCalibFitConfig import EffiReCalArg
from MuonCalibJobTransforms.MDTCalibFitConfig import EffiTimeCorrArg
from MuonCalibJobTransforms.MDTCalibFitConfig import TrackPCut
from MuonCalibJobTransforms.MDTCalibFitConfig import FastTrackMatch
from MuonCalibJobTransforms.MDTCalibFitConfig import SuperFastTrackMatch
from MuonCalibJobTransforms.MDTCalibFitConfig import ApplyTimeSlewingCorrection
from MuonCalibJobTransforms.MDTCalibFitConfig import ApplyRtScaling
from MuonCalibJobTransforms.MDTCalibFitConfig import SplitterStoreRaw, SplitterSplitLevel,SplitterADCCut,SplitterMergeOutputFiles,SplitterStoreTracks

class MDTCalibFitJobTransform( JobTransform ):
    def __init__(self):
        JobTransform.__init__(self,
                              authors = [ Author('Alessandro De Salvo','alessandro.desalvo@roma1.infn.it'),
                                          Author('Fabrizio Petrucci','fabrizio.petrucci@roma3.infn.it') ] ,
                              skeleton='MuonCalibJobTransforms/skeleton.mdt_calib_fit.py',
                              help = __doc__,
                              config=mdtCalibFitConfig )

        #add arguments
        self.add( CalibNtupleFileListArg() )
        self.add( MaxEventsArg() )
        self.add( SkipEventsArg() )
        self.add( GeometryVersionArg() )
        self.add( CalibrationAlgorithmArg(['RunScan','ClassicT0','MTT0','Integration', 'ClassicAutocal', 'AnalyticAutocal', 'CurvedAutocal', 'SimpleResolution', 'Chi2Resolution', 'Efficiency', 'RpcTiming', 'ControlHistograms', 'MDTChamberGeometryDetermination', 'MdtDqa', 'Splitter', 'TimeSlewing', 'ResidualVsTimeTool', 'Display']), default='ClassicT0' )
        self.add( SelectChamberArg(), default='[BML,4, 4]' )

        self.add( CalibSiteArg(), default='RM' )
        self.add( JobConfigArg(package='MuonCalibJobTransforms'), default='NONE' )
        self.add( DBReleaseArg(), default='NONE' )

        self.add(WriteToDBArg(), default="True")
        self.add(T0FitByArg(), default='MEZZ_CARD')
        self.add(MuonRecoArg(), default='4' )
        self.add(ADCFitByArg(), default='MEZZ_CARD')
        self.add(RefineT0Arg(), default='True')
        self.add(SegmentRefitArg(), default='False')
        self.add(InputFromDBArg(), default='False')
        self.add(InputFromFileArg(), default='True')
        self.add(OutputToDBArg(), default='False')
        self.add(OutputToFileArg(), default='True')
        self.add(HeadIdArg(), default='-1')
        self.add(MdtDqaFileArg(), default='MdtDqa')
        self.add(FillHistosArg(), default='True')
        self.add(DoFinalizeArg(), default='False')
        self.add(DoEfficiencyArg(), default='True')
        self.add(DoGlobalTimeFitArg(), default='False')
        self.add(SuppressRecalibrationArg(), default='True')
        self.add(SegsFromRawArg(), default='False')
        self.add(CurvedSegsArg(), default='False')
        self.add(BfieldCorrArg(), default='False')
        self.add(HitResidArg(), default='False')

        self.add(DeadElAlgArg(), default='1')
        self.add(ADCcutArg(), default='70.')
        self.add(EffNsigArg(), default='-1')
        self.add(EffChiCutArg(), default='5.')
        self.add(EffUseDefResArg(), default='True')

        self.add(EffiGtfArg(), default='False')
        self.add(EffiReCalArg(), default='False')
        self.add(EffiTimeCorrArg(), default='False')
        self.add(TrackPCut(), default=-3.0)
        self.add(FastTrackMatch(), default=True)
        self.add(SuperFastTrackMatch(), default=False)
        self.add(ApplyTimeSlewingCorrection(), default=True)
        self.add(ApplyRtScaling(), default=False)
        self.add(SplitterStoreRaw(), default=False)
        self.add(SplitterStoreTracks(), default=True)
        self.add(SplitterSplitLevel(name="splitterFileSplitLevel"), default="NONE")
        self.add(SplitterSplitLevel(name="splitterNtupleSplitLevel"), default="NONE")
        self.add(SplitterADCCut(), default=0)
        self.add(SplitterMergeOutputFiles(), default="")

        #add other features
        self.add( SQLiteSupport() )

# execute it if not imported
if __name__ == '__main__':
	trf = MDTCalibFitJobTransform()
	exit_code=trf.exeSysArgs().exitCode()
	if not trf.runArgs().splitterMergeOutputFiles or exit_code!=0:
		sys.exit(exit_code)
	import glob
	import shutil
	import subprocess
	for output in trf.runArgs().splitterMergeOutputFiles.split(","):
		out_items=output.split(":")
		if len(out_items)>2:
			sys.stderr.write("WARNING: Malformed outfile arg " + output + "\n")
			continue
		glob_string="OutNtuple*.root"
		target_file=output
		if len(out_items)==2:
			glob_string="OutNtuple_" + out_items[0] + "*.root"
			target_file = out_items[1]
		infiles=glob.glob(glob_string)
		if not infiles:
			prev_argv=sys.argv[:]
			sys.argv=[sys.argv[0], '-b']
			from ROOT import *
			dummy_outfile=TFile.Open(target_file, "RECREATE")
			dummy_outfile.Write()
			sys.argv=prev_argv
			continue
		if len(infiles)==1:
			shutil.move(infiles[0], target_file)
			continue
		#merge one by one to avoid disk-space restrictions
		file1=infiles[0]
		for next_file in infiles[1:]:
			if subprocess.call(["hadd", target_file, file1, next_file]):
				sys.stderr.write("WARNING: hadd failed!\n")
			subprocess.call(["rm", next_file, file1])
			if not next_file==infiles[-1]:
				shutil.move(target_file,  "tmp." + target_file)
				file1="tmp." + target_file
		
	sys.exit(exit_code)	
	
