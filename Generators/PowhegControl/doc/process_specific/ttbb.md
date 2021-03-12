# Process-specific information for $`\mathrm{t}\bar{\mathrm{t}}\mathrm{b}\bar{\mathrm{b}}`$ production

## Physics configuration

### Decay modes (`decay_mode`)

In PowhegControl, we have harmonised the way that decay modes are given by introducing the parameter `PowhegConfig.decay_mode` which accepts a MadGraph-inspired decay syntax. For this process, the possibilities are:

```
t t~ > all
t t~ > b j j b~ j j
t t~ > b l+ vl b~ l- vl~
t t~ > b emu+ vemu b~ emu- vemu~
t t~ > semileptonic
t t~ > undecayed
t t~ > all [MadSpin]
```

The native Powheg decays already **include spin correlations**, as described at the bottom of page 21 of [1802.00426](https://arxiv.org/abs/1802.00426). In addition, we have implemented an interface to MadSpin that also allows you to decay the top quarks (via any channel, let us know if you need more choices).
