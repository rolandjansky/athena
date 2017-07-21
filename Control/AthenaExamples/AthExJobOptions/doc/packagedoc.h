/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AthExJobOptions_page AthExJobOptions Package
@author Wim Lavrijsen <WLavrijsen@lbl.gov>
@author Martin Woudstra <Martin.Woudstra@cern.ch>

@section AthExJobOptions_AthExJobOptionsIntro Introduction

This package contains examples of configurable-style job options, and shows
how to write self-contained Gaudi component configurations by deriving your
own python classes from the generated configurables.
For this purpose, it uses a few "Hello World!"-style Algorithms and AlgTools;
the real meat, however, is in the custom python files.

@section AthExJobOptions_AthExJobOptionsOverview Class Overview

Of main interest are the python classes; the other classes are merely there in
order to have something to play with.
The python classes are:

  - CustomTopAlgorithm : shows how to implement a customization of a top algorithm by using a class derived from the generated configurable. The result is a well-contained representation of the corresponding Gaudi component.

The following Algorithm classes are available:

  - NoopAlgorithm : a do-nothing Algorithm, which only has the base-class Algorithm properties.

  - TopAlgorithm : an algorithm that makes use of a tool, set by configuration, to perform a task.

  - SubAlgorithm : an algorithm that presumes to be controlled by another algorithm.

The following AlgTool classes, all implementing the @c IVirtualTool interface, are available:

  - ConcreteTool : a tool that performs a task.

  - TemplatedTool : a tool that is templated in C++ and performs a task.

  - ToolUsingTool : a tool that makes use of a tool, set by configuration, to perform a task.

As well as the following helpers:

  - KeyMixin : a mixin class providing in/out key properties for use with @c StoreGate

  - SomeData : a custom data class, instances of which can be put in, and read from, @c StoreGate



*/
