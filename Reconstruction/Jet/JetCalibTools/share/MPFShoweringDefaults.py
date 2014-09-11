#
#  Default constants for the sequence MPFAbsResponse -> JetCaloShowering.
#  Provided by Doug Schouten
#

#================================================================
# barrel: |eta| < 1.2
MPFAbsResponse.Parameterization = 'PWR'
MPFAbsResponse.MPFCalibConstants = '[ 200.0, -.2241, .7775 ]'
MPFAbsResponse.CorrectByET = False

JetCaloShowering.Parameters = '[ 1.007, 0.3222, 0.9407 ]'
JetCaloShowering.CorrectByET = False

#================================================================
# endcap: 1.8 < |eta| < 2.9
MPFAbsResponse.Parameterization = 'PWR'
MPFAbsResponse.MPFCalibConstants = '[ 200.0, -.2241, .7775 ]'
MPFAbsResponse.CorrectByET = False

JetCaloShowering.Parameters = '[ 1.011, 0.2108, 0.6790 ]'
JetCaloShowering.CorrectByET = False
