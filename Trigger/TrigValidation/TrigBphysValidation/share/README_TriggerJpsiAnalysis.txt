
  TriggerJpsiAnalysis

  Analysis of b-physics trigger performance

  part of TrigBphysValidation package

  author: Daniel Scheirich <scheiric@cern.ch>

  Nov 26 2010


  !!! Tested with release AtlasOffline-16.0.2,32

===========================

  TABLE OF COTENT
===========================

  1/  Checking out the package
  2/  Running localy
  3/  Running on the Grid
  4/  Retrieving output datasets
  5/  Meging ouput files
  6/  ROOT analysis classes
  7/  Running the ROOT analysis
  8/  Python configuration scripts
  9/  Definition of histograms

===========================

  1/  Checking out the package. Before compiling, check out and compile the
      following packages:

        cmt co -r BPhysAnalysisSkeleton-00-00-56 PhysicsAnalysis/BPhys/BPhysAnalysisSkeleton
        cmt co -r BPhysAnalysisTools-00-01-65 PhysicsAnalysis/BPhys/BPhysAnalysisTools
        cmt co -r BPhysAnalysisObjects-00-00-34 PhysicsAnalysis/BPhys/BPhysAnalysisObjects
        cmt co -r TrkVertexAnalysisUtils-00-02-15 Tracking/TrkVertexFitter/TrkVertexAnalysisUtils
        cmt co -r TrigEffJpsiTools-01-00-13 Trigger/TrigAnalysis/TrigEffJpsiTools
        cmt co -r TrigDecisionTool-00-01-09-01 Trigger/TrigAnalysis/TrigDecisionTool
        cmt co PhysicsAnalysis/JpsiUpsilonTools
        cmt co Tracking/TrkVertexFitter/TrkVKalVrtFitter

      and don't forget the package itself:

        cmt co Trigger/TrigValidation/TrigBphysValidation

      NOTE: in fact, you don't need to compile BPhysAnalysisSkeleton.
            We only need some .py files from there.


  2/  Running localy. From PhysicsAnalysis/BPhys/BPhysAnalysisSkeleton/share
      copy the following python script into your run directory:

        BPhysAnalysisMasterAuto.py
        trigger.py

      from Trigger/TrigValidation/TrigBphysValidation/share copy the following
      scripts into your run directory

        configureServices.py
        selectJpsiCandidates.py
        RunTriggerJpsiAnalysis.py

      Edit BPhysAnalysisMasterAuto.py: line

        rec.UserAlgs.set_Value_and_Lock("RunEarlyOnia.py")

      replace by

        rec.UserAlgs.set_Value_and_Lock("RunTriggerJpsiAnalysis.py")

      Fill the path to your local AOD/ESD dataset here:

        athenaCommonFlags.FilesInput = [ "..." ]

      If you want to use Good Run List selection mechanism, copy the GRL file
      obtained from "https://atlasdqm.cern.ch/grl/ "or
      "http://atlas-runquery.cern.ch/" into your run directory. Then un-comment
      the following section of the "RunTriggerJpsiAnalysis.py" script:

        ## Configure the goodrunslist selector tool

        from GoodRunsLists.GoodRunsListsConf import *
        goodRunsListSelectorTool = GoodRunsListSelectorTool()
        goodRunsListSelectorTool.GoodRunsListVec = [ 'MyLBCollection.xml' ]

        ToolSvc += goodRunsListSelectorTool

        ## GRL selector

        from GoodRunsListsUser.GoodRunsListsUserConf import *
        job.ModSequence1 += GRLTriggerSelectorAlg( name = 'GRLTriggerAlg1' )

        job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = ['MyLBCollection']
        ## NOTE - variable above MUST match name inside GRL xml file

      String "MyLBCollection" must be replaced by the name of the GRL file

      Run localy by executing

        athena BPhysAnalysisMasterAuto.py| tee log

      NOTE: sometimes, it is necessary to get "authentication.xml" file to
      gain access to the condition database:

        get_files authentication.xml


  3/  Running on the Grid. In "TrigBphysValidation/grid" folder there are
      scripts that help you to submit to the Grid using pathena. You need to
      setup your grid environment and Panda

        see e.g. https://twiki.cern.ch/twiki/bin/view/Atlas/PandaAthena#1_Setup

      Then copy scripts from "Trigger/TrigValidation/TrigBphysValidation/grid"
      into your run directory. The scripts you need are:

        datastetsList.py
        submit.py
        download.py

      Edit the file "datastetsList.py" and specify datasets you want to use
      for the analysis. In file "submit.py" edit the following lines:

        userTag           = "user10.DanielScheirich"      # put your user tag here
        dataTag           = "data10_7TeV_MuonswBeam"      # string that describes the input data
        alg               = "TriggerJpsiAnalysis"         # string that describes algorithm
        tryNo             = "01"                          # must be unique for each execution of the job

      Name of the output datasets will be created automatically following this
      scheme:

        userTag.tryNo.dataTag.runNo.alg


      Run number is extracted automatically from the input dataset name, but
      you must specify "userTag" and "tryNo". "tryNo" must be a number or a
      string unique for each execution of the Grid job (Panda doesn't allow
      for 2 output datasets to have the same name)

      In addition you'll need all the scrips mentioned in section 2/.

      Submit to the grid by running

        python submit.py

      You can monitor progress of your job through Panda monitor

        http://panda.cern.ch:25980/server/pandamon/query?ui=users&days=7

      This lists user from last 7 days, so your name should be there after you
      submit the jobs.

      Sometimes, jobs fail so you have to resubmit them. You can resubmit the
      failed jobs by running "pbook" command. Once it loads the information
      about your jobs you can retry by calling

        retry( job_id )

      "job_id" is a number you can get for each job from Panda monitor.

      Sometimes, jobs fail because of the site problems. Then you have to
      resubmit the whole job to a new site. Just edit "datasetsList.py" to
      contain only those datasets that need resubmitting and also don't forget
      to change "tryNo" in "submit.py" before you execute it again.


  4/  Retrieving output datasets. script download.py download the output
      datasets using DQ2. First, setup DQ2 tool

        source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup.zsh
        voms-proxy-init -voms atlas

      Edit download.py: "tryNo" must correspond to the "tryNo" from "submit.py"
      script!

      Sometimes, jobs fail so you have to resubmit the jobs. Then you can end
      up with some datasets having different "tryNo". Just edit
      "datasetsList.py" to contain only those datasets that finished
      successfully before you start downloading.

      Download files by running

        python download.py


  5/  Meging ouput files. NOTE: new ROOT analysis program doesn't require
      ntuples to be merged. However, if you prefere your files to be merged,
      please continue reading.

      After the download you should have several folders containing output
      files. Each folder carries the name of the output dataset. To merge the
      ntuples, edit merge.py and specify names of the OUTPUT datasets in the
      dataset array (i.e. names of the folders created by DQ2)

      Run merge

        python merge.py

      It will create merged ntuple file for each dataset plus one merged file
      containing all the datastes called all.root

  6/  ROOT analysis classes. The ROOT analysis program is located in folders

        TrigBphysValidation/rootScripts/TriggerJpsiAnalysis
        TrigBphysValidation/rootScripts/RootAnalysisCommon

      The bulk of the analysis algorithm is implemented in
      "TrigJpsiRootAnalysis" and "CutsJpsi" classes. They are located in the
      following files:

        TrigJpsiRootAnalysis.h          header file
        TrigJpsiRootAnalysis.cpp        main source code
        CutsJpsi.h                      offline jpsi selection

      In addition the algorithm uses a lot of other support classes in the
      TrigBphysValidation/rootScripts/RootAnalysisCommon folder:

        Utils.h                         Various useful methods

        Log.h                           Messaging service similar to the
                                        one from Athena

        AnalysisBase.h                  Base class for analysis algorithm.
                                        TrigJpsiRootAnalysis inherits from
                                        this class

        CutsBase.h                      Base class for selection. CutsJpsi
                                        inherits from this class

        HistogramHolder.h               Class for managing histograms

      The following classes were taken from
      "BPhysAnalysisSkeleton/rootScripts" package. They are designed to read
      automatic ntuples created by b-physics tools

        NtupleReader.h
        BCompositeParticle.h
        Muon.h
        Track.h
        TriggerDecision.h
        Vertex.h

        Chain.h                         Branches of trigger tree created by
                                        TrigJpsiAnalysis algorithm

    7/  Running the ROOT analysis. First you have to copy files from
        "TriggerJpsiAnalysis" and "RootAnalysisCommon" into a single directory.
        To run the analysis program you have to compile
        "TrihJpsiRootAnalysis.cpp" first. There are two ways to do it.
        You can either use makefile located in the same folder, or you can
        compile directly in ROOT. Both ways should be equivalent. You need to
        setup ROOT enviroment first for instance on LXPLUS:

          export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.26.00/x86_64-slc5-gcc34-opt/root
          export PATH=$ROOTSYS/bin:$PATH
          export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib

        and then either run

          gmake

        or compile in ROOT by executing

          root
          .L TrigJpsiRootAnalysis.cpp+

        Once the main class "TrigJpsiRootAnalysis" is compiled, it can be
        loaded by ROOT macros and its methods can be executed. At the moment,
        there are five macros doing different analyzes:

          runAnalyzeJpsi.C              Draws basic J/psi plots. This is used
                                        to tune offline J/psi cuts

          runCompareChains.C            Compare over-all efficiencies of
                                        different chains

          runAnalyzeChain.C             Accesses trigger--offline matching from
                                        the ntuple and calculates
                                        efficiencies of different stages of
                                        the trigger chain.

          runAnalyzeRate.C              Draws rate histograms.

          runAnalyzeEffVsRate.C         Draws plot "Efficiency vs. Rate
                                        Reduction Factor" for different
                                        combinations of "nu-of-hits" and
                                        "vertexing chi2" cuts


    8/  Python configuration scripts. Even though ROOT macros can be executed
        directly from root, the recomended way to run the analysis is to use
        Python configuration scripts. The configuration algorithm options and
        cuts is performed via variables in the script. Script then uses those
        variables and generates ".C" ROOT macros and execute them. There are
        the following configuration scripts available:

          jpsiCuts.py                   Settings of the J/psi selection cuts.
                                        Those are imported to all the other
                                        scripts so one can be sure that only
                                        one set of cuts is used for all the
                                        analyzes.

          runAnalyzeJpsi.py             Generates "runAnalyzeJpsi.C" macro.
                                        Input files can be provided either as
                                        an explicit list of ".root" files, or
                                        one can specify a path to the
                                        directory where the files are located.
                                        Program doesn't check for the files
                                        extensions, thus all the files in the
                                        directory should be proper ROOT files.

          runCompareChains.py           Generates "runCompareChains.C" macro

          runAnalyzeChain.py            Generates "runAnalyzeChain.C" macro

          runAnalyzeRate.py             Generates "runAnalyzeRate.C" macro

    9/  Definition of histograms. Titles, ranges and binning of all the
        histograms are defined in the external files. They are then loaded by
        "HistogramHolder" class. The following histogram config must be present
        in the directory:

          histograms_analyzeJpsi.config
          histograms_compareChains.config
          histograms_analyzeChain.config
          histograms_analyzeRate.config


