/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage  MCTester_i
   @author  MCTester_i: Nadia Davidson nadia.davidson@cern.ch
   @author MC-TESTER tool: Nadia Davidson, Piotr Golonka, Tomasz Pierzchala, Tomasz Przedzinski and Zbigniew Was 

   @section  general  General Notes about MC-TESTER

   MC-TESTER in a tool external to Athena that allows the comparison of particle
   or resonance decay between two Monte-Carlo Generators. It produces a latex (or pdf)
   booklet with two sections: 
   1. A list of branching ratios for each generator with the maximum histogram shape difference 
   parameters for each mode.
   2. A section for each decay mode showing histograms comparing the invariant mass distributions
   for different combinations of daughters, plus their shape difference parameter.

   This package, MCTester_i is an interface to the tool and includes as few small extra checks such as
   four momentum conservation, and checks that the particle has a life-time and decays.
   As both the comparisons and extra checks are done in a semi-automated way on any
   MCEventCollection, it provides a useful tool for the validation of Monte-Carlo 
   production.

   Please see the official documentation for <A HREF="http://cern.ch/MC-TESTER/">MC-TESTER</A>,
   which has much more detail on the tool including links to papers, examples of comparisons, 
   instructions of use (outside Athena), the latest release to download, doxygen documentation
   and others.

   <hr>

   @section use Example of using MCTester_i

   Running a comparison using MC-TESTER is a two step process: "Generation and Analysis".
   Within Athena, the Generation Step runs on any MCEventColletion in StoreGate and produces
   a root file containing the decay modes and histograms. MCTesterToolJobOptions.py contains
   an example of running the tool which can easily be modified to run the job you need. 
   
   A simple example could be to check tau+ decays in a dataset "dataset1" against those is a 
   second dataset "dataset2". Firstly get the jobOption file:
   \verbatim  
   get_files MCTesterToolJobOptions.py \endverbatim 
   
   Modify the InputCollection. The argument to "glob" should the path to the files. Wildcards 
   allow a list of files to be returned. 
   \verbatim
   files=glob.glob("path_to_my_dataset1/dataset1/*.root") \endverbatim

   Change some configurables:
   \verbatim
   MCTesterAlg.result_path="tauValidation1.root" #output filename
   MCTesterAlg.decay_particle=15- #particle pdgID to be examined
   
   #Three lines of description to go in the booklet
   MCTesterAlg.generation_description_line1="Dataset1: ???"
   MCTesterAlg.generation_description_line2="Some description of dataset 1?"
   MCTesterAlg.generation_description_line3="Whatever else you think is relevant information"
   theApp.EvtMax = -1 #Number of events to process \endverbatim

   Now run in the usual way:
   \verbatim
   athena  MCTesterToolJobOptions.py \endverbatim
   
   You should see a bit of summary information printed from the tool and possibly some WARNINGs.
   There are three types of WARNING coming from the extra validation checks. Each should be printed
   with the offending event number:
   - Non-conservation of 4 momentum in the event
   - Particle being examined does not decay
   - Particle being examined has zero lifetime. For this last category there are several causes
   for a zero lifetime which should be considered. If the particle is stable, i.e. does not decay, its
   lifetime is returned as zero (this could be fixed in next release). If a particle has a decay into
   itself (such as particle -> particle + gamma) then it should also have zero lifetime as this is
   probably not a physical process, but a result of the construction of the event record. These type
   of "decays" can be switched off with:

   \verbatim
   MCTesterAlg.include_decays_to_self='false' #include particle -> particle + gamma decays? \endverbatim
   But leaving these type of "decays" may also be useful to examine how photos etc. are place in an event record. 


   After the above steps are completed the file "tauValidation1.root" should be located in the run directory. 
   The steps should be repeated with dataset2 to produce the file "tauValidation2.root"

   For the analysis step, a comparison booklet is created from the two root files. 
   This does not require running athena, but running root and latex with the MC-Tester root 
   macros and libraries. A simple shell script, compare.sh, is provided in MCTester_i and
   demonstrates how to do this step from your run directory (a cshell script is not provided yet). 

   \verbatim
   get_files compare.sh \endverbatim

   Then please modify as needed. eg.
   \verbatim
   FILE1=tauValidation1.root
   FILE2=tauValidation2.root \endverbatim

   Then run
   \verbatim
   ./compare.sh \endverbatim
   
   And your run directory should contain the results booklet: "tester.pdf" 
   plus all the files used to make it.

   For more freedom in configuration at both the Generation and Analysis step, you
   can place a file called "SETUP.C" in your working directory. An example is provided:

   \verbatim                                                                                                                                              
   get_files SETUP.C \endverbatim  

   Setup variables allow options like changing the algorithm for shape difference calculation,
   modifying the list of stable daughter particles before histograming, modifying descriptions
   after the generation step, using logarithmic scale etc. Please refer to the MC-Tester 
   documentation, or contact one of the authors, for a full description of the flexibility 
   allowed with this style of job configuration. 

**/
