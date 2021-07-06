##########################################################################
#
# An example for MC Truth
#
##########################################################################

# init application mgr
theApp.initialize()

# run 1 event
theApp.nextEvent()

# get MC Truth
mcc = PyTruthTools.getMcEvents('GEN_AOD')

# get GenEvent
mc = mcc[0]

# call a normal method
print (mc.alphaQCD())

# get particle-iterators
# GenEvent::particles_begin() and _end() give ietrators
it  = mc.particles_begin()
itE = mc.particles_end()

# patch because HepMC::GenEvent::particle_const_iterator doesn't define end()
it.end = mc.particles_end

# loop over all particles
while (it != itE):
    # dereference and increment the iterator
    # this corresponds to "p = *it++" 
    p = it.next()
    print (p.pdg_id())

# get vertex-iterator
it = mc.vertices_begin()

it.end = mc.vertices_end

# get GenVertex obj
v = it.next()

# loop over all particles connected via a graph
itp  = v.particles_begin()
itpE = v.particles_end(0)

itp.end = v.particles_end

while (itp != itpE):
    print (itp.next().pdg_id())

