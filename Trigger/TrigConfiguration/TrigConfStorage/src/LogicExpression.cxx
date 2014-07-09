/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./LogicExpression.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>

using namespace TrigConf;

LogicExpression::LogicExpression(std::ostream & o) : 
   mState(kELEMENT),
   mElement(""),
   m_ostream(o)
{}


LogicExpression::LogicExpression(const std::string& name, std::ostream & o) 
   : mState(kELEMENT),
     mElement(name),
     m_ostream(o)
{}


LogicExpression::~LogicExpression() {}


bool
LogicExpression::isValidElementChar(char c) {
   std::string valid_sym = "_";
   if (isalpha(c) || isdigit(c) || valid_sym.find(c)!=std::string::npos) {
      return true;
   } else {
      return false;
   }
}


std::string
LogicExpression::extractElementName(const std::string& expr) {
   std::string name="";
   for (unsigned int i=0; i<expr.size(); ++i) {
      if (isValidElementChar(expr[i])) name += expr[i];
      else break;
   }
   return name;
}


int
LogicExpression::parse(const std::string& expr, bool enclosed) {
   unsigned int i=0;
   char c;
   bool no_problem=true;
   char prev = ' ';
   int n;

   mLogicRep = expr;
   while (no_problem && i < expr.size()) {
      c = expr[i];

      switch (c) {
      case kOPEN:
         {
            LogicExpression sub(m_ostream);
            if ( (n = sub.parse(expr.substr(i+1), true)) > 0) {
               addSubLogic(sub);
               prev = kELEMENT;
               i += n;
            } else {
               no_problem = false;
            }
         }
         break;
      case kCLOSE:
         if ( ( (mState == kAND || mState == kOR) && subLogics().size()>=2) ||
              (mState == kNOT && subLogics().size()==1) || 
              (mState == kELEMENT && subLogics().size()<=1 && mElement=="")) {
            prev = kELEMENT;
            markPlaceHolder();
            return i + 1;
         } else {
            std::ostringstream formatedoutput;
            formatedoutput << "Invalid logic: State=" << mState << " Nsublogics=" << subLogics().size() << " Element=" << mElement;
            printError(formatedoutput.str(), i);
            no_problem = false;
         }
         break;
      case kAND: case kOR:
         if (prev == kELEMENT && 
             ( (subLogics().size() >= 2 && mState==c) || 
               (subLogics().size()==1 && mState==kELEMENT))
             ) {
            mState = c;
            prev = c;
         } else {
            std::ostringstream formatedoutput;
            formatedoutput << kAND << " or " << kOR << " must come after a logic element";
            printError(formatedoutput.str(), i);
            no_problem = false;
         }
         break;
      case kNOT:
         if (kELEMENT != prev) {
            LogicExpression sub(m_ostream);
            sub.setState(kNOT);
            if ( (i+1)<expr.size() && isValidElementChar(expr[i+1])) {
               std::string name = extractElementName(expr.substr(i+1));
               sub.addSubLogic(LogicExpression(name,m_ostream));
               i += name.size();
               addSubLogic(sub);
            } else if ( (i+1)<expr.size() && expr[i+1]==kOPEN) {
               LogicExpression sub2(m_ostream);
               if ( (n = sub2.parse(expr.substr(i+2), true)) > 0) {
                  sub.addSubLogic(sub2);
                  i += (n+1);
                  addSubLogic(sub);
               } else {
                  no_problem = false;
               }
            } else {
               printError("Was expecting a sub-logic after !", i);
               no_problem = false;
            }
         } else {
            printError("! must not come directly after a logic element", i);
            no_problem = false;
         }
         prev = kELEMENT;
         break;
      default:
         // Start of a candidate of an element string.
         if (subLogics().size()==0 || 
             (subLogics().size()>0 && mState != kELEMENT)) {
            std::string name = extractElementName(expr.substr(i));
            if (name.size() > 0) {
               addSubLogic(LogicExpression(name,m_ostream));
               i += name.size()-1;
            } else {
               printError("Unrecognized string", i);
               no_problem = false;
            }
            prev = kELEMENT;
         } else {
            printError("A logic element may not come immediately after another logic element", i);
            no_problem = false;
         }
         break;
      }
      i ++;

   }
   if (!no_problem) {
      return -1;
   }

   if (enclosed) {
      // The logic expression started with a '(' but didn't find ')' and 
      // reached the end of the string. 
      printError("The expression started with a '(' but ')' not found.", i);
      return 0;
   }
   markPlaceHolder();

   return i;
}

void
LogicExpression::markPlaceHolder() {
   if (mState == kELEMENT && subLogics().size() == 1 && mElement=="") {
      mState = kOPEN;
   }
}

std::string
LogicExpression::logicRep() const {
   std::string s="";
   if (mState == kELEMENT) {
      s = mElement;
   } else if (mState == kAND || mState == kOR) {
      LogicV_t::const_iterator p;
      s = kOPEN;
      for (p=subLogics().begin(); p!=subLogics().end(); ++p) {
         s += (*p)->logicRep();
         s += mState;
      }
      if (s.size() > 1) s[s.size()-1] = kCLOSE;
      else s[1] = kCLOSE;
   } else if (mState == kNOT) {
      s = kNOT;
      s += kOPEN;
      if (subLogics().size() == 1) s += subLogics()[0]->logicRep();
      s += kCLOSE;
   } else if (mState == kOPEN) {
      s += kOPEN;
      if (subLogics().size() == 1) s += subLogics()[0]->logicRep();
      s += kCLOSE;
   }
   return s;
}


void
LogicExpression::normalize() {
   LogicV_t::const_iterator p;
   for (p=subLogics().begin(); p!=subLogics().end(); ++p) {
      (*p)->normalize();
   }
   if (isPlaceHolder()) {
      char s = subLogics()[0]->state();
      std::string e = subLogics()[0]->element();
      LogicV_t sublogics = subLogics()[0]->subLogics();
      clear();
      setState(s);
      setElement(e);
      for (p=sublogics.begin(); p!=sublogics.end(); ++p) {
         addSubLogic(**p);
      }
   }
}


int
LogicExpression::totalNumberOfElements() const {
   int n = 0;
   LogicV_t::const_iterator p;

   switch (mState) {
   case kELEMENT:
      n = 1;
      break;
   case kAND: case kOR: case kNOT: case kOPEN:
      for (p=subLogics().begin(); p!=subLogics().end(); ++p) {
         n += (*p)->totalNumberOfElements();
      }
      break;
   default:
      n = 0;
      break;
   }
   return n;
}


void
LogicExpression::printError(const std::string& message, int i) const {
   std::string m="Error while parsing : ";
   std::string s="";
   int n = m.size();
   for (int j=0; j<(n+i); ++j) s += " ";
   s += "^";
   m_ostream << m << mLogicRep << " at " << i << "-th character" << std::endl;
   m_ostream << s << std::endl;
   m_ostream << message << std::endl;
}


void
LogicExpression::printCurrentState() const {
   char aaa[100];
   sprintf(aaa, "Current state: State=%c Nsublogics=%d Element=%s", 
           mState, (int)subLogics().size(), mElement.c_str());
   m_ostream << aaa << std::endl;
}


void
LogicExpression::print(const std::string& indent) const {
   std::string s=indent;
   if (mState == kELEMENT) {
      m_ostream << indent << mElement << std::endl;
   } else if (mState == kAND || mState == kOR) {
      if (mState==kAND) m_ostream << indent << "AND" << std::endl;
      if (mState==kOR) m_ostream << indent << "OR" << std::endl;
      LogicV_t::const_iterator p;
      for (p=subLogics().begin(); p!=subLogics().end(); ++p) {
         (*p)->print(indent + "  ");
      }
   } else if (mState == kNOT) {
      m_ostream << indent << "NOT" << std::endl;
      if (subLogics().size() == 1) {
         subLogic(0)->print(indent + "  ");
      }
   } else if (mState == kOPEN) {
      if (subLogics().size() == 1) subLogic(0)->print(indent);
   } else {
      m_ostream << indent << "ERROR" << std::endl;
   }
}


void
LogicExpression::clear() {
   mState = kELEMENT;
   mSubLogics.clear();
   mElement = "";
}

