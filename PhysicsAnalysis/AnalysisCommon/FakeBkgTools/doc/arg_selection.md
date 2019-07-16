# The *selection* argument

This argument allows the user to specify the definition of the region of interest, in terms of how many (tight) leptons are selected, and how extra leptons are treated (vetoed or not). This must describe **the way events are selected in the main analysis** (observed data, MC for processes with prompt leptons, signal of interest), while for the fake-lepton background estimate the tools figure out by themselves which events are relevant (given the target *selection*) as long as the function `addEvent()` is called for all events with sufficiently many loose leptons. 

The following sections describe the recognized syntax for this argument, with some examples following.
**Note that some of the methods only support a subset of the recognized options.** 


## Recognized syntax

+ Requirements on the number of tight leptons (`T`) and/or anti-tight leptons (`!T`)

+ Upper/lower bound on the number of leptons: `>1T`, `>=4T`, `<1!T`, `<=3T`

+ Exact number of leptons: `0!T`, `=2T`

+ Range: `1-3T`, `=0-5!T`

+ Lepton charge requirements: `OS`, `SS`. The former requires the presence of at least one pair of opposite-sign tight leptons, the latter the presence of at least one pair of same-sign tight leptons. Note that, unless combined with a `=2T` condition, these requirements are not orthogonal and may be satisfied simultaneously. 

+ Explicit list of all combinations relevant to the region of interest: e.g. in a trilepton event `TTT + T!TT + TT!T`, `TTT`, `!TTT + !TT!T + !T!TT`... All tokens composing the string must have the same size (i.e. `T + TT + TTT` is not allowed), which must equal the number of particles specified to `addEvent()`. See the examples with "lepton-based cuts". 

+ Combinations of any number of the above: e.g. `>=2T, 0!T, OS`. In such cases, the resulting selection is the intersection of all the clauses, not their union (i.e. the comma is a logical AND). 

+ Aliases: not yet implemented. 

## Examples

### Simple scenarios: one-lepton selection

+ exclusive (veto all extra **loose** leptons): `1T, 0!T` (`1T` sufficient if the tool is never called for events with >=2 loose leptons)

+ exclusive (veto all extra **tight** leptons): `1T`

+ inclusive (no veto of extra leptons): `>=1T`

+ inclusive but leading lepton must be tight: `T` for events with 1 lepton, `TT + T!T` for events with 2 leptons, `TTT+TT!T+T!TT+T!T!T` for events with 3 leptons...

### When the region of interest involves lepton-based cuts

**Note that the following only works for methods than inherit from BaseLinearFakeBkgTool (meaning not for the likelihood matrix method)**

For example the selection is based on $`H_{T} = \sum p_{T}^{\ell}`$, where the sum includes only tight leptons. In these cases, the selection argument must generally be changed from one event to the other (typically by, in each case, specifying all the relevant combinations). 

+ one-lepton inclusive; region of interest has a cut HT > 100 GeV: 
    - 1st event has leptons pT = 50 / 40 / 30 GeV: sel. = `TTT` (or `3T`, `0!T`, etc)
    - 2nd event has leptons pT = 120 / 10 GeV: sel. = `TT + T!T`
    - 3rd event has leptons pT = 120 / 60 / 50 GeV: sel. = `TTT + TT!T + T!TT + T!T!T + !TTT`

+ one-lepton inclusive; plotting HT (bin boundaries 0-100, 100-200, >200 GeV):
    - 1st event has leptons pT = 50 / 40 / 30 GeV:
        + fill 1st bin with getEventWeight(`T!T!T + TT!T + T!TT + !TTT + !TT!T + !T!TT`) (or `1-2T`)
        + fill 2nd bin with getEventWeight(`TTT`)
        + 3rd bin not filled
    - 2nd event has leptons pT = 120 / 10 GeV:
        + fill 1st bin with getEventWeight(`!TT`)
        + fill 2nd bin with getEventWeight(`TT + T!T`)
        + 3rd bin not filled
    - 3rd event has leptons pT = 120 / 60 / 50 GeV:
        + fill 1st bin with getEventWeight(`!TT!T + !T!TT`)
        + fill 2nd bin with getEventWeight(`T!T!T + T!TT + TT!T + !TTT + !TT!T + !T!TT`) (or...)
        + fill 2nd bin with getEventWeight(`TTT`) (or...)
