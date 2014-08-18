/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage JetEvent

@author P.A. Delsart (delsart at lapp.in2p3 dot fr)

@section Content

JetEvent defines EDM for Jet software.

Base objects are Jet and JetCollections.
From release 14, Jet and the previous analysis class ParticleJet have been merged
into the Jet class. 

JetAssociationBase and JetTagInfoBase are used to describe association to other particles and B-tagging 
information. Jet objects may hold several JetAssociationBase or JetTagInfoBase objects.

JetMoment describes a quantity (a "moment") associated to a jet. This class (and related classes) are
deprecated after release 14, since the new Jet now includes an equivalent concept.

JetKeyDescriptor allows to translate string keys (used to identify information, e.g. JetTagInfoBase) 
into int. This is done on a event-by-event basis.

Other classes descibe information related to H1-style calibration (JetECS.h, JetSums, JetSampling)



See also JetRec package

@htmlinclude used_packages.html

@include requirements

*/
