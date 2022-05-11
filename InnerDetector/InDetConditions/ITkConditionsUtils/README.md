# ITk Conditions Utilities

## ITk Pixel Charge Calibration

To dump pixel identifiers run
```
ITkPixelIdentifiers
```

To prepare ITk Pixel charge calibration DB file run
```
ITkPixelChargeCalibration.py
```

To test this with Athena run with the following `preExec`:
```
--preExec 'ConfigFlags.ITk.Conditions.PixelChargeCalibTag = "ChargeCalib-MC21-01"; ConfigFlags.ITk.Conditions.PixelChargeCalibFile = "ITkPixelChargeCalib.db"'
```
