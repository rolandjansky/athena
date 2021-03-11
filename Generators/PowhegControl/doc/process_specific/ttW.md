# Process-specific information for $`\mathrm{t}\bar{\mathrm{t}}\mathrm{W}`$ production

## Physics configuration


### Particle decays

The decay mode(s) may be given via the parameter `PowhegConfig.decay_mode`, which may take one of the strings given below as its value.

For $`\mathrm{t}\bar{\mathrm{t}}\mathrm{W}^+`$:

```py
't t~ W+ > all'                       # all possible decays are allowed
't t~ W+ > b j j b~ j j j j'          # all-hadronic
't t~ W+ > b l+ vl b~ l- vl~ l+ vl'   # "all-leptonic"
't t~ W+ > b l+ vl b~ j j l+ vl'      # exactly two positive-sign leptons
't t~ W+ > undecayed'                 # stable resonances
```

For $`\mathrm{t}\bar{\mathrm{t}}\mathrm{W}^-`$:

```py
't t~ W- > all'                       # all possible decays are allowed
't t~ W- > b j j b~ j j j j'          # all-hadronic
't t~ W- > b l+ vl b~ l- vl~ l- vl~'  # "all-leptonic"
't t~ W- > b j j vl b~ l- vl~ l- vl~' # exactly two negative-sign leptons
't t~ W- > undecayed'                 # stable resonances
```

If you need more fine-grained control over what decays are allowed, please contact the PowhegControl maintainers and we'll see what we can do!

Note: Powheg's native parameters `topdecmode` and `samesignleptons` for the process are set automatically based on the value provided to `PowhegConfig.decay_mode`.

The **total branching ratio of W to leptons** (including τ leptons) may be given as a float via the parameter `PowhegConfig.BR_W_to_leptons` (internal name in Powheg: `BrWlep`).


**Correlations of the decays of the top and antitop quark** may be turned on or off by setting `PowhegConfig.correlate_top_decays` (`1`: on, `0`: off). Note that this parameter corresponds to Powheg's native parameter `correlate`, we've just given it a more descriptive alias in PowhegControl.


### Choice of renormalisation and factorisation scale

The following scale choices are supported by setting the parameter `PowhegConfig.scalechoice` accordingly:

`0`: $`\mu_{\mathrm{R}} = \mu_{\mathrm{F}} = m_{\mathrm{t}} + m_{\mathrm{W}}/2`$ (default)

`1`: $`\mu_{\mathrm{R}} = \mu_{\mathrm{F}} = [ m_{\mathrm{T}}(\mathrm{W}) * m_{\mathrm{T}}(\mathrm{t}) * m_{\mathrm{T}}(\bar{\mathrm{t}}) ]^{(1/3)}`$

`2`: $`\mu_{\mathrm{R}} = \mu_{\mathrm{F}} = H_{\mathrm{T}} / 2`$, with $`H_{\mathrm{T}} = \sum_i m_{\mathrm{T}}(i)`$ , where $`m_{\mathrm{T}}(i)`$ is the transverse mass of outgoing particle $`i`$


### Dynamic $`h_{\mathrm{damp}}`$

Instead of a fixed $`h_{\mathrm{damp}}`$ scale, a dynamic scale may be used by setting `PowhegConfig.dynamic_hdamp = 1`. (The value of `PowhegConfig.hdamp` is ignored in that case.) A prefactor $`f`$ may be given via the argument `PowhegConfig.dampscfact = <float>`. The dynamic scale is given by

$`h_{\mathrm{damp}} = f \times H_{\mathrm{T}} / 2,`$

with $`H_{\mathrm{T}} = \sum_i m_{\mathrm{T}}(i)`$ , where $`m_{\mathrm{T}}(i)`$ is the transverse mass of outgoing particle $`i`$.

### Born zero damping

`hbzd`: $`\theta[h_{\mathrm{bzd}} - R/(P_{ij} B)]`$ Born zero damping. Set to 5 by default by the process authors.