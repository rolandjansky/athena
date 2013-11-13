/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage FourMom Package

This package provides base classes to give objects 4-momentum behavior, 
without introducing new data members. Some implementation classes, and 
helper classes for 4-momentum manipulation are also provided.

@author David Rousseau rousseau@lal.in2p3.fr

@section FourMomAbs Abstract base class

The list of kinematics accessors is provided by the abstract base 
class I4Momentum in a separate package (to minimize dependencies)
Event/EventKernel. The list of accessors can be extended easily.
 

@section FourMomImpl Implementation base class

These base classes are P4PxPyPzEBase , P4EEtaPhiMBase, P4IPtCoThPhiMBase .
The name of these classes lists what are the four basic parameters. Each 
class provide an implementation of all I4Momentum methods as a function of 
the four basic parameters accessors, which themselves are not provided.
When a class derives from one of this base class, it should implement 
the four basic parameters accessors as a function of its own data members; 
as soon as it does that all 15-odd kinematics accessors are available from 
this class, computed in an optimal way as a function of this class data 
members.

A concrete example is a CaloCell (in Calorimeter/CaloEvent), which basic 
parameters are energy, eta, phi (these two parameters are in fact taken
from a static class CaloDetDescrElement through a pointer) and m=0 (
Note that it might sound
strange to talk about mass of a calorimeter cell, but this is ok as soon 
as there is a well defined convention.)(Note that the main reason 
to have the mass is that calorimeters measure energy while trackers measure 
momentum, so the mass is needed as soon as the two need be combined). 
To dress CaloCell with all
kinematics accessors, it was sufficient to derive from P4EEtaPhiMBase, and 
implement methods e(), eta(), phi(), m(). This allows e.g. eta to be 
accessed without recomputing it from e.g. px,py,pz,m.
An additional refinement was to rewrite the sinTh(), cosTh() and cotTh() 
method to make use of the 
cached values of sinTh in the static CaloDetDescrElement.


@section FourMomConc Concrete class.

These classes are P4PxPyPzE, P4EEtaPhiM, P4IPtCoThPhiM. They simply 
derive from the corresponding P4PxPyPzEBase etc.. base class, providing
in addition the expected data members. These classes can be used directly 
for calculations, or can be used as base class if the provided 
data members are suitable. Examples of use can be found in P4DummyTest.cxx.
These classes provides set method for their four data members, and only for 
these ones; in many case setting another parameter (like Pt when Px, Py, Pz, 
E are the data members) is ill-defined and confusing.

@section FourMomHelp P4Help helper class

P4Help is a concrete class that provides calculation like invariant mass
or deltaR (more functions can easily be added). 
Examples of use can be found in P4DummyTest (this class is also useful 
to test at compile time all the virtual function are implemented).

@section FourMomCLHEP Note on relation with CLHEP HepLorentzVector

What's the benefit of these classes w.r.t CLHEP HepLorentzVector ?
CLHEP HepLorentzVector does not provide inheritance structure, so does not 
let the choice of what are the best 4 data members. CLHEP people claim 
that this is for performance reasons, while it has demonstrated that any 
computation using 4-vector (beyond addition/substraction) takes at least  
one order of magnitude more CPU time than the penality due to virtual
 inheritance.

 Connection between I4Momentum and CLHEP HepLorentzVector is provided by: 
(i) I4Momentum::hlv() method returns a CLHEP HepLorentzVector for any object
deriving from I4Momentum (ii) the concrete classes P4PxPyPzE, etc.. have 
a constructor taking a HepLorentzVector as argument, and have a 
method set4Mom(const HepLorentzVector & theHlv ).



*/
