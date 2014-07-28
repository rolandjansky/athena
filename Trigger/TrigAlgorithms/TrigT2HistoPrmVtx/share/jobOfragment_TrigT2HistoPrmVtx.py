#=============================================================
#
# TrigT2HistoPrmVtx job options
#
#==============================================================


#--------------------------------------------------
# TrigT2HistoPrmVtx algorithm
#--------------------------------------------------
theApp.Dlls += [ "TrigT2HistoPrmVtx" ]
TrigT2HistoPrmVtx_jet_L2 = Algorithm( "TrigT2HistoPrmVtx_jet_L2" )


#--------------------------------------------------------------
# Algorithm Private Options
#--------------------------------------------------------------


# SET GENERAL PROPERTIES
# track reconstruction algorithm: SiTrack(1) IdScan(2)
TrigT2HistoPrmVtx_jet_L2.AlgoId = 1


#==============================================================
#
# end of file
#
#==============================================================

