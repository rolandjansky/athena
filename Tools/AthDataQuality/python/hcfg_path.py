from DataQualityConfigurations._resolve_data_path import resolve_data_path
hcfg_dir = resolve_data_path('DataQualityConfigurations')
print hcfg_dir

'''
if hcfg_dir:
    print "Found DataQualityConfigurations data directory %s, using it" % hcfg_dir
else:
    hcfg_dir = os.getcwd()
    print "DataQualityConfigurations data directory not found, using $PWD instead."

print "Looking for collisions_*.hcfg files in %s" % (hcfg_dir)
'''

