theApp.TopAlg +=["FakeLArTimeOffset"]
FakeLArTimeOffset=Algorithm("FakeLArTimeOffset")
FakeLArTimeOffset.OutputLevel = INFO

# periode 4 1000817-1000917
if RunNumber >= 1000817 :
	FakeLArTimeOffset.GlobalTimeOffset = 0.00

# periode 5 1000919-2000102
if RunNumber >= 1000919 :
	FakeLArTimeOffset.GlobalTimeOffset = 3.12

if RunNumber >= 1001000 :
	FakeLArTimeOffset.GlobalTimeOffset = 2.08	

#periode 6-a 1002007-1004234	
if RunNumber >= 1002007 :
	FakeLArTimeOffset.GlobalTimeOffset =  0.0
	
# periode 6-b 2100042-2100132
if RunNumber >= 2100024 :
        FakeLArTimeOffset.GlobalTimeOffset = 13.52
	
#             2100133-2100482
if RunNumber >= 2100133 :
	FakeLArTimeOffset.GlobalTimeOffset = 10.04
	
# Period 8 2101022-2101223
if RunNumber >= 2101022 :
	FakeLArTimeOffset.GlobalTimeOffset = 10.04

#          2101224-2101570	
if RunNumber >= 2101224 :
	FakeLArTimeOffset.GlobalTimeOffset = 16.64

#          2101471-2102992	
if RunNumber >= 2101571 :
	FakeLArTimeOffset.GlobalTimeOffset = -6.64

