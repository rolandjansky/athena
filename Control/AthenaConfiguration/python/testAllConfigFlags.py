from AthenaConfiguration.AthConfigFlags import AthConfigFlags
acf=AthConfigFlags()
acf.addFlag("flag1",1)
acf.addFlag("flag2",2)
acf.addFlag("flag3", lambda prev: prev.get("flag2")*2 )
acf.addFlag("flag7", lambda prev: prev.get("flag1")+27)

print acf.get("flag1")
print acf.get("flag3")

#acf.addFlag("flag4", lambda prev: prev.get("flag5")*2 )
#acf.addFlag("flag5", lambda prev: prev.get("flag4")*2 )
#print acf.get("flag4") -> Circular dependency!  


acf.addFlag("domain1.flag1","bla")
acf.addFlag("domain1.flag2",lambda prev: prev.get("domain1.flag1")*2)
acf.addFlag("domain2.flag1","geh")
acf.addFlag("domain2.flag2","xyz")
#acf.addFlag("domain2.flagxxx","will fail")

acf.lock()

print "Initial flag container"
acf.dump()

acfPrime=acf.clone()
acfPrime.set("flag3",42)

print "Cloned flag container"
acfPrime.dump()

acfMod=acf.cloneAndReplace("domain1","domain2")

print "After cloneAndReplace"
acfMod.dump()

ff=acf.initAll()

print "Frozen dict:" 
acf.dump()



