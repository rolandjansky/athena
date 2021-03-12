# Sytematic variations

## Running with multiple scale/PDF weights (V2 processes only)

In POWHEG version 2, support was enabled for multiple weights to be
stored per event, representing multiple scale variations or PDFs. To
enable this behaviour, simply provide a python list to the `PDF` and/or
the `mu_R` and `mu_F` properties of `PowhegConfig`. For example

```py
include('PowhegControl/PowhegControl_WZ_Common.py')
PowhegConfig.PDF = range(10800, 10853)
PowhegConfig.mu_F = [1.0, 0.5, 0.5, 0.5, 2.0, 2.0, 2.0]
PowhegConfig.mu_R = [1.0, 0.5, 1.0, 2.0, 0.5, 1.0, 2.0]
PowhegConfig.generate()
```

The desired nominal values **MUST** be first in each list. If using
`mu_F` and `mu_R`, the two lists **MUST** be the same length.

## Running with multiple non-scale/PDF weights (V2 processes only)

Newer Powheg processes are able to re-run with other parameters changed,
giving additional weights as output in the same way as for scale/PDF
variations. The first thing to do is to define one or more groups of
event weights, each one needs a name and a list of the parameters that
will be altered, which can include scale/PDF parameters if desired.

In the example below, a weight group called `quark_mass_variation` is
defined, which consists of a series of variations of the
`bmass_in_minlo` and `quarkmasseffects` parameters

```py
PowhegConfig.define_event_weight_group(group_name='quark_mass_variation', parameters_to_vary=['bmass_in_minlo', 'quarkmasseffects'])
```


Next, one output weight must be defined for each set of parameter
variations. The name of the group to which this belongs **MUST** be
provided, along with the name of the weight and a value for each of the
parameters specified in the group definition. In the example below,
three weights are defined, one enables `quarkmasseffects` only, one
which disables both `bmass_in_minlo` and `quarkmasseffects` and one
which enables both. Other combinations are of course possible.

```py
PowhegConfig.add_weight_to_group(group_name='quark_mass_variation', weight_name='mtmb', parameter_values=[0, 1])
PowhegConfig.add_weight_to_group(group_name='quark_mass_variation', weight_name='mtinf', parameter_values=[0, 0])
PowhegConfig.add_weight_to_group(group_name='quark_mass_variation', weight_name='mtmb-bminlo', parameter_values=[1, 1])
```

The number of parameter values provided **MUST** be equal to the number
specified in the group definition or generation will terminate! The
example above will produce the following weights:

| Weight name   | PowhegControl options                        |
| :------------ | :------------------------------------------- |
| `mtmb`        | `bmass_in_minlo = 0`, `quarkmasseffects = 1` |
| `mtinf`       | `bmass_in_minlo = 0`, `quarkmasseffects = 0` |
| `mtmb-bminlo` | `bmass_in_minlo = 1`, `quarkmasseffects = 1` |

Please bear in mind that you cannot use the reweighting mechanism with
parameters related to the real cross-section, such as `hdamp`.
Reweighting is only applied to the inclusive cross section (what Powheg
calls the Bbar function), while the hardest radiation, built on top of
it, is left as is. Since the `hdamp` parameter selects the part of the
real cross section to be used for generating radiation, it cannot be
reweighted.
