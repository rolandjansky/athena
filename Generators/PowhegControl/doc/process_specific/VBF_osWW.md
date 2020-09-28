# Process-specific information for vector-boson scattering $`W^+W^-`$ production

## Overview

Opposite-sign W-boson pair plus jet pair production through vector boson fusion. Can generate resonant Higgs production diagrams and/or WW continuum diagrams.

## Physics configuration

### Resonant vs. continuum diagrams (`ww_res_type`)

Whether to generate resonant (H → WW -like) events or WW continuum is controlled with the `ww_res_type` parameter as follows.

In the presence of a very sharp resonance, as is the case in a scenario with a light Higgs boson with a mass below 200 GeV, the resonant Higgs contribution has to be evaluated separately from the WW continuum, as described in [1301.1695](https://arxiv.org/abs/1301.1695). In this case the entire event generation chain has to be performed for each of these two contributions separately in two distinct working directories, setting

```py
PowhegConfig.ww_res_type = 1
```

for the WW continuum and

```py
PowhegConfig.ww_res_type = 2
```

for the Higgs resonance contribution. After all the runs have been performed for each case, the results have to be added.

If the Higgs is heavy and has large width, such a splitting is not necessary, and all contributions can be evaluated at the same time, setting

```py
PowhegConfig.ww_res_type = 0
```

### Decay modes (`decay_mode`)

In the native interface, these are specified via the keywords `vdecaymodeWp` and `vdecaymodeWm`. In PowhegControl, we have harmonised the way that decay modes are given by introducing the parameter `PowhegConfig.decay_mode` which accepts a MadGraph-inspired decay syntax. For this process, the possibilities are:

```
w+ w- > e+ ve e- ve~
w+ w- > e+ ve mu- vm~
w+ w- > e+ ve tau- vt~
w+ w- > e+ ve d u~
w+ w- > e+ ve s c~
w+ w- > e+ ve (d u~ / s c~)
w+ w- > mu+ vm e- ve~
w+ w- > mu+ vm mu- vm~
w+ w- > mu+ vm tau- vt~
w+ w- > mu+ vm d u~
w+ w- > mu+ vm s c~
w+ w- > mu+ vm (d u~ / s c~)
w+ w- > tau+ vt e- ve~
w+ w- > tau+ vt mu- vm~
w+ w- > tau+ vt tau- vt~
w+ w- > tau+ vt d u~
w+ w- > tau+ vt s c~
w+ w- > tau+ vt (d u~ / s c~)
w+ w- > u d~ e- ve~
w+ w- > u d~ mu- vm~
w+ w- > u d~ tau- vt~
w+ w- > u d~ d u~
w+ w- > u d~ s c~
w+ w- > u d~ (d u~ / s c~)
w+ w- > c s~ e- ve~
w+ w- > c s~ mu- vm~
w+ w- > c s~ tau- vt~
w+ w- > c s~ d u~
w+ w- > c s~ s c~
w+ w- > c s~ (d u~ / s c~)
w+ w- > (u d~ / c s~) e- ve~
w+ w- > (u d~ / c s~) mu- vm~
w+ w- > (u d~ / c s~) tau- vt~
w+ w- > (u d~ / c s~) d u~
w+ w- > (u d~ / c s~) s c~
w+ w- > (u d~ / c s~) (d u~ / s c~)
```

You can generate the above list in Python as follows:

```py
import itertools
wm_decay_products = ["e- ve~", "mu- vm~", "tau- vt~", "d u~", "s c~", "(d u~ / s c~)"]
wp_decay_products = ["e+ ve", "mu+ vm", "tau+ vt", "u d~", "c s~", "(u d~ / c s~)"]
decays = ["w+ w- > {} {}".format(*x) for x in itertools.product(wp_decay_products, wm_decay_products)]
for decay in decays:
    print(decay)
```

PowhegControl then sets `vdecaymodeWp` and `vdecaymodeWm` internally accordingly.


### Resonance masses and widths

You can use the parameters `hmass`, `hwidth`,`wmass`, and `wwidth` to set the Higgs and W boson masses and widths.

### Fat jets

The process-specific "fat jet" parameter is **not currently implemented** in PowhegControl. If you need it, please contact the PowhegControl maintainers.
