# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from DataQualityConfigurations._resolve_data_path import resolve_data_path
hcfg_dir = resolve_data_path('DataQualityConfigurations')
print(hcfg_dir)
