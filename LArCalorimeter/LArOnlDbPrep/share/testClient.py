import xmlrpclib

s = xmlrpclib.Server('http://localhost:8000')
print s.runAthena()
