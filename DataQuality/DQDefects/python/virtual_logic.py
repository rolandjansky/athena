# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from __future__ import annotations

from DQDefects import DEFECT_IOV
from functools import reduce

from typing import Mapping

class DefectLogic(object):
    def __init__(self, defect_iov) -> None:
        self.name = defect_iov.channel
        self.clauses = defect_iov.clauses.split()
        self.operations = set()
        self.inverting = set()
        self._operations_parse()
        self.realclauses = set(_[1] for _ in self.operations)
        self.primary_defects = set()
        self.evaluate = self.bad_evaluator
        
    def bad_evaluator(self, states) -> DEFECT_IOV:
        assert False, "bad_evaluation should never be called"
    
    def set_evaluation(self, full: bool) -> None:
        if full:
            self.evaluate = self.evaluate_full
        else:
            self.evaluate = self.evaluate_partial

    def _operations_parse(self) -> None:
        from operator import truth, not_
        for name in self.clauses:
            if name[0] == '!':
                self.operations.add((not_, name[1:]))
                self.inverting.add(name[1:])
            else:
                self.operations.add((truth, name))
    
    def _populate(self, defect_logics: Mapping[str, DefectLogic]) -> None:
        """
        Populate the dependencies with logic objects, and the primary defects.
        """
        self.dependencies = [defect_logics[name] for _, name in self.operations 
                             if name in defect_logics]
                             
        # Primary defects are those which aren't in the logics dictionary.
        self.primary_defects = set(c for _, c in self.operations
                                   if c not in defect_logics)
                                   
    def evaluate_full(self, states: Mapping[str, DEFECT_IOV]) -> DEFECT_IOV:
        """
        Evaluate this defect logic for a point in time.
        
        Parameters
            `states` :
        """
        from operator import not_
        inputs = [(op, states[c]) for op, c in self.operations if c in states if states[c]]
        #inputs = [x for x in inputs if x]
        uninverted = [_ for _ in self.inverting if _ not in states or not states[_]]
        if not inputs and not uninverted:
            return None
        return DEFECT_IOV(None, None, self.name,
            present    =reduce(bool.__or__,  (op(i.present)     for op, i in inputs), False) or len(uninverted),
            recoverable=reduce(bool.__and__, (not not i.recoverable for op, i in inputs
                                              if op(i.present)), False),
            user="sys:virtual",
            comment=" ".join(sorted([(('!' if op == not_ else '') + i.channel) for op, i in inputs if op(i.present)]
                             +['!'+_ for _ in uninverted]))
        )
        
    def evaluate_partial(self, states: Mapping[str, DEFECT_IOV]) -> DEFECT_IOV:
        """
        Evaluate this defect logic for a point in time.
        
        Parameters
            `states` :
        """
        inputs = [(op, states[c]) for op, c in self.operations if c in states and states[c]]
        #inputs = [x for x in inputs if x]
        uninverted = [_ for _ in self.inverting if _ not in states or not states[_]]
        if not inputs and not uninverted:
            return None
        return DEFECT_IOV(None, None, self.name,
            present    =reduce(bool.__or__,  (op(i.present)     for op, i in inputs), False) or len(uninverted),
            recoverable=False,
            user="sys:virtual",
            comment=""
        )
