from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
#tf.inputLVL1configFile.set_Value_and_Lock("TriggerMenuXML/LVL1config_MC_pp_v7.xml")
tf.inputLVL1configFile.set_Value_and_Lock("./LVL1config_MC_pp_v7_Run3.xml")
tf.readLVL1configFromXML.set_Value_and_Lock(True)
tf.configurationSourceList.set_Value_and_Lock(['xml'])



