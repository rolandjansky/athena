# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from logging import getLogger; log = getLogger("DQDefects.virtual_defect_mixin")
from DQUtils import fetch_iovs

from .exceptions import DefectUnknownError, DefectExistsError
from .ids import choose_new_defect_id
from .virtual_logic import DefectLogic

NONHEAD_MODIFICATION_MSG = ("Operations which modify virtual defects can only "
    "be done on the HEAD tag.")
    
def _resolve_dependencies(node, resolved=None, unresolved=None):
    """
    Navigate the node to determine dependencies. Circular references raise an 
    exception.
    
    Shamelessly stolen algorithm from:
    http://www.electricmonk.nl/log/2008/08/07/dependency-resolving-algorithm/
    """
    if resolved is None: resolved = []
    if unresolved is None: unresolved = []
    unresolved.append(node)
    for dependency in node.dependencies:
        if dependency not in resolved:
            if dependency in unresolved:
                raise Exception('Circular reference detected: %s -> %s'
                                % (node.name, dependency.name))
            _resolve_dependencies(dependency, resolved, unresolved)

    resolved.append(node)
    unresolved.remove(node)
    return resolved   


class DefectsDBVirtualDefectsMixin(object):
    """
    A DefectsDB mixin for managing virtual defects 
    """
    
    def __init__(self):
        self._virtual_defect_logics = None
        super(DefectsDBVirtualDefectsMixin, self).__init__()

    def validate_clauses(self, clauses):
        all_defects = self.defect_names | self.virtual_defect_names
        for clause in clauses.split():
            if clause[0] in ('!', '-'):
                clause = clause[1:]
            assert clause in all_defects, (clause + " is not a known defect")
    
    def update_virtual_defect(self, defect_name, clauses, comment=None):
        # HEAD protection is here, to allow other functions to
        # change other tags in a known manner with _update_virtual_defect
        assert self.logics_tag == "HEAD", NONHEAD_MODIFICATION_MSG
        assert not self._read_only, "Insertion on read-only database"
        clauses = clauses.encode('ascii')
        comment = comment.encode('utf-8') if comment is not None else comment
        self._update_virtual_defect(defect_name, clauses, comment)
    
    def _update_virtual_defect(self, defect_name, clauses, comment=None, tag=None):
        if not tag: tag = 'HEAD'
        defect_id = self.defect_chan_as_id(defect_name)
        
        assert self.defect_is_virtual(defect_id), ("Tried to update nonvirtual"
            " defect with update_virtual_defect()")

        self.validate_clauses(clauses)
        
        self._defect_logic_payload["clauses"] = clauses
        
        store = self.defect_logic_folder.storeObject
        store(0, 2**63-1, self._defect_logic_payload, defect_id, tag,
              (True if tag != 'HEAD' else False))
        
        if comment:
            self.defect_logic_folder.setChannelDescription(defect_id, comment)

    def rename_defect(self, defect_name, new_defect_name):
        """
        Rename a defect (primary or virtual).  Will keep data and channel ID.
        Will fix up all virtual defect dependencies in all tags.
        """
        assert not self._read_only, "Channel rename on read-only database"
        defect_name = defect_name.encode('ascii')
        new_defect_name = new_defect_name.encode('ascii')
        
        try:
            oldname = self.normalize_defect_names(new_defect_name)
            already_exists = True
        except DefectUnknownError:
            already_exists = False
        if already_exists:
            raise DefectExistsError('Defect %s already exists' % oldname)
        
        defect_id = self.defect_chan_as_id(defect_name)
        
        if self.defect_is_virtual(defect_id):
            self.defect_logic_folder.setChannelName(defect_id, new_defect_name)
            self._virtual_defect_names = None
            self._virtual_defect_id_map = None
            self._virtual_defect_logics = None
        else:
            self.defects_folder.setChannelName(defect_id, new_defect_name)
            self._defect_names = None
            self._defect_id_map = None
        
        import contextlib
        @contextlib.contextmanager
        def logics_unlocking(tag):
            log.info('Unlocking tag %s', tag)
            folder = self.defect_logic_folder
            orig_status = folder.tagLockStatus(tag)
            if orig_status != 0:
                folder.setTagLockStatus(tag, 0)
            yield
            if orig_status != 0:
                folder.setTagLockStatus(tag, orig_status)
            log.info('Done with tag %s', tag)

        def logic_substitute(clause, oldname, newname):
            oclause = clause
            prefix = ''
            if clause[0] in ('!',):
                prefix = clause[0]
                clause = clause[1:]
            if clause != oldname:
                return oclause
            else:
                return prefix + newname
        
        for tag in ['HEAD'] + self.logics_tags:
            logics = self._get_virtual_defect_logics(tag)
            for vd, logic in logics.items():
                if defect_name in logic.realclauses:
                    newclauses = ' '.join([logic_substitute(x, defect_name,
                                                            new_defect_name)
                                           for x in logic.clauses
                                           ])
                    with logics_unlocking(tag):
                        self._update_virtual_defect(vd, newclauses, tag=tag)
        self._virtual_defect_logics = None
    
    def new_virtual_defect(self, defect_name, comment, clauses):
        """
        Create a new virtual defect
        """
        assert self.logics_tag == "HEAD", NONHEAD_MODIFICATION_MSG
        assert not self._read_only, "Insertion on read-only database"
        from DQUtils.channel_mapping import get_channel_ids_names
        clauses = clauses.encode('ascii')
        defect_name = defect_name.encode('ascii')
        comment = comment.encode('utf-8') if comment is not None else comment
        
        # Force load of defects_folder to populate _defect_payload
        store = self.defect_logic_folder.storeObject
        p = self._defect_logic_payload
        
        self.validate_clauses(clauses)

        p["clauses"] = clauses

        # need to get true mapping of folder, not just what is visible
        # from this tag
        ids, names, mapping = get_channel_ids_names(self.defect_logic_folder)
        defect_id = choose_new_defect_id(mapping, defect_name, True)
        log.debug("Creating new defect %s: 0x%08x", defect_name, defect_id)
        
        try:
            oldname = self.normalize_defect_names(defect_name)
            already_exists = True
        except DefectUnknownError:
            already_exists = False
        if already_exists:
            raise DefectExistsError('Defect %s already exists' % oldname)

        if (defect_id in ids):
            raise DefectExistsError(defect_name)
        self.defect_logic_folder.createChannel(defect_id, defect_name, comment)
        store(0, 2**63-1, p, defect_id)
        self._new_virtual_defect(defect_id, defect_name)
        return defect_id
    
    def _resolve_evaluation_order(self, defects=None):
        """
        Returns a list of DefectLogic objects that need to be evaluated, 
        in the correct order for them to be consistent.
        
        `defects` should be a list of names
        """
        if defects is None:
            logics = self.virtual_defect_logics.items()
        else:
            # flags begins as a list of names.
            # Make it a list of logic objects instead
            # If it is not in the logics map, the logic is not defined for the
            # query interval and we shouldn't try to resolve it.
            ##flags = [self.logics[f] for f in flags if f in self.logics]
            
            logics = [self.virtual_defect_logics[d] for d in defects]
        
        class MasterNode:
            dependencies = logics
            
        resolved = _resolve_dependencies(MasterNode)
        resolved.remove(MasterNode)
        return resolved
    
    def resolve_primary_defects(self, defect_logics):
        """
        Determine which primary flags are used for 
        a given list of input `virtual_defect_names`.
        """
        primary_defects = set()
        for defect_logic in defect_logics:
            primary_defects |= defect_logic.primary_defects
            
        return primary_defects
    
    @property
    def virtual_defect_logics(self):
        """
        Returns all virtual defects in the form {"defect_name" : DefectLogic()}
        for the tag DefectDB was constructed on.
        """
        if self._virtual_defect_logics is not None:
            return self._virtual_defect_logics
                
        self._virtual_defect_logics = self._get_virtual_defect_logics(self.logics_tag)
        
        return self._virtual_defect_logics

    def _get_virtual_defect_logics(self, tag):
        if tag != "HEAD" and not self.defect_logic_folder.existsUserTag(tag):
            # The tag doesn't exist, so there is nothing to fetch.
            return {}
        
        logics = fetch_iovs(self.defect_logic_folder, tag=tag, 
                            named_channels=True)
                            
        logics = dict((l.channel, DefectLogic(l)) for l in logics)
        
        for defect_name, defect_logic in logics.iteritems():
            defect_logic._populate(logics)
            
        return logics

    def get_intolerable_defects(self, old_primary_only=True,
                                exclude=['TIGHT', 'IDTIGHT', 'PHYS_.*']):
        """
        Returns primary defects that are depended on by a virtual defect
        if old_primary_only == True, only return those depended on by a
        virtual defect with no _ in the name
        ignores virtual defects in exclude list
        accepts regexes in exclude list
        """
        vdl = self.virtual_defect_logics
        if old_primary_only:
            vdl = dict(l for l in vdl.items() if '_' not in l[0])
        if len(exclude) > 0:
            import re
            rex = re.compile('(%s)' % '|'.join(exclude))
            vdl = dict(l for l in vdl.items() if not rex.match(l[0]))
        return self.resolve_primary_defects(vdl.values())
        
    def _virtual_defect_consistency_check(self):
        """
        When called, uses an assertion to check that there are no missing 
        defects. This is a database consistency check which should never be
        violated (but was when DQDefects was new).
        """
        all_defects = self.defect_names | self.virtual_defect_names
        used_defects = set()
        
        used_by = {}
        for defect, logic in self.virtual_defect_logics.iteritems():
            used_defects.update(logic.realclauses)
            for clause in logic.realclauses:
                used_by.setdefault(clause, []).append(defect)
                
        missing_defects = used_defects - all_defects
        
        if missing_defects:
            log.error("-- The missing defects are used by the following virtual defects")
            log.error("<missing> : <used by>")
            for missing in missing_defects:
                log.error(missing + ":" + ", ".join(used_by[missing]))
            
        assert not missing_defects, ("The following defects are used but not "
                                     "defined anywhere: %r" % missing_defects)
        return True
