/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page PathResolver_page 

 <center><b>This utility finds files or directories from a search path list.</b></center>

<hr />
@section PathResolver_Principle Principle

   Assuming that a path-like environment variable is defined and
   filled in with the possible prioritized locations where a category
   of files is to be looked for, this utility will locate any file of
   that category from those possible locations.

   <p>
   The following properties are implemented:

     <ul>

      <li>a directory offset can be specified. In this case, the file
      search will detect files either right in every search path or in
      the offset directory in every search path.</li>

      <li>the effective physical location of the file is returned,
      possibly with symbolic link resolution</li>

      <li>it is possible to locate regular files or directories </li>

      <li>the search can be local in every search path or recursive to
      all subdirectories below every serach path.</li>

      <li>it is possible to simply verify a search path against non
      existing entries</li>

      <li>Search paths can be specified either by giving the name of a
      path-like environment variable or by a text string containing
      the path list. In both cases the list follows the specific
      syntax convention of the operating system (ie <b>':'</b> on Unix
      systems and <b>';'</b> on DOS ones).</li>

     </ul>

   </p>

<hr />
@section PathResolver_Using Using the PathResolver

   @subsection Accessing Accessing the PathResolver package.

   One needs to add to the requirements file the following use statement:

      @code
use PathResolver PathResolver-00-* Tools
      @endcode

   @subsection FindingFile Finding a file

      <ol>

      <li> <i>Finding a data file from the DATAPATH search list:</i>

      @code
#include <PathResolver/PathResolver.h>

int main ()
{
  std::string name;

  name = PathResolver::find_file ("a.dat", "DATAPATH");

  if (name == "")
    {
      std::cerr << "Cannot locate a.dat from ${DATAPATH}" << std::endl;
    }

  return (0);
}
@endcode
      </li>

      <li> <i>Recursively finding a data file from the DATAPATH search list:</i>

      @code
#include <PathResolver/PathResolver.h>

int main ()
{
  std::string name;

  name = PathResolver::find_file ("a.dat", "DATAPATH", PathResolver::RecursiveSearch);

  if (name == "")
    {
      std::cerr << "Cannot locate a.dat from any subdirectory of ${DATAPATH}" << std::endl;
    }

  return (0);
}
@endcode
      </li>

      <li> <i>Finding a data file from an explicit search list:</i>

      @code
#include <PathResolver/PathResolver.h>

int main ()
{
  std::string name;

  name = PathResolver::find_file_from_list ("a.dat", "/here/data:/there/data");

  if (name == "")
    {
      std::cerr << "Cannot locate a.dat from either /here/data or /there/data" << std::endl;
    }

  return (0);
}
@endcode
      </li>

      </ol>

   @subsection FindingDir Finding a directory

      <ol>

      <li> <i>Finding a directory from the DATAPATH search list:</i>

      @code
#include <PathResolver/PathResolver.h>

int main ()
{
  std::string name;

  name = PathResolver::find_directory ("data", "DATAPATH");

  if (name == "")
    {
      std::cerr << "Cannot locate data directory from ${DATAPATH}" << std::endl;
    }

  return (0);
}
@endcode
      </li>

      </ol>


 */
