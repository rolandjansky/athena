Standalone AthLinks Implementation
==================================

This package provides a "standalone" implementation for the AthLinks
code. One that relies directly on the `xAOD::TEvent` code for handling
the smart links.

Note that this package must never be built together with:

    Control/AthLinks

In any given project only either one or the other must be included.
