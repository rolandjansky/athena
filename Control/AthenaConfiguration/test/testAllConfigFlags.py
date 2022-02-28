#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
acf=AthConfigFlags()
acf.addFlag("x.flag1",1)
acf.addFlag("x.flag2",2)
acf.addFlag("x.flag3", lambda prev: prev.x.flag2*2 )
acf.addFlag("x.flag7", lambda prev: prev.x.flag1+27)

print(acf.x.flag1)
print(acf.x.flag3)

#acf.addFlag("flag4", lambda prev: prev.get("flag5")*2 )
#acf.addFlag("flag5", lambda prev: prev.get("flag4")*2 )
#print (acf.get("flag4") -> Circular dependency!  )


acf.addFlag("domain1.flag1","bla")
acf.addFlag("domain1.flag2",lambda prev: prev.domain1.flag1*2)
acf.addFlag("domain2.flag1","geh")
acf.addFlag("domain2.flag2","xyz")
#acf.addFlag("domain2.flagxxx","will fail")

acf.lock()

print("Initial flag container")
acf.dump()

acfPrime=acf.clone()
acfPrime.x.flag3 = 42

print("Cloned flag container")
acfPrime.dump()

acfMod=acf.cloneAndReplace("domain1","domain2")

print("After cloneAndReplace")
acfMod.dump()

ff=acf.initAll()

print("Frozen dict:")
acf.lock()
acf.dump()
