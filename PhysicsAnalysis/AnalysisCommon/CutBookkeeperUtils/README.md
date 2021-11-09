xAOD Event Counter Utilities
============================

This packages houses tools to keep track of the original number of
events that are in your AOD. You probably need this for things like
calculating appropriate scaling for simulation.

High-level structure
--------------------

We provide a structure called `OriginalAodCounts` which contains
several fields:

- `nEventsProcessed`: Total number of events in the original AOD.
- `sumOfWeights`: Sum of weights in the original AOD.
- `sumOfWeightsSquared`: Sum of weights squared. You might need this
  to properly calculate statistical errors.
- `nIncomplete`: Number of incomplete CutBookkeepers objects.

You can get this object from a given file by calling

```
OriginalAodCounts counts = getOriginalAodCounts(tevent)
```

or (recommended interface), instance a `OriginalAodCountsTool` and call

```
OriginalAodCounts counts;
ATH_CHECK(tool->incrementAodCounts(counts, tevent));
```

where `tevent` is a the `TEvent` object for your job.

The `OriginalAodCounts` struct has addition operators defined, so if
you loop over multiple files you can easily sum the numbers.
