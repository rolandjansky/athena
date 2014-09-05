# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RunMultiRates import JobDesc,ratecodepath
import copy

#----------------------------------------------------------------------
# Build list of jobs 
#----------------------------------------------------------------------
ratecodedata=ratecodepath+'/Trigger/TrigCost/TrigCostRate/data'

def FillJobList(joblist,name):

    # test prediction
    if name=='eb166383_166783_525':
        jd = JobDesc()
        jd.name="eb166383_166783_525"
        jd.datasets=['data10_7TeV.166658.166466.166383.physics_EnhancedBias.daq.RAW_13-10-2010_MCppv1_AtlasCAFHLT-rel1-15.6.X.Y.Z_TMP001278']
        jd.bgxml=ratecodedata+'/enhanced_bias_run_166383.xml'
        jd.dqxml=ratecodedata+'/dq_166383.xml'
        jd.PSxml='output_xml_500000.xml'
        jd.predlumi=None
        jd.predrun="166786"
        jd.predlb="525"
        jd.AllOverlap=False
        jd.key='TrigRate'
        joblist.append(jd)            


    # v16 validation
    if name=='v16_validation':
        dslist = ['valid1.105001.pythia_minbias.recon.AOD.e603_s932_r1573_28-09-2010_AtlasProduction-16.0.1.1',
                  'valid1.105001.pythia_minbias.recon.AOD.e603_s932_r1527_28-09-2010_AtlasProduction-16.0.1.1',
                  'valid1.105001.pythia_minbias.recon.AOD.e609_s955_r1628_28-10-2010_MCppv1_AtlasProduction-16.0.2.2']
        
        for ds in dslist:
            jd = JobDesc()
            jd.name=ds
            jd.datasets=[ds,]
            jd.bgxml=None
            jd.PSxml='prescalexmls/LVL1Menu_887_2024.xml,prescalexmls/HLTMenu_887_1942.xml'
            jd.predlumi='1e31'
            jd.AllOverlap=False
            jd.key='TrigCost'
            joblist.append(jd)


    # v16 validation
    if name=='simlcombtest':
        jd = JobDesc()
        jd.name=name
        jd.datasets=['testebdata']
        jd.bgxml=ratecodedata+'/enhanced_bias_run_166383.xml'
        jd.dqxml=ratecodedata+'/dq_166383.xml'
        jd.predlumi='5e32'
        jd.AllOverlap=False
        joblist.append(jd)

    # Predict 5e32
    if name=='eb_5e32' or name=='eb_5e32_nops':
        jd = JobDesc()
        jd.name=name
        jd.datasets=['data10_7TeV.166658.166466.166383.physics_EnhancedBias.daq.RAW_13-10-2010_MCppv1_AtlasCAFHLT-rel1-15.6.X.Y.Z_TMP001278']
        jd.bgxml=ratecodedata+'/enhanced_bias_run_166383.xml'
        jd.dqxml=ratecodedata+'/dq_166383.xml'
        if name=='eb_5e32':
            jd.PSxml='output_xml_500000.xml'
        jd.predlumi='5e32'
        jd.AllOverlap=False
        joblist.append(jd)

    

#-----------------------------------------------------------------------
# Old configurations
#-----------------------------------------------------------------------

#pslist=[('1e31','/afs/cern.ch/user/t/trigcomm/www/rates_webpage/lipeles/psxml/ps_TMP_00-12-06/output_xml_10000.xml'),
#        ('2e31','/afs/cern.ch/user/t/trigcomm/www/rates_webpage/lipeles/psxml/ps_TMP_00-12-06/output_xml_20000.xml'),
#        ('3e31','/afs/cern.ch/user/t/trigcomm/www/rates_webpage/lipeles/psxml/ps_TMP_00-12-06/output_xml_30000.xml')]
#        
#for (lumi,ps) in pslist:
#    jd = JobDesc()
#    jd.name="eb_08-09-2010_MCppv1_AtlasCAFHLT-rel_2-15.6.X.Y.Z_"+lumi
#    jd.datasets=['data10_7TeV.00162882.physics_EnhancedBias.merge.RAW_08-09-2010_MCppv1_AtlasCAFHLT-rel_2-15.6.X.Y.Z',
#                 'data10_7TeV.00162843.physics_EnhancedBias.merge.RAW_08-09-2010_MCppv1_AtlasCAFHLT-rel_2-15.6.X.Y.Z']
#    jd.bgxml=ratecodedata+'/enhanced_bias_run_162882.xml'
#    jd.PSxml=ps
#    jd.predlumi=lumi
#    jd.AllOverlap=True
#    joblist.append(jd)
#

#pslist=[('1e31',None),
#        ('1e31','/afs/cern.ch/user/t/trigcomm/www/rates_webpage/lipeles/psxml/ps_TMP_00-12-06/output_xml_10000.xml'),
#        ('2e31','/afs/cern.ch/user/t/trigcomm/www/rates_webpage/lipeles/psxml/ps_TMP_00-12-06/output_xml_20000.xml'),
#        ('3e31','/afs/cern.ch/user/t/trigcomm/www/rates_webpage/lipeles/psxml/ps_TMP_00-12-06/output_xml_30000.xml')]

#pslist=[('1e31',None),
#        ('1e31','/afs/cern.ch/user/t/trigcomm/www/rates_webpage/lipeles/psxml/ps_TMP_00-12-06/output_xml_10000.xml')]
#
#for (lumi,ps) in pslist:
#    jd = JobDesc()
#    jd.name="eb_08-09-2010_MCppv1_AtlasCAFHLT-rel_2-15.6.X.Y.Z_"+lumi
#    if not ps:
#        jd.name+="_noPS"
#    jd.datasets=['data10_7TeV.00162843.physics_EnhancedBias.merge.RAW_14-09-2010_Physicsppv1_AtlasCAFHLT-rel2-15.6.X.Y.Z_TMP-00-12-20',
#                 'data10_7TeV.00162882.physics_EnhancedBias.merge.RAW_14-09-2010_Physicsppv1_AtlasCAFHLT-rel2-15.6.X.Y.Z_TMP-00-12-20']
#    jd.bgxml=ratecodedata+'/enhanced_bias_run_162882.xml'
#    jd.PSxml=ps
#    jd.predlumi=lumi
#    jd.AllOverlap=True
#    joblist.append(jd)
#

#pslist=[('1e31',None)]
#
#for (lumi,ps) in pslist:
#    jd = JobDesc()
#    jd.name="eb_08-09-2010_MCppv1_AtlasCAFHLT-rel_2-15.6.X.Y.Z_"+lumi
#    if not ps:
#        jd.name+="_noPS"
#    jd.datasets=['data10_7TeV.00162843.physics_EnhancedBias.merge.RAW_14-09-2010_Physicsppv1_AtlasCAFHLT-rel2-15.6.X.Y.Z_TMP-00-12-20',
#                 'data10_7TeV.00162882.physics_EnhancedBias.merge.RAW_14-09-2010_Physicsppv1_AtlasCAFHLT-rel2-15.6.X.Y.Z_TMP-00-12-20']
#    jd.bgxml=ratecodedata+'/enhanced_bias_run_162882.xml'
#    jd.PSxml=ps
#    jd.predlumi=lumi
#    jd.AllOverlap=False
#    joblist.append(jd)

#dslist = ['valid1.105001.pythia_minbias.recon.AOD.e603_s932_r1573_28-09-2010_AtlasProduction-16.0.1.1',
#          'valid1.105001.pythia_minbias.recon.AOD.e603_s932_r1527_28-09-2010_AtlasProduction-16.0.1.1']
#
#for ds in dslist:
#    jd = JobDesc()
#    jd.name=ds
#    jd.datasets=[ds,]
#    jd.bgxml=None
#    jd.PSxml='prescalexmls/LVL1Menu_887_2024.xml,prescalexmls/HLTMenu_887_1942.xml'
#    jd.predlumi='1e31'
#    jd.AllOverlap=False
#    joblist.append(jd)

#pslist=[('1e32',None)]
#
#for (lumi,ps) in pslist:
#    jd = JobDesc()
#    jd.name="eb_165732_Physicsppv1_AtlasP1HLT-15.6.9.25_"+lumi
#    if not ps:
#        jd.name+="_noPS"
#    jd.datasets=['data10_7TeV.00165732.physics_EnhancedBias.daq.RAW_27-09-2010_Physicsppv1_AtlasP1HLT-15.6.9.25']
#    jd.bgxml=ratecodedata+'/enhanced_bias_run_165732.xml'
#    jd.PSxml=ps
#    jd.predlumi=lumi
#    jd.AllOverlap=False
#    jd.groupxml='exampleGroups.xml'
#    joblist.append(jd)
#
#





