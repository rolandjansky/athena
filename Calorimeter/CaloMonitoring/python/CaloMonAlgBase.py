#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def CaloBaseHistConfig(group, histpath,binlabels):

    group.defineHistogram('EvtBin;nEvtsRejectByDifferentTool',
                   title='Total Events - bin 1, ATLAS Ready - 2, && Good LAr LB - 3, && No LAr collision - 4, && No Beam Background - 5, && No Trigger Filer - 6, && No LArError - 7: Tool : RejectedEvents',
                   type='TH1I',path=histpath,
                   xbins=len(binlabels), xmin=0.5, xmax=len(binlabels)+0.5,
                   xlabels=binlabels)
    return
