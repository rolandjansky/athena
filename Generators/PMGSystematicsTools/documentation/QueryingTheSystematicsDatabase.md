# Using the systematics database

Table of Contents:
- [Seeing what weights are available for a given DSID, and how they should be combined](#seeing-what-weights-are-available-for-a-given-dsid-and-how-they-should-be-combined)
<br/>

# Seeing what weights are available for a given DSID, and how they should be combined

For this task, you can use the `readDatabase.py` script which queries `DSID_Database.yaml` and `Weight_Database.yaml` to extract the information.
The `getWeights()` function of `readDatabase.py` can be imported in any script and used to retrieve the keys or the full dictionary given the input is the DSID of the sample.
This can either be used on the command line or imported into your favourite `python` script:

- *Command line*: Using the `readDatabase.py`, one can print the container list and the nested dictionary containing the weights in the EVNT sample grouped into separate containers with information about the combination recipe, type and nominal of the container,
```
 readDatabase.py --d XXXXXX 
```
where the XXXXXX is the 6 digit DSID.

- *In a script*: The `getWeights()` function of `readDatabase` can be imported and used to retrieve the nested dictionary and the key for weight containers for the DSID.
```
 import readDatabase
 dictionary, key = readDatabase.getWeights(dsid)
```
where `dsid` is the 6 digit DSID whose dictionary needs to be retrieved.

In both cases, if one does this for DISD 410503 for example, the output would be:

```
#list if variation types
['Powheg_ME_PDF260000_var', 'Powheg_ME_PDF90900_Hessian', 'Powheg_ME_PDF25200_scale', 'Powheg_ME_PDF260000_scale', 'Powheg_PDF260000_Nominal', 'Powheg_ME_PDF13165_scale', 'Powheg_ME_PDF90900_scale_type2']
#dictionary with combination recipes: for each entry in the above list, the nominal pdf, uncertainty type, name of the nominal weight, name of the weights needed for the comination, and finally the combination recipe itself, are stored.
{'Powheg_ME_PDF260000_var': {'nominal_pdf': '260000', 'type': 'PDF_ME', 'nominal': ' nominal ', 'weights': [' PDF set = 260001 ', ' PDF set = 260002 ', ' PDF set = 260003 ', ' PDF set = 260004 ', ' PDF set = 260005 ', ' PDF set = 260006 ', ' PDF set = 260007 ', ' PDF set = 260008 ', ' PDF set = 260009 ', ' PDF set = 260010 ', ' PDF set = 260011 ', ' PDF set = 260012 ', ' PDF set = 260013 ', ' PDF set = 260014 ', ' PDF set = 260015 ', ' PDF set = 260016 ', ' PDF set = 260017 ', ' PDF set = 260018 ', ' PDF set = 260019 ', ' PDF set = 260020 ', ' PDF set = 260021 ', ' PDF set = 260022 ', ' PDF set = 260023 ', ' PDF set = 260024 ', ' PDF set = 260025 ', ' PDF set = 260026 ', ' PDF set = 260027 ', ' PDF set = 260028 ', ' PDF set = 260029 ', ' PDF set = 260030 ', ' PDF set = 260031 ', ' PDF set = 260032 ', ' PDF set = 260033 ', ' PDF set = 260034 ', ' PDF set = 260035 ', ' PDF set = 260036 ', ' PDF set = 260037 ', ' PDF set = 260038 ', ' PDF set = 260039 ', ' PDF set = 260040 ', ' PDF set = 260041 ', ' PDF set = 260042 ', ' PDF set = 260043 ', ' PDF set = 260044 ', ' PDF set = 260045 ', ' PDF set = 260046 ', ' PDF set = 260047 ', ' PDF set = 260048 ', ' PDF set = 260049 ', ' PDF set = 260050 ', ' PDF set = 260051 ', ' PDF set = 260052 ', ' PDF set = 260053 ', ' PDF set = 260054 ', ' PDF set = 260055 ', ' PDF set = 260056 ', ' PDF set = 260057 ', ' PDF set = 260058 ', ' PDF set = 260059 ', ' PDF set = 260060 ', ' PDF set = 260061 ', ' PDF set = 260062 ', ' PDF set = 260063 ', ' PDF set = 260064 ', ' PDF set = 260065 ', ' PDF set = 260066 ', ' PDF set = 260067 ', ' PDF set = 260068 ', ' PDF set = 260069 ', ' PDF set = 260070 ', ' PDF set = 260071 ', ' PDF set = 260072 ', ' PDF set = 260073 ', ' PDF set = 260074 ', ' PDF set = 260075 ', ' PDF set = 260076 ', ' PDF set = 260077 ', ' PDF set = 260078 ', ' PDF set = 260079 ', ' PDF set = 260080 ', ' PDF set = 260081 ', ' PDF set = 260082 ', ' PDF set = 260083 ', ' PDF set = 260084 ', ' PDF set = 260085 ', ' PDF set = 260086 ', ' PDF set = 260087 ', ' PDF set = 260088 ', ' PDF set = 260089 ', ' PDF set = 260090 ', ' PDF set = 260091 ', ' PDF set = 260092 ', ' PDF set = 260093 ', ' PDF set = 260094 ', ' PDF set = 260095 ', ' PDF set = 260096 ', ' PDF set = 260097 ', ' PDF set = 260098 ', ' PDF set = 260099 ', ' PDF set = 260100 ', ' nominal '], 'combination': 'lhapdf'}, 'Powheg_ME_PDF90900_Hessian': {'nominal_pdf': '90900', 'type': 'PDF_ME', 'nominal': ' PDF set = 90900 ', 'weights': [' PDF set = 90900 ', ' PDF set = 90901 ', ' PDF set = 90902 ', ' PDF set = 90903 ', ' PDF set = 90904 ', ' PDF set = 90905 ', ' PDF set = 90906 ', ' PDF set = 90907 ', ' PDF set = 90908 ', ' PDF set = 90909 ', ' PDF set = 90910 ', ' PDF set = 90911 ', ' PDF set = 90912 ', ' PDF set = 90913 ', ' PDF set = 90914 ', ' PDF set = 90915 ', ' PDF set = 90916 ', ' PDF set = 90917 ', ' PDF set = 90918 ', ' PDF set = 90919 ', ' PDF set = 90920 ', ' PDF set = 90921 ', ' PDF set = 90922 ', ' PDF set = 90923 ', ' PDF set = 90924 ', ' PDF set = 90925 ', ' PDF set = 90926 ', ' PDF set = 90927 ', ' PDF set = 90928 ', ' PDF set = 90929 ', ' PDF set = 90930 '], 'combination': 'lhapdf'}, 'Powheg_PDF260000_Nominal': {'nominal_pdf': '260000', 'type': 'Nominal', 'nominal': ' nominal ', 'weights': [' nominal '], 'combination': 'none'}, 'Powheg_ME_PDF260000_scale': {'nominal_pdf': '260000', 'type': 'scale_ME', 'nominal': ' nominal ', 'weights': [' muR = 0.5, muF = 0.5 ', ' muR = 0.5, muF = 1.0 ', ' muR = 0.5, muF = 2.0 ', ' muR = 1.0, muF = 0.5 ', ' muR = 1.0, muF = 2.0 ', ' muR = 2.0, muF = 0.5 ', ' muR = 2.0, muF = 1.0 ', ' muR = 2.0, muF = 2.0 ', ' nominal '], 'combination': 'envelope'}, 'Powheg_ME_PDF25200_scale': {'nominal_pdf': '25200', 'type': 'Scale_ME', 'nominal': ' PDF set = 25200 ', 'weights': [' 0p5muF_0p5muR_MMHT ', ' 0p5muF_2muR_MMHT ', ' 0p5muF_MMHT ', ' 0p5muR_MMHT ', ' 2muF_0p5muR_MMHT ', ' 2muF_2muR_MMHT ', ' 2muF_MMHT ', ' 2muR_MMHT ', ' PDF set = 25200 '], 'combination': 'envelope'}, 'Powheg_ME_PDF90900_scale_type2': {'nominal_pdf': '90900', 'type': 'Scale_ME', 'nominal': ' PDF set = 90900 ', 'weights': [' 0p5muF_0p5muR_PDF4LHC15 ', ' 0p5muF_2muR_PDF4LHC15 ', ' 0p5muF_PDF4LHC15 ', ' 0p5muR_PDF4LHC15 ', ' 2muF_0p5muR_PDF4LHC15 ', ' 2muF_2muR_PDF4LHC15 ', ' 2muF_PDF4LHC15 ', ' 2muR_PDF4LHC15 ', ' PDF set = 90900 '], 'combination': 'envelope'}, 'Powheg_ME_PDF13165_scale': {'nominal_pdf': '13165', 'type': 'Scale_ME', 'nominal': ' PDF set = 13165 ', 'weights': [' 0p5muF_0p5muR_CT14 ', ' 0p5muF_2muR_CT14 ', ' 0p5muF_CT14 ', ' 0p5muR_CT14 ', ' 2muF_0p5muR_CT14 ', ' 2muF_2muR_CT14 ', ' 2muF_CT14 ', ' 2muR_CT14 ', ' PDF set = 13165 '], 'combination': 'envelope'}}
```

The `combination`  field can take the following values: 
- `lhadpdf` (for PDF uncertainties, delegate the job of conbining the weights into variations to LHAPDF, which internally knows how to do this) 
- `envelope` (take the max/min of each weight as the uncertainty in each bin of a distribution, eg for Scale Variations)
- `stat` (the uncertainty is taken to be the stat uncertainty in each bin. This is usually done for the nominal weight)
- `replicas` (placeholder for a future implementation of wg taking the 68th percentile of each weight in a bin. This would normally be done for some PDF uncertainties but in all cases so far this is handled by LHAPDF)
- `alphas` (used to take the two specified weight as up/down variations. As the name suggests, this is used primarily for alphaS variations. Practically this is a special case of  `envelope`)
- `customFunction` (for expert use only, it is possible to define custom combinations, such as in `examples/systematicsTool-examples/myCombinatioRecipes.yaml`)

