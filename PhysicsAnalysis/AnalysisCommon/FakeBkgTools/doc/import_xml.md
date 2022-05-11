# XML input format

If a more flexible parametrization of the efficiencies and uncertainties is required, XML files can be used as input instead of ROOT files (or in addition to). The full supported format is described below, with the exception of the few additions used to interact with ROOT files, which are detailed [elsewhere](./import_root.md). 


## General overview of the document

The whole contents of the document should be enclosed within a single root `<efficiencies> ... </efficiencies>` element. The general structure should follow this outline:

```xml
<efficiencies>
	<!-- definitions of parameters and sources of systematic uncertainties -->
	<param type="int" level="event"> nBjets </param>
	<syst affects="fake-efficiency"> bias </syst>
	
	<!-- electron fake factor -->
	<electron type="fake-factor" input="central-value"> <bin> 0.05 +- 0.01 (stat) +- 20% (bias) </bin> </electron>
	
	<!-- etc -->
</efficiencies>
```

Until the current custom parser is replaced by a common library, please strip XML declarations off the document, as they won't be recognized. 


## Parameters definition

Variables used in the parametrization of the efficiencies must be defined before being used. This can be done with the following declarations:

```xml
<param type="float" level="particle"> dRClosestJet </param>
<param type="int" level="event"> njets, nBjets </param>
```

These variables are then read from decorations (or the `custom` field in the standalone version) of the objects passed to the `addEvent()` function. The parameter name and decoration name must therefore be identical. 

Both attributes are mandatory. The following values are supported:

+ `type = "int", "float"`
+ `level = "particle", "event"`: the parameter is to be read respectively from the _IParticle_ or the _EventInfo_ object decorations 

The following parameters are predefined and do not need to be declared:`pt, eta, |eta|, phi`. They are read directly from the _IParticle_ object, and thus do not require extra decorations. 


## Systematic uncertainties definition

Similarly, systematic uncertainties must be be defined before being used. This can be done with the following declarations:

```xml
<syst affects="electron,muon"> closeByJet </syst>
<syst affects="fake-efficiency"> compo, prompt </syst>
```

The attribute is mandatory. The following values are supported:

+ `affects = "electron", "muon", "tau", "real-efficiency", "fake-efficiency", "fake-factor"` as well as combinations of the latter such as `"electron-real-efficiency"`.

The attribute `affects` may contain several comma-separated values; in this case the affected categories will correspond to the union of the attribute values, not the intersection. For example, to declare a systematic uncertainty affecting only the electron real efficiency, one should specify `"electron-real-efficiency"` and not `"electron, real-efficiency"`. 

If a source of uncertainty affects several categories, it will be treated as correlated between these categories. If this is unwanted, one should use instead separate declarations. In such a case, note that it is possible to use the same name in different declarations, as long as there is no overlap between the _affects_ attributes. For example:

```xml
<!-- 1) a single nuisance parameter, e and mu correlated: -->
<syst affects="electron,muon"> prompt </syst>
<!-- 2) two nuisance parameters, e and mu uncorrelated: --> 
<syst affects="electron"> trig </syst>
<syst affects="muon"> trig </syst>
```

Note that `stat`is a reserved keyword and consequently can't be used. 

## Tables of efficiencies

Efficiencies are tabulated between `<electron>`, `<muon>` or `<tau>` markups (refered to as _particle element_ in the following) defining the measurement type. 

```xml
<electron type="fake-factor" input="central-value">
  <bin pt="[20,inf]"> 0.20 +- 0.01 (stat) +- 0.05 (bias) </bin>
</electron>
```

The following attributes and values are supported:

+ `type = "real-efficiency", "fake-efficiency", "fake-factor"` (mandatory)
+ `input = "central-value", "correction"` (mandatory)
+ `stat = "global", "per-bin", "none"` (mandatory unless stat uncertainties not used)

More details are given in the following paragraphs. 

### Bins definition

Efficiencies are tabulated between `<bin>` or `<table>` markups, which define the bin boundaries. The former markup corresponds to a single one-entry bin, while the latter defines a multi-dimensional binned set of values. One may define several bins and/or tables inside a particle element. 

Bin boundaries are indicated as attributes, using predefined or declared parameters:

```xml
<electron type="fake-factor" input="central-value">
  <bin nbjet="[1,inf]" pt="[20,inf]">
    0.20 +- 0.01 (stat) +- 0.05 (bias)
  </bin>
  <table nbjet="[1,inf]" pt="[10, 12, 15,20]" |eta|="[0,1.5,2.5]">
    <!-- |eta|=0-1.5 -->              <!-- |eta| = 1.5-2.5 -->
    0.10+-0.01(stat)+-0.01(bias)   0.11+-0.01(stat)+-0.01 (bias) <!-- 10-12 GeV -->
    0.12+-0.02(stat)+-0.02(bias)   0.13+-0.02(stat)+-0.02 (bias) <!-- 12-15 GeV -->
    0.15+-0.03(stat)+-0.03(bias)   0.16+-0.03(stat)+-0.03 (bias) <!-- 15-20 GeV -->
  </table>
</electron>
```

There are no restrictions on the number of parameter attributes or the dimension/number of bins of the tables.  

The keywords `inf` / `-inf` can be used to denote positive/negative infinity. The bin boundaries are (like in a ROOT TH1) inclusive on the lower edge and exclusive on the upper edge, which means e.g. that `njets="[2,4,6]"` defines two bins accepting respectively events with 2,3 and 4,5 jets. The pt bin boundaries are interpreted either in MeV or GeV according to the *EnergyUnit* tool property. For other dimensionful parameters one must ensure that the same unit is used in the XML file and for the object decorations. 

For `table` markups, the values must be tabulated in the same order as they would be for the initialization of a multidimensional C++ static array. New lines separating rows of values for clarity (as in the example above) are ignored. 

For integer parameters, if the bin corresponds to a single value of the parameter one can simply write `param="val"` instead of `param="[val,val+1]"`: 

```xml
<bin nbjet="0" pt="[20,inf]"> ... </bin>
<bin nbjet="1" pt="[20,inf]"> ... </bin>
<bin nbjet="[2,inf]" pt="[20,inf]"> ... </bin>
```

### Format of tabulated values

Efficiencies are tabulated as a central value followed by the relevant uncertainties:
```xml
<bin pt="[20,inf]">
  0.20 +- 0.01 (stat) +- 0.05 (bias) +- 0.01 (trig)
</bin>
```

The name of the systematic uncertainty (or the keyword `stat`) must always be between parenthesis and correspond to one of the predefined systematics. The list of specified uncertainties (incluing stat) does not need to be consistent between different bins / values (e.g. one may omit to write uncertainties that are 0 in that bin).

Uncertainties are specified as absolute values, unless they are explicitly written in percents:
```xml
<bin pt="[20,inf]">
  0.20 +- 0.01 (stat) +- 25% (bias) +- 5% (trig)
</bin>
```

Asymmetric uncertainties are supported as well, by splitting the `+-` keyword into separate components:
```xml
<bin pt="[20,inf]">
  0.20 +- 0.01 (stat) +0.05-0.02 (bias) +0.01-0.005 (trig)
</bin>
```

Finally, it is possible to anti-correlate uncertainties between bins or elements, by swapping the + and - order:
```xml
<table pt="[10,20,inf]">
  0.10 +- 0.01 (stat) +0.05-0.02 (bias) +- 0.01 (trig)
  0.20 +- 0.01 (stat) -0.05+0.02 (bias) -+ 0.01 (trig) <!-- bias and trig anti-correlated with 10-20 bin -->
</table>
```

### Central value or correction?

The tabulated values inside a particle element may correspond either to the central values of the efficiencies, or to correction factors that are multiplicatively combined with the central values. This is controlled by the `input` attribute. 

Typical usages of correction factors:
+ extrapolation of the efficiencies to a different phase space by applying MC-based correction to the inclusive data measurements
+ factorization of multi-dimensional efficiency into 1D-dependencies $`\varepsilon(x,y,\ldots) = (\varepsilon(x)\times\varepsilon(y)\times\ldots)/(\langle\varepsilon\rangle^{n})`$

Correction factors can use different binnings (including different parameters) than the central values, and one can define as many (overlapping) correction factors as needed. For central values however, all the bins must be orthogonal, so that there is no ambiguity in retrieving the efficiency for a given particle. 

### Statistical uncertainties

There are two options for the correlation scheme inside a single particle element, controlled by the `stat` attribute. Either all bins are correlated and a single nuisance parameter is used (`stat="global"`), or all bins are uncorrelated (`stat="per-bin"`) in which case there will be one nuisance parameter per bin. 

Statistical uncertainties are always treated as uncorrelated between two different particle elements, even if they have identical types. Therefore if correlation is needed, one should either simply work with systematic uncertainties instead, or try to define all bins/tables inside a single particle element. 

The `stat="none"` attribute is intended for use only when importing histograms from a ROOT file, to force disabling statistical uncertainties even if the histograms have non-zero bin errors. 
