/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

@section Introduction Introduction

AthenaROOTAccess allows accessing Athena POOL data directly from Root, without
using the Athena framework.  Note, however, that it uses the transient
classes and converters from the Atlas software, so a substantial portion
of the offline tree is required.  It currently works with most of the
Athena AOD classes.  Other classes have not been tested.  

There is a <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/AthenaROOTAccess">wiki page</a> for this package.
See there for details.

@section overview Implementation overview.

The function @c transientTree.makeTree is responsible for setting things
up. The @c CollectionTree @c TTree from the Pool file is called the
``persistent'' tree. The @c makeTree() function creates a new, in-memory
``transient'' tree, to which the persistent tree is made a
friend. Then, for every data object in the persistent tree for which
there is a T/P converter, @c makeTree will create a new branch of type
@c TBranchTPConvert and add it to the transient tree. A @c TBranchTPConvert
derives from @c TBranchObject and holds an instance of the transient
object type. When @c TBranchObject::GetEntry() is called, it first forwards
the @c GetEntry to its corresponding branch in the transient tree, and
then calls the T/P converter to copy from the persistent object in the
branch in the persistent tree to the transient object managed by the
@c TBranchTPConvert object. Allowing the transient objects to be managed
by a TBranch is what allows for the standard Root operations such as
@c Draw() and @c Scan() to work with the transient data.

@c ElementLink dereferences are handled by the @c TTreeTrans class,
which implements the @c IProxyDict
interface. This is then installed as the default data source to be
picked up during @c ElementLink initialization. @c TTreeTrans then
looks up the @c ElementLink key in the data header and constructs a
@c DataProxy referencing the target data object. Normally, the data
header information is cached; however, for the case of an @c ElementLink
reference to an object that is stored in a separate pool container
(separate @c POOLContainer_ tree), the data header needs to be read and
searched for each event. This may be slow.

Here's a list of the classes in this package.

<ul>
 <li>@c TBranchTPConvert: This is the object that handles persistent->transient
     conversions.  It derives from @c TBranchObject.  It contains
     an instance of the transient object; it also holds an instance
     of the appropriate converter.  It maintains a reference to the
     corresponding persistent branch.  When @c GetEntry() is called,
     it first forwards the call to the persistent branch, then runs
     the persistent->transient conversion to copy from the persistent
     object to the transient object.

     @c TBranchTPConvert derives from @c TBranchObject, so Root can
     treat it like any other branch.

     Some extra work is required for the case of top-level converters.
     See the source for details.

     Each @c TBranchTPConvert instance maintains its own memory pool,
     based on the Arena classes from DataModel.
     This pool is made active when the branch's converter is running,
     and it is cleared before reading a new entry.

     @c TBranchTPConvert is also responsible for making sure that
     its parent @c TTreeTrans is installed as the default data source
     while its converter is running.

 <li>@c TBranchAlias: This is a branch which simply references another one.
     It can be used to refer to a branch with an alternate name.
     It's used for the case of non-T/P separated classes; in that case,
     we make an alias branch using the SG key as a name.  In this way,
     the user can refer to an object in the same way, regardless of whether
     or not the class has been T/P separated.

 <li>@c TTreeTrans: This is the transient tree object; it derives from
     @c TTreeBranchMap (which in turn derives from @c TTree).  It holds
     the @c TBranchTPConvert and @c TBranchAlias objects.  It implements
     the @c IProxyDict interface, allowing finding a @c DataProxy object
     given a SG key/CLID.  (Much of the work of this is factored out
     into the @c ProxyMap class.)

     (At most) one instance of this class is designated as current.
     When an @c ElementLink is constructed, the current @c TTreeTrans
     is taken as its data source.

 <li>@c TTreeBranchMap: This is a base class for @c TTreeTrans; it derives
     in turn from @c TTree.  This handles mapping from persistent
     to transient branches, allowing finding a transient branch
     given the persistent one.  This is broken out from @c TTreeTrans
     to prevent dependency cycles.

 <li>@c ProxyMap: This class factors out most of the guts of @c ElementLink
     dereferences from @c TTreeTrans.  It maintains the map from
     (key,clid) pairs to @c DataProxy instances.

 <li>@c TChainROOTAccess: This derives from @c TChain, and must be used
     instead of @c TChain for AthenaROOTAccess.  It works around a bug
     in Root's @c TChain::GetReadEntry() for Root versions before 5.16.

 <li>@c DataBucketVoid: This is a @c DataBucket class which can hold
      a dynamic type.  This is what the @c DataProxy objects we build
      reference.  (In Athena, the @c DataBucket classes are templated
      on the object type.  We avoid having to template by using
      information from the Root dictionary instead.)

 <li>@c DVCollectionProxy: This is a Root collection proxy that will work
     for @c DataVector classes.  We need to use this so that @c TTree::Draw()
     and friends will recognize @c DataVector classes as collections.

 <li>@c ELStreamer and @c ScanForEL: When a (non-T/P separated) @c ElementLink
     class is read, we need to call @c toTransient() and @c source() on it.
     These classes set up Root streamers so that that happens automatically.
</ul>

*/
