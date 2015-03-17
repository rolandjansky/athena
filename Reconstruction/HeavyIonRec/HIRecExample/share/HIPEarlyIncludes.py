os.system('get_files -data HIFlowWeight.root')

from RecExConfig.RecFlags import rec
rec.doAlfa.set_Value_and_Lock(True)
rec.doForwardDet.set_Value_and_Lock(True)
