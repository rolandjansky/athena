/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
\mainpage JetTagInfo

Package for info classes to be used in jet tagging.

@author Andreas.Wildauer@cern.ch

\section introductionJetTagInfo Introduction
JetTagInfo contains classes to store tagging information. The classes are
used by the b-tagging packages BTagging.

\section howitworksJetTagInfo Info objects
Each tagger has to implement its own info object. In order to be working in a jet tag
context like btagging the info object has to inherit from ITagInfo or BaseTagInfo (preferably
from the last one since it implements needed methods like infoType() and tagLikelihood()).

For more information on the different info objects have a look at the class descriptions.

\section packagecontentJetTagInfo Package Contents
JetTagInfo contains the following files/classes:
 - ITagInfo ... abstract base class which defines basic interfaces for all info objects
 - BaseTagInfo .. implements some methods of ITagInfo
 - LifetimeInfo .. class to store lifetime tag info (tracksignificance).
 - SecVtxInfo .. class to store tag relevant secondary vertex information.

*/
