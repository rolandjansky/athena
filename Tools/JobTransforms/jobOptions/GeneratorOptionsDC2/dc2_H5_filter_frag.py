# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Filter = Algorithm( "Filter" )
Filter.Members = ["MultiLeptonFilter"]
MultiLeptonFilter = Algorithm( "MultiLeptonFilter" )
MultiLeptonFilter.NLeptons = 1
MultiLeptonFilter.Etacut = 2.7
MultiLeptonFilter.Ptcut = 5000.
