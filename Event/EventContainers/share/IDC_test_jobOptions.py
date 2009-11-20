theApp.setup( MONTECARLO )

theApp.Dlls += ["EventContainers" ]; 
theApp.TopAlg += ["ID_ContainerTest" ] ; 
# every Skip collections are created.
ID_ContainerTest = Algorithm( "ID_ContainerTest" )
ID_ContainerTest.Skip=1; 
# Total number of collection 
ID_ContainerTest.NumCollections=20000; 
# Number of hits in each collection
ID_ContainerTest.Nhits=10; 
# Test level =0 for creation of Collections only
#            =1 for accessing the All Collections
#            =2 for accessing the All Collections twice, and all digits
#            =3 for testing lower/upper bound and find. 
ID_ContainerTest.Test=4; 
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = 3
theApp.EvtMax = 10
