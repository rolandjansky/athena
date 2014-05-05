######################################################################
#
# This script demonstrates how to use "plot" command
#
#  usage:
#   plot (Class#Key, parameter, criteria=True, nEvent=100)
#
#####################################################################

# plot pt of electrons.
# $x denotes an element of "Class#Key", if "Class#Key" is a collection
plot ("ElectronContainer#ElectronCollection","$x.pt()")

# plot et of MissingET.
# $x denotes "Class#Key" itself, if "Class#Key" is not a vector-like class
plot ("MissingET#MET_Calib","$x.sumet()")

# apply a selection criteria.
# Class#Key can be used in a criteria
plot ("ElectronContainer#ElectronCollection","$x.pt()","MissingET#MET_Calib.sumet()>100*GeV")

# Class#Key can be used in a parameter as well
plot ("ElectronContainer#ElectronCollection","$x.px()+MissingET#MET_Calib.etx()")

# access constituent object using element-link and back-navigation.
# egamma is retrieved from ESD and then is evaluated.
#  Note! Currently the back-navigation cannot be used after EventSelector
#         reaches the end of input. So set nEvent to a value less than
#         the number of event in the input.
#        This should be fixed in the future
plot ("ElectronContainer#ElectronCollection","$x.eg().e()","$x.track().pt()>10*GeV", 4)

# arithmetic/boolean operators and math-functions can be used
#  in the "parameter" and "criteria"
plot ("ElectronContainer#ElectronCollection","sqrt($x.px()**2 + $x.py()**2)","$x.pz()<10*GeV and CTP_Decision#CTP_Decision.isAccepted()==True")
