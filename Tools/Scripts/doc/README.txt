Instructions for using checkreq.sh:
===================================

The checkreq.sh script parses cmt-packages and finds
missing and unneeded use-statements to be 
inserted in or erased from the requirements file in the package.

It does this by finding all include-statements in all source- and 
headerfiles in the package. Since a generic includestatement looks as
follows:

#include "<somepackagename>/<headerfile>.h"

the script can from these includestatements collect a list of
packages used by the present cmt-package. Not all includestatements, though, 
have the simple form above and therefore the scripts uses some extra
parsing to determine used packages. Example: Most Geant4 headerfiles are
of the form G4xxx.h and therefore an includestatement for such a headerfile
is translated into a dependency on the (External) package Geant4.

Furthermore the script parses the requirementsfile looking for
linkopts-statements. Such linkopts-statements sometimes contain extra
libraries that need to be linked into binaries or libraries in the package.
These linkopts-statements are also translated into package-dependencies.

At the end a full list of package-dependencies are collected and these
dependencies are then compared with the list specified by use-statements
in the requirements file in the package. If any use-statements are missing
or not needed, the script outputs these.

A word or warning: The script tries as best as possible to identify all
missing and superflurious use-statements. However, it is not
always successful. The output of the script should therefore be
viewed critically.

The script can be run in two different ways: 1) either directly from
the cmt-directory of some package or 2) on packages and all their
subpackages in a release. An example is:

checkreq.sh -release 7.1.0 Calorimeter

that runs on all subpackages in the Calorimeter package in release 7.1.0

The first way of running can also be used with in conjunction with the
'cmt broadcast' command.

Options that the script understands:

-help    : writes a small help.
-debug   : the scripts writes extra debug information.
-writeuse: instead of writing the missing and unneeded packages, the
           script writes the complete list of use-statements needed in
           the requirements file. This requires that a section of CMTPATH
	   points to some AtlasRelease.
-ignore [ package ] : the script will ignore this package when parsing
		      packages.
-file <filename>    : the script will ignore the packages specified in the
	              file <filename> when parsing packages.
-release <n> [ packages ] : run the script on the packages (and their
                            subpackages) in the release <n>
