/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  Herwig7_i

@section Herwig7ForATLAS Herwig 7 for ATLAS

Herwig 7 is a general-purpose event generator written in C++.
It provides a significant amount of improvements and new features with respect
to its predecessor Herwig++ 2.X while at the same time featuring also all of
the physics capabilities that (Fortran) HERWIG did. In this sense it is meant
to succede both previous members of the Herwig generator family.

More information, including documentation and detailed tutorials, as well as
the source code of the generator are available on the project's page at
https://herwig.hepforge.org/

For more information on Herwig 7 in ATLAS please refer to the TWiki pages at
https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Herwig7ForAtlas


@subsection AthenaInterface The Interface between Herwig7 and Athena

Herwig 7 can be used in the same way as the Herwig++ 2.X series, in particular
it is capable of showering MadGraph5_aMC@NLO and PowhegBox events.

Apart from these 'legacy' run modes, the *Matchbox* module in Herwig 7 provides
automated LO and NLO hard process generation and two alternative internal
matching strategies, called 'subtractive' (MCatNLO-like) and 'multiplicative'
(Powheg-like) matching, to both the default, angular-ordered (q-tilde) parton
shower and the dipole shower. Integration of these new features in the interface
is still ongoing and will need to be validated afterwards.

\note Currently, showering of Alpgen inputs is deprecated and was removed from
the interface since it would require a significant amount of validation. In case
your are interested in this, please let us know.


@subsection TableOfContents Contents of this Documentation

- @ref Links
- @ref FAQ
- @ref MC15JobOptions

*/
