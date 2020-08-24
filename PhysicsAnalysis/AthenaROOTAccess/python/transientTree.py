# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##
# @file AthenaROOTAccess/python/transientTree.py
# @author Paolo Calafiura, sss
# @date May 2007
# @brief Set up the transient tree for AthenaROOTAccess.
#

from __future__ import print_function

"""The entry point here is the makeTree function.  Given a (persistent)
Root tree containing Atlas Pool-format data, this will return a new
(transient) tree.  This tree can be accessed like any other, and will
internally run the Atlas T/P conversions, so that you can use the
Atlas transient data types from Root.  The original persistent tree
will be made a friend of the transient tree, so that you can access
the persistent data as well.

Importing this module also does everything else needed to initialize
AthenaROOTAccess.
"""

from builtins import zip
from builtins import object
import re
import ROOT
import cppyy
import string
from AthenaROOTAccess.persTypeToTransType import persTypeToTransType
from AthenaROOTAccess.gaudiSvcExists import gaudiSvcExists
from RootUtils import PyROOTFixes

import ROOT
import glob
import sys
import os


from PyUtils.Helpers import ROOT6Setup
ROOT6Setup()

# Turn off annoying dict auto-generation --- it doesn't work anyway.
ROOT.gInterpreter.ProcessLine(".autodict")

# Make sure the proper dictionaries are loaded.
cppyy.load_library('liblibAtlasSTLAddReflexDict')

# Make sure abstract base classes have streaminfos built.
# Otherwise, we can get crashes from TTree::Scan.

# Prevent AthenaBarCodeImpl from trying to create JobIDSvc.
import os
import uuid
os.environ['_ATHENABARCODEIMPL_JOBUUID'] = uuid.uuid1().hex


# Bump up resource limits as much as possible.
from AthenaCommon.ResourceLimits import SetMaxLimits
SetMaxLimits()
del SetMaxLimits


#############################################################################
# Internal implementation details.
#

#
# Fix up persistent containers so that TTree recognizes them as containers.
#
# If you have a class which derives from a vector, Root will not automatically
# recognize the derived class as a container.  _fix_vec_proxy will take
# care of this.  It uses _get_vec_base to search for a vector base class
# of the supplied class clname.  If one is found, the vector's collection
# proxy is copied to the derived class.
#
def _get_vec_base (cl):
    name = cl.GetName()
    if name.startswith('vector<'):
        return name
    bases = cl.GetListOfBases()
    if not bases:
        return None
    for c in cl.GetListOfBases():
        name = _get_vec_base (c.GetClassPointer())
        if name: return name
    return None
def _fix_vec_proxy (clname):
    cl = ROOT.gROOT.GetClass(clname)
    if not cl: return None
    vecname = _get_vec_base (cl)
    if vecname == None or vecname == clname:
       return
    clv = ROOT.gROOT.GetClass(vecname)
    if not clv: return None
    cl.GetStreamerInfo(0)
    cl.CopyCollectionProxy (clv.GetCollectionProxy())
    return


#
# Helper function to fetch an object in an external tree via a DataLink.
# This will be added to TTreeTrans.
#
def _getViaDL (tt, type, key):
    type2 = type
    if type2[-1] == '>':
        type2 = type2 + ' '
    dlc = getattr(ROOT, "DataLink<%s,DataProxyStorage<%s> >" % 
                  (type,type2))
    dl = dlc (key, tt)
    return dl.getDataPtr()
ROOT.AthenaROOTAccess.TTreeTrans.getViaDL = _getViaDL


#
# Helper function to add a list of ext converters to a branch.
#
def _do_extcnv (br, cnvs):
    for (guid, cnvname) in list(cnvs.items()):
        l = cnvname.split ('.')
        cnv = ROOT
        for ll in l:
            try:
                cnv = getattr (cnv, ll)
            except AttributeError:
                print ("ERROR: Can't find ext converter %s for branch %s" % \
                       (cnvname, br.GetName()))
                return
        cnv = cnv()
        br.addExtCnv (guid, cnv)
        ROOT.SetOwnership (cnv, False)
    return
    


#
# Add a new TBranchTPConvert branch to a transient tree.
#  trans_branch_name - Name of the new transient branch.
#  elem              - DHE for the object.
#  pers_branch_name  - Name of the corresponding persistent branch.
#  trans_type        - Name of the transient data type.
#  trans_tree        - The transient tree to which the new branch
#                      should be added.
#  pers_type         - Name of the persistent data type.
#  pers_tree         - The persistent tree.
#  pers_tree_primary - If we're building an external tree, this is the
#                      primary persistent tree.
#
# Returns the new branch.
#
def _add_to_trans_tree (trans_branch_name,
                        elem,
                        pers_branch_name,
                        trans_type,
                        trans_tree,
                        pers_type,
                        pers_tree,
                        pers_tree_primary = None):

      # Look up the persistent branch.
      # Namespace qualifications are likely dropped in branch names.
      pers_br = pers_tree.GetBranch (pers_branch_name)
      if not pers_br:
          i = pers_branch_name.rfind('::')
          if i >= 0:
              pers_branch_name = pers_branch_name[i+2:]
              pers_br = pers_tree.GetBranch (pers_branch_name)
      if not pers_br:
          print ("ERROR: Can't find branch", pers_branch_name,
                 "in tree", pers_tree.GetName())

      
      # Make the branch.
      trans_br = ROOT.AthenaROOTAccess.TBranchTPConvert.addToTree\
                 (trans_tree,
                  trans_branch_name,
                  elem.key(),
                  elem.pClid(),
                  pers_tree,
                  pers_tree_primary,
                  pers_branch_name,
                  trans_type,
                  pers_type)
      trans_br.SetEntries (trans_tree.GetEntries())

      # Handle schema-evolved xAOD interface classes.
      if trans_branch_name.endswith ('_auxcnv'):
          trans_branch_name_orig = trans_branch_name[:-7]
          aux_branch_name = trans_branch_name_orig + 'Aux.'
          trans_br = ROOT.AthenaROOTAccess.TBranchAlias.addToTree\
            (trans_tree,
             trans_branch_name_orig,
             trans_branch_name_orig,
             elem.pClid(),
             trans_tree,
             trans_branch_name,
             aux_branch_name)

      # Do Root fixups.
      _fix_vec_proxy (pers_br.GetClassName())

      # Need to add any external converters?
      cnvs = _extCnv.get (trans_type)
      if cnvs:
          _do_extcnv (trans_br, cnvs)

      return trans_br


#
# Create a new transient tree.
#  pers_tree - The persistent tree.
#  name_in   - Specify the name for the new tree.
#              If defaulted, we construct a name from the persistent
#              tree name.
#
def _book_trans_tree (pers_tree, name_in = None):
    # Can't create the new TTree in a TFile; otherwise, we may run into
    # a TTreeCache array overflow bug.
    olddir = ROOT.gDirectory
    ROOT.gROOT.Cd(ROOT.gROOT.GetPath())

    # Get a unique name.
    # The root name defaults to the persistent tree name,
    # if it wasn't supplied.
    if name_in == None:
        name_in = pers_tree.GetName() + '_trans'
    name = name_in
    iname = 0
    while ROOT.gROOT.Get (name):
        iname = iname + 1
        name = name_in + '_%d' % iname

    # Create the tree.
    trans_tree = ROOT.AthenaROOTAccess.TTreeTrans (pers_tree, None, pers_tree,
                                                   name, name)

    # Make a directory for any aux transient tree.
    ROOT.gROOT.mkdir (name + '_auxtrees')
    trans_tree.aux_trees = {}

    # Move back to our original directory.
    olddir.cd()

    # Add the persistent tree as a friend.
    #trans_tree.AddFriend (pers_tree, "")
    # This is problematic; for xAOD types, we need to read the pers
    # tree before the trans tree; otherwise, the dv proxy code
    # in xAOD core will clobber the aux store.  So just have
    # TTreeTrans::GetEntry explicitly read the pers tree.

    # Set the number of entries in the tree.
    # ??? FIXME: GetEntries can be slow if pers_tree is a chain.
    trans_tree.SetEntries (pers_tree.GetEntries())

    ROOT.SetOwnership (trans_tree, False)

    return trans_tree


#
# Helper to extract pieces from tokens.  Returns the first matched group
# in compiled regexp pattern, or None.
#
def _getFromToken(pattern, token) :
    m = pattern.search(token)
    if m:
        return m.groups()[1]
    return None


#
# Handle the case of an object that lives in an external tree.
# These trees have names like POOLContainer_ plus the persistent type name.
#
#  elem       - The DHE for this item.
#  file       - The directory object containing the persistent tree.
#  trans_tree - The transient tree which we're constructing.
#  pers_type  - The persistent type of this object.
#  pers_tree  - The (primary) persistent tree.
#  
def _handle_aux_tree (elem, file, trans_tree, pers_type, pers_tree):
    # Find its transient type.
    trans_type = persTypeToTransType (pers_type)
    if not ROOT.gROOT.GetClass (trans_type):
        print ("Warning: Can't find transient class", trans_type, "for persistent class", pers_type)
        return

    # Nothing to do if no conversion required.
    if trans_type == pers_type:
        return

    # Find the external tree.
    aux_pers_tree = file.Get ('POOLContainer_' + pers_type)
    if not aux_pers_tree:
        print ('Warning: Cannot find persistent tree POOLContainer_' + pers_type)
        return

    # The directory in which to create the new aux tree.
    aux_tree_dir = trans_tree.GetName() + '_auxtrees'

    # The new aux tree name.
    aux_tree_name = 'POOLContainer_' + pers_type + '_aux'

    # Change to the aux tree directory.
    olddir = ROOT.gDirectory
    ROOT.gROOT.Cd(ROOT.gROOT.GetPath())
    ROOT.gROOT.Cd(aux_tree_dir)
    try:
        # Is it already there?
        if not ROOT.gDirectory.Get (aux_tree_name):
            # No --- need to make it.
            aux_tree = ROOT.AthenaROOTAccess.TTreeTrans (aux_pers_tree,
                                                         trans_tree,
                                                         pers_tree,
                                                         aux_tree_name,
                                                         aux_tree_name)
            # Add the branch.
            _add_to_trans_tree (trans_type, elem, pers_type, trans_type,
                                aux_tree, pers_type, aux_pers_tree, pers_tree)
            # And remember it.
            trans_tree.aux_trees[aux_tree_name] = aux_tree
    finally:
        # Make sure we set the directory back.
        olddir.cd()

    return


#
# If a separate POOLContainer tree has no keys for the first event
# in the file, then it won't have any DHEs in the first DH, and thus
# the scan over the DH won't build auxtrees for them.  Here, we make
# auxtrees for any remaining separate trees.
#
#  trans_tree - The transient tree which we're constructing.
#  pers_tree  - The (primary) persistent tree.
#
class DHE_dum(object):
    def key(self): return ""
    def pClid(self): return 0
def _handle_other_aux_trees (trans_tree, pers_tree):
    file = pers_tree.GetCurrentFile()
    trans_tree.sawFile (file)
    for obj in file.GetListOfKeys():
        if obj.GetName().startswith('POOLContainer_'):
            pers_type = obj.GetName()[14:]
            if pers_type in ['DataHeaderForm',
                             'basic_DataHeader',
                             'basic_DataHeaderForm']: continue
            _handle_aux_tree (DHE_dum(),file, trans_tree, pers_type, pers_tree)
    return


def _get_dbids (params):
    dbids = []
    for i in params:
        s = params.db_string
        if s.startswith ('[NAME=FID]'):
            ipos = s.find ('[VALUE=')
            if ipos > 0:
                beg = ipos + 7
                end = s.find (']', beg)
                if end > 0:
                    dbids.append (s[beg:end])
    return dbids


def _getTokenField (token, field):
    ipos = token.find ('[' + field + '=')
    if ipos < 0: return None
    beg = ipos + 2 + len(field)
    end = token.find (']', beg)
    if end < 0: return None
    return token[beg:end]

def _get_sections (sections):
    cnts = {}
    for i in sections:
        s = sections.db_string
        
        cnt = _getTokenField (s, 'CNT')
        start = _getTokenField (s, 'START')

        cnts.setdefault (cnt, []).append (int (start))
    return cnts

def _get_links (links):
    tokens = []
    for i in links:
        s = links.db_string
        tokens.append (s)
    return tokens 

#
# Fill in the redirection table for the current file.
#
def _collect_redirections (trans_tree, pers_tree):
    file = pers_tree.GetCurrentFile()
    trans_tree.clearRedirect()

    params = file.Get ('##Params')
    dbids = _get_dbids (params)
    if len (dbids) < 2: return

    sections = file.Get ('##Sections')
    cnts = _get_sections (sections)

    for (c, starts) in list(cnts.items()):
        for (dbid, start) in zip (dbids, starts):
            trans_tree.addRedirect (dbid, c, start)

    return
    
    
#
# Fill in the links table for the current file.
#
def _collect_links (trans_tree, pers_tree):
    file = pers_tree.GetCurrentFile()
    trans_tree.clearLinks()

    links = file.Get ('##Links')
    tokens = _get_links (links)

    i = 2;
    for token in tokens:
        trans_tree.addLink (token, i)
        i += 1;

    return
#
# Handle a data header element for an object in the persistent tree.
#
#  elem         - The DHE for this item.
#  file         - The directory object containing the persistent tree.
#  trans_tree   - The transient tree which we're constructing.
#  pers_type    - The persistent type of this object.
#  pers_tree    - The (primary) persistent tree.
#  branch_names - User-supplied branch name remapping dictionary.
#  aux_stores   - Map of aux store key names to branch names.
#
def _handle_elem (elem, file, trans_tree, pers_type, pers_tree, branch_names,
                  aux_stores):
    key = elem.key()
    # swizzle '/' to '_'
    key = string.replace(key, '/', '_')

    # Find the transient type.
    trans_type = persTypeToTransType (pers_type)

    # Exclude offline -> trigger rules for xaod aux containers.
    if pers_type.startswith ('xAOD::') and pers_type.find ('AuxContainer') >= 0:
       if pers_type.find ('Trig') < 0 and trans_type.find ('Trig') >= 0:
          trans_type = pers_type

    # Look for schema evolution of an xAOD interface class.
    trans_branch_name_suffix = ''
    if pers_type == trans_type and pers_type.startswith ('DataVector<xAOD::'):
        pers_type2 = pers_type[11:-1]
        pos = pers_type2.rfind ('_')
        if pos > 0:
            pers_type2 = pers_type2[:pos] + 'Container' + pers_type2[pos:]
            trans_type2 = persTypeToTransType (pers_type2)
            if trans_type2 != pers_type2:
                pers_type = pers_type2
                trans_type = trans_type2
                trans_branch_name_suffix = '_auxcnv'

    elif (pers_type != trans_type and
          (pers_type.startswith ('DataVector<xAOD::') or
           pers_type.startswith ('DataVector<DMTest::'))):
       trans_branch_name_suffix = '_auxcnv'

    elif (pers_type != trans_type and
          pers_type.startswith ('xAOD::') and
          pers_type.find('_v') >= 0 and
          pers_type.find('Aux') < 0):
       # Schema evolution of standalone interface object.
       trans_branch_name_suffix = '_auxcnv'
       

    if not ROOT.gROOT.GetClass (trans_type):
        print ("Warning: Can't find transient class", trans_type, "for persistent type", pers_type)
        return

    # The persistent branch name,
    if pers_tree.GetBranch(key):
        pers_branch_name = key
    else:
        pers_branch_name =  pers_type + '_' + key

    # Find the name for the transient branch.
    # Start with the SG key.
    trans_branch_name = key

    # Look for a remapping.  First in the built-in set.
    alias = None
    from AthenaROOTAccess.transBranchRemap import transBranchRemap
    remap = transBranchRemap.get ((trans_branch_name, trans_type))
    if remap:
        alias = trans_branch_name
        trans_branch_name = remap

    # Test for a user-supplied remap.
    if trans_branch_name in branch_names:
        trans_branch_name = branch_names[trans_branch_name]

    if trans_type == pers_type:
        # No conversion required/available.
        if pers_type.find ('_p') >= 0 or pers_type.find ('_tlp') >= 0:
            # But there should have been one --- give up.
            if pers_type not in _skipPers:
               print ("Warning: Cannot find transient class for", pers_type)
            else:
               br = pers_tree.GetBranch (pers_branch_name)
               if not br.GetClass().HasInterpreterInfo():
                  pers_tree.SetBranchStatus (pers_branch_name, 0)
            return

        # Persistent branch should be used directly.
        # Root fixes.
        _fix_vec_proxy (pers_type)

        # Allow branch remapping.
        name = trans_branch_name

        # Test for excluded branches.
        if name in _skipBranches:
            #print ("Warning: branch", trans_branch_name, "not working yet; skipped")
            return

        # Make a branch using the SG key name, for ease of use.
        pers_pytype = getattr (ROOT, pers_type, None)
        if not trans_tree.GetBranch (key) or hasattr (pers_pytype, 'setStore'):
            aux_branch_name = aux_stores.get (key + 'Aux.', '')
            trans_br = \
                     ROOT.AthenaROOTAccess.TBranchAlias.addToTree\
                     (trans_tree,
                      name,
                      key,
                      elem.pClid(),
                      pers_tree,
                      pers_branch_name,
                      aux_branch_name)

            cl = ROOT.gROOT.GetClass (pers_type)
            if cl and not hasattr (pers_pytype, 'setStore'):
                ROOT.AthenaROOTAccess.addDVProxy.add (cl)

            # Install other fixups.
            fixup_cl = _fixups.get (trans_type)
            if fixup_cl:
               fixup_cl = getattr (ROOT, fixup_cl)
               fixup = fixup_cl()
               trans_br.setFixup (fixup)
               ROOT.SetOwnership (fixup, False)

        return

    # Set up for P->T conversion.

    # Test for excluded branches.
    if trans_branch_name in _skipBranches:
        #print ("Warning: branch", trans_branch_name, "not working yet; skipped")
        return

    # If this guy requires detector description, skip it if the
    # detector store has not been set up.
    if trans_type in _skipNoDD and not gaudiSvcExists('DetectorStore'):
        return

    # Test for duplicates.
    br = trans_tree.GetBranch (trans_branch_name)
    if br and br.GetTree() == trans_tree:
        oldtype = trans_tree.GetBranch (trans_branch_name).GetClassName()
        oldname = trans_branch_name
        i = 1
        while 1:
            trans_branch_name = oldname + "_%d" % i
            if not trans_tree.GetBranch (trans_branch_name):
                print (("Warning: Branch %s renamed to %s to avoid duplicate" +
                        " name") % (oldname, trans_branch_name))
                print ("  Types: %s %s" % (oldtype, trans_type))
                break
            i = i + 1

    # Make the branch.
    br = _add_to_trans_tree (trans_branch_name + trans_branch_name_suffix,
                             elem, pers_branch_name,
                             trans_type, trans_tree, pers_type, pers_tree)

    if alias:
        trans_tree.addAlias (br, alias)

    return


#############################################################################
# User methods
#


# List of branches to skip because they're not working right,
# for whatever reason.
_skipBranches = []


# List of persistent types which we know have no transient type.
_skipPers = ["Trk::TrackCollection_tlp1",
             "Trk::TrackCollection_tlp2",
             "Trk::TrackCollection_tlp3",
             "Trk::TrackCollection_tlp4",
             "Trk::TrackCollection_tlp5",
             "Trk::SegmentCollection_tlp1",
             "Trk::SegmentCollection_tlp2",
             "BCM_RDO_Container_p0",
             'CosmicMuonCollection_tlp1',
             'MdtTrackSegmentCollection_p2',
             ]

# External converters that we need to add for some types.
# The keys are the type names.
# The values are mappings from converter guids to
# external converter class names.
_extCnv = { 'JetCollection' : {'977E2E76-4DA6-4A4B-87A8-2E41353DB9F4' :
                               'Analysis.JetTagInfoCnv_tlp1',
                               '50E644C1-E247-41B0-B873-416362824A21' :
                               'Analysis.JetTagInfoCnv_tlp2',
                               '8A57BABD-C361-4796-93CD-E8171EF06BC7' :
                               'Analysis.JetTagInfoCnv_tlp3',
                               } }

# Additional fixups to add.
_fixups = { 'DataVector<xAOD::HIEventShape_v1>' :
            'AthenaROOTAccess::HIEventShapeContainer_v1_fixup',

            'xAOD::MuonAuxContainer_v1' :
            'AthenaROOTAccess::MuonAuxContainer_v1_fixup',

            'xAOD::VertexAuxContainer_v1' :
            'AthenaROOTAccess::VertexAuxContainer_v1_fixup',
            }

# List of types to skip if no detector description is available.
_skipNoDD = ["CaloCellContainer",
             "TileRawChannelContainer",
             "TileDigitsContainer"]

# Additional aliases to make.
_addAlias = [('Cone4H1TowerAODJets',   'Cone4H1TowerJets'),
             ('Cone7H1TowerAODJets',   'Cone7H1TowerJets'),
             ('Kt4H1TowerAODJets',     'Kt4H1TowerJets'),
             ('Kt6H1TowerAODJets',     'Kt6H1TowerJets'),
             ('AntiKt4H1TowerAODJets', 'AntiKt4H1TowerJets'),
             ('AntiKt6H1TowerAODJets', 'AntiKt6H1TowerJets'),
             ('AntiKt4TowerAODJets',   'AntiKt4TowerJets'),
             ('AntiKt6TowerAODJets',   'AntiKt6TowerJets'),
             ]


def _makeAddAliases (tree):
    for src, dst in _addAlias:
        br = tree.GetBranch (src)
        if not br: continue
        if tree.GetBranch(dst): continue
        tree.addAlias (br, dst)
    return
    

def _errfunc (s):
    print ("ERROR: ", s)
    return


# A list of all files we're managing.
_allfiles = []


def get_dataheader (file,
                    entry = 0,
                    dhTreeName = 'POOLContainer_DataHeader',
                    dhBranchName = None):
    """Get the DataHeader for entry ENTRY in file FILE.
The DataHeader tree and branch names may optionally be overridden."""

    dh_tree = file.Get(dhTreeName)
    if not dh_tree:
        # Handle changes in AthenaPoolCnvSvc-00-22-23.
        dh_tree = file.Get ('POOLContainer')
    dh_tree.GetEntry(entry)
    if dhBranchName != None:
        dh = getattr( dh_tree, dhBranchName )
    else:
        dh = None
        for dhname in ['DataHeader_p5', 'DataHeader_p4',
                       'DataHeader_p3', 'DataHeader']:
            try:
                dh = getattr( dh_tree, dhname)
            except AttributeError:
                pass
            if dh: break
        if not dh:
            print ("ERROR: Can't find DataHeader branch")
    if dh.__class__.__name__ == 'DataHeader_p5':
        dh = get_DataHeader_p5 (dh_tree)
    return dh


def makeTree(handleTFileOrTree,
             nameIn = None,
             persTreeName='CollectionTree',
             dhTreeName='POOLContainer_DataHeader',
             dhBranchName=None,
             branchNames = {},
             dhfilt = None) :
    """Build a transient tree from a persistent tree.
The new tree will be created in the global Root directory.
The persistent tree will be added as a friend of the new transient tree.

The persistent tree is given by handleTFileOrTree.  This may be
either a TTree/TChain or a directory object.  In the latter case,
the name of the tree is taken from persTreeName (which defaults
to `CollectionTree'.

The name of the new transient tree is taken from nameIn.  If defaulted,
it is taken to be the name of the persistent tree with `_trans' appended
(a number will be appended if required to make the name unique).

We loop through the data header to find the branches to create.
The names of the data header tree and branch are given by
dhTreeName and dhBranchName.

You can override the names used for the transient branches by supplying
the branchNames argument; it should be a mapping of original branch
names to desired branch names.  This may be used to give branches
shorter names to save typing in interactive sessions.

If dhfilt is provided, it will be called for each DataHeaderElement.
It should return a DataHeaderElement object; this may be the same as
the one passed in, or it may be a new, modified one.
If the function returns False, no transient branch will be made for
this entry.  Caution: Any ElementLinks pointing at such a skipped branch
will cause crashes if dereferenced.
"""

    # Find the persistent tree object and its directory.
    if isinstance (handleTFileOrTree, ROOT.TChain):
        pers_tree = handleTFileOrTree
        handleTFile = pers_tree.GetFile()
    elif isinstance (handleTFileOrTree, ROOT.TTree):
        pers_tree = handleTFileOrTree
        handleTFile = pers_tree.GetDirectory()
        _allfiles.append (handleTFile)
    else:
        handleTFile = handleTFileOrTree
        pers_tree = handleTFile.Get(persTreeName)
        _allfiles.append (handleTFile)

    # Make the transient tree.
    trans_tree = _book_trans_tree (pers_tree, nameIn)

    # The GetEntries() call may have invalidated this.
    if isinstance (handleTFileOrTree, ROOT.TChain):
        handleTFile = pers_tree.GetFile()

    # Find the data header.  FIXME: one per event
    dh = get_dataheader (handleTFile, 0, dhTreeName, dhBranchName)
    _collect_redirections (trans_tree, pers_tree)
    _collect_links (trans_tree, pers_tree)

    # Patterns for pulling information out of pool tokens.
    pers_type_re = re.compile ('(' + persTreeName+r')?\(([^/)]+)/')
    aux_tree_re  = re.compile (r'(POOLContainer)_([^ \]]+)')

    # Fix up container field in tokens.
    for elem in dh.elements():
        s = elem.token()
        cnt = _getTokenField (s, 'CNT')
        if cnt == "":
            oid = _getTokenField (s, 'OID')
            offset = oid[0:oid.find ('-')]
            red = trans_tree.getRedirect(_getTokenField(s, 'DB'), "##Links");
            cnt = trans_tree.getLink( int(offset, 16) + red )
            tokenStr = s.replace("[CNT=]", "[CNT=" + cnt + "]");
            elem.setToken(tokenStr)

    # First scan to find aux store names.
    aux_stores = {}
    for elem in dh.elements():
        key = elem.key()
        if key.endswith ('Aux.'):
            if pers_tree.GetBranch (key):
                aux_stores[key] = key
                continue
            pers_type = _getFromToken (pers_type_re, elem.token())
            if pers_type == None:
                br = pers_tree.GetBranch (elem.key())
                if br:
                    pers_type = br.GetClassName()
            if pers_type:
                pers_branch_name = pers_type + '_' + key
                aux_stores[key] = pers_branch_name

    # Loop over elements.
    for elem in dh.elements():
        if dhfilt:
            elem = dhfilt (elem)
            if not elem: continue

        # Look for an element in an external tree.
        pers_type = _getFromToken (aux_tree_re, elem.token())
        if pers_type in ['DataHeader',
                         'basic_DataHeader',
                         'basic_DataHeaderForm']: continue
        if pers_type != None:
            _handle_aux_tree (elem,
                              handleTFile,
                              trans_tree,
                              pers_type,
                              pers_tree)
            continue

        # Handle other elements.
        pers_type = _getFromToken (pers_type_re, elem.token())
        if pers_type == None:
            # Try to get the type directly from the branch.
            br = pers_tree.GetBranch (elem.key())
            if br:
                pers_type = br.GetClassName()
        if pers_type != None:
            _handle_elem (elem,
                          handleTFile,
                          trans_tree,
                          pers_type,
                          pers_tree,
                          branchNames,
                          aux_stores)

    # Take care of separate trees that don't have a DHE in the first event.
    _handle_other_aux_trees (trans_tree, pers_tree)

    # Need to do this after classes are loaded.
    ROOT.DataModelAthenaPool.DataVectorConvert.initialize()
    logger = ROOT.RootUtils.PyLogger (None, _errfunc)
    #ROOT.DataModelAthenaPool.DataProxyStorageConvert.initialize(logger)

    # Make this tree current.
    # This is a hidden side effect, and thus kind of ugly.
    # But there are now some top-level containers that contain DL/EL
    # (such as JetCollection), so this may make things easier for people.
    ROOT.AthenaROOTAccess.TTreeTrans.setCurTree (trans_tree)

    # ParticleJetContainer, JetContainer
    trans_tree.addCLIDAlias (1118613496, 1162448536)

    _makeAddAliases (trans_tree)

    # Setup UserData if available
    # Check the first file where the persistent tree/chain is to see if the UserDataTree exist
    tmpudtree=handleTFile.Get("UserDataTree")
    if not trans_tree.GetFriend ('CollectionTree'):
        # The UD code hardcodes the `CollectionTree' name.
        # Prevent crashes.
        tmpudtree = None
    if tmpudtree: #UserDataTree exist, do more test
        from AraTool.PyUserDataAraReader import PyUserDataAraReader
        pread=PyUserDataAraReader(trans_tree)
        ud=pread.getUserDataTree()
        if ud!=0:
            print ("Adding UserData...")
            trans_tree.UD_Available=True
            
            trans_tree.UD_help=pread.getHelp
            trans_tree.UD_DumpDecorationInfo=pread.DumpDecorationInfo
            trans_tree.UD_getReader=pread
            trans_tree.UD_reader=pread

            trans_tree.UD_getDecoration=pread.getDecoration
            trans_tree.UD_getEventDecoration=pread.getEventDecoration
            trans_tree.UD_getCollectionDecoration=pread.getCollectionDecoration
            trans_tree.UD_getFileDecoration=pread.getFileDecoration
            trans_tree.UD_getListOfDecorations=pread.getAssociations
            trans_tree.UD_getAssociations=pread.getAssociations
            trans_tree.UD_BranchNames=[pread.getDecorationBranchName(x) for x in pread.getAssociations()]
            
            #Element level API
            trans_tree.UD_getElementDecoration=pread.getElementDecoration
            trans_tree.UD_getElListOfLabel=pread.getElListOfLabel
            trans_tree.UD_getElListOfABC=pread.getElListOfABC   
        else:
            trans_tree.UD_Available=False
    else:
        trans_tree.UD_Available=False
                
    return trans_tree 

def getFullTree(f, persTreeName='CollectionTree') :
    """ Return a reference to the persTreeName tree in file f.
    Usually this would be the original, persistent tree in the file, to
    which makeTree() added the transient object branches"""
    return f.Get(persTreeName)


#
# Dump out the data header (for debugging).
#
def dumpDH(f,
           ent=0,
           dhTreeName='POOLContainer_DataHeader',
           dhBranchName='DataHeader_p3'):
    dhTree = f.Get(dhTreeName)
    dhTree.GetEntry(ent)
    dh = getattr( dhTree, dhBranchName )
    for elem in dh.elements() :
        print (elem.key(), elem.token(), elem.pClid())
    return


#############################################################################
# DataHeader_p5 support.
#


class DataHeaderElement_p5(object):
    def __init__ (self, key, token, pClid, hashes):
        self._key = key
        self._token = token
        self._pClid = pClid
        self._hashes = hashes
        return
    def key(self):
        return self._key
    def token(self):
        return self._token
    def setToken(self, token):
        self._token = token
        return
    def pClid(self):
        return self._pClid
    def hashes(self):
        return self._hashes


class DataHeader_p5(object):
    def __init__ (self, elts):
        self._elts = elts
        return
    def elements(self):
        return self._elts


dhp5_cnt_pattern = re.compile (r'CNT=([^]]*)')
def get_DataHeader_p5 (dh_tree):
    try:
        dhp = getattr (dh_tree, 'DataHeader_p5')
    except AttributeError:
        try:
            dhp = getattr (dh_tree, 'DataHeader')
        except AttributeError:
            return None

    token = dhp.dhFormToken()
    m = dhp5_cnt_pattern.search (token)
    if not m:
        print ("ERROR: Can't find DataHeaderForm tree name from token!")
        return None

    formtree = m.groups()[0]
    formbranch = 'DataHeaderForm_p5'

    ii = formtree.find ('(')
    if ii > 0:
        formbranch = formtree[ii+1:-1]
        formtree = formtree[:ii]


    # ??? Form location is hardcoded here --- not using token!
    f = dh_tree.GetTree().GetDirectory()
    formt = f.Get(formtree)
    if not formt:
        print ("ERROR: Can't find DataHeaderForm tree!")
        return None
    formt.GetEntry(0)

    try:
        dhform = getattr (formt, formbranch)
    except AttributeError:
        print ("ERROR: Can't find DataHeaderForm branch", formbranch)
        return None

    dhp.setDhForm (dhform)

    tdh = ROOT.DataHeader()
    cnv = ROOT.DataHeaderCnv_p5()
    cnv.persToTrans (dhp, tdh)
    elts = []
    for dhe in tdh.elements():
        tokenStr = dhe.getToken().toString()
        elts.append (DataHeaderElement_p5 (dhe.getKey(),
                                           tokenStr,
                                           dhe.getPrimaryClassID(),
                                           list(dhe.getHashes())))
    return DataHeader_p5 (elts)


#############################################################################
# Root fixes/workarounds.
#


# Make sure this is loaded, so we can call it from the C++ converters.
ROOT.TDatabasePDG

# Initialize the history, except if we're using the CINT shell.
if ROOT.gApplication.GetName() != "TRint":
    ROOT.RootUtils.InitHist.initialize()

# Set up RootConversions converters.
#reg=ROOT.TConverterRegistry.Instance()
#reg.AddConverter ("TauJetContainer_p1_old_cnv")
#reg.AddConverter ("TrigSpacePointCounts_p1_old_cnv")
#if hasattr(ROOT, 'TrigInDetTrackTruthMap'):
#    ROOT.TrigInDetTrackTruthMap
#reg.AddConverter ("TrigInDetTrackTruthMap_old_cnv")


# Enable TTree speedups.
PyROOTFixes.enable_tree_speedups()

# Enable container conversions.
import ROOT
ROOT.TConvertingStreamerInfo.Initialize()
#ROOT.DataModelAthenaPool.CLHEPConverters.initialize()
ROOT.RootConversions.VectorConverters.initialize()
cppyy.load_library('liblibDataModelAthenaPool')
ROOT.DataModelAthenaPool.CLHEPConverters
ROOT.DataModelAthenaPool.installPackedContainerConverters()
if hasattr(ROOT, 'CaloEnergyCnv_p2_register_streamer'):
   ROOT.CaloEnergyCnv_p2_register_streamer.reg()

ROOT.TConvertingBranchElement.SetDoDel (True)

#
# Call Reset() on all instances of TChainROOTAccess before exiting.
# This is needed because PyROOT destroys all TFile instances on exit;
# but if that happens while TChain still owns a file, we'll get
# a double-delete.
# Works around root bug 45789
#
import atexit
def _reset_chains():
    ROOT.AthenaROOTAccess.TChainROOTAccess.ResetAll()
    return
atexit.register(_reset_chains)


#
# Root has a global dtor ordering problem: the cintex trampolines
# may be deleted before open files are closed.  Workaround is to explicitly
# close open files before terminating.
#
def _close_files():
    for f in _allfiles:
        if hasattr (f, 'Close'): f.Close()
    del _allfiles[0:-1]
    return
atexit.register(_close_files)
