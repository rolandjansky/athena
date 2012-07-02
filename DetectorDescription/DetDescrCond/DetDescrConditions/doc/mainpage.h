/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
\mainpage The DetDescrCondtions package

\section introductionDetDescrConditions Introduction

This package collects together conditions database objects used in detector
description applications. The POOL converters for all the objects are 
defined in the package InDetCondAthenaPool.

\section AlignableTransform

This class consists of a vector of HepGeom::Transform3Ds each associated with
an offline Identifier. It is used for example to store the alignment constants
for each layer of the SCT and pixel detector. Methods are provided to
get and set the tranforms using the Identifier as a key, to iterate over
all the transforms in the class, and to print the contents.

An AlignableTransformContainer is also defined, consisting of a DataVector
of AlignableTransforms, allowing them to be read/written as a single entity.

\section DetCondFloat

This class consists of a vector of vectors of floats, each associated
with an offline Identifier. Several identifiers can be mapped to the same
set of floats to save space where common constants are required. This
class is used for example by the TRT calibration data.

\section DetDescrKeyTrans

This class consists of a vector of HepGeom::Transform3Ds, each associated with
an artbitrary string identifier. It is similar to AlignableTranform, but
using strings instead of Identifiers, and the internal implementation is
simpler (using a map instead of an embedded class).

*/
