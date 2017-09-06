/**
 * @mainpage
 *
 * This is documentation for the top code in the release. The general idea is that people
 * will submit a job to the derived xAOD using the top-xaod executable (see @ref tapage).
 * The output of this will be downloaded and used to make histograms (see @ref tepage).
 * @ref tapage will run on the grid and do the object corrections, selection and
 * overlap-removal.  It will write out only events objects that pass the cuts and only for
 * the events that pass the selection the user provides.   @ref tepage can read these 
 * mini-xAOD files and perform any remaining cuts (e.g. for different signal and control
 * regions) and make plots.
 *
 * AnalysisTop is split into:
 * - @ref tapage Code to read xAOD and make mini-xAOD with some selection
 * - @ref tevpage Some tools for passing an event around
 * - @ref tconfpage Configuration for the top software
 * - @ref tsompage Systematics for object variations
 * - @ref tcpage Scale factors
 * - @ref tostpage Object selection
 * - @ref testpage Event selection (and event loading)
 * - @ref tertpage Event reconstruction (e.g. KLFitter)
 * - @ref tfpage Tools for data-driven background estimates
 * - @ref tepage Example of making plots from the output of the above
 *
 *
 * @page tapage TopAnalysis
 *
 * @section i Introduction
 *
 * Use like:
 *
 * @code
 * top-xaod $ROOTCOREBIN/data/TopAnalysis/validation-cuts.txt $ROOTCOREBIN/data/TopAnalysis/input-13TeV-fondueworld.txt
 * @endcode
 *
 * Some example selections are provided in:
 *
 * @code
 * share/nocuts.txt
 * share/dil-cuts.txt
 * share/ljets-cuts.txt
 * share/validation-cuts.txt
 * @endcode
 *
 * @section tc The code
 *
 * For object selection you probably want to look at the TopObjectSelectionTools
 * package and for event selection the TopEventSelectionTools package.
 *
 * The code is split into a few classes:
 *  - top::EventSelectionManager: Loads and runs multiple event selections (e.g. e+jets and mu+jets together)
 *  - top::EventSelection: Performs an event selection with the cuts loaded from file, stores cutflow
 *
 * an executable:
 *  - top-xaod.cxx
 *
 * and a file full of useful stuff (xAOD init, file checking, library loading):
 *  - Tools.h
 * @section userobjects Object selection configuration
 *
 * The object selection is loaded via the configuration file.  You can edit the
 * examples that we provide, or write your own (even in your own library) and
 * have that loaded at run time by supplying the name of the class.  The example
 * that we provide are
 * - top::ObjectLoaderStandardCuts
 *
 * Note that the important thing is that they inherit from
 * top::ObjectLoaderBase.
 *
 * @section userevents Event selection configuration
 *
 * The event selection is loaded via the configuration file.  But you should be
 * able to write your own tools and have them loaded by supplying the library
 * name in the configuration file.
 *
 * @section useroutput Output file configuration
 *
 * The output file should be user configurable. If you want to write your own,
 * then either base it on one of those or inherit from top::EventSaverBase.
 * That said, we provide two "defaults":
 *  - top::EventSaverFlatNtuple
 *  - top::EventSaverxAOD
 */
