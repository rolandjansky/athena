#!/usr/bin/perl -w

use strict;
use Getopt::Long;
use FileHandle;

my $datestring = `date --iso-8601`;
chomp( ${datestring} );
$datestring =~ s/\-//g;

my $sample = "";
my ${run_dir} = `pwd`; chomp( ${run_dir} );
my $maxEvents = -1;
my $bzip_output = '';
my $result_basedir = "";
my $result_rundir = "";
&GetOptions('s=s'=>\$sample,'d=s'=>\${run_dir},'n=i'=>\${maxEvents},'r=s'=>\${result_basedir},'bzip'=>\${bzip_output},'l=s'=>\${result_rundir});

die "provide sample name." unless (${sample} ne "");
die "number of events doesn't make sense" unless( ${maxEvents}!=0 && ${maxEvents}>=-1 );

my $result_subdir = "${sample}/${datestring}";
my $result_dir;
if( !($result_basedir eq "") ) {
    $result_dir = "${result_basedir}/${result_subdir}";
} elsif( !($run_dir eq "") ) {
    $result_dir = "${run_dir}/results/${result_subdir}";
} else {
    $result_dir = "./${result_subdir}";
}
if( $result_rundir eq "" ) {
    $result_rundir = "${run_dir}/submission/${result_subdir}";
}

print "running sample ${sample}\nin ${run_dir}\n";
if( $maxEvents != -1 ) {
    print "on ${maxEvents}\n";
}
if( !($bzip_output eq '') ) { print "with bzip2 compression\n"; }
print "with output to ${result_dir}\n";
print "and batch/log output to ${result_rundir}\n";

if( !(-e $result_dir) ) { system( "mkdir -p $result_dir" ); }
if( !(-e $result_rundir) ) { system( "mkdir -p $result_rundir" ); }

my @filenames;

# define filenames
if( $sample eq "test" ) {
    @filenames = ( "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9069.pool.root" );
}
if( $sample eq "v1213indep" ) {
    @filenames = ( "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9069.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9070.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9071.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9073.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9074.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9078.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9079.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9080.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9081.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9082.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9083.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9084.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9085.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9086.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9087.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9088.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9089.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9090.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9091.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9092.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9093.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9094.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9095.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9096.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9097.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9098.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9099.pool.root" ,
                   "dcap:///pnfs/uct3/data/users/brubaker/ftkprod1213_080712/digi/g4digi-9100.pool.root" );
}
if( $sample eq "cscwhbb1034" ) {
    @filenames = ( "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00001.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00004.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00005.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00006.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00007.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00008.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00009.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00010.pool.root.3" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00011.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00013.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00015.pool.root.3" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00019.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00021.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00024.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00025.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00026.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00031.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00033.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00035.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00040.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00042.pool.root.3" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00043.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00047.pool.root.3" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00048.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00049.pool.root.3" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00050.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00051.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00052.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00054.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00055.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00059.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00061.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00062.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00063.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00066.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00068.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00073.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00076.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00080.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00081.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00082.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00085.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00086.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00087.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00089.pool.root.3" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00090.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00091.pool.root.2" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00094.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00100.pool.root.1" );
}
if( $sample eq "cscwhbb1034anton" ) {
    @filenames = ( #"dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00001.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00013.pool.root.1" ,
                   "dcap:///pnfs/uct3/users/antonk/specialg_misal1_csc11.005850.WH120bb_pythia.digit.RDO.v13004003/RDO.022779._00015.pool.root.3" );
}


# create one batch job for each file
my $jobid = 0;
my $skipEvents = 0;
my $nsleepsec=0;
foreach my $filename (@filenames) {
    ++$jobid;
    $nsleepsec += 5;
    my $batchname = "${result_rundir}/batch_${jobid}.sh";
    my $logname = "${result_rundir}/batch_${jobid}.log";
    my $joboptionsname = "${result_rundir}/jobOptions_${jobid}.py";
    my $rawoutputname = "${result_dir}/ftksimwrap_hit_raw_${jobid}.dat";
    my $outputname = "${result_dir}/ftksimwrap_hit_${jobid}.dat";
    if( ${bzip_output} ne '' ) {
        $rawoutputname = "${rawoutputname}.bz2";
        $outputname = "${outputname}.bz2";
    }
    # remove output files if they exist
    if( -e $rawoutputname ) { system( "rm -f ${rawoutputname}" ); }
    if( -e $outputname ) { system( "rm -f ${outputname}" ); }
    if( $jobid==1 ) { 
        $skipEvents = 147;
    }
    if( $jobid==224 ) { 
        $skipEvents = 147;
    }    
    # write torque job script
    my $fhbatch = FileHandle::new();
    open $fhbatch, ">${batchname}";
    print $fhbatch <<ENDBATCHSCRIPT;
#!/bin/bash
sleep ${nsleepsec}s
cd ${run_dir}
cd ..
pwd
hostname
source setup_athena.sh
source setup_athena.sh
cd ${run_dir}
nohup athena.py ${joboptionsname}
ENDBATCHSCRIPT
    system( "chmod u+x $batchname" );
    # write job options
    my $fhjoboptions = FileHandle::new();
    open $fhjoboptions, ">${joboptionsname}";
    print $fhjoboptions <<ENDJOBOPTIONS;
# job options for 13.0.40 simulation
OutputLevel     = WARNING
doTrkNtuple     = False
doPixelTrkNtuple = False
doJiveXML       = False
doVP1           = False
doWriteESD      = False
doWriteAOD      = False
doReadBS        = False
doAuditors      = True

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False
include( "InDetRecExample/ConfiguredInDetFlags.py")
InDetFlags = ConfiguredInDetFlags(xKalman      = True,
                                  iPatRec      = True,
                                  newTracking  = True,
                                  doTruth      = True)
InDetFlags.enableBackTracking()
InDetFlags.enableV0Finder()
InDetFlags.enableConversions()
from AthenaCommon.DetFlags import DetFlags
DetDescrVersion = "ATLAS-CSC-01-02-00"
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator 
AtlasExtrapolator = AtlasExtrapolator(name='Trk::Extrapolator') 
ToolSvc += AtlasExtrapolator 
include("InDetRecExample/InDetRec_all.py")
# run fasttracksimwrap
theApp.TopAlg += [ "DumpSp" ]
theApp.Dlls   += [ "FastTrackSimWrap" ]
DumpSp = Algorithm( "DumpSp" )
DumpSp.OutFileName = "${outputname}"
DumpSp.OutFileNameRawHits = "${rawoutputname}"
DumpSp.OutputLevel = 4
DumpSp.minPt = 0.*GeV
DumpSp.maxEta = 3.3
DumpSp.TruthToTrackTool = InDetTruthToTrack
# input file
ServiceMgr.EventSelector.InputCollections = [ "${filename}" ]
theApp.EvtMax = $maxEvents
ServiceMgr.EventSelector.SkipEvents = $skipEvents
ENDJOBOPTIONS
    #
    system( "qsub -l walltime=23:59:59 -N wrapper_${sample} -j oe -o ${logname} ${batchname}" );
} # end for each input filename
