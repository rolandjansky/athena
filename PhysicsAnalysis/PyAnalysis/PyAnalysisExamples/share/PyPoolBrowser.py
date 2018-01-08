from idlelib  import TreeWidget
from PyKernel import PyKernel
from Tkinter  import *

import re
import time

# import PyDataHeader and PyCLIDSvc
import cppyy
cppyy.loadDictionary("PyAnalysisCoreDict")

#############################################################################

# EDM item
class EDMItem (TreeWidget.TreeItem):
    # name : name of the object
    # ref  : reference to the object
    # expandable : whether the object may be a leaf object
    # classKey : key of the class to which this obj belongs
    def __init__ (self, name, ref, expandable, classKey=None):
        self.name = name
        self.ref  = ref
        self.expandable = expandable
        self.classKey = classKey

    def GetText (self):
        return self.name

    def IsExpandable (self):
        return self.expandable

    def GetIconName(self):
        if not self.IsExpandable():
            return "python" # XXX wish there was a "file" icon

    # execute plot command when double-click
    def OnDoubleClick(self):
        # not a leaf
        if self.IsExpandable():
            return
        # doesn't belong to a class
        if self.classKey == None:
            return
        # draw histogram
        x = eval ('PyKernel.plot ("%s#%s", "$x.%s()")' % (objList[self.classKey], self.classKey, self.name))
        # sleep is needed to draw up canvas
        time.sleep(1)
        # return is also needed. see PyKernel.plot  
        return x
        
    # return list of items which belong to the object
    def GetSubList (self):
        sublist = []
        # if a vector-like class
        if hasattr(self.ref,'size') and hasattr(self.ref,'__getitem__'):
            # use the fist one
            if self.ref.size() != 0:
                item = EDMItem ('%s constituents' % self.ref.size(),self.ref[0],True,self.classKey)
                sublist.append(item)
            return sublist
        #others
        else:
            attrs=dir(self.ref)
            attrs.sort()
            for attr in attrs:
                # private 
                if re.search("\A_",attr):
                    continue
                # member
                if re.search("\Am_",attr):
                    continue
                # method
                attrRef = getattr(self.ref,attr)
                item = EDMItem (attr,attrRef,False,self.classKey)
                sublist.append(item)
            return sublist

# Root Item
class RootItem (EDMItem):
    # override GetSubList
    def GetSubList (self):
        sublist = []
        # get EDM objects
        attrs=dir(self.ref)
        attrs.sort()
        for attr in attrs:
            # private 
            if re.search("\A_",attr):
                continue
            # else
            attrRef = getattr(self.ref,attr)
            item = EDMItem (attr,attrRef,True,attr)
            sublist.append(item)
        return sublist

#############################################################################
    
# run 1 event to get DataHeader            
theApp.run(1)

# get EDM/Key list from DataHeader
dh = g.PyDataHeader()
objList = {}
for i in range(dh.size()):
    objList[dh.getKey(i)] = dh.getName(i)

# build Root object
class MyRootObj:
    pass

rootObj = MyRootObj()

# attach EDM objects to Root object if they are not in the ignore list
for key in objList.keys():
    name = objList[key]
    try:
        exec 'con = PyKernel.retrieve(g.%s,"%s")' % (name, key)
        setattr(rootObj,key, con)
    except:
        pass

# start Tk session
root=Tk()
root.title(os.path.basename(sys.argv[0]))
root.configure(bd=0)

sc = TreeWidget.ScrolledCanvas(root, bg="white", highlightthickness=0, takefocus=1)
sc.frame.pack(expand=1, fill="both")
item = RootItem('/',rootObj,True)
item.GetText()
node = TreeWidget.TreeNode(sc.canvas, None, item)
node.update()

root.mainloop()
