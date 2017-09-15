/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AthExThinning_page AthExThinning Package
@author Sebastien Binet

@section AthExThinning_AthExThinningIntro Introduction

This package serves both as an example on how to perform data thinning and as a unit test, making sure everything is working as planned.
It consists of 3 algorithms explaining how to use the @c IThinningSvc interface to perform thinning of @c DataVector containers as a possible way of reducing disk size of a given dataset.

@section AthExThinning_AthExThinningOverview Class Overview
  The AthExThinning package contains the following transient data classes:

  - AthExParticle : a simple 4-momentum-like data class. Its sole purpose is to hold data. Some of the instances of these @c AthExParticle will be removed (aka "thinning")

  - AthExParticles : this class inherits from @c DataVector<AthExParticle>. Hence it is a container of @c AthExParticle. As said earlier, the thinning process will consist of removing some of the elements of this container.

  - AthExDecay : a class aggregating two pointers (ie: @c ElementLink) to @c AthExParticle (hold by an @c AthExParticles container)

  - AthExElephantino : this class consists of a vector of @c ElementLink towards certain elements of a given (or multiple) container of @c AthExParticles.

  Then, a set of according persistent classes (ie: classes whose layout is optimized to be written on disk) have also been collected into this package:

  - AthExParticles_p1 : the persistent representation of the @c AthExParticles. There is no "rocket science" in this persistent representation (from a technical point of view), it is mostly to exercize the transient/persistent separation of data classes.

  - AthExDecay_p1 : the persistent representation of the @c AthExDecay data class. Same comment as above holds.

  - AthExElephantino_p1 : the persistent representation of the @c AthExElephantino transient data class.

  Finally, there are 3 algorithms manipulating the above transient data classes :

  - AthExThinning::CreateData : a simple algorithm creating a container of @c AthExParticles. Then it creates a @c AthExDecay pointing to some of the previously created @c AthExParticles. Finally a @c AthExElephantino is created. All these data classes are put into @c StoreGate through the @c StoreGateSvc and scheduled to be written out on disk (into a POOL file).

  - AthExThinning::WriteThinnedData : this is the algorithm actually exercizing the real thinning. It retrieves the previously created container of @c AthExParticles and, using the @c IThinningSvc, punchs holes into that container. It shows the various methods to remove elements out of a container while keeping the @c ElementLink pointing to other elements of this container, valid. Everything happens during the @c AthExThinning::WriteThinnedData::doThinning() method. Eventually the thinned container is written out into a new POOL file.

  - AthExThinning::ReadThinnedData : this is the algorithm reading back the thinned container, making sure that the removed elements are not in the container anymore and that the various @c ElementLink are still valid and pointing to the right elements.

The other classes are the usual boiler plate needed to implement the transient/persistent separation: T/P serializers and AthenaPOOL converters. They are implementation details as far as this package is concerned.

Note however that <b>only T/P separated</b> classes can make use of the thinning process as it relies on the T/P separation of the @c ElementLink to correct the persistent index (to the element in the persistent container) with the according offset, reflecting the removal of elements. (See @c IThinningSvc and @c ThinningSvc documentation in, respectively, packages AthenaKernel and AthenaServices.)



*/
