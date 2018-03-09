import sys
import bz2, gzip
from math import sqrt, atan, atan2, tan, log

if __name__ == "__main__" :
    print "Why are you using this code interactively?"
    sys.exit(1)

class FTKPlaneSection :
    """This class contains the details on each detector plane"""
    def __init__(self) :
        return None

class FTKPlaneMap :
    """This class stores a summary of the plane map"""
    def __init__(self,fname) :
        pmapfile = open(fname,"r")
        # form on the next structur
        # [Ecp: [Pxl[layer],Sct[Layers], Barrel: ...]
        self.data = [[{},{}],[{},{}]]
        

        nPXLBar = 0
        nPXLEcp = 0
        nSCTBar = 0
        nSCTEcp = 0
        NMaxLayer = 0
        for i, line in enumerate(pmapfile) :
            vals = line.split()

            if i<4 : # first 4 lines are an header
                print line[:-1]
                if i == 0 :
                    nPXLBar = int(vals[0])
                elif i == 1 :
                    nPXLEcp = int(vals[0])
                elif i == 2:
                    nSCTBar = int(vals[0])
                elif i == 3 :
                    nSCTEcp = int(vals[0])                
                continue 

            # for each layer stores the relative logical layer ID
            if vals[0]=='pixel' : # is a PXL
                nlayer = int(vals[7])
                if nlayer>NMaxLayer :
                    NMaxLayer = nlayer
                self.data[1][int(vals[1])][int(vals[3])] = int(vals[7]) 
            elif vals[0]=='SCT' : # is an SCT
                nlayer = int(vals[9])
                if nlayer > NMaxLayer :
                    NMaxLayer = nlayer
                self.data[0][int(vals[1])][int(vals[3])] = nlayer
        self.NLogicalLayers = NMaxLayer+1
        print self.data
        print "Number of layers:", self.NLogicalLayers
        return None 

class Pattern :
    """This class define a pattern or a sector"""
    def __init__(self,nplanes,pid=-1) :
        """Create the pattern, passing the number of planes and its ID"""
        # set number of planes
        self.nplanes = nplanes
        # set the Pattern-ID
        self.pid = pid
        # set the id of Super-Strips in this pattern, -1 means uninitialized
        self.ssid = [-1]*nplanes
        # set the PID of the onwer sector, valid only for patterns
        self.sector = -1
        self.coverage = -1
        return None

    def Clone(self) :
        """Clone this pattern"""
        newpatt = Pattern(self.nplanes,self.pid)
        for i in xrange(self.nplanes) :
            newpatt.ssid[i] = self.ssid[i]
        newpatt.sector = self.sector
        newpatt.coverage = self.coverage
        return newpatt

    def SetNPlanes(self,nplanes) :
        """Set the number of plane and sizes the internal structures"""
        self.nplanes = nplanes
        self.ssid = [-1]*nplanes
        return None

    def ReadFromFile(self,fdesc) :
        """Read the next line of a file as the description of a pattern"""
        #print "Read from file"
        line = fdesc.readline()
        #print "line"
        return self.ReadFromLine(line)
        
    def ReadFromLine(self,line) :
        vals = line.split()
        if len(vals)!=self.nplanes+3 :
            print "*** wrong number of arguments for a", self.nplanes, "pattern:"
            print ">", line
        else :
            # set the values
            if self.pid!=-1 :
                if self.pid != int(vals[0]) :
                    print "*** mismatch reading Pattern", int(vals[0]), "in pattern", self.pid, "->", self.ssid
            else :
                self.pid = int(vals[0])
            for ip in xrange(self.nplanes) :
                self.ssid[ip] = int(vals[ip+1])
            self.sector = int(vals[self.nplanes+1])
            self.coverage = int(vals[self.nplanes+2])
        return self.pid

    def getXmin(self,plane,isPixel,moduleID,ssmap) :
        """Return the minimum coordinates for a SS"""
        if isPixel :
            phioff = 200
        else :
            phioff = 100
        phimod = moduleID/1000
        if moduleID%100 < 20 : # is barrel
            return self.ssid[plane]/phioff*ssmap.ss_data[0][isPixel][0]-(ssmap.ss_data[0][isPixel][1]-ssmap.ss_data[0][isPixel][1]%ssmap.ss_data[0][isPixel][0]+ssmap.ss_data[0][isPixel][0])*phimod
               
    def PrintToFile(self,stream) :
        """Print the Pattern content into a file"""
        line = "ID "+str(self.pid)
        line += " SSs "
        for i in xrange(self.nplanes) :
            line += " "+str(self.ssid[i])
        line += " in "+str(self.sector)+" cov "+str(self.coverage)
        line += "\n"
        stream.write(line)
        return None

    def IsEqual(self,other) :
        """Return True if a given pattern is equal to this. All the elements are compared, the ones with -1 are ignored"""
        for i in xrange(self.nplanes) :
            if other.ssid[i] == -1:
                continue

            if self.ssid[i]!=other.ssid[i] :
                return False

        if other.sector != -1 and other.sector != self.sector :
            return False

        if other.coverage != -1 and other.coverage != self.coverage :
            return  False
        return True

class PatternKDTree :
    """This class represents a node, ora tree, using kd-tree convention"""
    def __init__(self,pattern,depth) :
        # set the object related to this node
        self.pattern = pattern
        # node depth
        self.depth = depth
        # select the dimension splitted by this level
        self.split = self.depth % pattern.nplanes
        self.left = None
        self.right = None
        return None

    def AddNode(self,pattern) :
        """Add a node into the KD-Tree"""
        newnode = None
        # test if is greater
        if self.pattern.ssid[self.split] > pattern.ssid[self.split] :
            if self.right == None :
                # create a new right node
                self.right = PatternKDTree(pattern,self.depth+1)
                newnode = self.right
            else :
                # continue the search
                newnode = self.right.AddNode(pattern)
        else : # place on the left
            if self.left == None :
                # create a new left node
                self.left = PatternKDTree(pattern,self.depth+1)
                newnode = self.left
            else :
                # continue the search
                newnode = self.left.AddNode(pattern)
        return newnode

    def FindNode(self,pattern) :
        """Find the correspondent node, return None if doesn't exist. If pattern fields with -1 are used, to look for multiple matches, only the first is returned."""
        # ntuple to accumulate the similare
        res = []
        # check for this node
        if self.pattern.IsEqual(pattern) :
            res.append(self)

        # continue the search if greater or is a not partitioning plane
        if self.pattern.ssid[self.split] > pattern.ssid[self.split] or pattern.ssid[self.split] == -1:
            if self.right != None :
                res += self.right.FindNode(pattern)
        if self.pattern.ssid[self.split] <= pattern.ssid[self.split] or pattern.ssid[self.split] == -1:
            if self.left != None :
                res += self.left.FindNode(pattern)
        return res

class PatternBank :
    def __init__(self,path,mode="r",nplanes=-1) :
        self.mode = mode
        self.stream = None
        if path[-3:] == "bz2" :
            print "INFO: Read bz2 compressed pattern bank",
            self.stream = bz2.BZ2File(path,mode)
            print "Done"
        elif path[-2:] == "gz" : 
            print "INFO: Read  gzip compressed pattern bank",
            self.stream = bz2.GzipFile(path,mode)
            print "Done"
        else :
            self.stream = open(path,mode)

        if mode.find("r")>-1 :
            # read the header of the nank
            line = self.stream.readline()
            vals = line.split()
            # set the global attributes fro the bank
            self.npatterns = int(vals[0])
            self.nplanes = int(vals[1])
            self.patterns = []
        else :
            self.npatterns = 0
            self.nplanes = nplanes
            self.patterns = []
        return None

    def LoadPatterns(self,maxpatt=-1,pattmask=None) :
        """Load patterns from the given files. "maxpatt" argument, if >-1, limit the number of patterns to read, "pattmask" is pattern and pattern equal to it are loaded, to build this pattern mask read the Pattern.IsEqual documentation"""
        # load the patterns
        ntoread = maxpatt
        if ntoread ==-1 :
            ntoread = self.npatterns
        fraction = ntoread/20
        print "Load %d pattens" % ntoread
        for i in xrange(ntoread) :
            if i % fraction == 0 :
                print i
            newpatt = Pattern(self.nplanes,i)
            newpatt.ReadFromFile(self.stream)
            if pattmask == None :
                self.patterns.append(newpatt)
            elif newpatt.IsEqual(pattmask) :
                self.patterns.append(newpatt)
                
        #i = 0
        # for line in self.stream :
        #     if i % fraction == 0:
        #         print i
        #     newpatt = Pattern(self.nplanes,i)
        #     newpatt.ReadFromLine(line)
        #     self.patterns.append(newpatt)
        #     i += 1
        print "Done"
        return None

    def FindPatterns(self,pattern) :
        """Search, using a linear algorithm, if exist an equal pattern. Because the -1 fields are interpreted as "any" multiple matches are possible, the method retuns a list"""
        res = []
        for patt in self.patterns :
            if patt.IsEqual(pattern) :
                res.append(patt)
        return res

    def BuildKDTree(self) :
        """Build a KD-Tree representation of the patterns"""
        kdtree = PatternKDTree(self.patterns[0],0)
        for node in self.patterns[1:] :
            kdtree.AddNode(node)
        return kdtree


class FTKHit :
    """this class describe the content of a single hit reading the format of the FTK wrapper"""
    def __init__(self,raw_elements) :
        # global position of the hit
        self.x = float(raw_elements[0])
        self.y = float(raw_elements[1])
        self.z = float(raw_elements[2])
        # type of the detector: 1 pxl, 0 SCT
        self.type = int(raw_elements[3])
        # detector position: 0 bar
        self.BarEcp = int(raw_elements[4])
        # modul layer number, phi and eta position
        self.layer = int(raw_elements[5])
        self.phi_module = int(raw_elements[6])
        self.eta_module = int(raw_elements[7])
        # for SCT side, PXL phi position of the channel
        self.side = int(raw_elements[8])
        # SCT number of strip, PXL eta position
        self.strip = int(raw_elements[9])
        # PXL ToT, SCT number of strips
        self.num_strips = int(raw_elements[10])
        # event number of the associated track
        self.event = int(raw_elements[11])
        # barcode of the associated track
        self.barcode = int(raw_elements[12])
        # pt of the associated track
        self.barcode_pt = float(raw_elements[13])
        # fraction of the association
        self.barcode_frac = float(raw_elements[14])

        if self.type != 0 :
            return None
        layer_offset = 0 # 3 if you focus on the barrel and after don't use the plane map
        # here elaborate SCT ID for barrel
        if self.BarEcp == 0 :
            if self.layer==0 and self.side == 1 :
                self.layer = 0 + layer_offset
            elif self.layer==0 and self.side == 0 :
                self.layer = 1 + layer_offset
            elif self.layer==1 and self.side == 0 :
                self.layer = 2 + layer_offset
            elif self.layer==1 and self.side == 1 :
                self.layer = 3 + layer_offset
            elif self.layer==2 and self.side == 1 :
                self.layer = 4 + layer_offset
            elif self.layer==2 and self.side == 0 :
                self.layer = 5 + layer_offset
            elif self.layer==3 and self.side == 0 :
                self.layer = 6 + layer_offset
            elif self.layer==3 and self.side == 1 :
                self.layer = 7 + layer_offset
        else : # endcap part, look FTKPMap.cxx for further details
            outside = 0
            inner_ring = 0;
            if self.eta_module > 0 :
                inner_ring = 1

            if (self.eta_module == 0 or self.eta_module == 2) and self.side == 1 :
                outside = 1

            if self.eta_module == 1 and self.side == 0 :
                outside = 1
    
            # /* disk 8 flipped: special case */
            if self.layer == 8 and self.side == 1 :
                outside = 0
            if self.layer == 8 and self.side == 0 :
                outside = 1
    
            # /* split the disks up by inner/outer, and by side */
            self.layer = 4*self.layer + 2*inner_ring + outside;
    
            # /* aaaaand fix eta index */
            if inner_ring:
                self.eta_module = self.eta_module - 1;

            
        return None

class FTKOffline :
    """This class summarize the content of the offline tracks as stored in the wrapper files"""
    def __init__(self,params) :
        # perigee parameters
        self.d0 = float(params[0])
        self.z0 = float(params[1])
        # direction
        self.phi0 = float(params[2])
        self.ctheta = float(params[3])
        self.eta = -log(tan(atan2(1.0,self.ctheta)/2.))
        self.qpt = float(params[4])
        self.curv = 1./self.qpt/2000.0  #offline tracks are in MeV
        # truth association
        self.evtidx = int(params[5])
        self.barcode = int(params[6])
        self.barcode_frac = float(params[7])
        return None

class FTKTruth :
    def __init__(self,params) :
        """FTKTruth is described in the files produced by the wrapper using 8 values, here the value are converted in the ones used in FTK"""
        # get the raw parameter
        self.x0 = float(params[0])
        self.y0 = float(params[1])
        self.z0 = float(params[2])
        self.charge = float(params[3])
        self.px = float(params[4])
        self.py = float(params[5])
        self.pz = float(params[6])
        self.pdgcode = int(params[7])
        self.assoc = int(params[8])
        self.barcode_frac = float(params[9])
        self.evtindex = int(params[10])
        self.barcode = int(params[11])
        #print self.barcode
        self.parentID = int(params[12])
        self.isfiducial = int(params[13])

        self.pt = sqrt(self.px**2+self.py**2)
        self.ctheta = self.pz/self.pt
        if self.charge >= 0 :
            signcharge = 1.0
        else :
            signcharge = -1.0
            
        # evaluate the derived parameters
        x = self.x0-self.charge*self.py
        y = self.y0+self.charge*self.px
        self.curv = self.charge/sqrt(self.px**2+self.py**2)
        try :
            self.d0 = sqrt(x**2+y**2)*signcharge-1./self.curv
        except ZeroDivisionError:
            # print "Warning: 0 curvature"
            self.d0 = 0
        self.phi0 = atan2(-x*signcharge,y*signcharge)
        self.curv /= 2.
        thetaOver2 = (self.ctheta**2-1)/(2*self.ctheta)
        try :
            theta = atan2(self.pt,self.pz)
            self.eta = -log(tan(theta/2.))
        except :
            self.eta = 0
        return None

    def Print(self) :
        print "(curv,phi0,d0,ctheta,z0)=(%f,%f,%f,%f,%f)" % (self.curv, self.phi0, self.d0, self.ctheta, self.z0), "(assoc,barcode,evtID,parentID)=(%d,%d,%d,%d)" % (self.assoc,self.barcode,self.evtindex,self.parentID), "pdg=%d" % self.pdgcode
        return None

class FTKEventStreamer :
    def __init__(self,outFile) :
        # Class to handle event outputing
        self.thisEvent = ""
        self.outFile = outFile

    def clear(self):
        # Clear data for next event
        self.thisEvent = ""

    def addLine(self,line):
        # Clear data for next event
        self.thisEvent += line 

    def write(self, roads=None):
        # output data for next event
        self.outFile.write(self.thisEvent)
        if roads:
            self.writeRoads(roads)
        
    def writeRoads(self,roads):

        for region in roads:        
            if not region:
                continue
            
            for road in region:
                if not road:
                    continue
                self.outFile.write("D\t"+str(road[0])+"\t"+str(road[1])+"\t"+str(road[2])+"\t"+str(road[3])+"\n")


class FTKEvent :
    def __init__(self, outFile=None) :
        # member to store event information
        self.run = -1
        self.event = -1;
        self.raw_hits = []
        self.truth_tracks = []
        self.off_tracks = []
        # flag to control how the events are parsed
        self.ignoreHits = False
        self.ignoreOffline = False
        self.ignoreTruth = False

        # flag to enable caching of input event
        if outFile:
            self.EventStreamer = FTKEventStreamer(outFile)
        else:
            self.EventStreamer = None
            
        return None

    def Reset(self) :
        self.run = -1
        self.event = -1;
        self.raw_hits = []
        self.truth_tracks = []
        self.off_tracks = []
        return None

    
    def ReadFromFile(self,stream) :
        """Read the event from an ascii encoded file"""
        inEvent = False

        if self.EventStreamer:
            self.EventStreamer.clear()

        for line in stream :

            if self.EventStreamer:
                self.EventStreamer.addLine(line)
            
            #elements = line.split()
            if line[0] == "R" :
                self.run = int(line.split()[1])
            elif line[0] == "F" :
                #print self.event
                self.event = int(line.split()[1])
                inEvent = True
            elif line[0] == "S" :
                if not self.ignoreHits :
                    self.raw_hits.append(FTKHit(line.split()[1:]))
            elif line[0] == "E" :
                if not self.ignoreOffline :
                    self.off_tracks.append(FTKOffline(line.split()[1:]))
            elif line[0] == "T" :
                if not self.ignoreTruth :
                    self.truth_tracks.append(FTKTruth(line.split()[1:]))
            elif line[0] == "L" :
                break
        return inEvent

    def WriteToFile(self,roads=None) :
        """Write event to an ascii encoded file"""
        self.EventStreamer.write(roads)

    def filterHitsBarcode(self,barcode) :
        """This function fitlers the hits by barcode"""
        newHitList = []
        for ihit in self.raw_hits : # hits loop
            if ihit.barcode == barcode :
                newHitList.append(ihit)
            #end hits loop
        self.raw_hits = newHitList
        return len(self.raw_hits)

    def IsBarrel(self) :
        """Return True if all the hits are in the barrel"""
        isBarrel = True
        for hit in self.raw_hits :
            if hit.BarEcp != 0 :
                isBarrel = False
                break
        return isBarrel

    def GroupHits(self) :
        """Return the number of planes with hits"""
        plane_list = {}
        for hit in self.raw_hits :
            if hit.layer in plane_list :
                plane_list[hit.layer] += [hit]
            else :
                plane_list[hit.layer] = [hit]
        return plane_list

    def PrintTruthTracks(self) :
        """Print the list of the truth tracks"""
        for track in self.truth_tracks :
            track.Print()
            
class FTKFileChain :
    def __init__(self,filelist,outFile=None) :
        self.filelist = filelist
        self.nfiles = len(filelist)
        self.currentID = -1
        self.event = -1
        self.localevent = -1
        self.doWriteOutput = False
        self.outFile = outFile
        self.FTKEvent = FTKEvent(self.outFile)

        return None

    def open_helper(self,path) :
        print "Open", path
        if path[-3:] == "bz2" :
            print "INFO: Read bz2 compressed data file",
            self.stream = bz2.BZ2File(path,"r")
            print "Done"
        elif path[-2:] == "gz" : 
            print "INFO: Read  gzip compressed data file",
            self.stream = bz2.GzipFile(path,"r")
            print "Done"
        else :
            self.stream = open(path,"r")
        return None

    def nextevent(self) :
        if self.currentID == -1 :
            self.currentID += 1
            print "Opening ", self.filelist[0]
            self.open_helper(self.filelist[0])
        self.FTKEvent.Reset()
        while not self.FTKEvent.ReadFromFile(self.stream) :
            self.currentID += 1
            if self.currentID == self.nfiles :
                print "All wrapper events read", self.filelist
                self.FTKEvent.Reset()
                return False
            self.stream.close()
            self.open_helper(self.filelist[self.currentID])
            self.localevent = -1
        # increment the event counter
        self.event += 1
        self.localevent += 1
        return True

class Constant :
    """This class defines constant"""
    def __init__(self,nplanes,pid=-1) :
        """Create the pattern, passing the number of planes and its ID"""
        # set number of planes
        self.nplanes = nplanes
        if int(nplanes) == 11:
            self.ncoords = 14
        elif int(nplanes) == 12:
            self.ncoords = 16
        # set the Constant-ID
        self.pid = pid
        # set the initial values in the constants
        self.Vc  = [0]*self.ncoords
        self.Vd  = [0]*self.ncoords
        self.Vf  = [0]*self.ncoords
        self.Vz0 = [0]*self.ncoords
        self.Vo  = [0]*self.ncoords
        self.kaverages = [0]*(self.ncoords-5)
        self.kernel = [0]*(self.ncoords-5)*self.ncoords
        self.Cc  = 0
        self.Cd  = 0
        self.Cf  = 0
        self.Cz0 = 0
        self.Co  = 0
        return None

    def SetNPlanes(self,nplanes) :
        """Set the number of plane and sizes the internal structures"""
        self.nplanes = nplanes
        self.ssid = [-1]*nplanes
        return None

    def ReadFromFile(self,fdesc) :
      while self.Co==0:
        line = fdesc.readline()        
        if line.find("sector")!=-1 :
            value = fdesc.readline()
            self.pid = value.split()[0]
        if line.find("Vc")!=-1 :
            for i in xrange(self.ncoords):
                value = fdesc.readline()
                self.Vc[i] = value.split()[0]
        if line.find("Vd")!=-1 :
            for i in xrange(self.ncoords):
                value = fdesc.readline()
                self.Vd[i] = value.split()[0]
        if line.find("Vf")!=-1 :
            for i in xrange(self.ncoords):
                value = fdesc.readline()
                self.Vf[i] = value.split()[0]
        if line.find("Vz0")!=-1 :
            for i in xrange(self.ncoords):
                value = fdesc.readline()
                self.Vz0[i] = value.split()[0]
        if line.find("Vo")!=-1 :
            for i in xrange(self.ncoords):
                value = fdesc.readline()
                self.Vo[i] = value.split()[0]
        if line.find("kaverages")!=-1 :
            for i in xrange(self.ncoords-5):
                value = fdesc.readline()
                self.kaverages[i] = value.split()[0]
        if line.find("kernel")!=-1 :
            for i in xrange((self.ncoords-5)*self.ncoords):
                value = fdesc.readline()
                self.kernel[i] = value.split()[0]
        if line.find("Cc")!=-1 :
            value = fdesc.readline()
            self.Cc = value.split()[0]
        if line.find("Cd")!=-1 :
            value = fdesc.readline()
            self.Cd = value.split()[0]
        if line.find("Cf")!=-1 :
            value = fdesc.readline()
            self.Cf = value.split()[0]
        if line.find("Cz0")!=-1 :
            value = fdesc.readline()
            self.Cz0 = value.split()[0]
        if line.find("Co")!=-1 :
            value = fdesc.readline()
            self.Co = value.split()[0]
      return self.pid

    def PrintToFile(self,stream,newpid='') :
        stream.flush()
        """Print the constant content into a file"""
        stream.write("sectors\n")
        # newpid is for when writing into 7_11 connections so that the 7-11L synchronziation is done
        if newpid!='':
          stream.write(str(newpid)+"\n")
        else:
          stream.write(str(self.pid)+"\n")
        stream.write(" Vc\n")
        for i in self.Vc:
            stream.write(str(i)+"\n")
        stream.write(" Vd\n")
        for i in self.Vd:
            stream.write(str(i)+"\n") 
        stream.write(" Vf\n")
        for i in self.Vf:
            stream.write(str(i)+"\n") 
        stream.write(" Vz0\n")
        for i in self.Vz0:
            stream.write(str(i)+"\n") 
        stream.write(" Vo\n")
        for i in self.Vo:
            stream.write(str(i)+"\n") 
        stream.write(" kaverages\n")
        for i in self.kaverages:
            stream.write(str(i)+"\n") 
        stream.write(" kernel\n")
        for i in self.kernel:
            stream.write(str(i)+"\n") 
        stream.write(" Cc\n")
        stream.write(str(self.Cc)+"\n")
        stream.write(" Cd\n")
        stream.write(str(self.Cd)+"\n")
        stream.write(" Cf\n")
        stream.write(str(self.Cf)+"\n")
        stream.write(" Cz0\n")
        stream.write(str(self.Cz0)+"\n")
        stream.write(" Co\n")
        stream.write(str(self.Co)+"\n")

class ConstantBank :
    def __init__(self,path,mode="r",nplanes=-1) :
        self.mode = mode
        self.stream = None
        self.path = path
        if path[-3:] == "bz2" :
            print "INFO: Read bz2 compressed pattern bank",
            self.stream = bz2.BZ2File(path,mode)
            print "Done"
        elif path[-2:] == "gz" :
            print "INFO: Read  gzip compressed pattern bank",
            self.stream = bz2.GzipFile(path,mode)
            print "Done"
        else :
            self.stream = open(path,mode)

        if mode.find("r")>-1 :
            # read the header of the bank
            line = ""
            while line.find("NPLANES")==-1:
                line = self.stream.readline()
                print " 1",line
            line = self.stream.readline()
            print "2",line
            self.nplanes = line.split()[0]
            while line.find("NSECTORS")==-1:
                line = self.stream.readline()
                print "3",line
            line = self.stream.readline()
            print "4",line
            self.nconstants = int(line.split()[0])
            while line.find("NDIM")==-1:
                line = self.stream.readline()
            line = self.stream.readline()
            self.ndim = line.split()[0]
            self.constants=[]
        else :
            self.nplanes = nplanes
            self.nconstants = 0
            self.ndims = 0
            self.patterns = []
        return None

    def LoadConstants(self,maxgcon=-1,gconmask=None) :
        """Load constants from the given files. "maxgcon" argument, if >-1, limit the number of constants to read, "gconmask" is constant and constant equal to it are loaded, to build this constant mask read the Constant.IsEqual documentation"""
        # load the constants
        ntoread = maxgcon
        if ntoread ==-1 :
            ntoread = self.nconstants
        fraction = ntoread/20
        print "Load %d constants" % ntoread
        for i in xrange(ntoread) :
            if i % fraction == 0 :
                print i
            newgcon = Constant(self.nplanes,i)
            newgcon.ReadFromFile(self.stream)
            self.constants.append(newgcon)
            if str(newgcon.pid) != str( i ):
                print 'Check do they match: ? ',newgcon.pid, i

        #i = 0
        # for line in self.stream :
        #     if i % fraction == 0:
        #         print i
        #     newpatt = Pattern(self.nplanes,i)
        #     newpatt.ReadFromLine(line)
        #     self.patterns.append(newpatt)
        #     i += 1
        print "Done"
        return None

    def FindConstant(self,pid) :
        """Search, using a linear algorithm, if exist an equal pattern. Because the -1 fields are interpreted as "any" multiple matches are possible, the method retuns a list"""
        res = []
        for gcon in self.constants :
            if str(gcon.pid) == str(pid) :
                return gcon
        print 'NOT found!! ??? ? !', pid,self.path,self.constants[int(pid)].pid


        # if not found
        return -1
