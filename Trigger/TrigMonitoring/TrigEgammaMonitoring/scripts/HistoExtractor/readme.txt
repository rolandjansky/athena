INTRODUCTION
This program extracts the TrigEgamma related histograms from the big Tier0
monitoring file. Ultimately it might also write han configuration files, but
this is not yet implemented.

The decision of whether to include or exclude a particular histogram from the
input in the output is made based on two regular expressions that can be
supplied as arguments. These are matched to the full path of the histogram in
the file. A histogram will be copied over if its path matches the include expression
and does not match the exclude expression. Defaults are set up to remove all
non-TrigEgamma histograms (and some unused TrigEgamma ones as well)

Tier0 monitoring files have a root folder like "run_XXXXXXX" into which the
output of the various monitoring routines is placed. This additional folder is
not present in standalone test output, so it cannot easily be used with the
standard han configurations. The program detects these cases and adds the
folder, based on a runnumber extracted from the input file name (at least six
digits enclosed by dots).

USAGE
This program uses ROOT and  boost libraries. The simplest way to have them
available is to setup a 64bit ATHENA release, like "asetup
16.6.5,64". The executable is placed at ./bin/extractor.exe and takes at least
a source file as argument.

Other possible arguments are:
-f <filename> ... output filename. If this is not supplied, a default filename
conforming to the format "HLT_HLTEgamma_Tier0Mon_R<run>_<userid>_<date>.root"
will be used.
--incl <expression> ... include expression
--excl <expression> ... exclude expression
