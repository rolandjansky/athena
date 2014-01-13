from runEB import JobDesc
import copy, re

#----------------------------------------------------------------------
# Build list of jobs 
#----------------------------------------------------------------------

def FillJobList(joblist,name):

    # String of the rate items plotted by histRates.py
    rateItems={'RateStreams':
               'L1_L1A,'+\
               'L2_recording_physics,'+\
               'EF_recording_physics,'+\
               'EF_str_Egamma,'+\
               'EF_str_Muons,'+\
               'EF_str_JetTauEtmiss',
               
               'RatesL1Primaries':
               'L1_EM18VH,'+\
               'L1_MU15,'+\
               'L1_XE40,'+\
               'L1_J50_XE40,'+\
               'L1_J75,'+\
               'L1_2TAU20',
               
               'RatesEFPrimaries': 
               'EF_e24vhi_medium1,'+\
               'EF_mu24i_tight,'+\
               'EF_xe80_tclcw_tight,'+\
               'EF_j80_a4tchad_xe100_tclcw_loose,'+\
               'EF_j360_a4tchad,'+\
               'EF_2tau38T_medium1'
               }
               
    # Title for the items above
    rateTitle={'RateStreams': 'Rates for Various Levels and Streams',
               'RatesL1Primaries': 'Level 1 Trigger Rates After Prescale',
               'RatesEFPrimaries': 'Event Filter Trigger Rates After Prescale'}
    
    # Generic predictions 
    if name=='default':
        jd = JobDesc()
        jd.name=None
        jd.ebdataset="TrigEB_ntuple.xml"
        jd.predlumi=None
        jd.predrun=None
        jd.predlb=None
        jd.AllOverlap=False
        jd.doOnlineComp=True
        jd.plotLumiCorrect=1.00
        jd.rateItems=rateItems
        jd.rateLabels=rateItems
        jd.rateTitle=rateTitle
        jd.reducedhtml=True
        joblist.append(jd)

    if name=='default_tiny':
        jd = JobDesc()
        jd.name=None
        jd.ebdataset="TrigEB_ntuple.xml"
        jd.predlumi=None
        jd.predrun=None
        jd.predlb=None
        jd.usetiny=True
        jd.AllOverlap=False
        jd.doOnlineComp=True
        jd.plotLumiCorrect=1.00
        jd.rateItems=rateItems
        jd.rateLabels=rateItems
        jd.rateTitle=rateTitle
        jd.reducedhtml=True
        joblist.append(jd)
        
    # Heavy ion runs
    if name=='HI_default':
        jd = JobDesc()
        jd.name=None
        jd.ebdataset="TrigEB_ntuple.xml"
        jd.predlumi=None
        jd.predrun=None
        jd.predlb=None
        jd.AllOverlap=False
        jd.doOnlineComp=True
        jd.reducedhtml=True
        joblist.append(jd)

    # rulebook predictions 
    if name=='rulebook':
        jd = JobDesc()
        jd.name=None
        jd.ebdataset="TrigEB_ntuple.xml"
        jd.predlumi=None
        jd.predrun=None
        jd.predlb=None
        jd.defaultps=-1
        jd.defaultpt=-1
        jd.AllOverlap=False
        jd.doOnlineComp=False
        jd.reducedhtml=True
        joblist.append(jd)

    # rulebook predictions (using tiny ntuple)
    if name=='rulebook_tiny':
        jd = JobDesc()
        jd.name=None
        jd.ebdataset="TrigEB_ntuple.xml"
        jd.predlumi=None
        jd.predrun=None
        jd.predlb=None
        jd.defaultps=-1
        jd.defaultpt=-1
        jd.usetiny=True
        jd.AllOverlap=False
        jd.doOnlineComp=False
        jd.reducedhtml=True
        joblist.append(jd)
        
    # Predictions with latest EB ntuples, scanning luminosity
    if name=='lumiscan':

        lumipoint = ['1e33', '1.5e33', '2.0e33']
        for lumi in lumipoint:
            jd = JobDesc()
            jd.name=None
            jd.ebdataset="TrigEB_ntuple.xml"
            jd.predlumi=lumi
            jd.predrun=None
            jd.predlb=None
            jd.AllOverlap=False
            jd.doOnlineComp=True
            jd.reducedhtml=True
            joblist.append(jd)

    
    # Predictions with latest EB ntuples, scanning pileup
    if name=='muscan_predrun':

        mupoints = [
            ['184169', '86',  '6.0'],
            ['184169', '322', '5.0'],
            ['184169', '647', '4.0'],
            ['184169', '886', '3.5']]                    

        for runset in mupoints:
            jd = JobDesc()
            jd.name=None
            jd.ebdataset="TrigEB_ntuple.xml"
            jd.predlumi=None
            jd.predrun=runset[0]
            jd.predlb=runset[1]
            jd.predmu=runset[2]
            jd.AllOverlap=False
            jd.doOnlineComp=True
            jd.reducedhtml=True
            joblist.append(jd)

    # RTT jobs
    # Just match regex instead of adding 16 stanzas
    # Prevent automatic comparison
    if re.match("\ARTT(.*)",name):
        jd = JobDesc()
        jd.name=name
        jd.ebdataset="TrigEB_ntuple.xml"
        jd.predlumi=None
        jd.predrun=None
        jd.predlb=None
        jd.AllOverlap=False
        jd.doOnlineComp=True
        jd.doHistPlotsOnly=True
        jd.trpFileName='file.xml'
        jd.plotLumiCorrect=1.00
        jd.rateItems=rateItems
        jd.rateLabels=rateItems
        jd.rateTitle=rateTitle
        jd.key='TrigRate'
        jd.reducedhtml=True
        joblist.append(jd)



