# testing exception handline

from RecExConfig.RecConfFlags import recConfFlags
# if true catch exception and proceed
# if false rethrow exception and stop
recConfFlags.AllowIgnoreConfigError=True


# if all that is needed is to protect an include 
from RecExConfig.Resilience import protected_include
protected_include ("truc/pouet.py")

from RecExConfig.Resilience import protected_import
protected_import (Truc,Pouet)


# if more than an include or if want to do sthg special in case of an exception
from RecExConfig.Resilience import signal_exception
try:
    include ("truc/pouet.py")
    pass
except Exception:
    print "doSomethingIntelligent about this exception"
    signal_exception(" failed to include truc/pouet.py. Did something intelligent about it")






stop
