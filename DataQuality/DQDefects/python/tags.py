# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from logging import getLogger; log = getLogger("DQDefects.tags")

from PyCool import cool

from DQUtils import IOVSet

from DQDefects import DEFECT_LOGIC_TAG_FORMAT

from .exceptions import InvalidTagError, InvalidDefectTagError, InvalidLogicTagError

from .virtual_mixin import NONHEAD_MODIFICATION_MSG

from typing import List, Optional, Iterable

import collections
tagtype = collections.namedtuple('tagtype', ['defects', 'logic'])

class DefectsDBTagsMixin(object):
    def __init__(self) -> None:
        super(DefectsDBTagsMixin, self).__init__()
        
        self.check_tag_validity()

    @property
    def logics_tags(self) -> List[str]:
        """
        Returns a list of existing logic tags
        """
        return [str(t) for t in self.defect_logic_folder.listTags()]

    @property
    def defects_tags(self) -> List[str]:
        """
        Returns a list of existing defect tags
        """
        return [str(t) for t in self.defects_folder.listTags()]
    
    @property
    def next_logics_tag(self) -> str:
        """
        Gives the next available DEFECTLOGICS tag
        """
        existing_tags = self.logics_tags
        if existing_tags:
            new_id = max(int(str(t).split("-")[-1]) for t in existing_tags) + 1
        else:
            new_id = 0
        return DEFECT_LOGIC_TAG_FORMAT % new_id
    
    def _tag_head_and_lock(self, folder: cool.IFolder, name: str, description: str) -> None:
        """
        Give the current HEAD of `folder` a new tag and lock it.
        """
        LOCKED = cool.HvsTagLock.LOCKED
        aname = name.encode('ascii')
        folder.cloneTagAsUserTag('HEAD', aname, description.encode('utf-8'))
        folder.setTagLockStatus(aname, LOCKED)
    
    def new_hierarchical_tag(self, defects_tag: str, logics_tag: str) -> str:
        """
        Create a new heirarchical tag relating the defects and logics
        
        New tag is of the form
        "DetStatus-[Logic revision number]-[Defect tagname]"
        and has description
        "(v%i) blah"
        """
        logic_revision = int(logics_tag.split("-")[-1])
        defect_part = "-".join(defects_tag.split("-")[1:])
        hierarchical_tag = "DetStatus-v%02i-%s" % (logic_revision, defect_part)
        logicspart = "(%i) " % logic_revision
        
        adefects_tag = defects_tag.encode('ascii')
        alogics_tag = logics_tag.encode('ascii')
        ahierarchical_tag = hierarchical_tag.encode('ascii')
        logicspart = logicspart.encode('ascii')
        defect_descr = self.defects_folder.tagDescription(adefects_tag).encode('utf-8')
        
        # Protection here against making descriptions too long
        description = logicspart + defect_descr[:255 - len(logicspart)]
        
        parent_folder = self.parent_folderset
        self.defects_folder.createTagRelation(ahierarchical_tag, adefects_tag)
        self.defect_logic_folder.createTagRelation(ahierarchical_tag, alogics_tag)
        parent_folder.setTagDescription(ahierarchical_tag, description)
        
        log.info("New hierarchical tag %s", hierarchical_tag)
        return hierarchical_tag
    
    @property
    def defects_tag_valid(self) -> bool:
        try:
            self.defects_tag
        except InvalidTagError:
            return False
        else:
            return True
            
    @property
    def logics_tag_valid(self) -> bool:
        try:
            self.logics_tag
        except InvalidTagError:
            return False
        else:
            return True
    
    def check_tag_validity(self) -> None:
        """
        Ensure that the tags that this DefectsDB instance was constructed with
        are functional
        """
        if not (self.defects_tag_valid or self.logics_tag_valid):
            raise InvalidTagError("Tag doesn't resolve: {0}".format(self._tag))
    
    @property
    def defects_tag(self) -> str:
        """
        Return the tag used for retrieving defects on this DefectsDB instance
        """
        if self._tag.defects == "HEAD": return "HEAD"
        try:
            return self.defects_folder.resolveTag(self._tag.defects)
        except Exception: 
            if self._read_only:
                raise InvalidDefectTagError(self._tag.defects)
            # assume user knows what they're doing (e.g. writing to fresh tag)
            return self._tag.defects
        
    @property
    def logics_tag(self) -> str:
        """
        Return the tag used for retrieving virtual defect clauses
        """
        if self._tag.logic == "HEAD": return "HEAD"
        try:
            return self.defect_logic_folder.resolveTag(self._tag.logic)
        except Exception:
            if self._read_only:
                raise InvalidLogicTagError(self._tag.logic)
            # assume user knows what they're doing (e.g. writing to fresh tag)
            return self._tag.logic
    
    @property
    def tag(self) -> tagtype:
        """
        Readonly property stating the tag that the database was instantiated on
        """
        return self._tag
    
    @property
    def tags(self) -> List[str]:
        """
        The list of tags which are on the database
        """
        return [str(t) for t in self.parent_folderset.listTags()]
    
    def new_logics_tag(self, description: str = "") -> str:
        """
        Create a new tag for the logic folder and lock it.
        
        Parameters:
            `description` : What changed in this tag? (optional, default "")
        """
        assert self.logics_tag == "HEAD", NONHEAD_MODIFICATION_MSG
        
        new_tag_name = self.next_logics_tag
        folder = self.defect_logic_folder
        self._tag_head_and_lock(folder, new_tag_name, description)
        log.info("Tagged DEFECTLOGICS HEAD with %s", new_tag_name)
        return new_tag_name
    
    def new_defects_tag(self, name: str, description: str, iovranges: Optional[Iterable] = None) -> str:
        """
        Clones the current DEFECTS tag (specified in the constructor) to a new one
        If iovranges != None, does the slower thing of copying IOVs one by one
        
        Parameters:
            `name` : Name of the new tag
            `description` : Description of the contents of this tag
        """
        if name.startswith("DetStatus"):
            raise RuntimeError("Only specify the last part of the defect tag")
        adescription = description.encode('utf-8')
        
        log.info("Creating new tag %s", name)
        
        name = f"DetStatusDEFECTS-{name}"
        aname = name.encode('ascii')
        
        if iovranges is None:
            self.defects_folder.cloneTagAsUserTag(self.defects_tag.encode('ascii'), aname, adescription)
            return name
        
        # Fetch all primary defects relevant to `iovranges`
        kwargs = dict(primary_only=True, nonpresent=True, intersect=True)
        iovsets = (self.retrieve(*iovrange, **kwargs) for iovrange in iovranges)
        to_copy = IOVSet.from_iovsets(iovsets)
        
        log.info("Copying IoVs: %r", to_copy)
        
        with self.storage_buffer:
            for iov in to_copy:
                self._insert_iov(iov, name)
        
        # If there are no IOVs to copy, there is no new tag to describe
        if to_copy:
            self.defects_folder.setTagDescription(aname, adescription)
            
        return name
