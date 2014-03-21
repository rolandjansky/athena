"""
Common configurations for ISF
KG Tan, 17/06/2012
"""

include.block('ISF_Example/ISF_ExampleCommonConfig.py')

print "DEPRECATION WARNING: include('ISF_Example/ISF_ExampleCommonConfig.py') is deprecated."
print "Please Use include('ISF_Config/ISF_ConfigJobInclude.py') instead."
include('ISF_Config/ISF_ConfigJobInclude.py')
