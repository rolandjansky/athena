/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PATHRESOLVER_PATHRESOLVER_H
#define PATHRESOLVER_PATHRESOLVER_H

#include "AsgTools/AsgMessaging.h"

#include <string>
#include <atomic>


class PathResolver

{
public:
  typedef enum
    {
      Ok,
      EnvironmentVariableUndefined,
      UnknownDirectory
    } SearchPathStatus;

  typedef enum
    {
      LocalSearch,
      RecursiveSearch
    } SearchType;

   typedef enum {
      PR_regular_file,
      PR_directory
   } PR_file_type;

   PathResolver(); //need a constructor to link the dictionary library with the implementation library

  /**

    @arg @c logical_file_name the name of the file to locate in the search path
    @arg @c search_path the name of a path-like environment variable
    @arg @c search_type characterizes the type of search. Can be either @c LocalSearch or @c RecursiveSearch

    @return the physical name of the located file or empty string if not found

   */

  static std::string find_file (const std::string& logical_file_name,
				const std::string& search_path,
				SearchType search_type = LocalSearch);

  /**

    @arg @c logical_file_name the name of the file to locate in the search path
    @arg @c search_list the prioritized list of possible locations separated by the usual path separator
    @arg @c search_type characterizes the type of search. Can be either @c LocalSearch or @c RecursiveSearch

    @return the physical name of the located file or empty string if not found

   */
  static std::string find_file_from_list (const std::string& logical_file_name,
					  const std::string& search_list,
					  SearchType search_type = LocalSearch);


  /**

    @arg @c logical_file_name the name of the directory to locate in the search path
    @arg @c search_path the name of a path-like environment variable
    @arg @c search_type characterizes the type of search. Can be either LocalSearch or RecursiveSearch

    @return the physical name of the located directory or empty string if not found

   */
  static std::string find_directory (const std::string& logical_file_name,
				     const std::string& search_path,
				     SearchType search_type = LocalSearch);

  /**

    @arg @c logical_file_name the name of the directory to locate in the search path
    @arg @c search_list the prioritized list of possible locations separated by the usual path separator
    @arg @c search_type characterizes the type of search. Can be either LocalSearch or RecursiveSearch

    @return the physical name of the located directory or empty string if not found

   */
  static std::string find_directory_from_list (const std::string& logical_file_name,
					       const std::string& search_list,
					       SearchType search_type = LocalSearch);


  /**

  @arg @c search_path the name of a path-like environment variable

  @return the result of the verification. Can be one of @c Ok, @c EnvironmentVariableUndefined, @c UnknownDirectory

   */
  static SearchPathStatus check_search_path (const std::string& search_path);


   static std::string find_calib_file(const std::string& logical_file_name);
   static std::string find_calib_directory(const std::string& logical_file_name);

   static void setOutputLevel(MSG::Level level);

   inline static std::string FindCalibFile(const std::string& logical_file_name) { return find_calib_file(logical_file_name); }
   inline static std::string FindCalibDirectory(const std::string& logical_file_name) { return find_calib_directory(logical_file_name); }
   inline static void SetOutputLevel(int lvl) { setOutputLevel(MSG::Level(lvl)); }

   private:
      static bool PR_find( const std::string& logical_file_name, const std::string& search_list, PR_file_type file_type, SearchType search_type,
         std::string& result );


       static std::atomic<MSG::Level> m_level;

       static bool msgLvl( const MSG::Level lvl ) { return asgMsg().msgLvl(lvl); }
       static MsgStream& msg() { return asgMsg().msg(); }
       static MsgStream& msg( const MSG::Level lvl ) { return asgMsg().msg(lvl); }
       static asg::AsgMessaging& asgMsg();


};

std::string PathResolverFindXMLFile (const std::string& logical_file_name);
std::string PathResolverFindDataFile (const std::string& logical_file_name);
std::string PathResolverFindCalibFile (const std::string& logical_file_name);
std::string PathResolverFindCalibDirectory (const std::string& logical_file_name);
void PathResolverSetOutputLevel(int lvl);

#endif
