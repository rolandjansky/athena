/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// this :

#include <Inventor/C/errors/debugerror.h>
#define private public
#include <Inventor/SbPList.h>//hack to work around numitems not initialised in coin
#undef private
#include "SbStringHelpers.h"

#include <Inventor/SbString.h>

#include <stdlib.h>
#include <string.h>

#define SbMinimum(a,b) ((a)<(b)?(a):(b))

static SbString* SbStringCreateV(int,const char*,va_list);
//static SbString* SbStringCreateF(int,const char*,...);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
char* SbSTRDUP(
 const char* aString
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  char* p = (char*)::malloc((unsigned)::strlen(aString)+1);
  if(p==NULL) return 0;
  return ::strcpy(p,aString);
}
//////////////////////////////////////////////////////////////////////////////
void SbSTRDEL(
 char*& aString
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  ::free(aString);
  aString = NULL;
}
//////////////////////////////////////////////////////////////////////////////
SbPList SbStringGetWords (
 const SbString& aString 
,const char* aLimiter 
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbPList list(16);

  if(aString.getLength()<=0) return list;

  SbString string = aString;

  char* token = (char*)string.getString();

  while(1) { 
    char* pos = ::strstr (token,aLimiter);
    if(pos!=NULL) {
      *pos = '\0';
      if(*token!='\0') {
        list.append(token);
      }
      token = pos + ::strlen(aLimiter);          
    } else {
      //last word
      if(*token!='\0') {
        list.append(token);
      }
      break;
    }
  }
  
  int wordn = list.getLength();
  char* word;
  for(int count=0;count<wordn;count++) { 
    word = (char*)list[count];
    list[count] = new SbString(word);
  }
  
  return list;
}
//////////////////////////////////////////////////////////////////////////////////
////SbPList SbStringGetLines(
//// const SbString& aString
////)
//////////////////////////////////////////////////////////////////////////////////
////// Items separated by "\n" or "\\n"
//////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
////{
////  SbPList list(16);
////  list.numitems=0;//hack to work around numitems not initialised in coin 
////  int length = aString.getLength();
////  if(length<=0) return list;
////  char* string = SbSTRDUP(aString.getString());
////  int pos = 0;
////  length++;
////  int count;
////  for(count=0;count<length;count++) {
////    if( (string[count]=='\n') || 
////        (string[count]=='\0') || 
////        ( (string[count]=='\\') && (string[count+1]=='n') ) ) { 
////      char shift_one = (string[count]=='\n' ? 1 : 0);
////      string[count] = '\0';
////      list.append(new SbString(string+pos));
////      if(shift_one==1) {
////        pos = count+1;
////      } else {
////        pos = count+2;
////        count++;
////      }
////    }
////  }
////  SbSTRDEL(string);
////  return list;
////}
//////////////////////////////////////////////////////////////////////////////
void SbStringDelete(
 SbPList& aList
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int linen = aList.getLength();
  for(int count=0;count<linen;count++) delete ((SbString*)aList[count]);
  //aList.Clear();
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbStringConvertToBool (
 const SbString& aString
,SbBool& aValue
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if( (aString=="true") || (aString=="TRUE") || 
      (aString=="yes") || (aString=="YES") ) {
    aValue = TRUE;
    return TRUE;
  } else if( (aString=="false") || (aString=="FALSE") || 
             (aString=="no") || (aString=="NO") ) {
    aValue = FALSE;
    return TRUE;
  } else {
    aValue = FALSE;
    return FALSE;
  }
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbStringConvertToInt (
 const SbString& aString
,int& aValue
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  const char* string = aString.getString();
  char* s;
  long value = ::strtol(string,&s,10);
  if(s==string) {
    aValue = 0L;
    return FALSE;
  }
  aValue = (int)value;
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbStringConvertToFloat (
 const SbString& aString
,float& aValue
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  const char* string = aString.getString();
  char* s;
  double value = ::strtod(string,&s);
  if(s==string) {
    aValue = 0;  
    return FALSE;
  }
  aValue = (float)value;
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbStringConvertToUnsignedLong(
 const SbString& aString
,unsigned long& aValue
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aValue = 0L;
  if(::sscanf(aString.getString(),"%lx",&aValue)!=1) {
    if(::sscanf(aString.getString(),"%lu",&aValue)!=1) {
      aValue = 0L;
      return FALSE;
    }
  }
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbStringPrintF (
 SbString& aString
,int   aNumber
,const char* aFormat
,...
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  va_list args;
  va_start(args,aFormat);
  SbString* sbString = SbStringCreateV(aNumber,aFormat,args);
  va_end(args);
  if(sbString==NULL) {
    aString = "";
    return FALSE;
  }
  aString = *sbString;
  delete sbString;
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbString SbStringPrintF (
 int   aNumber
,const char* aFormat
,...
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  va_list args;
  va_start(args,aFormat);
  SbString* sbString = SbStringCreateV(aNumber,aFormat,args);
  va_end(args);
  if(sbString==NULL) return "";
  SbString s(*sbString);
  delete sbString;
  return s;
}
//////////////////////////////////////////////////////////////////////////////
int SbStringFind (
 const SbString& aString
,const SbString& aSearch
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int lsearch = aSearch.getLength();
  if(lsearch<=0) return SbStringNotFound;
  int number = aString.getLength();
  for(int index=0;index<=(number-lsearch);index++) {
    if(aString.getSubString(index,index+lsearch-1)==aSearch) return index;
  }
  return SbStringNotFound;
}
//////////////////////////////////////////////////////////////////////////////
int SbStringReverseFind (
 const SbString& aString
,const SbString& aSearch
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int lsearch = aSearch.getLength();
  if(lsearch<=0) return SbStringNotFound;
  int number = aString.getLength();
  for(int index=number-lsearch;index>=0;index--) {
    if(aString.getSubString(index,index+lsearch-1)==aSearch) return index;
  }
  return SbStringNotFound;
}
//////////////////////////////////////////////////////////////////////////////
SbString SbStringSuffix (
 const SbString& aString
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int index = SbStringReverseFind(aString,".");
  if(index==SbStringNotFound) return SbString("");
  return aString.getSubString(index+1);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SbString* SbStringCreateEmpty (
 int aLength
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aLength<=0) aLength = 0;
  char* string = (char*)::malloc((aLength+1)*sizeof(char));
  if(string==NULL) return NULL;
  string[aLength] = '\0';
  for(int count=0;count<aLength;count++) string[count] = ' ';
  SbString* sbString = new SbString(string);
  free(string);
  return sbString;
}
/*
//////////////////////////////////////////////////////////////////////////////
SbString* SbStringCreateF (
 int   aNumber
,const char* aFormat
,...
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  va_list args;
  va_start(args,aFormat);
  SbString* sbString = SbStringCreateV(aNumber,aFormat,args);
  va_end(args);
  return sbString;
}
*/
//////////////////////////////////////////////////////////////////////////////
SbString* SbStringCreateV (
 int     aNumber
,const char*   aFormat
,va_list aArgs
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aNumber<0) return NULL;
  SbString* sbString = SbStringCreateEmpty(aNumber);
  if(sbString==NULL) return NULL;
  const char* string = sbString->getString();
  ::vsprintf((char*)string,aFormat,aArgs);
  if(string[aNumber]!='\0') {
    ::printf("SbStringCreateV : buffer overflow \n");
    delete sbString;
    return NULL;
  }
  return sbString;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbStringIsTTF(
 const SbString& aString
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aString.getLength()<4) return FALSE;
  if(aString.getSubString(0,3)!="TTF/") return FALSE;
  return TRUE;
}
#include <string>
//////////////////////////////////////////////////////////////////////////////
SbBool SbStringGetenv(
 SbString& aString
)
//////////////////////////////////////////////////////////////////////////////
// From Lib::smanip::getenv.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::string a_String(aString.getString());
  std::string::size_type dollar;
  while((dollar=a_String.find('$'))!=std::string::npos){
    std::string::size_type slash = a_String.find('/',dollar+1);
    std::string::size_type back_slash = a_String.find('\\',dollar+1);
    std::string::size_type pos = std::string::npos; 
    if(slash!=std::string::npos) {
      if(back_slash!=std::string::npos) {
        pos = SbMinimum(slash,back_slash);
      } else {
        pos = slash;
      }
    } else {
      if(back_slash!=std::string::npos) {
        pos = back_slash;
      } else {
        pos = std::string::npos; 
      }
    }
    std::string env;
    if(pos==std::string::npos) {
      env = a_String.substr(dollar+1,a_String.length()-(dollar+1));
    } else {
      //     abc$xxx/ef
      //     0  3   7 9
      env = a_String.substr(dollar+1,pos-(dollar+1));
    }
    char* val = ::getenv(env.c_str());
    if(val) {
      std::string value = a_String.substr(0,dollar);
      value += val;
      if(pos!=std::string::npos)
        value += a_String.substr(pos,a_String.length()-pos);
      a_String = value;
    }
    if(!val) {
      aString = SbString(a_String.c_str());
      return FALSE; // Stop looping if env variable not found.
    }
  }
  aString = SbString(a_String.c_str());
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbString_isenv(
 const SbString& aString
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  const char* env = ::getenv(aString.getString());
  return (env?TRUE:FALSE);
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbString_getenv(
 const SbString& aString
,SbString& aValue
)
//////////////////////////////////////////////////////////////////////////////
// From bool Lib::System::getenv(
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  const char* env = ::getenv(aString.getString());
  if(env) {
    aValue = SbString(env);
    return TRUE;
  } else {
    aValue = "";
    return FALSE;
  }
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbStringReplace(
 SbString& aString
,const SbString& aOld     
,const SbString& aNew     
)
//////////////////////////////////////////////////////////////////////////////
// Lib::smanip::replace.
// return true : some replacement done.
// return false : nothing replaced.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::string a_Old(aOld.getString());
  std::string a_New(aNew.getString());
  if(a_Old=="") return FALSE;
  std::string snew;
  std::string::size_type lold = a_Old.length();
  bool status = FALSE;
  std::string stmp(aString.getString());
  while(true) {
    std::string::size_type pos = stmp.find(a_Old);
    if(pos==std::string::npos){
      snew += stmp;
      break;
    } else {
      snew += stmp.substr(0,pos);
      snew += a_New;
      stmp = stmp.substr(pos+lold,stmp.length()-(pos+lold));
      status = TRUE;
    }
  }
  aString = SbString(snew.c_str());
  return status;
}
