# The *process* argument

This argument allows one to set what kind of processes, in terms of number of real and fake/non-prompt leptons in the final state, are estimated by the fake lepton background yields or weights computed by the tools. This can typically be used to ensure that there is no overlap with the Monte Carlo-based estimates of background processes with prompt leptons, which may contain additional fake leptons if the analysis chooses not to veto those based e.g. on truth-matching information. Another possible usage is to obtain separate estimates for backgrounds with exactly one, or at least two fake leptons.

**Note that the fake factor method only supports `>=1F[T]`.**

## Recognized syntax

The syntax is similar to that of the *[selection](doc/arg_selection.md)* argument: 

+ Intended number of real leptons (`R`) and/or fake/non-prompt leptons (`F`) to estimate: `0R`, `>=1F`, `1-2F`, `<3F`...

+ By default only tight leptons are counted; this can be overriden with the adequate suffix: `[T]` for tight leptons (but since this is the default, the user in never required to specify it), `[!T]` for anti-tight leptons, `[L]` for loose leptons. E.g. `>=1F`, `0-1R[!T]`...

+ Combination of several conditions: `0R, >=1F` or `0R[L], >=1F`

## Examples

Taking the example of an inclusive 1-lepton selection (*selection* = `>=1T`), which also relies on some variable (meff, HT...) built from all leptons in the event. Background with fake leptons may originate from QCD (all fake leptons) or W+jets (0-1 real lepton, 0+ fake leptons). The choice of the *process* depends on the treatment of the W+jets MC events: 

+ W+jets estimate includes all MC events. Then the data-driven fake background estimate should estimate only the QCD yield, which can be achieved by setting *process* = `0R[L]`. 

+ W+jets estimate includes all MC events, except those where none of the tight leptons is truth-matched to a prompt lepton. Then the data-driven fake background estimate should estimate both the QCD yield and the W+jets yields where the prompt lepton is not reconstructed as a tight lepton. Then one can set *process* = `0R`. 

+ W+jets estimate includes only MC events that have no tight fake lepton: *process* = `>=1F[T]`

+ W+jets estimate includes only MC events that have no loose fake lepton: *process* = `>=1F[L]`

(note: to be strictly accurate, some of these cases assume that the prompt lepton from the W decay is at least reconstructed as a loose lepton).

