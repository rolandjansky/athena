# Process-specific information for γ+jet production ("direct photon")

## Overview

This process is direct photon production, defined as having the leading-order processes

$`q\bar{q} \to \gamma \mathrm{g}`$
$`q\mathrm{g} \to \gamma q`$
$`\bar{q}\mathrm{g} \to \gamma \bar{q}`$


The publication is [1610.02275](https://arxiv.org/abs/1610.02275). Please cite it in addition to the usual Powheg publications if using this process.

For a study using this process, see [1709.04154](https://arxiv.org/abs/1709.04154).



## Physics configuration

### `enhanceradfac`

The enhancement of photon radiation is described on page 137 and following of [the PhD thesis of Florian König](https://www.uni-muenster.de/imperia/md/content/physik_tp/theses/klasen/koenig_phd.pdf).

Note: for technical reasons in the PowhegControl implementation, you _have_ to set `enhanceradfac` to a positive value, i.e. enable it! If you do not want to actually enhance photon radiation set it to `1` (rather than completely disabling the functionality by setting it to `-1`).


### `bornsuppfact`

According to the authors' example located at (e.g.)
`/cvmfs/atlas.cern.ch/repo/sw/Generators/powheg/ATLASOTF-00-04-02/POWHEG-BOX-V2/directphoton/testrun-lhc/powheg.input-save`
`bornsuppfact = 4000` is used "to get photons over a wide range of 100 GeV to over 1 TeV, not for low energy photons"



---

## Technical information for developers

To make enhancing the rate of photon emissions work and get physically correct cross sections, the events need to be weighted by an additional Sudakov weight. The way that this is technically solved (by Tomáš Ježo at our request) is that we incorporate the Sudakov weight into the first weight in the `rwgt` list in the LHE file. If this first weight is not written out, the Sudakov weight is lost. No recalculation later is possible.

Here's an example of how this looks, from Tomáš.

This is what an event looks like during the first stage:

```xml
<event>
      5  10001  5.32393E+01  2.58149E+02  7.81653E-03  1.07756E-01
       2    -1     0     0   511     0  0.000000000E+00  0.000000000E+00  1.936444170E+03  1.936444170E+03  0.000000000E+00  0.00000E+00  9.000E+00
       2    -1     0     0   512     0  0.000000000E+00  0.000000000E+00 -2.265099108E+03  2.265099108E+03  0.000000000E+00  0.00000E+00  9.000E+00
       2     1     1     2   511     0 -1.069028426E+03  1.699031663E+02 -1.954610850E+03  2.234321471E+03  4.315837288E-05  0.00000E+00  9.000E+00
      22     1     1     2     0     0  5.099857123E+02  3.578589455E+01  7.236542676E+02  8.860257084E+02  0.000000000E+00  0.00000E+00  9.000E+00
       2     1     1     2   512     0  5.590427139E+02 -2.056890608E+02  9.023016439E+02  1.081196098E+03  3.051757812E-05  0.00000E+00  9.000E+00
#rwgt            1          97   1.0134670173666233E-002           1          27           0
<rwgt>
<wgt id='0'>0.25383E-01</wgt>
</rwgt>
<scales uborns=' 0.108582116E+04'> </scales>
</event>
```

Note that the bare weight (third number in the first line after the `<event>` tag) and the `<wgt id='0'> `weights are different. Their ratio is the Sudakov weight:

```
sudakovwgt = 0.25383E-01 / 5.32393e+01 = 0.476772280E-03
```

Note: before Tomáš implemented multiplying the Sudakov weight into the first weight, the event looked like this:

```xml
<event>
      5  10001  5.32393E+01  2.58149E+02  7.81653E-03  1.07756E-01
       2    -1     0     0   511     0  0.000000000E+00  0.000000000E+00  1.936444170E+03  1.936444170E+03  0.000000000E+00  0.00000E+00  9.000E+00
       2    -1     0     0   512     0  0.000000000E+00  0.000000000E+00 -2.265099108E+03  2.265099108E+03  0.000000000E+00  0.00000E+00  9.000E+00
       2     1     1     2   511     0 -1.069028426E+03  1.699031663E+02 -1.954610850E+03  2.234321471E+03  4.315837288E-05  0.00000E+00  9.000E+00
      22     1     1     2     0     0  5.099857123E+02  3.578589455E+01  7.236542676E+02  8.860257084E+02  0.000000000E+00  0.00000E+00  9.000E+00
       2     1     1     2   512     0  5.590427139E+02 -2.056890608E+02  9.023016439E+02  1.081196098E+03  3.051757812E-05  0.00000E+00  9.000E+00
#rwgt            1          97   1.0134670173666233E-002           1          27           0
<rwgt>
<wgt id='central'>  0.532393343E+02 </wgt>
<wgt id='sudakovwgt'>  0.476772280E-03 </wgt>
</rwgt>
<scales uborns=' 0.108582116E+04'> </scales>
</event>
```

where instead the `<wgt id='central'>` is equal to the bare event weight and `sudakovwgt` is listed separately.

In ATLAS, we then usually want to add more weights, corresponding to PDF and QCD scale variations. This works by adding the following to the `powheg.input` as usual:

```bash
rwl_add 1
rwl_file 'reweighting_input.xml'
```

This yields:

```xml
<event>
      5  10001  5.32393E+01  2.58149E+02  7.81653E-03  1.07756E-01
       2    -1     0     0   511     0  0.000000000E+00  0.000000000E+00  1.936444170E+03  1.936444170E+03  0.000000000E+00  0.00000E+00  9.000E+00
       2    -1     0     0   512     0  0.000000000E+00  0.000000000E+00 -2.265099108E+03  2.265099108E+03  0.000000000E+00  0.00000E+00  9.000E+00
       2     1     1     2   511     0 -1.069028426E+03  1.699031663E+02 -1.954610850E+03  2.234321471E+03  4.315837288E-05  0.00000E+00  9.000E+00
      22     1     1     2     0     0  5.099857123E+02  3.578589455E+01  7.236542676E+02  8.860257084E+02  0.000000000E+00  0.00000E+00  9.000E+00
       2     1     1     2   512     0  5.590427139E+02 -2.056890608E+02  9.023016439E+02  1.081196098E+03  3.051757812E-05  0.00000E+00  9.000E+00
#rwgt            1          97   1.0134670173666233E-002           1          27           0
<rwgt>
<wgt id='0'>0.25383E-01</wgt>
<wgt id='1001'>0.24775E-01</wgt>
<wgt id='2001'>0.25861E-01</wgt>
</rwgt>
<scales uborns=' 0.108582116E+04'> </scales>
</event>
```

Note that `<wgt id='1001'>` and `<wgt id='2001'>` have already been multiplied by the Sudakov weight, as they should!

So everything is sort of seamless as long as the first run is made with at least one `<rwgt>` weight!
