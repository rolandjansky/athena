/**
 * @page tepage TopExamples
 *
 * @section in Introduction
 *
 * If you're looking for a tutorial on how to submit a grid job, you might like to try 
 * https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopxAODStartGuide.
 *
 * Here I'll just try to just go over the code that's included:
 *
 * - top::NoSelectionAnalysis: If the grid part didn't do any selection, use me!
 * - top::DileptonAnalysis: If the grid part did ee, mumu and emu, use me!
 * - top::LJetsAnalysis: For e+jets and mu+jets use me!
 * - top::LJetsTtresAnalysis: Includes largeR jets!
 *
 * All the codes above use (mostly) the same histograms which are added and filled by
 * using DefaultPlots.h. 
 *
 * The event loop is run by mini-to-plots.cxx, something like this:
 * 
 * @code
 * ./mini-to-plots Dilepton /path/to/file/grid_datasetname/input.txt
 * @endcode
 *
 * The word Dilepton tells it to load the top::DileptonAnalysis.  You can see how this
 * works in AnalysisTools.h addPlots().  The grid tag is extracted from the path, so that's important
 * to keep in.
 * 
 * @section ps Example python scripts scripts
 *
 * This packages also contains a bunch of python scripts to help with grid related book-
 * keeping.  To get them (for analysis) you need to do:
 * 
 * @code 
 * getExamples
 * @endcode
 *
 * This will make a directory called grid which contains the scripts you can edit and run
 * like this:
 * @code
 * ./01SubmitToGrid.py
 * ./02DownloadFromGrid.py
 * ./03LocalAnalysis.py --run
 * ./04Plot.py
 * @endcode
 *
 * @section bcs Boring code stuff
 *
 * The C++ analysis code has to inherit from top::AnalysisBase.  If you make a new class
 * you'll need to tell the loader about it in AnalysisTools.h (and the python scripts).
 *
 * Some useful code is:
 *  - top::ContainerNames: Names of the xAOD containers to use in the analysis
 *  - top::EventLoaderDirect: Loads xAOD objects from an input file in a speedy way
 *  - top::AnalysisBase: Base class for implementing analysis code
 *  - mini-to-plots.cxx: The one and only executable
 *  - AnalysisTools.h: Some tools for scaling, cutflows, loading analysis,
 *  decoding grid tags etc.
 *
 * The python scripts live in the TopExamples/python and TopExamples/scripts directories.
 */
