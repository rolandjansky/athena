#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int rc;
	std::string check_file;
	if (argc == 1)
	{
		printf("No files to merge specified, exiting...\n");
		return 1;
	}
	//argv[0] = a.out, all the others argv[?] - files to be merged into voltmerge.txt
	//writing shell command
	std::string shell_line = "cat ";

	for (int i=1;i<argc;i++)
	{
		check_file = "ls ";
		check_file += std::string(argv[i]).c_str();
		rc = system(check_file.c_str());
		if (rc == 0)
		{
			shell_line += std::string(argv[i]).c_str();
			shell_line += " ";
		}
	}
	shell_line += "| sort | uniq > voltmerge.txt";
//	shell_line += std::string(argv[1]).c_str();
//	shell_line += "_final.txt";

	printf("Shell command to execute:\n%s\n",shell_line.c_str());
	system(shell_line.c_str());
	return 0;
}
