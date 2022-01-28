Help on module covarianceTool

# NAME
covarianceTool

# FILE
/afs/cern.ch/user/l/lcorpe/work/private/testPMGSystTools_13Sept18/systematics-tools/local/bin/covarianceTool.py

# DESCRIPTION
The `covarianceTool.py` executable dis a helper which uses the functions provided by `covarianceToolsLibrary.py` to perform the  
comparison between MC predictions and HEPData records.

However, the `local/bin/covarianceToolsLibrary.py` can also be used directly to manipulate covariance info and evaluate goodness of fit.
Examples of how to do this are provided in the `examples` subdirectory: 
- `examples/covarianceTool-examples/README.md`

The first thing to do is test you can access the `covarianceTool.py` executable and see available options:
```
covarianceTool.py -h
```

Author: Louie D. Corpe (UCL)  
Email: l.corpe@cern.ch

# FUNCTIONS
## convertHistToScatter(ao)

## getCorrelationInfoFromWeb(name=None)

## isfloat(x)

## makeBeamer(plotDictionary, outdir)

## makeSummaryTable(plotDictionary, outfile, hdataName=None)

## markupYODAwithCorrelations(ref, corrDir, ignoreCorrelations)

## markupYODAwithCorrelationsFromYAML(reference, correlationsDir)

## markupYODAwithCorrelationsFromYODAs(reference, correlationsDir)

## matchAOs(aoList1, aoList2, strategy=0, recursive=True)

## renameAOs(fin_name, aoMap)

## returnFileContents(filename, inputType='data')

# DATA
## args = ['covarianceTool']
## histograms = {'covariance': {}, 'data': None, 'models': {}}
## opts = <Values at 0x7fd92d696bd8: {'corr_mc': None, 'nt... 'get_corr_f...
## parser = <optparse.OptionParser instance>


