# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DQUtils.quick_retrieve import make_fieldselection, make_selection_vector

import pyparsing

from pyparsing import (Word, Literal, oneOf, QuotedString, Optional, 
    operatorPrecedence, opAssoc, nums, alphanums, Regex, StringStart, StringEnd,
    Combine, CaselessLiteral)

# Taken from "Good usage of setWhitespaceChars" on pyparsing's discussion
def convertNumber(t):
    if t.is_float: 
        return float(t[0])
    else: 
        return int(t[0])
 
# Number defintion
sign    = oneOf('+ -')
integer = Word(nums)
 
number  = Combine(
          Optional(sign)
                 + (   ( integer + Optional('.')('is_float') + Optional(integer))
                     | (            Literal('.')('is_float') + Optional(integer))
                   )
                 + Optional( CaselessLiteral('E') + Optional(sign) + integer)('is_float')
        ).setParseAction(convertNumber)

boolValue = (Literal("True") .setParseAction(lambda t: True) |
             Literal("False").setParseAction(lambda t: False))
stringValue = QuotedString("'") | QuotedString('"')
value = boolValue | number | stringValue

variable = Word(alphanums, min=1)

def make_parser(payload_specification):
    """
    The parser has to be payload_specification specific because it needs to know
    the types of the records to build a FieldSelection.
    """

    def make_selection(string, location, tokens):
        variable, comparator, value = tokens
        if payload_specification and variable not in payload_specification:
            raise RuntimeError("%s is not a value in the folder payload "
                               "specification")
        elif not payload_specification:
            return (variable, comparator, value)                              
        typeid = payload_specification[variable].storageType().id()
        return make_fieldselection(variable, typeid, comparator, value)

    from PyCool import cool
    EQ = Literal("==").setParseAction(lambda t: cool.FieldSelection.EQ)
    NE = Literal("!=").setParseAction(lambda t: cool.FieldSelection.NE)
    GE = Literal(">=").setParseAction(lambda t: cool.FieldSelection.GE)
    LE = Literal("<=").setParseAction(lambda t: cool.FieldSelection.LE)
    GT = Literal(">") .setParseAction(lambda t: cool.FieldSelection.GT)
    LT = Literal("<") .setParseAction(lambda t: cool.FieldSelection.LT)
    comparator = EQ | NE | GE | LE | GT | LT
    operand = (variable + comparator + value).setParseAction(make_selection)

    orop = Literal("or").suppress()
    andop = Literal("and").suppress()

    def logic_builder(connective):
        def thunk(string, location, tokens):
            vec = make_selection_vector()
            for token in tokens[0]:
                vec.push_back(token)
            
            return cool.CompositeSelection(connective, vec)
        return thunk

    expr = StringStart() + operatorPrecedence( operand,
        [(andop, 2, opAssoc.LEFT, logic_builder(cool.CompositeSelection.AND)),
         (orop,  2, opAssoc.LEFT, logic_builder(cool.CompositeSelection.OR)),]
        ) + StringEnd()
    
    def go(selection):
        return expr.parseString(selection)[0]
    return go
        
def make_browse_objects_selection(folder, selection):
    parser = make_parser(folder.folderSpecification().payloadSpecification())
    return parser(selection)
