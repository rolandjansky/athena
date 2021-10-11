# Unit test for AthTPCnvSvc

theApp.EvtMax = 1
theApp.initialize()
tpsvc=PyAthena.py_svc('AthTPCnvSvc',True,'ITPCnvSvc')

clidsvc=PyAthena.py_svc('ClassIDSvc',True,'IClassIDSvc')
clidsvc.setTypeForID(20394829,'AthenaServicesTestConverters::TA','AthenaServicesTestConverters::TA')
clidsvc.setTypeForID(20394828,'xxx','AthenaServicesTestConverters::TA')
clidsvc.setTypeForID(20394827,'AthenaServicesTestConverters::TB','AthenaServicesTestConverters::TB')
clidsvc.setTypeForID(20394826,'yyy','AthenaServicesTestConverters::TB')
clidsvc.setTypeForID(20394825,'AthenaServicesTestConverters::TBTRIG','AthenaServicesTestConverters::TBTRIG')
clidsvc.setTypeForID(20394824,'zzz','AthenaServicesTestConverters::TBTRIG')

def getname(cnv):
    if not cnv:
        return 'None'
    return PyAthena.System.typeinfoName (cnv.transientTInfo())

assert getname(tpsvc.load_tpcnv('AthenaServicesTestConverters::TestConverter_TA_PA1')) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA1'

assert getname(tpsvc.t2p_cnv('AthenaServicesTestConverters::TA')) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'
assert getname(tpsvc.t2p_cnv('AthenaServicesTestConverters::TA',1)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'
assert getname(tpsvc.t2p_cnv('AthenaServicesTestConverters::TA',2)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'

assert getname(tpsvc.p2t_cnv('AthenaServicesTestConverters::PA1')) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA1'
assert getname(tpsvc.p2t_cnv('AthenaServicesTestConverters::PA2')) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'
assert getname(tpsvc.p2t_cnv('AthenaServicesTestConverters::PA2',1)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'
assert getname(tpsvc.p2t_cnv('AthenaServicesTestConverters::PA2',2)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'

assert getname(tpsvc.t2p_cnv(20394829)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'
assert getname(tpsvc.t2p_cnv(20394829,1)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'
assert getname(tpsvc.t2p_cnv(20394829,2)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'

assert getname(tpsvc.t2p_cnv(20394828)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'
assert getname(tpsvc.t2p_cnv(20394828,1)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'
assert getname(tpsvc.t2p_cnv(20394828,2)) == \
       'AthenaServicesTestConverters::TestConverter_TA_PA2'

assert getname(tpsvc.t2p_cnv('AthenaServicesTestConverters::TB')) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1'
assert getname(tpsvc.t2p_cnv('AthenaServicesTestConverters::TB',1)) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1_ARA'
assert getname(tpsvc.t2p_cnv('AthenaServicesTestConverters::TB',2)) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1'

assert getname(tpsvc.t2p_cnv('AthenaServicesTestConverters::TBTRIG')) == \
       'None'
assert getname(tpsvc.t2p_cnv('AthenaServicesTestConverters::TBTRIG',1)) == \
       'None'
assert getname(tpsvc.t2p_cnv('AthenaServicesTestConverters::TBTRIG',2)) == \
       'AthenaServicesTestConverters::TestConverter_TBTRIG_PB1'

assert getname(tpsvc.p2t_cnv('AthenaServicesTestConverters::PB1')) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1'
assert getname(tpsvc.p2t_cnv('AthenaServicesTestConverters::PB1',1)) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1_ARA'
assert getname(tpsvc.p2t_cnv('AthenaServicesTestConverters::PB1',2)) == \
       'AthenaServicesTestConverters::TestConverter_TBTRIG_PB1'

assert getname(tpsvc.t2p_cnv(20394827)) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1'
assert getname(tpsvc.t2p_cnv(20394827,1)) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1_ARA'
assert getname(tpsvc.t2p_cnv(20394827,2)) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1'

assert getname(tpsvc.t2p_cnv(20394826)) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1'
assert getname(tpsvc.t2p_cnv(20394826,1)) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1_ARA'
assert getname(tpsvc.t2p_cnv(20394826,2)) == \
       'AthenaServicesTestConverters::TestConverter_TB_PB1'

assert getname(tpsvc.t2p_cnv(20394825)) == \
       'None'
assert getname(tpsvc.t2p_cnv(20394825,1)) == \
       'None'
assert getname(tpsvc.t2p_cnv(20394825,2)) == \
       'AthenaServicesTestConverters::TestConverter_TBTRIG_PB1'

assert getname(tpsvc.t2p_cnv(20394824)) == \
       'None'
assert getname(tpsvc.t2p_cnv(20394824,1)) == \
       'None'
assert getname(tpsvc.t2p_cnv(20394824,2)) == \
       'AthenaServicesTestConverters::TestConverter_TBTRIG_PB1'

