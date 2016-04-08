from CoolDozer import *
import os.path

def init(func):
    def inner(self, dict={}):        
        self.jd = dict['JobDescriptor']
        self.log = DozerLogger("%s.log" % self.__class__.__name__, self.__class__.__name__, toConsole=True).getLogger(self)
        return func(self, dict)
    return inner

def run(func):
    def inner(self):
        def openDB(): 
            isOpened = None
            try:
                isOpened = self.db.openDB() 
            except DozerError, value:
                self.log.epanic( value )
            return isOpened
        def closeDB():  
            self.db.closeDB()        
        def getSchema():
            if not hasattr(self, 'db'):
                raise 
            if not self.db.isOpened():
                raise
            #try:
                
                #self.schema = self.db.getSchema(self.dbkey)
            #except DozerError, value:
            #    self.log.info("Schema %s hasn't been added to the database yet!" % self.dbkey)
                            
            # create new schema if necessary
            self.schema =  self.createSchema() #or self.schema
    
            if not self.schema:
                self.log.panic("No schema for config = '%s' found in DozerConfig " % self.dbkey )
                sys.exit(0)

            return self.schema
            #self.db += self.schema

        self.db = DozerDB(self.jd.package, "RTT", False, self.log )    
        self.dbkey = os.path.join(self.jd.paths.branch, self.jd.paths.runType, self.jd.paths.cmtConfig, self.jd.paths.topProject, self.jobId) # db key
        self.log.info("dbkey is %s" % self.dbkey )
        
        # try and open the DB
        if not openDB(): 
            self.log.panic('Unable to get a connection to the DB. Abandon ship...')
            raise

        # all ok, add schema to db
        self.db += getSchema()
        ret = None
        try:
            ret = func(self)
        except:
            self.log.epanic("%s threw an exception!" % func.__name__)
        finally:
            closeDB()
        return ret
    return inner


