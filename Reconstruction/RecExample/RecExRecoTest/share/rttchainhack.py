# This hack is to make the subsequent EventSelector.InputCollection lines dummy (D. Rousseau)

class DummyEventSelector :
     InputCollections = []
class DummyServiceMgr :
     EventSelector = DummyEventSelector
# Create a dummy ServiceMgr
ServiceMgr = DummyServiceMgr()

# end of hack
