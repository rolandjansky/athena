/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page AnalysisExamples_page Analysis Examples

 - In this package, we have few complete analysis codes to show the user how
to do analyses on ESD and AOD.

 - The user analysis code should be developed in the package PhysicsAnalysis/AnalysisCommon/UserAnalysis
The user should check out the package UserAnalysis and implement his/he analysis code there. In the
package UserAnalysis, we have a very simple analysis code: AnalysisSkeleton. But it does not scale.
The user may follow the structure of code development in this package AnalysisExamples, or not. In any
case however, the user should make his analysis code modular and scalable.

 - DoBJet: BJet Analysis code 
The user is free to modify this as he/she sees fit  This algorithm fills the container of user pre selected 
B-jets in store gate and fills an ntuple or histograms. The user must override the following 
methods, 
otherwise the base class methods will be called - they do nothing:

         *   virtual StatusCode doPreSelection();
         *   virtual StatusCode initializeParticle();
         *   virtual StatusCode finalizeParticle();

 - Job options: DoBJet_jobOptions.py. The cuts for b jet  selection are defined here, should be 
modified for the particular analysis

 - DoElectron: Electron Analysis code
The user is free to modify this as he/she sees fit. This algorithm fills the container of user pre 
selected electrons in store gate and fills an ntuple or histograms. The user must override the 
following methods, otherwise the base class methods will be called - they do nothing:

         *   virtual StatusCode doPreSelection();
         *   virtual StatusCode initializeParticle();
         *   virtual StatusCode finalizeParticle();

 - Job options: DoElectron_jobOptions.py. The cuts for electron selection are defined here, should be 
modified for the particular analysis

 - DoMuon: Muon analysis code
The use is free to modify this as he/she sees fit. This algorithm fills the container of user pre 
selected muons in store gate and fills an ntuple or histograms. The user must override the 
following methods, otherwise the base class methods will be called - they do nothing:

         *   virtual StatusCode doPreSelection();
         *   virtual StatusCode intializeParticle();
         *   virtual StatusCode finalizeParticle();

 - Job options: DoMuon_jobOptions.py. The cuts for muon seletion are defined here, should be modified 
for the particular analysis

 - DoParticle: Particle Analysis base class,  should never be instanciated!

 - DoParticleJet: ParticleJet analysis code
The user should modify this as he/she sees fit. This algorithm fills the container of user pre selected 
jets in store gate and fills an ntuple or histograms. The user must override the following 
methods, otherwise the base class methods will be called - they do nothing:

         *   virtual StatusCode doPreSelection();
         *   virtual StatusCode initializeParticle();
         *   virtual StatusCode finalizeParticle();

 - Job options: DoParticleJet_jobOptions.py. The cuts for q, g jet selectrion are defined here, should 
be modified for the particular analysis

 - DoPhoton: Photon Analysis code
The user is free to modify this as he/she sees fit. This algorithm fills the container of user pre 
selected photons in store gate and fills an ntuple or histograms. The user must override the 
following methods, otherwise the base class methods will be called - they do nothing:

         *   virtual StatusCode doPreSelection();
         *   virtual StatusCode initializeParticle();
         *   virtual StatusCode finalizeParticle();

 - Job options: DoPhoton_jobOptions.py. The cuts for photon selection are defined here, should be 
modified for the particular analysis

 - DoTauJet:  TauJet analysis code
The user should modify this as he/she sees fit. This algorithm fills the container of user pre selected 
tau-jets in store gate and fills an ntuple or histograms. The user must override the following 
methods, 
otherwise the base class methods will be called - they do nothing:

         *   virtual StatusCode doPreSelection();
         *   virtual StatusCode initializeParticle();
         *   virtual StatusCode finalizeParticle();

 - Job options: DoTauJet_jobOptions.py. The cuts for tau jet selection are defined here, should be 
modified for the particular analysis

 - Zll analysis Example (l = electron, muon, tau)
An analysis algorithm to demonstrate Z to leptons analysis. It simply reads the containers of the pre 
selected particles from StoreGate and does Z to lepton reconstruction. The pre selected particles 
are recorded in the transient event store (StoreGate) by the DoParticle algorithms described above. It 
therefore runs the DoParticle algorithms described above so that the pre selected (or selected) 
particles are available the event store beforehand.

 - Job Options: ZeeZmmOnAODExample_jobOptions.py, should be modified for cuts, etc. One ROOT output 
files are produced for histograms.

 - Job Options: VFitZmmOnAODExample_jobOptions.py, should be modified for cuts, etc. One ROOT output 
files are produced for histograms and ntuple.

 - Job Options: ZeeOnESDExample_jobOptions.py, should be modified for cuts, etc. For analysis on ESD.
One ROOT output files are produced for histograms.

 - Job Options: ZtautauExample_jobOptions.py, should be modified for cuts, etc. For Z -> tautau 
reconstruction on AOD. One ROOT output files are produced for histograms.

 - H to 4 lepton analysis Example (l = electron, muon)
An analysis algorithm to demonstrate Higgs to leptons analysis. It simply reads the containers of the 
pre selected particles from StoreGate and does H to 4-lepton reconstruction. The pre selected particles 
are recorded in the transient event store (StoreGate) by the DoParticle algorithms described above. It 
therefore runs the DoParticle algorithms described above so that the pre selected (or selected) 
particles are available the event store beforehand.

 - Job options: H4lExample_jobOptions.py, should be modified for cuts, etc. Two ROOT output 
files are produced: one for ntuples and the other for histograms.

 - gg to ttbar to jjb lnu b analysis example
An analyis algorithm to demoonstrate the reconstruction of ttbar to jjb lnu b. It reads the container of
electrons, muons, jets and b-jets, and also the missing Et object from StoreGate. It uses the SpecialUtils
for the solution to the neutrino pz problem, and also its uses analysis combinations and permutations. As
described above, the DoParticle algorithms run first, recording the pre-selected particles in StoreGate.
This ttbar algorithm does not have an ntuple, only histograms.

 - Job options: ttbarExample_jobOptions.py, should be modified for cuts, etc. One ROOT output file is 
produced for the histograms.

 - Job options: PIDinAOD_jobOptions.py, for single particle idnetification studies. It runs only the 
DoParticle algorithms. Note that the DoParticle_jobOptions.py cannot run independently by themselves,
they must be included as fragments in other job options such as PIDinAOD_jobOptions.py. Two 
ROOT output files are produced: one for histograms the other for ntuples.  

 - Job options: AnalysisCommon_jobOptions.py, for common options to all algorithms such as common DLL. 
Cannnot run independently, must be included as a fragment in other job options.

 - Job options: produceGenEvents_jobOptions.py, an example job options for event generation. The 
generated events can be passed to atlfast for fast simulation or to full simulation. Consult the 
Monte Carlo Generator Group for details.

 - Analysis flags: these are flags which control which DoParticle_jobOptions the user wants to run. For 
examples, in the H to 4 leptons analysis algorithms, only the electron and the muon selection algorithm 
are ON (all the other particle selection algorithms are switched OFF). All the flags are ON by default. 
Look at the job options ZllExamples_jobOptions.py or H4lExample_jobOptions.py to see how to switch OFF 
particular algorithms.

 - Examples of job transformations for large scale production of ESD and AOD: look in the 
sub-directory "scripts".

 - For details on how to produce ESD, AOD, how to set your input ESD or AOD in the analysis job 
options, see the following "Customizing ESD and AOD production" on this web page:
@htmlinclude https://uimon.cern.ch/twiki/bin/view/Atlas/PhysicsAnalysisTools

 - A python script to merge many AOD files into a single file: look in the directory mergeAOD. There 
you will find 2 files: globalMerge.py which allows you merge many AOD files into one. The reason you
may want to do this is that, for large statistics, running your analysis code over many AOD POOL 
files may be slow because of I/O operations. So you may be better off merging the AOD files, so your 
analysis code runs on a fewer number of AOD files (same statistics) and a much less processing time. 
The script globalMerge.py should be executed in python. It produces a shell script, merge.sh, and a 
number of job options fragments for the files to merge. You can then execute merge.sh, interactively 
or in batch. The script merge.sh run athena jobs which use the top job options Merge.py to do the 
merging. The output file names of the merged AOD POOL data files are defined in merge.sh when it is 
produced by globalMerge.py. 

 - globalMergeCLArgs.py. Purpose: set variables at commandline 
    Usage: python globalMergeCLArgs.py <dir> <fileprefix> <datasetid> 
    Example: python globalMerge2.py rfio:/castor/...  rome.004100.recov10.T1_McAtNLO_top 41 

 - globalMergeConfigFile.py. Purpose: set variables in config file mergeconfig.py 
    Usage: python globalMergeConfigFile.py 
    Example of Config file: 
      prefix="rome.004100.recov10.T1_McAtNLO_top" 
      dir="rfio:/castor/cern.ch/grid/atlas/datafiles/rome/recov10" 
      dsid=4100 
      catalog="file:PoolFileCatalog.xml" 
      nfiles=20 

 - MiscellaneousExamples: an example algorithm to show how to access TrackParticle (Trk::Track) 
information from the Electron, the Muon for example, how to do back navigation from the AOD to the ESD 
(from ParticleJet to calorimeter cells for examples), how to do symLink, how to use the CompositeParticle 
class and how to do combinations with a selection.

 - AthenaAwareNTuple: an example of how to create an ATHENA aware NTuple. What is meant by ATHENA 
aware is that it can be used as input to other ATHENA, for example to make pre-selection.

 - JetTagAna: To simply access the b-tagging weight for each jet, have a look at the section 
SECTION::WEIGHTS in the JetTagAna.cxx file. To access the Monte-Carlo label (b,c or light) of a jet, 
have a look at the section SECTION::LABEL. To access detailed information about each tag, see  
SECTION::DETAILS in the JetTagAna.cxx

 - TrackExample: an example algorithm to show how to access TrackParticle collections, useful for analyses
that requires access to TrackParticle data such as track summary, perigee parameters, vertices, etc.

 - FilterExample: an example algorithm to show how to filter various collections of AOD classes using the so-called Filter Tools of AnalysisUtils package.
It makes use of a collection of IParticleFilter classes for each of the collection one wants to filter.
The Algorithm first retrieves the containers from StoreGate and copy the elements in a new container only if it satisfies the criteria of the IParticleFilter.
Eventually it stores these filtered containers.
Notice the use of the 
@verbatim AnalysisUtils::copy_if( inCont->begin(), inCont->end(), std::back_inserter(*outCont), filter );@endverbatim
method which does the copy and the filtering in one go.

 - ReadTruthParticles: an example algorithm which exercises the TruthParticle interface. It shows how to navigate back to the HepMC::GenParticles and retrieve the children (in a HepMC format) as well as how to retrieve the (TruthParticle) children of a TruthParticle.
 The Algorithm first retrieves a container of TruthParticle, loop over its elements and for each of them asks for its HepMC::GenParticle creator (and use the GenParticle methods).
 Then, for each TruthParticle, it loops over its children in a TruthParticle format.

 - An example of LCG Minuit Minimization: see LCGMinuitMinimization.cxx 

 - For questions or comments: Ketevi A. Assamagan
   ketevi@bnl.gov


*/
