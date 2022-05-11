# ROOT input format

The real and fake/non-prompt lepton efficiencies required by the tools may be imported from histograms (TH1, TH2 or TH3) stored in ROOT files. The tool can understand on its own simple parametrizations and histograms following the naming convention detailed in the _Default format_ section below. Otherwise, it is necessary to provide an XML file describing the contents of the ROOT file(s) (see section _Custom format_). 

## Default format

A default ROOT file may contain, for each type of efficiency / lepton flavour, a single histogram storing the central value and statistical uncertainties, as well as extra histograms storing systematic uncertainties. 

### Central values

Histograms must be named according to the following convention that defines sequentially the type of efficiency, the type of lepton (electron,  muon or tau), and the parameter(s) corresponding to the X (and Y, Z) axis of the histogram:

```
{RealEfficiency | FakeEfficiency | FakeFactor}_{el | mu | tau}_paramX for 1D histograms
{RealEfficiency | FakeEfficiency | FakeFactor}2D_{el | mu | tau}_paramX_paramY for 2D histograms
{RealEfficiency | FakeEfficiency | FakeFactor}3D_{el | mu | tau}_paramX_paramY_paramZ for 3D histograms
```

Note that the parameter names _paramX_, _paramY_, _paramZ_ are not allowed to contain underscores. `pt, eta, phi` are predefined and read directly from the _IParticle_ object; for other parameters, it is assumed that a _float_ decoration of the _IParticle_ objects will be found with the same name. If a better control of the parameter type/scope is needed, it is possible to load a minimal XML file beforehand that only contains the declarations of the parameters. 

The histogram boundaries are always extended to $`\pm\infty`$; i.e. efficiencies from the first bin are used for underflows, and from the last bin for overflows. Note that this is a different behaviour than for XML files, where it is up to the user to specify the correct range. 

Finally, for parametrizations involving `eta`, the absolute value is used automatically if the histogram only covers the range eta >= 0.


### Statistical uncertainties

These are read from the bin errors of the histograms containing the central values. They are treated as uncorrelated between different bins. 

### Systematic uncertainties

Additional uncertainties can be provided via extra histograms using the same name as the histogram storing the central value, suffixed by a double underscore and the desired name of the systematic uncertainty. For example:

```
FakeEfficiency_el_pt
FakeEfficiency_el_pt__Bias
FakeEfficiency_el_pt__PromptSubtr
FakeEfficiency_el_pt__Trig
```

Two formats are accepted for the uncertainties:
 * Set the histogram bin contents to correspond to the uncertainty only (not the central value plus or minus the uncertainty). Errors in each bin should be set to zero to avoid ambiguities.
 * Set the histograms to have the same bin content as the nominal, and the uncertainty is encoded in the err in each bin.

Histograms containing nominal values and uncertainties must have identical binnings. Only symmetric uncertainties are supported for the moment (therefore if asymmetric uncertainties are needed, one must switch to using an XML file).
If systematic uncertainties with identical names affect different types of efficiencies, they will be treated as correlated, even if they were predefined in an XML file beforehand (in fact doing the latter is really not recommended, it will mess things up). 


## Custom format

XML file(s) must be provided with the structure detailed [here](./import_xml.md), including the declaration of parameters and uncertainties. Importing histograms can then be done simply by replacing the `<bin>` or `<table>` markups by `<TH1>` markups. The _particle elements_ must be embedded between `<ROOT>` markups indicating the source ROOT file where the histograms can be found (the _source_ attribute is mandatory):

##### example for one-dimensional histogram
#
```xml
<ROOT source="data/fakeEff_electron.root">
  <electron type="fake-efficiency" input="central_value">
    <TH1 nbjets="[0,2]" X="pt> h_El_Pt_nBjets0to1 </TH1>
    <TH1 nbjets="[2,inf]" X="pt> h_El_Pt_nBjets2 </TH1>
  </electron>
</ROOT>
```
##### example for two-dimensional histogram
#
```xml
<ROOT source="data/fakeEff_electron.root">
  <electron type="fake-efficiency" input="central_value">
    <TH1 nbjets="[0,2]" X="pt" Y="|eta|"> h_El_Pt_Eta_nBjets0to1 </TH1>
    <TH1 nbjets="[2,inf]" X="pt" Y="|eta|"> h_El_Pt_Eta_nBjets2 </TH1>
  </electron>
</ROOT>
```

Note that is perfectly possible to mix imported histograms and tabulated values within a single file. This can be used for example to quickly add / change systematic uncertainties. 

### Loading histograms

The `X, Y, Z` attributes of the `<TH1>` markup indicate which parameters the histogram axes correspond to. There is no need to specify the binnings, since they are read from the histogram. Axis boundaries are extended as well to $`\pm 1\infty`$, as described above for the default format. 

Similarly to the `<bin>` or `<table>` markups it is possible to specify extra cuts (see the example above that differentiates histograms based on the number of b-jets). 

Statistical uncertainties are read from the bin errors; the way they are interpreted is goverened by the `stat` attributes of the `<electron>`, `<muon>` and `<tau>` markdowns. 

### Normalizing histograms

It may be useful to rescale input histograms, for example to convert measured efficiencies into correction factors (needed when factorizing multi-dimensional efficiencies into 1D-dependencies). This can be down via the optional `norm` attribute of the `<TH1>` markdown. There are three options:

+ `norm = "auto"`: the values are divided by the weighted average of the histogram bins, computed according to the bin errors.
+ `norm = "label-name"`: the values are divided by a reference value defined earlier in the XML file by labelling it _label-name_ (see below)
+ `norm = "none"` (default): the values are unchanged

Reference values can be flagged via the `label` attribute of the `<bin>` and `<TH1>` markups (but not `<table>`):

```xml
<bin label="avg-el"> 0.3 +- 0.1 </bin>
<TH1 label="avg-mu" X="pt"> h_Mu_pt </TH1>
<!-- and later the correction factor: -->
<TH1 norm="avg-el"> h_El_dRJet </bin>
```

If the labelled reference TH1 has more than one bin, the reference value is again taken as the weighted average over the bins. 

When normalizing an histogram, the uncertainties in the reference value are ignored; i.e. the statistical uncertainties for the normalized histogram are simply taken as the original bin errors divided by the reference value. 

### Systematic uncertainties

For the moment there is no support for loading systematic uncertainties from custom ROOT files (only when using the default format), they must be tabulated inside the XML file. 
