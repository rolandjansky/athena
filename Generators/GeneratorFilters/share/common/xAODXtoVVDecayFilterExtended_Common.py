# common fragment for xAODtoVVDecayExtended filter
# conversion to XAOD, 
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODXtoVVDecayFilterExtended
xAODXtoVVDecayFilterExtended = xAODXtoVVDecayFilterExtended("xAODXtoVVDecayFilterExtended")
filtSeq += xAODXtoVVDecayFilterExtended

# to modify cuts put into JOs e.g.:
filtSeq.xAODXtoVVDecayFilterExtended.PDGGrandParent = 25
filtSeq.xAODXtoVVDecayFilterExtended.PDGParent = 15
filtSeq.xAODXtoVVDecayFilterExtended.StatusParent = 2
filtSeq.xAODXtoVVDecayFilterExtended.PDGChild1 = [211,213,215,311,321,323,10232,10323,20213,20232,20323,30213,100213,100323,1000213]
filtSeq.xAODXtoVVDecayFilterExtended.PDGChild2 = [211,213,215,311,321,323,10232,10323,20213,20232,20323,30213,100213,100323,1000213]





