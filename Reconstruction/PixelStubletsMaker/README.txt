This analysis software reads a special AOD file and reconstruct dE/dx tracks.
The result is written in a ROOT file as a TTree.

== Usage ==

For making an input file for this software, you need to create a special AOD file from a RAW, RDO orHITS file.

You can use Reco_ty.py for the reconstruction.
In addition to the standard options, you need to use the following option.
 --outputDAOD_IDTRKVALIDFile=yourOutputAOD.pool.root --preExec "from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags;InDetDxAODFlags;InDetDxAODFlags.ThinHitsOnTrack.set_Value_and_Lock(False);InDetDxAODFlags.DumpPixelInfo.set_Value_and_Lock(True)";

yourOutputAOD.pool.root in this example will be your input file for this software.


== Download ==
$ mkdir /path/to/a/directory
$ git clone https://:@gitlab.cern.ch:8443/rysawada/DEdxAnalysis.git

== Compile ==
$ export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
$ alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'
$ setupATLAS
$ asetup AtlasProduction 20.7.7.6 --testarea=path/to/a/directory
$ cd $TestArea/DEdxAnalysis
$ cd cmt/
$ checkreq.py 
$ cmt config
$ source setup.sh 
$ cmt make

You may use a newer ATLAS software which can be found here,
http://atlas-computing.web.cern.ch/atlas-computing/projects/releases/status/

== Run the software ==
athena.py ../share/DEdxAnalysisAlg_options.py 

You can edit the options file.
For exmaple, you can change ServiceMgr.EventSelector.InputCollections for setting the input file name.

Currently the output file name is hardcorded in src/DEdxAnalysisArg.cxx dedxout.root.
