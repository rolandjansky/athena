This tool is meant to do the repetitive part in setting up a new Tool or a new Algorithm. It is also set up in a way to encourage clean
programming, following the conventions for software development in Atlas. Just start the shell script corresponding
to your shell (.sh or .csh), it will inquire everything it needs to know interactively. Make sure that your environment is set up properly, 
the utility is working in the directory specified by $TestArea. The utility directory is for the time being assumed to be directly under the
$TestArea directory. Please make sure that the utility directory is located correctly. Once the tool is in CVS, the directory will 
automatically be located correctly, when it is checked out.

What the utility does:

- If the package directory for the new algorithm specified by the user  
  does not yet exist, the package directory will be set up, including subdirectories. 
  Otherwise, the files for the new algorithm or tool will be inserted in the already existing directory.

- It creates all files necessary for the new algorithm/tool to be compiled (Although, of course, the code won't do
  much at this stage.) For a new tool, named (by the user) 'NewTool', part of the package 'NewPackage' in the directory  
  '$TestArea/NewPath' the following files are created :
  ../NewPath/NewPackage:
        ChangeLog 
  ../NewPath/NewPackage/cmt:
        requirements
  ../NewPath/NewPackage/doc:
        mainpage.h
  ../NewPath/NewPackage/NewPackage:
        INewTool.h  (interface only for Tools, will not be created for algorithms)
        NewTool.h
  ../NewPath/NewPackage/share:
  ../NewPath/NewPackage/src:
        NewTool.cxx
  ../NewPath/NewPackage/src/components:
        NewPackage_entries.cxx
        NewPackage_load.cxx
  
If the new algorithm/tool is inserted into an already existing package, the ChangeLog, the requirements and the mainpage file 
as well as the files in the components directory are not created. A standard entry is added to the ChangeLog. 
However, the files in the components directory have to be edited by hand, when an algorithm/tool is added to an existing 
package in order to make it compile. 

Tools created by the utility are always derived from an Interface. The interface is always placed in the same directory
as the Tool's header file.

Martin Siebel <Martin.Siebel@CERN.ch>, May 2006