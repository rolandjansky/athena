## Extra chunk of config for JZxW slices, to be included in each JetFilter_JZxW.py file
## *after* the appropriate *_JZx.py fragment has been included.
	
## Use pT shaping in the truth jet filter
assert hasattr(filtSeq, "QCDTruthJetFilter")
filtSeq.QCDTruthJetFilter.DoShape = True

## Configure LogicalExpressionFilter to disable the default UseEventWeight setting
filtSeq.UseEventWeight = False
