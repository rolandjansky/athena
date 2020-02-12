# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram

# for the monitoring the only extra thing we use is the XS histogram
hXS        = defineHistogram('Hypo_XS',        type='TH1F', title="EF Significance; (XS/GeV^{1/2})",         xbins=40,  xmin=-0.025,   xmax=20.025)
hXS2       = defineHistogram('Hypo_XS2',        type='TH1F', title="EF Significance 2; (XS2/GeV^{1/2})",         xbins=40,  xmin=-0.025,   xmax=20.025)
